// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2011 - 2013 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/completion.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/rpmsg/dvf99_dua.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#define CSS_PCM_NUM		7
#define CSS_PCM_PCMFD		1
#define CSS_PCM_BUFFER_MAX	(32 * 1020)
#define CSS_PCM_POS_SIZE	sizeof(__le32)
#define CSS_PCM_PERIOD_MIN	64
#define CSS_PCM_PERIODS_MAX	1024
#define CSS_PCM_TIMEOUT		(3 * HZ)
#define CSS_PCM_ALREADY_OPEN	(-2)

#define CSS_STREAM_CAPTURE	BIT(0)
#define CSS_STREAM_PLAYBACK	BIT(1)

#define CSS_UNIT_PCMFD		"UT_PCMFD"
#define CSS_UNIT_PCMFD_MODE	"UT_PCMFD_MODE_DEFAULT"
#define CSS_UNIT_INST		0
#define CSS_VOLUME_MAX		8

#define CSS_PCM_FORMATS		(SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE | \
				 SNDRV_PCM_FMTBIT_A_LAW | \
				 SNDRV_PCM_FMTBIT_MU_LAW)
#define CSS_PCM_TRACE_RATES	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 | \
				 SNDRV_PCM_RATE_32000)
#define CSS_PCM_ANY_RATES	(CSS_PCM_TRACE_RATES | SNDRV_PCM_RATE_64000)

enum css_alsa_msg {
	CSS_ALSA_OPEN,
	CSS_ALSA_CLOSE,
	CSS_ALSA_START,
	CSS_ALSA_STOP,
	CSS_ALSA_RESET,
	CSS_ALSA_ELAPSED,
	CSS_ALSA_ACK,
	CSS_ALSA_NACK,
};

struct css_alsa_open {
	struct dvf99_coma_cmsg hdr;
	u8 type;
	u8 id;
	__le32 cookie;
	__le32 rate;
	__le32 sample_bits;
	__le32 period_bytes;
	__le32 channels;
	__le32 rx_addr;
	__le32 rx_size;
	__le32 tx_addr;
	__le32 tx_size;
} __packed;

struct css_alsa_cmd {
	struct dvf99_coma_cmsg hdr;
	u8 type;
	u8 id;
	u8 stream;
	__le32 cookie;
} __packed;

struct css_alsa_reply {
	u8 type;
	__le32 cookie;
	__le32 reason;
} __packed;

struct css_alsa_elapsed {
	u8 type;
	u8 id;
	u8 stream;
} __packed;

struct css_pcm {
	struct rpmsg_device *rpdev;
	struct device *dma_dev;
	struct dvf99_dua *dua;
	struct snd_soc_card card;
	struct snd_soc_dai_link links[CSS_PCM_NUM];
	struct snd_soc_dai_link_component dlcs[CSS_PCM_NUM][2];
	struct snd_soc_aux_dev aux;
	struct device_node *codec;
	struct work_struct setup_work;
	bool card_registered;

	struct mutex req_lock;
	struct completion req_done;
	u32 req_cookie;
	s32 req_reason;

	spinlock_t subs_lock;
	struct snd_pcm_substream *subs[CSS_PCM_NUM][2];

	int pcmfd_uid;
	int conn;
	unsigned int wbhf_mode;
	unsigned int volume;
	bool mute;
};

static const char * const css_pcm_names[CSS_PCM_NUM] = {
	"PCMHD", "PCMFD", "TRACE1", "USB", "TRACE2", "ANYFD", "TRACE3",
};

static const char * const css_wbhf_mode_texts[] = {
	"Idle", "Handset", "Group Handset", "Handsfree", "Group Headset",
	"D-Class", "USB", "Group USB",
};


static const struct snd_pcm_hardware css_pcm_hardware = {
	.info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
		SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_BATCH,
	.formats = CSS_PCM_FORMATS,
	.rates = SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_96000,
	.rate_min = 5500,
	.rate_max = 64000,
	.channels_min = 1,
	.channels_max = 3,
	.buffer_bytes_max = CSS_PCM_BUFFER_MAX,
	.period_bytes_min = CSS_PCM_PERIOD_MIN,
	.period_bytes_max = CSS_PCM_BUFFER_MAX / 4,
	.periods_min = 1,
	.periods_max = CSS_PCM_PERIODS_MAX,
};

