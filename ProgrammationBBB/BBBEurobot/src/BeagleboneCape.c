#include "BBBEurobot/BeagleboneCape.h"

// See header directly for documentation of these constants.
const gchar* SPI_0 = "/dev/spi1.0";
const gchar* SPI_10 = "/dev/spi2.0";
const gchar* SPI_11 = "/dev/spi2.1";
const gchar* I2C_1 = "/dev/i2c-1";
const gchar* I2C_2 = "/dev/i2c-2";
const int ANALOG[7] = {0, 1, 2, 3, 6, 5, 4};
const int DIGITAL[7] = {66, 67, 69, 68, 45, 44, 26};
const int LED[2] = {47, 46};
