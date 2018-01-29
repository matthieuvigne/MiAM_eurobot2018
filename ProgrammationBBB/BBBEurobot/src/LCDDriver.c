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

int displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
int displaycontrol = 0;
int displaymode = 0;

void lcd_pulseEnable(LCD lcd)
{
  mpc_digitalWrite(lcd, enable_pin, 1);
  g_usleep(2);    // enable pulse must be >450ns
  mpc_digitalWrite(lcd, enable_pin, 0);
  g_usleep(40);   // commands need > 37us to settle
}

// Send only 4 bits of data : used for initialisation only.
void lcd_send4bits(LCD lcd, guint8 data)
{
	guint16 portValue = mpc_readAll(lcd);

	// Set RS, RW and enable to 0
	portValue &= ~(1 << rs_pin);
	portValue &= ~(1 << rw_pin);
	portValue &= ~(1 << enable_pin);

	// Set data pin : first 4 bits.
	for (int x = 0; x < 4; x++)
	{
		portValue &= ~(1 << data_pins[x]);
		portValue |= (((data >> x) & 0x01) << data_pins[x]);
	}
	mpc_writeAll(lcd, portValue);
	lcd_pulseEnable(lcd);
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

	// Put the LCD into 4 bit mode : see HD44780 datasheet, figure 24 pg. 46.
	// This is the only time we do "half transactions" of only 4 bits.


	lcd_send4bits(*lcd, 0x03);
	g_usleep(4500);
	lcd_send4bits(*lcd, 0x03);
	g_usleep(4500);
	lcd_send4bits(*lcd, 0x03);
	g_usleep(150);
	lcd_send4bits(*lcd, 0x02);
	g_usleep(150);
	lcd_sendCommand(*lcd, LCD_FUNCTIONSET | displayfunction);
	g_usleep(20000);

	displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

	lcd_sendCommand(*lcd, LCD_DISPLAYCONTROL | displaycontrol);

	// Clear LCD display.
	lcd_clear(*lcd);

    int displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// Set the entry mode
	lcd_sendCommand(*lcd, LCD_ENTRYMODESET | displaymode);

    lcd_sendCommand(*lcd, LCD_RETURNHOME);  // set cursor position to zero
	//~ g_usleep(2000);
	lcd_sendChar(*lcd, 'A');
	lcd_sendChar(*lcd, 'B');
	lcd_sendChar(*lcd, 'C');
    lcd_sendCommand(*lcd, LCD_SETDDRAMADDR | 0x00);  // set cursor position to zero
    lcd_sendCommand(*lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);  // set cursor position to zero

	lcd_sendChar(*lcd, 'A');
	lcd_sendChar(*lcd, 'B');
    return TRUE;
}

void lcd_clear(LCD lcd)
{
	lcd_sendCommand(lcd, LCD_CLEARDISPLAY);
}

void lcd_setText(LCD lcd, gchar *text, int line)
{
	if(line < 0)
		line = 0;
	if(line > 1)
		line  = 1;
	// Go to beginning of line.
	lcd_sendCommand(lcd, LCD_SETDDRAMADDR | 0x00);
	for(int x = 0; x < 16; x++)
	{
		if(text[x] == '\0')
			break;
		lcd_sendChar(lcd, text[x]);
	}
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

