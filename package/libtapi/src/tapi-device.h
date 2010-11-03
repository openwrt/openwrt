#ifndef __TAPI_DEVICE_H__
#define __TAPI_DEVICE_H__

struct tapi_device {
	int control_fd;
	int stream_fd;
	struct tapi_port *ports;
	char stream_path[100];

	unsigned int id;

	unsigned int num_ports;
};

struct tapi_endpoint;

int tapi_device_open(unsigned int id, struct tapi_device *dev);

int tapi_link_alloc(struct tapi_device *dev, unsigned int ep1, unsigned int ep2);
int tapi_link_free(struct tapi_device *dev, unsigned int link);
int tapi_link_enable(struct tapi_device *dev, unsigned int link);
int tapi_link_disable(struct tapi_device *dev, unsigned int link);
int tapi_sync(struct tapi_device *dev);

#endif
