/*
 * Copyright (c) 2016, Niklas Hauser
 * Copyright (c) 2016-2019, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
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
#define MODM_DS18B20_DATA_HPP

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

// Forward declaration the test class
class Ds18b20Test;

namespace modm
{

template < typename OneWireMaster >
class Ds18b20;

namespace ds18b20data
{

/// @ingroup modm_driver_ds18b20
class modm_packed
Data
{
public:
	// Grant sensor full access to private members.
	template < typename OneWireMaster >
	friend class ::modm::Ds18b20;

	// Grant unit test full access to private members.
	friend class ::Ds18b20Test;

public:
	/**
	 * Get the calibrated temperature for the device in 0.01 degree Celsius
	 */
	int32_t inline
	getTemperature() { return convertedTemperature; };

	void inline
	getTemperature(int32_t &temp) { temp = getTemperature(); };

	void inline
	getTemperature(float &temp) { temp = float(getTemperature() / 100.0f); };

	void inline
	getTemperature(double &temp) { temp = double(getTemperature()) / double(100.0); };

protected:
	void
	updateConfig(uint8_t scratchpad[8]);

	void
	updateTemperature(uint8_t scratchpad[8]);

protected:
	int16_t convertedTemperature;

	uint16_t factor;
};

} // ds18b20data namespace

} // modm namespace

#include "ds18b20_data_impl.hpp"

#endif
