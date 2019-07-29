#ifndef THREAD_DS18B20
#error "Include only thread_ds18b20.hpp directly"
#endif

#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#ifdef  MODM_LOG_LEVEL
#undef	MODM_LOG_LEVEL
#endif
#define	MODM_LOG_LEVEL modm::log::DEBUG

template < typename I2cMaster >
Ds18b20Thread<I2cMaster>::Ds18b20Thread() :
	ds18b20(data),
	rom{},
	start_measurement(false),
	new_data(false)
{

}

template < typename OneWireMaster >
bool
Ds18b20Thread<OneWireMaster>::update()
{
	PT_BEGIN();

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "Ping the Thermometer DS18B20" << modm::endl;

	// Ping the bus for any devices
	while (true)
	{
		// we wait until the task started
		if (OneWireMaster::touchReset()) {
			break;
		}
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "At least one 1-Wire devices exists. (unreliable information)" << modm::endl;


	OneWireMaster::resetSearch();

	// Scan bus
	while (OneWireMaster::searchNext(rom)) {
		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << "1-Wire found: " << modm::hex;
		for (uint8_t ii = 0; ii < 8; ++ii) {
			MODM_LOG_DEBUG << rom[ii];
			MODM_LOG_DEBUG << modm::ascii << ":" << modm::hex;
		}
		MODM_LOG_DEBUG << modm::ascii << " ";

		if (rom[0] == 0x28) {
			MODM_LOG_DEBUG << "DS18B20";
		} else if (rom[0] == 0x10) {
			MODM_LOG_DEBUG << "DS18S20";
		} else {
			MODM_LOG_DEBUG << "unknown family code " << modm::hex << rom[0] << modm::ascii;
		}

		MODM_LOG_DEBUG << modm::endl;
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}
	MODM_LOG_DEBUG << MODM_FILE_INFO;
	MODM_LOG_DEBUG << "1-Wire search finished!" << modm::endl;

	// search for connected DS18S20 devices
	// MODM_LOG_DEBUG << MODM_FILE_INFO;
	// MODM_LOG_DEBUG << "1-Wire Looking for DS18B20 (0x10):" << modm::endl;
	// OneWireMaster::resetSearch(0x10);

	// while (OneWireMaster::searchNext(rom)) {
	// 	MODM_LOG_DEBUG << MODM_FILE_INFO;
	// 	MODM_LOG_DEBUG << "1-Wire found: " << modm::hex;
	// 	for (uint8_t ii = 0; ii < 8; ++ii) {
	// 		MODM_LOG_DEBUG << rom[ii];
	// 	}
	// 	MODM_LOG_DEBUG << modm::ascii << modm::endl;
	// 	this->timeout.restart(100);
	// 	PT_WAIT_UNTIL(this->timeout.isExpired());
	// }
	// MODM_LOG_DEBUG << MODM_FILE_INFO;
	// MODM_LOG_DEBUG << "1-Wire search finished!" << modm::endl;

	ds18b20.setIdentifier(rom);

	// ping the device until it responds
	if (ds18b20.isAvailable()) {
		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << "1-Wire device responding" << modm::endl;
	} else {
		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << "1-Wire device NOT responding" << modm::endl;
	}

	// Select Resolution
	// ds18b20.configure(modm::ds18b20::Resolution::Bit9, 0, 0);

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		ds18b20.startConversion();

		PT_WAIT_UNTIL(ds18b20.isConversionDone());

		// Returns when new data was read from the sensor
		// PT_CALL(thermometer.readTemperature());
		new_data = true;

		{
			if (ds18b20.readout()) {
				int32_t temp;
				data.getTemperature(temp);
				MODM_LOG_DEBUG << MODM_FILE_INFO;
				MODM_LOG_DEBUG.printf("DS18B20: Calibrated temperature in 0.01 degree Celsius is : %ld\n", temp);
			}
		}

		start_measurement = false;
	}

	PT_END();
}

template < typename OneWireMaster >
bool
Ds18b20Thread<OneWireMaster>::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}
