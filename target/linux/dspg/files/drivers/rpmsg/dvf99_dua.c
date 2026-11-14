// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/completion.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/firmware.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/rpmsg/dvf99_dua.h>
#include <linux/sizes.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/unaligned.h>

#define DUA_SERVICE_ID		2
#define DUA_TIMEOUT		msecs_to_jiffies(2000)
#define DUA_REQ_MAX		256
#define DUA_INIT_MODE		5
#define DUA_INIT_REF		0xdeadbeef
#define DUA_BUF_SIZE		SZ_512K
#define DUA_BUF_MAP_SIZE	(DUA_BUF_SIZE - SZ_4K)
#define DUA_BUF_HDR_SIZE	0x34
#define DUA_ANY_ELEM		"DUA_ANY"
#define DUA_PARAM_MODE		"DUA_PARAM_UMT_EXEC_GEN"
#define DUA_AUDIO_UNIT		"UT_WBHF"
#define DUA_AUDIO_INST		0
#define DUA_AUDIO_LEVELS	9
#define DUA_ELEM_V2_C		112
#define DUA_ELEM_V2_D		113
#define DUA_ELEM_OPI_A		80
#define DUA_PARAM_V2_VOL	0xa004
#define DUA_PARAM_ISWITCH	0x2000
#define DUA_PARAM_PIN_MUTE	0x10006
#define DUA_PARAM_UMT_LOAD_DYN	0x10104
#define DUA_UMT_UID		0
#define DUA_UMT_DYN_SLOTS	28
#define DUA_UMT_FW		"dvf99-css/wbhf-dyn-%u.bin"
#define DUA_ISWITCH_ON		20
#define DUA_MUTE_ON		3
#define DUA_MUTE_OFF		1
#define DUA_RAW_LOG		4096
#define DUA_RAW_WAIT_MS		500
#define DUA_RAW_EVENT		0xffff

enum dua_msg {
	DUA_INIT_REQ = 1,
	DUA_UNIT_CONNECT_REQ = 4,
	DUA_UNIT_DISCONNECT_REQ = 5,
	DUA_UNIT_FREE_REQ = 12,
	DUA_UNIT_SET_REQ = 13,
	DUASC_UNITALLOCATEREQ = 86,
	DUASC_UNITSETREQ = 88,
	DUA_CALLBACK = 127,
	DUA_FUNCTION_RETURN = 129,
	DUA_TEXT_TRACE = 130,
};

enum dua_event {
	DUAEV_INIT_IND = 2,
	DUAEV_UNITCONNECT_IND = 5,
	DUAEV_UNITDISCONNECT_IND = 6,
	DUAEV_UNITALLOCATE_IND = 9,
	DUAEV_UNITFREE_IND = 10,
	DUAEV_SET_IND = 11,
};

enum sharedmem_msg {
	SHAREDMEM_INIT,
	SHAREDMEM_INIT_REPLY,
};

struct dua_hdr {
	__le32 caller_id;
	__le32 txn_id;
	__le32 msg_id;
	__le32 nr_param;
	__le32 params[];
} __packed;

struct sharedmem_init {
	struct dvf99_coma_cmsg hdr;
	__le32 virt_base;
	__le32 phys_base;
	__le32 size;
} __packed;

struct dua_req {
	unsigned int len;
	unsigned int size;
	u8 buf[] __counted_by(size);
};

struct dvf99_dua {
	struct kref ref;
	struct rpmsg_device *rpdev;
	struct rpmsg_endpoint *shm_ept;
	struct list_head node;
	void *buf;
	dma_addr_t buf_dma;

	struct mutex lock;
	struct completion ret_done;
	struct completion cb_done;
	struct completion shm_done;
	u32 txn;
	s32 ret_val;
	s32 cb_ind;
	s32 cb_uid;
	s32 cb_elem;
	s32 shm_status;
	unsigned int want_event;
	bool dead;

	int audio_uid;
	int audio_conn;
	struct mutex audio_lock;
	enum dvf99_dua_audio_mode audio_mode;
	int audio_volume;
	bool audio_mute;

	struct dentry *debugfs;
	spinlock_t raw_lock;
	char *raw_log;
	size_t raw_len;
};

static LIST_HEAD(dua_list);
static DEFINE_MUTEX(dua_list_lock);

