#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "uavctrl.h"
#include "multiwii_serial_connection.h"

MWSerialHandle_t* mws_handle;

int CTRL_init() {
	mws_handle = (MWSerialHandle_t*)malloc(sizeof(MWSerialHandle_t));

	MWSERIAL_init(mws_handle);
	MWSERIAL_start(mws_handle);


	return 0;
}


void CTRL_end(){
	MWSERIAL_release(mws_handle);
	free(mws_handle);
	mws_handle = NULL;
}
