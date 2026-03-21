#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>


#include "buffer.h"
#include "checksum.h"
#include "tcp.h"
#include "socket.h"



#define LOCALHOST (0x7F000001)


void syn_header(struct tcphdr *hdr, uint16_t src_port, uint16_t dst_port) {


	hdr->seq = htonl(0x7a36fce8);
	hdr->ack_seq = htonl(0);

	// flags
	hdr->syn = 1;
	hdr->doff = 5;

	// This is arbitrary
	hdr->window = htons(0xffd7);
	hdr->urg_ptr = 0;
	hdr->check = 0;

	hdr->source = htons(src_port);
	hdr->dest = htons(dst_port);
}

void mss_syn_header(mss_tcphdr_t* mss_tcphdr, uint16_t src_port, uint16_t dst_port, uint16_t mss) {
	syn_header((struct tcphdr *)mss_tcphdr, src_port, dst_port);

	mss_tcphdr->mss_option.kind = 2;
	mss_tcphdr->mss_option.length = 4;
	mss_tcphdr->mss_option.mss = ntohs(mss);
	mss_tcphdr->hdr.doff = sizeof(*mss_tcphdr) >> 2;
}


int main(int argc, char **argv) {

	struct tcphdr hdr = {0};
	int sockfd = 0;
	buffer_t pkt = {0};

	pkt.data = (void *)(&hdr);
	pkt.len = sizeof(hdr);
	tcp_socket_init(&sockfd);
	syn_header(&hdr, atoi(argv[1]), 0x2222);
	
	hdr.check = tcp_checksum(&pkt, LOCALHOST, LOCALHOST);

	tcp_socket_send(sockfd, LOCALHOST, pkt);

	tcp_socket_destroy(sockfd);

	return 0;
}
