#ifndef __MQTT_H__
#define __MQTT_H__

#include <mosquitto.h>

extern void mqtt_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
#endif /*__MQTT_H__*/