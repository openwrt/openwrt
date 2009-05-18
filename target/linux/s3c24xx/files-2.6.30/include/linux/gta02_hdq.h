#ifndef __GTA02HDQ_H__
#define __GTA02HDQ_H__

/* platform data */

struct gta02_hdq_platform_data {
	/*
	 * give an opportunity to use us as parent for
	 * devices that depend on us
	 */
	void (*attach_child_devices)(struct device *parent_device);
};

int gta02hdq_read(int address);
int gta02hdq_write(int address, u8 data);
int gta02hdq_initialized(void);

#endif
