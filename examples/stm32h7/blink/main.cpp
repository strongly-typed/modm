/*
 * Copyright (c) 2015-2017, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#include <modm/platform.hpp>
using namespace modm::platform;

int main()
{
    GpioB0::setOutput();
    while (true) {
        GpioB0::toggle();
        modm::delay_ms(500);
    }
}
