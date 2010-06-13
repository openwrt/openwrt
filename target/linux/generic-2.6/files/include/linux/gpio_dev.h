#ifndef _GPIO_DEV_H__
#define _GPIO_DEV_H__

/*********************************************************************
 *
 * This Linux kernel header is expanded from the original driver
 * (gpio_dev) by John Crispin. It provides an ioctl based interface to
 * GPIO pins via the /dev/gpio char device and gpiolib within the kernel.
 * The third argument to each ioctl is the GPIO pin number.
 *
 * This driver has been tested with lk 2.6.31 and works. The original
 * driver fails quietly with this version. The protocol is now a bit
 * different: the ioctl(fd, GPIO_REQUEST, <pin>) should be called
 * after the open("/dev/gpio", O_RDWR) to determine if the <pin> is
 * already in use. If the ioctl is successful (i.e. returns 0 for not
 * in use) then the <pin> is claimed by this driver and
 * ioctl(fd, GPIO_FREE, <pin>) should be called prior to close(fd) .
 * 
 * See <kernel_source>/Documentation/gpio.txt
 * Note that kernel designers prefer the use of the sysfs gpio interface.
 * This char driver is easier to use from code and faster.
 ********************************************************************/

/* This header can be included in both the user and kernel spaces */
/* The _IO macro is defined in sys/ioctl.h */

#define IOC_GPIODEV_MAGIC  'B'

#define GPIO_GET        _IO(IOC_GPIODEV_MAGIC, 10)
#define GPIO_SET        _IO(IOC_GPIODEV_MAGIC, 11)
#define GPIO_CLEAR      _IO(IOC_GPIODEV_MAGIC, 12)
#define GPIO_DIR_IN     _IO(IOC_GPIODEV_MAGIC, 13)
#define GPIO_DIR_OUT    _IO(IOC_GPIODEV_MAGIC, 14)
        /* Sets the direction out and clears the <pin> (low) */

#define GPIO_DIR_HIGH   _IO(IOC_GPIODEV_MAGIC, 15)
        /* Sets the direction out and sets the <pin> (high) */
#define GPIO_REQUEST    _IO(IOC_GPIODEV_MAGIC, 16)
#define GPIO_FREE       _IO(IOC_GPIODEV_MAGIC, 17)
#define GPIO_CAN_SLEEP  _IO(IOC_GPIODEV_MAGIC, 18)

#endif
