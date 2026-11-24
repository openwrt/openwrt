// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSP Group
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/mfd/dvf9a.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/rpmsg.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/workqueue.h>

#define DVF9A_COMA_ACK_TIMEOUT	msecs_to_jiffies(100)
#define DVF9A_HW_REVISION	0
#define DVF9A_COMA_LINES	13

enum dvf9a_coma_type {
	DVF9A_COMA_REQUEST_REGISTER_IRQ,
	DVF9A_COMA_REPLY_REGISTER_IRQ,
	DVF9A_COMA_REQUEST_RELEASE_IRQ,
	DVF9A_COMA_REPLY_RELEASE_IRQ,
	DVF9A_COMA_INTERRUPT,
	DVF9A_COMA_ACK_INTERRUPT,
	DVF9A_COMA_ACQUIRE,
	DVF9A_COMA_ACK_ACQUIRE,
};

struct dvf9a_coma_msg {
	struct dvf99_coma_cmsg hdr;
	__le32 param;
} __packed;

struct dvf9a_coma;

struct dvf9a_coma_line {
	struct dvf9a_coma *dc;
	unsigned int hwirq;
	int virq;
	bool requested;
	bool masked;
};

struct dvf9a_coma_req {
	struct work_struct work;
	struct dvf9a_coma *dc;
	u32 type;
	u32 hwirq;
};

struct dvf9a_coma {
	struct rpmsg_device *rpdev;
	struct device *dvf9a_dev;
	struct dvf9a *dvf9a;
	struct completion ack;
	struct work_struct release_work;
	struct workqueue_struct *wq;
	struct mutex lines_lock;
	struct dvf9a_coma_line lines[DVF9A_COMA_LINES];
};

static int dvf9a_coma_send(struct dvf9a_coma *dc, enum dvf9a_coma_type type,
			   s32 param)
{
	struct dvf9a_coma_msg msg = {
		.hdr.type = cpu_to_le32(type),
		.hdr.params_size = cpu_to_le32(sizeof(msg.param)),
		.param = cpu_to_le32(param),
	};

	return rpmsg_send(dc->rpdev->ept, &msg, sizeof(msg));
}

static int dvf9a_coma_acquire(void *priv)
{
	struct dvf9a_coma *dc = priv;
	int ret;

	reinit_completion(&dc->ack);

	ret = dvf9a_coma_send(dc, DVF9A_COMA_ACQUIRE, 0);
	if (ret)
		return ret;

	if (!wait_for_completion_timeout(&dc->ack, DVF9A_COMA_ACK_TIMEOUT))
		return -ETIMEDOUT;

	return 0;
}

static void dvf9a_coma_release_work(struct work_struct *work)
{
	struct dvf9a_coma *dc = container_of(work, struct dvf9a_coma,
					     release_work);

	dvf9a_owner_release(dc->dvf9a);
	dvf9a_coma_send(dc, DVF9A_COMA_ACK_ACQUIRE, 0);
}

static irqreturn_t dvf9a_coma_irq(int virq, void *data)
{
	struct dvf9a_coma_line *line = data;

	disable_irq_nosync(virq);
	WRITE_ONCE(line->masked, true);
	dvf9a_coma_send(line->dc, DVF9A_COMA_INTERRUPT, line->hwirq);

	return IRQ_HANDLED;
}

static int dvf9a_coma_line_request(struct dvf9a_coma *dc, u32 hwirq)
{
	struct dvf9a_coma_line *line;
	int virq, ret;

	if (hwirq >= DVF9A_COMA_LINES)
		return -EINVAL;

	line = &dc->lines[hwirq];
	if (line->requested)
		return -EBUSY;

	virq = dvf9a_irq_get(dc->dvf9a, hwirq);
	if (virq < 0)
		return virq;

	line->virq = virq;
	line->masked = false;
	ret = request_threaded_irq(virq, NULL, dvf9a_coma_irq, IRQF_ONESHOT,
				   dev_name(&dc->rpdev->dev), line);
	if (ret)
		return ret;

	line->requested = true;

	return 0;
}

static int dvf9a_coma_line_release(struct dvf9a_coma *dc, u32 hwirq)
{
	struct dvf9a_coma_line *line;

	if (hwirq >= DVF9A_COMA_LINES)
		return -EINVAL;

	line = &dc->lines[hwirq];
	if (!line->requested)
		return -EINVAL;

	free_irq(line->virq, line);
	line->requested = false;

	return 0;
}

static void dvf9a_coma_req_work(struct work_struct *work)
{
	struct dvf9a_coma_req *req = container_of(work, struct dvf9a_coma_req,
						  work);
	struct dvf9a_coma *dc = req->dc;
	int ret;

	mutex_lock(&dc->lines_lock);
	if (req->type == DVF9A_COMA_REQUEST_REGISTER_IRQ)
		ret = dvf9a_coma_line_request(dc, req->hwirq);
	else
		ret = dvf9a_coma_line_release(dc, req->hwirq);
	mutex_unlock(&dc->lines_lock);

	if (ret)
		dev_warn(&dc->rpdev->dev, "DVF9A interrupt %u request %u: %pe\n",
			 req->hwirq, req->type, ERR_PTR(ret));

	dvf9a_coma_send(dc, req->type + 1, ret);
	kfree(req);
}

