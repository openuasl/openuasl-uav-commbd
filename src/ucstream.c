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
#include "auth.h"
#include "prtcdef.h"
#include "error_handling.h"

int UCS_init(SslHandle_t* ucs, char* ip) {

	if(SSLAYER_init(ucs, ip, UCSTREAM_SERVER_PORT)){
		perror("UCS_init > SSLAYER_init");
		return 1;
	}

	SSL_connect(ucs->ssl);

	return 0;
}

int UCS_run(SslHandle_t* ucs) {
	IplImage* image;
	CvCapture* capture;
	CvMat* cvmat;
	char imgseg[9];
	char recvbuf[2];
	size_t tsize, ssize, total_size;
	int err, flag;
	int is_stop_ucstream=0;
	static const int jpeg_param[3] = {
			CV_IMWRITE_JPEG_QUALITY,	UCS_JPEG_QUALITY, 0 };

	image = NULL;
	capture = cvCaptureFromCAM(0);

	flag = fcntl(ucs->ssl_fd, F_GETFL, 0);
	fcntl(ucs->ssl_fd, F_SETFL, flag | O_NONBLOCK);

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
			perror("UCS_run > cvGrabFrame\n");
		}

		image = cvRetrieveFrame(capture, 0);
		cvmat = cvEncodeImage(".jpg", image, jpeg_param);
		total_size = cvmat->cols * cvmat->rows;
		itobuf(total_size, imgseg + 5);

		if (SSL_write(ucs->ssl, imgseg, 9) <= 0) {
			perror("UCS_run > UCS_REQ_IMGSEG");
			cvReleaseMat(&cvmat);
			continue;
		}

		ssize = 0;

		while (SSL_write(ucs->ssl, cvmat->data.ptr, total_size) <= 0);

		cvReleaseMat(&cvmat);
		usleep(100000);

		if (SSL_read(ucs->ssl, recvbuf, 1) > 0) {
			if (recvbuf[0] == UCS_REP_STOP) {
				is_stop_ucstream = 1;
			} else {
				perror("UCS_run > UCS_REP_STOP");
				break;
			}
		}

	}

	cvReleaseCapture(&capture);
	is_stop_ucstream = 0;
	return 0;
}

void UCS_end(SslHandle_t* ucs) {
	SSLAYER_release(ucs);
}

