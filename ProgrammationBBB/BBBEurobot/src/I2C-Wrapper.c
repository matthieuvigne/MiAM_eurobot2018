#include "BBBEurobot/I2C-Wrapper.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>       
#include <string.h>
#include <glib.h>

//Mutex for thread safety.
GMutex mut;

int i2c_open(char *portName)
{
	int file = open(portName, O_RDWR);
	if(file < 0)
	{
		printf("Failed to open the i2c bus: %s\n", g_strerror(errno));
		return file;
	}
    return file;
}


void changeSlave(int port, int address)
{
	if (ioctl(port,I2C_SLAVE,address) < 0) 
        printf("I2C: failed to talk to slave %d : %s.\n", address, strerror(errno));
}


int i2c_writeRegister(int port, int address, unsigned char reg, unsigned char data)
{
	unsigned char txbuf[2] = {reg, data};
	g_mutex_lock (&mut);
	changeSlave(port, address);
	int res = write(port, txbuf, 2);
	g_mutex_unlock (&mut);
	if(res != 2)
		return -1;
	return 0;
}


unsigned char i2c_readRegister(int port, int address, unsigned char reg)
{
	unsigned char b;
	g_mutex_lock (&mut);
	changeSlave(port, address);
	write(port, &reg, 1);
	read(port, &b, 1);
	g_mutex_unlock (&mut);
	return b;
}


void i2c_readRegisters(int port, int address, unsigned char reg, int length, unsigned char *output)
{
	g_mutex_lock (&mut);
	changeSlave(port, address);
	write(port, &reg, 1);
	read(port, output, length);
	g_mutex_unlock (&mut);
}
	
void i2c_close(int device)
{
	if(device < 0)
		return;
	close(device);
}
