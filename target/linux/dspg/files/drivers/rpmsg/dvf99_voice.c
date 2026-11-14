// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2007 NXP Semiconductors
 * Copyright (C) 2008 - 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/completion.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/hrtimer.h>
#include <linux/kref.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/random.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/rpmsg/dvf99_dua.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/dvf99_voice.h>

#define VOICE_SESSIONS		16
#define VOICE_LINES		4
#define VOICE_FIFO_SIZE		20480
#define VOICE_TIMEOUT		msecs_to_jiffies(500)
#define VOICE_POLL_NS		(4 * NSEC_PER_MSEC)
#define VOICE_CODECS		20
#define VOICE_PT_NAME_LEN	15
#define VOICE_CODEC_NAME_LEN	16
#define VOICE_SYMM_TX_CNT	10
#define VOICE_DTMF_END_PKTS	3
#define VOICE_UNIT		"UT_VOIP"
#define VOICE_EVENT_SERVICE	12
#define VOICE_EVENTS		16
#define VOICE_DTMF_VOLUME_MAX	63
#define VOICE_DTMF_EVENT_MAX	16
#define VOICE_CLOCK_KHZ		8
#define VOICE_CLOCK_WB_KHZ	16

enum voice_msg {
	VOICE_GET_SESSION = 0,
	VOICE_SET_SESSION_FIFOS = 2,
	VOICE_START_SESSION = 4,
	VOICE_STOP_SESSION = 6,
	VOICE_FREE_SESSION = 8,
	VOICE_SEND_DTMF = 10,
	VOICE_START_RTCP = 13,
	VOICE_UPDATE_SESSION = 19,
	VOICE_UPDATE_RTCP = 24,
};

#define VOICE_RTCP_RX		0x01
#define VOICE_RTCP_TX		0x02
#define VOICE_RTCP_USER		0x20
#define VOICE_RTCP_SDES_ITEMS	8
#define VOICE_RTCP_SDES_LEN	50

enum voice_event_msg {
	VOICE_EVT_GET_STATS = 1,
	VOICE_EVT_NOTIFY_START,
	VOICE_EVT_NOTIFY_STOP,
};

enum voice_event_type {
	VOICE_EVT_RX_DTMF = 1,
	VOICE_EVT_STATS = 8,
	VOICE_EVT_FIRST_RX = 0x20000,
	VOICE_EVT_SSRC = 0x100000,
};

#define VOICE_EVT_MASK		(VOICE_EVT_FIRST_RX | VOICE_EVT_SSRC)

enum voice_audio_mode {
	VOICE_MODE_SEND_ONLY = 1,
	VOICE_MODE_RECV_ONLY,
	VOICE_MODE_ACTIVE,
	VOICE_MODE_INACTIVE,
};

#define VOICE_OPT_APP		0x00080000
#define VOICE_RTP_MODE_APP	3

#define VOICE_CODEC_OPT_VAD		0x00000004
#define VOICE_CODEC_OPT_ILBC_15K2	0x00000100
#define VOICE_CODEC_OPT_DTMF		0x00002000
#define VOICE_CODEC_OPT_JIB		0x00100000

#define VOICE_JIB_MAX		160
#define VOICE_JIB_MIN		60
#define VOICE_JIB_RESYNC	3
#define VOICE_JIB_TARGET	20
#define VOICE_JIB_MONITOR	1500
#define VOICE_JIB_STEP_RESET	60
#define VOICE_JIB_SLOPE		24
#define VOICE_JIB_POS_STEP	960
#define VOICE_JIB_POS_STEP_MED	480

#define VOICE_CNG_MAX_SID	200
#define VOICE_CNG_VAD_LEVEL	40
#define VOICE_CNG_HANGOVER	50
#define VOICE_CNG_LEVEL		10

#define VOICE_RETRANS_SIZE	180
#define VOICE_REDUNDANCY_LEVEL	3
#define VOICE_REDUNDANCY_PT	121
#define VOICE_REPORT_PT_CHANGE	5

#define VOICE_PT_ANY		0xff

struct voice_cng {
	__le32 mode_rx;
	__le32 mode_tx;
	__le32 max_sid_update;
	__le32 vad_detect_level;
	__le32 vad_hangover;
	__le32 level_rx;
	__le32 level_tx;
} __packed;

struct voice_pt {
	u8 pt;
	char name[VOICE_PT_NAME_LEN];
} __packed;

struct voice_codec {
	__le32 duration;
	__le32 opts;
	__le32 timestamp;
	__le32 ssrc;
	__le32 rx_pt_event;
	__le32 tx_pt;
	__le32 tx_pt_event;
	struct voice_cng cng;
	char name[VOICE_CODEC_NAME_LEN];
	struct voice_pt rx_list[VOICE_CODECS];
	__le32 vbr[32];
} __packed;

struct voice_jib {
	__le32 max_len;
	__le32 min_len;
	__le32 type;
	__le32 th_resync;
	__le32 target_delay;
	__le32 monitoring_interval;
	__le32 stepsize_reset_time;
	__le32 slope;
	__le32 pos_step;
	__le32 pos_step_medium;
} __packed;

struct voice_redundancy {
	__le32 mode;
	__le32 level;
	__le32 tx_pt_audio;
	__le32 rx_pt_audio;
	__le32 tx_pt_dtmf;
	__le32 rx_pt_dtmf;
	struct voice_pt codecs[3];
} __packed;

struct voice_session_cfg {
	__le32 dtmf_end_pkts;
	__le32 opts;
	__le32 symm_tx_cnt;
	__le32 current_time;
	__le32 audio_mode;
	__le32 loop_level;
	__le32 rtcp_mux;
	__le32 rtp_mode;
	struct voice_jib jib;
	__le32 t38[3];
	struct voice_codec codec;
	__le32 sid_update;
	__le32 retrans_mode;
	__le32 retrans_size;
	__le32 srtp_opts;
	u8 srtp_key_loc[48];
	u8 srtp_key_dist[48];
	struct voice_redundancy redundancy;
	__le32 voip_line_id;
	__le32 session_id;
	__le16 report_ssrc_change;
	__le16 report_pt_change;
} __packed;

