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

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

#include "threads/thread_bmp180.hpp"
#include "threads/thread_lm75.hpp"
#include "threads/thread_ds18b20.hpp"

using namespace Board;

using SensorsI2cMaster = I2cMaster1;
using OneWirePin = GpioB0;
using OneWireMaster = BitBangOneWireMaster<OneWirePin>;

int
main()
{
	Board::initialize();
	Leds::setOutput();

	// Initialise I2C
	SensorsI2cMaster::connect<GpioB6::Scl, GpioB7::Sda>();
	SensorsI2cMaster::initialize<Board::SystemClock, 10_kHz>();

	// Initialise 1-Wire
	OneWirePin::setInput(OneWirePin::InputType::PullUp);
	OneWireMaster::connect<OneWirePin::BitBang>();
	OneWireMaster::initialize<SystemClock>();

	// Create protothreads
	using Bmp180Thread = Bmp180Thread<SensorsI2cMaster>;
	using Lm75Thread = Lm75Thread<SensorsI2cMaster>;
	using Ds18b20Thread = Ds18b20Thread<OneWireMaster>;
	Bmp180Thread bmp180Thread;
	Lm75Thread lm75Thread;
	Ds18b20Thread ds18b20Thread;

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	modm::ShortPeriodicTimer timeout(1500);

	while (true)
	{
		bmp180Thread.update();
		lm75Thread.update();
		ds18b20Thread.update();

		if (timeout.execute()) {
			if (bmp180Thread.isNewDataAvailable()) {
			}
			bmp180Thread.startMeasurement();

			if (lm75Thread.isNewDataAvailable()) {
			}
			lm75Thread.startMeasurement();

			if (ds18b20Thread.isNewDataAvailable()) {
			}
			ds18b20Thread.startMeasurement();
		}

	}

	return 0;
}