static void dvf9a_coma_ack(struct dvf9a_coma *dc, u32 hwirq)
{
	struct dvf9a_coma_line *line;

	if (hwirq >= DVF9A_COMA_LINES)
		return;

	line = &dc->lines[hwirq];

	mutex_lock(&dc->lines_lock);
	if (line->requested && line->masked) {
		line->masked = false;
		enable_irq(line->virq);
	}
	mutex_unlock(&dc->lines_lock);
}

static int dvf9a_coma_cb(struct rpmsg_device *rpdev, void *data, int len,
			 void *priv, u32 src)
{
	struct dvf9a_coma *dc = dev_get_drvdata(&rpdev->dev);
	struct dvf9a_coma_msg *msg = data;
	struct dvf9a_coma_req *req;
	u32 type;

	if (len < sizeof(msg->hdr))
		return -EINVAL;

	type = le32_to_cpu(msg->hdr.type);

	switch (type) {
	case DVF9A_COMA_ACQUIRE:
		schedule_work(&dc->release_work);
		break;
	case DVF9A_COMA_ACK_ACQUIRE:
		complete(&dc->ack);
		break;
	case DVF9A_COMA_REQUEST_REGISTER_IRQ:
	case DVF9A_COMA_REQUEST_RELEASE_IRQ:
		if (len < sizeof(*msg))
			return -EINVAL;
		req = kzalloc(sizeof(*req), GFP_KERNEL);
		if (!req)
			return -ENOMEM;
		INIT_WORK(&req->work, dvf9a_coma_req_work);
		req->dc = dc;
		req->type = type;
		req->hwirq = le32_to_cpu(msg->param);
		queue_work(dc->wq, &req->work);
		break;
	case DVF9A_COMA_ACK_INTERRUPT:
		if (len < sizeof(*msg))
			return -EINVAL;
		dvf9a_coma_ack(dc, le32_to_cpu(msg->param));
		break;
	default:
		dev_err_ratelimited(&rpdev->dev, "unknown message %u\n", type);
		return -EINVAL;
	}

	return 0;
}

static int dvf9a_coma_probe(struct rpmsg_device *rpdev)
{
	struct device *dev = &rpdev->dev;
	struct platform_device *pdev;
	struct device_node *np;
	struct dvf9a_coma *dc;
	unsigned int i;
	int ret;

	dc = devm_kzalloc(dev, sizeof(*dc), GFP_KERNEL);
	if (!dc)
		return -ENOMEM;

	dc->rpdev = rpdev;
	init_completion(&dc->ack);
	INIT_WORK(&dc->release_work, dvf9a_coma_release_work);
	dev_set_drvdata(dev, dc);

	for (i = 0; i < DVF9A_COMA_LINES; i++) {
		dc->lines[i].dc = dc;
		dc->lines[i].hwirq = i;
	}

	ret = devm_mutex_init(dev, &dc->lines_lock);
	if (ret)
		return ret;

	np = of_parse_phandle(dev->of_node, "dspg,dvf9a", 0);
	if (!np)
		return dev_err_probe(dev, -ENODEV, "no dspg,dvf9a phandle\n");

	pdev = of_find_device_by_node(np);
	of_node_put(np);
	if (!pdev)
		return -EPROBE_DEFER;

	dc->dvf9a_dev = &pdev->dev;
	dc->dvf9a = platform_get_drvdata(pdev);
	if (!dc->dvf9a) {
		put_device(dc->dvf9a_dev);
		return -EPROBE_DEFER;
	}

	dc->wq = alloc_ordered_workqueue("dvf9a-coma", 0);
	if (!dc->wq) {
		put_device(dc->dvf9a_dev);
		return -ENOMEM;
	}

	ret = dvf9a_acquire_hook_set(dc->dvf9a, dvf9a_coma_acquire, dc);
	if (ret) {
		destroy_workqueue(dc->wq);
		put_device(dc->dvf9a_dev);
	}

	return ret;
}

static void dvf9a_coma_remove(struct rpmsg_device *rpdev)
{
	struct dvf9a_coma *dc = dev_get_drvdata(&rpdev->dev);
	struct regmap *regmap = dev_get_regmap(dc->dvf9a_dev, NULL);
	unsigned int val, i;

	destroy_workqueue(dc->wq);

	mutex_lock(&dc->lines_lock);
	for (i = 0; i < DVF9A_COMA_LINES; i++)
		if (dc->lines[i].requested)
			dvf9a_coma_line_release(dc, i);
	mutex_unlock(&dc->lines_lock);

	if (regmap)
		regmap_read(regmap, DVF9A_HW_REVISION, &val);

	dvf9a_acquire_hook_set(dc->dvf9a, NULL, NULL);
	cancel_work_sync(&dc->release_work);
	put_device(dc->dvf9a_dev);
}

static const struct rpmsg_device_id dvf9a_coma_id_table[] = {
	{ .name = "dvf9a" },
	{ }
};
MODULE_DEVICE_TABLE(rpmsg, dvf9a_coma_id_table);

static struct rpmsg_driver dvf9a_coma_driver = {
	.probe = dvf9a_coma_probe,
	.remove = dvf9a_coma_remove,
	.callback = dvf9a_coma_cb,
	.id_table = dvf9a_coma_id_table,
	.drv.name = "dvf9a-coma",
};
module_rpmsg_driver(dvf9a_coma_driver);

MODULE_DESCRIPTION("DSPG DVF9A DAIF sharing with the DVF99 CSS");
MODULE_LICENSE("GPL");
