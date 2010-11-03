
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#include <linux/input.h>
/*#include <sys/timerfd.h>*/
#include "timerfd.h"
#include "dialdetector.h"

#include "tapi-ioctl.h"

#include "tapi-device.h"
#include "tapi-port.h"

static struct tapi_device dev;
static struct tapi_port ports[2];

void dial_callback(struct tapi_port *port, size_t num_digits, const unsigned char *digits)
{
	unsigned int link;

	if (num_digits != 1)
		return;
	if (port->id == digits[0] || digits[0] > 1)
		return;

	tapi_port_set_ring(&ports[digits[0]], true);

	tapi_session_alloc(&dev, port, &ports[digits[0]]);
}

int main(int argc, char *argv[])
{
	struct dialdetector *dd, *dd2;
	unsigned int link;
	unsigned char buf[1024];
	int ret;

	tapi_device_open(0, &dev);
	tapi_port_open(0, 0, &ports[0]);
	tapi_port_open(0, 1, &ports[1]);

	dd = dialdetector_alloc(&ports[0]);
	dd->dial_callback = dial_callback;
	dd2 = dialdetector_alloc(&ports[1]);
	dd2->dial_callback = dial_callback;

	tapi_mainloop();

	return 0;
}
