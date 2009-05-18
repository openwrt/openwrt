#ifndef _LINUX_GLAMOFB_H
#define _LINUX_GLAMOFB_H

#include <linux/spi/glamo.h>

struct glamofb_val {
	unsigned int defval;
	unsigned int min;
	unsigned int max;
};

struct glamo_core;

struct glamofb_platform_data {
	int width, height;
	int pixclock;
	int left_margin, right_margin;
	int upper_margin, lower_margin;
	int hsync_len, vsync_len;
	int fb_mem_size;

	struct glamofb_val xres;
	struct glamofb_val yres;
	struct glamofb_val bpp;

	struct glamo_spi_info *spi_info;
	struct glamo_spigpio_info *spigpio_info;
	struct glamo_core *glamo;

	struct platform_device *mmc_dev;

	/* glamo mmc platform specific info */
	int		(*glamo_can_set_mci_power)(void);

	/* glamo-mci asking if it should use the slow clock to card */
	int		(*glamo_mci_use_slow)(void);
	int		(*glamo_irq_is_wired)(void);
	void		(*glamo_external_reset)(int);
};

int glamofb_cmd_mode(struct glamofb_handle *gfb, int on);
int glamofb_cmd_write(struct glamofb_handle *gfb, u_int16_t val);

#ifdef CONFIG_MFD_GLAMO
void glamo_lcm_reset(int level);
#else
#define glamo_lcm_reset(...) do {} while (0)
#endif

#endif
