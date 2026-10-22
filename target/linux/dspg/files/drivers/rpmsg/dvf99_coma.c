// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2007 NXP Semiconductors
 * Copyright (C) 2008 - 2013 DSP Group Inc.
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/mailbox_client.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#include "rpmsg_internal.h"

#define COMA_SERVICES		16
#define COMA_NAME_LEN		16
#define COMA_HDR		sizeof(__le32)
#define COMA_FIFO0_SIZE		SZ_4K
#define COMA_FIFO_SIZE		20480
#define COMA_REPLY_TIMEOUT	HZ
#define COMA_START_DELAY_MS	20
#define CFIFO_MAGIC		0x46494643

enum coma_msg {
	COMA_REQUEST_REGISTER = 3,
	COMA_REPLY_REGISTER,
	COMA_REQUEST_DEREGISTER,
	COMA_REPLY_DEREGISTER,
};

struct cfifo_shm {
	__le32 magic;
	__le32 size;
	__le32 in;
	__le32 out;
	u8 buffer[];
};

struct dvf99_cfifo {
	struct cfifo_shm *shm;
	dma_addr_t pa;
	size_t alloc;
	u32 size;
};

struct coma_register_params {
	__le32 id;
	char name[COMA_NAME_LEN];
	__le32 l2c;
	__le32 c2l;
} __packed;

struct coma_reply_params {
	__le32 id;
	__le32 result;
} __packed;

struct coma_register_msg {
	struct dvf99_coma_cmsg hdr;
	struct coma_register_params params;
} __packed;

struct coma_service {
	struct dvf99_coma *coma;
	unsigned int id;
	char name[COMA_NAME_LEN];
	struct dvf99_cfifo l2c;
	struct dvf99_cfifo c2l;
	spinlock_t tx_lock;
	struct mutex rx_lock;
	struct work_struct rx_work;
	struct mbox_client client;
	struct mbox_chan *chan;
	struct rpmsg_endpoint *ept;
	bool active;
};

struct coma_endpoint {
	struct rpmsg_endpoint ept;
	struct coma_service *svc;
};

struct coma_device {
	struct rpmsg_device rpdev;
	struct dvf99_coma *coma;
};

struct coma_channel {
	const char *name;
	unsigned int id;
	size_t fifo_size;
};

static const struct coma_channel coma_channels[] = {
	{ .name = "debug", .id = 1, .fifo_size = COMA_FIFO_SIZE },
	{ .name = "dvf9a", .id = 5, .fifo_size = 10240 },
	{ .name = "sharedmem", .id = 9, .fifo_size = 1024 },
	{ .name = "dua", .id = 2, .fifo_size = COMA_FIFO_SIZE },
	{ .name = "alsa", .id = 4, .fifo_size = 8000 * 14 },
	{ .name = "voice", .id = 6, .fifo_size = COMA_FIFO_SIZE },
	{ .name = "rtp-event", .id = 12, .fifo_size = COMA_FIFO_SIZE },
};

struct dvf99_coma {
	struct device *dev;
	struct coma_service svc[COMA_SERVICES];
	struct mutex lock;
	struct completion reply;
	s32 reply_result;
	struct workqueue_struct *wq;
	struct rpmsg_device *rpdevs[ARRAY_SIZE(coma_channels) + 1];
};

static const u32 coma_kick = 0xdeadbeef;

static int cfifo_alloc(struct device *dev, struct dvf99_cfifo *f, size_t size)
{
	f->alloc = ALIGN(size, COMA_HDR);
	f->size = f->alloc - sizeof(*f->shm);
	f->shm = dma_alloc_coherent(dev, f->alloc, &f->pa, GFP_KERNEL);
	if (!f->shm)
		return -ENOMEM;

	memset(f->shm, 0, f->alloc);
	f->shm->magic = cpu_to_le32(CFIFO_MAGIC);
	f->shm->size = cpu_to_le32(f->size);
	dma_wmb();

	return 0;
}

static void cfifo_free(struct device *dev, struct dvf99_cfifo *f)
{
	if (!f->shm)
		return;

	dma_free_coherent(dev, f->alloc, f->shm, f->pa);
	f->shm = NULL;
}

/**
 * dvf99_cfifo_write() - append one message to a Linux to CSS FIFO
 * @f: FIFO
 * @data: message
 * @len: message length in bytes
 *
 * The caller serialises the writers of one FIFO.
 *
 * Return: 0 on success, -EAGAIN if the FIFO is full, or -EPIPE if the FIFO
 * state is corrupt.
 */
