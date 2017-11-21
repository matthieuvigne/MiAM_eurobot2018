#include "BBBEurobot/L6470Driver.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <unistd.h>

// Internal functions: all functions accessible outside of this file are at the end.

// Mutex : for thread safety
GMutex mut;

//open SPI bus
int openBus(L6470 l)
{

    l.port = open(l.portName, O_RDWR) ;
    if (l.port  < 0)
    {
        printf("Error opening SPI bus %s %d\n", l.portName, errno);
        return -1 ;
    }

    // Setup the SPI bus with our current parameters
    int value = 3;
    if (ioctl (l.port, SPI_IOC_WR_MODE, &value)         < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }

    if (ioctl (l.port, SPI_IOC_RD_MODE, &value)         < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }
	value = 8;
    if (ioctl (l.port, SPI_IOC_WR_BITS_PER_WORD, &value) < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }

    if (ioctl (l.port, SPI_IOC_RD_BITS_PER_WORD, &value) < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }

    if (ioctl (l.port, SPI_IOC_WR_MAX_SPEED_HZ, &l.speed)   < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }

    if (ioctl (l.port, SPI_IOC_RD_MAX_SPEED_HZ, &l.speed)   < 0)
    {
        printf("Error configuring port %s %d\n", l.portName, errno);
        return -1 ;
    }
    return l.port;
}


//close port
void closeBus(int port)
{
    if (port > 0)
        close(port);
}


//Sends the data contained in the buffer to the bus and reads the incomming
// data from the bus.  The buffer is overwritten with the incoming data.
int rwData(L6470 l, uint8_t* data, uint8_t len)
{
	
    struct spi_ioc_transfer spiCtrl;

    spiCtrl.tx_buf        = (unsigned long)data;
    spiCtrl.rx_buf        = (unsigned long)data;
    spiCtrl.len           = len;
    spiCtrl.delay_usecs   = 0;
    spiCtrl.speed_hz      = l.speed;
    spiCtrl.bits_per_word = 8;
	int res = ioctl(l.port, SPI_IOC_MESSAGE(1), &spiCtrl);
    return res;
}


// Send and recieve one 8 bit byte of data.

uint8_t rwByte(L6470 l, uint8_t bt)
{
    rwData(l, &bt, 1);
    return bt;
}


// Send and recieve one 16 bit word of data.
uint16_t rwWord(L6470 l, uint16_t wd)
{
    rwData(l, (uint8_t*)&wd, 2);
    return wd;
}


// Moves bytes over the SPI interface, thread safe.
uint8_t dspin_xfer(L6470 l, uint8_t data)
{
	if(l.port>0)
		return 0;
		
	g_mutex_lock (&mut);
    uint8_t data_out = data;
    l.port = openBus(l);
    rwData(l, &data_out, 1);
    closeBus(l.port);
	g_mutex_unlock (&mut);
    return data_out;
}


/*   Generalization of the subsections of the register read/write functionality.
 *   We want the end user to just write the value without worrying about length,
 *   so we pass a bit length parameter from the calling function.
 */
uint32_t procParam(L6470 l, uint32_t value, uint8_t bit_len)
{
    uint32_t ret_val=0;

    uint8_t byte_len = bit_len/8;   // How many BYTES do we have?
    if (bit_len%8 > 0)              // Make sure not to lose any partial byte values.
        byte_len++;

    // Let's make sure our value has no spurious bits set, and if the value was too
    //  high, max it out.
    uint32_t mask = 0xffffffff >> (32-bit_len);
    if (value > mask)
        value = mask;

    // The following three if statements handle the various possible byte length
    //  transfers- it'll be no less than 1 but no more than 3 bytes of data.
    // dspin_xfer() sends a byte out through SPI and returns a byte received
    //  over SPI- when calling it, we typecast a shifted version of the masked
    //  value, then we shift the received value back by the same amount and
    //  store it until return time.
    if (byte_len == 3)
    {
        ret_val |= dspin_xfer(l, (uint8_t)(value>>16)) << 16;
    }

    if (byte_len >= 2)
    {
        ret_val |= dspin_xfer(l, (uint8_t)(value>>8)) << 8;
    }

    if (byte_len >= 1)
    {
        ret_val |= dspin_xfer(l, (uint8_t)value);
    }

    // Return the received values. Mask off any unnecessary bits, just for
    //  the sake of thoroughness- we don't EXPECT to see anything outside
    //  the bit length range but better to be safe than sorry.
    return (ret_val & mask);
}


/*  This function handles the variable length parameters for the various
 *  chip registers.  Since different parameters take different numbers of
 *  bits we select the correct size for the parameter and pass that to the next 
 *  function along with the proper bit length.
 */
