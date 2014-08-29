/*
 * secure_socket_layer.c
 *
 *  Created on: 2014. 7. 3.
 *      Author: root
 */
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "sslayer.h"
#include "error_handling.h"

void SSLAYER_load() {
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
}

int SSLAYER_init(SslHandle_t* handle, char* ip, int port) {
	int err;
	struct sockaddr_in sa;
	const SSL_METHOD *meth;

	meth = SSLv3_client_method();
	handle->ctx = SSL_CTX_new(meth);

	handle->ssl_fd = socket(AF_INET, SOCK_STREAM, 0);
	//BE_error(ssl_fd, "BE_init_ssl : socket");

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ip);
	sa.sin_port = htons(port);

	err = connect(handle->ssl_fd, (struct sockaddr*) &sa, sizeof(sa));
	//BE_error(err, "BE_init_ssl : connect");

	handle->ssl = SSL_new(handle->ctx);
	SSL_set_fd(handle->ssl, handle->ssl_fd);

	return 0;
}

void SSLAYER_release(SslHandle_t* handle) {
	SSL_shutdown(handle->ssl);
	close(handle->ssl_fd);
	SSL_free(handle->ssl);
	SSL_CTX_free(handle->ctx);
	handle->ssl = NULL;
	handle->ctx = NULL;
}