int dvf99_cfifo_write(struct dvf99_cfifo *f, const void *data, u32 len)
{
	u32 in = le32_to_cpu(READ_ONCE(f->shm->in));
	u32 out = le32_to_cpu(READ_ONCE(f->shm->out));
	u32 need = len + 2 * COMA_HDR;
	__le32 hdr = cpu_to_le32(len);
	u32 pos, next;

	if (in >= f->size || out >= f->size)
		return -EPIPE;

	if (in >= out && need <= f->size - in) {
		pos = in;
	} else if (in < out && need <= out - in) {
		pos = in;
	} else if (in >= out && need <= out) {
		memset(f->shm->buffer + in, 0, COMA_HDR);
		pos = 0;
	} else {
		return -EAGAIN;
	}

	next = ALIGN(pos + COMA_HDR + len, COMA_HDR);
	if (next > f->size - COMA_HDR)
		return -EPIPE;

	memcpy(f->shm->buffer + pos, &hdr, COMA_HDR);
	memcpy(f->shm->buffer + pos + COMA_HDR, data, len);
	dma_wmb();
	WRITE_ONCE(f->shm->in, cpu_to_le32(next));

	return 0;
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_write);

/**
 * dvf99_cfifo_read() - get the oldest message of a CSS to Linux FIFO
 * @f: FIFO
 * @msg: returns a pointer to the message inside the FIFO
 * @next: returns the read position for dvf99_cfifo_consume()
 *
 * The message stays in the FIFO until dvf99_cfifo_consume() releases it. The
 * caller serialises the readers of one FIFO.
 *
 * Return: the message length, 0 if the FIFO is empty, or -EPIPE if the FIFO
 * state is corrupt.
 */
int dvf99_cfifo_read(struct dvf99_cfifo *f, void **msg, u32 *next)
{
	u32 in = le32_to_cpu(READ_ONCE(f->shm->in));
	u32 out = le32_to_cpu(READ_ONCE(f->shm->out));
	__le32 hdr;
	u32 len;

	if (in >= f->size || out >= f->size)
		return -EPIPE;

	if (in == out)
		return 0;

	dma_rmb();

	if (f->size - out < COMA_HDR)
		return -EPIPE;

	memcpy(&hdr, f->shm->buffer + out, COMA_HDR);
	len = le32_to_cpu(hdr);
	if (!len) {
		if (!out || !in)
			return -EPIPE;

		out = 0;
		memcpy(&hdr, f->shm->buffer, COMA_HDR);
		len = le32_to_cpu(hdr);
		if (!len || len > f->size - COMA_HDR)
			return -EPIPE;
	}

	out += COMA_HDR;
	if (len > f->size - out)
		return -EPIPE;

	*msg = f->shm->buffer + out;
	*next = ALIGN(out + len, COMA_HDR);

	return len;
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_read);

/**
 * dvf99_cfifo_consume() - release the message returned by dvf99_cfifo_read()
 * @f: FIFO
 * @next: read position from dvf99_cfifo_read()
 */
void dvf99_cfifo_consume(struct dvf99_cfifo *f, u32 next)
{
	dma_mb();
	WRITE_ONCE(f->shm->out, cpu_to_le32(next));
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_consume);

/**
 * dvf99_cfifo_empty() - check whether a FIFO holds no message
 * @f: FIFO
 *
 * Return: true if the FIFO is empty.
 */
bool dvf99_cfifo_empty(struct dvf99_cfifo *f)
{
	return READ_ONCE(f->shm->in) == READ_ONCE(f->shm->out);
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_empty);

/**
 * dvf99_cfifo_create() - allocate a FIFO in the shared memory of the CSS
 * @dev: CSS device that owns the shared memory pool
 * @size: FIFO size in bytes, header included
 *
 * Return: the FIFO or an error pointer.
 */
struct dvf99_cfifo *dvf99_cfifo_create(struct device *dev, size_t size)
{
	struct dvf99_cfifo *f;

	f = kzalloc(sizeof(*f), GFP_KERNEL);
	if (!f)
		return ERR_PTR(-ENOMEM);

	if (cfifo_alloc(dev, f, size)) {
		kfree(f);
		return ERR_PTR(-ENOMEM);
	}

