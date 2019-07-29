/*
 * Copyright (c) 2019, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger/logger.hpp>

#ifndef MODM_DS18B20_DATA_HPP
#	error "Don't include this file directly, use 'ds18b20_data.hpp' instead!"
#endif

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

namespace modm {

namespace ds18b20data {

void
Data::updateConfig(uint8_t scratchpad[8])
{
	uint8_t config = (scratchpad[4] >> 5) & 0x03; // byte 4 configuration register

	// Calculate conversion factor depending on the sensors resolution
	//  9 bit = 0.5000 °C steps
	// 10 bit = 0.2500 °C steps
	// 11 bit = 0.1250 °C steps
	// 12 bit = 0.0625 °C steps
	switch (config) {
		case 0b11: factor =  625; break;
		case 0b10: factor = 1250; break;
		case 0b01: factor = 2500; break;
		case 0b00: factor = 5000; break;
	}
	MODM_LOG_DEBUG.printf("factor = %d\n", factor);
}

void
Data::updateTemperature(uint8_t scratchpad[8])
{
	// int16_t *rData = reinterpret_cast<int16_t*>(scratchpad);
	// int16_t temp = modm::fromLittleEndian(*rData);
	int16_t temp = scratchpad[0] | (scratchpad[1] << 8);

	// Convert to centigrad
	convertedTemperature = ((factor * temp) ) / 100;
}

} // ds18b20data namespace

} // modm namespace
