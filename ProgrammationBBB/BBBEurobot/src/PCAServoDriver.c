#include "BBBEurobot/PCAServoDriver.h"
#include "BBBEurobot/I2C-Wrapper.h"
#include <math.h>

// Set frequency of output (i.e. of chip PWM prescaler)
const double frequency = 50.0;

gboolean servoDriver_init(ServoDriver *driver, int port, int address)
{
	driver->port = port;
	if(port < 0)
		return FALSE;
	driver->address = address;
	// Set mode1 register to send chip to sleep.
	i2c_writeRegister(driver->port, driver->address, 0x00, 0x10);
	// Test communication with chip: check that mode1 was set correctly.
	if(i2c_readRegister(driver->port, driver->address, 0x00) != 0x10)
		return FALSE;
	
	// Set mode2 register to 0x00000000
	i2c_writeRegister(driver->port, driver->address, 0x01, 0x00);
	// Set PWM frequency to 50Hz -> decimal 121.
	int prescaler = (int) (round(25000000.0 / 4096.0 / frequency) -1);
	i2c_writeRegister(driver->port, driver->address, 0xFE, (prescaler & 0xFF));
	
	// Set mode1 register to 0b00100000 to enable chip with autoincrement
	i2c_writeRegister(driver->port, driver->address, 0x00, 0x20);
	
	// Set ALL_LED_OFF registers to turn off all outputs.
	i2c_writeRegister(driver->port, driver->address, 0xFA, 0x00);
	i2c_writeRegister(driver->port, driver->address, 0xFB, 0x00);
	i2c_writeRegister(driver->port, driver->address, 0xFC, 0x00);
	i2c_writeRegister(driver->port, driver->address, 0xFD, 0x10);
	return TRUE;
}


void servoDriver_setPosition(ServoDriver driver, int servo, int position)
{
	if(driver.port < 0)
		return;
	if(servo < 0 || servo > 15)
		return;
	
	if(position < 500)
		position = 500;
	if(position > 2500)
		position = 2500;
	
	// Convert position from microseconds to ticks.
	int ticks = (int) (floor(position * frequency * 4096 / 1000000.0));
	
	i2c_writeRegister(driver.port, driver.address, 0x08 + 4 * servo, (ticks & 0xFF));
	i2c_writeRegister(driver.port, driver.address, 0x09 + 4 * servo, (ticks >> 8));
}