	return f;
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_create);

/**
 * dvf99_cfifo_destroy() - free a FIFO from dvf99_cfifo_create()
 * @dev: CSS device passed to dvf99_cfifo_create()
 * @f: FIFO
 */
void dvf99_cfifo_destroy(struct device *dev, struct dvf99_cfifo *f)
{
	cfifo_free(dev, f);
	kfree(f);
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_destroy);

/**
 * dvf99_cfifo_phys() - get the bus address of a FIFO for the CSS
 * @f: FIFO
 *
 * Return: the bus address of the FIFO header.
 */
dma_addr_t dvf99_cfifo_phys(const struct dvf99_cfifo *f)
{
	return f->pa;
}
EXPORT_SYMBOL_GPL(dvf99_cfifo_phys);

static int coma_svc_send(struct coma_service *svc, const void *data, int len)
{
	unsigned long flags;
	int ret;

	if (!svc->active)
		return -EPIPE;

	spin_lock_irqsave(&svc->tx_lock, flags);
	ret = dvf99_cfifo_write(&svc->l2c, data, len);
	spin_unlock_irqrestore(&svc->tx_lock, flags);
	if (ret)
		return ret;

	ret = mbox_send_message(svc->chan, (void *)&coma_kick);
	if (ret < 0)
		return ret;

	mbox_client_txdone(svc->chan, 0);

	return 0;
}

static void coma_svc0_handle(struct dvf99_coma *coma, void *msg, u32 len)
{
	struct dvf99_coma_cmsg *cmsg = msg;
	struct coma_reply_params *reply = (void *)cmsg->body;

	if (len < sizeof(*cmsg) + sizeof(*reply) ||
	    le32_to_cpu(cmsg->params_size) < sizeof(*reply))
		return;

	switch (le32_to_cpu(cmsg->type)) {
	case COMA_REPLY_REGISTER:
		coma->reply_result = le32_to_cpu(reply->result);
		complete(&coma->reply);
		break;
	case COMA_REPLY_DEREGISTER:
		complete(&coma->reply);
		break;
	}
}

static void coma_rx_work(struct work_struct *work)
{
	struct coma_service *svc = container_of(work, struct coma_service,
						rx_work);
	struct dvf99_coma *coma = svc->coma;
	struct rpmsg_endpoint *ept;
	void *msg;
	u32 next;
	int len;

	mutex_lock(&svc->rx_lock);

	while (svc->active) {
		len = dvf99_cfifo_read(&svc->c2l, &msg, &next);
		if (len < 0)
			dev_err_ratelimited(coma->dev, "service %u: bad fifo\n",
					    svc->id);
		if (len <= 0)
			break;

		ept = svc->ept;
		if (!svc->id)
			coma_svc0_handle(coma, msg, len);
		else if (ept && ept->cb)
			ept->cb(ept->rpdev, msg, len, ept->priv,
				RPMSG_ADDR_ANY);

		dvf99_cfifo_consume(&svc->c2l, next);
	}

	mutex_unlock(&svc->rx_lock);
}

static void coma_mbox_rx(struct mbox_client *client, void *msg)
{
	struct coma_service *svc = container_of(client, struct coma_service,
						client);

	queue_work(svc->coma->wq, &svc->rx_work);
}

static void coma_svc_close(struct coma_service *svc)
{
	struct device *dev = svc->coma->dev;

	svc->active = false;
	flush_work(&svc->rx_work);
	cfifo_free(dev, &svc->c2l);
	cfifo_free(dev, &svc->l2c);
	svc->name[0] = '\0';
}

static int coma_svc_open(struct coma_service *svc, const char *name,
			 size_t size)
{
	struct device *dev = svc->coma->dev;
	int ret;

	ret = cfifo_alloc(dev, &svc->l2c, size);
	if (ret)
		return ret;

	ret = cfifo_alloc(dev, &svc->c2l, size);
	if (ret) {
		cfifo_free(dev, &svc->l2c);
		return ret;
	}

	strscpy(svc->name, name, sizeof(svc->name));
	svc->active = true;

	return 0;
}

static const struct coma_channel *coma_channel_get(const char *name)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(coma_channels); i++)
		if (!strcmp(coma_channels[i].name, name))
			return &coma_channels[i];

	return NULL;
}

static bool coma_id_reserved(unsigned int id)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(coma_channels); i++)
		if (coma_channels[i].id == id)
			return true;

	return false;
}

