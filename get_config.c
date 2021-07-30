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
#include <unistd.h>
#include <limits.h>
#include <libconfig.h>
#include "get_config.h"

#define CONFIG_FILE "/etc/kbus-daemon/kbus-daemon.cfg"
#define HOST_NAME_MAX 100

//struct progConfig thisConfig;

struct prog_config get_program_config() {
	config_t cfg, *cf;
	const config_setting_t *retries;
	const char *hostname = NULL;
	int cycleSpeed;
	struct prog_config config_hold;
	
	cf = &cfg;
	config_init(cf);
	
	if (!config_read_file(cf, CONFIG_FILE)) {
		fprintf(stderr,
			"%s:%d - %s\n",
			config_error_file(cf),
			config_error_line(cf),
			config_error_text(cf));
		config_destroy(cf);
		//return (EXIT_FAILURE);
	}
	
	// get the identification
	if(!config_lookup_string(cf, "node_id", &config_hold.node_id)) {
		printf("could not find the node id config param");
	}
	
	// see if we need to run local broker
	if(!config_lookup_bool(cf, "start_local_broker", &config_hold.start_local_broker)) {
		printf("could not find the local broker config param");
	}
	
	// get the mqtt endpoint
	if(!config_lookup_string(cf, "mqtt_endpoint", &config_hold.mqtt_endpoint)) {
		printf("could not find the endpoint config param");
	}
	
	// get the mqtt port
	if(!config_lookup_int(cf, "mqtt_port", &config_hold.mqtt_port)) {
		printf("could not find the mqtt port config param");
	}
	
	// get the tls string
	if(!config_lookup_bool(cf, "support_tls", &config_hold.support_tls)) {
		printf("could not find the tls enable param");
	}
	
	if (config_hold.support_tls)
	{
		// get the credentials
		if(!config_lookup_string(cf, "cert_path", &config_hold.cert_path))
		{
			printf("could not find the tls certificate path");
		}
		
		if (!config_lookup_string(cf, "key_path", &config_hold.key_path))
		{
			printf("could not find the tls key path");
		}
		
		if (!config_lookup_string(cf, "rootca_path", &config_hold.rootca_path))
		{
			printf("could not find the tls root ca path");
		}
		
		if (!config_lookup_bool(cf, "support_aws_shadow", &config_hold.support_aws_shadow))
		{
			printf("could not find the aws shadow setting");	
		}
	}
	
	if (config_hold.support_aws_shadow)	{
		// get the sub topic
		//asprintf(&config_hold.event_sub_topic, "$aws/things/%s/shadow/update", config_hold.node_id);
		// get the sub topic
		if(!config_lookup_string(cf, "event_sub_topic", &config_hold.event_sub_topic)) {
			printf("could not find the tls enable param");
		} asprintf(&config_hold.event_sub_topic, "%s%s", config_hold.node_id, config_hold.event_sub_topic);
	
		// get the pub topic
		asprintf(&config_hold.event_pub_topic, "$aws/things/%s/shadow/update", config_hold.node_id);
	
		// get the status topic
		asprintf(&config_hold.status_pub_topic, "$aws/things/%s/shadow/update", config_hold.node_id);
	}
	
	else	{
		// get the sub topic
		if(!config_lookup_string(cf, "event_sub_topic", &config_hold.event_sub_topic)) {
			printf("could not find the tls enable param");
		} asprintf(&config_hold.event_sub_topic, "%s%s", config_hold.node_id, config_hold.event_sub_topic);
	
		// get the pub topic
		if(!config_lookup_string(cf, "event_pub_topic", &config_hold.event_pub_topic)) {
			printf("could not find the tls enable param");
		} asprintf(&config_hold.event_pub_topic, "%s%s", config_hold.node_id, config_hold.event_pub_topic);
	
		// get the status topic
		if(!config_lookup_string(cf, "status_pub_topic", &config_hold.status_pub_topic)) {
			printf("could not find the tls enable param");
		} asprintf(&config_hold.status_pub_topic, "%s%s", config_hold.node_id, config_hold.status_pub_topic);
	}
	
	return (config_hold);
	
};