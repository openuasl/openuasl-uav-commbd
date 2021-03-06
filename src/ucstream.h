#ifndef __OPENUASL_COMMBD_UCSTREAM_H__
#define __OPENUASL_COMMBD_UCSTREAM_H__
#include "sslayer.h"

#define UCSTREAM_SERVER_PORT	54321

#define UCS_REQ_IMGSEG		0x13
#define UCS_REP_STOP			0x22

#define UCS_CAPTURE_SIZE_WIDTH		640
#define UCS_CAPTURE_SIZE_HIGHT		400
#define UCS_CAPTURE_FPS				20
#define UCS_SENDBUF_SIZE			4096
#define UCS_JPEG_QUALITY			50

int UCS_init(SslHandle_t* ucs, char* ip);
int UCS_run(SslHandle_t* ucs);
void UCS_end(SslHandle_t* ucs);


#endif /* __OPENUASL_COMMBD_UCSTREAM_H__ */
