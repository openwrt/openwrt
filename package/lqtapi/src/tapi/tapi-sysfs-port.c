#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>

#include <linux/err.h>
#include <linux/tapi/tapi.h>

struct tapi_sysfs_port {
	struct tapi_device *tdev;
	unsigned int id;
	struct kobject kobj;
};

struct tapi_sysfs_entry {
	ssize_t (*show)(struct tapi_device *, unsigned int port, char *);
	ssize_t (*store)(struct tapi_device *, unsigned int port, const char *, size_t);
	struct attribute attr;
};

static ssize_t tapi_port_store(struct kobject *kobj, struct attribute *attr,
	const char *s, size_t len)
{
	struct tapi_sysfs_port *port = container_of(kobj, struct tapi_sysfs_port, kobj);
	struct tapi_sysfs_entry *entry = container_of(attr, struct tapi_sysfs_entry,
								attr);

	if (!entry->store)
		return -ENOSYS;

	return entry->store(port->tdev, port->id, s, len);
}

static ssize_t tapi_port_show(struct kobject *kobj, struct attribute *attr,
	char *s)
{
	return -ENOSYS;
}

#define TAPI_PORT_ATTR(_name, _mode, _show, _store) \
	struct tapi_sysfs_entry tapi_port_ ## _name ## _attr = \
		__ATTR(_name, _mode, _show, _store)

static int tapi_port_store_ring(struct tapi_device *tdev, unsigned int port,
	const char *s, size_t len)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(s, 10, &val);

	if (ret)
		return ret;

	ret = tapi_port_set_ring(tdev, &tdev->ports[port], val);
	if (ret)
		return ret;
	return len;
}

static TAPI_PORT_ATTR(ring, 0644, NULL, tapi_port_store_ring);

static struct attribute *tapi_port_default_attrs[] = {
	&tapi_port_ring_attr.attr,
	NULL,
};

static void tapi_port_free(struct kobject *kobj)
{
	struct tapi_sysfs_port *port = container_of(kobj, struct tapi_sysfs_port, kobj);
	kfree(port);
}

static struct sysfs_ops tapi_port_sysfs_ops = {
	.show		= tapi_port_show,
	.store		= tapi_port_store,
};

static struct kobj_type tapi_port_ktype = {
	.release	= tapi_port_free,
	.sysfs_ops	= &tapi_port_sysfs_ops,
	.default_attrs	= tapi_port_default_attrs,
};

struct tapi_sysfs_port *tapi_port_alloc(struct tapi_device *tdev, unsigned int id)
{
	struct tapi_sysfs_port *port;
	int ret;

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	port->tdev = tdev;
	port->id = id;

	ret = kobject_init_and_add(&port->kobj, &tapi_port_ktype, &tdev->dev.kobj,
		"port%d", id);
	if (ret) {
		kfree(port);
		return ERR_PTR(ret);
	}

	return port;
}

void tapi_port_delete(struct tapi_sysfs_port *port)
{
	kobject_del(&port->kobj);
	kobject_put(&port->kobj);
}
