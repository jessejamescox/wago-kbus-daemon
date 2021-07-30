#pragma once
#ifndef __NODE_H__
#define __NODE_H__

#include <dal/adi_application_interface.h>
#include <ldkc_kbus_information.h>
#include <ldkc_kbus_register_communication.h>
#include "logger.h"

struct	module {
	char *type;
	int pn;
	int position;
	int channelCount;
	int bitOffsetIn;
	int bitOffsetOut;
	int channelData[16];
};

struct node
{
	char *nodeId;
	char *switch_state;
	int	number_of_modules;	
	struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX];
};

extern int build_module_object(int terminalCount, tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX], u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX], struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX]);
extern char *map_switch_state(int switch_state);

#endif /*__NODE_H__*/