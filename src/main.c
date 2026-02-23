#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>


#include "tcp.h"

typedef struct {
	size_t len;
	void *data;
} buffer_t;


void syn_header(struct tcphdr *hdr, uint32_t src_port, uint32_t dst_port) {

	hdr->source = htonl(src_port);
	hdr->dest = htonl(dst_port);

	hdr->seq = htonl(0);
	hdr->ack_seq = htonl(0);

	// flags
	hdr->syn = 1;

	// This is arbitrary
	hdr->window = htonl(40000);
	hdr->urg_ptr = htonl(0);
}

void ip_socket_init(int *sockfd) {
// 	int hdrincl = 1;
	int status = 0;
	*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (-1 == *sockfd) {
		perror("socket");
	}
	// status = setsockopt(*sockfd, IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl));
	// if (-1 == status) {
		// perror("setsockopt");
	// }
}

void ip_socket_send(int sockfd, uint32_t ip_address, buffer_t buffer) {
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
	char buf[2000] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
	buffer_t pkt = { .data = buf, .len = strlen(buf) };

	// syn_header(&hdr, 6969, 4000);
	
	ip_socket_init(&sockfd);

	ip_socket_send(sockfd, 0x7F000001, pkt);

	return 0;
}
