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
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <wago_oms_API.h>
#include "switch.h"

#define BUTTON_ERROR     -1
#define BUTTON_RUN       0x01
#define BUTTON_STOP      0x02
#define BUTTON_RESET     0x08
#define BUTTON_RESET_ALL 0x80

int get_switch_state(void) {

	// generic vars
	int32_t switchState  = 0;
	uint8_t button_state = 0;

	tOmsReturn retval;   //Unused retval of omsDev->Get..()

	// Open in passive mode, active polling for current state of RUN-STOP-RESET switch
	tOmsDevice *omsDev = oms_OpenDevice("/dev/input/event0", OMS_MODE_PASSIVE);
	if (omsDev == NULL)
	{
		//printf("ERROR: oms_OpenDevice() failes\n");
		return BUTTON_ERROR;
	}

	// Check RUN-STOP switch for position "RUN"
	if(OMS_RETURN_OK == omsDev->GetRunKey(omsDev, &switchState))
	{
		if (switchState == 1) // Switch in position "RUN"
			{
				button_state = BUTTON_RUN;
			}
	}

	// Check RUN-STOP switch for position "STOP"
	if(OMS_RETURN_OK == omsDev->GetStopKey(omsDev, &switchState))
	{
		if (switchState == 1)// Switch in position "STOP"
		{
			button_state = BUTTON_STOP;
		}
	}

	// Check RESET switch
	if(OMS_RETURN_OK == omsDev->GetResetKey(omsDev, &switchState))
	{
		if (switchState == 1) // Switch in position "RESET"
			{
				button_state = BUTTON_RESET;
			}
	}

	// Check RST_ALL button
	if(OMS_RETURN_OK == omsDev->GetRstAllKey(omsDev, &switchState))
	{
		if (switchState == 1) // Button RST_ALL is pressed
			{
				button_state |= BUTTON_RESET_ALL;
			}
	}

	oms_CloseDevice(omsDev);
	switch (button_state & ~BUTTON_RESET_ALL) //check for Main-switch only! So remeove RESET_ALL
		{
		case BUTTON_RUN:
			//printf("RUN");
			return 1;
			break;
		case BUTTON_STOP:
			//printf("STOP");
			return 0;
			break;
		case BUTTON_RESET:
			//printf("RESET");
			return 3;
			break;
		default:
			//printf("ERROR");
			return - 1;
			break;
		}

	//if also BUTTON_RESET_ALL is active add this to output
	if(button_state & BUTTON_RESET_ALL)
	{
		;//printf("-RESET_ALL");
	}

	//printf("\n");  //flush to stdout
	//return 0;
}
