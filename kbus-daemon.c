//--------------------------------------------------------------------------
//	This program is free software : you can redistribute it and / or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//--------------------------------------------------------------------------
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//  GNU General Public License for more details.
//--------------------------------------------------------------------------
//  You should have received a copy of the GNU General Public License
//  along with this program.If not, see < https : //www.gnu.org/licenses/>.
//--------------------------------------------------------------------------
///	\file		kbus_daemon
///
///
///	\version	0.0.0 (alpha)
///
///
///	\brief		open project to interact with the kbus process image via mqtt
///
///
///	\author		Jesse Cox jesse.cox@wago.com
///
//--------------------------------------------------------------------------

#include <stdio.h>
#include <mosquitto.h>
#include "kbus-daemon.h"
#include "get_config.h"
#include "node.h"
#include "kbus.h"
#include "switch.h"
#include "mqtt.h"
#include "led.h"

#define IS_RUNNING	0x01
#define IS_STOPPED	0x02
#define IS_ERROR	0x80

// main functional kbus object
//struct kbus kbus;

// main controller node object
struct node controller;
struct node controllerLast;

// main kbus adi object
//tApplicationDeviceInterface *adi;

// main mosquitto object
struct mosquitto *mosq = NULL;

// main switch state
int switch_state = 0;
int mqtt_state = 0;

int main(int argc, char *argv[]) {
		
	// set the led state to stopped
	set_led(IS_STOPPED);
	
	// get the config
	struct prog_config this_config = get_program_config();
	
	// set the sub topic
	char *sub_topic;
	asprintf(&sub_topic, "%s/kbus/event/outputs", this_config.node_id);
	
	// set the pub topic
	//char *pub_topic;// = "hello/world";
	asprintf(&pub_topic, "%s/kbus/event/inputs", this_config.node_id);
	
	// scan the kbus
	int kbus_resp = kbus_init(&kbus);//, &adi);
	
	// map everything to the controller object as part of the init process
	if(build_module_object(kbus.terminalCount, kbus.terminalDescription, kbus.terminals, &controller.modules)) {
		controller.number_of_modules = kbus.terminalCount;
	}
	
	
	
	// main while loop to check run-ready status
	while(1) {
		switch_state =	get_switch_state();
		controller.switch_state = map_switch_state(switch_state);
		
		if (switch_state == 1)
		{
			mosq = mosquitto_new(NULL, true, NULL);
			if (!mosq)
			{
				log_error("Can't initialize Mosquitto library");
				exit(-1);
			}
			// Establish a connection to the MQTT server. Do not use a keep-alive ping
			int mosqConnectResp = mosquitto_connect(mosq, this_config.mqtt_endpoint, this_config.mqtt_port, 0);
			if (mosqConnectResp)
			{
				log_error("Failed to connect to %s at port %d", this_config.mqtt_endpoint, this_config.mqtt_port);
				return -1;
			}
			else
			{
				controllerLast = controller;
				char *kbusJsonObject = build_kbus_object(controller, this_config);
				int pub_resp = mosquitto_publish(mosq, NULL, pub_topic, strlen(kbusJsonObject), kbusJsonObject, 0, 0);

				int mosqSubResp = mosquitto_subscribe(mosq, NULL, sub_topic, 0);
				if (mosqSubResp)
				{
					log_error("Problem subscribing to %s", sub_topic);
				}
				else
				{
					// Specify the function to call when a new message is received
					mosquitto_message_callback_set(mosq, mqtt_callback);
					mqtt_state = 1;
					set_led(IS_RUNNING);
				}
			}
		}
		else {
			//set_led(IS_STOPPED);
		}
		
		// secondary loop to check switch state
		while((switch_state == 1) && (mqtt_state == 1)) {
			//set_led(IS_RUNNING);
			// loop to get the switch state
			switch_state =	get_switch_state();
			
			// loop the mosquitto
			int mosq_loop = mosquitto_loop(mosq, -1, 1);
			if (mosq_loop) {					
				log_error("Mosquitto loop connection error!\n");
				//set_led(IS_ERROR);
				mosquitto_reconnect(mosq);
			}
			
			int kbus_resp = kbus_read(mosq, this_config, kbus, controller);//, &adi);
			if (kbus_resp) {
				//printf("Kbus response; %d /n", kbus_resp);
			}
		}
	} // main while loop 	
	printf("Got here");
}