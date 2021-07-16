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

#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "kbus-daemon.h"
#include "node.h"
#include "logger.h"
#include "get_config.h"
#include "json.h"
#include "kbus.h"

char *sub_topic;

void mqtt_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
	if(strcmp(message->topic, sub_topic) != 0) {
		log_error("received message out of topic range");
	}
	else	{
		// copy in the bu
		char *buffer = message->payload;
		int command_module_position, command_channel_position, command_channel_value;
		struct channel_command rcv_command = parse_command_message(buffer);
		kbus_write(rcv_command.module, rcv_command.channel, rcv_command.value);
	}
}