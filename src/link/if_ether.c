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

#include <sys/errno.h>
#include <byteswap.h>
#include <string.h>
#include "if_ether.h"

/*
 * Using the protocol ID 0xFD for testing,
 * see RFC 3692
 */
#define PROTO_ID 0xFD

/* 48-bit mask for MAC addresses */
#define MAC_MASK 0xFFFFFFFFFFFF

int
ether_load_route(mac_addr_t src, mac_addr_t dest, struct ether_hdr *res)
{
    uint32_t *p;

    if (res == NULL) {
        return -EINVAL;
    }

    /* Set up the source */
    res->source[0] = (src >> 40) & 0xFF;
    res->source[1] = (src >> 32) & 0xFF;
    res->source[2] = (src >> 24) & 0xFF;
    res->source[3] = (src >> 16) & 0xFF;
    res->source[4] = (src >> 8) & 0xFF;
    res->source[5] = src & 0xFF;

    /* Set up the destination */
    res->dest[0] = (dest >> 40) & 0xFF;
    res->dest[1] = (dest >> 32) & 0xFF;
    res->dest[2] = (dest >> 24) & 0xFF;
    res->dest[3] = (dest >> 16) & 0xFF;
    res->dest[4] = (dest >> 8) & 0xFF;
    res->dest[5] = dest & 0xFF;
    res->proto = PROTO_ID;
    return 0;
}