static void dua_req_init(struct dua_req *req, enum dua_msg msg_id,
			 unsigned int nr, const s32 *params)
{
	struct dvf99_coma_cmsg *cmsg = (void *)req->buf;
	struct dua_hdr *hdr = (void *)cmsg->body;
	unsigned int i;

	cmsg->type = cpu_to_le32(DUA_SERVICE_ID);
	cmsg->params_size = 0;
	hdr->caller_id = 0;
	hdr->msg_id = cpu_to_le32(msg_id);
	hdr->nr_param = cpu_to_le32(nr);
	for (i = 0; i < nr; i++)
		hdr->params[i] = cpu_to_le32(params[i]);

	req->len = sizeof(*cmsg) + struct_size(hdr, params, nr);
}

static void dua_req_str(struct dua_req *req, const char *s)
{
	unsigned int n = strlen(s) + 1;

	memcpy(req->buf + req->len, s, n);
	req->len += n;
}

static void dua_req_data(struct dua_req *req, const void *data,
			 unsigned int size)
{
	req->len = ALIGN(req->len, 4);
	put_unaligned_le32(size, req->buf + req->len);
	req->len += sizeof(__le32);
	if (size)
		memcpy(req->buf + req->len, data, size);
	req->len = ALIGN(req->len + size, 4);
}

static int dua_request(struct dvf99_dua *dua, struct dua_req *req,
		       enum dua_event event, s32 *uid, s32 *elem)
{
	struct dvf99_coma_cmsg *cmsg = (void *)req->buf;
	struct dua_hdr *hdr = (void *)cmsg->body;
	int ret;

	cmsg->payload_size = cpu_to_le32(req->len - sizeof(*cmsg));
	hdr->txn_id = cpu_to_le32(++dua->txn);

	reinit_completion(&dua->ret_done);
	reinit_completion(&dua->cb_done);
	dua->want_event = event;

	ret = rpmsg_send(dua->rpdev->ept, req->buf, req->len);
	if (ret)
		return ret;

	if (!wait_for_completion_timeout(&dua->ret_done, DUA_TIMEOUT) ||
	    !wait_for_completion_timeout(&dua->cb_done, DUA_TIMEOUT)) {
		dev_err(&dua->rpdev->dev, "request %u timed out\n",
			le32_to_cpu(hdr->msg_id));
		return -ETIMEDOUT;
	}

	if (dua->ret_val < 0 || dua->cb_ind < 0) {
		dev_err(&dua->rpdev->dev, "request %u failed: %d, event %d\n",
			le32_to_cpu(hdr->msg_id), dua->ret_val,
			dua->cb_ind >> 8);
		return -EIO;
	}

	if (uid)
		*uid = dua->cb_uid;
	if (elem)
		*elem = dua->cb_elem;

	return dua->ret_val;
}

static int dua_run(struct dvf99_dua *dua, struct dua_req *req,
		   enum dua_event event, s32 *uid, s32 *elem)
{
	int ret;

	mutex_lock(&dua->lock);
	if (dua->dead)
		ret = -ENODEV;
	else
		ret = dua_request(dua, req, event, uid, elem);
	mutex_unlock(&dua->lock);

	return ret;
}

static int dua_simple(struct dvf99_dua *dua, enum dua_msg msg_id,
		      unsigned int nr, const s32 *params, enum dua_event event,
		      s32 *elem)
{
	DEFINE_FLEX(struct dua_req, req, buf, size, DUA_REQ_MAX);

	dua_req_init(req, msg_id, nr, params);
	dua_req_data(req, NULL, 0);

	return dua_run(dua, req, event, NULL, elem);
}

/**
 * dvf99_dua_unit_alloc() - allocate a DSP unit in the CSS firmware
 * @dua: DUA instance from dvf99_dua_get()
 * @type: unit type name, for example "UT_PCMFD"
 * @inst: instance number of that type
 *
 * Return: the unit id or a negative error code.
 */
int dvf99_dua_unit_alloc(struct dvf99_dua *dua, const char *type,
			 unsigned int inst)
{
	DEFINE_FLEX(struct dua_req, req, buf, size, DUA_REQ_MAX);
	s32 params[1] = { inst };
	s32 uid;
	int ret;

	dua_req_init(req, DUASC_UNITALLOCATEREQ, 1, params);
	dua_req_str(req, type);

	ret = dua_run(dua, req, DUAEV_UNITALLOCATE_IND, &uid, NULL);
	if (ret < 0)
		return ret;

	return uid;
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_alloc);

