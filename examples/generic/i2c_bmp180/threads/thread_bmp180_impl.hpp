#ifndef THREAD_BMP180
#error "Include only thread_bmp180.hpp directly"
#endif

#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#ifdef  MODM_LOG_LEVEL
#undef	MODM_LOG_LEVEL
#endif
#define	MODM_LOG_LEVEL modm::log::DEBUG

template < typename I2cMaster >
Bmp180Thread<I2cMaster>::Bmp180Thread() :
	barometer(data, 0x77),
	start_measurement(false),
	new_data(false)
{

}

template < typename I2cMaster >
bool
Bmp180Thread<I2cMaster>::update()
{
	PT_BEGIN();

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Ping the Barometer BMP180" << modm::endl;

	// ping the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometer.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Barometer BMP180 responded" << modm::endl;

	// Configure the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometer.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "BMP180 configured" << modm::endl;

	// #if MODM_LOG_LEVEL == modm::log::DEBUG
	// 	static modm::bmp085::Calibration &cal = data.getCalibration();

	// 	MODM_LOG_DEBUG << "Calibration data is: \n";
	// 	MODM_LOG_DEBUG.printf(" ac1 %d\n", cal.ac1);
	// 	MODM_LOG_DEBUG.printf(" ac2 %d\n", cal.ac2);
	// 	MODM_LOG_DEBUG.printf(" ac3 %d\n", cal.ac3);
	// 	MODM_LOG_DEBUG.printf(" ac4 %d\n", cal.ac4);
	// 	MODM_LOG_DEBUG.printf(" ac5 %d\n", cal.ac5);
	// 	MODM_LOG_DEBUG.printf(" ac6 %d\n", cal.ac6);
	// 	MODM_LOG_DEBUG.printf(" b1 %d\n", cal.b1);
	// 	MODM_LOG_DEBUG.printf(" b2 %d\n", cal.b2);
	// 	MODM_LOG_DEBUG.printf(" mb %d\n", cal.mb);
	// 	MODM_LOG_DEBUG.printf(" mc %d\n", cal.mc);
	// 	MODM_LOG_DEBUG.printf(" md %d\n", cal.md);
	// #endif

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		// Returns when new data was read from the sensor
		PT_CALL(barometer.readout());
		new_data = true;

		{
			auto temp = data.getTemperature();
			auto press = data.getPressure();

			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("BMP180: Calibrated temperature in 0.1 degree Celsius is : %d\n", temp  );
			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("BMP180: Calibrated pressure in Pa is                    : %" PRId32 "\n", press );
		}

		start_measurement = false;
	}

	PT_END();
}

template < typename I2cMaster >
bool
Bmp180Thread<I2cMaster>::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}
