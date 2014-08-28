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
#include "authmgr.h"

int CTRL_init(MWSerialHandle_t* mws, SslHandle_t* ctrl, char* ip) {

	if(MWSERIAL_init(mws)){
		perror("CTRL_init > MWSERIAL_init");
		return 1;
	}

	if(SSLAYER_init(ctrl, ip, UAVCTRL_SERVER_PORT)){
		perror("CTRL_init > SSLAYER_init");
		return 1;
	}

	SSL_connect(ctrl->ssl);

	return 0;
}

void* send_multiwii_status(void** p){
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	ssize_t read_count;
	MWSerialHandle_t* mws = (MWSerialHandle_t*)p[0];
	SslHandle_t* ctrl = (SslHandle_t*)p[1];
	int* is_stop_ctrl = (int*)p[2];

	while(!(*is_stop_ctrl)){
		read_count = MWSERIAL_read(mws, in_buffer, UAVCTRL_BUF_SIZE);

		if(read_count == -1)	return NULL;

		memcpy(out_buffer+1, in_buffer, read_count);
		out_buffer[0] = CTRL_MWREP_HEADER;
		SSL_write(ctrl->ssl, out_buffer, read_count+1);
	}

	return NULL;
}

int CTRL_run(MWSerialHandle_t* mws, SslHandle_t* ctrl){
	pthread_t ths;
	int ths_id;
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	int flag, is_stop_ctrl;
	ssize_t read_count, i;
	void* thrp[3];
	thrp[0] = mws;
	thrp[1] = ctrl;
	thrp[2] = &is_stop_ctrl;

	ths_id = pthread_create(&ths, NULL, send_multiwii_status, thrp);

	if (ths_id < 0) {
		perror("CTRL_run > pthread_create");
		return 1;
	}

	pthread_detach(ths);

	while(!is_stop_ctrl){

		read_count = SSL_read(ctrl->ssl, in_buffer, UAVCTRL_BUF_SIZE);

		switch(in_buffer[0] & 0xFF){
		case CTRL_MWREQ_HEADER:
			MWSERIAL_write(mws, in_buffer+1, read_count-1);
			break;
		case CTRL_FKREQ_HEADER:
			// special function
			// read 한 값에 따라 SPECIAL_UP
			// MWSERIAL_special();
			break;
		case CTRL_SIREQ_HEADER:
			// survivor's information
			break;
		case CTRL_REP_STOP:
			is_stop_ctrl = 1;
			break;
		default:
			printf("CTRL_run > unknown : %02x\n", in_buffer[0] & 0xFF);
			continue;
		}
	}

	return 0;
}

void CTRL_end(MWSerialHandle_t* mws, SslHandle_t* ctrl) {
	MWSERIAL_release(mws);
	SSLAYER_release(ctrl);
}