/**
 * dvf99_dua_unit_free() - free a DSP unit
 * @dua: DUA instance from dvf99_dua_get()
 * @uid: unit id from dvf99_dua_unit_alloc()
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_unit_free(struct dvf99_dua *dua, int uid)
{
	s32 params[1] = { uid };

	return dua_simple(dua, DUA_UNIT_FREE_REQ, 1, params,
			  DUAEV_UNITFREE_IND, NULL);
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_free);

/**
 * dvf99_dua_unit_mode() - set the generic mode of a DSP unit
 * @dua: DUA instance from dvf99_dua_get()
 * @type: unit type name
 * @inst: instance number of that type
 * @mode: mode name, for example "UT_WBHF_MODE_HANDSET"
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_unit_mode(struct dvf99_dua *dua, const char *type,
			unsigned int inst, const char *mode)
{
	DEFINE_FLEX(struct dua_req, req, buf, size, DUA_REQ_MAX);
	s32 params[3] = { inst, 0, 0 };

	dua_req_init(req, DUASC_UNITSETREQ, 3, params);
	dua_req_str(req, type);
	dua_req_str(req, DUA_ANY_ELEM);
	dua_req_str(req, DUA_PARAM_MODE);
	dua_req_data(req, mode, strlen(mode) + 1);

	return dua_run(dua, req, DUAEV_SET_IND, NULL, NULL);
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_mode);

/**
 * dvf99_dua_unit_connect() - connect a DSP unit to a connection
 * @dua: DUA instance from dvf99_dua_get()
 * @uid: unit id from dvf99_dua_unit_alloc()
 * @conn: connection id, or DVF99_DUA_NEW_CONN to create one
 *
 * Return: the connection id or a negative error code.
 */
int dvf99_dua_unit_connect(struct dvf99_dua *dua, int uid, int conn)
{
	s32 params[2] = { uid, conn };
	s32 elem;
	int ret;

	ret = dua_simple(dua, DUA_UNIT_CONNECT_REQ, 2, params,
			 DUAEV_UNITCONNECT_IND, &elem);
	if (ret < 0)
		return ret;

	return conn < 0 ? elem : conn;
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_connect);

/**
 * dvf99_dua_unit_disconnect() - remove a DSP unit from a connection
 * @dua: DUA instance from dvf99_dua_get()
 * @uid: unit id from dvf99_dua_unit_alloc()
 * @conn: connection id from dvf99_dua_unit_connect()
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_unit_disconnect(struct dvf99_dua *dua, int uid, int conn)
{
	s32 params[2] = { uid, conn };

	return dua_simple(dua, DUA_UNIT_DISCONNECT_REQ, 2, params,
			  DUAEV_UNITDISCONNECT_IND, NULL);
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_disconnect);

/**
 * dvf99_dua_audio_conn() - get the connection of the audio front end
 * @dua: DUA instance from dvf99_dua_get()
 *
 * The DUA driver allocates the wideband handsfree unit and creates its
 * connection at probe. Audio sources and sinks join this connection.
 *
 * Return: the connection id.
 */
int dvf99_dua_audio_conn(struct dvf99_dua *dua)
{
	return dua->audio_conn;
}
EXPORT_SYMBOL_GPL(dvf99_dua_audio_conn);

static const char * const dua_audio_modes[] = {
	[DVF99_DUA_AUDIO_IDLE] = "UT_WBHF_MODE_IDLE",
	[DVF99_DUA_AUDIO_HANDSET] = "UT_WBHF_MODE_HANDSET",
	[DVF99_DUA_AUDIO_GROUP_HAND] = "UT_WBHF_MODE_GROUP_HAND",
	[DVF99_DUA_AUDIO_HEADSET] = "UT_WBHF_MODE_HANDSFREE",
	[DVF99_DUA_AUDIO_GROUP_HEAD] = "UT_WBHF_MODE_GROUP_HEAD",
	[DVF99_DUA_AUDIO_SPEAKER] = "UT_WBHF_MODE_D_CLASS",
	[DVF99_DUA_AUDIO_USB] = "UT_WBHF_MODE_USB",
	[DVF99_DUA_AUDIO_GROUP_USB] = "UT_WBHF_MODE_GROUP_USB",
};

static const u16 dua_volume_handset[DUA_AUDIO_LEVELS] = {
	0, 64, 128, 256, 384, 512, 864, 1312, 2304,
};

static const u16 dua_volume_default[DUA_AUDIO_LEVELS] = {
	0, 64, 128, 256, 384, 512, 864, 1312, 2048,
};

