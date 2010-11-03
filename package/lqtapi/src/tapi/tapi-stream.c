#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/poll.h>

#include <linux/tapi/tapi.h>
#include <linux/tapi/tapi-ioctl.h>


struct tapi_stream_file {
	struct tapi_device *tdev;
	struct tapi_stream *stream;
};

static inline struct tapi_device *inode_to_tdev(struct inode *inode)
{
	return container_of(inode->i_cdev, struct tapi_char_device, cdev)->tdev;
}

static int tapi_stream_open(struct inode *inode, struct file *file)
{
	int ret;
	struct tapi_device *tdev = inode_to_tdev(inode);
	struct tapi_stream_file *stream;

	get_device(&tdev->dev);

	stream = kzalloc(sizeof(*stream), GFP_KERNEL);
	if (!stream) {
		ret = -ENOMEM;
		goto err_put;
	}

	stream->stream = tapi_stream_alloc(tdev);
	if (IS_ERR(stream->stream)) {
		ret = PTR_ERR(stream->stream);
		goto err_free;
	}
	stream->tdev = tdev;

	init_waitqueue_head(&stream->stream->recv_wait);
	skb_queue_head_init(&stream->stream->recv_queue);

	file->private_data = stream;

	return 0;

err_free:
	kfree(stream);
err_put:
	put_device(&tdev->dev);
	return ret;
}

static int tapi_stream_release(struct inode *inode, struct file *file)
{
	struct tapi_stream_file *stream = file->private_data;

	if (stream) {
		tapi_stream_free(stream->tdev, stream->stream);
		put_device(&stream->tdev->dev);
		kfree(stream);
	}

	return 0;
}

static long tapi_stream_ioctl(struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret = 0;
	struct tapi_stream_file *stream = file->private_data;
	struct tapi_device *tdev = stream->tdev;

	switch (cmd) {
	case TAPI_STREAM_IOCTL_GET_ENDPOINT:
		ret = stream->stream->ep.id;
		break;
	case TAPI_STREAM_IOCTL_CONFIGURE:
		break;
	case TAPI_STREAM_IOCTL_START:
		ret = tapi_stream_start(tdev, stream->stream);
		break;
	case TAPI_STREAM_IOCTL_STOP:
		ret = tapi_stream_stop(tdev, stream->stream);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static unsigned int tapi_stream_poll(struct file *file, struct poll_table_struct *wait)
{
	struct tapi_stream_file *stream = file->private_data;
	int ret;

	poll_wait(file, &stream->stream->recv_wait, wait);

	ret = POLLOUT;

	if (!skb_queue_empty(&stream->stream->recv_queue))
		ret |= POLLIN;

	return ret;
}

static ssize_t tapi_stream_read(struct file *file, char __user *buffer,
	size_t count, loff_t *offset)
{
	struct tapi_stream_file *stream = file->private_data;
	struct sk_buff *skb;

	skb = skb_dequeue(&stream->stream->recv_queue);
	if (!skb) {
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		do {
			interruptible_sleep_on(&stream->stream->recv_wait);
			skb = skb_dequeue(&stream->stream->recv_queue);
		} while (skb == NULL && !signal_pending(current));

		if (skb == NULL)
			return -ERESTARTNOHAND;
	}

	if (skb->len > count) {
		skb_queue_head(&stream->stream->recv_queue, skb);
		return -EMSGSIZE;
	}

	if (copy_to_user(buffer, skb->data, skb->len)) {
		skb_queue_head(&stream->stream->recv_queue, skb);
		return -EFAULT;
	}

	count = skb->len;

	kfree_skb(skb);

	return count;
}

static ssize_t tapi_stream_write(struct file *file, const char __user *buffer,
	size_t count, loff_t *ppos)
{
	struct tapi_stream_file *stream = file->private_data;
	struct tapi_device *tdev = stream->tdev;
	struct sk_buff *skb;

	if (count == 0)
		return 0;

	skb = alloc_skb(count, GFP_USER);
	if (!skb)
		return -ENOMEM;

	if (copy_from_user(skb_put(skb, count), buffer, count)) {
		kfree_skb(skb);
		return -EFAULT;
	}

	tdev->ops->stream_send(tdev, stream->stream, skb);

	return count;
}

static const struct file_operations tapi_stream_file_ops = {
	.owner = THIS_MODULE,
	.read = tapi_stream_read,
	.write = tapi_stream_write,
	.open = tapi_stream_open,
	.release = tapi_stream_release,
	.poll = tapi_stream_poll,
	.unlocked_ioctl = tapi_stream_ioctl,
};

int tapi_register_stream_device(struct tapi_device* tdev)
{
	dev_set_name(&tdev->stream_dev.dev, "tapi%uS", tdev->id);
	return tapi_char_device_register(tdev, &tdev->stream_dev, &tapi_stream_file_ops);
}

int tapi_stream_recv(struct tapi_device *tdev, struct tapi_stream * stream,
	struct sk_buff *skb)
{
	skb_queue_tail(&stream->recv_queue, skb);
	wake_up(&stream->recv_wait);

	return 0;
}
EXPORT_SYMBOL_GPL(tapi_stream_recv);
