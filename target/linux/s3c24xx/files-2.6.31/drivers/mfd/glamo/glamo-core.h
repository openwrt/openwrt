#ifndef __GLAMO_CORE_H
#define __GLAMO_CORE_H

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

enum glamo_engine_state {
    GLAMO_ENGINE_DISABLED,
    GLAMO_ENGINE_SUSPENDED,
    GLAMO_ENGINE_ENABLED,
};

struct glamo_core {
	int irq;
	int irq_base;
	int irq_works; /* 0 means PCB does not support Glamo IRQ */
	struct resource *mem;
	void __iomem *base;
	struct platform_device *pdev;
	struct glamo_platform_data *pdata;
	enum glamo_engine_state engine_state[__NUM_GLAMO_ENGINES];
	spinlock_t lock;
};

struct glamo_script {
	uint16_t reg;
	uint16_t val;
};

int glamo_pll_rate(struct glamo_core *glamo, enum glamo_pll pll);

int glamo_engine_enable(struct glamo_core *glamo, enum glamo_engine engine);
int glamo_engine_suspend(struct glamo_core *glamo, enum glamo_engine engine);
int glamo_engine_disable(struct glamo_core *glamo, enum glamo_engine engine);
void glamo_engine_reset(struct glamo_core *glamo, enum glamo_engine engine);
int glamo_engine_reclock(struct glamo_core *glamo,
			 enum glamo_engine engine, int ps);

void glamo_reg_read_batch(struct glamo_core *glamo, uint16_t reg,
				uint16_t count, uint16_t *values);
void glamo_reg_write_batch(struct glamo_core *glamo, uint16_t reg,
				uint16_t count, uint16_t *values);
#endif /* __GLAMO_CORE_H */
