/*
 * @Author: liuguan
 * @Date: 2022-07-21 23:08:20
 * @LastEditors: liuguan
 * @LastEditTime: 2022-07-25 23:09:07
 * @FilePath: /esp8266-rtos/src/lettershell/cmd_ping.c
 * @Description:
 *
 * Copyright (c) 2022 by liuguan, All Rights Reserved.
 */
/*
 * @Author: liuguan
 * @Date: 2022-07-21 23:08:20
 * @LastEditors: liuguan
 * @LastEditTime: 2022-07-21 23:08:21
 * @FilePath: /esp8266-rtos/src/lettershell/command.c
 * @Description:
 *
 * Copyright (c) 2022 by liuguan, All Rights Reserved.
 */
/**
 * Copyright 2022 dengzhijun
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "SHELL"
#if 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <lwip/tcpip.h>
//#include <lwip/inet.h>
//#include "lwip/sockets.h"
//#include "lwip/netdb.h"
//#include <lwip/ip.h>
//#include <lwip/icmp.h>
//#include <lwip/inet_chksum.h>
//#include "lwip/netdb.h"
#include <getopt.h>
//#include "core/libsock/libsock_ext.h"
//#include "core/libthread/libthread.h"
#include "elog.h"

#define ping_help_info                                                            \
    "[*] -c count\n"                                                              \
    "\tStop after sending count ECHO_REQUEST packets. With deadline option,\n"    \
    "\tping waits for count ECHO_REPLY packets, until    the timeout expires.\n"  \
    "[*] -i interval\n"                                                           \
    "\tWait  interval seconds between sending each packet.\n"                     \
    "\tThe default is to wait for one second between each packet normally.\n"     \
    "[*] -s packetsize\n"                                                         \
    "\tSpecifies  the  number  of data bytes to be sent.\n"                       \
    "[*] -W timeout\n"                                                            \
    "\tTime to wait for a response, in seconds.\n"                                \
    "[*] -w deadline\n"                                                           \
    "\tSpecify a timeout, in seconds, before ping exits regardless of how many\n" \
    "\tpackets have been sent or received.\n"                                     \
    "\tIn this case ping  does  not stop after count packet are sent,\n"          \
    "\tit waits either for deadline expire or until count probes are answered\n"  \
    "\tor for some error notification from network.\n"                            \
    "[*] -e exit\n"                                                               \
    "\tExit the ping thread according to the handle."

#define CMD_PING_DEFAULT_COUNT (3)
#define CMD_PING_DEFAULT_DATA_LONG (0xffff)
#define CMD_PING_DEFAULT_INTERVAL (1)
#define CMD_PING_DEFAULT_TIMEOUT (5 * 1000)

#define PING_TO 5000 /* timeout to wait every reponse(ms) */
#define PING_ID 0xABCD
#define PING_DATA_SIZE 100 /* size of send frame buff, not include ICMP frma head */
#define PING_IP_HDR_SIZE 40
#define GET_TICKS EspMillis 

struct ping_data
{
    ip_addr_t sin_addr; /* server addr */

    u32_t count;     /* number of ping */
    u32_t data_long; /* the ping packet data long */
    u32_t interval;  /* Wait interval seconds between sending each packet, default 1s */
    u32_t timeout;   /* Time to wait for a response, in seconds */
    u32_t deadline;  /* Specify a timeout, in seconds, ping thread will stop if timeout */
    u32_t ttl;       /* ttl ping only. Set the IP Time to Live. */

    int run_flag; /* run flag, 0:stop 1:start */
};

typedef struct
{
    thread_t ping_thread;
    int handle; /* the ping hread handle */

    struct ping_data ping_arg;
} cmd_ping_arg;

#define PING_ARG_HANDLE_MAX 2
cmd_ping_arg *s_cmd_ping_arg_handle[PING_ARG_HANDLE_MAX] = {NULL};

