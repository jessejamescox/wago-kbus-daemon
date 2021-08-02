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
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include "json-c/json.h"
#include "kbus-daemon.h"
#include "kbus.h"
#include "json.h"
#include "node.h"
#include "get_config.h"
#include <time.h>

#define MAX_IMG_LENGTH 2048

struct prog_config this_config;

char *build_error_object(bool error, struct node controller, struct prog_config this_config, char *error_msg) {
	json_object *json_error_object = json_object_new_object();
	json_object_object_add(json_error_object, "node_id", json_object_new_string(this_config.node_id));
	json_object_object_add(json_error_object, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(json_error_object, "error", json_object_new_boolean(error));
	json_object_object_add(json_error_object, "error_msg", json_object_new_string(error_msg));
	char *return_string = json_object_to_json_string(json_error_object);
	return return_string;
}

void build_controller_object(struct mosquitto *mosq, struct node controller) {
	
	//index vars
	int iModules, iChannels;
	
	// main object creation
	struct json_object	*jsonMain		= json_object_new_object();
	struct json_object	*jsonState		= json_object_new_object();
	struct json_object	*jsonReported	= json_object_new_object();
	struct json_object	*jsonController	= json_object_new_object();
	struct json_object	*jsonModules	= json_object_new_object();
	
	for (iModules = 0; iModules < controller.number_of_modules; iModules++) {
		
		struct json_object	*jsonModule		= json_object_new_object();
		struct json_object	*jsonChannels	= json_object_new_object();
		
		// build the module object one-by-one
		char *mod;
		asprintf(&mod, "module%i", (iModules + 1));
		
		for (iChannels = 0; iChannels < controller.modules[iModules].channelCount; iChannels++) {
			
			struct json_object	*jsonChannel	= json_object_new_object();
			
			if (!strcmp(controller.modules[iModules].type, "DI"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_boolean(false));
			}
			if (!strcmp(controller.modules[iModules].type, "DO"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_boolean(false));
			}
			if (!strcmp(controller.modules[iModules].type, "AI"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_int(0));
			}
			if (!strcmp(controller.modules[iModules].type, "AO"))
			{
				json_object_object_add(jsonChannel, "value", json_object_new_int(0));
			}
			
			// build the modules channels one-by-one
			char *chn;
			asprintf(&chn, "channel%i", (iChannels + 1));
			
			// build the module object
			json_object_object_add(jsonChannels, chn, json_object_get(jsonChannel));
			
		}
		
		json_object_object_add(jsonModule, "pn", json_object_new_int(controller.modules[iModules].pn));
		json_object_object_add(jsonModule, "module_type", json_object_new_string(controller.modules[iModules].type));
		json_object_object_add(jsonModule, "position", json_object_new_int(iModules + 1));
		json_object_object_add(jsonModule, "channel_count", json_object_new_int(controller.modules[iModules].channelCount));
		json_object_object_add(jsonModule, "channels", json_object_get(jsonChannels));
		
		// add this to the reported object
		json_object_object_add(jsonModules, mod, json_object_get(jsonModule));
		
		json_object_put(jsonChannels);
		json_object_put(jsonModule);
	}
	
	// build the main json object	
	json_object_object_add(jsonController, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jsonController, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(jsonController, "module_count", json_object_new_int(controller.number_of_modules));
	
	// add this to the reported object
	json_object_object_add(jsonController, "modules", json_object_get(jsonModules));
	
	// add this to the reported object
	json_object_object_add(jsonReported, "controller", json_object_get(jsonController));
	
	// add the module to the reported
	json_object_object_add(jsonState, "reported", json_object_get(jsonReported));
	
	// add the reported to the state
	json_object_object_add(jsonMain, "state", json_object_get(jsonState));
	
	//char *controller_string = (char*)malloc(MAX_IMG_LENGTH * sizeof(char));
	// do I need to free this??
	char *controller_string = json_object_to_json_string(jsonMain);
	
	int pub_resp = mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(controller_string), controller_string, 0, 0);
	
	// clean up the json objects
	json_object_put(jsonModules);
	json_object_put(jsonController);
	json_object_put(jsonReported);
	json_object_put(jsonState);
	json_object_put(jsonMain);

}

void build_event_object(struct mosquitto *mosq, struct node controller, int modulePosition, int channelPosition, int channelValue) {
	
	char *md;
	char *ch;
	
	asprintf(&md, "module%i", (modulePosition + 1));
	asprintf(&ch, "channel%i", (channelPosition + 1));
	 
	struct json_object	*jsonMain		= json_object_new_object();
	struct json_object	*jsonState		= json_object_new_object();
	struct json_object	*jsonReported	= json_object_new_object();
	struct json_object	*jsonController	= json_object_new_object();
	struct json_object	*jsonModules	= json_object_new_object();
	struct json_object	*jsonModule		= json_object_new_object();
	struct json_object  *jsonChannels	= json_object_new_object();
	struct json_object	*jsonChannel	= json_object_new_object();	
	
	// build the channel object
//	if(strcmp(controller.modules[modulePosition].type, "AI"))
//	{
//		json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
//	}
//	else {
//		json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
//	}
	if (!strcmp(controller.modules[modulePosition].type, "DI"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "DO"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_boolean(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "AI"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
	}
	if (!strcmp(controller.modules[modulePosition].type, "AO"))
	{
		json_object_object_add(jsonChannel, "value", json_object_new_int(channelValue));
	}
	
	// build the module object
	json_object_object_add(jsonChannels, ch, json_object_get(jsonChannel));
	
	json_object_object_add(jsonModule, "channels", json_object_get(jsonChannels));
	
	
	// add this to the reported object
	json_object_object_add(jsonModules, md, json_object_get(jsonModule));
	
	// add this to the reported object
	json_object_object_add(jsonController, "node_id", json_object_new_string(controller.nodeId));
	json_object_object_add(jsonController, "switch_state", json_object_new_string(controller.switch_state));
	json_object_object_add(jsonController, "modules", json_object_get(jsonModules));
	
	// add this to the reported object
	json_object_object_add(jsonReported, "controller", json_object_get(jsonController));
	
	// add the module to the reported
	json_object_object_add(jsonState, "reported", json_object_get(jsonReported));
	
	// add the reported to the state
	json_object_object_add(jsonMain, "state", json_object_get(jsonState));
	
	char *event_string = json_object_to_json_string(jsonMain);
	
	int pub_resp = mosquitto_publish(mosq, NULL, this_config.event_pub_topic, strlen(event_string), event_string, 0, 0);
	
	// clean up the json objects
	json_object_put(jsonChannel);
	json_object_put(jsonChannels);
	json_object_put(jsonModule);
	json_object_put(jsonModules);
	json_object_put(jsonController);
	json_object_put(jsonReported);
	json_object_put(jsonState);
	json_object_put(jsonMain);

}

int *parse_mqtt(struct mosquitto *mosq, char *message) {
	
	struct channel_command channelCmd;
	
	// the objects
	struct json_object *jsonState, *jsonDesired, *jsonController, *jsonModules, *jsonModule, *jsonChannels, *jsonChannel, *jsonValue;
	
	// get the main json object
	struct json_object *parsed_json = json_tokener_parse(message);
	
	// start checking the json objects
	if (json_object_object_get_ex(parsed_json, "state", &jsonState)) {
		if (json_object_object_get_ex(jsonState, "desired", &jsonDesired)) {
			if (json_object_object_get_ex(jsonDesired, "controller", &jsonController)) {
				if (json_object_object_get_ex(jsonController, "modules", &jsonModules)) {
					
					// search for the module object 
					for(int iModules = 0 ; iModules < controller.number_of_modules ; iModules++) {
						char *mod;
						asprintf(&mod, "module%i", (iModules + 1));
						
						// find the object 
						if(json_object_object_get_ex(jsonModules, mod, &jsonModule)) {
							
							// found the channel, record the module position
							channelCmd.module = iModules;
							if (json_object_object_get_ex(jsonModule, "channels", &jsonChannels)) {
							
								// search for the channels
								for(int iChannels = 0 ; iChannels < controller.modules[iModules].channelCount ; iChannels++) {
									char *chn;
									asprintf(&chn, "channel%i", (iChannels + 1));
									
									// find the object 
									if(json_object_object_get_ex(jsonChannels, chn, &jsonChannel)) {
										
										// found the channel, record the module position
										channelCmd.channel = iChannels;
										
										if (json_object_object_get_ex(jsonChannel, "value", &jsonValue)) {
											channelCmd.value = json_object_get_int(jsonValue);
											
											// write the kbus regs
											kbus_write(mosq, controller, channelCmd.module, channelCmd.channel, channelCmd.value);			
											
											// clean up
											json_object_put(jsonValue);
											json_object_put(jsonChannel);
											json_object_put(jsonChannels);
											json_object_put(jsonModule);
											json_object_put(jsonModules);
											json_object_put(jsonController);
											json_object_put(jsonDesired);
											json_object_put(jsonState);
											json_object_put(parsed_json);
											
											// return without error
											return 0;
										}
									}
								}
							}
						}
					}
				}	
			}
		}
	}
	
	// clean up
//	json_object_put(jsonValue);
//	json_object_put(jsonChannel);
//	json_object_put(jsonChannels);
//	json_object_put(jsonModule);
//	json_object_put(jsonModules);
//	json_object_put(jsonController);
	json_object_put(jsonDesired);
	json_object_put(jsonState);
	json_object_put(parsed_json);
	
	//return with error
	return 1;
};