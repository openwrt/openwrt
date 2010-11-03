#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/tapi/tapi.h>
#include <linux/tapi/tapi-ioctl.h>

/* FIXME Does it acutally make sense to allow more then one application at a
 * time to open the control device? For example calling sync from one app will
 * also sync all others. */

struct tapi_control_file {
	struct tapi_device *tdev;
	struct list_head links;
};

static struct tapi_endpoint *tapi_lookup_endpoint(struct tapi_device *tdev,
	unsigned int ep_id)
{
	struct tapi_stream *stream;

	if (ep_id < tdev->num_ports)
		return &tdev->ports[ep_id].ep;

	list_for_each_entry(stream, &tdev->streams, head) {
		if (stream->ep.id == ep_id)
			return &stream->ep;
	}

	return ERR_PTR(-ENOENT);
}

static inline struct tapi_device *inode_to_tdev(struct inode *inode)
{
	return container_of(inode->i_cdev, struct tapi_char_device, cdev)->tdev;
}

static int tapi_control_open(struct inode *inode, struct file *file)
{
	int ret;
	struct tapi_device *tdev = inode_to_tdev(inode);
	struct tapi_control_file *tctrl;

	get_device(&tdev->dev);

	tctrl = kzalloc(sizeof(*tctrl), GFP_KERNEL);
	if (!tctrl) {
		ret = -ENOMEM;
		goto err_put_device;
	}

	INIT_LIST_HEAD(&tctrl->links);
	tctrl->tdev = tdev;

	file->private_data = tctrl;

	return 0;

err_put_device:
	put_device(&tdev->dev);

	return ret;
}

static int tapi_control_release(struct inode *inode, struct file *file)
{
	struct tapi_control_file *tctrl = file->private_data;
	struct tapi_link *link;

	if (tctrl) {
		list_for_each_entry(link, &tctrl->links, head)
			tapi_link_free(tctrl->tdev, link);

		put_device(&tctrl->tdev->dev);
	}

	return 0;
}

static long tapi_control_ioctl_link_alloc(struct tapi_control_file *tctrl,
	unsigned long arg)
{
	struct tapi_link *link;
	struct tapi_endpoint *ep1, *ep2;

	ep1 = tapi_lookup_endpoint(tctrl->tdev, arg >> 16);
	ep2 = tapi_lookup_endpoint(tctrl->tdev, arg & 0xffff);

	link = tapi_link_alloc(tctrl->tdev, ep1, ep2);
	if (IS_ERR(link))
		return PTR_ERR(link);

	list_add_tail(&link->head, &tctrl->links);

	return link->id;
}

struct tapi_link *tapi_control_lookup_link(struct tapi_control_file *tctrl,
	unsigned int id)
{
	struct tapi_link *link;

	list_for_each_entry(link, &tctrl->links, head) {
		if (link->id == id)
			return link;
	}

	return NULL;
}

static long tapi_control_ioctl_link_free(struct tapi_control_file *tctrl,
	unsigned long arg)
{
	struct tapi_link *link = tapi_control_lookup_link(tctrl, arg);
	if (!link)
		return -ENOENT;

	tapi_link_free(tctrl->tdev, link);
	list_del(&link->head);

	return 0;
}

static long tapi_control_ioctl_link_enable(struct tapi_control_file *tctrl,
	unsigned long arg)
{
	struct tapi_link *link = tapi_control_lookup_link(tctrl, arg);
	if (!link)
		return -ENOENT;

	return tapi_link_enable(tctrl->tdev, link);
}

static long tapi_control_ioctl_link_disable(struct tapi_control_file *tctrl,
	unsigned long arg)
{
	struct tapi_link *link = tapi_control_lookup_link(tctrl, arg);
	if (!link)
		return -ENOENT;

	return tapi_link_disable(tctrl->tdev, link);
}

static long tapi_control_ioctl_sync(struct tapi_control_file *tctrl)
{
	return tapi_sync(tctrl->tdev);
}

static long tapi_control_ioctl(struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret;
	struct tapi_control_file *tctrl = file->private_data;

	switch (cmd) {
	case TAPI_CONTROL_IOCTL_LINK_ALLOC:
		ret = tapi_control_ioctl_link_alloc(tctrl, arg);
		break;
	case TAPI_CONTROL_IOCTL_LINK_FREE:
		ret = tapi_control_ioctl_link_free(tctrl, arg);
		break;
	case TAPI_CONTROL_IOCTL_LINK_ENABLE:
		ret = tapi_control_ioctl_link_enable(tctrl, arg);
		break;
	case TAPI_CONTROL_IOCTL_LINK_DISABLE:
		ret = tapi_control_ioctl_link_disable(tctrl, arg);
		break;
	case TAPI_CONTROL_IOCTL_SYNC:
		ret = tapi_control_ioctl_sync(tctrl);
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

static const struct file_operations tapi_control_file_ops = {
	.owner = THIS_MODULE,
	.open = tapi_control_open,
	.release = tapi_control_release,
	.unlocked_ioctl = tapi_control_ioctl,
};

int tapi_register_control_device(struct tapi_device* tdev)
{
	dev_set_name(&tdev->control_dev.dev, "tapi%uC", tdev->id);
	return tapi_char_device_register(tdev, &tdev->control_dev, &tapi_control_file_ops);
}
