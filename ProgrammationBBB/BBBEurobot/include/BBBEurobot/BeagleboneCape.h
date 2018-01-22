/// \file BeagleboneCape.h
/// \brief Code description of the PCB BeagleboneCape.
///
/// \details This file defines constants to easily access the exposed ports of the Beaglebone through the
///			 BeagleboneCape. These constants map to output ports defined from the document BeagleboneCapeDoc below :
/// \image html BeagleboneCapeDoc.svg
#ifndef BBB_CAPE
#define BBB_CAPE

	#include <glib.h>
	
	extern const gchar* SPI_0;	///<  First SPI port.
	extern const gchar* SPI_10; ///<  Second SPI port.
	extern const gchar* SPI_11; ///<  Third SPI port.
	extern const gchar* I2C_1; ///<  First I2C port.
	extern const gchar* I2C_2; ///<  Second I2C port.
	extern const int ANALOG[7]; ///<  Analong input.
	extern const int DIGITAL[7]; ///<  Digital GPIO.
	extern const int LED[2]; ///<  LED output.
	
#endif
