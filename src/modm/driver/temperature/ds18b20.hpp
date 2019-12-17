/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2019, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DS18B20_HPP
#define MODM_DS18B20_HPP

#include <modm/architecture/interface/one_wire.hpp>
#include <modm/processing/timer/timeout.hpp>

#include "ds18b20_data.hpp"

namespace modm
{

template < typename OneWire >
class Ds18b20;

struct ds18b20
{

static constexpr uint8_t FAMILIY_CODE_DS18B20 = 0x28;

using Data = modm::ds18b20data::Data;

enum class
Resolution : uint8_t
{
	Bit12 = 0b0111'1111, // 0.0625 degree steps, 750    msec
	Bit11 = 0b0101'1111, // 0.1250 degree steps, 375    msec
	Bit10 = 0b0011'1111, // 0.2500 degree steps, 187.5  msec
	Bit9  = 0b0001'1111, // 0.5000 degree steps,  93.75 msec
};

};

/**
 * \ingroup	modm_driver_ds18b20
 */
template <typename OneWire>
class Ds18b20 : public ds18b20
{
public:
	/**
	 * \brief	Constructor
	 *
	 * Default resolution is 12-bit.
	 *
	 * \param 	rom		8-byte unique ROM number of the device
	 */
	Ds18b20(Data &data, const uint8_t *rom);

	/// Constructor for setting address later
	Ds18b20(Data &data) : data(data) {};

	/**
	 * \brief	Set the address after class is constructed
	 *
	 * When ds18b20 class is a member of a class, an address of a device should be set after device search.
	 *
	 * \param 	rom		8-byte unique ROM number of the device
	**/
	bool
	setIdentifier(const uint8_t *rom);

	/**
	 * \brief	Set up three register values on the scratchpad
	 *
	 * Three registers are the high alarm trigger register, low alarm trigger register and the configuration register
	 * When alarm function is not used, TH and TL registers can serve as general-purpose memory
	 * Used write scratchpad command which can be found in the ds18b20 datasheet, 11p
	 *
	 * \param  upperAlarm  1-byte value for high alarm trigger register
	 * \param  lowerAlarm  1-byte value for low alarm trigger register
	 * \param  resolution  Thermometer Resolution in bits (9, 10, 11, 12)
	 *
	 * Temperature(°C) and alarm triger value
	 *   +125      0x7D
	 *    +85      0x55
	 *    +25.0625 0x19
	 *    +10.125  0x0A
	 *      0      0x00
	 *     -0.5    0xFF
	 *    -10.125  0xF5
	 *    -25.0625 0xE6
	 *    -55      0xC9
	 * Details in the DS18B20 datasheet on pp.6
	**/
	bool
	configure(Resolution res, uint8_t upperAlarm, uint8_t lowerAlarm);

	/**
	 * \brief	Check if the device is present
	 *
	 * \return	\c true if the device is found, \c false if the
	 * 			ROM number is not available on the bus.
	 */
	bool
	isAvailable();

	/**
	 * \brief	Start the conversion of this device
	 */
	void
	startConversion();

	/**
	 * \brief	Start the conversion for all connected devices
	 *
	 * Uses the SKIP_ROM command to start the conversion on all
	 * connected DS18B20 devices.
	 *
	 * \warning	Use this function with caution. If you have devices other
	 * 			than the DS18B20 connected to your 1-wire bus check if
	 * 			they tolerate the SKIP_ROM + CONVERT_T command.
	 */
	void
	startConversions();

	/**
	 * \brief	Check if the last conversion is complete
	 *
	 * \return	\c true conversion complete, \n
	 * 			\c false conversion in progress.
	 */
	bool
	isConversionDone();

	/**
	 * \brief	Read the current temperature from sensor to local storage
	 *
	 * \todo	Needs a better output format
	 * \return	\c true if reading temperature is successful, \c false if it was not successful	
	 */
	bool
	readout();

protected:
	/**
	 * \brief	Select the current device
	 */
	bool
	selectDevice();

	bool
	readScratchpad();

	uint8_t identifier[8];

	static constexpr size_t scratchpad_size = 9;
	uint8_t scratchpad[scratchpad_size];

	modm::ShortTimeout timeout;

	// Rom commands can be found pp.10-12 in the datasheet of DS18B20
	static const uint8_t READ_ROM = 0x33;
	static const uint8_t MATCH_ROM = 0x55;
	static const uint8_t SKIP_ROM = 0xCC;
	static const uint8_t ALARM_SEARCH = 0xEC;

	// DS18B20 function commands
	static const uint8_t CONVERT_T = 0x44;
	static const uint8_t WRITE_SCRATCHPAD = 0x4e;
	static const uint8_t READ_SCRATCHPAD = 0xbe;
	static const uint8_t COPY_SCRATCHPAD = 0x48;
	static const uint8_t RECALL_E2 = 0xb8;
	static const uint8_t READ_POWER_SUPPLY = 0xb4;

	static OneWire ow;

private:
	Data &data;
};

} // modm namespace

#include "ds18b20_impl.hpp"

#endif // MODM_DS18B20_HPP
