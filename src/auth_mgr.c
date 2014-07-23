#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/fcntl.h>

#include <openssl/sha.h>

#include "auth_mgr.h"
#include "error_handling.h"

int AUTH_get_disk_id(char* buf) {
	int fd, err;

	fd = open(AMGR_SD_CARD_CID_PATH, O_RDONLY);

	if(fd == -1){
		perror("AMGR_get_disk_id : open");
		close(fd);
		return 1;
	}

	read(fd, buf, 32);

	close(fd);

	return 0;
}

int AUTH_get_uav_id(char* diskid, char* uavid){
	SHA256_CTX ctx;
	char buf[SHA256_DIGEST_LENGTH];
	int i;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (const void *) diskid, 32);
	SHA256_Final(buf, &ctx);

	for(i=0; i<SHA256_DIGEST_LENGTH; i++){
		sprintf(uavid + (i*2), "%02x", buf[i]&0xff);
	}


	return 0;
}
