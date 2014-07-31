#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#include "multiwii_serial_connection.h"

int MWSERIAL_init(MWSerialHandle_t* handle) {
	char buf;

	handle->serial_fd = open(MWSERIAL_USB_SERIAL_PATH,
	O_RDWR | O_NOCTTY, O_NONBLOCK);

	if (handle->serial_fd < 0) {
		perror("SERIAL_init > open "MWSERIAL_USB_SERIAL_PATH);
		return 1;
	}

	bzero(&handle->tio, sizeof(handle->tio));

	handle->tio.c_cflag =
	MWSERIAL_BAUDRATE | CS8 |
	CLOCAL | CREAD;			// control mode

	handle->tio.c_iflag = ICRNL; 	// input mode
	handle->tio.c_oflag = 0;			// output mode
	handle->tio.c_lflag = 0;			// local mode

	// special functional character(non-canonical)
	handle->tio.c_cc[VTIME] = 0;
	handle->tio.c_cc[VMIN] = 1;

	if (tcsetattr(handle->serial_fd, TCSANOW, &handle->tio) < 0) {
		perror("SERIAL_init > tcsetattr");
		return 1;
	}

	if (tcflush(handle->serial_fd, TCIOFLUSH) < 0) {
		perror("SERIAL_init > tcflush");
		return 1;
	}

	return 0;
}
/*	internal functions area	start ****************************/
void set_checksum(MultiWiiPacket_t* p){
	int i;

	p->checksum = 0;
	p->checksum ^= p->payload_length;
	p->checksum ^= p->cmd_type;

	for(i=0; i<p->payload_length; i++)
		p->checksum ^= p->payload[i];
}
void get_buffer(MultiWiiPacket_t* p, char* dst, int* len){
	int i;

	*len = strlen(p->header);
	strcpy(dst, p->header);
	dst[(*len)++] = p->payload_length;
	dst[(*len)++] = p->cmd_type;

	for(i=0; i<p->payload_length; i++)
		dst[(*len)++] = p->payload[i];

	dst[(*len)++] = p->checksum;
	dst[(*len)++] = '\0';
}
/*	internal functions area	end ******************************/

int MWSERIAL_start(MWSerialHandle_t* handle) {
	MultiWiiPacket_t p;
	char buf[7];
	int len;

	strcpy(p.header, MWSERIAL_REQ_HEADER);
	p.payload_length = 0;
	p.cmd_type = MWCMD_Attitude;
	p.payload = NULL;
	set_checksum(&p);

	get_buffer(&p, buf, &len);

	if(write(handle->serial_fd, buf, len) < 0){
		perror("MWSERIAL_start > write");
		return 1;
	}

	return 0;
}

int MWSERIAL_release(MWSerialHandle_t* handle) {
	if (tcdrain(handle->serial_fd) < 0) {
		perror("SERIAL_release > tcdrain");
		return 1;
	}

	if (close(handle->serial_fd) < 0) {
		perror("SERIAL_release > close");
		return 1;
	}

	return 0;
}
