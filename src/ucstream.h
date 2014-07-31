#ifndef __OPENUASL_COMMBD_UCSTREAM_H__
#define __OPENUASL_COMMBD_UCSTREAM_H__


#define UCSTREAM_SERVER_PORT	54321

#define UCS_REQ_SERIAL		0x11
#define UCS_REQ_START		0x12
#define UCS_REQ_IMGSEG		0x13

#define UCS_REP_READY		0x21
#define UCS_REP_STOP			0x22

#define UCS_CAPTURE_SIZE_WIDTH		1280
#define UCS_CAPTURE_SIZE_HIGHT		800
#define UCS_CAPTURE_FPS				20
#define UCS_SENDBUF_SIZE			4096
#define UCS_JPEG_QUALITY			50



int UCS_init(char* ip);
int UCS_start();
int UCS_run();
void UCS_end();


#endif /* __OPENUASL_COMMBD_UCSTREAM_H__ */