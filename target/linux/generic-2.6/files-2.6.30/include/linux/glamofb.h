#ifndef _LINUX_GLAMOFB_H
#define _LINUX_GLAMOFB_H

#include <linux/fb.h>
#include <linux/glamo-engine.h>

#ifdef __KERNEL__

#include <linux/spi/glamo.h>

struct glamo_core;

struct glamofb_platform_data {
	int width, height;

	int num_modes;
	struct fb_videomode *modes;

	struct glamo_spigpio_info *spigpio_info;
	struct glamo_core *glamo;

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
void glamo_lcm_reset(struct platform_device *pdev, int level);
#else
#define glamo_lcm_reset(...) do {} while (0)
#endif

#endif

#define GLAMOFB_ENGINE_ENABLE _IOW('F', 0x1, __u32)
#define GLAMOFB_ENGINE_DISABLE _IOW('F', 0x2, __u32)
#define GLAMOFB_ENGINE_RESET _IOW('F', 0x3, __u32)

#endif
