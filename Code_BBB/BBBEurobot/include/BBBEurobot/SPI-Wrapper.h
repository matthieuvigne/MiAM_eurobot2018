/// \file SPI-Wrapper.h
/// \brief Wrapper for SPI communication.
///
/// \details This file implements SPI file opening and closing. Communication itself isn't handled yet,
///			 but it should probably be there instead of in each driver (L6470 and ADNS9800 implement it separately for
///			 now).
///	\note	 All functions in this header should be prefixed with spi_.
#ifndef SPI_WRAPPER
#define SPI_WRAPPER
	#include <glib.h>

	/// \brief Open SPI port for communication.
	///
	/// \param[in] portName string to port file, e.g. /dev/spi1.0
	/// \param[in] frequnecy clock frequency, in Hz.
	/// \return port number, or -1 if opening failed.
	int spi_open(const gchar *portName, const int frequency);

	/// \brief Close an SPI port.
	///
	/// \param[in] port File descriptor of the SPI port.
	void spi_close(const int port);
#endif
