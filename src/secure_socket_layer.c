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

#include "secure_socket_layer.h"
#include "error_handling.h"

int SECL_init(SslHandle_t* handle, char* ip, int port) {
	int err;
	struct sockaddr_in sa;
	const SSL_METHOD *meth;

	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	meth = SSLv3_client_method();
	handle->ctx = SSL_CTX_new(meth);

	handle->ssl_fd = socket(AF_INET, SOCK_STREAM, 0);
	//BE_error(ssl_fd, "BE_init_ssl : socket");

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(ip);
	sa.sin_port = htons(port);

	err = connect(handle->ssl_fd,
			(struct sockaddr*) &sa, sizeof(sa));
	//BE_error(err, "BE_init_ssl : connect");

	handle->ssl = SSL_new(handle->ctx);
	SSL_set_fd(handle->ssl, handle->ssl_fd);

	return 0;
}

void SECL_release(SslHandle_t* handle){
	SSL_shutdown(handle->ssl);
	close(handle->ssl_fd);
	SSL_free(handle->ssl);
	SSL_CTX_free(handle->ctx);
}

/*
int main(void)
{
    int err;
    int sd;
    struct sockaddr_in sa;


    X509 *server_cert;
    char *str;
    char buf[4096];
    SSL_METHOD *meth;

    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();
    meth = SSLv3_client_method();
    ctx = SSL_CTX_new(meth);

    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    sa.sin_port = htons(SERVER_PORT_NUMBER);

    err = connect(sd, (struct sockaddr*)&sa, sizeof(sa));

    //
    ssl = SSL_new(ctx);  // 세션을 위한 자원을 할당받는다.

    SSL_set_fd(ssl, sd);
    err = SSL_connect(ssl); // 기존의 connect() 함수 대신 사용하여 서버로 접속한다.

    // Following two steps are optional and not required for data exchange to be successful.


    // Get server’s certificate (note: beware of dynamic allocation) – opt

    server_cert = SSL_get_peer_certificate(ssl);


    str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
    OPENSSL_free(str);


    str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
    OPENSSL_free(str);


    X509_free(server_cert);

    //
    err = SSL_write(ssl, "Hello World!", strlen("Hello World!"));

    err = SSL_read(ssl, buf, sizeof(buf)-1);

    buf[err] = '\0';
    SSL_shutdown(ssl);    // SSL로 연결된 접속을 해지한다.

    ///
    close(sd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}*/
