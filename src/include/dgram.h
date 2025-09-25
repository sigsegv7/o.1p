/*
 * Copyright (c) 2023-2025 Ian Marco Moffett and the Osmora Team.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Hyra nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DGRAM_H
#define DGRAM_H

#include <stdint.h>
#include "if_ether.h"
#include "link.h"

typedef int16_t tx_len_t;
typedef tx_len_t rx_len_t;

/*
 * Represents an ONET datagram
 *
 * @crc32: CRC32 checksum of data + header
 * @length: Packet length in bytes
 * @reserved: Reserved for future use
 * @reserved1: Reserved for future use
 * @port: Datagram port number to send on
 */
struct onet_dgram {
    uint32_t crc32;
    uint16_t length;
    uint16_t reserved;
    uint16_t reserved1;
    uint8_t port;
} __attribute__((packed));

/*
 * Get the total length of a datagram including
 * the length of the ethernet header and the header
 * of the datagram itself.
 */
#define DGRAM_LEN(len) \
    (sizeof(struct onet_dgram) + sizeof(struct ether_hdr) + len)

/*
 * Get the start of the datagram header start by passing
 * a pointer to the buffer that holds the entire packet
 */
#define DGRAM_HDR(packet_p) \
    ((void *)((char *)packet_p + sizeof(struct ether_hdr)))

/*
 * Get the start of the datagram data start by passing
 * a pointer to the buffer that holds the entire packet
 */
#define DGRAM_DATA(packet_p) \
    ((char *)packet_p + sizeof(struct onet_dgram) + sizeof(struct ether_hdr))

/*
 * Initialize an ONET datagram
 *
 * @length: Length of a packet to send
 * @port: Port number to send on
 * @res: Result is written here
 *
 * Returns zero on success, otherwise a less than zero
 * value on failure.
 */
int dgram_load(uint16_t length, uint8_t port, struct onet_dgram *res);

/*
 * Send a datagram through ONET
 *
 * @link: The ONET link to send data over
 * @dst: Destination address to send to
 * @buf: The buffer containing data to send
 * @len: Length of buffer to send
 *
 * Returns the number of bytes transmitted on success, otherwise
 * a less than zero value on failure.
 */
tx_len_t dgram_send(
    struct onet_link *link, mac_addr_t dst,
    void *buf, uint16_t len
);

/*
 * Get data from an ONET link
 *
 * @link: The ONET link to recv data from
 * @buf: The buffer to recv data into
 * @len: The length of expected data
 */
rx_len_t dgram_recv(struct onet_link *link, void *buf, uint16_t len);

#endif  /* DGRAM_H */
