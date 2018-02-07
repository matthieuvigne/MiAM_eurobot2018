/// \file BBBEurobot.h
/// \brief Master header file.
///
/// \details This header simply includes all the other headers from the library, as well as the glib header,
///          in order to ease library loading.
#ifndef BBB_EUROBOT_DRIVER
#define BBB_EUROBOT_DRIVER

	#include <glib.h>

	#include <BBBEurobot/ADNS9800Driver.h>
	#include <BBBEurobot/Ax12Driver.h>
	#include <BBBEurobot/BBBGpio.h>
	#include <BBBEurobot/BeagleboneCape.h>
	#include <BBBEurobot/I2C-Wrapper.h>
	#include <BBBEurobot/IMUDriver.h>
	#include <BBBEurobot/LCDDriver.h>
	#include <BBBEurobot/KalmanFilter.h>
	#include <BBBEurobot/L6470Driver.h>
	#include <BBBEurobot/PCA9635Driver.h>

#endif
