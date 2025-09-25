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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "link.h"

int
onet_open(const char *iface, struct onet_link *res)
{
    struct ifreq ifr;
    int error;

    if (res == NULL) {
        return -EINVAL;
    }

    /* Open a raw socket */
    res->sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if (res->sockfd < 0) {
        return res->sockfd;
    }

    /* Set the interface we want to use */
    strlcpy(ifr.ifr_name, iface, IFNAMSIZ);
    error = ioctl(res->sockfd, SIOGIFINDEX, &ifr);
    if (error < 0) {
        printf("ioctl[SIOGIFHWADDR]: could not read hwaddr \"%s\"\n", iface);
        return error;
    }

    res->iface_idx = ifr.ifr_ifindex;

    /* Get the hardware address */
    error = ioctl(res->sockfd, SIOCGIFHWADDR, &ifr);
    if (error < 0) {
        printf("ioctl[SIOGIFHWADDR]: could not read hwaddr \"%s\"\n", iface);
        return error;
    }

    res->hwaddr = mac_swap((void *)ifr.ifr_hwaddr.sa_data);
    return 0;
}

int
onet_close(struct onet_link *olp)
{
    if (olp == NULL) {
        return -EINVAL;
    }

    close(olp->sockfd);
}