static u8 css_pcm_stream(struct snd_pcm_substream *substream)
{
	return substream->stream == SNDRV_PCM_STREAM_PLAYBACK ?
	       CSS_STREAM_PLAYBACK : CSS_STREAM_CAPTURE;
}

static unsigned int css_pcm_id(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);

	return snd_soc_rtd_to_cpu(rtd, 0)->id;
}

static struct css_pcm *css_pcm_get(struct snd_soc_component *component)
{
	return snd_soc_card_get_drvdata(component->card);
}

static int css_pcm_send(struct css_pcm *cp, struct dvf99_coma_cmsg *hdr,
			size_t len)
{
	hdr->type = 0;
	hdr->params_size = 0;
	hdr->payload_size = cpu_to_le32(len - sizeof(*hdr));

	return rpmsg_send(cp->rpdev->ept, hdr, len);
}

static int css_pcm_request(struct css_pcm *cp, struct dvf99_coma_cmsg *hdr,
			   size_t len, __le32 *cookie)
{
	int ret;

	mutex_lock(&cp->req_lock);

	*cookie = cpu_to_le32(++cp->req_cookie);
	cp->req_reason = 0;
	reinit_completion(&cp->req_done);

	ret = css_pcm_send(cp, hdr, len);
	if (ret)
		goto out;

	if (!wait_for_completion_timeout(&cp->req_done, CSS_PCM_TIMEOUT))
		ret = -ETIMEDOUT;
	else
		ret = cp->req_reason;
out:
	mutex_unlock(&cp->req_lock);

	return ret;
}

static int css_pcm_cmd(struct css_pcm *cp, enum css_alsa_msg type,
		       struct snd_pcm_substream *substream)
{
	struct css_alsa_cmd msg = {
		.type = type,
		.id = css_pcm_id(substream),
		.stream = css_pcm_stream(substream),
	};

	if (type == CSS_ALSA_STOP)
		return css_pcm_send(cp, &msg.hdr,
				    offsetof(struct css_alsa_cmd, cookie));

	if (type == CSS_ALSA_START)
		return css_pcm_send(cp, &msg.hdr, sizeof(msg));

	return css_pcm_request(cp, &msg.hdr, sizeof(msg), &msg.cookie);
}

static int css_pcm_open(struct snd_soc_component *component,
			struct snd_pcm_substream *substream)
{
	struct css_pcm *cp = css_pcm_get(component);
	unsigned int id = css_pcm_id(substream);
	unsigned long flags;

	if (id != CSS_PCM_PCMFD)
		return -ENODEV;

	snd_soc_set_runtime_hwparams(substream, &css_pcm_hardware);

	spin_lock_irqsave(&cp->subs_lock, flags);
	cp->subs[id][substream->stream] = substream;
	spin_unlock_irqrestore(&cp->subs_lock, flags);

	return 0;
}

static int css_pcm_close(struct snd_soc_component *component,
			 struct snd_pcm_substream *substream)
{
	struct css_pcm *cp = css_pcm_get(component);
	unsigned int id = css_pcm_id(substream);
	unsigned long flags;

	spin_lock_irqsave(&cp->subs_lock, flags);
	cp->subs[id][substream->stream] = NULL;
	spin_unlock_irqrestore(&cp->subs_lock, flags);

	return css_pcm_cmd(cp, CSS_ALSA_CLOSE, substream);
}

