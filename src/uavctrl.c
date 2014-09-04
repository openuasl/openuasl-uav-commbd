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
#include "auth.h"
#include "svinfo.h"

int CTRL_init(SslHandle_t* ctrl, char* ip) {

	if (SSLAYER_init(ctrl, ip, UAVCTRL_SERVER_PORT)) {
		perror("CTRL_init > SSLAYER_init");
		return 1;
	}

	if (SSL_connect(ctrl->ssl) != 1) {
		perror("CTRL_init > SSL_connect");
		return 1;
	}

	return 0;
}

void* send_multiwii_status(void** p){
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	ssize_t read_count, wr;
	MWSerialHandle_t* mws = (MWSerialHandle_t*)p[0];
	SslHandle_t* ctrl = (SslHandle_t*)p[1];
	int* is_stop_ctrl = (int*)p[2];

	while(!(*is_stop_ctrl)){
		read_count = MWSC_read(mws, in_buffer, UAVCTRL_BUF_SIZE);

		if(read_count < 0){
			*is_stop_ctrl = 1;
			break;
		}

		memcpy(out_buffer+1, in_buffer, read_count);
		out_buffer[0] = CTRL_MWREP_HEADER;
		wr = SSL_write(ctrl->ssl, out_buffer, read_count+1);

		if(wr <= 0){
			*is_stop_ctrl = 1;
			break;
		}
	}

	return NULL;
}

int CTRL_run(MWSerialHandle_t* mws, SslHandle_t* ctrl){
	pthread_t ths;
	int ths_id;
	char in_buffer[UAVCTRL_BUF_SIZE];
	char out_buffer[UAVCTRL_BUF_SIZE];
	int flag, is_stop_ctrl=0;
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

		if(read_count < 0){
			is_stop_ctrl = 1;
			break;
		}

		SVINFO_send_svinfos(ctrl);

		switch(in_buffer[0] & 0xFF){
		case CTRL_MWREQ_HEADER:
			MWSC_write(mws, in_buffer+1, read_count-1);
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
			is_stop_ctrl = 1;
		}
	}

	sleep(1);

	return 0;
}

void CTRL_end(SslHandle_t* ctrl) {
	SSLAYER_release(ctrl);
}
