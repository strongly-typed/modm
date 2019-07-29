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

#include <modm/driver/temperature/ds18b20.hpp>
#include <modm/debug/logger/logger.hpp>
#include <cmath>

#include "ds18b20_test.hpp"

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

void
Ds18b20Test::testConversion()
{
	// Demo values from datasheet DS18B20 p6, Table 1
	uint16_t raw_table[]    = { 0x07d0,  0x0550, 0x0191, 0x00a2, 0x0008, 0x0000, 0xfff8, 0xff5e,   0xfe6f,  0xfc90   };
	int16_t  temp_table_i[] = {   12500,   8500,   2506,   1012,     50,   0,        -50,  -1012,   -2506,   -5500   };
	float    temp_table_f[] = {   125.00f, 85.00f, 25.06f, 10.12f, 0.50f,  0.0f,   -0.50f, -10.12f, -25.06f, -55.00f };
	double   temp_table_d[] = {   125.00,  85.00,  25.06,  10.12,  0.50,   0.0,    -0.50,  -10.12,  -25.06,  -55.00  };

	TEST_ASSERT_EQUALS(MODM_ARRAY_SIZE(raw_table), MODM_ARRAY_SIZE(temp_table_i));
	TEST_ASSERT_EQUALS(MODM_ARRAY_SIZE(raw_table), MODM_ARRAY_SIZE(temp_table_f));
	TEST_ASSERT_EQUALS(MODM_ARRAY_SIZE(raw_table), MODM_ARRAY_SIZE(temp_table_d));

	modm::ds18b20::Data data;

	for (uint8_t ii = 0; ii < MODM_ARRAY_SIZE(raw_table); ++ii) {
		uint8_t scratchpad[] = { raw_table[ii] & 0xff, (raw_table[ii] >> 8) & 0xff, 0x00, 0x00, 0b0111'1111 };

		data.updateConfig(scratchpad);
		TEST_ASSERT_EQUALS(data.factor, 625);

		data.updateTemperature(scratchpad);

		{
			int32_t temperature;
			data.getTemperature(temperature);
			TEST_ASSERT_EQUALS(temperature, temp_table_i[ii]);
		}

		{
			float temperature;
			data.getTemperature(temperature);
			TEST_ASSERT_EQUALS(temperature, temp_table_f[ii]);
		}

		{
			double temperature;
			data.getTemperature(temperature);
			TEST_ASSERT_EQUALS(temperature, temp_table_d[ii]);
		}
	}
}