/**
 * dvf99_dua_unit_set() - set one numeric parameter of a DSP unit element
 * @dua: DUA instance from dvf99_dua_get()
 * @uid: unit id
 * @elem: element number
 * @param: parameter code
 * @value: value
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_unit_set(struct dvf99_dua *dua, int uid, int elem, u32 param,
		       s32 value)
{
	s32 params[4] = { uid, elem, param, value };

	return dua_simple(dua, DUA_UNIT_SET_REQ, 4, params, DUAEV_SET_IND,
			  NULL);
}
EXPORT_SYMBOL_GPL(dvf99_dua_unit_set);

static int dua_audio_volume_apply(struct dvf99_dua *dua)
{
	const u16 *table = dua_volume_default;
	int ret;

	if (dua->audio_volume < 0)
		return 0;

	if (dua->audio_mode == DVF99_DUA_AUDIO_HANDSET ||
	    dua->audio_mode == DVF99_DUA_AUDIO_GROUP_HAND)
		table = dua_volume_handset;

	ret = dvf99_dua_unit_set(dua, dua->audio_uid, DUA_ELEM_V2_C,
				 DUA_PARAM_V2_VOL, table[dua->audio_volume]);
	if (ret)
		return ret;

	return dvf99_dua_unit_set(dua, dua->audio_uid, DUA_ELEM_V2_D,
				  DUA_PARAM_V2_VOL, table[dua->audio_volume]);
}

static int dua_audio_mute_apply(struct dvf99_dua *dua)
{
	return dvf99_dua_unit_set(dua, dua->audio_uid, DUA_ELEM_OPI_A,
				  DUA_PARAM_PIN_MUTE,
				  dua->audio_mute ? DUA_MUTE_ON : DUA_MUTE_OFF);
}

static int dua_audio_apply(struct dvf99_dua *dua)
{
	int ret;

	ret = dua_audio_volume_apply(dua);
	if (ret)
		return ret;

	return dua_audio_mute_apply(dua);
}

/**
 * dvf99_dua_audio_mode() - set the mode of the audio front end
 * @dua: DUA instance from dvf99_dua_get()
 * @mode: audio path mode
 *
 * The receive volume and the microphone mute are applied again after the
 * mode change.
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_audio_mode(struct dvf99_dua *dua, enum dvf99_dua_audio_mode mode)
{
	int ret;

	if (mode >= ARRAY_SIZE(dua_audio_modes))
		return -EINVAL;

	mutex_lock(&dua->audio_lock);

	ret = dvf99_dua_unit_mode(dua, DUA_AUDIO_UNIT, DUA_AUDIO_INST,
				  dua_audio_modes[mode]);
	if (ret)
		goto out;
	dua->audio_mode = mode;

	if (mode == DVF99_DUA_AUDIO_SPEAKER)
		ret = dvf99_dua_unit_set(dua, dua->audio_uid, DUA_ELEM_V2_C,
					 DUA_PARAM_ISWITCH, DUA_ISWITCH_ON);
	else if (mode == DVF99_DUA_AUDIO_GROUP_HAND ||
		 mode == DVF99_DUA_AUDIO_GROUP_HEAD)
		ret = dvf99_dua_unit_set(dua, dua->audio_uid, DUA_ELEM_V2_D,
					 DUA_PARAM_ISWITCH, DUA_ISWITCH_ON);
	if (ret)
		goto out;

	ret = dua_audio_apply(dua);

out:
	mutex_unlock(&dua->audio_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dvf99_dua_audio_mode);

/**
 * dvf99_dua_audio_volume() - set the receive volume of the audio front end
 * @dua: DUA instance from dvf99_dua_get()
 * @level: volume level, 0 to 8
 *
 * The level maps to the gain table of the current mode.
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_audio_volume(struct dvf99_dua *dua, unsigned int level)
{
	int old, ret;

	if (level >= DUA_AUDIO_LEVELS)
		return -EINVAL;

	mutex_lock(&dua->audio_lock);
	old = dua->audio_volume;
	dua->audio_volume = level;
	ret = dua_audio_volume_apply(dua);
	if (ret)
		dua->audio_volume = old;
	mutex_unlock(&dua->audio_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dvf99_dua_audio_volume);

/**
 * dvf99_dua_audio_mute() - mute or unmute the microphone
 * @dua: DUA instance from dvf99_dua_get()
 * @mute: true to mute
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_dua_audio_mute(struct dvf99_dua *dua, bool mute)
{
	int ret;

	mutex_lock(&dua->audio_lock);
	dua->audio_mute = mute;
	ret = dua_audio_mute_apply(dua);
	if (ret)
		dua->audio_mute = !mute;
	mutex_unlock(&dua->audio_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dvf99_dua_audio_mute);

/**
 * dvf99_dua_audio_join() - connect a unit to the audio front end
 * @dua: DUA instance from dvf99_dua_get()
 * @uid: unit id from dvf99_dua_unit_alloc()
 *
 * The receive volume and the microphone mute are applied again after the
 * connect.
 *
 * Return: the connection id or a negative error code.
 */