static int coma_request(struct dvf99_coma *coma, enum coma_msg type,
			struct coma_service *svc)
{
	struct coma_register_msg msg = { };
	int ret;

	msg.hdr.type = cpu_to_le32(type);
	msg.hdr.params_size = cpu_to_le32(sizeof(msg.params));
	msg.params.id = cpu_to_le32(svc->id);

	if (type == COMA_REQUEST_REGISTER) {
		strscpy(msg.params.name, svc->name, sizeof(msg.params.name));
		msg.params.l2c = cpu_to_le32(svc->l2c.pa);
		msg.params.c2l = cpu_to_le32(svc->c2l.pa);
	}

	reinit_completion(&coma->reply);
	coma->reply_result = -1;

	ret = coma_svc_send(&coma->svc[0], &msg, sizeof(msg));
	if (ret)
		return ret;

	if (!wait_for_completion_timeout(&coma->reply, COMA_REPLY_TIMEOUT))
		return -ETIMEDOUT;

	if (type == COMA_REQUEST_REGISTER && coma->reply_result)
		return -ECONNREFUSED;

	return 0;
}

static struct coma_service *coma_svc_register(struct dvf99_coma *coma,
					      const char *name)
{
	const struct coma_channel *chan = coma_channel_get(name);
	struct coma_service *svc = NULL;
	unsigned int i;
	int ret;

	if (strlen(name) >= COMA_NAME_LEN)
		return ERR_PTR(-EINVAL);

	mutex_lock(&coma->lock);

	if (!coma->svc[0].active) {
		ret = -ENODEV;
		goto err;
	}

	for (i = 1; i < COMA_SERVICES; i++) {
		if (coma->svc[i].active && !strcmp(coma->svc[i].name, name)) {
			ret = -EBUSY;
			goto err;
		}
		if (!chan && !svc && !coma->svc[i].active &&
		    !coma_id_reserved(i))
			svc = &coma->svc[i];
	}

	if (chan)
		svc = &coma->svc[chan->id];

	if (!svc) {
		ret = -EMFILE;
		goto err;
	}

	if (svc->active) {
		ret = -EBUSY;
		goto err;
	}

	ret = coma_svc_open(svc, name, chan ? chan->fifo_size : COMA_FIFO_SIZE);
	if (ret)
		goto err;

	ret = coma_request(coma, COMA_REQUEST_REGISTER, svc);
	if (ret) {
		coma_svc_close(svc);
		goto err;
	}

	mutex_unlock(&coma->lock);

	return svc;

err:
	mutex_unlock(&coma->lock);

	return ERR_PTR(ret);
}

static void coma_svc_deregister(struct coma_service *svc)
{
	struct dvf99_coma *coma = svc->coma;

	mutex_lock(&coma->lock);

	if (svc->active && coma->svc[0].active &&
	    coma_request(coma, COMA_REQUEST_DEREGISTER, svc))
		dev_warn(coma->dev, "deregister of %s timed out\n", svc->name);

	coma_svc_close(svc);

	mutex_unlock(&coma->lock);
}

static void coma_ept_release(struct kref *kref)
{
	struct rpmsg_endpoint *ept = container_of(kref, struct rpmsg_endpoint,
						  refcount);

	kfree(container_of(ept, struct coma_endpoint, ept));
}

static void coma_destroy_ept(struct rpmsg_endpoint *ept)
{
	struct coma_endpoint *cept = container_of(ept, struct coma_endpoint,
						  ept);
	struct coma_service *svc = cept->svc;

	mutex_lock(&svc->rx_lock);
	svc->ept = NULL;
	mutex_unlock(&svc->rx_lock);

	coma_svc_deregister(svc);
	kref_put(&ept->refcount, coma_ept_release);
}

static int coma_send(struct rpmsg_endpoint *ept, void *data, int len)
{
	struct coma_endpoint *cept = container_of(ept, struct coma_endpoint,
						  ept);

	return coma_svc_send(cept->svc, data, len);
}

static int coma_sendto(struct rpmsg_endpoint *ept, void *data, int len,
		       u32 dst)
{
	return coma_send(ept, data, len);
}

static ssize_t coma_get_mtu(struct rpmsg_endpoint *ept)
{
	struct coma_endpoint *cept = container_of(ept, struct coma_endpoint,
						  ept);

	return cept->svc->l2c.size / 2 - 2 * COMA_HDR;
}