static void generate_ping_echo(u8_t *buf, u32_t len, u16_t seq, u16_t id)
{
    u32_t i;
    u32_t data_len = len - sizeof(struct icmp_echo_hdr);
    struct icmp_echo_hdr *pecho;

    pecho = (struct icmp_echo_hdr *)buf;

    ICMPH_TYPE_SET(pecho, ICMP_ECHO);
    ICMPH_CODE_SET(pecho, 0);

    pecho->chksum = 0;
    pecho->id = id;
    pecho->seqno = htons(seq);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++)
    {
        buf[sizeof(struct icmp_echo_hdr) + i] = (unsigned char)i;
    }
    /* Checksum of icmp header and data */
    pecho->chksum = inet_chksum(buf, len);
}

s32_t sendIcmpPacket(struct ping_data *data)
{
    struct sockaddr_in ToAddr;
    struct sockaddr_in FromAddr;
    socklen_t FromLen;
    int iSockID, iStatus;
    fd_set ReadFds;
    struct timeval Timeout;

    u8_t *ping_buf = NULL;
    u32_t buf_size, request_size, reply_size;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *pecho;
    u16_t ping_seq_num = 1;
    s32_t ping_pass = 0;
    u16_t ping_fail = 0;
    u32_t i;
    u32_t TimeStart = 0, TimeNow = 0, TimeElapse = 0;
    u32_t start_time = 0, stop_time = 0;
    u16_t ping_ids = 0x1234;
    u32_t min_time = 0, max_time = 0, avg_time = 0;

    memset(&FromAddr, 0, sizeof(FromAddr));
    FromLen = sizeof(FromAddr);

    iSockID = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (iSockID < 0)
    {
        printf("create socket fail. errno:%d\n", errno);
        return -1;
    }

    // fcntl(iSockID, F_SETFL, O_NONBLOCK);  /* set noblocking */
    int val = 1;
    iStatus = ioctlsocket(iSockID, FIONBIO, (void *)&val); /* set noblocking */
    if (iStatus < 0)
        printf("setsockopt err! errno:%d\n", errno);

    if (data->ttl > 0)
    {
        val = data->ttl;
        if (val > 255)
            val = 255;
        iStatus = setsockopt(iSockID, IPPROTO_IP, IP_TTL, &val, sizeof(val));
        if (iStatus < 0)
            printf("setsockopt err! errno:%d\n", errno);
    }

    memset(&ToAddr, 0, sizeof(ToAddr));
    ToAddr.sin_len = sizeof(ToAddr);
    ToAddr.sin_family = AF_INET;
    // ToAddr.sin_port = data->port;
    inet_addr_from_ipaddr(&ToAddr.sin_addr, &data->sin_addr);
    if (data->data_long != 0xffff)
    {
        request_size = data->data_long;
        if (request_size > 65500)
            request_size = 65500;
    }
    else
    {
        request_size = PING_DATA_SIZE;
    }
    request_size += sizeof(struct icmp_echo_hdr);
    buf_size = request_size + PING_IP_HDR_SIZE;
    ping_buf = malloc(buf_size);
    if (!ping_buf)
    {
        goto exit;
    }

    start_time = GET_TICKS();
    if (data->deadline > 0)
        stop_time = start_time + data->deadline * 1000;

    data->run_flag = 1;

    printf("PING %s %d bytes of data.\n", inet_ntoa(data->sin_addr), request_size);

    for (i = 0; i < data->count && data->run_flag; i++)
    {
        generate_ping_echo(ping_buf, request_size, ping_seq_num, ping_ids);
        sendto(iSockID, ping_buf, request_size, 0, (struct sockaddr *)&ToAddr, sizeof(ToAddr));
        TimeStart = GET_TICKS();
        while (data->run_flag)
        {
            FD_ZERO(&ReadFds);
            FD_SET(iSockID, &ReadFds);
            Timeout.tv_sec = 0;
            Timeout.tv_usec = 50 * 1000; /* 50ms */
            iStatus = select(iSockID+1, &ReadFds, NULL, NULL, &Timeout);
            if (iStatus > 0 && FD_ISSET(iSockID, &ReadFds))
            {
                /* block mode can't be used, we wait here if receiving party has sended,
                 * but we can set select to timeout mode to lower cpu's utilization */
                reply_size = recvfrom(iSockID, ping_buf, buf_size, 0,
                                      (struct sockaddr *)&FromAddr, &FromLen);
                if (reply_size >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)))
                {
                    TimeNow = GET_TICKS();
                    if (TimeNow >= TimeStart)
                    {
                        TimeElapse = TimeNow - TimeStart;
                    }
                    else
                    {
                        TimeElapse = 0xffffffffUL - TimeStart + TimeNow;
                    }
                    iphdr = (struct ip_hdr *)ping_buf;
                    pecho = (struct icmp_echo_hdr *)(ping_buf + (IPH_HL(iphdr) * 4));
                    if ((pecho->id == ping_ids) && (pecho->seqno == htons(ping_seq_num)))
                    {
                        /* do some ping result processing */
                        if (ping_pass)
                        {
                            if (TimeElapse > max_time)
                                max_time = TimeElapse;
                            if (TimeElapse < min_time)
                                min_time = TimeElapse;
                        }
                        else
                        {
                            max_time = min_time = TimeElapse;
                        }
                        avg_time += TimeElapse;

                        printf("%d bytes from %s: icmp_seq=%d    time=%d ms\n",
                               (reply_size - (IPH_HL(iphdr) * 4) - sizeof(struct icmp_echo_hdr)),
                               inet_ntoa(FromAddr.sin_addr),
                               htons(pecho->seqno), TimeElapse);
                        ping_pass++;
                        break;
                    }
                }
            }

            TimeNow = GET_TICKS();
            if (TimeNow >= TimeStart)
            {
                TimeElapse = TimeNow - TimeStart;
            }
            else
            {
                TimeElapse = 0xffffffffUL - TimeStart + TimeNow;
            }
            if (TimeElapse >= data->timeout)
            { /* giveup this wait, if wait timeout */
                printf("Request timeout for icmp_seq=%d\n", ping_seq_num);
                ping_fail++;
                break;
            }
        }

        ping_seq_num++;
        ping_ids++;
        if (ping_ids == 0x7FFF)
            ping_ids = 0x1234;

        if (data->deadline > 0)
        {
            if (GET_TICKS() >= stop_time)
            {
                data->run_flag = 0;
                break;
            }
        }

        sleep(data->interval);
    }

    log_d("\n--- %s ping statistics ---\n"
           "%d packets transmitted, %d received, %d%% packet loss, time %dms\n"
           "rtt min/avg/max/mdev = %d/%d/%d/%d ms\n",
           inet_ntoa(data->sin_addr), ping_seq_num - 1, ping_pass,
           ping_fail * 100 / (ping_seq_num - 1), GET_TICKS() - start_time,
           min_time, ping_pass > 0 ? avg_time / ping_pass : 0, max_time, max_time - min_time);