struct voice_dtmf_event {
	__le32 status;
	__le32 duration;
	__le32 evt_duration;
	__le32 max_evt_duration;
	__le32 opts;
	__le32 event;
	__le32 volume;
	__le32 reserved;
} __packed;

struct voice_rtcp_cfg {
	__le32 interval;
	__le32 opts;
	char sdes[VOICE_RTCP_SDES_ITEMS][VOICE_RTCP_SDES_LEN];
	__le32 fb_type;
	__le32 fb_bw;
	__le32 fb_trr_interval;
	__le32 adaptation[5];
	__le32 max_rtt;
	__le32 gmin;
	__le32 thinning;
	__le32 rb_type;
} __packed;

struct voice_params {
	__le32 session_id;
	__le32 line_id;
	__le32 result;
	union {
		struct {
			__le32 enc;
			__le32 dec;
		} fifos;
		struct voice_session_cfg session;
		struct voice_dtmf_event dtmf;
		struct voice_rtcp_cfg rtcp;
	};
} __packed;

struct voice_msg_buf {
	struct dvf99_coma_cmsg hdr;
	struct voice_params params;
} __packed;

struct voice_evt_req {
	struct dvf99_coma_cmsg hdr;
	__le32 msg_id;
	__le32 id;
	__le32 opts;
} __packed;

struct voice_evt_hdr {
	__le32 id;
	__le32 type;
	__le32 length;
} __packed;

struct voice_evt_dtmf {
	u8 event;
	u8 marker;
	u8 end;
	u8 volume;
	__le32 timestamp;
	__le16 duration;
	u8 reserved;
	u8 pad;
} __packed;

struct voice_evt_session {
	u8 version;
	u8 padding;
	u8 extension;
	u8 csrc_count;
	u8 marker;
	u8 pt;
	__le16 payload_len;
	__be16 seq;
	char codec[18];
	__be32 timestamp;
	__be32 ssrc;
} __packed;

struct voice_evt_stats {
	__le32 sys[4];
	__le32 pkts_sent;
	__le32 pkts_recvd;
	__le32 bytes_sent;
	__le32 bytes_recvd;
	__le32 jitter;
	__le32 rtt;
	__le32 pkts_lost;
	__le32 pkts_dropped;
	__le32 decode_latency;
	u8 rx_pt;
	u8 pad[3];
	__le32 interval_min;
	__le32 interval_max;
} __packed;

struct voice_codec_desc {
	const char *name;
	u32 opts;
};

static const struct voice_codec_desc voice_codecs[DVF99_VOICE_CODEC_MAX] = {
	[DVF99_VOICE_CODEC_PCMU] = { "pcmu/8000", 0 },
	[DVF99_VOICE_CODEC_PCMA] = { "pcma/8000", 0 },
	[DVF99_VOICE_CODEC_G722] = { "G722/8000", 0 },
	[DVF99_VOICE_CODEC_G729] = { "G729/8000", 0 },
	[DVF99_VOICE_CODEC_G726_16] = { "G726-16/8000", 0 },
	[DVF99_VOICE_CODEC_G726_24] = { "G726-24/8000", 0 },
	[DVF99_VOICE_CODEC_G726_32] = { "G726-32/8000", 0 },
	[DVF99_VOICE_CODEC_G726_40] = { "G726-40/8000", 0 },
	[DVF99_VOICE_CODEC_ILBC_13] = { "iLBC/8000", 0 },
	[DVF99_VOICE_CODEC_ILBC_15] = { "iLBC/8000", VOICE_CODEC_OPT_ILBC_15K2 },
	[DVF99_VOICE_CODEC_L16] = { "L16/16000", 0 },
};

static const char voice_dtmf_chars[VOICE_DTMF_EVENT_MAX + 1] = "0123456789*#ABCDR";

struct voice_dev {
	struct kref ref;
	struct rpmsg_device *rpdev;
	struct device *dma_dev;
	struct dvf99_dua *dua;
	struct miscdevice misc;
	bool dead;

	struct mutex lock;
	struct voice_session *sessions[VOICE_SESSIONS];

	struct mutex setup_lock;
	struct mutex req_lock;
	struct completion req_done;
	u32 req_type;
	s32 req_result;
	struct voice_msg_buf req;

	struct rpmsg_endpoint *evt_ept;
	struct mutex evt_lock;
	struct completion evt_done;
	struct voice_evt_stats evt_stats;
};

struct voice_record {
	struct dvf99_voice_pkt hdr;
	union {
		struct dvf99_voice_dtmf_rx dtmf;
		struct dvf99_voice_event event;
	};
};

struct voice_session {
	struct voice_dev *vd;
	unsigned int id;
	struct dvf99_cfifo *enc;
	struct dvf99_cfifo *dec;
	wait_queue_head_t wq;
	struct hrtimer timer;

	struct mutex lock;
	struct mutex rd_lock;
	struct mutex wr_lock;
	bool started;
	bool rtcp;
	int uid;
	int conn;
	struct dvf99_voice_config cfg;
	u8 wbuf[sizeof(struct dvf99_voice_pkt) + DVF99_VOICE_PKT_MAX];

	spinlock_t evq_lock;
	unsigned int evq_head;
	unsigned int evq_tail;
	struct voice_record evq[VOICE_EVENTS];
};

static DEFINE_MUTEX(voice_open_lock);

static void voice_dev_release(struct kref *ref)
{
	struct voice_dev *vd = container_of(ref, struct voice_dev, ref);

	dvf99_dua_put(vd->dua);
	put_device(vd->dma_dev);
	mutex_destroy(&vd->evt_lock);
	mutex_destroy(&vd->setup_lock);
	mutex_destroy(&vd->req_lock);
	mutex_destroy(&vd->lock);
	kfree(vd);
}

