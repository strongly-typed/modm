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
#include <modm/debug/logger.hpp>
using namespace modm::literals;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Board::LedRed::set();

	// Enable UART 1
	Uart1::connect<GpioOutput1_7::Tx>();
	Uart1::initialize<Board::SystemClock, 9600_Bd>();

	while (true)
	{
		static uint8_t c = 'A';
		Board::LedRed::toggle();
		Uart1::write(c);
		++c;
		if (c > 'Z') {
			c = 'A';
		}
		modm::delayMilliseconds(500);
	}

	return 0;
}
