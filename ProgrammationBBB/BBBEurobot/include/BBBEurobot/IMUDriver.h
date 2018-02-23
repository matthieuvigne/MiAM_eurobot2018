/// \file IMUDriver.h
/// \brief Driver for Pololu MiniIMU (L3GD20H and LSM303D).
///
/// \details This file contains all the functions related to communication
///			 with the IMU, via I2C.
///	\note	 All functions in this header should be prefixed with imu_.
///	\note	 Sensor precision setting is hardcoded, see source code directly.
#ifndef IMU_DRIVER_H
	#define IMU_DRIVER_H
	#include "BBBEurobot/I2C-Wrapper.h"

	/// IMU structure.
	typedef struct {
		I2CAdapter *adapter; ///< Pointer to the I2C port being used.
		guint8 imuAddress;	///< IMU (accel + gyro) I2C address.
		guint8 magnetoAddress;	///< Magnetometer I2C address.
		gboolean magnetoEnabled;	///< If the magnetometer is enabled or not.
	}IMU;

	/// \brief Initialize IMU structure.
    ///
    /// \details This function tests the communication with the IMU, and, if successful, inits the structure.
    ///
    /// \param[out] imu The IMU structure, to be used whenever communication with the IMU.
    /// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C port (as returned by the i2c_open function,
    ///                    see I2C-Wrapper.h).
    /// \param[in] imuAddress I2C address of the imu.
    /// \param[in] magnetoAddress I2C address of the magnetometer.
    /// \param[in] enableMagneto If false, the magnetometer chip will remain in standby mode. Calling any getMagneto...
    ///            function will then return 0
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean imu_init(IMU *imu, I2CAdapter *port, int imuAddress, int magnetoAddress, gboolean enableMagneto);

	/// \brief Calls imu_init with the default sensor I2C addresses.
    ///
	static inline gboolean imu_initDefault(IMU *imu, I2CAdapter *adapter, gboolean enableMagneto)
	{
		return imu_init(imu, adapter, 0b1101011, 0b00011110, enableMagneto);
	}

	/// \brief Get gyroscope reading along X axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Gyroscope reading along X, in rad/s.
	double imu_gyroGetXAxis(IMU imu);


	/// \brief Get gyroscope reading along Y axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Gyroscope reading along Y, in rad/s.
	double imu_gyroGetYAxis(IMU imu);


	/// \brief Get gyroscope reading along Z axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Gyroscope reading along Z, in rad/s.
	double imu_gyroGetZAxis(IMU imu);



	/// \brief Get all gyroscope readings at once.
	///
    /// \param[in] imu The IMU structure to use for communication.
    /// \param[out] x Gyroscope reading along X, in rad/s.
    /// \param[out] y Gyroscope reading along Y, in rad/s.
    /// \param[out] z Gyroscope reading along Z, in rad/s.
	void imu_gyroGetValues(IMU imu, double *x, double *y, double *z);


	/// \brief Get accelerometer reading along X axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Accelerometer reading along X, in m/s^2.
	double imu_accelGetXAxis(IMU imu);


	/// \brief Get accelerometer reading along Y axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Accelerometer reading along Y, in m/s^2.
	double imu_accelGetYAxis(IMU imu);


	/// \brief Get accelerometer reading along Z axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Accelerometer reading along Z, in m/s^2.
	double imu_accelGetZAxis(IMU imu);


	/// \brief Get all accelerometer readings at once.
	///
    /// \param[in] imu The IMU structure to use for communication.
    /// \param[out] x Accelerometer reading along X, in m/s^2.
    /// \param[out] y Accelerometer reading along Y, in m/s^2.
    /// \param[out] z Accelerometer reading along Z, in m/s^2.
	void imu_accelGetValues(IMU imu, double *x, double *y, double *z);


	/// \brief Get magnetometer reading along X axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Magnetometer reading along X, in mgauss.
	double imu_magnetoGetXAxis(IMU imu);


	/// \brief Get magnetometer reading along Y axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Magnetometer reading along Y, in mgauss.
	double imu_magnetoGetYAxis(IMU imu);


	/// \brief Get magnetometer reading along Z axis.
    ///
    /// \param[in] imu The IMU structure to use for communication.
    /// \returns Magnetometer reading along Z, in mgauss.
	double imu_magnetoGetZAxis(IMU imu);


	/// \brief Get all magnetometer readings at once.
	///
    /// \param[in] imu The IMU structure to use for communication.
    /// \param[out] x Magnetometer reading along X, in mgauss.
    /// \param[out] y Magnetometer reading along Y, in mgauss.
    /// \param[out] z Magnetometer reading along Z, in mgauss.
	void imu_magnetoGetValues(IMU imu, double *x, double *y, double *z);

#endif
