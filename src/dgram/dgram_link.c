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

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include "if_ether.h"
#include "dgram.h"

tx_len_t
dgram_send(struct onet_link *link, mac_addr_t dst, void *buf, uint16_t len)
{
    struct sockaddr_ll saddr;
    struct ether_hdr *eth;
    struct onet_dgram *dgram;
    size_t dgram_len;
    char *p, *data;

    dgram_len = DGRAM_LEN(len);
    p = malloc(dgram_len);
    if (p == NULL) {
        return -1;
    }

    eth = (struct ether_hdr *)p;
    data = DGRAM_DATA(p);
    dgram = DGRAM_HDR(p);

    /* Copy data to send buffer */
    memset(data, 0, len);
    memcpy(data, buf, len);

    /* Hardware address needs to be big endian */
    dst = mac_swap((void *)&dst);

    /*
     * Set up link layer sockaddr, load up the frame, datagram
     * and send it off.
     */
    saddr.sll_ifindex = link->iface_idx;
    saddr.sll_halen = HW_ADDR_LEN;
    ether_load_route(link->hwaddr, dst, eth);
    dgram_load(len, 50, dgram);
    sendto(
        link->sockfd, p, dgram_len, 0,
        (struct sockaddr *)&saddr, sizeof(struct sockaddr_ll)
    );

    free(p);
    return len;
}

rx_len_t
dgram_recv(struct onet_link *link, void *buf, uint16_t len)
{
    socklen_t addr_len;
    struct sockaddr_ll saddr;
    struct ether_hdr *hdr;
    size_t dgram_len, recv_len;
    uint16_t proto;
    mac_addr_t dest_mac;
    char *p;

    if (link == NULL || buf == NULL) {
        return -1;
    }

    if (len == 0) {
        return -1;
    }

    /* Allocate an RX buffer */
    dgram_len = DGRAM_LEN(len);
    p = malloc(dgram_len);
    if (p == NULL) {
        return -1;
    }

    addr_len = sizeof(struct sockaddr_ll);
    saddr.sll_ifindex = link->iface_idx;
    saddr.sll_halen = HW_ADDR_LEN;

    /*
     * Wait until we get a packet for us with the right
     * protocol ID.
     */
    for (;;) {
        recv_len = recvfrom(
            link->sockfd, p, dgram_len,
            0, (struct sockaddr *)&saddr,
            &addr_len
        );

        hdr = (void *)p;
        proto = ntohs(hdr->proto);
        dest_mac = mac_swap(hdr->dest);

        if (recv_len != dgram_len) {
            continue;
        }

        if (proto != PROTO_ID) {
            continue;
        }

        /* If this is for everyone, take it */
        if (dest_mac == MAC_BROADCAST) {
            break;
        }

        /* If this is for us, take it */
        if (dest_mac == link->hwaddr) {
            break;
        }
    }

    memcpy(buf, DGRAM_DATA(p), len);
    free(p);
    return dgram_len;
}
