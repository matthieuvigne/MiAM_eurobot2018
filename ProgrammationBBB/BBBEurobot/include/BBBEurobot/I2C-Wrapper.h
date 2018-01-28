/// \file I2C-Wrapper.h
/// \brief Wrapper for I2C communication.
///
/// \details This file implements helper functions to make I2C communication both easier and thread safe.
///			 All these functions will write debug messages on the terminal on transfert failure.
///	\warning As several threads may require bus access at the same time, it is important that all I2C access
///			 are done with these functions only.
///	\note	 All functions in this header should be prefixed with i2c_.
#ifndef I2C_WRAPPER
#define I2C_WRAPPER
	#include <glib.h>

	///< Structure representing an I2C port access, with thread safety.
	typedef struct{
		int file;	///< The file descriptor of the port number to use.
		GMutex portMutex; ///< A mutex used internally to guarantee a thread-safe implementation.
	}I2CAdapter;

	/// \brief Open an I2C port.
	///
    /// \param[in] adapter An I2CAdapter structure to fill.
    /// \param[in] portName Name of the I2C port to use (in the file system, i.e. a string "/dev/i2c-x").
	/// \returns TRUE on success, FALSE on failure.
	gboolean i2c_open(I2CAdapter *adapter, gchar *portName);


	/// \brief Write data to an I2C register
    ///
    /// \param[in] adapter The I2CAdapter structure to use: this structure defines the port being used.
    /// \param[in] port File descriptor of the I2C port.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] reg Address of the register to set.
    /// \param[in] data Data to write to the register.
	/// \returns TRUE on success, FALSE on failure.
	gboolean i2c_writeRegister(I2CAdapter *adapter, guint8 address, guint8 reg, guint8 data);


	/// \brief Read an I2C register
    ///
    /// \param[in] adapter The I2CAdapter structure to use: this structure defines the port being used.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] registerAddress Address of the register to read.
    /// \returns   The value of the specified register.
	guint8 i2c_readRegister(I2CAdapter *adapter, guint8 address, guint8 registerAddress);


	/// \brief Read several I2C registers at once.
	/// \note Not all device support reading several registers - check device documentation beforehand.
    /// \note This function performs no memory allocation.
    ///
    /// \param[in] adapter The I2CAdapter structure to use: this structure defines the port being used.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] registerAddress Address of the first register to read.
    /// \param[in] length Number of registers to read.
    /// \param[out] output Char array of all the register read. Memory must have already been alocated.
	gboolean i2c_readRegisters(I2CAdapter *adapter, guint8 address, guint8 registerAddress, int length, guint8 *output);


	/// \brief Close an I2C port.
	///
	/// \param[in] port File descriptor of the I2C port.
	void i2c_close(int port);
#endif