uint32_t paramHandler(L6470 l, uint8_t param, uint32_t value)
{
  uint32_t ret_val = 0;

  switch (param)
  {
    //  ABS_POS is the current absolute offset from home. It is a 22 bit number
    //  expressed in two's complement. At power up, this value is 0. It cannot
    //  be written when the motor is running, but at any other time, it can be
    //  updated to change the interpreted position of the motor.
    case dSPIN_ABS_POS:
      ret_val = procParam(l, value, 22);
      break;
          
    //  EL_POS is the current electrical position in the step generation cycle.
    //  It can be set when the motor is not in motion. Value is 0 on power up.
    case dSPIN_EL_POS:
      ret_val = procParam(l, value, 9);
      break;
          
    //  MARK is a second position other than 0 that the motor can be told to go
    //  to. As with ABS_POS, it is 22-bit two's complement. Value is 0 on power
    //  up.
    case dSPIN_MARK:
      ret_val = procParam(l, value, 22);
      break;
          
    //  SPEED contains information about the current speed. It is read-only. It
    //  does NOT provide direction information.
    case dSPIN_SPEED:
      ret_val = procParam(l, 0, 20);
      break;
          
    //  ACC and DEC set the acceleration and deceleration rates. Set ACC to
    //  0xFFF to get infinite acceleration/decelaeration- there is no way to
    //  get infinite deceleration w/o infinite acceleration (except the HARD
    //  STOP command). Cannot be written while motor is running. Both default
    //  to 0x08A on power up.
    case dSPIN_ACC:
      ret_val = procParam(l, value, 12);
      break;
          
    case dSPIN_DEC:
      ret_val = procParam(l, value, 12);
      break;
          
    //  MAX_SPEED is just what it says: any command which attempts to set the
    //  speed of the motor above this value will simply cause the motor to turn
    //  at this speed. Value is 0x041 on power up.
    case dSPIN_MAX_SPEED:
      ret_val = procParam(l, value, 10);
      break;
          
    //  MIN_SPEED controls two things- the activation of the low-speed
    //  optimization feature and the lowest speed the motor will be allowed to
    //  operate at. LSPD_OPT is the 13th bit, and when it is set, the minimum
    //  allowed speed is automatically set to zero. This value is 0 on startup.
    //  SetLSPDOpt() function exists to enable/disable the optimization feature.
    case dSPIN_MIN_SPEED:
      ret_val = procParam(l, value, 12);
      break;
          
    //  FS_SPD register contains a threshold value above which microstepping is
    //  disabled and the dSPIN operates in full-step mode. Defaults to 0x027 on
    //  power up.
    case dSPIN_FS_SPD:
      ret_val = procParam(l, value, 10);
      break;
          
    //  KVAL is the maximum voltage of the PWM outputs. These 8-bit values are
    //  ratiometric representations: 255 for full output voltage, 128 half, etc.
    //  Default is 0x29. The implications of different KVAL settings is too
    //  complex to dig into here, but it will usually work to max the value for
    //  RUN, ACC, and DEC. Maxing the value for HOLD may result in excessive
    //  power dissipation when the motor is not running.
    case dSPIN_KVAL_HOLD:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    case dSPIN_KVAL_RUN:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    case dSPIN_KVAL_ACC:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    case dSPIN_KVAL_DEC:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    //  INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back
    //  EMF compensation functionality. Please see the datasheet for details of
    //  this function, it is too complex to discuss here. Default values seem
    //  to work well enough.
    case dSPIN_INT_SPD:
      ret_val = procParam(l, value, 14);
      break;
          
    case dSPIN_ST_SLP:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    case dSPIN_FN_SLP_ACC:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    case dSPIN_FN_SLP_DEC:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    //  K_THERM is motor winding thermal drift compensation. Please see the
    //  datasheet for full details on operation- the default value should be
    //  okay for most use.
    case dSPIN_K_THERM:
      ret_val = dspin_xfer(l, (uint8_t)value & 0x0F);
      break;
          
    //  ADC_OUT is a read-only register containing the result of the ADC
    //  measurements. This is less useful than it sounds; see the datasheet
    //  for more information.
    case dSPIN_ADC_OUT:
      ret_val = dspin_xfer(l, 0);
      break;
          
    //  Set the overcurrent threshold. Ranges from 375mA to 6A in steps of
    //  375mA. A set of defined constants is provided for the user's
    //  convenience. Default value is 3.375A- 0x08. This is a 4-bit value.
    case dSPIN_OCD_TH:
      ret_val = dspin_xfer(l, (uint8_t)value & 0x0F);
      break;
          
    //  Stall current threshold. Defaults to 0x40, or 2.03A. Value is from
    //  31.25mA to 4A in 31.25mA steps. This is a 7-bit value.
    case dSPIN_STALL_TH:
      ret_val = dspin_xfer(l, (uint8_t)value & 0x7F);
      break;
          
    //  STEP_MODE controls the microstepping settings, as well as the generation
    //  of an output signal from the dSPIN. Bits 2:0 control the number of
    //  microsteps per step the part will generate. Bit 7 controls whether the
    //  BUSY/SYNC pin outputs a BUSY signal or a step synchronization signal.
    //  Bits 6:4 control the frequency of the output signal relative to the
    //  full-step frequency; see datasheet for that relationship as it is too
    //  complex to reproduce here. Most likely, only the microsteps per step
    //  value will be needed; there is a set of constants provided for ease of
    //  use of these values.
    case dSPIN_STEP_MODE:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    // ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of
    //  constants is provided to make this easy to interpret. By default,
    //  ALL alarms will trigger the FLAG pin.
    case dSPIN_ALARM_EN:
      ret_val = dspin_xfer(l, (uint8_t)value);
      break;
          
    //  CONFIG contains some assorted configuration bits and fields. A fairly
    //  comprehensive set of reasonably self-explanatory constants is provided,
    //  but users should refer to the datasheet before modifying the contents
    //  of this register to be certain they understand the implications of their
    //  modifications. Value on boot is 0x2E88; this can be a useful way to
    //  verify proper start up and operation of the dSPIN chip.
    case dSPIN_CONFIG:
      ret_val = procParam(l, value, 16);
      break;
          
    //  STATUS contains read-only information about the current condition of the
    //  chip. A comprehensive set of constants for masking and testing this
    //  register is provided, but users should refer to the datasheet to ensure
    //  that they fully understand each one of the bits in the register.
    case dSPIN_STATUS:
      ret_val = procParam(l, 0, 16);
      break;
          
    default:
      ret_val = dspin_xfer(l, (uint8_t)(value));
      break;
  }
  return ret_val;
}




