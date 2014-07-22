/*
 * commbd.c
 *
 *  Created on: 2014. 7. 5.
 *      Author: aroom1
 */

#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/fcntl.h>

#include <openssl/sha.h>

#include "commbd.h"
#include "error_handling.h"

int BE_get_disk_id(char* buf) {
	int fd, err;

	fd = open("/sys/block/mmcblk0/device/cid", O_RDONLY);
	BE_error(fd, "BE_get_sda_id : open");

	read(fd, buf, 32);

	close(fd);

	return 0;
}

int BE_get_uav_id(char* diskid, char* uavid){
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
