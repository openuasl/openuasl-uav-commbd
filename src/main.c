#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>

#include "sslayer.h"
#include "auth.h"
#include "ucstream.h"
#include "uavctrl.h"
#include "btnav.h"

void* (*ptr_dlopen)(__const char *, int) = dlopen;

void* ucstream_thread(void* argv) {

	SslHandle_t ucs;

	while (1) {
		if (UCS_init(&ucs, argv)){
			printf("UCS_init fail\n");
			return 1;
		}

		if (AUTH_cert_uav(&ucs)){
			printf("AUTH_cert_uav(ucs) fail\n");
			return 1;
		}

		if (UCS_run(&ucs)){
			printf("UCS_run fail\n");
			return 1;
		}

		UCS_end(&ucs);

		sleep(5);
	}

	return 0;
}

SslHandle_t ctrl;

void* ctrlcmd_thread(void* argv) {

	MWSerialHandle_t mws;

	while (1) {
		if (CTRL_init(&mws, &ctrl, argv)) {
			printf("CTRL_init fail\n");
			return 1;
		}

		if (AUTH_cert_uav(&ctrl)) {
			printf("AUTH_cert_uav(&ctrl) fail\n");
			return 1;
		}

		if (CTRL_run(&mws, &ctrl)) {
			printf("CTRL_run fail\n");
			return 1;
		}

		CTRL_end(&mws, &ctrl);

		sleep(5);
	}

	return 0;
}

// 1 = ip
int main(int argc, char* argv[]) {

	pthread_t thid_ucs, thid_ctrl;
	int ucs_id, ctrl_id;

	if (argc != 2) {
		printf("openuasl_uav ip\n");
		return EXIT_FAILURE;
	}

	SSLAYER_load();

	ucs_id = pthread_create(&thid_ucs, NULL, ucstream_thread, argv[1]);

	if (ucs_id < 0) {
		perror("main > pthread_create(ucs_id)");
		return EXIT_FAILURE;
	}

	pthread_detach(thid_ucs);

	ctrl_id = pthread_create(&thid_ctrl, NULL, ctrlcmd_thread, argv[1]);

	if (ctrl_id < 0) {
		perror("main > pthread_create(ctrl_id)");
		return EXIT_FAILURE;
	}

	pthread_detach(thid_ctrl);


	{
		BTNavHandle_t bt;
		if(BTNAV_init(&bt)){
			return 1;
		}

		if(BTNAV_run(&bt)){
			return 1;
		}

		BTNAV_end(&bt);
	}

	return EXIT_SUCCESS;
}
