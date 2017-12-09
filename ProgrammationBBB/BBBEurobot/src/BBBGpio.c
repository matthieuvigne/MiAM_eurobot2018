#include "BBBEurobot/BBBGpio.h"
#include <stdlib.h>
#include <stdio.h>

// Internal function : access the cape manager to check if Eurobot is enabled. Exists if file access is not granted.
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
	gchar *line="";
	
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
	return isEurobotEnabled;
}

void gpio_enableSerialPorts()
{
	// Check if the overlay is already enabled.
	if(isEurobotEnabled())
		return;
	// Else, let us first check that the overlay file exists.
	gchar *overlayFile = "/lib/firmware/Eurobot-00A0.dtbo";
	if(g_file_test(overlayFile, G_FILE_TEST_EXISTS) == FALSE)
	{
		printf("Enabling serial ports failed: cannot find overlay (%s).\n", overlayFile);
		exit(0);
	}
	
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
}
