#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <BBBEurobot/I2C-Wrapper.h>
#include "Global.h"
#include "Initialize.h"
#include "Motion.h"
#include "Strategy.h"
#include "Config.h"


// Becomes true once the motors have been initialised correctly.
gboolean successfulMotorInit = FALSE;

void init_MainStartup()
{
	gboolean error = FALSE;
	// Activate beaglebone overlays, if needed
	GIOChannel *slot = g_io_channel_new_file ("/sys/devices/bone_capemgr.9/slots", "r", NULL);
	if(slot == NULL)
	{
		printf("Critical error: cannot open cape manager (/sys/devices/bone_capemgr.9/slots).\nAborting execution.\n");
		exit(0);
	}
	
	gboolean enableADC = TRUE;
	gboolean enableSPI = TRUE;
	gboolean enableUART = TRUE;
	gboolean enableGPIO = TRUE;
	gchar *line="";
	
	// Check if the overlay is already enabled
	while ( g_io_channel_read_line (slot, &line, NULL, NULL, NULL )!= G_IO_STATUS_EOF)
	{
		if(g_strstr_len (line, -1, "BB-ADC")!= NULL)
			enableADC = FALSE;
		if(g_strstr_len (line, -1, "spidev1")!= NULL)
			enableSPI = FALSE;
		if(g_strstr_len (line, -1, "BB-UART1")!= NULL)
			enableUART = FALSE;
		if(g_strstr_len (line, -1, "MinotaureGPIO")!= NULL)
			enableGPIO = FALSE;
		g_free(line);
	}
	g_free(line);
	g_io_channel_shutdown(slot, TRUE, NULL);
	
	// Enable the overlays that are not yet enabled.
	if(enableADC)
		system("echo BB-ADC > /sys/devices/bone_capemgr.9/slots");
	if(enableSPI)
		system("echo spidev1 > /sys/devices/bone_capemgr.9/slots");
	if(enableUART)
		system("echo BB-UART1 > /sys/devices/bone_capemgr.9/slots");
	if(enableGPIO)
		system("echo Minotaure-GPIO > /sys/devices/bone_capemgr.9/slots");

	// Open i2c port.
	int dev = i2c_open(CAPE_I2C);
	if(dev < 0)
	{
		printf("Error opening i2c port\n");
		error = TRUE;
	}
	
	// Init led display
	if(ledDriver_init(&leddriver, dev, 0x43) == -1)
	{
		printf("Error creating led driver\n");
		error = TRUE;
	}
	
	// Led testing
	//~ while(TRUE)
	//~ {
		//~ for(int x=0; x<16; x++)
			//~ ledDriver_setLedBrightness(&leddriver, x, 127);
		//~ g_usleep(250000);
		//~ for(int x=0; x<16; x++)
			//~ ledDriver_setLedBrightness(&leddriver, x, 0);
		//~ g_usleep(250000);
	//~ }
	
	// Init imu	
	if(imu_initDefault(&imu, dev) == -1)
	{
		printf("Error initializing IMU\n");
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_STATUS, 127,0,0);
		error = TRUE;
	}

	// Init motor drivers
	if(!motion_initMotors())
	{
		printf("Error initializing motors\n");
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_STATUS, 127,0,127);
		error = TRUE;
	}
	else
		successfulMotorInit = TRUE;
		
	// Init AX12
	if(!ax12_init(&ax12, CAPE_AX12_PORT,B57600, CAPE_AX12_DIRECTION_PIN))
	{
		printf("Error initializing Ax12\n");
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_STATUS, 0,0,127);
		error = TRUE;
	}
	else
		strategy_servoToInitialPosition();
	
	// Activate roof switch, pin configuration already done thanks to MINOTAURE_GPIO
	GIOChannel *gpioExportFile = g_io_channel_new_file("/sys/class/gpio/export", "a", NULL);
	if(gpioExportFile == NULL)
	{
		printf("Critical error: cannot open gpio export (/sys/class/gpio/export).\nAborting execution.\n");
		exit(0);
	}
	gchar *switchNumber = g_strdup_printf("%d", ROOF_SIDE_CHOOSING_SWITCH);
	g_io_channel_write_chars(gpioExportFile, switchNumber, -1, NULL, NULL);
	g_io_channel_flush (gpioExportFile, NULL);
	g_free(switchNumber);
	switchNumber = g_strdup_printf("%d", ROOF_STARTING_SWITCH);
	g_io_channel_write_chars(gpioExportFile, switchNumber, -1, NULL, NULL);
	g_io_channel_flush (gpioExportFile, NULL);
	g_free(switchNumber);
	g_io_channel_shutdown(gpioExportFile, TRUE, NULL);
	
	if(!error)
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_STATUS, 0,127,0);
}


void init_getSide()
{
	// Get the string representing the switch.
	gchar *switchName = g_strdup_printf("/sys/class/gpio/gpio%d/value", ROOF_SIDE_CHOOSING_SWITCH);
	int f = open(switchName, O_RDONLY);
	g_free(switchName);
	char data;
	read(f, &data, 1);
	close(f);
	blueSide = (data == '0' ? TRUE : FALSE);
	
	if(blueSide)
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_TEAM, 0,0,127);
	else
		ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_TEAM, 127,127,0);
}


void init_waitForStart()
{
	gboolean start = FALSE;
	while (!start)
	{
		// Retry to initialize the motors if they are not initialized already.
		if(!successfulMotorInit)
		{
			successfulMotorInit = motion_initMotors();
			if(successfulMotorInit)
			{
				printf("Motor init sucessful.\n");
				ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_STATUS, 0,127,0);
				strategy_servoToInitialPosition();
			}
		}
		
		init_getSide();
		// Read the sate of the jack.
		gchar *switchName = g_strdup_printf("/sys/class/gpio/gpio%d/value", ROOF_STARTING_SWITCH);
		int f = open(switchName, O_RDONLY);
		g_free(switchName);
		char data;
		read(f, &data, 1);
		close(f);
		start = (data == '1' ? TRUE : FALSE);
		g_usleep(10000);
	}
}


void init_waitForJackPlug()
{
	gboolean plug = FALSE;
	while (!plug)
	{
		gchar *switchName = g_strdup_printf("/sys/class/gpio/gpio%d/value", ROOF_STARTING_SWITCH);
		int f = open(switchName, O_RDONLY);
		g_free(switchName);
		char data;
		read(f, &data, 1);
		close(f);
		plug = (data == '0' ? TRUE : FALSE);
		g_usleep(10000);
	}
}