int dvf99_dua_audio_join(struct dvf99_dua *dua, int uid)
{
	int conn, ret;

	mutex_lock(&dua->audio_lock);
	conn = dvf99_dua_unit_connect(dua, uid, dua->audio_conn);
	if (conn >= 0) {
		ret = dua_audio_apply(dua);
		if (ret) {
			dvf99_dua_unit_disconnect(dua, uid, conn);
			conn = ret;
		}
	}
	mutex_unlock(&dua->audio_lock);

	return conn;
}
EXPORT_SYMBOL_GPL(dvf99_dua_audio_join);

static int dua_audio_setup(struct dvf99_dua *dua)
{
	int ret;

	ret = dvf99_dua_unit_alloc(dua, DUA_AUDIO_UNIT, DUA_AUDIO_INST);
	if (ret < 0)
		return ret;
	dua->audio_uid = ret;

	ret = dvf99_dua_unit_connect(dua, dua->audio_uid, DVF99_DUA_NEW_CONN);
	if (ret < 0)
		return ret;
	dua->audio_conn = ret;

	return 0;
}

static int dua_umt_dyn_load(struct dvf99_dua *dua, unsigned int slot,
			    const struct firmware *fw)
{
	s32 params[3] = { DUA_UMT_UID, slot, DUA_PARAM_UMT_LOAD_DYN };
	struct dua_req *req __free(kfree) = NULL;
	unsigned int size = DUA_REQ_MAX + fw->size;

	req = kzalloc(struct_size(req, buf, size), GFP_KERNEL);
	if (!req)
		return -ENOMEM;
	req->size = size;

	dua_req_init(req, DUA_UNIT_SET_REQ, 3, params);
	dua_req_data(req, fw->data, fw->size);

	return dua_run(dua, req, DUAEV_SET_IND, NULL, NULL);
}

static void dua_umt_dyn_setup(struct dvf99_dua *dua)
{
	struct device *dev = &dua->rpdev->dev;
	const struct firmware *fw;
	unsigned int slot, loaded = 0;
	char name[64];
	int ret;

	for (slot = 1; slot <= DUA_UMT_DYN_SLOTS; slot++) {
		snprintf(name, sizeof(name), DUA_UMT_FW, slot);
		if (request_firmware_direct(&fw, name, dev))
			continue;

		ret = dua_umt_dyn_load(dua, slot, fw);
		release_firmware(fw);
		if (ret) {
			dev_warn(dev, "cannot load %s: %d\n", name, ret);
			continue;
		}
		loaded++;
	}

	if (loaded)
		dev_info(dev, "loaded %u dynamic mode tables\n", loaded);
}

static void dua_audio_release(struct dvf99_dua *dua)
{
	if (dua->audio_conn >= 0)
		dvf99_dua_unit_disconnect(dua, dua->audio_uid, dua->audio_conn);
	if (dua->audio_uid >= 0)
		dvf99_dua_unit_free(dua, dua->audio_uid);
	dua->audio_conn = -1;
	dua->audio_uid = -1;
}

static void dua_raw_printf(struct dvf99_dua *dua, const char *fmt, ...)
{
	unsigned long flags;
	va_list args;

	spin_lock_irqsave(&dua->raw_lock, flags);
	if (dua->raw_len < DUA_RAW_LOG) {
		va_start(args, fmt);
		dua->raw_len += vscnprintf(dua->raw_log + dua->raw_len,
					   DUA_RAW_LOG - dua->raw_len, fmt, args);
		va_end(args);
	}
	spin_unlock_irqrestore(&dua->raw_lock, flags);
}

static void dua_raw_log_cb(struct dvf99_dua *dua, const struct dua_hdr *hdr,
			   unsigned int nr)
{
	unsigned int i;

	dua_raw_printf(dua, "callback");
	for (i = 0; i < nr; i++)
		dua_raw_printf(dua, " %d", (s32)le32_to_cpu(hdr->params[i]));
	dua_raw_printf(dua, "\n");
}

