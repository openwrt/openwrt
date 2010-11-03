#include <stdio.h>

#include "tapi-device.h"
#include "tapi-ioctl.h"

int tapi_device_open(unsigned int id, struct tapi_device *dev)
{
	char path[100];

	snprintf(path, sizeof(path), "/dev/tapi%dC", id);
	dev->control_fd = open(path, 0);
	if (dev->control_fd < 0)
		return -1;

	snprintf(dev->stream_path, 100, "/dev/tapi%dS", id);

	dev->id = id;
	dev->num_ports = 2;

	return 0;
}

int tapi_link_alloc(struct tapi_device *dev, unsigned int ep1, unsigned int ep2)
{
	return ioctl(dev->control_fd, TAPI_CONTROL_IOCTL_LINK_ALLOC, (ep1 << 16) | ep2);
}

int tapi_link_free(struct tapi_device *dev, unsigned int link)
{
	return ioctl(dev->control_fd, TAPI_CONTROL_IOCTL_LINK_FREE, link);
}

int tapi_link_enable(struct tapi_device *dev, unsigned int link)
{
	return ioctl(dev->control_fd, TAPI_CONTROL_IOCTL_LINK_ENABLE, link);
}

int tapi_link_disable(struct tapi_device *dev, unsigned int link)
{
	return ioctl(dev->control_fd, TAPI_CONTROL_IOCTL_LINK_DISABLE, link);
}

int tapi_sync(struct tapi_device *dev)
{
	return ioctl(dev->control_fd, TAPI_CONTROL_IOCTL_SYNC, 0);
}
