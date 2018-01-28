#include "BBBEurobot/TCS3472ColorSensorDriver.h"
#include <math.h>
#include <stdio.h>

// Sensor configuration options: for now the defaults are always used, the user
// doesn't have the choice of changing them.
const double frequency = 50.0;

gboolean colorSensor_init(ColorSensorTCS3472 *sensor, I2CAdapter *adapter, guint8 address)
{
	if(adapter->file < 0)
		return FALSE;
	sensor->adapter = adapter;
	sensor->address = address;
	// Check chip identity.
	guint8 chipIdentity = i2c_readRegister(sensor->adapter, sensor->address, 0x12);
	if(chipIdentity != 0x44 && chipIdentity != 0x4D)
	{
		printf("Error : TCS3472 not detected\n");
		return FALSE;
	}
	// Configure chip.
	// Set command register
	i2c_writeRegister(sensor->adapter, sensor->address, 0x00, 0b10100000);
	colorSensor_setIntegrationTime(*sensor, 3);
	colorSensor_setGain(*sensor, TCS34725_GAIN_1X);
	// Enable chip.
	i2c_writeRegister(sensor->adapter, sensor->address, 0x00, 0x01);
	// Wait 3ms for oscillator to start, then powerup the device.
	g_usleep(3000);
	i2c_writeRegister(sensor->adapter, sensor->address, 0x00, 0x03);
	return TRUE;
}


gboolean colorSensor_setIntegrationTime(ColorSensorTCS3472 sensor, int integrationTime)
{
	// Compute register value.
	guint8 registerValue = (guint8) (256 - (floor(integrationTime / 2.4)));
	return i2c_writeRegister(sensor.adapter, sensor.address, 0x01, registerValue);
}

gboolean colorSensor_setGain(ColorSensorTCS3472 sensor, TCS34725Gain_t gain)
{
	return i2c_writeRegister(sensor.adapter, sensor.address, 0x0F, gain);
}

ColorOutput colorSensor_getData(ColorSensorTCS3472 sensor)
{
	ColorOutput color;
	guint8 sensorValue[8];
	i2c_readRegisters(sensor.adapter, sensor.address, 0x14, 8, sensorValue);
	color.clear = (sensorValue[0] << 8) + sensorValue[1];
	color.red = (sensorValue[2] << 8) + sensorValue[3];
	color.green = (sensorValue[4] << 8) + sensorValue[5];
	color.blue = (sensorValue[6] << 8) + sensorValue[7];

	return color;
}
