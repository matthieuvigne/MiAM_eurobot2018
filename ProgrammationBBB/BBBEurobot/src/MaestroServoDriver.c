#include "BBBEurobot/MaestroServoDriver.h"

#include <math.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

// Send a command to the device.
// driver: device to send command to.
// commandID: the command ID to use.
// parameters: command parameters.
// length: length of parameters.
// return: result of write.
int sendCommand(MaestroDriver driver, int commandID, char *parameters, int length)
{
	char message[3 + length];
	message[0] = 0xAA;
	message[1] = driver.deviceID;
	message[2] = commandID;
	for(int i = 0; i < length; i++)
		message[3+i] = parameters[i];
	return write(driver.port, message, 3 + length);
}


gboolean maestro_init(MaestroDriver *driver, gchar *portName, int deviceID)
{
	// Open port
	driver->port = open(portName, O_RDWR | O_NDELAY);
	driver->deviceID = deviceID;

	if(driver->port == -1)
		return FALSE;

	// Nonblocking operation
	//~ fcntl(fd, F_SETFL, FNDELAY);

	// Setup UART port.
	struct termios options;
	tcgetattr(driver->port, &options);

	//Set communication speed and options
	//8 bit, 1 stop, no parity
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	//~ options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~ECHO;
	tcsetattr(driver->port, TCSANOW, &options);

	return TRUE;
}


void maestro_setPosition(MaestroDriver driver, int servo, double position)
{
	char parameters[3];
	parameters[0] = servo;
	// Command unit: 0.25us.
	int servoCommand = (int) floor(position * 4);
	parameters[1] = servoCommand & 0xFF;
	parameters[2] = (servoCommand >> 8) & 0xFF;
	sendCommand(driver, 0x04, parameters, 3);
}


void maestro_setSpeed(MaestroDriver driver, int servo, int speed)
{
	char parameters[3];
	parameters[0] = servo;
	// Command unit: 0.25 us/s
	int servoCommand = speed / 25;
	parameters[1] = servoCommand & 0xFF;
	parameters[2] = (servoCommand >> 8) & 0xFF;
	sendCommand(driver, 0x07, parameters, 3);
}
