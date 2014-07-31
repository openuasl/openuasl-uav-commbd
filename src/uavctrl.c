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
#include "secure_socket_layer.h"

MWSerialHandle_t* mws_handle;
SslHandle_t* ctrl_handle;

int is_stop_ctrl;

int CTRL_init(char* ip) {

	// init multiwii serial connection
	mws_handle = (MWSerialHandle_t*) malloc(sizeof(MWSerialHandle_t));
	MWSERIAL_init(mws_handle);

	/*

	// init control connection over secure socket layer
	ctrl_handle = (SslHandle_t*) malloc(sizeof(SslHandle_t));
	SSLAYER_init(ctrl_handle, ip, UAVCTRL_SERVER_PORT);
	SSL_connect(ctrl_handle->ssl);
*/
	return 0;
}

int CTRL_start() {

	if(MWSERIAL_start(mws_handle))
		return 1;

	return 0; //AUTH_cert_uav(ctrl_handle);
}

int CTRL_run(){
	char in_buffer[256];
	int flag;
	ssize_t read_count, i;
/*
	flag = fcntl(ctrl_handle->ssl_fd, F_GETFL, 0);
	fcntl(ctrl_handle->ssl_fd, F_SETFL, flag | O_NONBLOCK);
*/

	//이거 request 한번 날릴때마다 한꺼번에 read 못함
	usleep(50000);

	while(!is_stop_ctrl){

		read_count = read(mws_handle->serial_fd, in_buffer, sizeof(in_buffer));

		printf("read_count : %d\n", read_count);

		printf("%s\n", in_buffer);

		for(i=0; i<read_count; i++){
			printf("%02X", in_buffer[i] & 0xff);
		}

		printf("\n\n");
	}



	return 0;
}

void CTRL_end() {
	MWSERIAL_release(mws_handle);
	free(mws_handle);
	mws_handle = NULL;

	SSLAYER_release(ctrl_handle);
	free(ctrl_handle);
	ctrl_handle = NULL;
}
