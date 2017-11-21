#include "BBBEurobot/IRRecieverDriver.h"
#include "BBBEurobot/I2C-Wrapper.h"
#include <stdio.h>

#define N_SENSORS 9
const int SENSOR_ANGULAR_PLACEMENT = 360 / N_SENSORS;

gboolean IR_init(IRReciever *ir, int port, int address)
{
	ir->port = port;
	ir->address = address;
	if(port < 0)
		return FALSE;
	if(i2c_readRegister(ir->port, ir->address, IR_WHO_AM_I) != IR_WHO_AM_I_VALUE)
    {
		printf("Error : IR reciever not detected\n");
		return FALSE;
	}
	IR_getFrequency(ir);
	return TRUE;
}


int IR_getRawValue(IRReciever ir)
{
	unsigned char rawValues[2];
	i2c_readRegisters(ir.port, ir.address, IR_RAW_DATA1, 2, rawValues);
	int result = rawValues[0] + (rawValues[1] << 8);
	return result;
}

gboolean IR_getObstacle(IRReciever ir, int obstacle, int *size, int *position)
{
	int registerAddress = IR_ROBOT1_POS;
	if(obstacle != 0)
		registerAddress = IR_ROBOT2_POS;
	int result = i2c_readRegister(ir.port, ir.address, registerAddress);
	
	*size = (result & 0b11100000) >> 5;
	*position = result & 0b00011111;
	*position = *position * SENSOR_ANGULAR_PLACEMENT / 2;
	if(ir.frequency == IR_FREQUENCY_38k)
	{
		*position += SENSOR_ANGULAR_PLACEMENT / 2;
		if(*position == 2 * N_SENSORS)
			*position = 0;
	}
	return (size>0 ? 1 : 0);
}


unsigned char IR_getFrequency(IRReciever *ir)
{
	ir->frequency = i2c_readRegister(ir->port, ir->address, IR_FREQUENCY);
	return ir->frequency;
}