static int voice_request(struct voice_dev *vd, struct voice_session *vs,
			 enum voice_msg type, struct voice_params *params)
{
	struct voice_msg_buf *msg = &vd->req;
	int ret;

	mutex_lock(&vd->req_lock);

	if (vd->dead) {
		ret = -ENODEV;
		goto out;
	}

	msg->hdr.type = cpu_to_le32(type);
	msg->hdr.params_size = cpu_to_le32(sizeof(msg->params));
	msg->hdr.payload_size = 0;
	msg->params = *params;
	msg->params.session_id = cpu_to_le32(vs->id);

	vd->req_type = type + 1;
	reinit_completion(&vd->req_done);

	ret = rpmsg_send(vd->rpdev->ept, msg, sizeof(*msg));
	if (ret)
		goto out;

	if (!wait_for_completion_timeout(&vd->req_done, VOICE_TIMEOUT)) {
		dev_err(&vd->rpdev->dev, "session %u: request %u timed out\n",
			vs->id, type);
		ret = -ETIMEDOUT;
		goto out;
	}

	if (vd->req_result) {
		dev_err(&vd->rpdev->dev, "session %u: request %u failed: %d\n",
			vs->id, type, vd->req_result);
		ret = -EIO;
	}

out:
	mutex_unlock(&vd->req_lock);

	return ret;
}

static int voice_simple(struct voice_session *vs, enum voice_msg type)
{
	struct voice_params params = { };

	params.line_id = cpu_to_le32(vs->cfg.line);

	return voice_request(vs->vd, vs, type, &params);
}

static int voice_evt_send(struct voice_dev *vd, enum voice_event_msg msg,
			  unsigned int id, u32 opts)
{
	struct voice_evt_req req = {
		.hdr.type = cpu_to_le32(VOICE_EVENT_SERVICE),
		.hdr.payload_size = cpu_to_le32(3 * sizeof(__le32)),
		.msg_id = cpu_to_le32(msg),
		.id = cpu_to_le32(id),
		.opts = cpu_to_le32(opts),
	};

	return rpmsg_send(vd->evt_ept, &req, sizeof(req));
}

static int voice_evt_notify(struct voice_session *vs, enum voice_event_msg msg)
{
	struct voice_dev *vd = vs->vd;
	int ret = -ENODEV;

	mutex_lock(&vd->evt_lock);
	if (!vd->dead)
		ret = voice_evt_send(vd, msg, vs->id, VOICE_EVT_MASK);
	mutex_unlock(&vd->evt_lock);

	return ret;
}

static int voice_cb(struct rpmsg_device *rpdev, void *data, int len,
		    void *priv, u32 src)
{
	struct voice_dev *vd = priv;
	struct dvf99_coma_cmsg *cmsg = data;
	struct voice_params *params;
	u32 type;

	if (len < sizeof(*cmsg) + offsetof(struct voice_params, fifos) ||
	    le32_to_cpu(cmsg->params_size) < offsetof(struct voice_params, fifos))
		return -EINVAL;

	params = (void *)cmsg->body;
	type = le32_to_cpu(cmsg->type);

	if (type != vd->req_type) {
		dev_warn_ratelimited(&rpdev->dev, "unexpected reply %u\n",
				     type);
		return 0;
	}

	vd->req_result = le32_to_cpu(params->result);
	complete(&vd->req_done);

	return 0;
}

static int voice_config_check(const struct dvf99_voice_config *cfg)
{
	if (cfg->line >= VOICE_LINES || cfg->codec >= DVF99_VOICE_CODEC_MAX ||
	    cfg->direction > DVF99_VOICE_DIR_INACTIVE ||
	    cfg->jb_mode > DVF99_VOICE_JB_ADAPTIVE || cfg->reserved)
		return -EINVAL;

	if (cfg->ptime_ms != 10 && cfg->ptime_ms != 20 && cfg->ptime_ms != 30)
		return -EINVAL;

	if (!memchr(cfg->cname, 0, sizeof(cfg->cname)))
		return -EINVAL;

	if ((cfg->flags & DVF99_VOICE_F_RTCP) && !cfg->cname[0])
		return -EINVAL;

	return 0;
}

static void voice_config_fill(struct voice_session *vs,
			      struct voice_params *params)
{
	static const u32 audio_modes[] = {
		[DVF99_VOICE_DIR_SENDRECV] = VOICE_MODE_ACTIVE,
		[DVF99_VOICE_DIR_SENDONLY] = VOICE_MODE_SEND_ONLY,
		[DVF99_VOICE_DIR_RECVONLY] = VOICE_MODE_RECV_ONLY,
		[DVF99_VOICE_DIR_INACTIVE] = VOICE_MODE_INACTIVE,
	};
	const struct dvf99_voice_config *cfg = &vs->cfg;
	const struct voice_codec_desc *desc = &voice_codecs[cfg->codec];
	struct voice_session_cfg *s = &params->session;
	struct voice_codec *c = &s->codec;
	u32 copts = desc->opts;
	unsigned int i;

	if (cfg->jb_mode == DVF99_VOICE_JB_ADAPTIVE)
		copts |= VOICE_CODEC_OPT_JIB;
	if (cfg->flags & DVF99_VOICE_F_VAD)
		copts |= VOICE_CODEC_OPT_VAD;
	if (cfg->tx_pt_event != DVF99_VOICE_PT_NONE ||
	    cfg->rx_pt_event != DVF99_VOICE_PT_NONE)
		copts |= VOICE_CODEC_OPT_DTMF;

	params->line_id = cpu_to_le32(cfg->line);