static int dua_raw_parse(struct dua_req *req, char *line)
{
	s32 params[16];
	unsigned int nr = 0;
	char *tok, *strs[4];
	unsigned int nstr = 0;
	u8 *data = NULL;
	int dlen = -1, msg_id, ret = 0;

	tok = strsep(&line, " \t\n");
	if (!tok || kstrtoint(tok, 0, &msg_id))
		return -EINVAL;

	while ((tok = strsep(&line, " \t\n"))) {
		if (!*tok)
			continue;
		if (!strcmp(tok, "s")) {
			tok = strsep(&line, " \t\n");
			if (!tok || nstr >= ARRAY_SIZE(strs))
				return -EINVAL;
			strs[nstr++] = tok;
		} else if (!strcmp(tok, "d")) {
			tok = strsep(&line, " \t\n");
			if (!tok)
				tok = "";
			dlen = strlen(tok) / 2;
			data = kmalloc(dlen + 1, GFP_KERNEL);
			if (!data)
				return -ENOMEM;
			if (hex2bin(data, tok, dlen)) {
				ret = -EINVAL;
				goto out;
			}
		} else {
			if (nr >= ARRAY_SIZE(params) ||
			    kstrtos32(tok, 0, &params[nr])) {
				ret = -EINVAL;
				goto out;
			}
			nr++;
		}
	}

	dua_req_init(req, msg_id, nr, params);
	for (nr = 0; nr < nstr; nr++)
		dua_req_str(req, strs[nr]);
	if (dlen >= 0) {
		if (req->len + dlen + 12 > req->size) {
			ret = -E2BIG;
			goto out;
		}
		dua_req_data(req, data, dlen);
	}

out:
	kfree(data);

	return ret;
}

static ssize_t dua_raw_write(struct file *file, const char __user *ubuf,
			     size_t count, loff_t *ppos)
{
	struct dvf99_dua *dua = file->private_data;
	struct dvf99_coma_cmsg *cmsg;
	struct dua_hdr *hdr;
	struct dua_req *req;
	char *line;
	int ret;

	if (count > 2 * DUA_REQ_MAX)
		return -E2BIG;

	line = memdup_user_nul(ubuf, count);
	if (IS_ERR(line))
		return PTR_ERR(line);

	req = kzalloc(struct_size(req, buf, DUA_REQ_MAX), GFP_KERNEL);
	if (!req) {
		ret = -ENOMEM;
		goto out_line;
	}
	req->size = DUA_REQ_MAX;

	ret = dua_raw_parse(req, line);
	if (ret)
		goto out_req;

	cmsg = (void *)req->buf;
	hdr = (void *)cmsg->body;
	cmsg->payload_size = cpu_to_le32(req->len - sizeof(*cmsg));

	mutex_lock(&dua->lock);
	if (dua->dead) {
		ret = -ENODEV;
		goto out_unlock;
	}

	dua->raw_len = 0;
	hdr->txn_id = cpu_to_le32(++dua->txn);
	reinit_completion(&dua->ret_done);
	dua->want_event = DUA_RAW_EVENT;

	ret = rpmsg_send(dua->rpdev->ept, req->buf, req->len);
	if (ret)
		goto out_raw;

	if (wait_for_completion_timeout(&dua->ret_done, DUA_TIMEOUT))
		dua_raw_printf(dua, "return %d\n", dua->ret_val);
	else
		dua_raw_printf(dua, "return timeout\n");
	msleep(DUA_RAW_WAIT_MS);
	ret = count;

out_raw:
	dua->want_event = 0;
out_unlock:
	mutex_unlock(&dua->lock);
out_req:
	kfree(req);
out_line:
	kfree(line);

	return ret;
}

static ssize_t dua_raw_read(struct file *file, char __user *ubuf,
			    size_t count, loff_t *ppos)
{
	struct dvf99_dua *dua = file->private_data;

	return simple_read_from_buffer(ubuf, count, ppos, dua->raw_log,
				       dua->raw_len);
}

static const struct file_operations dua_raw_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = dua_raw_read,
	.write = dua_raw_write,
	.llseek = default_llseek,
};

static void dua_release(struct kref *ref)
{
	struct dvf99_dua *dua = container_of(ref, struct dvf99_dua, ref);

	mutex_destroy(&dua->audio_lock);
	mutex_destroy(&dua->lock);
	kfree(dua->raw_log);
	kfree(dua);
}

/**
 * dvf99_dua_get() - find the DUA instance of a CSS device
 * @css_dev: the remoteproc device of the CSS
 *
 * Return: the instance with a reference held, or an error pointer.
 * -EPROBE_DEFER means that the DUA driver did not probe yet. The instance
 * stays valid after the driver is removed; its requests then fail with
 * -ENODEV.
 */
struct dvf99_dua *dvf99_dua_get(struct device *css_dev)
{
	struct dvf99_dua *dua, *found = ERR_PTR(-EPROBE_DEFER);

