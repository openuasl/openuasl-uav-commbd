#ifndef __OPENUASL_COMMBD_AUTH_MGR_H__
#define __OPENUASL_COMMBD_AUTH_MGR_H__

#define AMGR_SD_CARD_CID_PATH "/sys/block/mmcblk0/device/cid"

// buf size = 20
int AUTH_get_disk_id(char* buf);
int AUTH_get_uav_id(char* diskid, char* uavid);

#endif /* __OPENUASL_COMMBD_AUTH_MGR_H__ */
