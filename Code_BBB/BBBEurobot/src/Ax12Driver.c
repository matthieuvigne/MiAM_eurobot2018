//driver pour les Dynamixels AX12
#include "BBBEurobot/Ax12Driver.h"

//communication par port serie
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>


int ax12_getRegister(Ax12 ax12, int motorID, int reg)
{
	unsigned char message[8];
	message[0] = 0xFF;
	message[1] = 0xFF;
	message[2] = motorID;
	message[3] = 4;
	message[4] = 2;
	message[5] = reg;
	message[6] = 1;
	message[7] = ~((motorID + 7 + reg)%256);
	write(ax12.port, message,8);
	
	g_usleep(520);
//	char reponse[6];
	//if(read(port, reponse, 6) > 0) 
		//return reponse[5];
	
	return -1;
}

// Internal functions to set one or several registers.
void ax12SetRegister(Ax12 ax12, int motorID, int reg, int data)
{
	// Set direction to 0 to enable a write, then reset it to 1.
	FILE *f = fopen(ax12.dir, "w");
	fprintf(f,"0");
	fclose(f);
	g_usleep(100);
	
    char message[8];
    message[0] = 0xFF;
	message[1] = 0xFF;
	message[2] = motorID;
	message[3] = 4;
	message[4] = 3;
	message[5] = reg;
	message[6] = data&0xFF;
	message[7] = ~((motorID + 7 + reg + message[6])%256);
	write(ax12.port, message,8);
	
	g_usleep(100);
	f = fopen(ax12.dir, "w");
	fprintf(f,"1");
	fclose(f);
}


void ax12SetRegister2(Ax12 ax12, int motorID, int reg, int value)
{
	// Set direction to 0 to enable a write, then reset it to 1.
	FILE *f = fopen(ax12.dir, "w");
	fprintf(f,"0");
	fclose(f);
	g_usleep(100);
	
    unsigned char message[9];
    message[0] = 0xFF;
	message[1] = 0xFF;
	message[2] = motorID;
	message[3] = 5;
	message[4] = 3;
	message[5] = reg;
	message[6] = value%256;
	message[7] = ((value/256) % 256 );
	message[8] = ~((motorID + 8 + reg + message[6] + message[7])%256);
	write(ax12.port, message,9);
	g_usleep(100);
	f = fopen(ax12.dir, "w");
	fprintf(f,"1");
	fclose(f);
}


void ax12SetRegister4(Ax12 ax12, int motorID, int reg, int data, int data2)
{
	// Set direction to 0 to enable a write, then reset it to 1.
	FILE *f = fopen(ax12.dir, "w");
	fprintf(f,"0");
	fclose(f);
	g_usleep(100);
	
    unsigned char message[11];
    message[0] = 0xFF;
	message[1] = 0xFF;
	message[2] = motorID;
	message[3] = 7;
	message[4] = 3;
	message[5] = reg;
	message[6] = data%256;
	message[7] = ((data/256) % 256 );
	message[8] = data2%256;
	message[9] = ((data2/256) % 256 );
	message[10] = ~((motorID + 10 + reg + message[6] + message[7] + message[8] + message[9])%256);
	write(ax12.port, message,11);
	
	g_usleep(100);
	f = fopen(ax12.dir, "w");
	fprintf(f,"1");
	fclose(f);
}


// See Ax12 documentation for details on the registers.
void ax12_setPosition(Ax12 ax12, int motorID, int position)
{
	ax12SetRegister2(ax12, motorID, AX12_RAM_GOAL_POSITION_L, position);
}


void ax12_setSpeed(Ax12 ax12, int motorID, int speed)
{
	ax12SetRegister2(ax12, motorID, AX12_RAM_MOVING_SPEED_L, speed);
}


void ax12_eeprom_setAngleLimit(Ax12 ax12, int motorID, int lowLimit, int highLimit)
{
	if(lowLimit > highLimit)
		return;
	ax12SetRegister2(ax12, motorID, AX12_EEPROM_CW_ANGLE_LIM_L, lowLimit);
	g_usleep(50000);
	ax12SetRegister2(ax12, motorID, AX12_EEPROM_CCW_ANGLE_LIM_L, highLimit);
}


gboolean ax12_isMoving(Ax12 ax12, int motorID)
{
	if(ax12_getRegister(ax12, motorID, AX12_RAM_MOVING) == 1)
		return TRUE;
	return FALSE;
}

void ax12_servoLed(Ax12 ax12, int motorID, gboolean on)
{
	ax12SetRegister(ax12, motorID, AX12_RAM_LED, on);
}

void ax12_torqueOn(Ax12 ax12, int motorID, gboolean on)
{
	ax12SetRegister(ax12, motorID, AX12_RAM_TORQUE_ENABLE, on);
}


gboolean ax12_init(Ax12 *ax12, gchar *portName, int speed, int dirPin) 
{
	// Open UART port
	int fd;
	struct termios options;

	fd = open(portName, O_RDWR | O_NDELAY);

	if (fd == -1) 
	{
	  printf("Ax 12 init error: cannot open %s\n", portName);
	  return FALSE;
	}
	// Nonblocking operation
	//~ fcntl(fd, F_SETFL, FNDELAY);


	tcgetattr(fd, &options);

	//Set communication speed and options
	//8 bit, 1 stop, no parity
	cfsetispeed(&options, speed);
	cfsetospeed(&options, speed);

	//~ options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_lflag &= ~ECHO; 
	tcsetattr(fd, TCSANOW, &options);
	
	//setup direction pin
	FILE *f = fopen("/sys/class/gpio/export", "a");
	gchar *dir= g_strdup_printf("%d", dirPin);
	fprintf(f,dir);
	fclose(f);
	g_free(dir);
	
	dir = g_strdup_printf("/sys/class/gpio/gpio%d/direction", dirPin);
	f = fopen(dir, "w");
	g_free(dir);
	fprintf(f, "out");
	fclose(f);
	
	dir = g_strdup_printf("/sys/class/gpio/gpio%d/value", dirPin);
	
	f = fopen(dir, "w");
	fprintf(f,"1");
	fclose(f);
	
	ax12->port = fd;
	ax12->dir = g_strdup(dir);
	g_free(dir);
	
	// Setup all servos : use broadcast address.
	int broadcast = 0xFE;
	// Set return delay time to 10us.
	ax12SetRegister(*ax12, broadcast, AX12_EEPROM_RETURN_DELAY, 5);
	// Respond only to read data package, not in	struction package.
	ax12SetRegister(*ax12, broadcast, AX12_EEPROM_STATUS_RETURN_LEVEL, 1);
	// Blink led on all errors
	ax12SetRegister(*ax12, broadcast, AX12_EEPROM_STATUS_RETURN_LEVEL, 0x7F);
	// Set servo to maximum torque
	ax12SetRegister2(*ax12, broadcast, AX12_EEPROM_MAX_TORQUE_L, 0x3FF);
	// Turn on torque on all servos
	ax12_torqueOn(*ax12, broadcast, TRUE);

	return TRUE;
}
