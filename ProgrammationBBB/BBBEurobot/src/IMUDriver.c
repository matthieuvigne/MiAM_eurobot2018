#include "BBBEurobot/IMUDriver.h"
#include <stdio.h>

// Note: all three components can work with different resolution: a greater
// 		 precision can be achieved, at the cost of a smaller working range.
//		 For now, all three sensors are set at max resolution (smallest range),
//		 to get the best precision: the maximum limits are never reached anyway.


// Choose gyroscope precision: 0 (least precise) - 2 (maximum precision, but smaller max speed)
// Corresponding speed: 2000 dps, 500 dps, 245 dps
#define GYRO_PRES 2
const char gyroReg[3] = {0x20,0x10,0};
const double gyroMultiplier[3] = {0.070, 0.0175, 0.00875};

// Choose accelerometer precision: 0 (least precise) - 4 (maximum precision, but smaller max acceleration)
// Corresponding acceleration: 16g, 8g, 6g, 4g, 2g
#define ACCEL_PRES 4
const char accelReg[5] = {0x20,0x18,0x10,0x08,0};
const double accelMultiplier[5] = {0.00718092,0.00239364,0.00179523,0.00119682,0.00059841};

// Choose magnetometer precision: 0 (least precise) - 4 (maximum precision, but smaller max field)
// Corresponding field: 12gauss, 8gauss, 4gauss, 2gauss
#define MAGNETO_PRES 3
const char magnetoReg[4] = {0x60,0x40,0x20,0};
const double magnetoMultiplier[4] = {0.479,0.32,0.16,0.080};


// Initalize gyroscope
gboolean initGyro(IMU i)
{
    // Verify it is the right gyroscope.
    if(i2c_readRegister(i.adapter, i.gyroAddress, 0x0F) != 0xD7)
    {
		printf("Error : L3GD20H not detected\n");
		return FALSE;
	}
    
	// Configure gyroscope to match GYRO_PRES
    i2c_writeRegister(i.adapter, i.gyroAddress, 0x23, gyroReg[GYRO_PRES]);	
    
    // DR = 01 (200 Hz ODR); BW = 10 (50 Hz bandwidth); PD = 1 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)
	i2c_writeRegister(i.adapter, i.gyroAddress, 0x20, 0b11101111);
    
	return TRUE;
}

double imu_gyroGetXAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.gyroAddress, 0xA8, 2, rxbuf);	//x register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * gyroMultiplier[GYRO_PRES];
}

double imu_gyroGetYAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.gyroAddress, 0xAA, 2, rxbuf);	//y register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * gyroMultiplier[GYRO_PRES];
}

double imu_gyroGetZAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.gyroAddress, 0xAC, 2, rxbuf);	//z register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * gyroMultiplier[GYRO_PRES];
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
	i2c_readRegisters(i.adapter, i.gyroAddress, 0xA8, 6, rxbuf);
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	*x = res * gyroMultiplier[GYRO_PRES];
	
	res = (rxbuf[3] << 8 ) + rxbuf[2];
	if(res > 32767)
		res -= 65536;
	*y = res * gyroMultiplier[GYRO_PRES];
	res = (rxbuf[5] << 8 ) + rxbuf[4];
	if(res > 32767)
		res -= 65536;
	*z = res * gyroMultiplier[GYRO_PRES];
}


// Initialize accelerometer
gboolean initAccel(IMU i)
{
    // Verify it is the right accelerometer.
    if(i2c_readRegister(i.adapter, i.accelAddress, 0x0F) != 0x49)
    {
		printf("Error : LSM303D not detected\n");
		return -1;
	}
	//Configure accelerometer and magnetometer.
	
    //CTRL1, Output-enable, data rate
	i2c_writeRegister(i.adapter, i.accelAddress, 0x20, 0b10000111);
	//CTRL2, resolution
	i2c_writeRegister(i.adapter, i.accelAddress, 0x21, accelReg[ACCEL_PRES]);
	
	//Magnetometer
	//CTRL5
    i2c_writeRegister(i.adapter, i.accelAddress, 0x24, 0x64);
    //CTRL6, echelle
    i2c_writeRegister(i.adapter, i.accelAddress, 0x25, magnetoReg[MAGNETO_PRES]);
    //CTRL7, enable		
    i2c_writeRegister(i.adapter, i.accelAddress, 0x26, 0b10000000	);		
    
	return 0;
}

double imu_accelGetXAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.accelAddress, 0xA8, 2, rxbuf);	//x register, auto-increment
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
	i2c_readRegisters(i.adapter, i.accelAddress, 0xAA, 2, rxbuf);	//y register, auto-increment
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
	i2c_readRegisters(i.adapter, i.accelAddress, 0xAC, 2, rxbuf);	//z register, auto-increment
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
	i2c_readRegisters(i.adapter, i.accelAddress, 0xA8, 6, rxbuf);
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

double imu_magnetoGetXAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.accelAddress, 0x88, 2, rxbuf);	//x register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}

double imu_magnetoGetYAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.accelAddress, 0x8A, 2, rxbuf);	//y register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}


double imu_magnetoGetZAxis(IMU i)
{
	if(i.adapter < 0)
		return 0;
	unsigned char rxbuf[2];
	i2c_readRegisters(i.adapter, i.accelAddress, 0x8C, 2, rxbuf);	//z register, auto-increment
	int res = (rxbuf[1] << 8 ) + rxbuf[0];
	if(res > 32767)
		res -= 65536;
	return res * magnetoMultiplier[MAGNETO_PRES];
}


void imu_magnetoGetValues(IMU i, double *x, double *y, double *z)
{
	if(i.adapter < 0)
	{
		*x = 0;
		*y = 0;
		*z = 0;
		return;
	}
	unsigned char rxbuf[6];
	i2c_readRegisters(i.adapter, i.accelAddress, 0x88, 6, rxbuf);
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


gboolean imu_init(IMU *i, I2CAdapter *adapter, int gyro, int accel)
{
	if(adapter->file < 0)
		return FALSE;
	i->adapter = adapter;
	i->gyroAddress = gyro;
	i->accelAddress = accel;
	if(initGyro(*i) == -1)
	{
		return FALSE;
	}
		
	if(initAccel(*i) == -1)
	{
		return FALSE;
	}
	return TRUE;
}
