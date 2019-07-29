/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DS18B20_HPP
	#error	"Don't include this file directly, use 'ds18b20.hpp' instead!"
#endif

#include <cstring>		// for std::memcpy

#include <modm/debug/logger.hpp>

#ifdef  MODM_LOG_LEVEL
#undef	MODM_LOG_LEVEL
#endif
#define	MODM_LOG_LEVEL modm::log::DEBUG

template <typename OneWire>
modm::Ds18b20<OneWire>::Ds18b20(Data &data, const uint8_t *rom) :
	data(data)
{
	std::memcpy(this->identifier, rom, 8);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
void
modm::Ds18b20<OneWire>::setIdentifier(const uint8_t *rom)
{
	std::memcpy(this->identifier, rom, 8);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
modm::Ds18b20<OneWire>::isAvailable()
{
	return ow.verifyDevice(this->identifier);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
void
modm::Ds18b20<OneWire>::startConversion()
{
	selectDevice();

	ow.writeByte(CONVERT_T);
}

template <typename OneWire>
void
modm::Ds18b20<OneWire>::startConversions()
{
	//Reset the bus / Initialization
	if (not ow.touchReset()) {
		//no devices detected
		return;
	}

	// Send this to everybody
	ow.writeByte(one_wire::SKIP_ROM);

	// Issue Convert Temperature command
	ow.writeByte(this->CONVERT_T);
}

template <typename OneWire>
bool
modm::Ds18b20<OneWire>::isConversionDone()
{
	return ow.readBit();
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
// modm::Ds18b20<OneWire>::readTemperature(int16_t &temperature)
modm::Ds18b20<OneWire>::readout()
{
	selectDevice();
	ow.writeByte(this->READ_SCRATCHPAD);

	static constexpr uint8_t scratchpad_size = 9;
	uint8_t scratchpad[scratchpad_size];

	// Read data
	uint8_t crc = 0;
	for (uint8_t ii = 0; ii < scratchpad_size; ++ii) {
		scratchpad[ii] = ow.readByte();
	}

	// CRC Check
	for (uint8_t ii = 0; ii < 8; ++ii) {
		crc = ow.crcUpdate(crc, scratchpad[ii]);
	}

	// MODM_LOG_DEBUG.printf("calculated crc: %02x \n\r", crc);
	// MODM_LOG_DEBUG.printf("read crc: %02x \n\r", scratchpad[8]);

	data.updateConfig(scratchpad);

	crc_match = (crc == scratchpad[8]);

	if (crc_match) {
		data.updateTemperature(scratchpad);
	} else {
		MODM_LOG_ERROR.printf("CRC mismatch. Read: %x, Expected: %x\n", crc, scratchpad[8]);
	}

	return crc_match;
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
modm::Ds18b20<OneWire>::selectDevice()
{
	// Reset the bus / Initialization
	if (not ow.touchReset()) {
		// no devices detected
		return false;
	}

	ow.writeByte(one_wire::MATCH_ROM);

	for (uint8_t ii = 0; ii < 8; ++ii) {
		ow.writeByte(this->identifier[ii]);
	}

	return true;
}