static int css_pcm_prepare(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream)
{
	struct css_pcm *cp = css_pcm_get(component);
	struct snd_pcm_runtime *runtime = substream->runtime;
	size_t bytes = snd_pcm_lib_buffer_bytes(substream);
	u32 size = bytes + CSS_PCM_POS_SIZE;
	struct css_alsa_open msg = {
		.type = CSS_ALSA_OPEN,
		.id = css_pcm_id(substream),
		.rate = cpu_to_le32(runtime->rate),
		.sample_bits = cpu_to_le32(runtime->sample_bits),
		.period_bytes = cpu_to_le32(frames_to_bytes(runtime,
							    runtime->period_size)),
		.channels = cpu_to_le32(runtime->channels),
	};
	int ret;

	memset(runtime->dma_area + bytes, 0, CSS_PCM_POS_SIZE);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		msg.tx_addr = cpu_to_le32(runtime->dma_addr);
		msg.tx_size = cpu_to_le32(size);
	} else {
		msg.rx_addr = cpu_to_le32(runtime->dma_addr);
		msg.rx_size = cpu_to_le32(size);
	}

	ret = css_pcm_request(cp, &msg.hdr, sizeof(msg), &msg.cookie);
	if (ret == CSS_PCM_ALREADY_OPEN)
		ret = css_pcm_cmd(cp, CSS_ALSA_RESET, substream);

	return ret;
}

static int css_pcm_trigger(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream, int cmd)
{
	struct css_pcm *cp = css_pcm_get(component);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		return css_pcm_cmd(cp, CSS_ALSA_START, substream);
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		return css_pcm_cmd(cp, CSS_ALSA_STOP, substream);
	}

	return -EINVAL;
}

static snd_pcm_uframes_t css_pcm_pointer(struct snd_soc_component *component,
					 struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	size_t bytes = snd_pcm_lib_buffer_bytes(substream);
	__le32 *pos = (__le32 *)(runtime->dma_area + bytes);
	u32 val = le32_to_cpu(READ_ONCE(*pos));

	if (val >= bytes)
		return 0;

	return bytes_to_frames(runtime, val);
}

static int css_pcm_construct(struct snd_soc_component *component,
			     struct snd_soc_pcm_runtime *rtd)
{
	struct css_pcm *cp = css_pcm_get(component);

	return snd_pcm_set_managed_buffer_all(rtd->pcm, SNDRV_DMA_TYPE_DEV,
					      cp->dma_dev,
					      CSS_PCM_BUFFER_MAX +
					      CSS_PCM_POS_SIZE,
					      CSS_PCM_BUFFER_MAX +
					      CSS_PCM_POS_SIZE);
}

static int css_wbhf_mode_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);

	ucontrol->value.enumerated.item[0] = cp->wbhf_mode;

	return 0;
}

static int css_wbhf_mode_put(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);
	unsigned int item = ucontrol->value.enumerated.item[0];
	int ret;

	if (item >= ARRAY_SIZE(css_wbhf_mode_texts))
		return -EINVAL;

	if (item == cp->wbhf_mode)
		return 0;

	ret = dvf99_dua_audio_mode(cp->dua, item);
	if (ret)
		return ret;

	cp->wbhf_mode = item;

	return 1;
}

static int css_volume_get(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);

	ucontrol->value.integer.value[0] = cp->volume;

	return 0;
}

static int css_volume_put(struct snd_kcontrol *kcontrol,
			  struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);
	long level = ucontrol->value.integer.value[0];
	int ret;

	if (level < 0 || level > CSS_VOLUME_MAX)
		return -EINVAL;

	ret = dvf99_dua_audio_volume(cp->dua, level);
	if (ret)
		return ret;

	if (level == cp->volume)
		return 0;

	cp->volume = level;

	return 1;
}

static int css_mute_get(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);

	ucontrol->value.integer.value[0] = cp->mute;

	return 0;
}

static int css_mute_put(struct snd_kcontrol *kcontrol,
			struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct css_pcm *cp = css_pcm_get(component);
	bool mute = ucontrol->value.integer.value[0];
	int ret;

	if (mute == cp->mute)
		return 0;

	ret = dvf99_dua_audio_mute(cp->dua, mute);
	if (ret)
		return ret;

	cp->mute = mute;

	return 1;
}

static SOC_ENUM_SINGLE_EXT_DECL(css_wbhf_mode_enum, css_wbhf_mode_texts);

