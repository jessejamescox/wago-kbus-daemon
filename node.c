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
#include "node.h"
#include "logger.h"

int build_module_object(int terminalCount, tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX], u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX], struct module modules[LDKC_KBUS_TERMINAL_COUNT_MAX]) {
	int i = 0;
	// cycle through the modules and read the values
	for(i = 0 ; i < terminalCount ; i++) {
		modules[i].position = i + 1;
		switch (terminals[i])
		{
		case 400 ... 499:
			modules[i].type = "AI";
			modules[i].pn = terminals[i];
			modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
			log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			break;
		case 500 ... 599:
			modules[i].type = "AO";
			modules[i].pn = terminals[i];
			modules[i].channelCount = terminalDescription[i].AdditionalInfo.ChannelCount;
			modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
			log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
			break;
		case 34000 ... 37000:
			if (terminals[i] % 2 == 0) {
				modules[i].type = "DO";
				modules[i].pn = terminals[i];
				modules[i].channelCount = (terminalDescription[i].SizeOutput_bits);
				modules[i].bitOffsetOut = terminalDescription[i].OffsetOutput_bits;
				log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				break;
			}
			else {
				modules[i].type = "DI";
				modules[i].pn = terminals[i];
				modules[i].channelCount = (terminalDescription[i].SizeInput_bits);
				modules[i].bitOffsetIn = terminalDescription[i].OffsetInput_bits;
				log_trace("Module %d is a %d channel %s at bit offset %d", modules[i].position, modules[i].channelCount, modules[i].type, modules[i].bitOffsetIn);
				break;
			}
			// default to "unsupported module type	
		default :
			modules[i].type = "unsupported";
			modules[i].pn = terminals[i];
			log_error("Module %d is unsupported", i);
			break;
		}
	} 
	return 1;
}

char *map_switch_state(int switch_state) {
	switch (switch_state) {
	case -1:
		return "ERROR";
	case 1:
		return "RUN";
	case 2:
		return "STOP";
	case 8:
		return "RESET";
	case 128:
		return "RESET_ALL";
	default:
		return "NA";
	}
}
