#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>


#include "tcp.h"

typedef struct {
	size_t len;
	void *data;
} buffer_t;

// TODO: export
void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}


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

// TODO: return status code
void tcp_socket_init(int *sockfd) {
	int status = 0;

	*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if (-1 == *sockfd) {
		perror("socket");
	}
}

static inline void ipv4_pseudo_header_init(ipv4_pseudo_header_t *hdr, uint32_t source_ip, uint32_t dest_ip, uint8_t proto, uint16_t tcp_length) {
	hdr->source_address = ntohl(source_ip);
	hdr->dest_address = ntohl(dest_ip);
	hdr->zero = 0;
	hdr->ipproto = proto;
	hdr->tcp_length = ntohs(tcp_length);
}

// TODO: export to buffer module...
uint32_t sum_16bit_words(buffer_t *buffer) {
	uint16_t current_word = 0;
	size_t i = 0;
	uint32_t sum = 0;

	for (i = 0; i < ((buffer->len) & (~((size_t)1))) / 2; i++)
	{
		printf("word: %04x\n", ((uint16_t *)(buffer->data))[i]);
		sum += ntohs(((uint16_t *)(buffer->data))[i]);
	}

	// if the length is odd, append a null byte to the last byte and
	// include the newly created 16bit word into the sum
	if (1 == (buffer->len & 1)) {
		sum += (uint16_t)(((uint8_t *)(buffer->data))[i]);
	}

	return sum;
}

__sum16 ipv4_checksum(ipv4_pseudo_header_t *pseudo_header, buffer_t *packet) {
	// TODO: append last odd byte with 0
	buffer_t pseudo_header_buffer = {0};
	uint32_t sum = 0;

	// TODO: export this to a macro??
	pseudo_header_buffer.data = pseudo_header;
	pseudo_header_buffer.len = sizeof(*pseudo_header);

	printf("printing pseudo header\n");
	DumpHex(pseudo_header_buffer.data, pseudo_header_buffer.len);
	printf("printing tcp header\n");
	DumpHex(packet->data, packet->len);
	sum = sum_16bit_words(&pseudo_header_buffer);
	sum += sum_16bit_words(packet);

	printf("sum 1: %x\n", sum);
	// convert to one's complement
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += sum >> 16;
	sum = ~sum;

	// convert endianness
	sum = htons(sum);
	printf("sum 2: %x\n", sum);
	return sum;
}


__sum16 tcp_calculate_checksum(buffer_t *packet, uint32_t source_ip, uint32_t dest_ip) {
	uint16_t tcp_length = 0;
	ipv4_pseudo_header_t pseudo_header = {0};

	tcp_length = packet->len;
	ipv4_pseudo_header_init(&pseudo_header, source_ip, dest_ip, IPPROTO_TCP, packet->len);

	return ipv4_checksum(&pseudo_header, packet);

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


int main(int argc, char **argv) {

	struct tcphdr hdr = {0};
	int sockfd = 0;
	buffer_t pkt = {0};
	__sum16 tcp_checksum = 0;

	pkt.data = (void *)(&hdr);
	pkt.len = sizeof(hdr);
	tcp_socket_init(&sockfd);
	// mss_syn_header(&hdr, atoi(argv[1]), 0x2222, 0xffd7);
	syn_header(&hdr, atoi(argv[1]), 0x2222);
	
	hdr.check = tcp_calculate_checksum(&pkt, LOCALHOST, LOCALHOST);

	tcp_socket_send(sockfd, LOCALHOST, pkt);

	return 0;
}
