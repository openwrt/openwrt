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


#ifndef _DESS_FDB_PRV_H_
#define _DESS_FDB_PRV_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */


#define ARL_FLUSH_ALL             1
#define ARL_LOAD_ENTRY            2
#define ARL_PURGE_ENTRY           3
#define ARL_FLUSH_ALL_UNLOCK      4
#define ARL_FLUSH_PORT_UNICAST    5
#define ARL_NEXT_ENTRY            6
#define ARL_FIND_ENTRY            7
#define ARL_TRANSFER_ENTRY        8

#define ARL_FIRST_ENTRY           1001
#define ARL_FLUSH_PORT_NO_STATIC  1002
#define ARL_FLUSH_PORT_AND_STATIC 1003

#define DESS_MAX_FID                   4095
#define DESS_MAX_LEARN_LIMIT_CNT       2048
#define DESS_MAX_PORT_LEARN_LIMIT_CNT  1024

    sw_error_t
    inter_dess_fdb_flush(a_uint32_t dev_id, a_uint32_t flag);


#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _DESS_FDB_PRV_H_ */