	s->dtmf_end_pkts = cpu_to_le32(VOICE_DTMF_END_PKTS);
	s->opts = cpu_to_le32(VOICE_OPT_APP);
	s->symm_tx_cnt = cpu_to_le32(VOICE_SYMM_TX_CNT);
	s->audio_mode = cpu_to_le32(audio_modes[cfg->direction]);
	s->rtcp_mux = cpu_to_le32(!!(cfg->flags & DVF99_VOICE_F_RTCP_MUX));
	s->rtp_mode = cpu_to_le32(VOICE_RTP_MODE_APP);

	s->jib.max_len = cpu_to_le32(VOICE_JIB_MAX);
	s->jib.min_len = cpu_to_le32(VOICE_JIB_MIN);
	s->jib.type = cpu_to_le32(cfg->jb_mode);
	s->jib.th_resync = cpu_to_le32(VOICE_JIB_RESYNC);
	s->jib.target_delay = cpu_to_le32(VOICE_JIB_TARGET);
	s->jib.monitoring_interval = cpu_to_le32(VOICE_JIB_MONITOR);
	s->jib.stepsize_reset_time = cpu_to_le32(VOICE_JIB_STEP_RESET);
	s->jib.slope = cpu_to_le32(VOICE_JIB_SLOPE);
	s->jib.pos_step = cpu_to_le32(VOICE_JIB_POS_STEP);
	s->jib.pos_step_medium = cpu_to_le32(VOICE_JIB_POS_STEP_MED);

	c->duration = cpu_to_le32(cfg->ptime_ms);
	c->opts = cpu_to_le32(copts);
	c->timestamp = cpu_to_le32(cfg->timestamp ?: get_random_u32());
	c->ssrc = cpu_to_le32(cfg->ssrc);
	c->tx_pt = cpu_to_le32(cfg->tx_pt);
	c->tx_pt_event = cpu_to_le32(cfg->tx_pt_event);
	c->rx_pt_event = cpu_to_le32(cfg->rx_pt_event);
	c->cng.max_sid_update = cpu_to_le32(VOICE_CNG_MAX_SID);
	c->cng.vad_detect_level = cpu_to_le32(VOICE_CNG_VAD_LEVEL);
	c->cng.vad_hangover = cpu_to_le32(VOICE_CNG_HANGOVER);
	c->cng.level_rx = cpu_to_le32(VOICE_CNG_LEVEL);
	c->cng.level_tx = cpu_to_le32(VOICE_CNG_LEVEL);
	strscpy(c->name, desc->name, sizeof(c->name));

	for (i = 0; i < VOICE_CODECS; i++)
		c->rx_list[i].pt = VOICE_PT_ANY;
	c->rx_list[0].pt = cfg->rx_pt;
	strscpy(c->rx_list[0].name, desc->name, sizeof(c->rx_list[0].name));

	s->retrans_size = cpu_to_le32(VOICE_RETRANS_SIZE);
	s->redundancy.level = cpu_to_le32(VOICE_REDUNDANCY_LEVEL);
	s->redundancy.tx_pt_audio = cpu_to_le32(VOICE_REDUNDANCY_PT);
	s->redundancy.rx_pt_audio = cpu_to_le32(VOICE_REDUNDANCY_PT);
	s->redundancy.tx_pt_dtmf = cpu_to_le32(VOICE_REDUNDANCY_PT);
	s->redundancy.rx_pt_dtmf = cpu_to_le32(VOICE_REDUNDANCY_PT);
	s->voip_line_id = cpu_to_le32(cfg->line);
	s->report_pt_change = cpu_to_le16(VOICE_REPORT_PT_CHANGE);
}

static enum hrtimer_restart voice_timer(struct hrtimer *timer)
{
	struct voice_session *vs = container_of(timer, struct voice_session,
						timer);

	if (!dvf99_cfifo_empty(vs->enc))
		wake_up_interruptible(&vs->wq);

	hrtimer_forward_now(timer, ns_to_ktime(VOICE_POLL_NS));

	return HRTIMER_RESTART;
}

static int voice_unit_setup(struct voice_session *vs)
{
	struct voice_dev *vd = vs->vd;
	int ret;

	ret = dvf99_dua_unit_alloc(vd->dua, VOICE_UNIT, vs->cfg.line);
	if (ret < 0)
		return ret;
	vs->uid = ret;

	return 0;
}

static int voice_unit_attach(struct voice_session *vs)
{
	struct voice_dev *vd = vs->vd;
	bool detached = vs->cfg.flags & DVF99_VOICE_F_DETACHED;
	int ret;

	if (detached && vs->conn >= 0) {
		ret = dvf99_dua_unit_disconnect(vd->dua, vs->uid, vs->conn);
		if (ret)
			return ret;
		vs->conn = -1;
	} else if (!detached && vs->conn < 0) {
		ret = dvf99_dua_audio_join(vd->dua, vs->uid);
		if (ret < 0)
			return ret;
		vs->conn = ret;
	}

	return 0;
}

static void voice_unit_release(struct voice_session *vs)
{
	struct voice_dev *vd = vs->vd;

	if (vs->conn >= 0)
		dvf99_dua_unit_disconnect(vd->dua, vs->uid, vs->conn);
	if (vs->uid >= 0)
		dvf99_dua_unit_free(vd->dua, vs->uid);
	vs->conn = -1;
	vs->uid = -1;
}

static int voice_rtcp_apply(struct voice_session *vs)
{
	bool on = vs->cfg.flags & DVF99_VOICE_F_RTCP;
	struct voice_params *params;
	u32 opts = VOICE_RTCP_USER | VOICE_RTCP_RX;
	int ret;

	if (!on && !vs->rtcp)
		return 0;

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params)
		return -ENOMEM;

	if (on)
		opts |= VOICE_RTCP_TX;

	params->line_id = cpu_to_le32(vs->cfg.line);
	params->rtcp.interval = cpu_to_le32(vs->cfg.rtcp_interval_s);
	params->rtcp.opts = cpu_to_le32(opts);
	strscpy(params->rtcp.sdes[0], vs->cfg.cname,
		sizeof(params->rtcp.sdes[0]));

	ret = voice_request(vs->vd, vs,
			    vs->rtcp ? VOICE_UPDATE_RTCP : VOICE_START_RTCP,
			    params);
	kfree(params);
	if (!ret)
		vs->rtcp = true;

	return ret;
}

