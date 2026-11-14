/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#ifndef _UAPI_LINUX_DVF99_VOICE_H
#define _UAPI_LINUX_DVF99_VOICE_H

#include <linux/ioctl.h>
#include <linux/types.h>

enum dvf99_voice_codec {
	DVF99_VOICE_CODEC_PCMU,
	DVF99_VOICE_CODEC_PCMA,
	DVF99_VOICE_CODEC_G722,
	DVF99_VOICE_CODEC_G729,
	DVF99_VOICE_CODEC_G726_16,
	DVF99_VOICE_CODEC_G726_24,
	DVF99_VOICE_CODEC_G726_32,
	DVF99_VOICE_CODEC_G726_40,
	DVF99_VOICE_CODEC_ILBC_13,
	DVF99_VOICE_CODEC_ILBC_15,
	DVF99_VOICE_CODEC_L16,
	DVF99_VOICE_CODEC_MAX,
};

enum dvf99_voice_dir {
	DVF99_VOICE_DIR_SENDRECV,
	DVF99_VOICE_DIR_SENDONLY,
	DVF99_VOICE_DIR_RECVONLY,
	DVF99_VOICE_DIR_INACTIVE,
};

enum dvf99_voice_jb {
	DVF99_VOICE_JB_FIXED,
	DVF99_VOICE_JB_ADAPTIVE,
};

#define DVF99_VOICE_F_VAD		(1 << 0)
#define DVF99_VOICE_F_RTCP_MUX		(1 << 1)
#define DVF99_VOICE_F_RTCP		(1 << 2)
#define DVF99_VOICE_F_DETACHED		(1 << 3)

#define DVF99_VOICE_CNAME_LEN		32

#define DVF99_VOICE_PT_NONE		0xff

struct dvf99_voice_config {
	__u32 line;
	__u32 codec;
	__u32 flags;
	__u32 ssrc;
	__u32 timestamp;
	__u16 ptime_ms;
	__u8 tx_pt;
	__u8 rx_pt;
	__u8 tx_pt_event;
	__u8 rx_pt_event;
	__u8 direction;
	__u8 jb_mode;
	__u8 rtcp_interval_s;
	__u8 reserved;
	char cname[DVF99_VOICE_CNAME_LEN];
};

struct dvf99_voice_dtmf {
	__u8 event;
	__u8 volume;
	__u16 duration_ms;
};

#define DVF99_VOICE_PKT_RTP		1
#define DVF99_VOICE_PKT_RTCP		2
#define DVF99_VOICE_PKT_DTMF		3
#define DVF99_VOICE_PKT_EVENT		4

enum dvf99_voice_event_type {
	DVF99_VOICE_EV_FIRST_RX,
	DVF99_VOICE_EV_SSRC,
};

struct dvf99_voice_pkt {
	__u32 type;
	__u32 ttl;
	__u32 time_ms;
	__u8 data[];
};

struct dvf99_voice_dtmf_rx {
	__u32 timestamp;
	__u16 duration;
	__u8 event;
	__u8 volume;
	__u8 end;
	__u8 reserved[3];
};

struct dvf99_voice_event {
	__u32 type;
	__u32 ssrc;
	__u32 timestamp;
	__u16 seq;
	__u8 pt;
	__u8 reserved;
	char codec[16];
};

struct dvf99_voice_stats {
	__u32 pkts_sent;
	__u32 pkts_recvd;
	__u32 bytes_sent;
	__u32 bytes_recvd;
	__u32 jitter;
	__u32 rtt;
	__u32 pkts_lost;
	__u32 pkts_dropped;
	__u32 decode_latency;
	__u8 rx_pt;
	__u8 reserved[3];
};

#define DVF99_VOICE_PKT_MAX		1500

#define DVF99_VOICE_IOC_MAGIC		0xd9

#define DVF99_VOICE_START	_IOW(DVF99_VOICE_IOC_MAGIC, 0, struct dvf99_voice_config)
#define DVF99_VOICE_UPDATE	_IOW(DVF99_VOICE_IOC_MAGIC, 1, struct dvf99_voice_config)
#define DVF99_VOICE_STOP	_IO(DVF99_VOICE_IOC_MAGIC, 2)
#define DVF99_VOICE_DTMF	_IOW(DVF99_VOICE_IOC_MAGIC, 3, struct dvf99_voice_dtmf)
#define DVF99_VOICE_STATS	_IOR(DVF99_VOICE_IOC_MAGIC, 4, struct dvf99_voice_stats)

#endif
