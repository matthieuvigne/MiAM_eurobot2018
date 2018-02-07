#include <stdlib.h>
#include <stdio.h>

#include "BBBEurobot/BeagleboneCape.h"
#include "BBBEurobot/BBBGpio.h"

// See header directly for documentation of these constants.
const gchar* SPI_0 = "/dev/spi1.0";
const gchar* SPI_10 = "/dev/spi2.0";
const gchar* SPI_11 = "/dev/spi2.1";
const gchar* I2C_1 = "/dev/i2c-1";
const gchar* I2C_2 = "/dev/i2c-2";
const int CAPE_ANALOG[CAPE_N_ANALOG] = {0, 1, 2, 3, 6, 5, 4};
const int CAPE_DIGITAL[CAPE_N_DIGITAL] = {66, 67, 69, 68, 45, 44, 26};
const int CAPE_LED[CAPE_N_LED] = {47, 46};

// Internal function : access the cape manager to check if Eurobot cape is enabled. Exits if file access is not granted.
gboolean isEurobotEnabled()
{
	// Look in slots file if the Eurobot overlay is already enabled.
	GError *error = NULL;
	GIOChannel *slot = g_io_channel_new_file ("/sys/devices/bone_capemgr.9/slots", "r", &error);
	if(error != NULL)
	{
		printf("Enabling serial ports failed: cannot open cape manager (/sys/devices/bone_capemgr.9/slots).\n");
		exit(0);
	}

	gboolean isEurobotEnabled = FALSE;
	gchar *line= NULL;

	// Check if the overlay is already enabled
	while ( g_io_channel_read_line (slot, &line, NULL, NULL, NULL )!= G_IO_STATUS_EOF)
	{
		if(g_strstr_len (line, -1, "Eurobot")!= NULL)
		{
			isEurobotEnabled = TRUE;
			break;
		}
		g_free(line);
	}
	g_free(line);
	g_io_channel_shutdown(slot, TRUE, NULL);
	g_io_channel_unref(slot);
	return isEurobotEnabled;
}


void BBB_enableCape()
{
	// Check if the overlay is already enabled.
	if(isEurobotEnabled())
		return;

	// Else, let us first check that the overlay file exists.
	gchar *overlayFile = g_strdup_printf("/lib/firmware/Eurobot-00A0.dtbo");
	if(g_file_test(overlayFile, G_FILE_TEST_EXISTS) == FALSE)
	{
		printf("Enabling serial ports failed: cannot find overlay (%s).\n", overlayFile);
		exit(0);
	}
	g_free(overlayFile);
	// Enable the overlay.
	system("echo Eurobot > /sys/devices/bone_capemgr.9/slots");

	// Check that the overlay is indeed enabled.
	if(isEurobotEnabled())
		return;
	else
	{
		printf("Enabling serial ports failed: unknown error.\n");
		exit(0);
	}

	// Serial ports are enabled, nothing else to be done there.
	// Analog ports do not need to be enable.
	// Set all exposed GPIOs as input, except LEDs which should be outputs, set to low.
	for(int i = 0; i < CAPE_N_DIGITAL; i++)
		gpio_exportPin(CAPE_DIGITAL[i], "in");

	for(int i = 0; i < CAPE_N_LED; i++)
	{
		gpio_exportPin(CAPE_LED[i], "out");
		gpio_digitalWrite(CAPE_LED[i], 0);
	}
}
