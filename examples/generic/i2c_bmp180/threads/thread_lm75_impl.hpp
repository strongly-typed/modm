#ifndef THREAD_LM75
#error "Include only thread_lm75.hpp directly"
#endif

#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#ifdef  MODM_LOG_LEVEL
#undef	MODM_LOG_LEVEL
#endif
#define	MODM_LOG_LEVEL modm::log::DEBUG

template < typename I2cMaster >
Lm75Thread<I2cMaster>::Lm75Thread() :
	thermometer(data, 0x48),
	start_measurement(false),
	new_data(false)
{

}

template < typename I2cMaster >
bool
Lm75Thread<I2cMaster>::update()
{
	PT_BEGIN();

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Ping the Thermometer LM75" << modm::endl;

	// ping the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(thermometer.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Thermometer LM75 responded" << modm::endl;

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		// Returns when new data was read from the sensor
		PT_CALL(thermometer.readTemperature());
		new_data = true;

		{
			auto temp = data.getTemperature();

			MODM_LOG_DEBUG << MODM_FILE_INFO;
			MODM_LOG_DEBUG.printf("LM75: Calibrated temperature in degree Celsius is : %f\n", double(temp)  );
		}

		start_measurement = false;
	}

	PT_END();
}

template < typename I2cMaster >
bool
Lm75Thread<I2cMaster>::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}
