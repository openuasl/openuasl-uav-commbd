#ifndef __OPENUASL_COMMBD_SVINFO_H__
#define __OPENUASL_COMMBD_SVINFO_H__

#include "btnav.h"
#include "sslayer.h"

typedef struct _SVINFO_t{
	char addr[BTNAV_MAX_ADDR_LENGTH];
	char name[BTNAV_MAX_NAME_LENGTH];
	int rssi;
	int distance;
}SVINFO_t;

double SVINFO_get_distance(int rssi);

void SVINFO_find_bt_callaback(
		char* addr, char* name, int rssi);



#endif /* __OPENUASL_COMMBD_SVINFO_H__ */