	mutex_lock(&dua_list_lock);
	list_for_each_entry(dua, &dua_list, node) {
		if (dua->rpdev->dev.parent != css_dev)
			continue;
		kref_get(&dua->ref);
		found = dua;
		break;
	}
	mutex_unlock(&dua_list_lock);

	return found;
}
EXPORT_SYMBOL_GPL(dvf99_dua_get);

/**
 * dvf99_dua_put() - drop the reference taken by dvf99_dua_get()
 * @dua: DUA instance
 */
void dvf99_dua_put(struct dvf99_dua *dua)
{
	kref_put(&dua->ref, dua_release);
}
EXPORT_SYMBOL_GPL(dvf99_dua_put);

static int dua_cb(struct rpmsg_device *rpdev, void *data, int len, void *priv,
		  u32 src)
{
	struct dvf99_dua *dua = priv;
	struct dvf99_coma_cmsg *cmsg = data;
	struct dua_hdr *hdr;
	unsigned int nr, skip;
	s32 ind;

	if (len < sizeof(*cmsg))
		return -EINVAL;

	skip = sizeof(*cmsg) + le32_to_cpu(cmsg->params_size);
	if (len < skip + sizeof(*hdr))
		return -EINVAL;

	hdr = data + skip;
	nr = le32_to_cpu(hdr->nr_param);
	if (len < skip + struct_size(hdr, params, nr))
		return -EINVAL;

	switch (le32_to_cpu(hdr->msg_id)) {
	case DUA_FUNCTION_RETURN:
		if (nr < 1 || le32_to_cpu(hdr->txn_id) != dua->txn)
			break;
		dua->ret_val = le32_to_cpu(hdr->params[0]);
		complete(&dua->ret_done);
		break;
	case DUA_CALLBACK:
		if (nr < 4)
			break;
		ind = le32_to_cpu(hdr->params[3]);
		dev_dbg(&rpdev->dev, "callback uid %d elem %d ind 0x%x\n",
			(s32)le32_to_cpu(hdr->params[1]),
			(s32)le32_to_cpu(hdr->params[2]), ind);
		if (dua->want_event == DUA_RAW_EVENT) {
			dua_raw_log_cb(dua, hdr, nr);
			break;
		}
		if (ind >= 0 && (ind >> 8) != dua->want_event)
			break;
		dua->cb_ind = ind;
		dua->cb_uid = le32_to_cpu(hdr->params[1]);
		dua->cb_elem = le32_to_cpu(hdr->params[2]);
		complete(&dua->cb_done);
		break;
	case DUA_TEXT_TRACE:
		dev_dbg(&rpdev->dev, "trace: %.*s\n",
			(int)(len - skip - struct_size(hdr, params, nr)),
			(char *)hdr->params + 4 * nr);
		break;
	default:
		dev_warn_ratelimited(&rpdev->dev, "unknown message %u\n",
				     le32_to_cpu(hdr->msg_id));
		break;
	}

	return 0;
}

static int sharedmem_cb(struct rpmsg_device *rpdev, void *data, int len,
			void *priv, u32 src)
{
	struct dvf99_dua *dua = priv;
	struct dvf99_coma_cmsg *cmsg = data;

	if (len < sizeof(*cmsg) + sizeof(__le32) ||
	    le32_to_cpu(cmsg->type) != SHAREDMEM_INIT_REPLY)
		return -EINVAL;

	dua->shm_status = get_unaligned_le32(cmsg->body);
	complete(&dua->shm_done);

	return 0;
}

static int sharedmem_init(struct dvf99_dua *dua)
{
	struct sharedmem_init msg = {
		.hdr.type = cpu_to_le32(SHAREDMEM_INIT),
		.hdr.params_size = cpu_to_le32(3 * sizeof(__le32)),
		.virt_base = cpu_to_le32(dua->buf_dma),
		.phys_base = cpu_to_le32(dua->buf_dma),
		.size = cpu_to_le32(DUA_BUF_MAP_SIZE),
	};
	int ret;

	ret = rpmsg_send(dua->shm_ept, &msg, sizeof(msg));
	if (ret)
		return ret;

	if (!wait_for_completion_timeout(&dua->shm_done, DUA_TIMEOUT))
		return -ETIMEDOUT;

	if (dua->shm_status) {
		dev_err(&dua->rpdev->dev, "shared memory init failed: %d\n",
			dua->shm_status);
		return -EIO;
	}

	return 0;
}

