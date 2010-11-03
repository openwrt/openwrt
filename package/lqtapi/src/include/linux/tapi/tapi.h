#ifndef __LINUX_TAPI_H__
#define __LINUX_TAPI_H__

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/mutex.h>

#include <linux/input.h>

#include <asm/atomic.h>
#include <linux/list.h>

#include <linux/cdev.h>

#include <linux/skbuff.h>
#include <linux/wait.h>

#include <linux/tapi/tapi-event.h>

struct tapi_device;

struct tapi_char_device {
	struct tapi_device *tdev;
	struct device dev;
	struct cdev cdev;
};

static inline struct tapi_char_device *cdev_to_tapi_char_device(struct cdev *cdev)
{
	return container_of(cdev, struct tapi_char_device, cdev);
}

int tapi_char_device_register(struct tapi_device *tdev,
	struct tapi_char_device *tchrdev, const struct file_operations *fops);


struct tapi_endpoint {
	unsigned int id;
	void *data;
};

static inline void tapi_endpoint_set_data(struct tapi_endpoint *ep, void *data)
{
	ep->data = data;
}

static inline void *tapi_endpoint_get_data(struct tapi_endpoint *ep)
{
	return ep->data;
}

struct tapi_port {
	unsigned int id;
	struct tapi_endpoint ep;
	struct input_dev *input;
	struct tapi_char_device chrdev;
};

struct tapi_stream {
	unsigned int id;
	struct list_head head;
	struct tapi_endpoint ep;

	struct sk_buff_head recv_queue;
	wait_queue_head_t recv_wait;
	struct sk_buff_head send_queue;
};

struct tapi_link {
	unsigned int id;
	struct list_head head;
};

enum tapi_codec {
	TAPI_CODEC_L16,
};

struct tapi_stream_config {
	enum tapi_codec codec;
	unsigned int buffer_size;
};

struct tapi_ops {
	int (*send_dtmf_events)(struct tapi_device *, struct tapi_port *port,
		struct tapi_dtmf_event *, size_t num_events, unsigned int dealy);
	int (*send_dtmf_event)(struct tapi_device *, struct tapi_port *port,
		struct tapi_dtmf_event *);
	int (*ring)(struct tapi_device *, struct tapi_port *port, bool ring);

	struct tapi_stream *(*stream_alloc)(struct tapi_device *);
	void (*stream_free)(struct tapi_device *, struct tapi_stream *);
	int (*stream_configure)(struct tapi_device *, struct tapi_stream *,
		struct tapi_stream_config *);
	int (*stream_start)(struct tapi_device *, struct tapi_stream *);
	int (*stream_stop)(struct tapi_device *, struct tapi_stream *);
	int (*stream_send)(struct tapi_device *, struct tapi_stream *,
		struct sk_buff *);

	struct tapi_link *(*link_alloc)(struct tapi_device *,
		struct tapi_endpoint *ep1, struct tapi_endpoint *ep2);
	void (*link_free)(struct tapi_device *, struct tapi_link *);
	int (*link_enable)(struct tapi_device *, struct tapi_link *);
	int (*link_disable)(struct tapi_device *, struct tapi_link *);

	int (*sync)(struct tapi_device *);
};

int tapi_stream_recv(struct tapi_device *, struct tapi_stream *, struct sk_buff *);

struct tapi_device {
	unsigned int id;

	const struct tapi_ops *ops;
	unsigned int num_ports;

	struct device dev;

	struct mutex lock;

	struct tapi_port *ports;
	struct list_head streams;
	struct list_head links;
	atomic_t stream_id;
	atomic_t link_id;

	struct tapi_char_device stream_dev;
	struct tapi_char_device control_dev;
};

static inline struct tapi_device *dev_to_tapi(struct device *dev)
{
	return container_of(dev, struct tapi_device, dev);
}

static inline struct tapi_stream *tapi_stream_from_id(struct tapi_device *tdev,
	unsigned int id)
{
	struct tapi_stream *stream;

	mutex_lock(&tdev->lock);

	list_for_each_entry(stream, &tdev->streams, head) {
		if (stream->id == id)
			goto out;
	}
	stream = NULL;

out:
	mutex_unlock(&tdev->lock);
	return stream;
}

struct tapi_link *tapi_link_alloc(struct tapi_device *, struct tapi_endpoint *,
	struct tapi_endpoint *);
void tapi_link_free(struct tapi_device *, struct tapi_link *);

struct tapi_stream *tapi_stream_alloc(struct tapi_device *tdev);
void tapi_stream_free(struct tapi_device *tdev, struct tapi_stream *stream);

static inline int tapi_sync(struct tapi_device *tdev)
{
	if (!tdev->ops || !tdev->ops->sync)
		return 0;

	return tdev->ops->sync(tdev);
}

static inline int tapi_link_enable(struct tapi_device *tdev,
	struct tapi_link *link)
{
	if (!tdev->ops || !tdev->ops->link_enable)
		return 0;

	return tdev->ops->link_enable(tdev, link);
}

static inline int tapi_link_disable(struct tapi_device *tdev,
	struct tapi_link *link)
{
	if (!tdev->ops || !tdev->ops->link_disable)
		return 0;

	return tdev->ops->link_disable(tdev, link);
}

static inline int tapi_port_send_dtmf(struct tapi_device *tdev,
	struct tapi_port *port,	struct tapi_dtmf_event *dtmf)
{
	if (!tdev->ops || !tdev->ops->send_dtmf_event)
		return -ENOSYS;

	return tdev->ops->send_dtmf_event(tdev, port, dtmf);
}

static inline int tapi_port_set_ring(struct tapi_device *tdev,
	struct tapi_port *port, bool ring)
{
	if (!tdev->ops || !tdev->ops->ring)
		return -ENOSYS;

	return tdev->ops->ring(tdev, port, ring);
}

static inline int tapi_stream_start(struct tapi_device *tdev,
struct tapi_stream *stream)
{
	if (!tdev->ops || !tdev->ops->stream_start)
		return -ENOSYS;

	return tdev->ops->stream_start(tdev, stream);
}

static inline int tapi_stream_stop(struct tapi_device *tdev,
struct tapi_stream *stream)
{
	if (!tdev->ops || !tdev->ops->stream_stop)
		return -ENOSYS;

	return tdev->ops->stream_stop(tdev, stream);
}

int tapi_device_register(struct tapi_device *tdev, const char *name,
	struct device *parent);
void tapi_device_unregister(struct tapi_device *tdev);

struct tapi_sysfs_port;

struct tapi_sysfs_port *tapi_port_alloc(struct tapi_device *tdev, unsigned int id);
void tapi_port_delete(struct tapi_sysfs_port *);

#endif
