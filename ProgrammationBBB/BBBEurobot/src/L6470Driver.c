#include "BBBEurobot/L6470Driver.h"
#include "BBBEurobot/SPI-Wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>


// Internal functions: all functions accessible outside of this file are at the end.

// Mutex : for thread safety
GMutex mut;


// Sends the data contained in the buffer to the bus and reads the incomming
// data from the bus.  The buffer is overwritten with the incoming data.
int spiReadWrite(L6470 l, uint8_t* data, uint8_t len)
{
	if(l.port>0)
		return -1;
	g_mutex_lock(&mut);
	l.port = spi_open(l.portName, l.frequency);

	struct spi_ioc_transfer spiCtrl[len];
	for(int x = 0; x < len; x++)
	{
		spiCtrl[x].tx_buf        = (unsigned long)&data[x];
		spiCtrl[x].rx_buf        = (unsigned long)&data[x];
		spiCtrl[x].len           = 1;
		spiCtrl[x].delay_usecs   = 1;
		spiCtrl[x].speed_hz      = l.frequency;
		spiCtrl[x].bits_per_word = 8;
		spiCtrl[x].cs_change = TRUE;
	}
	int res = ioctl(l.port, SPI_IOC_MESSAGE(len), &spiCtrl);
	spi_close(l.port);
	g_mutex_unlock(&mut);

    return res;
}


// Send a given command to the L6470, with an addition of data of a given length.
uint32_t sendCommand(L6470 l, uint8_t command, uint32_t dataValue, uint8_t length)
{
	uint8_t data[1 + length];
	data[0] = command;
	// Fill buffer with given input data
	for(int i = length - 1; i >= 0; i--)
	{
		data[length - i] = (dataValue >> (8 * i)) & 0xFF;
	}

	int result = spiReadWrite(l, data, length + 1);

	if(result < 0)
		printf("L6470 SPI error\n");
	// Decode response.
	uint32_t response = 0;
	for(uint8_t i = 1; i <= length; i++)
		response += data[i] << (8 * (length - i));
    return response;
}

// Given a parameter, returns the length of that parameter, in byte
// See Table 9 for parameter length
uint8_t paramLength(uint8_t param)
{
	switch (param)
	{
		case dSPIN_ABS_POS: return 3;
		case dSPIN_EL_POS: return 2;
		case dSPIN_MARK: return 3;
		case dSPIN_SPEED: return 3;
		case dSPIN_ACC: return 2;
		case dSPIN_DEC: return 2;
		case dSPIN_MAX_SPEED: return 2;
		case dSPIN_MIN_SPEED: return 2;
		case dSPIN_FS_SPD: return 2;
		case dSPIN_INT_SPD: return 2;
		case dSPIN_CONFIG: return 2;
		case dSPIN_STATUS: return 2;
		default: return 1;
	}
}


//Write a specific parameter
void setParam(L6470 l, uint8_t param, uint32_t value)
{
	sendCommand(l, dSPIN_SET_PARAM | param, value, paramLength(param));
}

// Get the value of a specific parameter.
uint32_t L6470_getParam(L6470 l, uint8_t param)
{
	return sendCommand(l, dSPIN_GET_PARAM | param, 0, paramLength(param));
}

void L6470_initMotion(L6470 l, int maxSpeed, int accel, int decel)
{
	// Set bridget output to high impedence.
	sendCommand(l, dSPIN_SOFT_HIZ, 0, 0);
    // Reset device.
    sendCommand(l, dSPIN_ACTION_RESET, 0, 0);
    //~ g_usleep(50000);
    // Set step mode param.
    // Half step operation: double precision, but reduces motor torque between two steps (by about 30%).
    setParam(l, dSPIN_STEP_MODE, 1);
    sendCommand(l, dSPIN_RESET_POS, 0, 0);
    // Set velocity profile data.
    L6470_setVelocityProfile(l, maxSpeed, accel, decel);
    // Set config param.
    setParam(l, dSPIN_CONFIG, dSPIN_CONFIG_PWM_DIV_1          | dSPIN_CONFIG_PWM_MUL_2
                  | dSPIN_CONFIG_SR_290V_us       | dSPIN_CONFIG_OC_SD_DISABLE
                  | dSPIN_CONFIG_VS_COMP_DISABLE| dSPIN_CONFIG_SW_USER
                  | dSPIN_CONFIG_INT_16MHZ);
}


void L6470_initBEMF(L6470 l,uint32_t k_hld, uint32_t k_mv, uint32_t int_spd, uint32_t st_slp, uint32_t slp_acc)
{
	// Set all back-EMF parameters.
    setParam(l, dSPIN_KVAL_HOLD, k_hld);
    setParam(l, dSPIN_KVAL_ACC, k_mv);
    setParam(l, dSPIN_KVAL_DEC, k_mv);
    setParam(l, dSPIN_KVAL_RUN, k_mv);
    setParam(l, dSPIN_INT_SPD, int_spd);
    setParam(l, dSPIN_ST_SLP, st_slp);
    setParam(l, dSPIN_FN_SLP_ACC, slp_acc);
    setParam(l, dSPIN_FN_SLP_DEC, slp_acc);
}

