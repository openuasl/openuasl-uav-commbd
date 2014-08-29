#ifndef _OPENUASL_COMMBD_SSLAYER_H_
#define _OPENUASL_COMMBD_SSLAYER_H_

#include <openssl/ssl.h>

#define INTERMEDIATE_SERVER_PORT	12345

typedef struct _SslHandle_t{
	SSL* ssl;
	SSL_CTX* ctx;
	int ssl_fd;
}SslHandle_t;

void SSLAYER_load();

int SSLAYER_init(SslHandle_t* handle, char* ip, int port);
void SSLAYER_release(SslHandle_t* handle);



#endif /* _OPENUASL_COMMBD_SSLAYER_H_ */
