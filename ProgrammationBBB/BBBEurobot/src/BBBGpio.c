#include "BBBEurobot/BBBGpio.h"

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


int gpio_analogRead(int pin)
{
	if(pin < 0 || pin > 6)
		return -2;

	gchar *fileName = g_strdup_printf("/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", pin);

	gchar *portValue = "";
	gboolean returnCode = getFileContent(fileName, &portValue);
	g_free(fileName);
	if(returnCode == FALSE)
		return -1;

	int value = g_ascii_strtoll(portValue, NULL, 10);
	g_free(portValue);
	return value;
}
