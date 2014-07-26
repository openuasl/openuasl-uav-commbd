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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <opencv/cv.h>
#include <opencv/highgui.h> // gui

#include "ucstream_sender.h"
#include "secure_socket_layer.h"
#include "auth_mgr.h"
#include "error_handling.h"


SslHandle_t* ucs_handle;

int is_stop_imgstream;

int UCS_init(char* ip) {

	ucs_handle = (SslHandle_t*)malloc(sizeof(SslHandle_t));

	SECL_init(ucs_handle, ip, UCSTREAM_SERVER_PORT);

	SSL_connect(ucs_handle->ssl);

	return 0;
}

// thread
int UCS_start() {
	char diskid[32];
	char uavid_hex_string[66];
	char buffer[2];
	int err;

	err = AUTH_get_disk_id(diskid);
	//BE_error(err, "BE_start_imgstream : BE_get_disk_id");

	err = AUTH_get_uav_id(diskid, uavid_hex_string + 1);
	//BE_error(err, "BE_start_imgstream : BE_get_uav_id");

	uavid_hex_string[0] = UCS_REQ_SERIAL;
	uavid_hex_string[65] = '\0';

	printf("uavid : %s", uavid_hex_string + 1);

	SSL_write(ucs_handle->ssl, uavid_hex_string, 66);

	//BE_error(err, "BE_start_imgstream : IMGS_REQ_SERIAL");

	do {
		SSL_read(ucs_handle->ssl, buffer, 1);
		//BE_error(err, "BE_start_imgstream : IMGS_REP_READY");

	} while (buffer[0] != UCS_REP_READY);

	buffer[0] = UCS_REQ_START;

	SSL_write(ucs_handle->ssl, buffer, 1);
	//BE_error(err, "BE_start_imgstream : IMGS_REQ_START");

	return 0;
}

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
	int flag = fcntl(ucs_handle->ssl_fd, F_GETFL, 0 );
	fcntl(ucs_handle->ssl_fd, F_SETFL, flag | O_NONBLOCK);
}

int UCS_run() {
	IplImage* image;
	CvCapture* capture;
	pthread_t check_stop;
	CvMat* cvmat;
	char imgseg[9];
	char recvbuf[2];
	size_t tsize, ssize, total_size;
	int err;

	image = NULL;
	capture = cvCaptureFromCAM(0);

	convert_non_block_mode();

	// 1280 x 800 tablet size
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,
			UCS_CAPTURE_SIZE_WIDTH);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,
			UCS_CAPTURE_SIZE_HIGHT);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, UCS_CAPTURE_FPS);

	imgseg[0] = UCS_REQ_IMGSEG;

	itobuf(UCS_SENDBUF_SIZE, imgseg + 1);

	while (is_stop_imgstream != 1) {

		while(cvGrabFrame(capture) == 0){
			perror("UCS_run : cvGrabFrame\n");
		}

		image = cvRetrieveFrame(capture, 0);

		cvmat = cvEncodeImage(".jpg", image, jpeg_param);
		total_size = cvmat->cols * cvmat->rows;
		itobuf(total_size, imgseg + 5);

		if(SSL_write(ucs_handle->ssl, imgseg, 9) <= 0){
			perror("UCS_run : UCS_REQ_IMGSEG");
			cvReleaseMat(&cvmat);
			continue;
		}

		ssize = 0;

		while(SSL_write(ucs_handle->ssl,
				cvmat->data.ptr, total_size) <= 0);

		cvReleaseMat(&cvmat);

		usleep(50000);

		if(SSL_read(ucs_handle->ssl, recvbuf, 1) > 0)
		{
			if(recvbuf[0] == UCS_REP_STOP){
				is_stop_imgstream = 1;
			}else{
				perror("UCS_run : UCS_REP_STOP");
				break;
			}
		}

	}

	cvReleaseCapture(&capture);

	is_stop_imgstream = 0;

	return 0;
}

void UCS_end() {
	SECL_release(ucs_handle);
	free(ucs_handle);
	ucs_handle = NULL;
}

