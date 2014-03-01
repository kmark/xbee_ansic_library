#include "xbee_device.h"
#include "xbee_platform.h"
#include "xbee_atcmd.h"
#include "_atinter.h"

xbee_dev_t my_xbee;
int status;

void setup() {
	Serial.begin(115200);

	xbee_serial_t serialStruct;
	serialStruct.baudrate = 19200;
	serialStruct.serial = &Serial3;
	xbee_dev_init(&my_xbee, &serialStruct, NULL, NULL);
	xbee_cmd_init_device(&my_xbee);
	delay(1000);
	do {
		xbee_dev_tick(&my_xbee);
		status = xbee_cmd_query_status(&my_xbee);
	} while(status == -EBUSY);
	if(status) {
		Serial.print("ERROR -> ");
		Serial.println(status);
		while(1);
	}
	printATCmds(&my_xbee);
}

char cmdstr[80];
void loop() {
	xbee_dev_tick(&my_xbee);
	if(xbee_readline(cmdstr, sizeof cmdstr) != -EAGAIN) {
		if(!strncmpi(cmdstr, "menu", 4)) {
			printATCmds(&my_xbee);
			Serial.println();
			Serial.print("FW ");
			Serial.println(my_xbee.firmware_version, HEX);
			Serial.print("HW ");
			Serial.println(my_xbee.hardware_version, HEX);
			Serial.println();
		} else {
			process_command(&my_xbee, cmdstr);
		}
	}
}

const xbee_dispatch_table_entry_t xbee_frame_handlers[] =
{
    XBEE_FRAME_HANDLE_LOCAL_AT,
    XBEE_FRAME_MODEM_STATUS_DEBUG,
    XBEE_FRAME_TABLE_END
};
