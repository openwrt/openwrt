/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include "sw.h"
#include "sw_api.h"
#include "sw_api_us.h"

#define SSDK_SOCK_SEND_TRY_NUM 1000
#define SSDK_SOCK_RCV_TRY_NUM 1000
#define SSDK_SOCK_FD_NUM 16
typedef struct
{
    a_uint32_t   ssdk_sock_pid;
    a_int32_t    ssdk_sock_fd;
} ssdk_sock_t;
ssdk_sock_t ssdk_sock[SSDK_SOCK_FD_NUM];

static a_uint32_t ssdk_sock_prot = 0;
static struct nlmsghdr *nl_hdr = NULL;
#if defined(API_LOCK)
static aos_lock_t ssdk_sock_lock;
#define SOCK_LOCKER_INIT    aos_lock_init(&ssdk_sock_lock)
#define SOCK_LOCKER_LOCK    aos_lock(&ssdk_sock_lock)
#define SOCK_LOCKER_UNLOCK  aos_unlock(&ssdk_sock_lock)
#else
#define SOCK_LOCKER_INIT
#define SOCK_LOCKER_LOCK
#define SOCK_LOCKER_UNLOCK
#endif

static ssdk_sock_t *
ssdk_sock_alloc(a_uint32_t pid)
{
    a_uint32_t i;

    for (i = 0; i < SSDK_SOCK_FD_NUM; i++)
    {
        if (!ssdk_sock[i].ssdk_sock_pid)
        {
            return &ssdk_sock[i];
        }
        else
        {
            if (0 != kill(ssdk_sock[i].ssdk_sock_pid, 0))
            {
                return &ssdk_sock[i];
            }
        }
    }

    return NULL;
}

static ssdk_sock_t *
ssdk_sock_find(a_uint32_t pid)
{
    a_uint32_t i;

    for (i = 0; i < SSDK_SOCK_FD_NUM; i++)
    {
        if (ssdk_sock[i].ssdk_sock_pid == pid)
        {
            return &ssdk_sock[i];
        }
    }

    return NULL;
}

sw_error_t
sw_uk_if(a_uint32_t arg_val[SW_MAX_API_PARAM])
{
    struct sockaddr_nl src_addr;
    struct sockaddr_nl dest_addr;
    struct msghdr msg;
    struct iovec  iov;
    struct nlmsghdr *nlh;
    ssdk_sock_t * sock;
    a_int32_t     sock_fd;
    a_uint32_t    curr_pid;
    sw_error_t    rv = SW_OK;
    a_uint32_t    i, j, flag;

    curr_pid = getpid();

    SOCK_LOCKER_LOCK;
    sock = ssdk_sock_find(curr_pid);
    if (!sock)
    {
        sock = ssdk_sock_alloc(curr_pid);
        if (!sock)
        {
            SW_OUT_ON_ERROR(SW_NO_RESOURCE);
        }

        sock_fd = socket(PF_NETLINK, SOCK_RAW, ssdk_sock_prot);
        aos_mem_set(&src_addr, 0, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid    = curr_pid;
        src_addr.nl_groups = 0;
        bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

        sock->ssdk_sock_fd  = sock_fd;
        sock->ssdk_sock_pid = curr_pid;
    }
    else
    {
        sock_fd = sock->ssdk_sock_fd;
    }

    aos_mem_set(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid    = 0;
    dest_addr.nl_groups = 0;

    nlh = nl_hdr;
    aos_mem_set(nlh, 0, NLMSG_SPACE(SW_MAX_PAYLOAD));
    nlh->nlmsg_len   = NLMSG_SPACE(SW_MAX_PAYLOAD);
    nlh->nlmsg_pid   = curr_pid;
    nlh->nlmsg_flags = 0;
    aos_mem_copy(NLMSG_DATA(nlh), arg_val, SW_MAX_PAYLOAD);

    iov.iov_base    = (void *)nlh;
    iov.iov_len     = nlh->nlmsg_len;

    aos_mem_set(&msg, 0, sizeof(msg));
    msg.msg_name    = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    for (i = 0; i < SSDK_SOCK_SEND_TRY_NUM; i++)
    {
        if (0 < sendmsg(sock_fd, &msg, MSG_DONTWAIT))
        {
            break;
        }
    }

    if (SSDK_SOCK_SEND_TRY_NUM <= i)
    {
        SW_OUT_ON_ERROR(SW_TIMEOUT);
    }

    flag = 0;
    aos_mem_set(nlh, 0, NLMSG_SPACE(SW_MAX_PAYLOAD));
    for (i = 0; i < SSDK_SOCK_RCV_TRY_NUM; i++)
    {
        for (j = 0; j < 1000; j++)
        {
            if (0 < recvmsg(sock_fd, &msg, MSG_DONTWAIT))
            {
                flag = 1;
                break;
            }
        }

        if (flag)
        {
            break;
        }
        else
        {
            aos_mdelay(10);
        }
    }

    if (SSDK_SOCK_RCV_TRY_NUM <= i)
    {
        SW_OUT_ON_ERROR(SW_TIMEOUT);
    }

out:
    SOCK_LOCKER_UNLOCK;
    return rv;
}

sw_error_t
sw_uk_init(a_uint32_t nl_prot)
{
    if (!nl_hdr)
    {
        nl_hdr = (struct nlmsghdr *)aos_mem_alloc(NLMSG_SPACE(SW_MAX_PAYLOAD));
    }

    if (!nl_hdr)
    {
        return SW_NO_RESOURCE;
    }

#if defined UK_NL_PROT
    ssdk_sock_prot = UK_NL_PROT;
#else
    ssdk_sock_prot = nl_prot;
#endif
    SOCK_LOCKER_INIT;
    aos_mem_zero(ssdk_sock, sizeof(ssdk_sock_t) * SSDK_SOCK_FD_NUM);
    return SW_OK;
}

sw_error_t
sw_uk_cleanup(void)
{
    aos_mem_zero(ssdk_sock, sizeof(ssdk_sock_t) * SSDK_SOCK_FD_NUM);

    if (nl_hdr)
    {
        aos_mem_free(nl_hdr);
        nl_hdr = NULL;
    }

    return SW_OK;
}