static void voice_session_stop(struct voice_session *vs)
{
	if (!vs->started)
		return;

	vs->started = false;
	vs->rtcp = false;
	hrtimer_cancel(&vs->timer);
	voice_evt_notify(vs, VOICE_EVT_NOTIFY_STOP);
	voice_simple(vs, VOICE_STOP_SESSION);
	voice_unit_release(vs);
	wake_up_interruptible(&vs->wq);
}

static int voice_session_start(struct voice_session *vs,
			       const struct dvf99_voice_config *cfg)
{
	struct voice_params *params;
	int ret;

	ret = voice_config_check(cfg);
	if (ret)
		return ret;

	voice_session_stop(vs);
	vs->cfg = *cfg;

	ret = voice_unit_setup(vs);
	if (ret)
		goto err_unit;

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params) {
		ret = -ENOMEM;
		goto err_unit;
	}

	voice_config_fill(vs, params);
	ret = voice_request(vs->vd, vs, VOICE_START_SESSION, params);
	kfree(params);
	if (ret)
		goto err_unit;

	ret = voice_unit_attach(vs);
	if (ret) {
		voice_simple(vs, VOICE_STOP_SESSION);
		goto err_unit;
	}

	ret = voice_rtcp_apply(vs);
	if (ret) {
		voice_simple(vs, VOICE_STOP_SESSION);
		goto err_unit;
	}

	vs->started = true;
	hrtimer_start(&vs->timer, ns_to_ktime(VOICE_POLL_NS), HRTIMER_MODE_REL);

	if (voice_evt_notify(vs, VOICE_EVT_NOTIFY_START))
		dev_warn(&vs->vd->rpdev->dev,
			 "session %u: no event notification\n", vs->id);

	return 0;

err_unit:
	voice_unit_release(vs);

	return ret;
}

static int voice_session_update(struct voice_session *vs,
				const struct dvf99_voice_config *cfg)
{
	struct voice_params *params;
	int ret;

	if (!vs->started)
		return -EINVAL;

	ret = voice_config_check(cfg);
	if (ret)
		return ret;

	if (cfg->line != vs->cfg.line)
		return voice_session_start(vs, cfg);

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params)
		return -ENOMEM;

	vs->cfg = *cfg;
	voice_config_fill(vs, params);
	ret = voice_request(vs->vd, vs, VOICE_UPDATE_SESSION, params);
	kfree(params);
	if (ret)
		return ret;

	ret = voice_unit_attach(vs);
	if (ret)
		return ret;

	return voice_rtcp_apply(vs);
}

static int voice_session_dtmf(struct voice_session *vs,
			      const struct dvf99_voice_dtmf *dtmf)
{
	struct voice_params params = { };
	unsigned int khz = VOICE_CLOCK_KHZ;

	if (!vs->started)
		return -EINVAL;

	if (dtmf->event > VOICE_DTMF_EVENT_MAX || !dtmf->duration_ms)
		return -EINVAL;

	if (vs->cfg.codec == DVF99_VOICE_CODEC_L16)
		khz = VOICE_CLOCK_WB_KHZ;

	params.line_id = cpu_to_le32(vs->cfg.line);
	params.dtmf.duration = cpu_to_le32(vs->cfg.ptime_ms);
	params.dtmf.evt_duration = cpu_to_le32(vs->cfg.ptime_ms * khz);
	params.dtmf.max_evt_duration = cpu_to_le32(dtmf->duration_ms * khz);
	params.dtmf.event = cpu_to_le32(voice_dtmf_chars[dtmf->event]);
	params.dtmf.volume = cpu_to_le32(min_t(u32, dtmf->volume,
					       VOICE_DTMF_VOLUME_MAX));

	return voice_request(vs->vd, vs, VOICE_SEND_DTMF, &params);
}

static int voice_session_stats(struct voice_session *vs,
			       struct dvf99_voice_stats *stats)
{
	struct voice_dev *vd = vs->vd;
	struct voice_evt_stats *st = &vd->evt_stats;
	int ret;

	if (!vs->started)
		return -EINVAL;

	mutex_lock(&vd->evt_lock);

	if (vd->dead) {
		ret = -ENODEV;
		goto out;
	}

	reinit_completion(&vd->evt_done);
	ret = voice_evt_send(vd, VOICE_EVT_GET_STATS, vs->id, 0);
	if (ret)
		goto out;

	if (!wait_for_completion_timeout(&vd->evt_done, VOICE_TIMEOUT)) {
		ret = -ETIMEDOUT;
		goto out;
	}

	memset(stats, 0, sizeof(*stats));
	stats->pkts_sent = le32_to_cpu(st->pkts_sent);
	stats->pkts_recvd = le32_to_cpu(st->pkts_recvd);
	stats->bytes_sent = le32_to_cpu(st->bytes_sent);
	stats->bytes_recvd = le32_to_cpu(st->bytes_recvd);
	stats->jitter = le32_to_cpu(st->jitter);
	stats->rtt = le32_to_cpu(st->rtt);
	stats->pkts_lost = le32_to_cpu(st->pkts_lost);
	stats->pkts_dropped = le32_to_cpu(st->pkts_dropped);
	stats->decode_latency = le32_to_cpu(st->decode_latency);
	stats->rx_pt = st->rx_pt;

out:
	mutex_unlock(&vd->evt_lock);

	return ret;
}

