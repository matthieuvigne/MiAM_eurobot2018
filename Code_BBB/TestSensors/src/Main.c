/// \file Main.c
/// \brief Test various sensors.
///
/// \details
///	\note	Feb, 7th 2018


#include <TestSensors.h>	// See this header for function details.

#include <stdio.h>
/*
 * PARAMETERS
 */

/// Select tests to run
// Comment #define line if test should be disabled
#define run_IMU
#define run_ADNS9800
#define run_ColorSensorTCS3472
#define run_LCD
#define run_DIGITAL_READ
#define run_DIGITAL_WRITE
#define run_ANALOG_READ


/// General parameters
// Test run time for each test
#define TEST_TIME 5


/// Addresses & port numbers
#define IMU_PORT_NAME "/dev/i2c-1"
#define MOUSE_PORT_NAME "/dev/spidev2.0"
#define COLOR_PORT_NAME "/dev/i2c-1"
#define LCD_PORT_NAME "/dev/i2c-1"

int main(int argc, char **argv)
{
	// Initialize the BB
	BBB_enableCape();

	/*
	 * IMU
	 */
#ifdef run_IMU
	{
		printf(">>> Test IMU <<<\n");
		// Define I2CAdapter
		I2CAdapter adapter;
		i2c_open(&adapter, IMU_PORT_NAME);
		// Test IMU
		testIMU(&adapter, TEST_TIME);
	}
#endif

	/*
	 * Mouse sensor
	 */
#ifdef run_ADNS9800
	{
		printf(">>> Test mouse sensor ADNS9800 <<<\n");
		testMouse(MOUSE_PORT_NAME, TEST_TIME);

	}
#endif

	/*
	 * Color sensor
	 */
#ifdef run_ColorSensorTCS3472
	{
		printf(">>> Test color sensor ColorSensorTCS3472 <<<\n");
		// Define I2CAdapter
		I2CAdapter adapter;
		i2c_open(&adapter, COLOR_PORT_NAME);
		// Test Color sensor
		testColor(&adapter, TEST_TIME);
	}
#endif

	/*
	 * Screen and buttons
	 */
#ifdef run_LCD
	{
		printf(">>> Test LCD and buttons <<<\n");
		// Define I2CAdapter
		I2CAdapter adapter;
		i2c_open(&adapter, LCD_PORT_NAME);
		// Test LCD
		testLCD(&adapter, TEST_TIME);
	}
#endif

	/*
	 * Digital read
	 */
#ifdef run_DIGITAL_READ
	{
		printf(">>> Test Digital read <<<\n");
		testDigitalRead(CAPE_DIGITAL, CAPE_N_DIGITAL, TEST_TIME);
	}
#endif

	/*
	 * Digital write
	 */
#ifdef run_DIGITAL_WRITE
	{
		printf(">>> Test Digital write <<<\n");
		testDigitalWrite(CAPE_LED, CAPE_N_LED, TEST_TIME);
	}
#endif

	/*
	 * Analogic inputs
	 */
#ifdef run_ANALOG_READ
	{
		printf(">>> Test Analog read <<<\n");
		testAnalogRead(CAPE_ANALOG, CAPE_N_ANALOG, TEST_TIME);
	}
#endif

}
