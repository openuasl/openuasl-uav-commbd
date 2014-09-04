#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <signal.h>

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
			UCS_end(&ucs);
			continue;
		}

		if (AUTH_cert_uav(&ucs)){
			printf("AUTH_cert_uav(ucs) fail\n");
			UCS_end(&ucs);
			continue;
		}

		if (UCS_run(&ucs)){
			printf("UCS_run fail\n");
			UCS_end(&ucs);
			continue;
		}

		UCS_end(&ucs);

		sleep(5);
	}

	return 0;
}



void* ctrlcmd_thread(void* argv) {

	MWSerialHandle_t mws;
	SslHandle_t ctrl;

	if (MWSC_init(&mws)) {
		perror("MWSC_init");
		return 1;
	}

	while (1) {
		if (CTRL_init(&ctrl, argv)) {
			printf("CTRL_init fail\n");
			CTRL_end(&ctrl);
			continue;
		}

		if (AUTH_cert_uav(&ctrl)) {
			printf("AUTH_cert_uav(&ctrl) fail\n");
			CTRL_end(&ctrl);
			continue;
		}

		if (CTRL_run(&mws, &ctrl)) {
			printf("CTRL_run fail\n");
			CTRL_end(&ctrl);
			continue;
		}

		CTRL_end(&ctrl);

		sleep(5);
	}

	MWSC_release(&mws);

	return 0;
}

// 1 = ip
int main(int argc, char* argv[]) {

	int pid, ret, cnt=0;

	if (argc < 2) {
		printf("usage : openuasl-uav-commbd [ip]\n");
		printf("if you want to run debug print mode, add more arguments.\n");
		printf("example : openuasl-uav-commbd 210.123.45.6\n");
		printf("debug print example : openuasl-uav-commbd 210.123.45.6 d\n");
		return EXIT_FAILURE;
	} else if (argc == 2) {
		pid = fork();

		if (pid < 0) {
			perror("fork");
			return EXIT_FAILURE;
		} else if (pid > 0) {
			// if run in parent process
			printf("child:[%d]-parent:[%d]\n", pid, getpid());
			return EXIT_SUCCESS;
		} else if (pid == 0) {
			// if run in child process
			printf("process : [%d]\n", getpid());
		}

		// next routine must run in child process.

		signal(SIGHUP, SIG_IGN);
		close(stdin);
		close(stdout);
		close(stderr);

		chdir("/");
		setsid();

		while (1) {

			if ((pid = fork()) < 0) {
				printf("fork error : restart daemon");
			} else if (pid == 0) {
				break;
			} else if (pid > 0) {
				wait(&ret);
			}

			sleep(10);
		}
	}

	{ // scope of running threads.
		pthread_t thid_ucs, thid_ctrl;
		int ucs_id, ctrl_id;

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
	}

	{ // scope of running bluetooth search module.
		BTNavHandle_t bt;
		if(BTNAV_init(&bt)){
			printf("BTNAV_init fail\n");
			return 1;
		}

		if(BTNAV_run(&bt)){
			printf("BTNAV_run fail\n");
			return 1;
		}

		BTNAV_end(&bt);
	}

	return EXIT_SUCCESS;
}
