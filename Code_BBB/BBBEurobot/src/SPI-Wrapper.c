#include "BBBEurobot/SPI-Wrapper.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int spi_open(const gchar *portName, const int frequency)
{

    int port = open(portName, O_RDWR) ;
    if(port < 0)
    {
        printf("Error opening SPI bus %s %d\n", portName, errno);
        return -1 ;
    }

    // Setup the SPI bus, mode 3 (SPI_CPOL | SPI_CPHA).
    // See documentation of spi/spidev for details.
    int mode = SPI_MODE_3;
    if(ioctl(port, SPI_IOC_WR_MODE, &mode) < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }

    if(ioctl(port, SPI_IOC_RD_MODE, &mode) < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }
	int nBits = 8;
    if(ioctl(port, SPI_IOC_WR_BITS_PER_WORD, &nBits) < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }

    if(ioctl(port, SPI_IOC_RD_BITS_PER_WORD, &nBits) < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }

    if(ioctl(port, SPI_IOC_WR_MAX_SPEED_HZ, &frequency)   < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }

    if(ioctl(port, SPI_IOC_RD_MAX_SPEED_HZ, &frequency)   < 0)
    {
        printf("Error configuring port %s %d\n", portName, errno);
        return -1 ;
    }
    return port;
}


void spi_close(const int port)
{
	if(port > 0)
		close(port);
}
