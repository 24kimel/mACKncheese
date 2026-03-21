#ifndef TCP_H
#define TCP_H

#include <stdint.h>

#include "types.h"
 
typedef enum tcp_status_s {
    TCP_STATUS_OK = 0,
    TCP_STATUS_ERROR = 1,
} tcp_status;
 
 
struct __attribute__((packed)) tcphdr {
	__be16	source;
	__be16	dest;
	__be32	seq;
	__be32	ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u16	ae:1,
		res1:3,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u16	doff:4,
		res1:3,
		ae:1,
		cwr:1,
		ece:1,
		urg:1,
		ack:1,
		psh:1,
		rst:1,
		syn:1,
		fin:1;
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif
	__be16	window;
	__sum16	check;
	__be16	urg_ptr;
};

typedef struct __attribute__((packed)) mss_option {
	uint8_t kind;
	uint8_t length;
	__be16 mss;
} mss_option_t;

typedef struct __attribute__((packed)) mss_tcphdr {
	struct tcphdr hdr;
	mss_option_t mss_option;
} mss_tcphdr_t;

typedef struct __attribute__((packed)) ipv4_pseudo_header {
	__be32 source_address;
	__be32 dest_address;
	uint8_t zero;
	uint8_t ipproto;
	__be16 tcp_length;
} ipv4_pseudo_header_t;
 
 
#endif
