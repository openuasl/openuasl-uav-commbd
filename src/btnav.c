#include "btnav.h"
#include <stdlib.h>

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
			malloc(MAX_RSP * sizeof(inquiry_info))) == NULL){
		perror("BTNAV_init > malloc inquiry_info");
		return 1;
	}

	return 0;
}

int BTNAV_run(BTNavHandle_t* btn){
	int num_rsp, i, flags = IREQ_CACHE_FLUSH;
	char addr[19] = {0,};
	char name[248] = {0,};

	num_rsp = hci_inquiry(btn->dev_id,
				8, MAX_RSP, NULL, &btn->inq, flags);

	printf("hci_inquiry : %d\n", num_rsp);

	while (num_rsp >= 0) {
		for (i = 0; i < num_rsp; i++) {
			ba2str(&(btn->inq + i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(btn->hci_fd, &(btn->inq + i)->bdaddr,
					sizeof(name), name, 0) < 0)
				strcpy(name, "[unknown]");

			printf("[%s]%s\n", addr, name);
		}

		num_rsp = hci_inquiry(btn->dev_id,
						8, MAX_RSP, NULL, &btn->inq, flags);
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
