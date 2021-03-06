/// \file LCDDriver.h
/// \brief Driver for Adafruit RBG LCD shield.
///
/// \details This shield is made of an MCP23017 I2C IO expander, wired to an HD44780-compatible LCD screen.
///	\note	 All functions in this header should be prefixed with lcd_.
#ifndef LCD_DRIVER_H
	#define LCD_DRIVER_H
	#include <glib.h>
	#include "BBBEurobot/MPC23017Driver.h"

	///< LCD structure MPC + mutex.
	typedef struct
	{
		MPC mpc;
		GMutex mutex;
	}LCD;

	///< Mapping of board buttons.
	typedef enum
	{
	  LCD_BUTTON_SELECT	= 0,
	  LCD_BUTTON_RIGHT 	= 1,
	  LCD_BUTTON_DOWN  	= 2,
	  LCD_BUTTON_UP    	= 3,
	  LCD_BUTTON_LEFT   = 4
	}LCDButton;

	/// \brief Initialize LCD structure.
    ///
    /// \details This function tests the communication with the LCD, and, if successful, inits the structure.
    ///
    /// \param[out] lcd The LCD structure, to be used whenever communication with the IMU.
    /// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C port (as returned by the i2c_open function,
    ///                    see I2C-Wrapper.h).
    /// \param[in] slaveAddress Address of the I2C slave.
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean lcd_init(LCD *lcd, I2CAdapter *adapter, int slaveAddress);

	/// \brief Call lcd_init with the default sensor I2C addresses.
	static inline gboolean lcd_initDefault(LCD *lcd, I2CAdapter *adapter){return lcd_init(lcd, adapter, 0x20);}

	/// \brief Clear LCD screen.
	///
	/// \param[in] lcd A valid lcd structure.
	void lcd_clear(LCD lcd);

	/// \brief Set the text of a given LCD line, left-aligned.
	/// \note  Writing a full line to the LCD currently takes about 70ms.
	///
	/// \param[in] lcd A valid lcd structure.
	/// \param[in] text Text to display. Only the first 16 characters will fit the screen.
	///                 The string should be null-terminated, otherwise random garbage will be displayed.
	///					The text is left-aligned and padded by spaces to clear the display line.
	/// \param[in] line Line number (0 or 1).
	void lcd_setText(LCD lcd, gchar *text, int line);

	/// \brief Set a single character of the LCD screen.
	///
	/// \param[in] lcd A valid lcd structure.
	/// \param[in] text char to display
	/// \param[in] line Line number (0 or 1).
	/// \param[in] column Cursor position in line (0 to 15).
	void lcd_setChar(LCD lcd, gchar text, int line, int column);

	/// \brief Set the text of a given LCD line, centering it.
	/// \note  Writing a full line to the LCD currently takes about 70ms.
	///
	/// \param[in] lcd A valid lcd structure.
	/// \param[in] text Text to display. Only the first 16 characters will fit the screen.
	///                 The string should be null-terminated, otherwise it will not be centered correctly,
	///					and garbage will be displayed.
	/// \param[in] line Line number (0 or 1).
	void lcd_setTextCentered(LCD lcd, gchar *text, int line);

	/// \brief Set LCD backlight
	///
	/// \param[in] lcd A valid lcd structure.
    /// \param[in] red Whether to turn on the red led.
    /// \param[in] green Whether to turn on the green led.
    /// \param[in] blue Whether to turn on the blue led.
	void lcd_setBacklight(LCD lcd, gboolean red, gboolean green, gboolean blue);

	/// \brief Get button status.
	///
	/// \param[in] lcd A valid lcd structure.
    /// \param[in] button Member of LCDButton enum specifying the button to read.
    /// \returns   TRUE is button is pressed, False otherwise
	gboolean lcd_isButtonPressed(LCD lcd, LCDButton button);
#endif