static void voice_record_push(struct voice_session *vs,
			      const struct voice_record *rec)
{
	unsigned long flags;

	spin_lock_irqsave(&vs->evq_lock, flags);
	if (vs->evq_head - vs->evq_tail < VOICE_EVENTS) {
		vs->evq[vs->evq_head % VOICE_EVENTS] = *rec;
		vs->evq_head++;
	}
	spin_unlock_irqrestore(&vs->evq_lock, flags);
	wake_up_interruptible(&vs->wq);
}

static void voice_dtmf_queue(struct voice_dev *vd, unsigned int line,
			     const struct voice_evt_dtmf *ev)
{
	struct voice_record rec = {
		.hdr.type = DVF99_VOICE_PKT_DTMF,
		.dtmf.timestamp = le32_to_cpu(ev->timestamp),
		.dtmf.duration = le16_to_cpu(ev->duration),
		.dtmf.event = ev->event,
		.dtmf.volume = ev->volume,
		.dtmf.end = ev->end,
	};
	struct voice_session *vs;
	unsigned int i;

	mutex_lock(&vd->lock);
	for (i = 0; i < VOICE_SESSIONS; i++) {
		vs = vd->sessions[i];
		if (vs && READ_ONCE(vs->started) && vs->cfg.line == line)
			voice_record_push(vs, &rec);
	}
	mutex_unlock(&vd->lock);
}

static void voice_session_event_queue(struct voice_dev *vd, unsigned int id,
				      u32 type,
				      const struct voice_evt_session *ev)
{
	struct voice_record rec = {
		.hdr.type = DVF99_VOICE_PKT_EVENT,
		.event.ssrc = be32_to_cpu(ev->ssrc),
		.event.timestamp = be32_to_cpu(ev->timestamp),
		.event.seq = be16_to_cpu(ev->seq),
		.event.pt = ev->pt,
	};
	struct voice_session *vs;

	rec.event.type = type == VOICE_EVT_SSRC ? DVF99_VOICE_EV_SSRC :
						  DVF99_VOICE_EV_FIRST_RX;
	strscpy(rec.event.codec, ev->codec, sizeof(rec.event.codec));

	if (id >= VOICE_SESSIONS)
		return;

	mutex_lock(&vd->lock);
	vs = vd->sessions[id];
	if (vs && READ_ONCE(vs->started))
		voice_record_push(vs, &rec);
	mutex_unlock(&vd->lock);
}

static int voice_evt_cb(struct rpmsg_device *rpdev, void *data, int len,
			void *priv, u32 src)
{
	struct voice_dev *vd = priv;
	struct dvf99_coma_cmsg *cmsg = data;
	struct voice_evt_hdr *hdr;
	unsigned int skip, size;

	if (len < sizeof(*cmsg))
		return -EINVAL;

	skip = sizeof(*cmsg) + le32_to_cpu(cmsg->params_size);
	if (len < skip + sizeof(*hdr))
		return -EINVAL;

	hdr = data + skip;
	size = len - skip - sizeof(*hdr);

	switch (le32_to_cpu(hdr->type)) {
	case VOICE_EVT_RX_DTMF:
		if (size < sizeof(struct voice_evt_dtmf))
			return -EINVAL;
		voice_dtmf_queue(vd, le32_to_cpu(hdr->id), (void *)(hdr + 1));
		break;
	case VOICE_EVT_FIRST_RX:
	case VOICE_EVT_SSRC:
		if (size < sizeof(struct voice_evt_session))
			return -EINVAL;
		voice_session_event_queue(vd, le32_to_cpu(hdr->id),
					  le32_to_cpu(hdr->type),
					  (void *)(hdr + 1));
		break;
	case VOICE_EVT_STATS:
		if (size < sizeof(vd->evt_stats))
			return -EINVAL;
		memcpy(&vd->evt_stats, hdr + 1, sizeof(vd->evt_stats));
		complete(&vd->evt_done);
		break;
	default:
		dev_dbg(&rpdev->dev, "event 0x%x for %u\n",
			le32_to_cpu(hdr->type), le32_to_cpu(hdr->id));
		break;
	}

	return 0;
}

static void voice_session_free(struct voice_session *vs)
{
	struct voice_dev *vd = vs->vd;

	if (vs->enc)
		dvf99_cfifo_destroy(vd->dma_dev, vs->enc);
	if (vs->dec)
		dvf99_cfifo_destroy(vd->dma_dev, vs->dec);

	mutex_lock(&vd->lock);
	vd->sessions[vs->id] = NULL;
	mutex_unlock(&vd->lock);

	mutex_destroy(&vs->wr_lock);
	mutex_destroy(&vs->rd_lock);
	mutex_destroy(&vs->lock);
	kfree(vs);
	kref_put(&vd->ref, voice_dev_release);
}

static int voice_session_init(struct voice_session *vs)
{
	struct voice_dev *vd = vs->vd;
	struct voice_params params = { };
	int ret;

	vs->enc = dvf99_cfifo_create(vd->dma_dev, VOICE_FIFO_SIZE);
	if (IS_ERR(vs->enc)) {
		ret = PTR_ERR(vs->enc);
		vs->enc = NULL;
		return ret;
	}

	vs->dec = dvf99_cfifo_create(vd->dma_dev, VOICE_FIFO_SIZE);
	if (IS_ERR(vs->dec)) {
		ret = PTR_ERR(vs->dec);
		vs->dec = NULL;
		return ret;
	}

	ret = voice_request(vd, vs, VOICE_GET_SESSION, &params);
	if (ret)
		return ret;

	params.fifos.enc = cpu_to_le32(dvf99_cfifo_phys(vs->enc));
	params.fifos.dec = cpu_to_le32(dvf99_cfifo_phys(vs->dec));
	ret = voice_request(vd, vs, VOICE_SET_SESSION_FIFOS, &params);
	if (ret) {
		voice_simple(vs, VOICE_FREE_SESSION);
		return ret;
	}

	return 0;
}

