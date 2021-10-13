/*
 **************************************************************************
 * Copyright (c) 2014-2015, 2018, 2020 The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */
/*
 * nss_log.h
 *	NSS FW debug log memory header file
 */

#ifndef __NSS_LOG_H
#define __NSS_LOG_H

#define NSS_DEBUG_LOG_VERSION		0x1

/**
 * Dynamic Interface types
 */
enum nss_debug_interface_msg_type {
	NSS_DEBUG_INTERFACE_TYPE_NONE = 0,
	NSS_DEBUG_INTERFACE_TYPE_LOG_BUF_INIT = 1,
	NSS_DEBUG_INTERFACE_TYPE_MAX,
};

/*
 * The size of each log entry to be displayed.
 */
#define	NSS_LOG_OUTPUT_LINE_SIZE	151	/* 5 + 12 + 132 + '\n' + '\0' (see below) */
#define	NSS_LOG_LINE_FORMAT		"%3d: %010u: %s\n"
#define	NSS_LOG_LINE_WIDTH		132
#define	NSS_LOG_COOKIE			0xFF785634

/*
 * Dump last N entry during the coredump.
 * This number should be lower than the minimum size of the logbuf
 * which 32 right now.
 */
#define NSS_LOG_COREDUMP_LINE_NUM	25

/*
 * Saves the ring buffer address for logging per NSS core
 */
struct nss_log_ring_buffer_addr {
	void *addr;		/* Pointer to struct nss_log_descriptor */
	dma_addr_t dma_addr;	/* DMA Handle */
	uint32_t nentries;	/* Number of entries in the ring buffer */
	int ref_cnt;		/* Reference count */
};

/*
 * nss_log_entry is shared between Host and NSS FW
 */
struct nss_log_entry {
	uint64_t sequence_num;		/* Sequence number */
	uint32_t cookie;		/* Magic for verification */
	uint32_t thread_num;		/* thread-id */
	uint32_t timestamp;		/* timestamp in ticks */
	char message[NSS_LOG_LINE_WIDTH];	/* actual debug message */
} __attribute__((aligned(NSS_CACHE_LINE_SIZE)));

/*
 * The NSS log descripts holds ring-buffer along with other variables and
 * it is shared between NSS FW and Host.
 *
 * NSS FW writes to ring buffer and current_entry but read by only Host.
 */
struct nss_log_descriptor {
	uint32_t cookie;		/* Magic for verification */
	uint32_t log_nentries;		/* No.of log entries */
	uint32_t current_entry;		/* pointer to current log entry */
	uint8_t  pad[20];			/* pad to align ring buffer at cacheline boundary */
	struct nss_log_entry log_ring_buffer[0];	/* The actual log entry ring buffer */
} __attribute__((aligned(NSS_CACHE_LINE_SIZE)));

struct nss_log_debug_memory_msg {
	uint32_t version;
	uint32_t nentry;
	uint32_t phy_addr;
};

struct nss_log_debug_interface_msg {
	struct nss_cmn_msg cm;
	union {
		struct nss_log_debug_memory_msg addr;
	} msg;
};

/**
 * @brief Callback to receive debug interface messages
 *
 * @param app_data Application context of the message
 * @param msg Message data
 *
 * @return void
 */
typedef void (*nss_log_msg_callback_t)(void *app_data, struct nss_log_debug_interface_msg *msg);

/*
 * Exported by nss_init.c and used in nss_log.c
 */
extern int nss_ctl_logbuf;

extern struct nss_log_ring_buffer_addr nss_rbe[NSS_MAX_CORES];

#endif /* __NSS_LOG_H */