static const struct snd_kcontrol_new css_pcm_controls[] = {
	SOC_ENUM_EXT("WBHF Mode", css_wbhf_mode_enum, css_wbhf_mode_get,
		     css_wbhf_mode_put),
	SOC_SINGLE_EXT("Receive Volume", SND_SOC_NOPM, 0, CSS_VOLUME_MAX, 0,
		       css_volume_get, css_volume_put),
	SOC_SINGLE_BOOL_EXT("Mic Mute", 0, css_mute_get, css_mute_put),
};

static const struct snd_soc_component_driver css_pcm_component = {
	.name = "dvf99-css-pcm",
	.controls = css_pcm_controls,
	.num_controls = ARRAY_SIZE(css_pcm_controls),
	.open = css_pcm_open,
	.close = css_pcm_close,
	.prepare = css_pcm_prepare,
	.trigger = css_pcm_trigger,
	.pointer = css_pcm_pointer,
	.pcm_construct = css_pcm_construct,
};

#define CSS_PCM_STREAM(_name, _ch, _rates, _fmts)	\
	{						\
		.stream_name = _name,			\
		.channels_min = 1,			\
		.channels_max = _ch,			\
		.rates = _rates,			\
		.formats = _fmts,			\
	}

#define CSS_FMT_S16	SNDRV_PCM_FMTBIT_S16_LE
#define CSS_FMT_HD	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_MU_LAW | \
			 SNDRV_PCM_FMTBIT_A_LAW)

static struct snd_soc_dai_driver css_pcm_dais[CSS_PCM_NUM] = {
	{
		.name = "css-pcm0",
		.id = 0,
		.playback = CSS_PCM_STREAM("PCMHD Playback", 1,
					   SNDRV_PCM_RATE_8000 |
					   SNDRV_PCM_RATE_16000, CSS_FMT_HD),
	}, {
		.name = "css-pcm1",
		.id = 1,
		.playback = CSS_PCM_STREAM("PCMFD Playback", 1,
					   SNDRV_PCM_RATE_8000_48000,
					   CSS_FMT_S16),
		.capture = CSS_PCM_STREAM("PCMFD Capture", 1,
					  SNDRV_PCM_RATE_16000 |
					  SNDRV_PCM_RATE_32000, CSS_FMT_S16),
	}, {
		.name = "css-pcm2",
		.id = 2,
		.capture = CSS_PCM_STREAM("TRACE1 Capture", 3,
					  CSS_PCM_TRACE_RATES, CSS_FMT_S16),
	}, {
		.name = "css-pcm3",
		.id = 3,
		.playback = CSS_PCM_STREAM("USB Playback", 2,
					   SNDRV_PCM_RATE_16000, CSS_FMT_S16),
		.capture = CSS_PCM_STREAM("USB Capture", 2,
					  SNDRV_PCM_RATE_16000, CSS_FMT_S16),
	}, {
		.name = "css-pcm4",
		.id = 4,
		.capture = CSS_PCM_STREAM("TRACE2 Capture", 3,
					  CSS_PCM_TRACE_RATES, CSS_FMT_S16),
	}, {
		.name = "css-pcm5",
		.id = 5,
		.playback = CSS_PCM_STREAM("ANYFD Playback", 1,
					   CSS_PCM_ANY_RATES, CSS_FMT_S16),
		.capture = CSS_PCM_STREAM("ANYFD Capture", 1,
					  CSS_PCM_ANY_RATES, CSS_FMT_S16),
	}, {
		.name = "css-pcm6",
		.id = 6,
		.capture = CSS_PCM_STREAM("TRACE3 Capture", 3,
					  CSS_PCM_TRACE_RATES, CSS_FMT_S16),
	},
};

