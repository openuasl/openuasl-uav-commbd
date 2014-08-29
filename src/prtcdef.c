#include "prtcdef.h"

void itobuf(int i, char* buf) {
	buf[0] = 0xff & (i >> 24);
	buf[1] = 0xff & (i >> 16);
	buf[2] = 0xff & (i >> 8);
	buf[3] = 0xff & (i);
}
