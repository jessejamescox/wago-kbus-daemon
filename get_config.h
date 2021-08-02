#pragma once
#ifndef __GET_CONFIG_H__
#define __GET_CONFIG_H__

#include <stdbool.h>
#include <libconfig.h>

struct	prog_config {
	char *node_id;
	bool start_local_broker;
	char *mqtt_endpoint;
	int mqtt_port;
	bool support_tls;
	char *cert_path;
	char *key_path;
	char *rootca_path;
	bool support_aws_shadow;
	char *event_sub_topic;
	char *event_pub_topic;
	char *status_pub_topic;
	int analog_deadband;
};

extern struct prog_config this_config;

extern struct prog_config get_program_config();

#endif /*__GET_CONFIG_H__*/
