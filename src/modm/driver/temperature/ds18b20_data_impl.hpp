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
Data::updateTemperature(uint8_t scratchpad[8])
{
	int16_t temp = scratchpad[0] | (scratchpad[1] << 8);
	// int16_t *rData = reinterpret_cast<int16_t*>(scratchpad);
	// int16_t temp = modm::fromLittleEndian(*rData);

	// Depending on resolution, lower bits are undefined, datasheet p6
	// Just clearing them for reproducible results
	uint8_t config = (scratchpad[4] >> 5) & 0x03; // byte 4 configuration register
	switch (config) {
		case 0b11: break; // Nothing, all bits valid
		case 0b10: temp &= 0xfffe; break; // clear bit 0
		case 0b01: temp &= 0xfffc; break; // clear bit 1 and 0
		case 0b00: temp &= 0xfff8; break; // clear bit 2, 1 and 0
	}
	MODM_LOG_DEBUG.printf("temp = %d\n", temp);

	// Convert to centigrad
	convertedTemperature = ((625 * temp) ) / 100;
}

} // ds18b20data namespace

} // modm namespace