static const struct rpmsg_endpoint_ops coma_endpoint_ops = {
	.destroy_ept = coma_destroy_ept,
	.send = coma_send,
	.trysend = coma_send,
	.sendto = coma_sendto,
	.trysendto = coma_sendto,
	.get_mtu = coma_get_mtu,
};

static struct rpmsg_endpoint *coma_create_ept(struct rpmsg_device *rpdev,
					      rpmsg_rx_cb_t cb, void *priv,
					      struct rpmsg_channel_info chinfo)
{
	struct coma_device *cdev = container_of(rpdev, struct coma_device,
						rpdev);
	struct coma_endpoint *cept;
	struct coma_service *svc;

	cept = kzalloc(sizeof(*cept), GFP_KERNEL);
	if (!cept)
		return NULL;

	kref_init(&cept->ept.refcount);
	cept->ept.rpdev = rpdev;
	cept->ept.cb = cb;
	cept->ept.priv = priv;
	cept->ept.ops = &coma_endpoint_ops;

	svc = coma_svc_register(cdev->coma, chinfo.name);
	if (IS_ERR(svc)) {
		dev_err(&rpdev->dev, "cannot register %s: %pe\n", chinfo.name,
			svc);
		kfree(cept);
		return NULL;
	}

	cept->svc = svc;

	mutex_lock(&svc->rx_lock);
	svc->ept = &cept->ept;
	mutex_unlock(&svc->rx_lock);

	queue_work(cdev->coma->wq, &svc->rx_work);

	return &cept->ept;
}

static const struct rpmsg_device_ops coma_device_ops = {
	.create_ept = coma_create_ept,
};

static void coma_device_release(struct device *dev)
{
	struct rpmsg_device *rpdev = to_rpmsg_device(dev);

	of_node_put(dev->of_node);
	kfree(container_of(rpdev, struct coma_device, rpdev));
}

static struct rpmsg_device *coma_device_create(struct dvf99_coma *coma,
					       const char *name)
{
	struct coma_device *cdev;
	int ret;

	cdev = kzalloc(sizeof(*cdev), GFP_KERNEL);
	if (!cdev)
		return ERR_PTR(-ENOMEM);

	cdev->coma = coma;
	cdev->rpdev.ops = &coma_device_ops;
	cdev->rpdev.src = RPMSG_ADDR_ANY;
	cdev->rpdev.dst = RPMSG_ADDR_ANY;
	cdev->rpdev.dev.parent = coma->dev;
	cdev->rpdev.dev.release = coma_device_release;

	if (name) {
		strscpy(cdev->rpdev.id.name, name, sizeof(cdev->rpdev.id.name));
		cdev->rpdev.dev.of_node = of_get_child_by_name(coma->dev->of_node,
							       name);
		ret = rpmsg_register_device(&cdev->rpdev);
	} else {
		ret = rpmsg_ctrldev_register_device(&cdev->rpdev);
	}

	if (ret)
		return ERR_PTR(ret);

	return &cdev->rpdev;
}

static void coma_devices_remove(struct dvf99_coma *coma)
{
	unsigned int i = ARRAY_SIZE(coma->rpdevs);

	while (i--) {
		if (!coma->rpdevs[i])
			continue;

		device_unregister(&coma->rpdevs[i]->dev);
		coma->rpdevs[i] = NULL;
	}
}

/**
 * dvf99_coma_prepare() - allocate the manager FIFOs before the CSS starts
 * @coma: coma instance from devm_dvf99_coma_create()
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_coma_prepare(struct dvf99_coma *coma)
{
	struct coma_service *svc0 = &coma->svc[0];
	int ret;

	mutex_lock(&coma->lock);
	ret = coma_svc_open(svc0, "coma", COMA_FIFO0_SIZE);
	mutex_unlock(&coma->lock);

	return ret;
}
EXPORT_SYMBOL_GPL(dvf99_coma_prepare);

/**
 * dvf99_coma_fifos_get() - get the manager FIFO addresses for the CSS
 * @coma: coma instance from devm_dvf99_coma_create()
 * @l2c: returns the bus address of the Linux to CSS FIFO
 * @c2l: returns the bus address of the CSS to Linux FIFO
 */
