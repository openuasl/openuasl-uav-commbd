#ifndef __OPENUASL_COMMBD_AUTH_MGR_H__
#define __OPENUASL_COMMBD_AUTH_MGR_H__

#include "secure_socket_layer.h"

#define AUTH_SD_CARD_CID_PATH "/sys/block/mmcblk0/device/cid"

typedef enum _AuthCommand_t{
	auth_cmd_req_serial = 0x11,
	auth_cmd_rep_ready = 0x21,
	auth_cmd_req_start = 0x12
}AuthCommand_t;

// buf size = 20
int AUTH_get_disk_id(char* buf);
int AUTH_get_uav_id(char* diskid, char* uavid);

int AUTH_cert_uav(SslHandle_t* handle);

#endif /* __OPENUASL_COMMBD_AUTH_MGR_H__ */
