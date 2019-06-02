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

using namespace Board;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	Uart1::connect<GpioOutput1_7::Tx>();

	LedRed::reset();

	while (true)
	{
		LedRed::toggle();
		modm::delayMilliseconds(50);
	}

	return 0;
}