static void css_pcm_reply(struct css_pcm *cp, const u8 *data, size_t len,
			  bool nack)
{
	const struct css_alsa_reply *reply = (const void *)data;

	if (len < offsetof(struct css_alsa_reply, reason) ||
	    (nack && len < sizeof(*reply)))
		return;

	if (le32_to_cpu(reply->cookie) != cp->req_cookie)
		return;

	cp->req_reason = nack ? (s32)le32_to_cpu(reply->reason) : 0;
	complete(&cp->req_done);
}

static void css_pcm_elapsed(struct css_pcm *cp, const u8 *data, size_t len)
{
	const struct css_alsa_elapsed *msg = (const void *)data;
	struct snd_pcm_substream *subs[2];
	unsigned long flags;
	unsigned int i;

	if (len < sizeof(*msg) || msg->id >= CSS_PCM_NUM)
		return;

	spin_lock_irqsave(&cp->subs_lock, flags);
	subs[SNDRV_PCM_STREAM_PLAYBACK] = msg->stream & CSS_STREAM_PLAYBACK ?
		cp->subs[msg->id][SNDRV_PCM_STREAM_PLAYBACK] : NULL;
	subs[SNDRV_PCM_STREAM_CAPTURE] = msg->stream & CSS_STREAM_CAPTURE ?
		cp->subs[msg->id][SNDRV_PCM_STREAM_CAPTURE] : NULL;
	spin_unlock_irqrestore(&cp->subs_lock, flags);

	for (i = 0; i < ARRAY_SIZE(subs); i++)
		if (subs[i])
			snd_pcm_period_elapsed(subs[i]);
}

static int css_pcm_cb(struct rpmsg_device *rpdev, void *data, int len,
		      void *priv, u32 src)
{
	struct css_pcm *cp = priv;
	struct dvf99_coma_cmsg *cmsg = data;
	u32 skip = sizeof(*cmsg);
	const u8 *payload;

	if (len < skip)
		return -EINVAL;

	skip += le32_to_cpu(cmsg->params_size);
	if (skip >= len)
		return -EINVAL;

	payload = (const u8 *)data + skip;
	len -= skip;

	switch (payload[0]) {
	case CSS_ALSA_ACK:
		css_pcm_reply(cp, payload, len, false);
		break;
	case CSS_ALSA_NACK:
		css_pcm_reply(cp, payload, len, true);
		break;
	case CSS_ALSA_ELAPSED:
		css_pcm_elapsed(cp, payload, len);
		break;
	default:
		dev_err_ratelimited(&rpdev->dev, "unknown message %u\n",
				    payload[0]);
		return -EINVAL;
	}

	return 0;
}

static int css_pcm_units_setup(struct css_pcm *cp)
{
	int ret;

	ret = dvf99_dua_unit_alloc(cp->dua, CSS_UNIT_PCMFD, CSS_UNIT_INST);
	if (ret < 0)
		return ret;
	cp->pcmfd_uid = ret;

	ret = dvf99_dua_audio_join(cp->dua, cp->pcmfd_uid);
	if (ret < 0)
		return ret;
	cp->conn = ret;

	ret = dvf99_dua_unit_mode(cp->dua, CSS_UNIT_PCMFD, CSS_UNIT_INST,
				  CSS_UNIT_PCMFD_MODE);
	if (ret)
		return ret;

	ret = dvf99_dua_audio_mode(cp->dua, DVF99_DUA_AUDIO_HANDSET);
	if (ret)
		return ret;
	cp->wbhf_mode = DVF99_DUA_AUDIO_HANDSET;

	return 0;
}

static void css_pcm_units_release(struct css_pcm *cp)
{
	if (cp->conn >= 0)
		dvf99_dua_unit_disconnect(cp->dua, cp->pcmfd_uid, cp->conn);
	if (cp->pcmfd_uid >= 0)
		dvf99_dua_unit_free(cp->dua, cp->pcmfd_uid);
	cp->conn = -1;
	cp->pcmfd_uid = -1;
}

