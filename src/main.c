#include <stdio.h>
#include "secure_socket_layer.h"
#include "ucstream_sender.h"
#include <pthread.h>
#include <dlfcn.h>

void* (*ptr_dlopen)(__const char *, int) = dlopen;

// odroid client main
int main(void) {

//	if (BE_init_ssl()) {
//		return 1;
//	}
//
//	printf("ssl connection success\n");

	if (BE_init_imgstream()) {
		return 2;
	}

	printf("image stream connection success\n");

	if(BE_start_imgstream()){
		return 3;
	}

	printf("image stream module start success\n");

	printf("image stream module is running...\n");

	if(BE_run_imgstream()){
		return 4;
	}

	return 0;
}
