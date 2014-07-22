/*
 * secure_socket_layer.h
 *
 *  Created on: 2014. 7. 3.
 *      Author: root
 */

#ifndef SECURE_SOCKET_LAYER_H_
#define SECURE_SOCKET_LAYER_H_

#define INTERMEDIATE_SERVER_IP		"210.118.69.65"
#define INTERMEDIATE_SERVER_PORT	12345
#include <openssl/ssl.h>

extern SSL *ssl;

int BE_init_ssl();
void BE_free_ssl();



#endif /* SECURE_SOCKET_LAYER_H_ */
