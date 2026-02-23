#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>


#include "tcp.h"

typedef struct {
	size_t len;
	void *data;
} buffer_t;

#define LOCALHOST (0x7F000001)


void syn_header(struct tcphdr *hdr, uint16_t src_port, uint16_t dst_port) {


	hdr->seq = htonl(0x31313131);
	hdr->ack_seq = htonl(0);

	// flags
	hdr->syn = 1;
	hdr->doff = 5;

	// This is arbitrary
	hdr->window = htonl(40000);
	hdr->urg_ptr = htonl(0);

	hdr->source = htons(src_port);
	hdr->dest = htons(dst_port);
}

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


int main(void) {

	struct tcphdr hdr = {0};
	int sockfd = 0;
	buffer_t pkt = {0};

	pkt.data = (void *)(&hdr);
	pkt.len = sizeof(hdr);
	tcp_socket_init(&sockfd);
	syn_header(&hdr, 0x1111, 0x2222);

	tcp_socket_send(sockfd, LOCALHOST, pkt);

	return 0;
}
