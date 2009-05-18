
/*
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <a_config.h>
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"
#include "htc_packet.h"

#define AR6000_DATA_OFFSET    64

void a_netbuf_enqueue(A_NETBUF_QUEUE_T *q, void *pkt)
{
    skb_queue_tail((struct sk_buff_head *) q, (struct sk_buff *) pkt);
}

void a_netbuf_prequeue(A_NETBUF_QUEUE_T *q, void *pkt)
{
    skb_queue_head((struct sk_buff_head *) q, (struct sk_buff *) pkt);
}

void *a_netbuf_dequeue(A_NETBUF_QUEUE_T *q)
{
    return((void *) skb_dequeue((struct sk_buff_head *) q));
}

int a_netbuf_queue_size(A_NETBUF_QUEUE_T *q)
{
    return(skb_queue_len((struct sk_buff_head *) q));
}

int a_netbuf_queue_empty(A_NETBUF_QUEUE_T *q)
{
    return(skb_queue_empty((struct sk_buff_head *) q));
}

void a_netbuf_queue_init(A_NETBUF_QUEUE_T *q)
{
    skb_queue_head_init((struct sk_buff_head *) q);
}

void *
a_netbuf_alloc(int size)
{
    struct sk_buff *skb;
    skb = dev_alloc_skb(AR6000_DATA_OFFSET + sizeof(HTC_PACKET) + size);
    skb_reserve(skb, AR6000_DATA_OFFSET + sizeof(HTC_PACKET));
    return ((void *)skb);
}

/*
 * Allocate an SKB w.o. any encapsulation requirement.
 */
void *
a_netbuf_alloc_raw(int size)
{
    struct sk_buff *skb;

    skb = dev_alloc_skb(size);

    return ((void *)skb);
}

void
a_netbuf_free(void *bufPtr)
{
    struct sk_buff *skb = (struct sk_buff *)bufPtr;

    dev_kfree_skb(skb);
}

A_UINT32
a_netbuf_to_len(void *bufPtr)
{
    return (((struct sk_buff *)bufPtr)->len);
}

void *
a_netbuf_to_data(void *bufPtr)
{
    return (((struct sk_buff *)bufPtr)->data);
}

/*
 * Add len # of bytes to the beginning of the network buffer
 * pointed to by bufPtr
 */
A_STATUS
a_netbuf_push(void *bufPtr, A_INT32 len)
{
    skb_push((struct sk_buff *)bufPtr, len);

    return A_OK;
}

/*
 * Add len # of bytes to the beginning of the network buffer
 * pointed to by bufPtr and also fill with data
 */
A_STATUS
a_netbuf_push_data(void *bufPtr, char *srcPtr, A_INT32 len)
{
    skb_push((struct sk_buff *) bufPtr, len);
    A_MEMCPY(((struct sk_buff *)bufPtr)->data, srcPtr, len);

    return A_OK;
}

/*
 * Add len # of bytes to the end of the network buffer
 * pointed to by bufPtr
 */
A_STATUS
a_netbuf_put(void *bufPtr, A_INT32 len)
{
    skb_put((struct sk_buff *)bufPtr, len);

    return A_OK;
}

/*
 * Add len # of bytes to the end of the network buffer
 * pointed to by bufPtr and also fill with data
 */
A_STATUS
a_netbuf_put_data(void *bufPtr, char *srcPtr, A_INT32 len)
{
    char *start = ((struct sk_buff *)bufPtr)->data +
        ((struct sk_buff *)bufPtr)->len;
    skb_put((struct sk_buff *)bufPtr, len);
    A_MEMCPY(start, srcPtr, len);

    return A_OK;
}


/*
 * Trim the network buffer pointed to by bufPtr to len # of bytes
 */
A_STATUS
a_netbuf_setlen(void *bufPtr, A_INT32 len)
{
    skb_trim((struct sk_buff *)bufPtr, len);

    return A_OK;
}

/*
 * Chop of len # of bytes from the end of the buffer.
 */
A_STATUS
a_netbuf_trim(void *bufPtr, A_INT32 len)
{
    skb_trim((struct sk_buff *)bufPtr, ((struct sk_buff *)bufPtr)->len - len);

    return A_OK;
}

/*
 * Chop of len # of bytes from the end of the buffer and return the data.
 */
A_STATUS
a_netbuf_trim_data(void *bufPtr, char *dstPtr, A_INT32 len)
{
    char *start = ((struct sk_buff *)bufPtr)->data +
        (((struct sk_buff *)bufPtr)->len - len);

    A_MEMCPY(dstPtr, start, len);
    skb_trim((struct sk_buff *)bufPtr, ((struct sk_buff *)bufPtr)->len - len);

    return A_OK;
}


/*
 * Returns the number of bytes available to a a_netbuf_push()
 */
A_INT32
a_netbuf_headroom(void *bufPtr)
{
    return (skb_headroom((struct sk_buff *)bufPtr));
}

/*
 * Removes specified number of bytes from the beginning of the buffer
 */
A_STATUS
a_netbuf_pull(void *bufPtr, A_INT32 len)
{
    skb_pull((struct sk_buff *)bufPtr, len);

    return A_OK;
}

/*
 * Removes specified number of bytes from the beginning of the buffer
 * and return the data
 */
A_STATUS
a_netbuf_pull_data(void *bufPtr, char *dstPtr, A_INT32 len)
{
    A_MEMCPY(dstPtr, ((struct sk_buff *)bufPtr)->data, len);
    skb_pull((struct sk_buff *)bufPtr, len);

    return A_OK;
}

