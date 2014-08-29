#ifndef __OPENUASL_COMMBD_BTNAV_H__
#define __OPENUASL_COMMBD_BTNAV_H__

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define BTNAV_MAX_ADDR_LENGTH		19
#define BTNAV_MAX_NAME_LENGTH		248
#define BTNAV_MAX_RSP				255

typedef struct _BTNavHandle_t{
	inquiry_info* inq;
	int dev_id;
	int hci_fd;
}BTNavHandle_t;

int BTNAV_init(BTNavHandle_t* btn);
int BTNAV_run(BTNavHandle_t* btn);
void BTNAV_end(BTNavHandle_t* btn);

#endif /* __OPENUASL_COMMBD_BTNAV_H__ */
