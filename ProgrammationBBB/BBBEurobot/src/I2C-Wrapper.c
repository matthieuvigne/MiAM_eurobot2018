#include "BBBEurobot/I2C-Wrapper.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

gboolean i2c_open(I2CAdapter *adapter, gchar *portName)
{
	adapter->file = open(portName, O_RDWR);
	g_mutex_init(&(adapter->portMutex));
	if(adapter->file < 0)
	{
		printf("Failed to open i2c bus %s: %s\n", portName, g_strerror(errno));
		return FALSE;
	}
    return TRUE;
}


void changeSlave(int file, guint8 address)
{
	if (ioctl(file,I2C_SLAVE,address) < 0)
        printf("I2C: failed to talk to slave %d : %s.\n", address, strerror(errno));
}


gboolean i2c_writeRegister(I2CAdapter *adapter, guint8 address, guint8 reg, guint8 data)
{
	if(adapter->file < 0)
	{
		printf("Error writing to I2C port: invalid file descriptor.\n");
		return FALSE;
	}
	guint8 txbuf[2] = {reg, data};
	g_mutex_lock (&(adapter->portMutex));
	changeSlave(adapter->file, address);
	int result = write(adapter->file, txbuf, 2);
	g_mutex_unlock (&(adapter->portMutex));
	if(result != 2)
	{
		printf("Error writing to slave %d: %s\n", address, g_strerror(errno));
		return FALSE;
	}
	return TRUE;
}


guint8 i2c_readRegister(I2CAdapter *adapter, guint8 address, guint8 registerAddress)
{
	guint8 registerValue;
	i2c_readRegisters(adapter, address, registerAddress, 1, &registerValue);
	return registerValue;
}


gboolean i2c_readRegisters(I2CAdapter *adapter, guint8 address, guint8 registerAddress, int length, guint8 *output)
{
	if(adapter->file < 0)
	{
		printf("Error reading from I2C port: invalid file descriptor.\n");
		return FALSE;
	}
	gboolean returnValue = TRUE;
	g_mutex_lock (&(adapter->portMutex));
	changeSlave(adapter->file, address);
	int result = write(adapter->file, &registerAddress, 1);
	if(result < 0)
	{
		printf("Error writing to slave %d: %s\n", address, g_strerror(errno));
		returnValue = FALSE;
	}
	result = read(adapter->file, output, length);
	if(result < 0)
	{
		printf("Error reading from slave %d: %s\n", address, g_strerror(errno));
		returnValue = FALSE;
	}
	g_mutex_unlock (&(adapter->portMutex));

	return returnValue;
}

void i2c_close(int device)
{
	if(device < 0)
		return;
	close(device);
}
