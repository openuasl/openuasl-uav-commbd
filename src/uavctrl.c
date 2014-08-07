#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#include "uavctrl.h"
#include "multiwii_serial_connection.h"
#include "secure_socket_layer.h"
#include "auth_mgr.h"

MWSerialHandle_t* mws_handle;
SslHandle_t* ctrl_handle;

int is_stop_ctrl;

int CTRL_init(char* ip) {

	// init multiwii serial connection
	mws_handle = (MWSerialHandle_t*) malloc(sizeof(MWSerialHandle_t));
	MWSERIAL_init(mws_handle);

	// init control connection over secure socket layer
	ctrl_handle = (SslHandle_t*) malloc(sizeof(SslHandle_t));
	SSLAYER_init(ctrl_handle, ip, UAVCTRL_SERVER_PORT);
	SSL_connect(ctrl_handle->ssl);

	return 0;
}

int CTRL_start() {
	return AUTH_cert_uav(ctrl_handle);
}

void* send_multiwii_status(void* p){
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	ssize_t read_count;

	while(!is_stop_ctrl){
		read_count = MWSERIAL_read(mws_handle, in_buffer, UAVCTRL_BUF_SIZE);

		if(read_count <= 0){
			usleep(1000000);
			continue;
		}

		memcpy(out_buffer+1, in_buffer, read_count);
		out_buffer[0] = CTRL_MWREP_HEADER;
		SSL_write(ctrl_handle->ssl, out_buffer, read_count+1);
	}

	return NULL;
}

int CTRL_run(){
	pthread_t ths;
	int ths_id;
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	int flag;
	ssize_t read_count, i;

	ths_id = pthread_create(&ths, NULL, send_multiwii_status, NULL);

	if (ths_id < 0) {
		perror("CTRL_run > send_multiwii_status");
		return 1;
	}

	pthread_detach(ths);

	flag = fcntl(ctrl_handle->ssl_fd, F_GETFL, 0);
	fcntl(ctrl_handle->ssl_fd, F_SETFL, flag | O_NONBLOCK);

	while(!is_stop_ctrl){

		read_count = SSL_read(ctrl_handle->ssl, in_buffer, UAVCTRL_BUF_SIZE);

		if(read_count <= 0) {
			usleep(100000);
			continue;
		}

		switch(in_buffer[0]){
		case CTRL_MWREQ_HEADER:
			MWSERIAL_write(mws_handle, in_buffer+1, read_count-1);
			break;
		case CTRL_FKREQ_HEADER:
			// special function
			break;
		case CTRL_SIREQ_HEADER:
			// survivor's information
			break;
		case CTRL_REP_STOP:
			is_stop_ctrl = 1;
			break;
		default:
			printf("CTRL_run > unknown : %02x\n", in_buffer[0]);
			continue;
		}


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