int32_t L6470_getPosition(L6470 l)
{
    int32_t regVal = L6470_getParam(l, dSPIN_ABS_POS);
    int32_t result = -1;
    if(regVal > 0x1FFFFF)
    {
        result = regVal + 0xFFC00000;
        return result;
    }
    else
        return regVal;
}


double L6470_getSpeed(L6470 l)
{
    int32_t regVal = L6470_getParam(l, dSPIN_SPEED);
    double conversionRatio = 0.01490116119;
    return  regVal * conversionRatio;
}


void L6470_setSpeed(L6470 l, int speed)
{
	int dir =1;
	if(speed < 0)
	{
		speed = -speed;
		dir = 0;
	}

    uint32_t regVal = (speed * 67.108864) + 0.5;
    if (regVal > 0xFFFFF) regVal = 0xFFFFF;

    sendCommand(l, dSPIN_RUN | dir, regVal, paramLength(dSPIN_SPEED));
}


void L6470_setVelocityProfile(L6470 l, int maxSpeed, int accel, int decel)
{
    sendCommand(l, dSPIN_SOFT_HIZ,0 ,0);
    // Wait for motor to fully stop.
    g_usleep(100000);
	// Set max speed
    float temp = maxSpeed * 0.065536;
    uint32_t regVal = (unsigned long) temp;
    if( regVal > 0x000003FF)
        regVal = 0x000003FF;
    setParam(l, dSPIN_MAX_SPEED, regVal);

    // Set max acceleration
    temp = accel * 0.068728;
    regVal = (uint32_t) temp;
    if(regVal > 0x00000FFF)
        regVal = 0x00000FFF;
    setParam(l, dSPIN_ACC, regVal);

    // Set max deceleration
    temp = decel * 0.068728;
    regVal = (uint32_t) temp;
    if(regVal > 0x00000FFF)
        regVal = 0x00000FFF;
    setParam(l, dSPIN_DEC, regVal);
}


void L6470_softStop(L6470 l)
{
    sendCommand(l, dSPIN_SOFT_STOP, 0 , 0);
}


uint32_t L6470_getStatus(L6470 l)
{
	return sendCommand(l, dSPIN_GET_STATUS, 0, paramLength(dSPIN_STATUS));
}


uint32_t L6470_getError(L6470 l)
{
    uint32_t result = 0;
    uint32_t status = L6470_getStatus(l);
    uint32_t temp = 0;

    temp = status & dSPIN_STATUS_NOTPERF_CMD;
    if (temp) result |= dSPIN_ERR_NOEXEC;

    temp = status & dSPIN_STATUS_WRONG_CMD;
    if (temp) result |= dSPIN_ERR_BADCMD;

    temp = status & dSPIN_STATUS_UVLO;
    if (temp == 0) result |= dSPIN_ERR_UVLO;
    temp = status & dSPIN_STATUS_TH_SD;
    if (temp == 0) result |= dSPIN_ERR_THSHTD;

    temp = status & dSPIN_STATUS_OCD;
    if (temp == 0) result |= dSPIN_ERR_OVERC;

    temp = status & dSPIN_STATUS_STEP_LOSS_A;
    if (temp == 0) result |= dSPIN_ERR_STALLA;

    temp = status & dSPIN_STATUS_STEP_LOSS_B;
    if (temp == 0) result |= dSPIN_ERR_STALLB;

    return result;
}


 void L6470_highZ(L6470 l)
{
    sendCommand(l, dSPIN_SOFT_HIZ, 0, 0);
}


void L6470_goToPosition(L6470 l, int32_t pos)
{
	int dir = 0;
    if(pos > 0)
		dir = 1;
    uint32_t abs_steps = abs(pos);
    if (abs_steps > 0x3FFFFF) abs_steps = 0x3FFFFF;

	sendCommand(l, dSPIN_MOVE | dir, abs_steps, paramLength(dSPIN_ABS_POS));
}


int L6470_isBusy(L6470 l)
{
    uint32_t temp = L6470_getParam(l, dSPIN_STATUS);
    uint32_t busy = temp & dSPIN_STATUS_BUSY;

    if (busy == 0)
    {
        temp &= dSPIN_STATUS_MOT_STATUS;
        temp >>= 5;
        //if (temp == 0) temp = 0xFF;
        return temp;
    }

    return 0;
}


void L6470_initStructure(L6470 *l, const gchar *portName)
{
	l->portName = g_strdup(portName);
	// Set bus frequency: default 800kHz
	//~ l->frequency = 800000;
	l->frequency = 100000;
}
