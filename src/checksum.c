#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "tcp.h"
#include "types.h"
#include "buffer.h"
#include "checksum.h"
#include "debug.h"

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
	dump_hex(pseudo_header_buffer.data, pseudo_header_buffer.len);
	printf("printing tcp header\n");
	dump_hex(packet->data, packet->len);
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


__sum16 tcp_checksum(buffer_t *packet, uint32_t source_ip, uint32_t dest_ip) {
	uint16_t tcp_length = 0;
	ipv4_pseudo_header_t pseudo_header = {0};

	tcp_length = packet->len;
	ipv4_pseudo_header_init(&pseudo_header, source_ip, dest_ip, IPPROTO_TCP, packet->len);

	return ipv4_checksum(&pseudo_header, packet);

}