exit:
    free(ping_buf);
    closesocket(iSockID);
    if (ping_pass > 0)
        return ping_pass;
    else
        return -1;
}

static int ping_handle_new(struct ping_data *arg)
{
    if (arg == NULL)
        return -1;
    for (int i = 0; i < PING_ARG_HANDLE_MAX; i++)
    {
        if (s_cmd_ping_arg_handle[i] == NULL)
        {
            s_cmd_ping_arg_handle[i] = malloc(sizeof(cmd_ping_arg));
            if (s_cmd_ping_arg_handle[i] == NULL)
            {
                printf("ping data malloc err!\n");
                return -1;
            }
            memset(s_cmd_ping_arg_handle[i], 0, sizeof(cmd_ping_arg));
            memcpy(&s_cmd_ping_arg_handle[i]->ping_arg, arg, sizeof(*arg));
            s_cmd_ping_arg_handle[i]->handle = i;
            return i;
        }
    }
    return -1;
}

static int ping_handle_free(int handle)
{
    if (handle < 0 || handle >= PING_ARG_HANDLE_MAX)
        return -1;

    if (s_cmd_ping_arg_handle[handle] != NULL)
    {
        free(s_cmd_ping_arg_handle[handle]);
        s_cmd_ping_arg_handle[handle] = NULL;
    }
    return 0;
}

