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

void gpio_enablePorts()
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
}

// Internal function : get the content of the first line of the file. Returns TRUE on success, FALSE on error.
gboolean getFileContent(gchar *fileName, gchar **output)
{
	GError *error = NULL;
	GIOChannel *slot = g_io_channel_new_file (fileName, "r", &error);
	if(error != NULL)
	{
		g_error_free(error);
		return FALSE;
	}
	g_io_channel_read_line (slot, output, NULL, NULL, &error);
	g_io_channel_shutdown(slot, TRUE, NULL);
	g_io_channel_unref(slot);
	if(error != NULL)
	{
		g_error_free(error);
		return FALSE;
	}
	return TRUE;
}

int gpio_digitalRead(int pin)
{
	// Get path to gpio direction file.
	gchar *directionFile = g_strdup_printf("/sys/class/gpio/gpio%d/direction", pin);
	gchar *direction = "";
	gboolean returnCode = getFileContent(directionFile, &direction);
	g_free(directionFile);

	if(returnCode == FALSE)
		return -1;
	// Check file is an input.
	if(g_strcmp0 (direction, "in\n") != 0)
	{
		g_free(direction);
		return -2;
	}
	g_free(direction);

	// Read gpio value.
	gchar *valueFile = g_strdup_printf("/sys/class/gpio/gpio%d/value", pin);
	gchar *value = NULL;
	returnCode = getFileContent(valueFile, &value);
	g_free(valueFile);

	if(returnCode == FALSE)
	{
		g_free(value);
		return -1;
	}
	int pinValue = g_ascii_strtoll(value, NULL, 10);
	g_free(value);
	return pinValue;
}

int gpio_digitalWrite(int pin, int value)
{
	// Get path to gpio direction file.
	gchar *directionFile = g_strdup_printf("/sys/class/gpio/gpio%d/direction", pin);
	gchar *direction = "";
	gboolean returnCode = getFileContent(directionFile, &direction);
	g_free(directionFile);

	if(returnCode == FALSE)
		return -1;
	// Check file is an input.
	if(g_strcmp0 (direction, "out\n") != 0)
	{
		g_free(direction);
		return -2;
	}
	g_free(direction);

	// Write gpio value.
	gchar *valueFile = g_strdup_printf("/sys/class/gpio/gpio%d/value", pin);
	GError *error = NULL;
	GIOChannel *slot = g_io_channel_new_file (valueFile, "w", &error);
	g_free(valueFile);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}
	gchar *valueString = g_strdup_printf("%d\n", (value == 0 ? 0 : 1));
	g_io_channel_write_chars(slot, valueString, -1, NULL, &error);
	g_io_channel_shutdown(slot, TRUE, NULL);
	g_free(valueString);
	g_io_channel_unref(slot);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}
	return 0;
}


int gpio_exportPin(int pin, gchar *direction)
{
	// Export pin value.
	gchar *exportFile = g_strdup_printf("/sys/class/gpio/export");
	GError *error = NULL;
	GIOChannel *slot = g_io_channel_new_file (exportFile, "a", &error);
	g_free(exportFile);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}
	gchar *valueString = g_strdup_printf("%d\n", pin);
	g_io_channel_write_chars(slot, valueString, -1, NULL, &error);
	g_io_channel_shutdown(slot, TRUE, NULL);
	g_free(valueString);
	g_io_channel_unref(slot);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}

	// Set gpio direction.
	gchar *directionFile = g_strdup_printf("/sys/class/gpio/gpio%d/direction", pin);
	slot = g_io_channel_new_file (directionFile, "w", &error);
	g_free(directionFile);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}
	g_io_channel_write_chars(slot, direction, -1, NULL, &error);
	g_io_channel_shutdown(slot, TRUE, NULL);
	g_io_channel_unref(slot);
	if(error != NULL)
	{
		g_error_free(error);
		return -1;
	}
	return 0;
}
