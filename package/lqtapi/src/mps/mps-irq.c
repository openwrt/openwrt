#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <ifxmips_irq.h>

#include "mps.h"
#include "mps-irq.h"

#define MPS_REG_AD0_IRQ_BASE	0x40
#define MPS_REG_AD1_IRQ_BASE	0x44

#define MPS_REG_AD_IRQ_STATUS	0x00
#define MPS_REG_AD_IRQ_SET	0x08
#define MPS_REG_AD_IRQ_CLEAR	0x10
#define MPS_REG_AD_IRQ_ENABLE	0x18

struct mps_irq_desc
{
    void __iomem *base;
    unsigned int irq_base;
};

static inline unsigned int mps_irq_bit(struct mps_irq_desc *mps_desc, int irq)
{
	return BIT(irq - mps_desc->irq_base);
}

static void mps_irq_ack(unsigned int irq)
{
	struct mps_irq_desc *mps_desc = get_irq_chip_data(irq);

	__raw_writel(mps_irq_bit(mps_desc, irq),
		mps_desc->base + MPS_REG_AD_IRQ_CLEAR);
}

static void mps_irq_mask(unsigned int irq)
{
	struct mps_irq_desc *mps_desc = get_irq_chip_data(irq);
	uint32_t mask;

	mask = __raw_readl(mps_desc->base + MPS_REG_AD_IRQ_ENABLE);
	mask &= ~mps_irq_bit(mps_desc, irq);
	__raw_writel(mask, mps_desc->base + MPS_REG_AD_IRQ_ENABLE);
}

static void mps_irq_unmask(unsigned int irq)
{
	struct mps_irq_desc *mps_desc = get_irq_chip_data(irq);
	uint32_t mask;

	mask = __raw_readl(mps_desc->base + MPS_REG_AD_IRQ_ENABLE);
	mask |= mps_irq_bit(mps_desc, irq) | 0xffff;
	__raw_writel(mask, mps_desc->base + MPS_REG_AD_IRQ_ENABLE);
}

static struct irq_chip mps_irq_chip = {
	.name = "mps",
	.ack = mps_irq_ack,
	.mask = mps_irq_mask,
	.unmask = mps_irq_unmask,
};

static void mps_irq_demux_handler(unsigned int irq, struct irq_desc *desc)
{
	struct mps_irq_desc *mps_desc = get_irq_data(irq);
	uint32_t val;
	int mps_irq;

	desc->chip->mask(irq);

	val = __raw_readl(mps_desc->base + MPS_REG_AD_IRQ_STATUS);
	mps_irq = ffs(val);

/*	printk("irq: %d %x\n", mps_irq, val);*/

	if (mps_irq > 16)
	    printk("PANIC!\n");

	if (mps_irq)
		generic_handle_irq(mps_irq + mps_desc->irq_base - 1);

	desc->chip->ack(irq);
	desc->chip->unmask(irq);
}

#if 0

static const uint32_t ring_msg[] = {
0x01010004, 0x00030000,
};

static irqreturn_t mps_irq_ad0(int irq, void *devid)
{
    struct mps *mps = devid;
    uint32_t val;

    val = __raw_readl(mps->base + MPS_REG_AD0_IRQ_STATUS);
    printk("WOHO ein IRQ: %x\n", val);
    __raw_writel(val, mps->base + MPS_REG_AD0_IRQ_CLEAR);

	if (val & BIT(MPS_IRQ_DOWNLOAD_DONE))
		complete(&mps->init_completion);

	if (val & BIT(MPS_IRQ_EVENT))
		mps_fifo_in(&mps->mbox_cmd.downstream, ring_msg, ARRAY_SIZE(ring_msg));

    return IRQ_HANDLED;
}
#endif

#define MPS_NUM_AD_IRQS 32

struct mps_irq_desc mps_irq_descs[2];

int mps_irq_init(struct mps *mps)
{
	int ret = 0;
	int irq;

	mps_irq_descs[0].base = mps->base + MPS_REG_AD0_IRQ_BASE;
	mps_irq_descs[0].irq_base = mps->irq_base;
	mps_irq_descs[1].base = mps->base + MPS_REG_AD1_IRQ_BASE;
	mps_irq_descs[1].irq_base = mps->irq_base + 16;


	set_irq_data(mps->irq_ad0, &mps_irq_descs[0]);
	set_irq_chained_handler(mps->irq_ad0, mps_irq_demux_handler);
	set_irq_data(mps->irq_ad1, &mps_irq_descs[1]);
	set_irq_chained_handler(mps->irq_ad1, mps_irq_demux_handler);

/*
	ret = request_irq(mps->irq_ad0, mps_irq_demux_handler, IRQF_DISABLED,
		"mps ad0", &mps_irq_descs[0]);
	ret = request_irq(mps->irq_ad1, mps_irq_demux_handler, IRQF_DISABLED,
		"mps ad0", &mps_irq_descs[1]);
*/
	for (irq = 0; irq < MPS_NUM_AD_IRQS; ++irq) {
	    set_irq_chip_data(irq + mps->irq_base, &mps_irq_descs[irq / 16]);
	    set_irq_chip_and_handler(irq + mps->irq_base, &mps_irq_chip, handle_level_irq);
	}


/*
	res = request_irq(INT_NUM_IM4_IRL18, mps_irq_ad0, IRQF_DISABLED,
	    "mps ad0", mps);

	irqs = BIT(MPS_IRQ_CMD_UPSTREAM) | BIT(MPS_IRQ_DATA_UPSTREAM)
	 | BIT(MPS_IRQ_DOWNLOAD_DONE) | BIT(MPS_IRQ_EVENT) | BIT(MPS_IRQ_CMD_ERROR); 

	 __raw_writel(irqs, mps->base + MPS_REG_AD0_IRQ_ENA);
*/
	return ret;
}

void mps_irq_exit(struct mps *mps)
{
	free_irq(INT_NUM_IM4_IRL18, mps);
}