static int voice_open(struct inode *inode, struct file *file)
{
	struct voice_dev *vd = container_of(file->private_data,
					    struct voice_dev, misc);
	struct voice_session *vs;
	unsigned int id;
	int ret;

	vs = kzalloc(sizeof(*vs), GFP_KERNEL);
	if (!vs)
		return -ENOMEM;

	vs->uid = -1;
	vs->conn = -1;
	spin_lock_init(&vs->evq_lock);
	init_waitqueue_head(&vs->wq);
	hrtimer_setup(&vs->timer, voice_timer, CLOCK_MONOTONIC,
		      HRTIMER_MODE_REL);
	mutex_init(&vs->lock);
	mutex_init(&vs->rd_lock);
	mutex_init(&vs->wr_lock);

	mutex_lock(&voice_open_lock);
	mutex_lock(&vd->lock);
	for (id = 0; id < VOICE_SESSIONS; id++)
		if (!vd->sessions[id])
			break;
	if (vd->dead || id >= VOICE_SESSIONS) {
		ret = vd->dead ? -ENODEV : -EBUSY;
		mutex_unlock(&vd->lock);
		mutex_unlock(&voice_open_lock);
		kfree(vs);
		return ret;
	}
	vd->sessions[id] = vs;
	vs->vd = vd;
	vs->id = id;
	kref_get(&vd->ref);
	mutex_unlock(&vd->lock);
	mutex_unlock(&voice_open_lock);

	ret = voice_session_init(vs);
	if (ret) {
		voice_session_free(vs);
		return ret;
	}

	file->private_data = vs;

	return stream_open(inode, file);
}

static int voice_release(struct inode *inode, struct file *file)
{
	struct voice_session *vs = file->private_data;

	mutex_lock(&vs->lock);
	mutex_lock(&vs->vd->setup_lock);
	voice_session_stop(vs);
	mutex_unlock(&vs->vd->setup_lock);
	voice_simple(vs, VOICE_FREE_SESSION);
	mutex_unlock(&vs->lock);

	voice_session_free(vs);

	return 0;
}

static bool voice_event_pending(struct voice_session *vs)
{
	return READ_ONCE(vs->evq_head) != READ_ONCE(vs->evq_tail);
}

static bool voice_readable(struct voice_session *vs)
{
	return !vs->started || READ_ONCE(vs->vd->dead) ||
	       voice_event_pending(vs) || !dvf99_cfifo_empty(vs->enc);
}

static ssize_t voice_event_read(struct voice_session *vs, char __user *buf,
				size_t count)
{
	struct voice_record rec;
	unsigned long flags;
	size_t len;

	spin_lock_irqsave(&vs->evq_lock, flags);
	rec = vs->evq[vs->evq_tail % VOICE_EVENTS];
	vs->evq_tail++;
	spin_unlock_irqrestore(&vs->evq_lock, flags);

	len = sizeof(rec.hdr);
	if (rec.hdr.type == DVF99_VOICE_PKT_DTMF)
		len += sizeof(rec.dtmf);
	else
		len += sizeof(rec.event);
	len = min(count, len);
	if (copy_to_user(buf, &rec, len))
		return -EFAULT;

	return len;
}

static ssize_t voice_read(struct file *file, char __user *buf, size_t count,
			  loff_t *ppos)
{
	struct voice_session *vs = file->private_data;
	void *msg;
	u32 next;
	int len;
	ssize_t ret;

	if (mutex_lock_interruptible(&vs->rd_lock))
		return -ERESTARTSYS;

	for (;;) {
		if (READ_ONCE(vs->vd->dead)) {
			ret = -ENODEV;
			goto out;
		}

		if (!READ_ONCE(vs->started)) {
			ret = -EPIPE;
			goto out;
		}

		if (voice_event_pending(vs)) {
			ret = voice_event_read(vs, buf, count);
			goto out;
		}

		len = dvf99_cfifo_read(vs->enc, &msg, &next);
		if (len < 0) {
			ret = len;
			goto out;
		}
		if (len)
			break;

		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}

		ret = wait_event_interruptible(vs->wq, voice_readable(vs));
		if (ret)
			goto out;
	}

	ret = min_t(size_t, len, count);
	if (copy_to_user(buf, msg, ret))
		ret = -EFAULT;
	dvf99_cfifo_consume(vs->enc, next);

out:
	mutex_unlock(&vs->rd_lock);

	return ret;
}

static ssize_t voice_write(struct file *file, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	struct voice_session *vs = file->private_data;
	ssize_t ret;

	if (count <= sizeof(struct dvf99_voice_pkt) ||
	    count > sizeof(vs->wbuf))
		return -EINVAL;

	if (mutex_lock_interruptible(&vs->wr_lock))
		return -ERESTARTSYS;

	if (READ_ONCE(vs->vd->dead)) {
		ret = -ENODEV;
		goto out;
	}

	if (!READ_ONCE(vs->started)) {
		ret = -EPIPE;
		goto out;
	}

	if (copy_from_user(vs->wbuf, buf, count)) {
		ret = -EFAULT;
		goto out;
	}

	ret = dvf99_cfifo_write(vs->dec, vs->wbuf, count);
	if (!ret)
		ret = count;

out:
	mutex_unlock(&vs->wr_lock);

	return ret;
}

static __poll_t voice_poll(struct file *file, poll_table *wait)
{
	struct voice_session *vs = file->private_data;
	__poll_t mask = 0;

	poll_wait(file, &vs->wq, wait);

	if (READ_ONCE(vs->vd->dead))
		return EPOLLERR | EPOLLHUP;

	if (!READ_ONCE(vs->started))
		return mask;

	mask |= EPOLLOUT | EPOLLWRNORM;
	if (voice_event_pending(vs) || !dvf99_cfifo_empty(vs->enc))
		mask |= EPOLLIN | EPOLLRDNORM;

	return mask;
}

