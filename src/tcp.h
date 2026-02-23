#ifndef TCP_H
#define TCP_H
#include <stdint.h>
 
typedef enum tcp_status_s {
    TCP_STATUS_OK = 0,
    TCP_STATUS_ERROR = 1,
} tcp_status;
 
 
#define __LITTLE_ENDIAN_BITFIELD
 
/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */
 
// #define __CHECKER__
 
/* sparse defines __CHECKER__; see Documentation/dev-tools/sparse.rst */
#ifdef __CHECKER__
#define __bitwise	__attribute__((bitwise))
#else
#define __bitwise
#endif
 
/* The kernel doesn't use this legacy form, but user space does */
#define __bitwise__ __bitwise
 
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;
 
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
 
typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;
 
struct tcphdr {
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
 
 
#endif
