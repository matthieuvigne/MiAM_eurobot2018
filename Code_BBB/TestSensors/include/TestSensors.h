/// \file TestSensors.h
/// \brief Test various sensors.
///
/// \details
///	\note	Feb, 7th 2018

// Include BBBEurobot library.
#include <BBBEurobot/BBBEurobot.h>


/// \brief Tests IMU.
///
/// \details This function inits the IMU, and if successful, prints read
///	values for TEST_TIME_MICROSECONDS microseconds.
///
///
/// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C
///				port (as returned by the i2c_open function, see I2C-Wrapper.h).
/// \param[in] testDuration Duration of the test (in seconds).
void testIMU(I2CAdapter* adapter, double testDuration);


/// \brief Tests Mouse.
///
/// \details This function inits the Mouse, and if successful, prints
/// read values for test_duration microseconds.
///
///
/// \param[in] portName Name of the SPI port
/// \param[in] portName Name of the port, in the file system (i.e. a
///	string "/dev/spidevx").
/// \param[in] testDuration Duration of the test (in seconds).
void testMouse(gchar *portName, double testDuration);


/// \brief Tests color sensor.
///
/// \details This function inits the color sensor, and if successful,
///	prints read values for test_duration microseconds.
///
///
/// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C
///	port (as returned by the i2c_open function, see I2C-Wrapper.h).
/// \param[in] testDuration Duration of the test (in seconds).
void testColor(I2CAdapter* adapter, double testDuration);


/// \brief Tests LCD screen.
///
/// \details This function inits the LCD screen, and if successful,
/// writes something on the screen on the two lines alternatively each
/// 1 second, changes the color and monitor button pressed for
///	test_duration microseconds.
///
///
/// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C
///	port (as returned by the i2c_open function, see I2C-Wrapper.h).
/// \param[in] address Address of the I2C LCD screen.
/// \param[in] testDuration Duration of the test (in seconds).
void testLCD(I2CAdapter* adapter, double testDuration);


/// \brief Tests digital input.
///
/// \details This function reads the digital input of the provided pins
/// each 10ms for test_duration microseconds.
///
///
/// \param[in] cape_dig_in Array containing digital input pins.
/// \param[in] cape_dig_in_length Length of cape_dig_in.
/// \param[in] testDuration Duration of the test (in seconds).
void testDigitalRead(const int* cape_dig_in, int cape_dig_in_length, double testDuration);


/// \brief Tests digital write.
///
/// \details This function writes successively 0, 1 on the provided pins
/// each 500ms for test_duration microseconds.
///
///
/// \param[in] cape_dig_out Array containing digital output pins.
/// \param[in] cape_dig_out_length Length of cape_dig_out.
/// \param[in] testDuration Duration of the test (in seconds).
void testDigitalWrite(const int* cape_dig_out, int cape_dig_out_length, double testDuration);


/// \brief Tests analogic input.
///
/// \details This function reads the analogic input of the provided pins
/// each 10ms for test_duration microseconds.
///
///
/// \param[in] cape_analog_in Array containing analog input pins.
/// \param[in] cape_analog_in_length Length of cape_analog_in.
/// \param[in] testDuration Duration of the test (in seconds).
void testAnalogRead(const int* cape_analog_in, int cape_analog_in_length, double testDuration);
