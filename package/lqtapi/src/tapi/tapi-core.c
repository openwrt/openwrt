#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/list.h>

#include <linux/cdev.h>
#include <linux/err.h>

#include <linux/tapi/tapi.h>


void tapi_alloc_input(struct tapi_device *tdev, struct tapi_port *port);
int tapi_register_port_device(struct tapi_device* tdev, struct tapi_port *port);
int tapi_register_stream_device(struct tapi_device* tdev);
int tapi_register_control_device(struct tapi_device* tdev);

static struct class *tapi_class;
static int tapi_major;

#define TAPI_MAX_MINORS 255

static bool tapi_minors[TAPI_MAX_MINORS];

static int tapi_get_free_minor(void)
{
	int i;
	for (i = 0; i < TAPI_MAX_MINORS; ++i) {
		if (!tapi_minors[i]) {
			tapi_minors[i] = true;
			return i;
		}
	}

	return -1;
}

/*
int tapi_port_send_dtmf_events(struct tapi_device *tdev, unsigned int port, struct tapi_dtmf *, size_t num_events, unsigned int dealy)
{
}
EXPORT_SYMBOL_GPL(tapi_port_send_dtmf_events);
*/

void tapi_report_hook_event(struct tapi_device *tdev, struct tapi_port *port,
	bool on)
{
	struct tapi_event event;
	event.type = TAPI_EVENT_TYPE_HOOK;
	event.port = port->id;
	event.hook.on = on;

	tapi_report_event(tdev, &event);
}
EXPORT_SYMBOL_GPL(tapi_report_hook_event);

void tapi_report_dtmf_event(struct tapi_device *tdev, struct tapi_port *port,
	unsigned char code)
{
	struct tapi_event event;
	event.type = TAPI_EVENT_TYPE_DTMF;
	event.port = port->id;
	event.dtmf.code = code;

	tapi_report_event(tdev, &event);
}
EXPORT_SYMBOL_GPL(tapi_report_dtmf_event);

struct tapi_stream *tapi_stream_alloc(struct tapi_device *tdev)
{
	struct tapi_stream *stream;
	printk("tdev %p\n", tdev);

	if (!tdev->ops || !tdev->ops->stream_alloc)
		return ERR_PTR(-ENOSYS);

	stream = tdev->ops->stream_alloc(tdev);
	printk("stream %p\n", stream);
	if (IS_ERR(stream))
		return stream;

	stream->id = atomic_inc_return(&tdev->stream_id) - 1;
	stream->ep.id = stream->id;

/*	mutex_lock(&tdev->lock);*/
	list_add_tail(&stream->head, &tdev->streams);
/*	mutex_unlock(&tdev->lock);*/

	return stream;
}
EXPORT_SYMBOL_GPL(tapi_stream_alloc);

void tapi_stream_free(struct tapi_device *tdev, struct tapi_stream *stream)
{
	mutex_lock(&tdev->lock);
	list_del(&stream->head);
	mutex_unlock(&tdev->lock);

	tdev->ops->stream_free(tdev, stream);
}
EXPORT_SYMBOL_GPL(tapi_stream_free);

struct tapi_link *tapi_link_alloc(struct tapi_device *tdev,
	struct tapi_endpoint *ep1, struct tapi_endpoint *ep2)
{
	struct tapi_link *link;

	if (!tdev->ops || !tdev->ops->link_alloc)
		return ERR_PTR(-ENOSYS);

	link = tdev->ops->link_alloc(tdev, ep1, ep2);
	if (IS_ERR(link))
		return link;

	link->id = atomic_inc_return(&tdev->link_id) - 1;

/*
	mutex_lock(&tdev->lock);
	list_add_tail(&link->head, &tdev->links);
	mutex_unlock(&tdev->lock);
*/
	return link;
}
EXPORT_SYMBOL_GPL(tapi_link_alloc);

void tapi_link_free(struct tapi_device *tdev, struct tapi_link *link)
{
/*
	mutex_lock(&tdev->lock);
	list_del(&link->head);
	mutex_unlock(&tdev->lock);
*/
	tdev->ops->link_free(tdev, link);
}
EXPORT_SYMBOL_GPL(tapi_link_free);

int tapi_char_device_register(struct tapi_device *tdev,
	struct tapi_char_device *tchrdev, const struct file_operations *fops)
{
	int ret;
	struct device *dev = &tchrdev->dev;
	dev_t devt;
	int minor = tapi_get_free_minor();

	devt = MKDEV(tapi_major, minor);

	dev->devt = devt;
	dev->class = tapi_class;
	dev->parent = &tdev->dev;

	tchrdev->tdev = tdev;

	cdev_init(&tchrdev->cdev, fops);
	tchrdev->cdev.owner = THIS_MODULE;
	ret = cdev_add(&tchrdev->cdev, devt, 1);
	if (ret)
		return ret;

	ret = device_register(&tchrdev->dev);
	if (ret)
		goto err_cdev_del;

	return 0;

err_cdev_del:
	cdev_del(&tchrdev->cdev);

	return ret;
}

int tapi_device_register(struct tapi_device *tdev, const char *name,
	struct device *parent)
{
	static atomic_t tapi_device_id = ATOMIC_INIT(0);
	int ret, i;

	tdev->dev.class = tapi_class;
	tdev->dev.parent = parent;
	dev_set_name(&tdev->dev, "%s", name);

	ret = device_register(&tdev->dev);
	if (ret)
		return ret;

	tdev->id = atomic_inc_return(&tapi_device_id) - 1;

	mutex_init(&tdev->lock);
	INIT_LIST_HEAD(&tdev->streams);
	INIT_LIST_HEAD(&tdev->links);
	atomic_set(&tdev->link_id, 0);
	atomic_set(&tdev->stream_id, tdev->num_ports);

	tapi_register_stream_device(tdev);
	tapi_register_control_device(tdev);

	for (i = 0; i < tdev->num_ports; ++i) {
		tapi_port_alloc(tdev, i);
		tapi_alloc_input(tdev, &tdev->ports[i]);
		tapi_register_port_device(tdev, &tdev->ports[i]);
		tdev->ports[i].id = i;
		tdev->ports[i].ep.id = i;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(tapi_device_register);

void tapi_device_unregister(struct tapi_device *tdev)
{
	device_unregister(&tdev->dev);
}
EXPORT_SYMBOL_GPL(tapi_device_unregister);

static int __init tapi_class_init(void)
{
	int ret;
	dev_t dev;

	tapi_class = class_create(THIS_MODULE, "tapi");

	if (IS_ERR(tapi_class)) {
		ret = PTR_ERR(tapi_class);
		printk(KERN_ERR "tapi: Failed to create device class: %d\n", ret);
		goto err;
	}

	ret = alloc_chrdev_region(&dev, 0, TAPI_MAX_MINORS, "tapi");
	if (ret) {
		printk(KERN_ERR "tapi: Failed to allocate chrdev region: %d\n", ret);
		goto err_class_destory;
	}
	tapi_major = MAJOR(dev);

	return 0;
err_class_destory:
	class_destroy(tapi_class);
err:
	return ret;
}
subsys_initcall(tapi_class_init);

static void __exit tapi_class_exit(void)
{
	unregister_chrdev_region(MKDEV(tapi_major, 0), TAPI_MAX_MINORS);
	class_destroy(tapi_class);
}
module_exit(tapi_class_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("TAPI class");
MODULE_LICENSE("GPL");
