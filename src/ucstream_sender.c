/*
 * image_transmission.c
 *
 *  Created on: 2014. 7. 3.
 *      Author: root
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#include <opencv/cv.h>
#include <opencv/highgui.h> // gui

#include "ucstream_sender.h"
#include "auth_mgr.h"
#include "error_handling.h"


int ucsock;

int is_stop_imgstream;

int UCS_init() {
	struct sockaddr_in serv_addr;
	int err;

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(UCSTREAM_SERVER_IP);
	serv_addr.sin_port = htons(UCSTREAM_SERVER_PORT);

	ucsock = socket(PF_INET, SOCK_STREAM, 0);
	//BE_syserr(ucsock, -1, "BE_init_ucstream : socket");

	err = connect(ucsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	//BE_syserr(err, -1, "BE_init_ucstream : connect");

	return 0;
}

// thread
int UCS_start() {
	char diskid[32];
	char uavid_hex_string[66];
	char buffer[2];
	int err;

	err = AMGR_get_disk_id(diskid);
	//BE_error(err, "BE_start_imgstream : BE_get_disk_id");

	err = AMGR_get_uav_id(diskid, uavid_hex_string + 1);
	//BE_error(err, "BE_start_imgstream : BE_get_uav_id");

	uavid_hex_string[0] = UCS_REQ_SERIAL;
	uavid_hex_string[65] = '\0';

	printf("uavid : %s", uavid_hex_string + 1);

	err = send(ucsock, uavid_hex_string, 66, 0);
	//BE_error(err, "BE_start_imgstream : IMGS_REQ_SERIAL");

	do {
		err = recv(ucsock, buffer, 1, 0);
		//BE_error(err, "BE_start_imgstream : IMGS_REP_READY");

	} while (buffer[0] != UCS_REP_READY);

	buffer[0] = UCS_REQ_START;
	err = send(ucsock, buffer, 1, 0);
	//BE_error(err, "BE_start_imgstream : IMGS_REQ_START");

	return 0;
}

void* wait_for_stop(void*);

int jpeg_param[3] = {
		CV_IMWRITE_JPEG_QUALITY,
		UCS_JPEG_QUALITY,
		0
};

void itobuf(int i,char* buf){
	buf[0] = 0xff & (i >> 24);
	buf[1] = 0xff & (i >> 16);
	buf[2] = 0xff & (i >> 8);
	buf[3] = 0xff & (i);
}

void convert_non_block_mode(){
	int flag = fcntl( ucsock, F_GETFL, 0 );
	fcntl(ucsock, F_SETFL, flag | O_NONBLOCK);
}

int UCS_run() {
	IplImage* image;
	CvCapture* capture;
	pthread_t check_stop;
	CvMat* cvmat;
	char imgseg[9];
	char recvbuf[2];
	size_t rsize, ssize, total_ssize;
	int err;

	image = NULL;
	capture = cvCaptureFromCAM(0);

	convert_non_block_mode();

	//err = pthread_create(&check_stop, NULL, wait_for_stop, NULL);
	//BE_error(err, "BE_run_imgstream : pthread_create");

	// 1280 x 800 tablet size
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,
			UCS_CAPTURE_SIZE_WIDTH);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,
			UCS_CAPTURE_SIZE_HIGHT);

//	cvNamedWindow("odroid capture", 0);
//	cvResizeWindow("odroid capture",
//			IMGS_CAPTURE_SIZE_WIDTH, IMGS_CAPTURE_SIZE_HIGHT);

	imgseg[0] = UCS_REQ_IMGSEG;

	itobuf(UCS_SENDBUF_SIZE, imgseg + 1);

	while (is_stop_imgstream != 1) {
		if (cvGrabFrame(capture) == 0) {
			perror("BE_start_imgstream : cvGrabFrame\n");
			return 1;
		}

		image = cvRetrieveFrame(capture, 0);
		cvmat = cvEncodeImage(".jpg", image, jpeg_param);
		itobuf(cvmat->cols * cvmat->rows, imgseg + 5);

		err = send(ucsock, imgseg, 9, 0);
		if(err){
			perror("BE_start_imgstream : send imgseg");
			break;
		}

		total_ssize = 0;
		while (rsize) {
			ssize = send(ucsock, cvmat->data.ptr + total_ssize,
					UCS_SENDBUF_SIZE, 0);

			if (ssize > 0) {
				total_ssize += ssize;
				rsize -= ssize;
			}else if(ssize == -1){
				perror("UCS_run");
				break;
			}
		}

		cvReleaseMat(&cvmat);

		if(recv(ucsock, recvbuf, 1, 0) > 0){
			if(recvbuf[0] == UCS_REP_STOP){
				is_stop_imgstream = 1;
			}else{
				perror("UCS_run");
				break;
			}
		}

//		cvShowImage("odroid capture", image);
//		if (cvWaitKey(10) >= 0)
//					break;
	}

	cvReleaseCapture(&capture);

//	cvDestroyWindow("odroid capture");

	is_stop_imgstream = 0;

	pthread_detach(check_stop);

	return 0;
}

void BE_end_imgstream() {

}

void* wait_for_stop(void* p) {
	char buf[2];
	int err;

	do {

		err = recv(ucsock, buf, 1, 0);
		if(err < 0){
			perror("BE_start_imgstream : wait_for_stop");
			break;
		}

		if (buf[0] == UCS_REP_STOP)
			is_stop_imgstream = 1;

	} while (is_stop_imgstream != 1);

	return NULL;
}
