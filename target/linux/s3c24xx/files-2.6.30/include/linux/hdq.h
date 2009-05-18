#ifndef __LINUX_HDQ_H__
#define __LINUX_HDQ_H__

#include <linux/device.h>

#define HDQ_SAMPLE_PERIOD_US	10

/* platform data */

struct hdq_platform_data {
	/*
	 * give an opportunity to use us as parent for
	 * devices that depend on us
	 */
	void (*attach_child_devices)(struct device *parent_device);
	
	void (*gpio_dir_out)(void);
	void (*gpio_dir_in)(void);
	void (*gpio_set)(int);
	int (*gpio_get)(void);

	int (*enable_fiq)(void);
	void (*disable_fiq)(void);
	void (*kick_fiq)(void);

};

int hdq_read(int address);
int hdq_write(int address, u8 data);
int hdq_initialized(void);

#endif
