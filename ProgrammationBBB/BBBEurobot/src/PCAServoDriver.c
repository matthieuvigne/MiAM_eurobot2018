#include "BBBEurobot/PCAServoDriver.h"
#include <math.h>
#include <stdio.h>

// Set frequency of servo output signal to 50Hz.
const double servoFrequency = 50.0;

gboolean servoDriver_init(ServoDriver *driver, I2CAdapter *adapter, guint8 address, int clockFrequency)
{
	if(adapter->file < 0)
		return FALSE;
	driver->adapter = adapter;
	driver->address = address;
	driver->clockFrequency = clockFrequency;
	// Set mode1 register to send chip to sleep.
	i2c_writeRegister(driver->adapter, driver->address, 0x00, 0x10);
	// Test communication with chip: check that mode1 was set correctly.
	// The first three bits are read only and shouldn't be checked.
	if((i2c_readRegister(driver->adapter, driver->address, 0x00) & 0b01111111) != 0x10)
	{
		printf("Failed to init servo driver: slave responds but register was not set correclty\n");
		return FALSE;
	}
	// Set mode2 register to 0x00000000
	i2c_writeRegister(driver->adapter, driver->address, 0x01, 0x00);

	// Set PWM frequency to servoFrequency.
	// See chip prescaler register for more information.
	guint8 prescaler = (guint8) (round(driver->clockFrequency / (4096.0 * servoFrequency)) -1);
	i2c_writeRegister(driver->adapter, driver->address, 0xFE, (prescaler & 0xFF));

	// Set mode1 register to 0x00 to enable chip.
	i2c_writeRegister(driver->adapter, driver->address, 0x00, 0x20);

	// Set ALL_LED_OFF registers to turn off all outputs.
	i2c_writeRegister(driver->adapter, driver->address, 0xFA, 0x00);
	i2c_writeRegister(driver->adapter, driver->address, 0xFB, 0x00);
	i2c_writeRegister(driver->adapter, driver->address, 0xFC, 0x00);
	i2c_writeRegister(driver->adapter, driver->address, 0xFD, 0x10);

	// Reset led on time registers to 0.
	for(int i = 0; i < 16; i++)
	{
		i2c_writeRegister(driver->adapter, driver->address, 0x06 + 4 * i, 0);
		i2c_writeRegister(driver->adapter, driver->address, 0x07 + 4 * i, 0);
	}
	return TRUE;
}


void servoDriver_setPosition(ServoDriver driver, int servo, int position)
{
	if(driver.adapter < 0)
		return;
	if(servo < 0 || servo > 15)
		return;

	if(position < 500)
		position = 500;
	if(position > 2500)
		position = 2500;

	// Convert position from microseconds to ticks.
	int ticks = (int) (floor(position * servoFrequency * 4096 / 1000000.0));

	// Set LED off time
	i2c_writeRegister(driver.adapter, driver.address, 0x08 + 4 * servo, (ticks & 0xFF));
	i2c_writeRegister(driver.adapter, driver.address, 0x09 + 4 * servo, (ticks >> 8));
}
