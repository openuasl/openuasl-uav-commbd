#include <stdio.h>
#include "secure_socket_layer.h"
#include "ucstream_sender.h"
#include <pthread.h>
#include <dlfcn.h>

void* (*ptr_dlopen)(__const char *, int) = dlopen;

// odroid client main
int main(int argc, char* argv[]) {

//	if (BE_init_ssl()) {
//		return 1;
//	}
//
//	printf("ssl connection success\n");

	if (UCS_init()) {
		return 2;
	}

	printf("image stream connection success\n");

	if(UCS_start()){
		return 3;
	}

	printf("image stream module start success\n");

	printf("image stream module is running...\n");

	if(BE_run_imgstream()){
		return 4;
	}

	return 0;
}