static void css_pcm_setup_work(struct work_struct *work)
{
	struct css_pcm *cp = container_of(work, struct css_pcm, setup_work);
	struct device *dev = &cp->rpdev->dev;
	int ret;

	ret = css_pcm_units_setup(cp);
	if (ret) {
		dev_err(dev, "cannot set up the DSP units: %d\n", ret);
		css_pcm_units_release(cp);
		return;
	}

	ret = snd_soc_register_card(&cp->card);
	if (ret) {
		dev_err(dev, "cannot register the card: %d\n", ret);
		css_pcm_units_release(cp);
		return;
	}

	cp->card_registered = true;
}

static int css_pcm_probe(struct rpmsg_device *rpdev)
{
	struct device *dev = &rpdev->dev;
	struct css_pcm *cp;
	unsigned int i;
	int ret;

	cp = devm_kzalloc(dev, sizeof(*cp), GFP_KERNEL);
	if (!cp)
		return -ENOMEM;

	cp->rpdev = rpdev;
	cp->dma_dev = dev->parent;
	cp->pcmfd_uid = -1;
	cp->conn = -1;
	cp->volume = CSS_VOLUME_MAX;
	init_completion(&cp->req_done);
	spin_lock_init(&cp->subs_lock);
	INIT_WORK(&cp->setup_work, css_pcm_setup_work);
	rpdev->ept->priv = cp;

	ret = devm_mutex_init(dev, &cp->req_lock);
	if (ret)
		return ret;

	cp->dua = dvf99_dua_get(dev->parent);
	if (IS_ERR(cp->dua))
		return PTR_ERR(cp->dua);

	ret = devm_snd_soc_register_component(dev, &css_pcm_component,
					      css_pcm_dais,
					      ARRAY_SIZE(css_pcm_dais));
	if (ret)
		goto err_put;

	for (i = 0; i < CSS_PCM_NUM; i++) {
		struct snd_soc_dai_link *link = &cp->links[i];
		struct snd_soc_dai_link_component *cpu = &cp->dlcs[i][0];
		struct snd_soc_dai_link_component *platform = &cp->dlcs[i][1];

		cpu->dai_name = css_pcm_dais[i].name;
		platform->name = dev_name(dev);

		link->name = css_pcm_names[i];
		link->stream_name = css_pcm_names[i];
		link->cpus = cpu;
		link->num_cpus = 1;
		link->codecs = &snd_soc_dummy_dlc;
		link->num_codecs = 1;
		link->platforms = platform;
		link->num_platforms = 1;
		link->nonatomic = 1;
	}

	cp->card.name = "dvf99-css";
	cp->card.owner = THIS_MODULE;
	cp->card.dev = dev;
	cp->card.dai_link = cp->links;
	cp->card.num_links = CSS_PCM_NUM;
	snd_soc_card_set_drvdata(&cp->card, cp);

	cp->codec = of_parse_phandle(dev->of_node, "dspg,codec", 0);
	if (cp->codec) {
		cp->aux.dlc.of_node = cp->codec;
		cp->card.aux_dev = &cp->aux;
		cp->card.num_aux_devs = 1;
	}

	schedule_work(&cp->setup_work);

	return 0;

err_put:
	dvf99_dua_put(cp->dua);

	return ret;
}

static void css_pcm_remove(struct rpmsg_device *rpdev)
{
	struct css_pcm *cp = rpdev->ept->priv;

	cancel_work_sync(&cp->setup_work);
	if (cp->card_registered)
		snd_soc_unregister_card(&cp->card);
	css_pcm_units_release(cp);
	of_node_put(cp->codec);
	dvf99_dua_put(cp->dua);
}

static const struct rpmsg_device_id css_pcm_id_table[] = {
	{ .name = "alsa" },
	{ }
};
MODULE_DEVICE_TABLE(rpmsg, css_pcm_id_table);

static struct rpmsg_driver css_pcm_driver = {
	.probe = css_pcm_probe,
	.remove = css_pcm_remove,
	.callback = css_pcm_cb,
	.id_table = css_pcm_id_table,
	.drv.name = "dvf99-css-pcm",
};
module_rpmsg_driver(css_pcm_driver);

MODULE_DESCRIPTION("DSPG DVF99 CSS PCM driver");
MODULE_LICENSE("GPL");
