#ifndef __GLAMO_CORE_H
#define __GLAMO_CORE_H

#include <asm/system.h>
#include <linux/glamo-engine.h>

/* for the time being, we put the on-screen framebuffer into the lowest
 * VRAM space.  This should make the code easily compatible with the various
 * 2MB/4MB/8MB variants of the Smedia chips */
#define GLAMO_OFFSET_VRAM	0x800000
#define GLAMO_OFFSET_FB	(GLAMO_OFFSET_VRAM)

/* we only allocate the minimum possible size for the framebuffer to make
 * sure we have sufficient memory for other functions of the chip */
//#define GLAMO_FB_SIZE	(640*480*4)	/* == 0x12c000 */
#define GLAMO_INTERNAL_RAM_SIZE 0x800000
#define GLAMO_MMC_BUFFER_SIZE (64 * 1024)
#define GLAMO_FB_SIZE	(GLAMO_INTERNAL_RAM_SIZE - GLAMO_MMC_BUFFER_SIZE)

struct glamo_core {
	int irq;
	int irq_works; /* 0 means PCB does not support Glamo IRQ */
	struct resource *mem;
	struct resource *mem_core;
	void __iomem *base;
	struct platform_device *pdev;
	struct glamofb_platform_data *pdata;
	u_int16_t type;
	u_int16_t revision;
	spinlock_t lock;
	u32 engine_enabled_bitfield;
	u32 engine_enabled_bitfield_suspend;
	int suspending;
};

struct glamo_script {
	u_int16_t reg;
	u_int16_t val;
};

int glamo_run_script(struct glamo_core *glamo,
		     struct glamo_script *script, int len, int may_sleep);

struct glamo_mci_pdata {
	struct glamo_core * pglamo;
	unsigned int	gpio_detect;
	unsigned int	gpio_wprotect;
	int		(*glamo_can_set_mci_power)(void);
	/* glamo-mci asking if it should use the slow clock to card */
	int		(*glamo_mci_use_slow)(void);
	int		(*glamo_irq_is_wired)(void);
	void		(*mci_suspending)(struct platform_device *dev);
	int		(*mci_all_dependencies_resumed)(struct platform_device *dev);

};

int glamo_engine_enable(struct glamo_core *glamo, enum glamo_engine engine);
int glamo_engine_disable(struct glamo_core *glamo, enum glamo_engine engine);
void glamo_engine_reset(struct glamo_core *glamo, enum glamo_engine engine);
int glamo_engine_reclock(struct glamo_core *glamo,
			 enum glamo_engine engine, int ps);

void glamo_engine_clkreg_set(struct glamo_core *glamo,
			     enum glamo_engine engine,
			     u_int16_t mask, u_int16_t val);

u_int16_t glamo_engine_clkreg_get(struct glamo_core *glamo,
				  enum glamo_engine engine);
#endif /* __GLAMO_CORE_H */
