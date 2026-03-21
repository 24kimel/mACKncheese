#ifndef SOCKET_H
#define SOCKET_H
#include <stdint.h>
#include "buffer.h"

void tcp_socket_init(int *sockfd);

void tcp_socket_send(int sockfd, uint32_t ip_address, buffer_t buffer);

void tcp_socket_destroy(int sockfd);

#endif /* SOCKET_H */
