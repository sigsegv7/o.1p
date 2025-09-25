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

#ifndef IF_ETHER_H
#define IF_ETHER_H

#include <stdint.h>

#define HW_ADDR_LEN 6
#define PROTO_ID 0x88B5
#define MAC_BROADCAST 0xFFFFFFFFFFFF

typedef uint64_t mac_addr_t;

struct ether_hdr {
    uint8_t dest[HW_ADDR_LEN];
    uint8_t source[HW_ADDR_LEN];
    uint16_t proto;
} __attribute__((packed));

static inline mac_addr_t
mac_swap(uint8_t mac[HW_ADDR_LEN])
{
    mac_addr_t swapped = 0;

    swapped |= (uint64_t)mac[0] << 40;
    swapped |= (uint64_t)mac[1] << 32;
    swapped |= (uint64_t)mac[2] << 24;
    swapped |= (uint64_t)mac[3] << 16;
    swapped |= (uint64_t)mac[4] << 8;
    swapped |= (uint64_t)mac[5];
    return swapped;
}

/*
 * Load the source and dest routes into an ethernet
 * frame.
 *
 * @src: Source address to use
 * @dest: Dest address to use
 * @res: Result is written here
 */
int ether_load_route(mac_addr_t src, mac_addr_t dest, struct ether_hdr *res);

#endif  /* !IF_ETHER_H */
