#ifndef UCSTREAM_SENDER_H_
#define UCSTREAM_SENDER_H_

#include "commbd.h"

#define UCSTREAM_SERVER_IP		"210.118.69.65"
#define UCSTREAM_SERVER_PORT	54321

#define UCS_REQ_SERIAL		0x11
#define UCS_REQ_START		0x12
#define UCS_REQ_IMGSEG		0x13

#define UCS_REP_READY		0x21
#define UCS_REP_STOP		0x22

#define UCS_CAPTURE_SIZE_WIDTH	640
#define UCS_CAPTURE_SIZE_HIGHT	400
#define UCS_SENDBUF_SIZE			4096
#define UCS_JPEG_QUALITY			70

int BE_init_ucstream_sender();
int BE_start_ucs_sender();
int BE_run_ucs_sender();
void BE_end_ucs_sender();


#endif /* UCSTREAM_SENDER_H_ */
