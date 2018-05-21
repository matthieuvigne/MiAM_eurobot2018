/// \file MPC23017Driver.h
/// \brief Driver for MPC23017 IO expander chip.
///
/// \details This chip controls 16 digital I/O from a I2C signal. It is used in Adafruit LCD shield.
///			 The functions is this file are named to emulate Arduino IO operations.
///			 The interrupt functionnality of the MPC are not implemented.
///	\note	 All functions in this header should be prefixed with mpc_.
#ifndef MPC_DRIVER_H
	#define MPC_DRIVER_H
	#include <glib.h>
	#include "BBBEurobot/I2C-Wrapper.h"

	/// MPC structure.
	typedef struct {
		I2CAdapter *adapter; ///< Pointer to the I2C port being used.
		int address;	///< I2C slave address.
	}MPC;

	typedef enum{
		MPC_INPUT = 0,
		MPC_INPUT_PULLUP = 1,
		MPC_OUTPUT = 2
	}MPCPinState;

	/// \brief Initialize MPC structure.
    ///
    /// \details This function tests the communication with the MPC, and, if successful, inits the structure.
    ///
    /// \param[inout] mpc The MPC structure, to be used whenever communication with the chip.
    /// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C port (as returned by the i2c_open function,
    ///                    see I2C-Wrapper.h).
    /// \param[in] slaveAddress Address of the I2C slave.
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean mpc_init(MPC *mpc, I2CAdapter *adapter, int slaveAddress);


	/// \brief Calls mpc_init with the default sensor I2C addresses.
    ///
	static inline gboolean mpc_initDefault(MPC *mpc, I2CAdapter *adapter){return mpc_init(mpc, adapter, 0x20);}


	/// \brief Set specified pin mode.
	///
	/// \details Set whether the pin is an input or an output, and, if input, whether to use the pullup resistor.
    /// \param[in] mpc The MPC chip to talk to.
    /// \param[in] pin Pin number, between 0 and 15. Values outside of this range do nothing.
    /// \param[in] state State of the pin (MPCPinState enum).
	void mpc_pinMode(MPC mpc, int pin, MPCPinState state);


	/// \brief Read state of specified pin.
	///
    /// \param[in] mpc The MPC chip to talk to.
    /// \param[in] pin Pin number, between 0 and 15. Values outside of this range return 0.
    /// \return 0 on low, 1 on high.
	int mpc_digitalRead(MPC mpc, int pin);


	/// \brief Set state of specified pin.
	///
    /// \param[in] mpc The MPC chip to talk to.
    /// \param[in] pin Pin number, between 0 and 15. Values outside of this range return 0.
    /// \param[in] value Value to write: 0 for low, else high.
	void mpc_digitalWrite(MPC mpc, int pin, int value);


	/// \brief Write to all output pins at once.
	/// \details This enables for much faster communication.
	///
    /// \param[in] mpc The MPC chip to talk to.
    /// \param[in] value Value to give to each pin. Note that writing to an input pin has no effect.
	void mpc_writeAll(MPC mpc, guint16 value);

	/// \brief Read all pins at once.
	/// \details This enables for much faster communication.
	///
    /// \param[in] mpc The MPC chip to talk to.
    /// \returns The value of all the pins of the device
	guint16 mpc_readAll(MPC mpc);
#endif
