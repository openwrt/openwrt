/* usbreset -- send a USB port reset to a USB device */

/*

http://marc.info/?l=linux-usb-users&m=116827193506484&w=2

and needs mounted usbfs filesystem

	sudo mount -t usbfs none /proc/bus/usb

There is a way to suspend a USB device.  In order to use it,
you must have a kernel with CONFIG_PM_SYSFS_DEPRECATED turned on.  To
suspend a device, do (as root):

	echo -n 2 >/sys/bus/usb/devices/.../power/state

where the "..." is the ID for your device.  To unsuspend, do the same
thing but with a "0" instead of the "2" above.

Note that this mechanism is slated to be removed from the kernel within
the next year.  Hopefully some other mechanism will take its place.

> To reset a
> device?

Here's a program to do it.  You invoke it as either

	usbreset /proc/bus/usb/BBB/DDD
or
	usbreset /dev/usbB.D

depending on how your system is set up, where BBB and DDD are the bus and
device address numbers.

Alan Stern

*/

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>


static char *usbfs = NULL;

struct usbentry {
	int bus_num;
	int dev_num;
	int vendor_id;
	int product_id;
	char vendor_name[128];
	char product_name[128];
};


static bool find_usbfs(void)
{
	FILE *mtab;

	char buf[1024], type[32];
	static char path[1024];

	if ((mtab = fopen("/proc/mounts", "r")) != NULL)
	{
		while (fgets(buf, sizeof(buf), mtab))
		{
			if (sscanf(buf, "%*s %1023s %31s ", path, type) == 2 &&
				!strncmp(type, "usbfs", 5))
			{
				usbfs = path;
				break;
			}
		}

		fclose(mtab);
	}

	return !!usbfs;
}

static FILE * open_devlist(void)
{
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s/devices", usbfs);
	return fopen(buf, "r");
}

static void close_devlist(FILE *devs)
{
	fclose(devs);
}

static struct usbentry * parse_devlist(FILE *devs)
{
	char buf[1024];
	static struct usbentry dev;

	memset(&dev, 0, sizeof(dev));

	while (fgets(buf, sizeof(buf), devs))
	{
		buf[strlen(buf)-1] = 0;

		switch (buf[0])
		{
		case 'T':
			sscanf(buf, "T: Bus=%d Lev=%*d Prnt=%*d Port=%*d Cnt=%*d Dev#=%d",
				   &dev.bus_num, &dev.dev_num);
			break;

		case 'P':
			sscanf(buf, "P: Vendor=%x ProdID=%x",
				   &dev.vendor_id, &dev.product_id);
			break;

		case 'S':
			if (!strncmp(buf, "S:  Manufacturer=", 17))
				snprintf(dev.vendor_name, sizeof(dev.vendor_name),
				         "%s", buf+17);
			else if (!strncmp(buf, "S:  Product=", 12))
				snprintf(dev.product_name, sizeof(dev.product_name),
				         "%s", buf+12);
			break;
		}

		if (dev.product_name[0])
			return &dev;
	}

	return NULL;
}

static void list_devices(void)
{
	FILE *devs = open_devlist();
	struct usbentry *dev;

	if (!devs)
		return;

	while ((dev = parse_devlist(devs)) != NULL)
	{
		printf("  Number %03d/%03d  ID %04x:%04x  %s\n",
			   dev->bus_num, dev->dev_num,
			   dev->vendor_id, dev->product_id,
			   dev->product_name);
	}

	close_devlist(devs);
}

struct usbentry * find_device(int *bus, int *dev,
                              int *vid, int *pid,
                              const char *product)
{
	FILE *devs = open_devlist();

	struct usbentry *e, *match = NULL;

	if (!devs)
		return NULL;

	while ((e = parse_devlist(devs)) != NULL)
	{
		if ((bus && (e->bus_num == *bus) && (e->dev_num == *dev)) ||
			(vid && (e->vendor_id == *vid) && (e->product_id == *pid)) ||
			(product && !strcasecmp(e->product_name, product)))
		{
			match = e;
			break;
		}
	}

	close_devlist(devs);

	return match;
}

static void reset_device(struct usbentry *dev)
{
	int fd;
	char path[1024];

	snprintf(path, sizeof(path), "%s/%03d/%03d",
			 usbfs, dev->bus_num, dev->dev_num);

	printf("Resetting %s ... ", dev->product_name);

	if ((fd = open(path, O_WRONLY)) > -1)
	{
		if (ioctl(fd, USBDEVFS_RESET, 0) < 0)
			printf("failed [%s]\n", strerror(errno));
		else
			printf("ok\n");

		close(fd);
	}
	else
	{
		printf("can't open [%s]\n", strerror(errno));
	}
}


int main(int argc, char **argv)
{
	int id1, id2;
	struct usbentry *dev;

	if (!find_usbfs())
	{
		fprintf(stderr, "Unable to find usbfs, is it mounted?\n");
		return 1;
	}

	if ((argc == 2) && (sscanf(argv[1], "%3d/%3d", &id1, &id2) == 2))
	{
		dev = find_device(&id1, &id2, NULL, NULL, NULL);
	}
	else if ((argc == 2) && (sscanf(argv[1], "%4x:%4x", &id1, &id2) == 2))
	{
		dev = find_device(NULL, NULL, &id1, &id2, NULL);
	}
	else if ((argc == 2) && strlen(argv[1]) < 128)
	{
		dev = find_device(NULL, NULL, NULL, NULL, argv[1]);
	}
	else
	{
		printf("Usage:\n"
		       "  usbreset PPPP:VVVV - reset by product and vendor id\n"
		       "  usbreset BBB/DDD   - reset by bus and device number\n"
		       "  usbreset \"Product\" - reset by product name\n\n"
		       "Devices:\n");
		list_devices();
		return 1;
	}

	if (!dev)
	{
		fprintf(stderr, "No such device found\n");
		return 1;
	}

	reset_device(dev);
	return 0;
}
