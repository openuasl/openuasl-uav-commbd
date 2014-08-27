#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include "sslayer.h"
#include "ucstream.h"
#include "uavctrl.h"

void* (*ptr_dlopen)(__const char *, int) = dlopen;

void* ucstream_thread(void* argv) {

	SslHandle_t* ucs;

	while (1) {
		if (UCS_init(&ucs, argv))
			return (void*) EXIT_FAILURE;

		printf("uav camera stream connection success\n");

		if (UCS_start(ucs))
			return (void*) EXIT_FAILURE;

		printf("uav camera stream module start success\n");

		printf("uav camera stream module is running...\n");

		if (UCS_run(ucs))
			return (void*) EXIT_FAILURE;

		UCS_end(ucs);

		sleep(5);
	}

	return EXIT_SUCCESS;
}

void* ctrlcmd_thread(void* argv) {

	MWSerialHandle_t *mws;
	SslHandle_t *ctrl;

	while (1) {
		if (CTRL_init(&mws, &ctrl, argv)) {
			printf("CTRL_init fail\n");
			return EXIT_FAILURE;
		}

		printf("uav control stream module start success\n");

		if (CTRL_start(ctrl)) {
			printf("CTRL_start fail\n");
			return EXIT_FAILURE;
		}

		if (CTRL_run(mws, ctrl)) {
			printf("CTRL_run fail\n");
			return EXIT_FAILURE;
		}

		CTRL_end(mws, ctrl);

		sleep(5);
	}

	return EXIT_SUCCESS;
}

// 1 = ip
int main(int argc, char* argv[]) {

	pthread_t ucs, ctrl;
	int ucs_id, ctrl_id;

	if (argc != 2) {
		printf("openuasl_uav ip\n");
		return EXIT_FAILURE;
	}

	SSLAYER_load();

	ucs_id = pthread_create(&ucs, NULL, ucstream_thread, argv[1]);

	if (ucs_id < 0) {
		perror("main > pthread_create(ucs_id)");
		return EXIT_FAILURE;
	}

	pthread_detach(ucs);

	ctrl_id = pthread_create(&ctrl, NULL, ctrlcmd_thread, argv[1]);

	if (ctrl_id < 0) {
		perror("main > pthread_create(ctrl_id)");
		return EXIT_FAILURE;
	}

	pthread_detach(ctrl);


	getchar();


	return EXIT_SUCCESS;
}
