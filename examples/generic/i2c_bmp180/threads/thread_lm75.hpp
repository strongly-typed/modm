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

#ifndef THREAD_LM75
#define THREAD_LM75

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/temperature/lm75.hpp>

template < typename I2cMaster >
class Lm75Thread: public modm::pt::Protothread
{
public:
	Lm75Thread();

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

	modm::lm75::Data data;
	modm::Lm75<I2cMaster> thermometer;

	bool start_measurement;
	bool new_data;
};

#include "thread_lm75_impl.hpp"

#endif // THREAD_LM75