void dvf99_coma_fifos_get(struct dvf99_coma *coma, dma_addr_t *l2c,
			  dma_addr_t *c2l)
{
	*l2c = coma->svc[0].l2c.pa;
	*c2l = coma->svc[0].c2l.pa;
}
EXPORT_SYMBOL_GPL(dvf99_coma_fifos_get);

/**
 * dvf99_coma_start() - create the rpmsg devices once the CSS runs
 * @coma: coma instance from devm_dvf99_coma_create()
 *
 * Return: 0 on success or a negative error code.
 */
int dvf99_coma_start(struct dvf99_coma *coma)
{
	struct rpmsg_device *rpdev;
	unsigned int i;

	msleep(COMA_START_DELAY_MS);

	for (i = 0; i < ARRAY_SIZE(coma->rpdevs); i++) {
		rpdev = coma_device_create(coma, i < ARRAY_SIZE(coma_channels) ?
					   coma_channels[i].name : NULL);
		if (IS_ERR(rpdev)) {
			coma_devices_remove(coma);
			return PTR_ERR(rpdev);
		}

		coma->rpdevs[i] = rpdev;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(dvf99_coma_start);

/**
 * dvf99_coma_stop() - remove the rpmsg devices before the CSS stops
 * @coma: coma instance from devm_dvf99_coma_create()
 * @crashed: true if the CSS crashed and cannot answer
 */
void dvf99_coma_stop(struct dvf99_coma *coma, bool crashed)
{
	mutex_lock(&coma->lock);
	coma->svc[0].active = false;
	mutex_unlock(&coma->lock);

	coma_devices_remove(coma);
}
EXPORT_SYMBOL_GPL(dvf99_coma_stop);

/**
 * dvf99_coma_unprepare() - free the manager FIFOs after the CSS stopped
 * @coma: coma instance from devm_dvf99_coma_create()
 */
void dvf99_coma_unprepare(struct dvf99_coma *coma)
{
	mutex_lock(&coma->lock);
	coma_svc_close(&coma->svc[0]);
	mutex_unlock(&coma->lock);
}
EXPORT_SYMBOL_GPL(dvf99_coma_unprepare);

static void coma_release(void *data)
{
	struct dvf99_coma *coma = data;
	unsigned int i;

	for (i = 0; i < COMA_SERVICES; i++)
		if (!IS_ERR_OR_NULL(coma->svc[i].chan))
			mbox_free_channel(coma->svc[i].chan);

	destroy_workqueue(coma->wq);
}

/**
 * devm_dvf99_coma_create() - create the coma transport of a CSS device
 * @dev: CSS device; it carries the "coma0" to "coma15" mailboxes and the
 *	 shared memory pool
 *
 * Return: the coma instance or an error pointer.
 */
struct dvf99_coma *devm_dvf99_coma_create(struct device *dev)
{
	struct dvf99_coma *coma;
	char name[8];
	unsigned int i;
	int ret;

	coma = devm_kzalloc(dev, sizeof(*coma), GFP_KERNEL);
	if (!coma)
		return ERR_PTR(-ENOMEM);

	coma->dev = dev;
	init_completion(&coma->reply);

	ret = devm_mutex_init(dev, &coma->lock);
	if (ret)
		return ERR_PTR(ret);

	coma->wq = alloc_ordered_workqueue("coma", 0);
	if (!coma->wq)
		return ERR_PTR(-ENOMEM);

	ret = devm_add_action_or_reset(dev, coma_release, coma);
	if (ret)
		return ERR_PTR(ret);

	for (i = 0; i < COMA_SERVICES; i++) {
		struct coma_service *svc = &coma->svc[i];

		svc->coma = coma;
		svc->id = i;
		spin_lock_init(&svc->tx_lock);
		mutex_init(&svc->rx_lock);
		INIT_WORK(&svc->rx_work, coma_rx_work);

		svc->client.dev = dev;
		svc->client.rx_callback = coma_mbox_rx;
		svc->client.knows_txdone = true;

		snprintf(name, sizeof(name), "coma%u", i);
		svc->chan = mbox_request_channel_byname(&svc->client, name);
		if (IS_ERR(svc->chan))
			return ERR_CAST(svc->chan);
	}

	return coma;
}
EXPORT_SYMBOL_GPL(devm_dvf99_coma_create);

MODULE_DESCRIPTION("DSPG DVF99 coma rpmsg transport");
MODULE_LICENSE("GPL");
