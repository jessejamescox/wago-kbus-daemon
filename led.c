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
#include <wago_oms_API.h>
#include <diagnostic_API.h>
#include <led_server.h>
#include "led.h"

#define RUN_COLOR_OFF     0x00040004
#define RUN_COLOR_GREEN   0x00040000
#define RUN_COLOR_BLINK   0x00040005
#define RUN_COLOR_RESET   0x00040001

void setRunLEDColor(int color)
{
	log_EVENT_LogId(color, true);
}