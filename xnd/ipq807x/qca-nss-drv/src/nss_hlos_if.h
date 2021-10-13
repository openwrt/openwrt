/*
 **************************************************************************
 * Copyright (c) 2013-2019, 2021, The Linux Foundation. All rights reserved.
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
 * nss_hlos_if.h
 *	NSS to HLOS interface definitions.
 */

#ifndef __NSS_HLOS_IF_H
#define __NSS_HLOS_IF_H

#define NSS_MIN_NUM_CONN			256		/* MIN Connection shared between IPv4 and IPv6 */
#define NSS_FW_DEFAULT_NUM_CONN			1024		/* Firmware default number of connections for IPv4 and IPv6 */
#define NSS_NUM_CONN_QUANTA_MASK		(1024 - 1)	/* Quanta of number of connections 1024 */
#define NSS_CONN_CFG_TIMEOUT			6000		/* 6 sec timeout for connection cfg message */

/*
 * The following definitions sets the maximum number of connections
 * based on the type of memory profile that the system is operating with
 */
#if defined (NSS_MEM_PROFILE_LOW)
#define NSS_DEFAULT_NUM_CONN			512		/* Default number of connections for IPv4 and IPv6 each, for low memory profile */
#if defined (NSS_DRV_IPV6_ENABLE)
#define NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6	1024		/* MAX Connection shared between IPv4 and IPv6 for low memory profile */
#else
#define NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6	512		/* MAX Connection for IPv4 for low memory profile */
#endif
#define NSS_LOW_MEM_EMPTY_POOL_BUF_SZ		4096		/* Default empty buffer pool size for low profile */
#elif defined (NSS_MEM_PROFILE_MEDIUM)
#define NSS_DEFAULT_NUM_CONN			2048		/* Default number of connections for IPv4 and IPv6 each, for medium memory profile */
#define NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6	4096		/* MAX Connection shared between IPv4 and IPv6 for medium memory profile */
#else
#define NSS_DEFAULT_NUM_CONN			4096		/* Default number of connections for each IPv4 and IPv6 */
#define NSS_MAX_TOTAL_NUM_CONN_IPV4_IPV6	8192		/* MAX Connection shared between IPv4 and IPv6 */
#endif

#if defined(NSS_SKB_FIXED_SIZE_2K) && !defined(__LP64__)
#define NSS_EMPTY_BUFFER_SIZE			1792		/* Default buffer size for reduced memory profiles. */
#define NSS_FIXED_BUFFER_SIZE					/* For low memory profiles, maximum buffer size/MTU is fixed */
#else
#define NSS_EMPTY_BUFFER_SIZE			1984		/* Default buffer size for regular memory profiles. */
#undef NSS_FIXED_BUFFER_SIZE
#endif

enum {
	NSS_SUCCESS = 0,
	NSS_FAILURE = 1,
};

/*
 * Request/Response types
 */
enum nss_if_metadata_types {
	NSS_TX_METADATA_TYPE_INTERFACE_OPEN,
	NSS_TX_METADATA_TYPE_INTERFACE_CLOSE,
	NSS_TX_METADATA_TYPE_INTERFACE_LINK_STATE_NOTIFY,
	NSS_TX_METADATA_TYPE_INTERFACE_MTU_CHANGE,
	NSS_TX_METADATA_TYPE_INTERFACE_MAC_ADDR_SET,
	NSS_TX_METADATA_TYPE_INTERFACE_MSS_SET,
	NSS_RX_METADATA_TYPE_INTERFACE_STATS_SYNC,
	NSS_METADATA_TYPE_INTERFACE_MAX,
};

/*
 * General statistics messages
 */

/*
 * IPv4 reasm node stats
 */
struct nss_ipv4_reasm_stats_sync {
	struct nss_cmn_node_stats node_stats;
					/* Common node stats for ipv4_reasm */
	uint32_t ipv4_reasm_evictions;
	uint32_t ipv4_reasm_alloc_fails;
	uint32_t ipv4_reasm_timeouts;
};

/*
 * IPv4 reasm message types
 */
enum nss_ipv4_reasm_message_types {
	NSS_IPV4_REASM_STATS_SYNC_MSG,
};

/*
 * IPv4 reassembly message structure
 */
struct nss_ipv4_reasm_msg {
	struct nss_cmn_msg cm;
	union {
		struct nss_ipv4_reasm_stats_sync stats_sync;
	} msg;
};

/*
 * IPv6 reasm node stats
 */
struct nss_ipv6_reasm_stats_sync {
	struct nss_cmn_node_stats node_stats;
					/* Common node stats for ipv6_reasm */
	uint32_t ipv6_reasm_alloc_fails;
	uint32_t ipv6_reasm_timeouts;
	uint32_t ipv6_reasm_discards;
};

/*
 * IPv6 reasm message types
 */
enum nss_ipv6_reasm_message_types {
	NSS_IPV6_REASM_STATS_SYNC_MSG,
};

/*
 * IPv6 reassembly message structure
 */
struct nss_ipv6_reasm_msg {
	struct nss_cmn_msg cm;
	union {
		struct nss_ipv6_reasm_stats_sync stats_sync;
	} msg;
};