static long voice_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct voice_session *vs = file->private_data;
	void __user *argp = (void __user *)arg;
	struct dvf99_voice_config cfg;
	struct dvf99_voice_dtmf dtmf;
	struct dvf99_voice_stats stats;
	long ret;

	switch (cmd) {
	case DVF99_VOICE_START:
	case DVF99_VOICE_UPDATE:
		if (copy_from_user(&cfg, argp, sizeof(cfg)))
			return -EFAULT;
		break;
	case DVF99_VOICE_DTMF:
		if (copy_from_user(&dtmf, argp, sizeof(dtmf)))
			return -EFAULT;
		break;
	case DVF99_VOICE_STOP:
	case DVF99_VOICE_STATS:
		break;
	default:
		return -ENOTTY;
	}

	if (mutex_lock_interruptible(&vs->lock))
		return -ERESTARTSYS;

	switch (cmd) {
	case DVF99_VOICE_START:
		mutex_lock(&vs->vd->setup_lock);
		ret = voice_session_start(vs, &cfg);
		mutex_unlock(&vs->vd->setup_lock);
		break;
	case DVF99_VOICE_UPDATE:
		mutex_lock(&vs->vd->setup_lock);
		ret = voice_session_update(vs, &cfg);
		mutex_unlock(&vs->vd->setup_lock);
		break;
	case DVF99_VOICE_DTMF:
		ret = voice_session_dtmf(vs, &dtmf);
		break;
	case DVF99_VOICE_STATS:
		ret = voice_session_stats(vs, &stats);
		break;
	default:
		mutex_lock(&vs->vd->setup_lock);
		voice_session_stop(vs);
		mutex_unlock(&vs->vd->setup_lock);
		ret = 0;
		break;
	}

	mutex_unlock(&vs->lock);

	if (!ret && cmd == DVF99_VOICE_STATS &&
	    copy_to_user(argp, &stats, sizeof(stats)))
		ret = -EFAULT;

	return ret;
}

static const struct file_operations voice_fops = {
	.owner = THIS_MODULE,
	.open = voice_open,
	.release = voice_release,
	.read = voice_read,
	.write = voice_write,
	.poll = voice_poll,
	.unlocked_ioctl = voice_ioctl,
	.compat_ioctl = compat_ptr_ioctl,
};

static int voice_probe(struct rpmsg_device *rpdev)
{
	struct rpmsg_channel_info chinfo = {
		.name = "rtp-event",
		.src = RPMSG_ADDR_ANY,
		.dst = RPMSG_ADDR_ANY,
	};
	struct device *dev = &rpdev->dev;
	struct voice_dev *vd;
	int ret;

	BUILD_BUG_ON(sizeof(struct voice_session_cfg) != 800);
	BUILD_BUG_ON(sizeof(struct voice_params) != 812);
	BUILD_BUG_ON(sizeof(struct voice_rtcp_cfg) != 456);

	vd = kzalloc(sizeof(*vd), GFP_KERNEL);
	if (!vd)
		return -ENOMEM;

	kref_init(&vd->ref);
	vd->rpdev = rpdev;
	vd->dma_dev = get_device(dev->parent);
	mutex_init(&vd->lock);
	mutex_init(&vd->setup_lock);
	mutex_init(&vd->req_lock);
	mutex_init(&vd->evt_lock);
	init_completion(&vd->req_done);
	init_completion(&vd->evt_done);

	vd->dua = dvf99_dua_get(dev->parent);
	if (IS_ERR(vd->dua)) {
		ret = PTR_ERR(vd->dua);
		put_device(vd->dma_dev);
		kfree(vd);
		return ret;
	}

	vd->misc.minor = MISC_DYNAMIC_MINOR;
	vd->misc.name = "css-voice";
	vd->misc.fops = &voice_fops;
	vd->misc.parent = dev;

	rpdev->ept->priv = vd;
	dev_set_drvdata(dev, vd);

	vd->evt_ept = rpmsg_create_ept(rpdev, voice_evt_cb, vd, chinfo);
	if (!vd->evt_ept) {
		ret = -ENODEV;
		goto err_put;
	}

	ret = misc_register(&vd->misc);
	if (ret)
		goto err_ept;

	return 0;

err_ept:
	rpmsg_destroy_ept(vd->evt_ept);
err_put:
	kref_put(&vd->ref, voice_dev_release);

	return ret;
}

static void voice_remove(struct rpmsg_device *rpdev)
{
	struct voice_dev *vd = dev_get_drvdata(&rpdev->dev);
	unsigned int i;

	mutex_lock(&voice_open_lock);
	misc_deregister(&vd->misc);
	mutex_unlock(&voice_open_lock);

	mutex_lock(&vd->req_lock);
	mutex_lock(&vd->evt_lock);
	mutex_lock(&vd->lock);
	vd->dead = true;
	for (i = 0; i < VOICE_SESSIONS; i++)
		if (vd->sessions[i])
			wake_up_interruptible(&vd->sessions[i]->wq);
	mutex_unlock(&vd->lock);
	mutex_unlock(&vd->evt_lock);
	mutex_unlock(&vd->req_lock);

	rpmsg_destroy_ept(vd->evt_ept);

	kref_put(&vd->ref, voice_dev_release);
}

static const struct rpmsg_device_id voice_id_table[] = {
	{ .name = "voice" },
	{ }
};
MODULE_DEVICE_TABLE(rpmsg, voice_id_table);

static struct rpmsg_driver voice_driver = {
	.probe = voice_probe,
	.remove = voice_remove,
	.callback = voice_cb,
	.id_table = voice_id_table,
	.drv.name = "dvf99-voice",
};
module_rpmsg_driver(voice_driver);

MODULE_DESCRIPTION("DSPG DVF99 CSS voice service");
MODULE_LICENSE("GPL");
