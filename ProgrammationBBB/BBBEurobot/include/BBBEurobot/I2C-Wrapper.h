/// \file I2C-Wrapper.h
/// \brief Wrapper for I2C communication.
///
/// \details This file implements helper functions to make I2C communication both easier and thread safe.
///	\warning As several threads may require bus access at the same time, it is important that all I2C access
///			 are done with and only with these functions.
///	\note	 All functions in this header should be prefixed with i2c_.
#ifndef I2C_WRAPPER
#define I2C_WRAPPER	
	/// \brief Open an I2C port.
	///
    /// \param[in] portName Name of the I2C port to use (in the file system, i.e. a string "/dev/i2c-x").
	/// returns -1 on failure, otherwise a valid file descriptor.
	int i2c_open(char *portName);	
	
	
	/// \brief Write data to an I2C register
    ///
    /// \param[in] port File descriptor of the I2C port.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] reg Address of the register to set.
    /// \param[in] data Data to write to the register.
    /// \returns   0 on success, -1 otherwise.
	int i2c_writeRegister(int port, int address, unsigned char reg, unsigned char data);
	
	
	/// \brief Read an I2C register
    ///
    /// \param[in] port File descriptor of the I2C port.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] reg Address of the register to read.
    /// \returns   The value of the specified register.
	unsigned char i2c_readRegister(int port, int address, unsigned char reg);
	
	
	/// \brief Read several I2C registers at once.
	/// \note Not all device support reading several registers - check device documentation beforehand.
    /// \note This function performs no memory allocation.
    ///
    /// \param[in] port File descriptor of the I2C port.
    /// \param[in] address Address of the device to talk to.
    /// \param[in] reg Address of the first register to read.
    /// \param[in] length Number of registers to read.
    /// \param[out] output Char array of all the register read. Memory must have already been alocated.
	void i2c_readRegisters(int port, int address, unsigned char reg, int length, unsigned char *output);
	
	
	/// \brief Close an I2C port.
	///
	/// \param[in] port File descriptor of the I2C port.
	void i2c_close(int port);
#endif
