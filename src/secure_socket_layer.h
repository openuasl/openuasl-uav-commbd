/*
 * secure_socket_layer.h
 *
 *  Created on: 2014. 7. 3.
 *      Author: root
 */

#ifndef _OPENUASL_COMMBD_SECURE_SOCKET_LAYER_H_
#define _OPENUASL_COMMBD_SECURE_SOCKET_LAYER_H_

#define INTERMEDIATE_SERVER_IP		"210.118.69.65"
#define INTERMEDIATE_SERVER_PORT	12345
#include <openssl/ssl.h>

typedef struct _SslHandle_t{
	SSL* ssl;
	SSL_CTX* ctx;
	int ssl_fd;
}SslHandle_t;

int SSLAYER_init(SslHandle_t* handle, char* ip, int port);
void SSLAYER_release(SslHandle_t* handle);



#endif /* _OPENUASL_COMMBD_SECURE_SOCKET_LAYER_H_ */
