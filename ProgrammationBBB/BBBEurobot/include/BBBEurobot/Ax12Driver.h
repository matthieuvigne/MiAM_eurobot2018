/// \file Ax12Driver.h
/// \brief Driver for the Ax12 servomotors.
///
/// \details This file contains all the functions related to communication
///			 with the Ax12 servomotors on a serial port. Because the Ax12
///			 use a half-duplex UART, an additional pin is required to switch
///			 the direction of the communication. Also, because of this, a delay
///			 should be observed between two Ax12 request, in case the first request
/// 		 triggers a response from the servo.
///	\note	 All functions in this header should be prefixed with ax12_.
///	\note	 So far, only write commands have been tested.
#ifndef AX12DRIVER_H
	#define AX12DRIVER_H
	#include <glib.h>
    
	/// Ax12 structure.
    typedef struct {
		int port;	///< Ax12 file descriptor.
		gchar *dir;	///< File name of the direction port GPIO.
	}Ax12;
	
	
    /// \brief Initialize Beaglebone for communication with the Ax12.
    ///
    /// \details This functions configures the choosen serial port and direction
    ///			 pin to talk to the servos. It sets an Ax12 struct that
    ///			 should then be used with all other Ax12 functions.
    ///
    /// \param[out] ax12 An ax12 struct that should be used whenever talking to the servos on that serial port.
    /// \param[in] port Name of the serial port to use (in the file system, i.e. a string "/dev/ttyOx")
    /// \param[in] speed Baud rate of the serial communication, as defined is termios.h.
    /// \param[in] dirPin Number of the GPIO used for choosing the direction. This number is fixed in hardware
    ///			   by the wirering of the Beaglebone shield.
    /// \returns   TRUE on success, FALSE otherwise.
    gboolean ax12_init(Ax12 *ax12, gchar *portName, int speed, int dirPin);
    
    
    /// \brief Moves desired servo to specified position.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \param[in] position	The desired servo position (from 0 to 1023).
    void ax12_setPosition(Ax12 ax12, int motorID, int position);
    
    
    /// \brief Set servo speed for all following motion.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \param[in] speed	The desired servo speed (from 0 to 1023 in joint mode, 0 to 2047 in continuous rotation mode).
    void ax12_setSpeed(Ax12 ax12, int motorID, int speed);	
    
    
    /// \brief Set servo angular limit. 
    /// \details Setting both to 0 enables continuous rotation mode.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \param[in] lowLimit	Lower servo limit (from 0 to 1023, should be less than highLimit).
    /// \param[in] highLimit	Higher servo limit (from 0 to 1023, should be greater than lowLimit).
    /// \note This function writes to the servo EEPROM.
    void ax12_eeprom_setAngleLimit(Ax12 ax12, int motorID, int lowLimit, int highLimit);
    
    
    /// \brief Test if a servo is currently moving.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \return TRUE if the servo is still moving.
    /// \note This function has not yet been tested.
    gboolean ax12_isMoving(Ax12 ax12, int motorID);
    
    /// \brief Turn on or off the servo led.
    /// \details This can be used to visually test connexion to servo.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \param[in] on If TRUE, turn on the LED. Otherwise, turn it off.
    void ax12_servoLed(Ax12 ax12, int motorID, gboolean on);
    
     /// \brief Turn on or off servo torque.
    ///
    /// \param[in] ax12	A valid Ax12 structure.
    /// \param[in] motorID The ID of the motor to talk to.
    /// \param[in] on If TRUE, turn on the torque. Otherwise, turn it off.
    void ax12_torqueOn(Ax12 ax12, int motorID, gboolean on);
    
    
    /// \brief Reads a single register from an Ax12.
    /// \note  This function has not been tested yet.
    int ax12_getRegister(Ax12 ax12, int motorID, int reg);
    
    /// \brief List of all Ax12 EEPROM registers
    /// \note Changes in the EEPROM are permanent, i.e. they remain active after servo has been powered off.
    /// 	  For this reason, one should always be careful when changing on of these values.
    typedef enum AX12_EEPROM_REGISTER{
		AX12_EEPROM_MODEL_NUMBER_L = 0x01,
		AX12_EEPROM_MODEL_NUMBER_H = 0x02,
		AX12_EEPROM_ID = 0x03,
		AX12_EEPROM_BAUD_RATE = 0x04,
		AX12_EEPROM_RETURN_DELAY = 0x05,
		AX12_EEPROM_CW_ANGLE_LIM_L = 0x06,
		AX12_EEPROM_CW_ANGLE_LIM_H = 0x07,
		AX12_EEPROM_CCW_ANGLE_LIM_L = 0x08,
		AX12_EEPROM_CCW_ANGLE_LIM_H = 0x09,
		AX12_EEPROM_TEMPERATURE_LIM = 0x0B,
		AX12_EEPROM_VOLTAGE_LIM_L = 0x0C,
		AX12_EEPROM_VOLTAGE_LIM_H = 0x0D,
		AX12_EEPROM_MAX_TORQUE_L = 0x0E,
		AX12_EEPROM_MAX_TORQUE_H = 0x0F,
		AX12_EEPROM_STATUS_RETURN_LEVEL = 0x10,
		AX12_EEPROM_STATUS_ALARM_LED = 0x11,
		AX12_EEPROM_STATUS_ALARM_SHUTDOWN = 0x12,
		AX12_EEPROM_STATUS_DOWN_CALIBRATION_L = 0x14,
		AX12_EEPROM_STATUS_DOWN_CALIBRATION_H = 0x15,
		AX12_EEPROM_STATUS_UP_CALIBRATION_L = 0x16,
		AX12_EEPROM_STATUS_UP_CALIBRATION_H = 0x17,
	}AX12_EEPROM_REGISTER;
	
	/// \brief List of all Ax12 RAM registers.
	///
	typedef enum AX12_RAM_REGISTER{
		AX12_RAM_TORQUE_ENABLE = 0x18,
		AX12_RAM_LED = 0x19,
		AX12_RAM_CW_COMPLIANCE = 0x1A,
		AX12_RAM_CCW_COMPLIANCE = 0x1B,
		AX12_RAM_CW_COMPLIANCE_SLOPE = 0x1C,
		AX12_RAM_CCW_COMPLIANCE_SLOPE = 0x1D,
		AX12_RAM_GOAL_POSITION_L = 0x1E,
		AX12_RAM_GOAL_POSITION_H = 0x1F,
		AX12_RAM_MOVING_SPEED_L = 0x20,
		AX12_RAM_MOVING_SPEED_H = 0x21,
		AX12_RAM_TORQUE_LIMIT_L = 0x22,
		AX12_RAM_TORAUE_LILMIT_H = 0x23,
		AX12_RAM_PRESENT_POSITION_L = 0x24,
		AX12_RAM_PRESENT_POSITION_H = 0x25,
		AX12_RAM_PRESENT_SPEED_L = 0x26,
		AX12_RAM_PRESENT_SPEED_H = 0x27,
		AX12_RAM_PRESENT_LOAD_L = 0x28,
		AX12_RAM_PRESENT_LOAD_H = 0x29,
		AX12_RAM_PRESENT_VOLTAGE = 0x2A,
		AX12_RAM_PRESENT_TEMPERATURE = 0x2B,
		AX12_RAM_REGISTERED_INSTRUCTION = 0x2C,
		AX12_RAM_MOVING = 0x2E,
		AX12_RAM_LOCK = 0x2F,
		AX12_RAM_PUNCH_L = 0x30,
		AX12_RAM_PUNCH_H = 0x31,
	}AX12_RAM_REGISTER;
#endif 
