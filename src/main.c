#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include "secure_socket_layer.h"
#include "ucstream_sender.h"


void* (*ptr_dlopen)(__const char *, int) = dlopen;

// 1 = ip
int main(int argc, char* argv[]) {

	if(argc != 2){

		// print usage

		return EXIT_FAILURE;
	}

	if (UCS_init(argv[1])) {
		return EXIT_FAILURE;
	}

	printf("image stream connection success\n");

	if(UCS_start()){
		return EXIT_FAILURE;
	}

	printf("image stream module start success\n");

	printf("image stream module is running...\n");

	if(UCS_run()){
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
