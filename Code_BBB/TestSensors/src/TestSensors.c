/// \file TestSensors.c
/// \brief Test various sensors.
///
/// \details
///	\note	Feb, 7th 2018


// Include standard libraries
#include <stdlib.h>
#include <stdio.h>

#include <TestSensors.h>


void testIMU(I2CAdapter* adapter, double testDuration)
{
	IMU imu;
	// Initialize
	if (imu_initDefault(&imu, adapter, TRUE))
	{
		printf("OK: IMU initialization success.\n");
		// Print readings for duration of the test.
		printf("Gyro x\tGyro y\tGyro z\tAccel x\tAccel y\tAccel z\tMag x\tMag y\tMag z\n");
		GTimer *testTimer = g_timer_new();
		g_timer_start(testTimer);
		while(g_timer_elapsed(testTimer, NULL) < testDuration)
		{
			double gx, gy, gz;
			double ax, ay, az;
			double mx, my, mz;
			imu_gyroGetValues(imu, &gx, &gy, &gz);
			imu_accelGetValues(imu, &ax, &ay, &az);
			imu_magnetoGetValues(imu, &mx, &my, &mz);
			printf("%c[2K\r%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", 27, gx, gy, gz, ax, ay, az, mx, my, mz);
			fflush(stdout);
			g_usleep(50000);
		}
		printf("\n");
		g_free(testTimer);
	}
	else
	{
		printf("ERROR: IMU initialization failed.\n");
	}
}

void testMouse(gchar *portName, double testDuration)
{
	// Initialize sensor (cannot determine if success).
	ADNS9800 mouseSensor;
	if(ANDS9800_init(&mouseSensor, portName))
	{
		printf("OK: Mouse sensor initialization success.\n");
		printf("X\tY\n");
		// Integrate sensor data.
		double x = 0;
		double y = 0;

		GTimer *testTimer = g_timer_new();
		g_timer_start(testTimer);
		while(g_timer_elapsed(testTimer, NULL) < testDuration)
		{
			double deltaX, deltaY;
			ADNS9800_getMotion(mouseSensor, &deltaX, &deltaY);
			x += deltaX;
			y += deltaY;
			printf("%c[2K %.1f\t%.1f\r", 27, x, y);
			fflush(stdout);
			g_usleep(10000);
		}
		printf("\n");
	}
	else
	{
		printf("ERROR: Mouse sensor initialization failed.\n");
	}
}

void testColor(I2CAdapter* adapter, double testDuration)
{
	ColorSensorTCS3472 sensor;
	// Initialize
	if (colorSensor_init(&sensor, adapter))
	{
		colorSensor_setGain(sensor, TCS34725_GAIN_60X);
		colorSensor_setIntegrationTime(sensor, 10);
		printf("OK: ColorSensorTCS3472 initialization success.\n");
		// Print readings for duration of the test.
		GTimer *testTimer = g_timer_new();
		g_timer_start(testTimer);
		while(g_timer_elapsed(testTimer, NULL) < testDuration)
		{
			ColorOutput colorOutput = colorSensor_getData(sensor);
			printf("Color reads: (%d, %d, %d, %d)\n",
				colorOutput.red,
				colorOutput.green,
				colorOutput.blue,
				colorOutput.clear);
			g_usleep(500000);
		}
		g_free(testTimer);
	}
	else
	{
		printf("ERROR: ColorSensorTCS3472 initialization failed.\n");
	}
}

void testLCD(I2CAdapter* adapter, double testDuration)
{
	LCD lcd;
	// Initialize
	if (lcd_initDefault(&lcd, adapter))
	{
		printf("OK: LCD initialization success.\n");
		printf("Testing LCD screen and RGB backlight\n");
		lcd_setText(lcd, "First line", 0);
		lcd_setText(lcd, "Second line", 1);
		g_usleep(2000000);
		lcd_setTextCentered(lcd, "RGB Test", 0);
		lcd_setTextCentered(lcd, "Red", 1);
		lcd_setBacklight(lcd, TRUE, FALSE, FALSE);
		g_usleep(1000000);
		lcd_setTextCentered(lcd, "Green", 1);
		lcd_setBacklight(lcd, FALSE, TRUE, FALSE);
		g_usleep(1000000);
		lcd_setTextCentered(lcd, "Blue", 1);
		lcd_setBacklight(lcd, FALSE, FALSE, TRUE);
		g_usleep(1000000);
		// Monitor buttons pressed or not
		lcd_setTextCentered(lcd, "Button status", 0);

		GTimer *testTimer = g_timer_new();
		g_timer_start(testTimer);
		while(g_timer_elapsed(testTimer, NULL) < testDuration)
		{
			gchar *buttonStatus = g_strdup_printf("S%d R%d U%d L%d D%d",
				lcd_isButtonPressed(lcd, LCD_BUTTON_SELECT),
				lcd_isButtonPressed(lcd, LCD_BUTTON_RIGHT),
				lcd_isButtonPressed(lcd, LCD_BUTTON_UP),
				lcd_isButtonPressed(lcd, LCD_BUTTON_LEFT),
				lcd_isButtonPressed(lcd, LCD_BUTTON_DOWN));
			printf("\rButton status: %s", buttonStatus);
			lcd_setTextCentered(lcd, buttonStatus, 1);
			fflush(stdout);
			g_usleep(50000);
		}
		printf("\n");
	}
	else
	{
		printf("ERROR: LCD initialization failed.\n");
	}
}


void testDigitalRead(const int* cape_dig_in, int cape_dig_in_length, double testDuration)
{
	// Read value each 10 ms
	GTimer *testTimer = g_timer_new();
	g_timer_start(testTimer);
	while(g_timer_elapsed(testTimer, NULL) < testDuration)
	{
		printf("Digital read (pin, value):\t");
		for (int j=0; j<cape_dig_in_length; j++)
		{
			printf("(%d, %d) \t", cape_dig_in[j],
				gpio_digitalRead(cape_dig_in[j]));
			fflush(stdout);
		}
		printf("\r");
		g_usleep(10000);
	}
	printf("\n");
	g_free(testTimer);
}


void testDigitalWrite(const int* cape_dig_out, int cape_dig_out_length, double testDuration)
{
	// Write value each 500 ms
	GTimer *testTimer = g_timer_new();
	g_timer_start(testTimer);
	int i = 0;
	while(g_timer_elapsed(testTimer, NULL) < testDuration)
	{
		i++;
		printf("Dig write (pin, value, result):\t");
		for (int j=0; j<cape_dig_out_length; j++)
		{
			printf("(%d, %d, %d) \t", cape_dig_out[j], i%2,
				gpio_digitalWrite(cape_dig_out[j], i%2));
		}
		printf("\r");
		g_usleep(500000);
	}
	g_free(testTimer);
}


void testAnalogRead(const int* cape_analog_in, int cape_analog_in_length, double testDuration)
{
	// Read value each 10 ms
	GTimer *testTimer = g_timer_new();
	g_timer_start(testTimer);
	while(g_timer_elapsed(testTimer, NULL) < testDuration)
	{
		printf("\r%c[2KAnalog read (0-%d):\t", 27, cape_analog_in_length -1);
		for (int j=0; j<cape_analog_in_length; j++)
			printf("%d\t", gpio_analogRead(cape_analog_in[j]));
		fflush(stdout);
		g_usleep(50000);
	}
	printf("\n");
}
