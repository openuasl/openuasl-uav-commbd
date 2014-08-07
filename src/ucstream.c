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

#include "ucstream.h"
#include "secure_socket_layer.h"
#include "auth_mgr.h"
#include "error_handling.h"

SslHandle_t* ucs_handle;

int is_stop_ucstream;

int UCS_init(char* ip) {

	ucs_handle = (SslHandle_t*) malloc(sizeof(SslHandle_t));
	SSLAYER_init(ucs_handle, ip, UCSTREAM_SERVER_PORT);
	SSL_connect(ucs_handle->ssl);

	return 0;
}

// thread
int UCS_start() {
	return AUTH_cert_uav(ucs_handle);
}

int jpeg_param[3] = { CV_IMWRITE_JPEG_QUALITY,
UCS_JPEG_QUALITY, 0 };

void itobuf(int i, char* buf) {
	buf[0] = 0xff & (i >> 24);
	buf[1] = 0xff & (i >> 16);
	buf[2] = 0xff & (i >> 8);
	buf[3] = 0xff & (i);
}

void convert_non_block_mode() {
	int flag = fcntl(ucs_handle->ssl_fd, F_GETFL, 0);
	fcntl(ucs_handle->ssl_fd, F_SETFL, flag | O_NONBLOCK);
}

int UCS_run() {
	IplImage* image;
	CvCapture* capture;
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

	while (!is_stop_ucstream) {

		while (!cvGrabFrame(capture)) {
			perror("UCS_run : cvGrabFrame\n");
		}

		image = cvRetrieveFrame(capture, 0);

		cvmat = cvEncodeImage(".jpg", image, jpeg_param);
		total_size = cvmat->cols * cvmat->rows;
		itobuf(total_size, imgseg + 5);

		if (SSL_write(ucs_handle->ssl, imgseg, 9) <= 0) {
			perror("UCS_run : UCS_REQ_IMGSEG");
			cvReleaseMat(&cvmat);
			continue;
		}

		ssize = 0;

		while (SSL_write(ucs_handle->ssl, cvmat->data.ptr, total_size) <= 0);

		cvReleaseMat(&cvmat);

		usleep(100000);

		if (SSL_read(ucs_handle->ssl, recvbuf, 1) > 0) {
			if (recvbuf[0] == UCS_REP_STOP) {
				is_stop_ucstream = 1;
			} else {
				perror("UCS_run : UCS_REP_STOP");
				break;
			}
		}

	}

	cvReleaseCapture(&capture);
	is_stop_ucstream = 0;
	return 0;
}

void UCS_end() {
	SSLAYER_release(ucs_handle);
	free(ucs_handle);
	ucs_handle = NULL;
}

