#include "BBBEurobot/IMUDriver.h"
#include <stdio.h>

// Note: all three components can work with different resolution: a greater
// 		 precision can be achieved, at the cost of a smaller working range.
//		 For now, all three sensors are set at max resolution (smallest range),
//		 to get the best precision: the maximum limits are never reached anyway.


// Choose gyroscope precision: 0 (least precise) - 2 (maximum precision, but smaller max speed)
// Corresponding speed: 2000 dps, 1000dps, 500 dps, 245 dps, 125dps
#define GYRO_PRES 4
const char gyroReg[5] = {0b110,0b100,0b010, 0b000, 0b001};
const double gyroMultiplier[5] = {0.070, 0.035, 0.0175, 0.00875, 0.004375};
const double degToRad = G_PI / 180.0;

// Choose accelerometer precision: 0 (least precise) - 4 (maximum precision, but smaller max acceleration)
// Corresponding acceleration: 16g, 8g, 4g, 2g
#define ACCEL_PRES 0
const char accelReg[4] = {0b01,0b11,0b10,0b00};
const double accelMultiplier[4] = {0.00478728,0.00239364,0.00119682,0.00059841};

// Choose magnetometer precision: 0 (least precise) - 4 (maximum precision, but smaller max field)
// Corresponding field: 16gauss, 12gauss, 8gauss, 4gauss
#define MAGNETO_PRES 3
const char magnetoReg[4] = {0b11, 0b10, 0b01, 0b00};
const double magnetoMultiplier[4] = {0.64, 0.479,0.32,0.16};

// Initalize gyroscope
gboolean initIMU(IMU i)
{
    // Verify it is the right gyroscope.
    if(i2c_readRegister(i.adapter, i.imuAddress, 0x0F) != 0x69)
    {
		printf("Error : LSM6DS33 (IMU) not detected\n");
		return FALSE;
	}

	// Configure accelerometer to match ACCEL_PRES, anti-aliasing 400Hz, 1.6kHz output rate
    i2c_writeRegister(i.adapter, i.imuAddress, 0x10, 0b10000000 | (accelReg[ACCEL_PRES] << 2));
	// Configure gyroscope to match GYRO_PRES, 1.6kHz output rate
    i2c_writeRegister(i.adapter, i.imuAddress, 0x11, 0b10000000 | (gyroReg[GYRO_PRES] << 1));
	return TRUE;
}

double imu_gyroGetXAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x22, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return degToRad * res * gyroMultiplier[GYRO_PRES];
}

double imu_gyroGetYAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x24, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return degToRad * res * gyroMultiplier[GYRO_PRES];
}

double imu_gyroGetZAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x26, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return degToRad * res * gyroMultiplier[GYRO_PRES];
}


void imu_gyroGetValues(IMU i, double *x, double *y, double *z)
{
	if(i.adapter < 0)
	{
		*x = 0;
		*y = 0;
		*z = 0;
		return;
	}
	unsigned char rxbuf[6];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x22, 6, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	*x = degToRad * res * gyroMultiplier[GYRO_PRES];

	res = (rxbuf[3] << 8 ) + rxbuf[2];
	if(res > 32767)
		res -= 65536;
	*y = degToRad * res * gyroMultiplier[GYRO_PRES];
	res = (rxbuf[5] << 8 ) + rxbuf[4];
	if(res > 32767)
		res -= 65536;
	*z = degToRad * res * gyroMultiplier[GYRO_PRES];
}

double imu_accelGetXAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x28, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * accelMultiplier[ACCEL_PRES];
}

double imu_accelGetYAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x2A, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * accelMultiplier[ACCEL_PRES];
}


double imu_accelGetZAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x2C, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * accelMultiplier[ACCEL_PRES];
}


void imu_accelGetValues(IMU i, double *x, double *y, double *z)
{
	if(i.adapter < 0)
	{
		*x = 0;
		*y = 0;
		*z = 0;
		return;
	}
	unsigned char rxbuf[6];
	i2c_readRegisters(i.adapter, i.imuAddress, 0x28, 6, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	*x = (double) (res * accelMultiplier[ACCEL_PRES]);

	res = (rxbuf[3] << 8 ) + rxbuf[2];
	if(res > 32767)
		res -= 65536;
	*y = res * accelMultiplier[ACCEL_PRES];
	res = (rxbuf[5] << 8 ) + rxbuf[4];
	if(res > 32767)
		res -= 65536;
	*z = (double) (res * accelMultiplier[ACCEL_PRES]);
}

// Initialize magnetometer
gboolean initMagneto(IMU i)
{
    // Verify it is the right accelerometer.
    if(i2c_readRegister(i.adapter, i.magnetoAddress, 0x0F) != 0x3D)
    {
		printf("Error : LIS3MDL (magnetometer) not detected\n");
		return -1;
	}
	//Configure magnetometer.

    //CTRL1, ultra-high-performance, fast output
	i2c_writeRegister(i.adapter, i.magnetoAddress, 0x20, 0b01111110);
	//CTRL2, resolution
	i2c_writeRegister(i.adapter, i.magnetoAddress, 0x21, magnetoReg[MAGNETO_PRES] << 5);
	//CTRL3, enable
    i2c_writeRegister(i.adapter, i.magnetoAddress, 0x22, 0);
    //CTRL4, ultra-high-performance on z
    i2c_writeRegister(i.adapter, i.magnetoAddress, 0x23, 0b1100);

	return 0;
}


double imu_magnetoGetXAxis(IMU i)
{
	if(!i.magnetoEnabled || i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.magnetoAddress, 0x28, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}

double imu_magnetoGetYAxis(IMU i)
{
	if(!i.magnetoEnabled || i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.magnetoAddress, 0x2A, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}


double imu_magnetoGetZAxis(IMU i)
{
	if(!i.magnetoEnabled || i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.magnetoAddress, 0x2C, 2, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}


void imu_magnetoGetValues(IMU i, double *x, double *y, double *z)
{
	if(!i.magnetoEnabled || i.adapter < 0)
	{
		*x = 0;
		*y = 0;
		*z = 0;
		return;
	}
	unsigned char rxbuf[6];
	i2c_readRegisters(i.adapter, i.magnetoAddress, 0x28, 6, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	*x = res * magnetoMultiplier[MAGNETO_PRES];

	res = (rxbuf[3] << 8 ) + rxbuf[2];
	if(res > 32767)
		res -= 65536;
	*y = res * magnetoMultiplier[MAGNETO_PRES];
	res = (rxbuf[5] << 8 ) + rxbuf[4];
	if(res > 32767)
		res -= 65536;
	*z = res * magnetoMultiplier[MAGNETO_PRES];
}


gboolean imu_init(IMU *i, I2CAdapter *adapter, int imuAddress, int magnetoAddress, gboolean enableMagneto)
{
	if(adapter->file < 0)
		return FALSE;
	i->adapter = adapter;
	i->imuAddress=imuAddress;
	i->magnetoAddress=magnetoAddress;
	i->magnetoEnabled=enableMagneto;
	if(initIMU(*i) == -1)
		return FALSE;
	if(enableMagneto)
		if(initMagneto(*i) == -1)
			return FALSE;
	return TRUE;
}
