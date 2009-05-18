#ifndef __JBT6K74_H__
#define __JBT6K74_H__

#include <linux/spi/spi.h>

struct jbt6k74_platform_data {
	void (*reset)(int devindex, int level);
	void (*resuming)(int devindex); /* called when LCM is resumed */
	void (*probe_completed)(struct device *dev);
};

#endif
