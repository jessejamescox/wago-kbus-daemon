//  This file is part of kbus_mqtt_client.
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
#include "json-c/json.h"
#include "json.h"
#include "node.h"
#include <time.h>

char *build_kbus_object(struct node controller, struct prog_config this_config) {
	//index vars
	int iModules, iChannels;
	
	// start to build the json struct
	json_object *jsonOutHold = json_object_new_object(); 
	json_object *jsonModules = json_object_new_array();
	json_object	*json_channels;
	json_object *jsonModuleHoldObject;
	json_object *jsonControllerObject = json_object_new_object();
	
	// build an object per module
	for(iModules = 0 ; iModules < controller.number_of_modules ; iModules++) {
		// build an object per channel
		json_channels = json_object_new_array();
		for (iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++) {
			int typResp = strcmp("DI", controller.modules[iModules].type);
			if ((0 == strcmp(controller.modules[iModules].type, "DI")) || (0 == strcmp(controller.modules[iModules].type, "DO"))) {
				json_object_array_add(json_channels, json_object_new_boolean(controller.modules[iModules].channelData[iChannels]));
			}
			if ((0 == strcmp(controller.modules[iModules].type, "AI")) || (0 == strcmp(controller.modules[iModules].type, "AO"))) {
				json_object_array_add(json_channels, json_object_new_int(controller.modules[iModules].channelData[iChannels]));
			}
		}
		jsonModuleHoldObject = json_object_new_object();
		json_object_object_add(jsonModuleHoldObject, "pn", json_object_new_int(controller.modules[iModules].pn));
		json_object_object_add(jsonModuleHoldObject, "module_type", json_object_new_string(controller.modules[iModules].type));
		json_object_object_add(jsonModuleHoldObject, "position", json_object_new_int(iModules + 1));
		json_object_object_add(jsonModuleHoldObject, "channel_count", json_object_new_int(controller.modules[iModules].channelCount));
		json_object_object_add(jsonModuleHoldObject, "channel_data", json_channels);
		json_object_array_add(jsonModules, jsonModuleHoldObject);
	}
	
	// build the main json object	
	json_object_object_add(jsonOutHold, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(jsonOutHold, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(jsonOutHold, "module_count", json_object_new_int(controller.number_of_modules));
	json_object_object_add(jsonOutHold, "modules", jsonModules);
	
	json_object_object_add(jsonControllerObject, "controller", jsonOutHold);
	char *jsonString = json_object_to_json_string(jsonControllerObject);
		
	return jsonString;
	
}

char *build_analog_event_object(struct prog_config this_config, int modulePosition, int channelPosition, int channelValue) {
	time_t time(time_t *t);
	
	json_object *jsonModule = json_object_new_object();
	json_object *jsonChannel = json_object_new_object();
	char *return_string;
	
	json_object_object_add(jsonChannel, "module", json_object_new_int(modulePosition));
	json_object_object_add(jsonChannel, "channel", json_object_new_int(channelPosition));
	json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
	
	json_object_object_add(jsonModule, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(jsonModule, "timestamp", json_object_new_int64(time));
	//json_object_object_add(jsonModule, "switch_state", json_object_new_string(switchState));
	json_object_object_add(jsonModule, "payload", jsonChannel);	
	
	return_string = json_object_to_json_string(jsonModule);	
	
	return return_string;
}

char *build_digital_event_object(struct prog_config this_config, int modulePosition, int channelPosition, bool channelValue) {
	time_t time(time_t *t);
	
	json_object *jsonModule = json_object_new_object();
	json_object *jsonChannel = json_object_new_object();
	char *return_string;
	
	json_object_object_add(jsonChannel, "module", json_object_new_int(modulePosition));
	json_object_object_add(jsonChannel, "channel", json_object_new_int(channelPosition));
	json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
	
	json_object_object_add(jsonModule, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(jsonModule, "timestamp", json_object_new_int64(time));
	//json_object_object_add(jsonModule, "switch_state", json_object_new_string(switchState));
	json_object_object_add(jsonModule, "payload", jsonChannel);	
	
	return_string = json_object_to_json_string(jsonModule);	
	
	return return_string;
}

char *build_error_object(struct prog_config this_config, char *error_msg) {
	json_object *json_error_object = json_object_new_object();
	json_object_object_add(json_error_object, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(json_error_object, "error", json_object_new_string(error_msg));
	char *return_string = json_object_to_json_string(json_error_object);
	return return_string;
}

struct channel_command parse_command_message(char *message)	{
	struct channel_command rcv_command;
	struct json_object *parsed_json, *module_num, *channel_num, *channel_val;
	parsed_json = json_tokener_parse(message);
	enum json_type value_type;
	if (json_object_object_get_ex(parsed_json, "module", &module_num) == true) {
		rcv_command.module = json_object_get_int(module_num);
	}
	if (json_object_object_get_ex(parsed_json, "channel", &channel_num) == true) {
		rcv_command.channel = json_object_get_int(channel_num);
	}
	if (json_object_object_get_ex(parsed_json, "value", &channel_val) == true) {
		rcv_command.value = json_object_get_int(channel_val);
	}
	//printf("Module: %d Channel: %d Value: %d \n", rcv_command.module, rcv_command.channel, rcv_command.value);
	return rcv_command;
};