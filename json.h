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

extern struct json_object *jsonCompleteKbus;
extern struct json_object *jsonSend;

extern char *build_kbus_object(struct node controller, struct prog_config thisConfig);

extern char *build_event_object(struct prog_config this_config, int modulePosition, int channelPosition, int channelValue, int switchState);

extern char *build_error_object(struct prog_config, char *error_msg);

#endif /*__JSON_H__*/