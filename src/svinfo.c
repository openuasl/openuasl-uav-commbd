#include "svinfo.h"
#include "btnav.h"
#include "uavctrl.h"
#include "prtcdef.h"
#include <math.h>
#include <string.h>


double SVINFO_get_distance(int rssi){
	static const double p = -1; // m/dbm
	double r;

	if(rssi == 0)
		return -1.0;

	r = (double)rssi * 1.0 / p;

	if((double)r < 1.0)
		r = pow(r, 10);
	else{
		r = (0.89976) * pow(r,7.7095) + 0.111;
	}

	return r;
}

int number_of_svinfo;
SVINFO_t svinfo[BTNAV_MAX_RSP];

void SVINFO_find_bt_callaback(
		char* addr, char* name, int rssi){
	strcpy(svinfo[number_of_svinfo].addr, addr);
	strcpy(svinfo[number_of_svinfo].name, name);
	svinfo[number_of_svinfo].rssi = rssi;
	svinfo[number_of_svinfo++].distance = SVINFO_get_distance(rssi);
}

void SVINFO_send_svinfos(SslHandle_t* ctrl){
	char buffer[UAVCTRL_BUF_SIZE];
	int i, name_len;

	if(!number_of_svinfo)
		return;

	for (i = 0; i < number_of_svinfo; i++) {
		buffer[0] = CTRL_SIREP_HEADER;
		memcpy(buffer + 1, svinfo[i].addr, BTNAV_MAX_ADDR_LENGTH);

		name_len = strlen(svinfo[i].name);
		itobuf(name_len, buffer + 20);
		memcpy(buffer + 24, svinfo[i].name, name_len);

		itobuf(svinfo[i].rssi, buffer + 24 + name_len);
		itobuf(svinfo[i].distance, buffer + 28 + name_len);

		printf("%p %p %d\n", ctrl->ssl, ctrl->ctx, ctrl->ssl_fd);

		if (ctrl->ssl != NULL)
			SSL_write(ctrl->ssl, buffer, 32 + name_len);

		printf("%s %s %d\n", svinfo[i].addr, svinfo[i].name, svinfo[i].rssi);
	}

	number_of_svinfo = 0;
}

/*
rollingRssi = (beacon.rssi * kFilteringFactor) + (rollingRssi * (1.0 - kFilteringFactor));

- (double)calculateAccuracyWithRSSI:(double)rssi {
    //formula adapted from David Young's Radius Networks Android iBeacon Code
    if (rssi == 0) {
        return -1.0; // if we cannot determine accuracy, return -1.
    }


    double txPower = -70;
    double ratio = rssi*1.0/txPower;
    if (ratio < 1.0) {
        return pow(ratio,10);
    }
    else {
        double accuracy =  (0.89976) * pow(ratio,7.7095) + 0.111;
        return accuracy;
    }
}


 < distance conversion from beacons >

float xa = beacon1.locationX;
float ya = beacon1.locationY;
float xb = beacon2.locationX;
float yb = beacon2.locationY;
float xc = beacon3.locationX;
float yc = beacon3.locationY;
float ra = beacon1.filteredDistance;
float rb = beacon2.filteredDistance;
float rc = beacon3.filteredDistance;

float S = (pow(xc, 2.) - pow(xb, 2.) + pow(yc, 2.) - pow(yb, 2.) + pow(rb, 2.) - pow(rc, 2.)) / 2.0;
float T = (pow(xa, 2.) - pow(xb, 2.) + pow(ya, 2.) - pow(yb, 2.) + pow(rb, 2.) - pow(ra, 2.)) / 2.0;
float y = ((T * (xb - xc)) - (S * (xb - xa))) / (((ya - yb) * (xb - xc)) - ((yc - yb) * (xb - xa)));
float x = ((y * (ya - yb)) - T) / (xb - xa);

CGPoint point = CGPointMake(x, y);
return point;
 * */
