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
#define	MODM_LOG_LEVEL modm::log::DISABLE

template <typename OneWire>
modm::Ds18b20<OneWire>::Ds18b20(Data &data, const uint8_t *rom) :
	timeout(0), data(data)
{
	std::memcpy(this->identifier, rom, 8);
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
modm::Ds18b20<OneWire>::setIdentifier(const uint8_t *rom)
{
	if (rom[0] != FAMILIY_CODE_DS18B20) {
		MODM_LOG_ERROR.printf("Wrong familiy code in rom. Expected %02x, received %02x\n", FAMILIY_CODE_DS18B20, rom[0]);
		return false;
	}
	std::memcpy(this->identifier, rom, 8);
	return true;
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
modm::Ds18b20<OneWire>::configure(Resolution res, uint8_t upperAlarm, uint8_t lowerAlarm)
{
	if (not selectDevice()) {
		MODM_LOG_WARNING.printf("Device not found");
		return false;
	}

	ow.writeByte(WRITE_SCRATCHPAD);

	ow.writeByte(upperAlarm);
	ow.writeByte(lowerAlarm);
	ow.writeByte(static_cast<uint8_t>(res));

	// Readback to verify
	// It is considered that the configure method is used seldom and thus the
	// overhead of reading back can be accepted.
	if (readScratchpad() and (static_cast<uint8_t>(res) == scratchpad[4])) {
		return true;
	} else {
		MODM_LOG_ERROR.printf("configure: Readback failed. Expected: %02x, received: %02x\n", static_cast<uint8_t>(res), scratchpad[4]);
		return false;
	}
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

	// Conversion will take 750 msec at 12 bits
	timeout.restart(750);
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

	// Conversion will take 750 msec at 12 bits
	timeout.restart(750);
}

template <typename OneWire>
bool
modm::Ds18b20<OneWire>::isConversionDone()
{
	if (not timeout.isExpired()) {
		return false;
	}
	return ow.readBit();
}

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
// modm::Ds18b20<OneWire>::readTemperature(int16_t &temperature)
modm::Ds18b20<OneWire>::readout()
{
	if (readScratchpad()) {
		data.updateTemperature(scratchpad);
		return true;
	}
	return false;
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

// ----------------------------------------------------------------------------
template <typename OneWire>
bool
modm::Ds18b20<OneWire>::readScratchpad()
{
	if (not selectDevice()) {
		MODM_LOG_WARNING.printf("Device not found");
		return false;
	}
	ow.writeByte(this->READ_SCRATCHPAD);

	// Read data
	uint8_t crc = 0;
	for (uint8_t ii = 0; ii < scratchpad_size; ++ii) {
		scratchpad[ii] = ow.readByte();
	}

	// CRC Check
	for (uint8_t ii = 0; ii < 8; ++ii) {
		crc = ow.crcUpdate(crc, scratchpad[ii]);
	}

	// MODM_LOG_DEBUG.printf("calculated crc: %02x\n", crc);
	// MODM_LOG_DEBUG.printf("read crc: %02x\n", scratchpad[8]);

	MODM_LOG_DEBUG.printf("config: %02x\n", scratchpad[4]);

	bool crc_match = (crc == scratchpad[8]);

	if (not crc_match) {
		MODM_LOG_DEBUG.printf("CRC did not match. Expected: %02x, received: %02x\n", crc, scratchpad[8]);
	}

	return crc_match;
}