static void ping_thread_fun(thread_t *th, void *arg)
{
    cmd_ping_arg *cmd_ping = s_cmd_ping_arg_handle[(int)arg];

    sendIcmpPacket(&cmd_ping->ping_arg);

    ping_handle_free(cmd_ping->handle);
}

static int ping_handle_start(int handle)
{
    if (handle < 0 || handle >= PING_ARG_HANDLE_MAX)
        return -1;

    if (s_cmd_ping_arg_handle[handle] == NULL)
        return -1;

    thread_t *th = thread_create(ping_thread_fun, (void *)handle);
    if (th != NULL)
        return -1;

    return 0;
}

static int ping_handle_stop(int handle)
{
    if (handle < 0 || handle >= PING_ARG_HANDLE_MAX)
        return -1;

    if (s_cmd_ping_arg_handle[handle])
        s_cmd_ping_arg_handle[handle]->ping_arg.run_flag = 0;

    return 0;
}

void ping(int argc, char *argv[])
{
    int temp = 0;
    int handle = -1;
    struct ping_data ping_data_t;
    int opt = 0;
    char *short_opts = "c:i:s:W:w:t:e:";
    memset(&ping_data_t, 0, sizeof(ping_data_t));

    /* set default value */
    ping_data_t.count = CMD_PING_DEFAULT_COUNT;
    ping_data_t.data_long = CMD_PING_DEFAULT_DATA_LONG;
    ping_data_t.interval = CMD_PING_DEFAULT_INTERVAL;
    ping_data_t.timeout = CMD_PING_DEFAULT_TIMEOUT;

    opterr = 0; /* close the "invalid option" warning */
    while ((opt = getopt(argc, argv, short_opts)) != -1)
    {
        // printf("optind:%d\t opt:%c\t optarg:%s\t argv[optind]:%s\n", optind, opt, optarg, argv[optind]);
        switch (opt)
        {
        case 'c':
            temp = atoi(optarg);
            ping_data_t.count = temp > 0 ? temp : CMD_PING_DEFAULT_COUNT;
            break;
        case 'i':
            temp = atoi(optarg);
            ping_data_t.interval = temp > 0 ? temp : CMD_PING_DEFAULT_INTERVAL;
            break;
        case 's':
            ping_data_t.data_long = (uint32_t)atoi(optarg);
            if (ping_data_t.data_long > 65500)
                ping_data_t.data_long = 65500;
            break;
        case 'W':
            temp = atoi(optarg);
            ping_data_t.timeout = temp > 0 ? temp : CMD_PING_DEFAULT_TIMEOUT;
            break;
        case 'w':
            temp = atoi(optarg);
            ping_data_t.deadline = temp > 0 ? temp : 1;
            break;
        case 't':
            temp = atoi(optarg);
            ping_data_t.ttl = temp > 0 && temp < 255 ? temp : 255;
            break;
        case 'e':
            temp = atoi(optarg);
            ping_handle_stop(temp);
            goto exit;
            break;
        case '?':
            printf("invalid option -- '%s'\n", argv[optind - 1]);
            goto exit;
            break;
        default:
            goto exit;
            break;
        }
    }

    if (optind >= argc)
    {
        printf("err: no destination!\n");
        goto exit;
    }

    struct hostent *host_entry;
    unsigned int address = 0;
    host_entry = gethostbyname(argv[optind]);
    if (host_entry)
    {
        address = *((u_long *)host_entry->h_addr_list[0]);
    }
    else
    {
        printf("invalid ping host.\n");
        goto exit;
    }

    ip4_addr_set_u32(&ping_data_t.sin_addr, address);

    handle = ping_handle_new(&ping_data_t);
    if (handle >= 0)
        ping_handle_start(handle);
exit:
    optind = 1; /* reset the index, optind must be 1, the first arg is argv[1] */

    return handle >= 0 ? 0 : -1;
}
#endif