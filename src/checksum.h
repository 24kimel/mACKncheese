#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "buffer.h"
#include "types.h"
#include <stdint.h>

/**
 * @brief calculate the checksum field for a TCP packet.
 *
 * @param[in] packet a pointer to the TCP packet, starting with the TCP header.
 * @param[in] source_ip source IP address.
 * @param[in] dest_ip destination IP address.
 *
 * @returns the checksum field for the TCP packet.
 */
__sum16 tcp_checksum (buffer_t *packet, uint32_t source_ip, uint32_t dest_ip);

#endif /* CHECKSUM_H */
