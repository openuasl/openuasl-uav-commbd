#include "btnav.h"
#include "svinfo.h"
#include <stdlib.h>
#include <sys/ioctl.h>

int BTNAV_init(BTNavHandle_t* btn){
	if((btn->dev_id = hci_get_route(NULL)) < 0){
		perror("BTNAV_init > hci_get_route");
		return 1;
	}

	if((btn->hci_fd = hci_open_dev(btn->dev_id)) < 0){
		perror("BTNAV_init > hci_get_route");
		return 1;
	}

	if((btn->inq = (inquiry_info*)
			malloc(BTNAV_MAX_RSP * sizeof(inquiry_info))) == NULL){
		perror("BTNAV_init > malloc inquiry_info");
		return 1;
	}

	return 0;
}

int BTNAV_run(BTNavHandle_t* btn){
	int num_rsp, i, flags = IREQ_CACHE_FLUSH;
	char addr[BTNAV_MAX_ADDR_LENGTH] = {0,},
			name[BTNAV_MAX_NAME_LENGTH] = {0,};
	struct hci_conn_info_req conn;
	unsigned int ptype =
			HCI_DM1 | HCI_DM3 | HCI_DM5 |
			HCI_DH1 | HCI_DH3 | HCI_DH5;
	uint64_t handle;
	int8_t rssi;
	double dist;

	num_rsp = hci_inquiry(btn->dev_id,
				8, BTNAV_MAX_RSP, NULL, &btn->inq, flags);

	printf("hci_inquiry : %d\n", num_rsp);

	while (num_rsp >= 0) {
		for (i = 0; i < num_rsp; i++) {
			ba2str(&(btn->inq + i)->bdaddr, addr);
			memset(name, 0, sizeof(name));

			if (hci_create_connection(btn->hci_fd,
					&(btn->inq + i)->bdaddr,	htobs(ptype),
					(btn->inq + i)->clock_offset, 0x01, &handle, 0) < 0) {
				perror("BTNAV_run > hci_create_connection");
				continue;
			}

			bacpy(&conn.bdaddr, &(btn->inq + i)->bdaddr);
			conn.type = ACL_LINK;

			if (ioctl(btn->hci_fd, HCIGETCONNINFO,
					(unsigned long)&conn) < 0) {
				perror("BTNAV_run > ioctl");
				continue;
			}

			if (hci_read_rssi(btn->hci_fd, htobs(conn.conn_info->handle),
					&rssi, 1000) < 0) {
				perror("BTNAV_run > hci_read_rssi");
				continue;
			}

			if (hci_read_remote_name(btn->hci_fd, &(btn->inq + i)->bdaddr,
					sizeof(name), name, 0) < 0)
				strcpy(name, "[unknown]");

			SVINFO_find_bt_callaback(addr, name, rssi);
		}

		num_rsp = hci_inquiry(btn->dev_id,
						8, BTNAV_MAX_RSP, NULL, &btn->inq, flags);
		printf("hci_inquiry : %d\n", num_rsp);
	}

	if (num_rsp < 0){
		perror("BTNAV_run > hci_inquiry");
		return 1;
	}

	return 0;
}

void BTNAV_end(BTNavHandle_t* btn){
	close(btn->hci_fd);
	free(btn->inq);
}
