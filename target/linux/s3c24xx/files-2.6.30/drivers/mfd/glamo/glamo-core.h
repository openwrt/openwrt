#ifndef __GLAMO_CORE_H
#define __GLAMO_CORE_H

#include <asm/system.h>
#include <linux/mfd/glamo.h>

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

enum glamo_pll {
    GLAMO_PLL1,
    GLAMO_PLL2,
};

struct glamo_core {
	int irq;
	int irq_works; /* 0 means PCB does not support Glamo IRQ */
	struct resource *mem;
	struct resource *mem_core;
	void __iomem *base;
	struct platform_device *pdev;
	struct glamo_platform_data *pdata;
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

void glamo_engine_div_enable(struct glamo_core *glamo, enum glamo_engine engine);
void glamo_engine_div_disable(struct glamo_core *glamo, enum glamo_engine engine);


int glamo_pll_rate(struct glamo_core *glamo, enum glamo_pll pll);

int glamo_run_script(struct glamo_core *glamo,
		     const struct glamo_script *script, int len, int may_sleep);

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
