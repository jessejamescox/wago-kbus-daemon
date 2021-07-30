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

#define CAFLE "/etc/ssl/certs/root.ca.pem"
#define CERTFILE "/etc/ssl/certs/f8c21b07a4a54d66ad1d71b14dbf9a7db4848817d9837385d10b6eedd90df4f3-certificate.pem.crt"
#define KEYFILE "/etc/ssl/certs/f8c21b07a4a54d66ad1d71b14dbf9a7db4848817d9837385d10b6eedd90df4f3-private.pem.key"

// main controller node object
struct node controller;
struct node controllerLast;

// main mosquitto object
struct mosquitto *mosq;// = NULL;
struct prog_config this_config;
int switch_state = 0;
int mqtt_state = 0;

int led_state = 0;

int i_cycles = 0;

int main(int argc, char *argv[]) {
	
	log_set_quiet(true);
	log_set_level(0);

	// get the config
	this_config = get_program_config();
	usleep(1000);
	
	// scan the kbus
	int kbus_resp = kbus_init(&kbus);//, &adi);
	
	// map everything to the controller object as part of the init process
	if(build_module_object(kbus.terminalCount, kbus.terminalDescription, kbus.terminals, &controller.modules)) {
		controller.number_of_modules = kbus.terminalCount;
	}
	
	controller.nodeId = this_config.node_id;
	
	mosquitto_lib_init();
	
	mosq = mosquitto_new(NULL, true, NULL);
	if (!mosq)
	{
		log_error("Can't initialize Mosquitto library");
		exit(-1);
	}
	
	if (this_config.support_tls)
	{
		mosquitto_tls_set(mosq, CAFLE, NULL, CERTFILE, KEYFILE, NULL);
	}
	while(1) {
		int runState = 0;
		
		switch_state =	get_switch_state();
		controller.switch_state = map_switch_state(switch_state);
		
		if (switch_state == 1)
		{
			
			// Establish a connection to the MQTT server. Do not use a keep-alive ping
				int mosqConnectResp = mosquitto_connect(mosq, this_config.mqtt_endpoint, this_config.mqtt_port, 0);
				if (mosqConnectResp)
				{
					log_error("Failed to connect to %s at port %d", this_config.mqtt_endpoint, this_config.mqtt_port);
					//return -1;
				}
				else
				{
					controllerLast = controller;
					//int *kbusJsonObject = build_controller_object(mosq , controller);

					int mosqSubResp = mosquitto_subscribe(mosq, NULL, this_config.event_sub_topic, 0);
					if (mosqSubResp)
					{
						log_error("Problem subscribing to %s", this_config.event_sub_topic);
					}
					else
					{
						// Specify the function to call when a new message is received
						mosquitto_message_callback_set(mosq, mqtt_callback);
						mqtt_state = 1;
						//set_led(IS_RUNNING);
					}
				}
			}
			else {
				if (led_state != 0) {
					controller.switch_state = map_switch_state(switch_state);
					int *kbusJsonObject = build_controller_object(mosq , controller);
					//set_led(IS_STOPPED);
					led_state = 1;
				} 
			}
		
		int initCycles = 0;
		sleep(2);
		
			// secondary loop to check switch state
			while((switch_state == 1) && (mqtt_state == 1)) {
				// loop to get the switch state
				switch_state =	get_switch_state();
			
				// loop the mosquitto
				int mosq_loop = mosquitto_loop(mosq, -1, 1);
				if (mosq_loop) {					
					log_error("Mosquitto loop connection error!\n");
					char *mosq_err = mosquitto_strerror(mosq_loop);
					usleep(100000);
					runState = 0;
					initCycles = 0;
					mosquitto_reconnect(mosq);
				}
				
				if ((!runState) && (initCycles == 100))	{
					int *kbusJsonObject = build_controller_object(mosq, controller);
					runState = 1;
					initCycles = 0;
					sleep(1);
				}
				else
				{
					int kbus_resp = kbus_read(mosq, this_config, kbus, controller);
					if (kbus_resp != 0) {
						char *kbus_error_string = build_error_object(true, controller, this_config, "kbus error present");
						mosquitto_publish(mosq, NULL, this_config.status_pub_topic, strlen(kbus_error_string), kbus_error_string, 0, 0);
					}	
				}
				initCycles++;
				usleep(50000);
			}
		sleep(1);
	} // main while loop 	
	//printf("Got here");
}