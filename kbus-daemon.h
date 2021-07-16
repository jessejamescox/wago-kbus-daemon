#pragma once
#ifndef __KBUS_DAEMON__
#define __KBUS_DAEMON__

#include "kbus.h"

extern struct node controller;
extern char *event_pub_topic;	// = malloc(sizeof(char) * 100);
extern char *status_pub_topic;
extern char *sub_topic;
// extern tApplicationDeviceInterface *adi;

#endif /*__KBUS-DAEMON__*/