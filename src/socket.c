#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "buffer.h"

// TODO: return status code
void tcp_socket_init(int *sockfd) {
	int status = 0;

	*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (-1 == *sockfd) {
		perror("socket");
	}
}


void tcp_socket_send(int sockfd, uint32_t ip_address, buffer_t buffer) {
	int status = 0;
	struct sockaddr_in addr = {0};

	addr.sin_addr.s_addr = (in_addr_t)(htonl(ip_address));
	addr.sin_port = 0;
	addr.sin_family = AF_INET;

	status = sendto(sockfd, buffer.data, buffer.len, 0, (struct sockaddr *)(&addr), sizeof(addr));
	if (-1 == status) {
		perror("sendto");
	}
}

void tcp_socket_destroy(int sockfd) {
	int status = 0;

	status = close(sockfd);
	if (-1 == status) {
		perror("close");
	}
}
