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

#ifndef THREAD_DS18B20
#define THREAD_DS18B20

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/temperature/ds18b20.hpp>

template < typename OneWireMaster >
class Ds18b20Thread: public modm::pt::Protothread
{
public:
	Ds18b20Thread();

	bool
	update();

	bool
	startMeasurement();

	bool
	isNewDataAvailable() {
		return new_data;
	}

	int16_t
	getTemperature() {
		return data.getTemperature();
	}

private:
	modm::ShortTimeout timeout;

	modm::ds18b20::Data data;
	modm::Ds18b20< OneWireMaster > ds18b20;

	uint8_t rom[8];

	bool start_measurement;
	bool new_data;
};

#include "thread_ds18b20_impl.hpp"

#endif // THREAD_DS18B20
