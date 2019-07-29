/*
 * Copyright (c) 2016-2019, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef THREAD_BMP180
#define THREAD_BMP180

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/pressure/bmp085.hpp>

template < typename I2cMaster >
class Bmp180Thread: public modm::pt::Protothread
{
public:
	Bmp180Thread();

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

	int32_t
	getPressure() {
		return data.getPressure();
	}

private:
	modm::ShortTimeout timeout;

	modm::bmp085::Data data;
	modm::Bmp085<I2cMaster> barometer;

	bool start_measurement;
	bool new_data;
};

#include "thread_bmp180_impl.hpp"

#endif // THREAD_BMP180