//Write a specific parameter
void setParam(L6470 l, uint8_t param, uint32_t value)
{
    dspin_xfer(l, dSPIN_SET_PARAM | param);
    paramHandler(l, param, value);
}


void L6470_initMotion(L6470 l, int maxSpeed, int accel, int decel)
{
    dspin_xfer(l, dSPIN_SOFT_HIZ);
    dspin_xfer(l, dSPIN_ACTION_RESET);
    L6470_setVelocityProfile(l, maxSpeed, accel, decel);
    setParam(l, dSPIN_STALL_TH, 93);
    // Half step operation: double precision, but reduces motor torque between two steps (by about 30%).
    setParam(l, dSPIN_STEP_MODE,0);
    dspin_xfer(l, dSPIN_RESET_POS);
    setParam(l, dSPIN_CONFIG, dSPIN_CONFIG_PWM_DIV_1          | dSPIN_CONFIG_PWM_MUL_2
                  | dSPIN_CONFIG_SR_290V_us       | dSPIN_CONFIG_OC_SD_DISABLE
                  | dSPIN_CONFIG_VS_COMP_DISABLE| dSPIN_CONFIG_SW_USER
                  | dSPIN_CONFIG_INT_16MHZ);
}


void L6470_initBEMF(L6470 l,uint32_t k_hld, uint32_t k_mv, uint32_t int_spd, uint32_t st_slp, uint32_t slp_acc)
{
    setParam(l, dSPIN_KVAL_HOLD, k_hld);
    setParam(l, dSPIN_KVAL_ACC, k_mv);
    setParam(l, dSPIN_KVAL_DEC, k_mv);
    setParam(l, dSPIN_KVAL_RUN, k_mv);
    setParam(l, dSPIN_INT_SPD, int_spd);
    setParam(l, dSPIN_ST_SLP, st_slp);
    setParam(l, dSPIN_FN_SLP_ACC, slp_acc);
    setParam(l, dSPIN_FN_SLP_DEC, slp_acc);
}


uint32_t L6470_getParam(L6470 l, uint8_t param)
{
    dspin_xfer(l, dSPIN_GET_PARAM | param);
    return paramHandler(l, param, 0);
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
    dspin_xfer(l, dSPIN_RUN | dir);
    dspin_xfer(l, (uint8_t)(regVal >> 16));
    dspin_xfer(l, (uint8_t)(regVal >> 8));
    dspin_xfer(l, (uint8_t)(regVal));
}


void L6470_setVelocityProfile(L6470 l, int maxSpeed, int accel, int decel)
{
    dspin_xfer(l, dSPIN_SOFT_STOP);
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
    dspin_xfer(l, dSPIN_SOFT_STOP);
}


uint32_t L6470_getStatus(L6470 l)
{
    int temp = 0;
    dspin_xfer(l, dSPIN_GET_STATUS);
    temp = dspin_xfer(l, 0)<<8;
    temp |= dspin_xfer(l, 0);
    return temp;
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
    dspin_xfer(l, dSPIN_SOFT_HIZ);
}


void L6470_goToPosition(L6470 l, int32_t pos)
{
	int dir = 0;
    if(pos > 0)
		dir = 1;
    uint32_t abs_steps = abs(pos);
    if (abs_steps > 0x3FFFFF) abs_steps = 0x3FFFFF;
    dspin_xfer(l, dSPIN_MOVE | dir);
    dspin_xfer(l, (uint8_t)(abs_steps >> 16));
    dspin_xfer(l, (uint8_t)(abs_steps >> 8));
    dspin_xfer(l, (uint8_t)(abs_steps));
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


void L6470_initStructure(L6470 *l, gchar *portName, int speed)
{
	l->portName = g_strdup(portName);
	l->speed = speed;
}
