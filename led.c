//  This file is part of kbus_daemon.
//--------------------------------------------------------------------------
//  kbus_mqtt_client is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//--------------------------------------------------------------------------
//  kbus_mqtt_client is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//--------------------------------------------------------------------------
//  You should have received a copy of the GNU General Public License
//  along with kbus_mqtt_client.  If not, see <https://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------

#include <stdio.h>
#include "led.h"

void set_led(int led_state) {
	switch (led_state) {
	case 1:
		system("echo 255 > /sys/class/leds/run-green/brightness");
		system("echo 0 > /sys/class/leds/run-red/brightness");
		break;
	case 2:
		system("echo 255 > /sys/class/leds/run-green/brightness");
		system("echo 255 > /sys/class/leds/run-red/brightness");
		break;
	case 128:
		system("echo 0 > /sys/class/leds/run-green/brightness");
		system("echo 255 > /sys/class/leds/run-red/brightness");
		break;
	default:
		system("echo 0 > /sys/class/leds/run-green/brightness");
		system("echo 0 > /sys/class/leds/run-red/brightness");
		break;
	}
}