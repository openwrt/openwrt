#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/tapi/tapi.h>
#include <linux/tapi/tapi-ioctl.h>

static inline struct tapi_port *tapi_char_device_to_port(struct tapi_char_device *chrdev)
{
	return container_of(chrdev, struct tapi_port, chrdev);
}

static int tapi_port_open(struct inode *inode, struct file *file)
{
	struct tapi_device *tdev = cdev_to_tapi_char_device(inode->i_cdev)->tdev;

	get_device(&tdev->dev);
	file->private_data = cdev_to_tapi_char_device(inode->i_cdev);

	return 0;
}

static int tapi_port_release(struct inode *inode, struct file *file)
{
	struct tapi_device *tdev = cdev_to_tapi_char_device(inode->i_cdev)->tdev;

	put_device(&tdev->dev);

	return 0;
}

static long tapi_port_ioctl_get_endpoint(struct tapi_device *tdev,
	struct tapi_port *port, unsigned long arg)
{
	return port->ep.id;
}

static long tapi_port_ioctl_set_ring(struct tapi_device *tdev,
	struct tapi_port *port, unsigned long arg)
{
	tapi_port_set_ring(tdev, port, arg);
	return 0;
}

static long tapi_port_ioctl(struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret;
	struct tapi_char_device *tchrdev = file->private_data;
	struct tapi_device *tdev = tchrdev->tdev;
	struct tapi_port *port = tapi_char_device_to_port(tchrdev);

	switch (cmd) {
	case TAPI_PORT_IOCTL_GET_ENDPOINT:
		ret = tapi_port_ioctl_get_endpoint(tdev, port, arg);
		break;
	case TAPI_PORT_IOCTL_SET_RING:
		ret = tapi_port_ioctl_set_ring(tdev, port, arg);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static const struct file_operations tapi_port_file_ops = {
	.owner = THIS_MODULE,
	.open = tapi_port_open,
	.release = tapi_port_release,
	.unlocked_ioctl = tapi_port_ioctl,
};

int tapi_register_port_device(struct tapi_device* tdev, struct tapi_port *port)
{
	dev_set_name(&port->chrdev.dev, "tapi%uP%u", tdev->id, port->id);
	return tapi_char_device_register(tdev, &port->chrdev, &tapi_port_file_ops);
}
