#ifndef __KBUS_H__
#define __KBUS_H__

//--------------------------------------------------------------------------
/// include files for WAGO ADI
//--------------------------------------------------------------------------
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include "node.h"
#include "logger.h"
#include <dal/adi_application_interface.h>
#include <ldkc_kbus_information.h>
#include <ldkc_kbus_register_communication.h>

typedef struct kbus {
	// vars for kbus interface
	tDeviceInfo deviceList[10];  					// the list of devices returned by the ADI
	size_t		nrDevicesFound;  					// number of ADI devices found in /dev
	size_t		nrKbusFound;  						// position of the kbus in the list
	tDeviceId	kbusDeviceId;  						// device ID returned by the ADI
	//tApplicationDeviceInterface *adi;  				// pointer to the application interface
	tApplicationStateChangedEvent event;  			// var to track the event of the ADI
	size_t	terminalCount;  							// num of modules returned
	u16 terminals[LDKC_KBUS_TERMINAL_COUNT_MAX];  									// array of module identifiers
	tldkc_KbusInfo_TerminalInfo terminalDescription[LDKC_KBUS_TERMINAL_COUNT_MAX];  	// array of module oobjects
	u_int32_t taskId;
}
;

extern int kbus_init(struct kbus *kbus, tApplicationDeviceInterface *adi);
extern int kbus_scan(struct kbus kbus, struct node controller, tApplicationDeviceInterface *adi);
extern int kbus_read_inputs(struct kbus kbus, struct node controller);

#endif /*__KBUS_H__*/