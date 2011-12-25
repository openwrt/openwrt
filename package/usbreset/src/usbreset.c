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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>


int main(int argc, char **argv)
{
	const char *filename;
	int fd;
	int rc;

	if (argc != 2) {
		fprintf(stderr, "Usage: usbreset device-filename\n");
		return 1;
	}
	filename = argv[1];

	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		perror("Error opening output file");
		return 1;
	}

	printf("Resetting USB device %s\n", filename);
	rc = ioctl(fd, USBDEVFS_RESET, 0);
	if (rc < 0) {
		perror("Error in ioctl");
		return 1;
	}
	printf("Reset successful\n");

	close(fd);
	return 0;
}
