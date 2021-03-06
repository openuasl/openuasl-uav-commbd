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

#include "mwsc.h"

int MWSC_init(MWSerialHandle_t* handle) {
	char path[16];
	int num=0;

	while(num < 20){
		sprintf(path, "%s%d",MWSERIAL_USB_SERIAL_PATH, num);

		handle->serial_fd = open(path, O_RDWR | O_NOCTTY);

		if (handle->serial_fd < 0) {
			num++;
		}else{
			break;
		}
	}

	if(num < 20){
		printf("MWSERIAL_init > %s\n", path);
	}else{
		perror("MWSERIAL_init > ");
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

ssize_t MWSC_read(MWSerialHandle_t* handle, void* buffer, size_t size){
	int read_count;
	int i;
	char* pr = buffer;

	read_count = read(handle->serial_fd, buffer, size);

	if (tcflush(handle->serial_fd, TCOFLUSH) < 0) {
		perror("MWSERIAL_read > tcflush");
		return -1;
	}

	return read_count;
}

ssize_t MWSC_write(MWSerialHandle_t* handle, void* buffer, size_t size){
	ssize_t write_count;

	write_count = write(handle->serial_fd, buffer, size);

	if (tcflush(handle->serial_fd, TCIFLUSH) < 0) {
		perror("MWSERIAL_write > tcflush");
		return 1;
	}

	return write_count;
}

int MWSC_special(SpecialFunc sf){

	switch(sf){
	case SPECIAL_UP:
		//
		break;
	}

	return 0;
}

int MWSC_release(MWSerialHandle_t* handle) {
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
