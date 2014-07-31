#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/fcntl.h>

#include <openssl/sha.h>

#include "auth_mgr.h"
#include "error_handling.h"

int AUTH_get_disk_id(char* buf) {
	int fd, err;

	fd = open(AUTH_SD_CARD_CID_PATH, O_RDONLY);

	if (fd == -1) {
		perror("AMGR_get_disk_id : open");
		close(fd);
		return 1;
	}

	read(fd, buf, 32);

	close(fd);

	return 0;
}

int AUTH_get_uav_id(char* diskid, char* uavid) {
	SHA256_CTX ctx;
	char buf[SHA256_DIGEST_LENGTH];
	int i;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (const void *) diskid, 32);
	SHA256_Final(buf, &ctx);

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(uavid + (i * 2), "%02x", buf[i] & 0xff);
	}

	return 0;
}

int AUTH_cert_uav(SslHandle_t* handle) {
	char diskid[32];
	char uavid_hex_string[66];
	char buffer[2];
	int err;

	err = AUTH_get_disk_id(diskid);
	if(err)	return err;

	err = AUTH_get_uav_id(diskid, uavid_hex_string + 1);
	if(err)	return err;

	uavid_hex_string[0] = auth_cmd_req_serial;
	uavid_hex_string[65] = '\0';

	printf("uavid : %s\n", uavid_hex_string + 1);

	SSL_write(handle->ssl, uavid_hex_string, 66);

	//BE_error(err, "BE_start_imgstream : IMGS_REQ_SERIAL");

	do {
		SSL_read(handle->ssl, buffer, 1);
		//BE_error(err, "BE_start_imgstream : IMGS_REP_READY");

	} while (buffer[0] != auth_cmd_rep_ready);

	buffer[0] = auth_cmd_req_start;

	SSL_write(handle->ssl, buffer, 1);

	return 0;
}
