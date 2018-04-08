#include "BBBEurobot/LCDDriver.h"
#include <stdio.h>

// Define registers and command list.

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// Constant definition
int rs_pin = 15;
int rw_pin = 14;
int enable_pin = 13;
int data_pins[4] = {12, 11, 10, 9};

void lcd_pulseEnable(LCD lcd)
{
	mpc_digitalWrite(lcd, enable_pin, 1);
	g_usleep(1);    // enable pulse must be >450ns
	mpc_digitalWrite(lcd, enable_pin, 0);
	g_usleep(40);   // commands need > 37us to settle
}

// Send data over 4bits interface, with specific rs pin value.
void lcd_sendData(LCD lcd, guint8 data, guint8 rsValue)
{
	guint16 portValue = mpc_readAll(lcd);

	// Set RS, RW and enable to 0
	portValue &= ~(1 << rs_pin);
	portValue &= ~(1 << rw_pin);
	portValue &= ~(1 << enable_pin);

	// Set RS pin to right value
	portValue |= (rsValue << rs_pin);

	// Set data pin : first 4 bits.
	for (int x = 0; x < 4; x++)
	{
		portValue &= ~(1 << data_pins[x]);
		portValue |= (((data >> (4 + x)) & 0x01) << data_pins[x]);
	}
	mpc_writeAll(lcd, portValue);
	lcd_pulseEnable(lcd);

	// Second 4 bits.
	for (int x = 0; x < 4; x++)
	{
		portValue &= ~(1 << data_pins[x]);
		portValue |= (((data >> (x)) & 0x01) << data_pins[x]);
	}
	mpc_writeAll(lcd, portValue);
	lcd_pulseEnable(lcd);
}


// Send a command to the LCD screen, in 4 bits mode
void lcd_sendCommand(LCD lcd, unsigned char value)
{
	lcd_sendData(lcd, value, 0);
}

// Send a character to the LCD screen, in 4 bits mode
void lcd_sendChar(LCD lcd, unsigned char value)
{
	lcd_sendData(lcd, value, 1);
}

gboolean lcd_init(LCD *lcd, I2CAdapter *adapter, int address)
{
	// Init MPC chip.
	if(mpc_init(lcd, adapter, address) == FALSE)
		return FALSE;
	// Configure MPC I/O.
    mpc_pinMode(*lcd, 8, MPC_OUTPUT);
    mpc_pinMode(*lcd, 7, MPC_OUTPUT);
    mpc_pinMode(*lcd, 6, MPC_OUTPUT);

    mpc_pinMode(*lcd, rs_pin, MPC_OUTPUT);
    mpc_pinMode(*lcd, rw_pin, MPC_OUTPUT);
    mpc_pinMode(*lcd, enable_pin, MPC_OUTPUT);

	for(int x=0; x < 4; x++)
		mpc_pinMode(*lcd, data_pins[x], MPC_OUTPUT);

	// Turn off all legs, set all the other lines to low.
	guint16 pinValue = 0;
	pinValue = pinValue | (0b111 << 6);
	mpc_writeAll(*lcd, pinValue);

	// Reset LCD into 4 bit mode : see HD44780 datasheet, figure 24 pg. 46.
	lcd_sendCommand(*lcd, 0x33);
	lcd_sendCommand(*lcd, 0x32);

	// Set display mode.
	lcd_sendCommand(*lcd, LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
	// Turn on display, disable cursor and blink.
	lcd_sendCommand(*lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
	// Set the entry mode
	lcd_sendCommand(*lcd, LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

	// Clear LCD display.
	lcd_clear(*lcd);
    return TRUE;
}

void lcd_clear(LCD lcd)
{
	lcd_sendCommand(lcd, LCD_CLEARDISPLAY);
}

void lcd_setText(LCD lcd, gchar *text, int line)
{
	// If line is not zero, set offset to write to line 1.
	if(line != 0)
		line  = 0x40;
	// Go to beginning of line.
	lcd_sendCommand(lcd, LCD_SETDDRAMADDR | line);
	int stringLength = 16;
	for(int x = 0; x < 16; x++)
	{
		if(text[x] == '\0')
		{
			stringLength = x;
			break;
		}
		lcd_sendChar(lcd, text[x]);
	}
	// Pad string with spaces if needed.
	for(int x = stringLength; x < 16; x++)
		lcd_sendChar(lcd, ' ');
}

void lcd_setChar(LCD lcd, gchar text, int line, int column)
{
	int offset = 0;
	// If line is not zero, set offset to write to line 1.
	if(line != 0)
		offset  = 0x40;
	if(column < 0)
		column = 0;
	if(column > 15)
		column = 15;
	offset += column;
	// Go to beginning of line.
	lcd_sendCommand(lcd, LCD_SETDDRAMADDR | offset);
	lcd_sendChar(lcd, text);
}


void lcd_setTextCentered(LCD lcd, gchar *text, int line)
{
	// Find length of string.
	int stringLength = 0;
	for(int x = 0; x < 16; x++)
	{
		if(text[x] == '\0')
		{
			stringLength = x;
			break;
		}
	}
	// Compute number of space needed - on odd number the string will be left-aligned.
	int numberOfSpaces = (16 - stringLength) / 2;
	gchar *centeredText = g_strdup_printf("%*c%s", numberOfSpaces, ' ', text);
	lcd_setText(lcd, centeredText, line);
	g_free(centeredText);
}


void lcd_setBacklight(LCD lcd, gboolean red, gboolean green, gboolean blue)
{
	mpc_digitalWrite(lcd, 6, ~(red) & 0x1);
	mpc_digitalWrite(lcd, 7, ~(green) & 0x1);
	mpc_digitalWrite(lcd, 8, ~(blue) & 0x1);
}

gboolean lcd_isButtonPressed(LCD lcd, LCDButton button)
{
	if(mpc_digitalRead(lcd, button) == 0)
		return TRUE;
	return FALSE;
}

