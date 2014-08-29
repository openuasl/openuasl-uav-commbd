#ifndef __OPENUASL_COMMBD_MWSC_H__
#define __OPENUASL_COMMBD_MWSC_H__

#include <termios.h>

//  B300, B1200, B4800, B9600, B19200, B38400, B57600, B115200
#define MWSERIAL_BAUDRATE B115200
#define MWSERIAL_USB_SERIAL_PATH "/dev/ttyUSB"

// multiwii request message header
#define MWSERIAL_REQ_HEADER		"$M<"
#define MWSERIAL_REP_HEADER		"$M>"
#define MWSERIAL_ERR_HEADER		"$M!"

typedef enum _MultiWiiCommand_t {
	MWCMD_Attitude = 108
} _MultiWiiCommand_t;

typedef struct _MWSerialHandle_t {
	struct termios tio;
	int serial_fd;
} MWSerialHandle_t;

typedef struct _MultiWiiPacket_t {
	char header[4];
	unsigned char payload_length;
	unsigned char cmd_type;
	char* payload;
	unsigned char checksum;
} MultiWiiPacket_t;

typedef enum _SpecialFunc{
	SPECIAL_UP,
	SPECIAL_DOWN,
	SPECIAL_MOVE_FRONT,
	SPECIAL_MOVE_BACK,
	SPECIAL_MOVE_LEFT,
	SPECIAL_MOVE_RIGHT
}SpecialFunc;

int MWSC_init(MWSerialHandle_t* handle);

ssize_t MWSC_read(MWSerialHandle_t* handle, void* buffer, size_t size);
ssize_t MWSC_write(MWSerialHandle_t* handle, void* buffer, size_t size);

int MWSC_special(SpecialFunc sf);

int MWSC_release(MWSerialHandle_t* handle);

#endif /* __OPENUASL_COMMBD_MWSC_H__ */
