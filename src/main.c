#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dlfcn.h>
#include "ucstream.h"
#include "uavctrl.h"

void* (*ptr_dlopen)(__const char *, int) = dlopen;

void* ucstream_thread(void* argv) {

	if (UCS_init(argv))
		return (void*) EXIT_FAILURE;

	printf("uav camera stream connection success\n");

	if (UCS_start())
		return (void*) EXIT_FAILURE;

	printf("uav camera stream module start success\n");

	printf("uav camera stream module is running...\n");

	if (UCS_run())
		return (void*) EXIT_FAILURE;

	return EXIT_SUCCESS;
}

// 1 = ip
int main(int argc, char* argv[]) {

	pthread_t ucs;
	int ucs_id;

	if (argc != 2) {
		printf("openuasl_uav ip\n");
		return EXIT_FAILURE;
	}

	ucs_id = pthread_create(&ucs, NULL, ucstream_thread, argv[1]);

	if (ucs_id < 0) {
		perror("creating ucstream thread is error occurred");
		return EXIT_FAILURE;
	}

	pthread_detach(ucs);

	if (CTRL_init(argv[1]))
	{
		printf("CTRL_init fail\n");
		return EXIT_FAILURE;
	}

	printf("uav control stream module start success\n");

	if(CTRL_start())
		return EXIT_FAILURE;

	if(CTRL_run()){
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
