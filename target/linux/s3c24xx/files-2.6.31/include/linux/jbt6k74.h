#ifndef __JBT6K74_H__
#define __JBT6K74_H__

#include <linux/spi/spi.h>

struct jbt6k74_platform_data {
	void (*reset)(int devindex, int level);
	void (*probe_completed)(struct device *dev);
	void (*enable_pixel_clock)(struct device *dev, int enable);
};

#endif
