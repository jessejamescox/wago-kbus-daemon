#pragma once
#ifndef __GET_CONFIG_H__
#define __GET_CONFIG_H__

#include <libconfig.h>

struct	prog_config {
	char *node_id;
	char *mqtt_endpoint;
	//int tls_encryption;
	int mqtt_port;
	//int cycleSpeedMs;
};

extern struct prog_config get_program_config();

#endif /*__GET_CONFIG_H__*/
