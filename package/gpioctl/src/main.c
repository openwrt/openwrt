/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA02111-1307USA
*
* Feedback, Bugs...  blogic@openwrt.org 
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/gpio_dev.h>
#include <linux/ioctl.h>

void
print_usage()
{
	printf("gpioctl dirin|dirout|get|set|clear gpio\n");
	exit(0);
}

int
main(int argc, char **argv)
{
	int gpio_pin;
	int fd;
	int result = 0;

	if (argc != 3)
	{
		print_usage();
	}

	if ((fd = open("/dev/gpio", O_RDWR)) < 0)
	{
        printf("Error whilst opening /dev/gpio\n");
        return -1;
	}

	gpio_pin = atoi(argv[2]);

	printf("using gpio pin %d\n", gpio_pin);

	if (!strcmp(argv[1], "dirin"))
	{
		ioctl(fd, GPIO_DIR_IN, gpio_pin);
	} else if (!strcmp(argv[1], "dirout"))
	{
		ioctl(fd, GPIO_DIR_OUT, gpio_pin);
	} else if (!strcmp(argv[1], "get"))
	{
		result = ioctl(fd, GPIO_GET, gpio_pin);
		printf("Pin %d is %s\n", gpio_pin, (result ? "HIGH" : "LOW"));
	} else if (!strcmp(argv[1], "set"))
	{
		ioctl(fd, GPIO_SET, gpio_pin);
	} else if (!strcmp(argv[1], "clear"))
	{
		ioctl(fd, GPIO_CLEAR, gpio_pin);
	} else print_usage();

	return result;
}
