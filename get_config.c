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
	if(!config_lookup_string(cf, "node_id", &config_hold.node_id))
		printf("could not find the node id config param");
	
	// see if we need to run local broker
	if (!config_lookup_bool(cf, "start_local_broker", &config_hold.start_local_broker))
		printf("could not find the local broker config param");
	
	// get the mqtt endpoint
	if(!config_lookup_string(cf, "mqtt_endpoint", &config_hold.mqtt_endpoint))
		printf("could not find the endpoint config param");
	
	// get the mqtt port
	if(!config_lookup_int(cf, "mqtt_port", &config_hold.mqtt_port))
		printf("could not find the mqtt port config param");
	
	return (config_hold);
	
};