#ifndef __JSON_H__
#define __JSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <mosquitto.h>
#include "get_config.h"
#include "node.h"
#include "logger.h"

struct channel_command
{
	int module;
	int channel;
	int value;
};

//extern struct json_object *jsonCompleteKbus;
//extern struct json_object *jsonSend;

extern char *build_error_object(bool error, struct node controller, struct prog_config, char *error_msg);

extern struct channel_command parse_command_message(char *message);//, int *module_position, int * channel_position, int *channel_value);

extern voidbuild_controller_object(struct mosquitto *mosq, struct node controller);

extern void build_event_object(struct mosquitto *mosq, struct node controller, int modulePosition, int channelPosition, int channelValue);

extern int *parse_mqtt(char *message);

#endif /*__JSON_H__*/