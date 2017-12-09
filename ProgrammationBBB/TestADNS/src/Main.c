// This code is a test bench of the ADNS9800 mouse sensor.
// This sensor works with SPI communication - it returns deltaX and deltaY position to the master.
// This simple demo integrates the position from the sensor and prints it in the terminal.

// Include standard libraries
#include <stdlib.h>
#include <stdio.h>
// Include BBBEurobot library.
#include <BBBEurobot/BBBEurobot.h>


int main(int argc, char **argv)
{
	// Enable serial ports to talk to the mouse sensor using SPI.
	gpio_enableSerialPorts();
		
	// Initialize the SPI communication.
	ADNS9800 mouseSensor;
	ANDS9800_initStructure(&mouseSensor, "/dev/spidev2.1", 500000);
	printf("Starting optical sensor\n");
	ADNS9800_performStartup(&mouseSensor);
	printf("Reading data\n");
	// Integrate sensor data.
	double x = 0;
	double y = 0;	
	while(TRUE)
	{
		double deltaX, deltaY;
		ADNS9800_getMotion(mouseSensor, &deltaX, &deltaY);
		x += deltaX;
		y += deltaY;
		printf("%c[2K %f %f\r", 27, x, y);
		fflush(stdout);
		g_usleep(10000);
	}
	
	return 0;
}