/*
 * Generic interface messages
 */
enum nss_generic_metadata_types {
	NSS_TX_METADATA_TYPE_GENERIC_IF_PARAMS,
	NSS_METADATA_TYPE_GENERIC_IF_MAX
};

/*
 * Interface params command
 */
struct nss_generic_if_params {
	uint8_t buf[1];		/* Buffer */
};

/*
 * Message structure to send/receive ipsec messages
 */
struct nss_generic_msg {
	struct nss_cmn_msg cm;			/* Message Header */
	union {
		struct nss_generic_if_params rule;	/* Message: generic rule */
	} msg;
};

/*
 * NSS frequency scaling messages
 */
enum nss_freq_stats_metadata_types {
	COREFREQ_METADATA_TYPE_ERROR,
	COREFREQ_METADATA_TYPE_RX_FREQ_CHANGE,
	COREFREQ_METADATA_TYPE_TX_FREQ_ACK,
	COREFREQ_METADATA_TYPE_TX_CORE_STATS,
	COREFREQ_METADATA_TYPE_MAX,
};

 /*
 * Types of TX metadata -- legacy code needs to be removed
 */
enum nss_tx_metadata_types {
	NSS_TX_METADATA_TYPE_LEGACY_0,
	NSS_TX_METADATA_TYPE_NSS_FREQ_CHANGE,
	NSS_TX_METADATA_TYPE_SHAPER_CONFIGURE,
};

/*
 * The NSS freq start or stop strcture
 */
struct nss_freq_msg {
	/* Request */
	uint32_t frequency;
	uint32_t start_or_end;
	uint32_t stats_enable;

	/* Response */
	uint32_t freq_current;
	int32_t ack;
};

/*
 * NSS core stats
 */
struct nss_core_stats {
	uint32_t inst_cnt_total;
};

/*
 * Message structure to send/receive NSS Freq commands
 */
struct nss_corefreq_msg {
	struct nss_cmn_msg cm;			/* Message Header */
	union {
		struct nss_freq_msg nfc;	/* Message: freq stats */
		struct nss_core_stats ncs;	/* Message: NSS stats sync */
	} msg;
};

/*
 * H2N Buffer Types
 */
#define H2N_BUFFER_EMPTY			0
#define H2N_PAGED_BUFFER_EMPTY			1
#define H2N_BUFFER_PACKET			2
#define H2N_BUFFER_CTRL				4
#define H2N_BUFFER_NATIVE_WIFI			8
#define H2N_BUFFER_SHAPER_BOUNCE_INTERFACE	9
#define H2N_BUFFER_SHAPER_BOUNCE_BRIDGE		10
#define H2N_BUFFER_RATE_TEST			14
#define H2N_BUFFER_MAX				16

/*
 * H2N Bit Flag Definitions
 */
#define H2N_BIT_FLAG_GEN_IPV4_IP_CHECKSUM		0x0001
#define H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM		0x0002
#define H2N_BIT_FLAG_FIRST_SEGMENT			0x0004
#define H2N_BIT_FLAG_LAST_SEGMENT			0x0008

#define H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM_NONE	0x0010
#define H2N_BIT_FLAG_TX_TS_REQUIRED			0x0040
#define H2N_BIT_FLAG_DISCARD				0x0080
#define H2N_BIT_FLAG_SEGMENTATION_ENABLE		0x0100

#define H2N_BIT_FLAG_VIRTUAL_BUFFER			0x2000
#define H2N_BIT_FLAG_BUFFER_REUSABLE			0x8000

/*
 * HLOS to NSS descriptor structure.
 */
struct h2n_descriptor {
	uint32_t interface_num;	/* Interface number to which the buffer is to be sent (where appropriate) */
	uint32_t buffer;	/* Physical buffer address. This is the address of the start of the usable buffer being provided by the HLOS */
	uint32_t qos_tag;	/* QoS tag information of the buffer (where appropriate) */
	uint16_t buffer_len;	/* Length of the buffer (in bytes) */
	uint16_t payload_len;	/* Length of the active payload of the buffer (in bytes) */
	uint16_t mss;		/* MSS to be used with TSO/UFO */
	uint16_t payload_offs;	/* Offset from the start of the buffer to the start of the payload (in bytes) */
	uint16_t bit_flags;	/* Bit flags associated with the buffer */
	uint8_t buffer_type;	/* Type of buffer */
	uint8_t reserved;	/* Reserved for future use */
	nss_ptr_t opaque;	/* 32 or 64-bit value provided by the HLOS to associate with the buffer. The cookie has no meaning to the NSS */
#ifndef __LP64__
	uint32_t padding;	/* Pad to fit 64bits, do not reuse */
#endif
};

/*
 * N2H Buffer Types
 */
