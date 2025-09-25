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

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <onet/if_ether.h>
#include <onet/dgram.h>
#include <onet/link.h>

#define TEST_STR "Hello from o.1p!! Meow meow!"

static const char *iface = NULL;
static bool do_recv = false;

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
    struct onet_link link;
    int error;

    /* Open a link */
    error = onet_open(iface, &link);
    if (error < 0) {
        return error;
    }

    /* Send test packet to FF:FF:FF:FF:FF:FF */
    dgram_send(
        &link, 0xFFFFFFFFFFFF,
        TEST_STR, sizeof(TEST_STR)
    );

    onet_close(&link);
    return 0;
}

static int
data_recv(void)
{
    char buf[sizeof(TEST_STR) + 8];
    struct onet_link link;
    int error;

    /* Open a link */
    error = onet_open(iface, &link);
    if (error < 0) {
        return error;
    }

    memset(buf, 0, sizeof(buf));

    /* Recv data */
    dgram_recv(
        &link, buf,
        sizeof(TEST_STR)
    );

    printf("%s\n", buf);
    onet_close(&link);
    return 0;
}

int
main(int argc, char **argv)
{
    int opt, error;

    if (argc < 2) {
        printf("error: too few arguments!\n");
        help(argv);
        return -1;
    }

    while ((opt = getopt(argc, argv, "i:hr")) != -1) {
        switch (opt) {
        case 'h':
            help(argv);
            return -1;
        case 'i':
            iface = optarg;
            break;
        case 'r':
            do_recv = true;
            break;
        }
    }

    /* We need an interface */
    if (iface == NULL) {
        printf("error: interface not supplied\n");
        return -1;
    }

    error = do_recv ? data_recv() : data_send();
    return error;
}