static int dua_init(struct dvf99_dua *dua)
{
	__le32 *buf = dua->buf;
	DEFINE_FLEX(struct dua_req, req, buf, size, DUA_REQ_MAX);
	s32 params[5] = { DUA_INIT_MODE, -1, -1, DUA_INIT_REF, dua->buf_dma };

	memset(buf, 0, DUA_BUF_SIZE);
	buf[1] = cpu_to_le32(DUA_BUF_MAP_SIZE);
	buf[2] = cpu_to_le32(DUA_BUF_HDR_SIZE);

	dua_req_init(req, DUA_INIT_REQ, 5, params);
	dua_req_data(req, NULL, 0);

	return dua_run(dua, req, DUAEV_INIT_IND, NULL, NULL);
}

static int dua_probe(struct rpmsg_device *rpdev)
{
	struct rpmsg_channel_info chinfo = {
		.name = "sharedmem",
		.src = RPMSG_ADDR_ANY,
		.dst = RPMSG_ADDR_ANY,
	};
	struct device *dev = &rpdev->dev;
	struct dvf99_dua *dua;
	int ret;

	dua = kzalloc(sizeof(*dua), GFP_KERNEL);
	if (!dua)
		return -ENOMEM;

	kref_init(&dua->ref);
	dua->rpdev = rpdev;
	dua->audio_uid = -1;
	dua->audio_conn = -1;
	dua->audio_volume = -1;
	mutex_init(&dua->audio_lock);
	rpdev->ept->priv = dua;
	init_completion(&dua->ret_done);
	init_completion(&dua->cb_done);
	init_completion(&dua->shm_done);
	mutex_init(&dua->lock);
	spin_lock_init(&dua->raw_lock);
	dev_set_drvdata(dev, dua);

	dua->buf = dma_alloc_coherent(dev->parent, DUA_BUF_SIZE, &dua->buf_dma,
				      GFP_KERNEL);
	if (!dua->buf) {
		ret = -ENOMEM;
		goto err_put;
	}

	dua->shm_ept = rpmsg_create_ept(rpdev, sharedmem_cb, dua, chinfo);
	if (!dua->shm_ept) {
		ret = -ENODEV;
		goto err_free;
	}

	ret = sharedmem_init(dua);
	if (ret)
		goto err_ept;

	ret = dua_init(dua);
	if (ret < 0)
		goto err_ept;

	ret = dua_audio_setup(dua);
	if (ret) {
		dev_err(dev, "cannot set up the audio front end: %d\n", ret);
		goto err_audio;
	}

	dua_umt_dyn_setup(dua);

	mutex_lock(&dua_list_lock);
	list_add_tail(&dua->node, &dua_list);
	mutex_unlock(&dua_list_lock);

	dua->raw_log = kzalloc(DUA_RAW_LOG, GFP_KERNEL);
	if (dua->raw_log)
		dua->debugfs = debugfs_create_file("dvf99-dua", 0600, NULL, dua,
						   &dua_raw_fops);

	dev_info(dev, "DSP unit API ready, DSP id 0x%08x\n",
		 le32_to_cpu(((__le32 *)dua->buf)[4]));

	return 0;

err_audio:
	dua_audio_release(dua);
err_ept:
	rpmsg_destroy_ept(dua->shm_ept);
err_free:
	dma_free_coherent(dev->parent, DUA_BUF_SIZE, dua->buf, dua->buf_dma);
err_put:
	kref_put(&dua->ref, dua_release);

	return ret;
}

static void dua_remove(struct rpmsg_device *rpdev)
{
	struct dvf99_dua *dua = dev_get_drvdata(&rpdev->dev);

	mutex_lock(&dua_list_lock);
	list_del(&dua->node);
	mutex_unlock(&dua_list_lock);

	debugfs_remove(dua->debugfs);
	dua_audio_release(dua);

	mutex_lock(&dua->lock);
	dua->dead = true;
	mutex_unlock(&dua->lock);

	rpmsg_destroy_ept(dua->shm_ept);
	dma_free_coherent(rpdev->dev.parent, DUA_BUF_SIZE, dua->buf,
			  dua->buf_dma);
	kref_put(&dua->ref, dua_release);
}

static const struct rpmsg_device_id dua_id_table[] = {
	{ .name = "dua" },
	{ }
};
MODULE_DEVICE_TABLE(rpmsg, dua_id_table);

static struct rpmsg_driver dua_driver = {
	.probe = dua_probe,
	.remove = dua_remove,
	.callback = dua_cb,
	.id_table = dua_id_table,
	.drv.name = "dvf99-dua",
};
module_rpmsg_driver(dua_driver);

MODULE_DESCRIPTION("DSPG DVF99 CSS DSP unit API");
MODULE_LICENSE("GPL");