#define N2H_BUFFER_EMPTY			1
#define N2H_BUFFER_PACKET			3
#define N2H_BUFFER_COMMAND_RESP			5
#define N2H_BUFFER_STATUS			6
#define N2H_BUFFER_CRYPTO_RESP			8
#define N2H_BUFFER_PACKET_VIRTUAL		10
#define N2H_BUFFER_SHAPER_BOUNCED_INTERFACE	11
#define N2H_BUFFER_SHAPER_BOUNCED_BRIDGE	12
#define N2H_BUFFER_PACKET_EXT			13
#define N2H_BUFFER_RATE_TEST			14
#define N2H_BUFFER_MAX				16

/*
 * Command Response Types
 */
#define N2H_COMMAND_RESP_OK			0
#define N2H_COMMAND_RESP_BUFFER_TOO_SMALL	1
#define N2H_COMMAND_RESP_BUFFER_NOT_WRITEABLE	2
#define N2H_COMMAND_RESP_UNSUPPORTED_COMMAND	3
#define N2H_COMMAND_RESP_INVALID_PARAMETERS	4
#define N2H_COMMAND_RESP_INACTIVE_SUBSYSTEM	5

/*
 * N2H Bit Flag Definitions
 */
#define N2H_BIT_FLAG_IPV4_IP_CHECKSUM_VALID		0x0001
#define N2H_BIT_FLAG_IP_TRANSPORT_CHECKSUM_VALID	0x0002
#define N2H_BIT_FLAG_FIRST_SEGMENT			0x0004
#define N2H_BIT_FLAG_LAST_SEGMENT			0x0008
#define N2H_BIT_FLAG_INGRESS_SHAPED			0x0010

/*
 * NSS to HLOS descriptor structure
 */
struct n2h_descriptor {
	uint32_t interface_num;	/* Interface number to which the buffer is to be sent (where appropriate) */
	uint32_t buffer;	/* Physical buffer address. This is the address of the start of the usable buffer being provided by the HLOS */
	uint16_t buffer_len;	/* Length of the buffer (in bytes) */
	uint16_t payload_len;	/* Length of the active payload of the buffer (in bytes) */
	uint16_t payload_offs;	/* Offset from the start of the buffer to the start of the payload (in bytes) */
	uint16_t bit_flags;	/* Bit flags associated with the buffer */
	uint8_t buffer_type;	/* Type of buffer */
	uint8_t response_type;	/* Response type if the buffer is a command response */
	uint8_t pri;		/* Packet priority */
	uint8_t service_code;	/* Service code */
	uint32_t reserved;	/* Reserved for future use */
	nss_ptr_t opaque;	/* 32 or 64-bit value provided by the HLOS to associate with the buffer. The cookie has no meaning to the NSS */
#ifndef __LP64__
	uint32_t padding;	/* Pad to fit 64 bits, do not reuse */
#endif
};

/*
 * Device Memory Map Definitions
 */
#define DEV_MAGIC		0x4e52522e
#define DEV_INTERFACE_VERSION	1
#define DEV_DESCRIPTORS		256 /* Do we need it here? */

/**
 * H2N descriptor METADATA
 */
struct h2n_desc_if_meta {
	uint32_t desc_addr;
	uint16_t size;
	uint16_t padding;
};

/**
 * H2N descriptor ring
 */
struct h2n_desc_if_instance {
	struct h2n_descriptor *desc;
	uint16_t size;			/* Size in entries of the H2N0 descriptor ring */
};

/**
 * N2H descriptor METADATA
 */
struct n2h_desc_if_meta {
	uint32_t desc_addr;
	uint16_t size;
	uint16_t padding;
};

/**
 * N2H descriptor ring
 */
struct n2h_desc_if_instance {
	struct n2h_descriptor *desc;
	uint16_t size;			/* Size in entries of the H2N0 descriptor ring */
};

/**
 * NSS virtual interface map
 */
struct nss_if_mem_map {
	struct h2n_desc_if_meta h2n_desc_if[16];/* Base address of H2N0 descriptor ring */
	struct n2h_desc_if_meta n2h_desc_if[15];/* Base address of N2H0 descriptor ring */
	uint32_t magic;				/* Magic value used to identify NSS implementations (must be 0x4e52522e) */
	uint16_t if_version;			/* Interface version number (must be 1 for this version) */
	uint8_t h2n_rings;			/* Number of descriptor rings in the H2N direction */
	uint8_t n2h_rings;			/* Number of descriptor rings in the N2H direction */
	uint32_t h2n_nss_index[16];
			/* Index number for the next descriptor that will be read by the NSS in the H2N0 descriptor ring (NSS owned) */
	volatile uint32_t n2h_nss_index[15];
			/* Index number for the next descriptor that will be written by the NSS in the N2H0 descriptor ring (NSS owned) */
	uint8_t num_phys_ports;
	uint8_t reserved1[3];	/* Reserved for future use */
	uint32_t h2n_hlos_index[16];
			/* Index number for the next descriptor that will be written by the HLOS in the H2N0 descriptor ring (HLOS owned) */
	volatile uint32_t n2h_hlos_index[15];
			/* Index number for the next descriptor that will be read by the HLOS in the N2H0 descriptor ring (HLOS owned) */
	uint32_t reserved;	/* Reserved for future use */
};
#endif /* __NSS_HLOS_IF_H */
