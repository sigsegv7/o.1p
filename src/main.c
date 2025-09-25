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
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "if_ether.h"
#include "dgram.h"

#define TEST_STR "Hello from o.1p!! Meow meow!"

static const char *iface = NULL;

static void
help(char **argv)
{
    printf(
        "usage: %s -i <iface>\n"
        "[-h]   Show this message\n"
        "[-i]   Interface to use\n",
        argv[0]
    );
}

static int
data_send(void)
{
    char data[DGRAM_LEN(128)], *p;
    struct sockaddr_ll saddr;
    struct ether_hdr *eth;
    struct onet_dgram *dgram;
    struct ifreq ifr;
    int error, sockfd;

    eth = (struct ether_hdr *)data;
    p = DGRAM_DATA(data);
    dgram = DGRAM_HDR(data);

    memset(data, 0, sizeof(data));
    memcpy(p, TEST_STR, sizeof(TEST_STR));

    sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        printf("error: failed to create socket (error=%d)\n", sockfd);
        return sockfd;
    }

    /* Set the interface that we want to use */
    strlcpy(ifr.ifr_name, iface, IFNAMSIZ);
    error = ioctl(sockfd, SIOGIFINDEX, &ifr);
    if (error < 0) {
        printf("ioctl[SIOGIFINDEX]: bad iface \"%s\"\n", iface);
        return error;
    }

    /*
     * Set up link layer sockaddr, load up the frame, datagram
     * and send it off.
     */
    saddr.sll_ifindex = ifr.ifr_ifindex;
    saddr.sll_halen = ETH_ALEN;
    ether_load_route(0x54E1AD2CAE48, 0xFFFFFFFFFFFF, eth);
    dgram_load(sizeof(TEST_STR), 50, dgram);
    sendto(
        sockfd, &data, sizeof(data), 0,
        (struct sockaddr *)&saddr, sizeof(struct sockaddr_ll)
    );

    close(sockfd);
    return 0;
}

int
main(int argc, char **argv)
{
    int opt;

    if (argc < 2) {
        printf("error: too few arguments!\n");
        help(argv);
        return -1;
    }

    while ((opt = getopt(argc, argv, "i:h")) != -1) {
        switch (opt) {
        case 'h':
            help(argv);
            return -1;
        case 'i':
            iface = optarg;
            break;
        }
    }

    /* We need an interface */
    if (iface == NULL) {
        printf("error: interface not supplied\n");
        return -1;
    }

    return data_send();
}
