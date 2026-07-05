// SPDX-License-Identifier: GPL-2.0-only

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/types.h>

#include <soc/bcm/bcm3380-msp.h>

#define MSP_SMISB_CTRL			0xff400030
#define MSP_SMISB_CTRL_ENABLE		0x18000007

#define MSP_CTRL_OFFSET			0x1000
#define MSP_OG_OFFSET			0x1100
#define MSP_IN_OFFSET			0x1200
#define MSP_MSGID_OFFSET		0x1700
#define MSP_DQM_OFFSET			0x1800
#define MSP_DQM_QCTRL_OFFSET		0x1a00
#define MSP_DQM_QDATA_OFFSET		0x1c00

// IoprocBlockOutgoing:
// IoprocIoprocOgMsgSts OgMsgSts
#define MSP_OG_STS			0x0004



// IoprocBlockIncoming:
// IoprocIoprocInMsgCtl InMsgCtl
#define MSP_IN_MSG_CTL			0x0000
#define MSP_IN_MSG_CTL_LOW_WM_WORDS_MASK	0x3f
// IoprocIoprocInMsgSts InMsgSts
#define MSP_IN_MSG_STS			0x0004
// u32 InMsgData
#define MSP_IN_MSG_DATA			0x0040

#define MSP_MSGID_STRIDE		0x0004

// IoprocIoprocDqmRegs:
// IoprocIoprocDqmCfg DqmCfg
#define MSP_DQM_CFG			0x0000
// IoprocIoprocDqm4KeLowWtmkIrqMsk Dqm4keLowWtmkIrqMsk
#define MSP_DQM_4KE_LOW_WTMK_IRQ_MASK	0x0004
// IoprocIoprocDqmMipsLowWtmkIrqMsk DqmMipsLowWtmkIrqMsk
#define MSP_DQM_MIPS_LOW_WTMK_IRQ_MASK	0x0008
// IoprocIoprocDqmLowWtmkIrqSts DqmLowWtmkIrqSts
#define MSP_DQM_LOW_WTMK_IRQ_STS	0x000c
// IoprocIoprocDqm4KeNotEmptyIrqMsk Dqm4keNotEmptyIrqMsk
#define MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK	0x0010
// IoprocIoprocDqmMipsNotEmptyIrqMsk DqmMipsNotEmptyIrqMsk
#define MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK	0x0014
// IoprocIoprocDqmNotEmptyIrqSts DqmNotEmptyIrqSts
#define MSP_DQM_NOT_EMPTY_IRQ_STS	0x0018
// IoprocIoprocDqmNotEmptySts DqmNotEmptySts
#define MSP_DQM_NOT_EMPTY_STS		0x0020

// sizeof(IoprocIoprocQueueControl)
#define MSP_DQM_QCTRL_STRIDE		0x0010
// IoprocIoprocDqmQSize Size
#define MSP_DQM_QCTRL_SIZE		0x0000
// IoprocIoprocDqmQueueCfga Cfga
#define MSP_DQM_QCTRL_CFGA		0x0004
// IoprocIoprocDqmQueueCfgb Cfgb
#define MSP_DQM_QCTRL_CFGB		0x0008
// IoprocIoprocDqmQStatus Sts
#define MSP_DQM_QCTRL_STS		0x000c

#define MSP_DQM_QDATA_STRIDE		0x0010
#define MSP_DQM_QDATA_WORD_STRIDE	0x0004

// IoprocIoprocControlRegs:
// IoprocIoprocIrq4KeMask L1Irq4keMask
#define MSP_CTRL_L1_IRQ_4KE_MASK	0x0000
// u32 HostMboxIn
#define MSP_CTRL_HOST_MBOX_IN		0x0028
// u32 HostMboxOut
#define MSP_CTRL_HOST_MBOX_OUT		0x002c
// u32 Address1WindowMask
#define MSP_CTRL_ADDRESS1_WINDOW_MASK	0x006c
// u32 Address1WindowBaseIn
#define MSP_CTRL_ADDRESS1_WINDOW_BASE_IN	0x0070
// u32 Address1WindowBaseOut
#define MSP_CTRL_ADDRESS1_WINDOW_BASE_OUT	0x0074
// u32 Address2WindowMask
#define MSP_CTRL_ADDRESS2_WINDOW_MASK	0x0078
// u32 Address2WindowBaseIn
#define MSP_CTRL_ADDRESS2_WINDOW_BASE_IN	0x007c
// u32 Address2WindowBaseOut
#define MSP_CTRL_ADDRESS2_WINDOW_BASE_OUT	0x0080
// IoprocIoprocSoftResets SoftResets
#define MSP_CTRL_SOFT_RESETS		0x008c
#define MSP_CTRL_SOFT_RESET_M4KE	0x00000001
// IoprocIoproc4KeCoreStatus M4keCoreStatus
#define MSP_CTRL_M4KE_CORE_STATUS	0x0094

#define MSP_IOP4KE_RESET_VECTOR_PHYS	0x1fc00000
#define MSP_IOP4KE_ALIVE_MAGIC		0x4b454f4b
#define MSP_IOP_BUS_ADDR_MASK		0x1fffffff

struct bcm3380_msp {
	struct device *dev;
	void __iomem *base;
	u32 producer_base;
	const void *firmware_4ke;
	size_t firmware_4ke_size;
	u32 memory_4ke_size;
	void *firmware_4ke_mem;
	dma_addr_t firmware_4ke_dma;
	struct clk *clk;
	struct reset_control *reset;
	bool ready;
};

static bool msp_ready(struct bcm3380_msp *msp)
{
	return msp && msp->ready;
}

static void __iomem *msp_qctrl(struct bcm3380_msp *msp,
				       unsigned int queue)
{
	return msp->base + MSP_DQM_QCTRL_OFFSET +
	       queue * MSP_DQM_QCTRL_STRIDE;
}

static void __iomem *msp_qdata(struct bcm3380_msp *msp,
				       unsigned int queue)
{
	return msp->base + MSP_DQM_QDATA_OFFSET +
	       queue * MSP_DQM_QDATA_STRIDE;
}

static void __iomem *msp_ctrl(struct bcm3380_msp *msp, unsigned int offset)
{
	return msp->base + MSP_CTRL_OFFSET + offset;
}

static void msp_assert_4ke_reset(struct bcm3380_msp *msp)
{
	writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_SOFT_RESETS)) |
		  MSP_CTRL_SOFT_RESET_M4KE,
		  msp_ctrl(msp, MSP_CTRL_SOFT_RESETS));
	wmb();
}

static void msp_free_4ke_firmware(struct bcm3380_msp *msp)
{
	if (!msp->firmware_4ke_mem)
		return;

	dma_free_coherent(msp->dev, msp->memory_4ke_size,
			  msp->firmware_4ke_mem, msp->firmware_4ke_dma);
	msp->firmware_4ke_mem = NULL;
}

static int msp_start_4ke_firmware(struct bcm3380_msp *msp)
{
	u32 fw_bus_base;
	u32 fw_window_mask;
	unsigned int i;
	int ret;

	if (!msp->firmware_4ke)
		return 0;

	if (!msp->memory_4ke_size ||
	    (msp->memory_4ke_size & (msp->memory_4ke_size - 1)) ||
	    msp->firmware_4ke_size > msp->memory_4ke_size) {
		dev_err(msp->dev,
			"invalid MSP 4KE memory: firmware=%zu memory=%u\n",
			msp->firmware_4ke_size, msp->memory_4ke_size);
		return -EINVAL;
	}

	ret = dma_set_mask_and_coherent(msp->dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	msp->firmware_4ke_mem = dma_alloc_coherent(msp->dev,
						   msp->memory_4ke_size,
						   &msp->firmware_4ke_dma,
						   GFP_KERNEL);
	if (!msp->firmware_4ke_mem)
		return -ENOMEM;

	memset(msp->firmware_4ke_mem, 0, msp->memory_4ke_size);
	memcpy(msp->firmware_4ke_mem, msp->firmware_4ke,
	       msp->firmware_4ke_size);

	fw_bus_base = (u32)msp->firmware_4ke_dma & MSP_IOP_BUS_ADDR_MASK;
	fw_window_mask = ~(msp->memory_4ke_size - 1);

	msp_assert_4ke_reset(msp);
	mdelay(1);

	writel_be(0, msp_ctrl(msp, MSP_CTRL_L1_IRQ_4KE_MASK));
	writel_be(0, msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN));
	writel_be(MSP_IOP4KE_RESET_VECTOR_PHYS, msp_ctrl(msp, MSP_CTRL_HOST_MBOX_OUT));
	writel_be(fw_window_mask, msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_MASK));
	writel_be(MSP_IOP4KE_RESET_VECTOR_PHYS, msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_BASE_IN));
	writel_be(fw_bus_base, msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_BASE_OUT));
	writel_be(fw_window_mask, msp_ctrl(msp, MSP_CTRL_ADDRESS2_WINDOW_MASK));
	writel_be(0, msp_ctrl(msp, MSP_CTRL_ADDRESS2_WINDOW_BASE_IN));
	writel_be(fw_bus_base, msp_ctrl(msp, MSP_CTRL_ADDRESS2_WINDOW_BASE_OUT));
	wmb();

	writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_SOFT_RESETS)) &
		  ~MSP_CTRL_SOFT_RESET_M4KE,
		  msp_ctrl(msp, MSP_CTRL_SOFT_RESETS));
	wmb();

	for (i = 0; i < 100; i++) {
		if (readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN)) ==
		    MSP_IOP4KE_ALIVE_MAGIC) {
			dev_info(msp->dev,
				 "MSP 4KE RX->DQM firmware alive: fw_dma=0x%08x bus=0x%08x HostMboxIn=0x%08x CoreStatus=0x%08x\n",
				 (u32)msp->firmware_4ke_dma, fw_bus_base,
				 readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN)),
				 readl_be(msp_ctrl(msp, MSP_CTRL_M4KE_CORE_STATUS)));
			return 0;
		}
		mdelay(1);
	}

	dev_err(msp->dev,
		"MSP 4KE RX->DQM firmware did not report alive: fw_dma=0x%08x bus=0x%08x HostMboxIn=0x%08x SoftResets=0x%08x CoreStatus=0x%08x A1Mask=0x%08x A1In=0x%08x A1Out=0x%08x\n",
		(u32)msp->firmware_4ke_dma, fw_bus_base,
		readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN)),
		readl_be(msp_ctrl(msp, MSP_CTRL_SOFT_RESETS)),
		readl_be(msp_ctrl(msp, MSP_CTRL_M4KE_CORE_STATUS)),
		readl_be(msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_MASK)),
		readl_be(msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_BASE_IN)),
		readl_be(msp_ctrl(msp, MSP_CTRL_ADDRESS1_WINDOW_BASE_OUT)));

	return -ETIMEDOUT;
}

int msp_get(struct device *consumer, struct bcm3380_msp **msp)
{
	struct bcm3380_msp *provider;
	struct platform_device *pdev;
	struct device_node *np;

	if (!consumer || !consumer->of_node || !msp)
		return -EINVAL;

	np = of_parse_phandle(consumer->of_node, "msp", 0);
	if (!np)
		return -ENODEV;

	pdev = of_find_device_by_node(np);
	of_node_put(np);
	if (!pdev)
		return -EPROBE_DEFER;

	provider = platform_get_drvdata(pdev);
	if (!provider) {
		put_device(&pdev->dev);
		return -EPROBE_DEFER;
	}

	if (!msp_ready(provider)) {
		put_device(&pdev->dev);
		return -EINVAL;
	}

	*msp = provider;

	return 0;
}
EXPORT_SYMBOL_GPL(msp_get);

void msp_put(struct bcm3380_msp *msp)
{
	if (msp && msp->dev)
		put_device(msp->dev);
}
EXPORT_SYMBOL_GPL(msp_put);

u32 msp_in_msg_data_bus_addr(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return msp->producer_base + MSP_IN_OFFSET + MSP_IN_MSG_DATA;
}
EXPORT_SYMBOL_GPL(msp_in_msg_data_bus_addr);

void msp_init_messages(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return;

	writel_be(6 & MSP_IN_MSG_CTL_LOW_WM_WORDS_MASK,
		  msp->base + MSP_IN_OFFSET + MSP_IN_MSG_CTL);
	writel_be(1, msp->base + MSP_MSGID_OFFSET + 0 * MSP_MSGID_STRIDE);
	writel_be(1, msp->base + MSP_MSGID_OFFSET + 1 * MSP_MSGID_STRIDE);
	writel_be(2, msp->base + MSP_MSGID_OFFSET + 2 * MSP_MSGID_STRIDE);
	writel_be(1, msp->base + MSP_MSGID_OFFSET + 3 * MSP_MSGID_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_init_messages);

u32 msp_in_msg_status(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp->base + MSP_IN_OFFSET + MSP_IN_MSG_STS);
}
EXPORT_SYMBOL_GPL(msp_in_msg_status);

u32 msp_in_msg_read(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp->base + MSP_IN_OFFSET + MSP_IN_MSG_DATA);
}
EXPORT_SYMBOL_GPL(msp_in_msg_read);

u32 msp_og_msg_status(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp->base + MSP_OG_OFFSET + MSP_OG_STS);
}
EXPORT_SYMBOL_GPL(msp_og_msg_status);

u32 msp_4ke_host_mbox_in(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN));
}
EXPORT_SYMBOL_GPL(msp_4ke_host_mbox_in);

u32 msp_4ke_core_status(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp_ctrl(msp, MSP_CTRL_M4KE_CORE_STATUS));
}
EXPORT_SYMBOL_GPL(msp_4ke_core_status);

int msp_dqm_config_queue(struct bcm3380_msp *msp, unsigned int queue,
				 unsigned int token_words,
				 unsigned int mem_words,
				 unsigned int start_words,
				 unsigned int low_water_mark)
{
	void __iomem *qctrl;
	u32 k4_not_empty_mask;
	u32 mips_not_empty_mask;
	u32 queue_bit;

	if (!msp_ready(msp))
		return -ENODEV;
	if (queue >= 32 || !token_words || !mem_words)
		return -EINVAL;

	qctrl = msp_qctrl(msp, queue);
	queue_bit = BIT(queue);
	mips_not_empty_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	k4_not_empty_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);

	writel_be(mips_not_empty_mask & ~queue_bit,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(k4_not_empty_mask & ~queue_bit,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);

	writel_be(token_words - 1, qctrl + MSP_DQM_QCTRL_SIZE);
	writel_be((mem_words << 16) | start_words,
		  qctrl + MSP_DQM_QCTRL_CFGA);
	writel_be(((mem_words / token_words) << 16) | low_water_mark,
		  qctrl + MSP_DQM_QCTRL_CFGB);

	writel_be(queue_bit, msp->base + MSP_DQM_OFFSET + MSP_DQM_LOW_WTMK_IRQ_STS);
	writel_be(queue_bit, msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS);
	writel_be(mips_not_empty_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(k4_not_empty_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);

	dev_info(msp->dev,
		 "DQM q%u Size=0x%08x Cfga=0x%08x Cfgb=0x%08x Sts=0x%08x\n",
		 queue, readl_be(qctrl + MSP_DQM_QCTRL_SIZE),
		 readl_be(qctrl + MSP_DQM_QCTRL_CFGA),
		 readl_be(qctrl + MSP_DQM_QCTRL_CFGB),
		 readl_be(qctrl + MSP_DQM_QCTRL_STS));

	return 0;
}
EXPORT_SYMBOL_GPL(msp_dqm_config_queue);

void msp_dqm_test_init(struct bcm3380_msp *msp,
			       unsigned int normal_queue,
			       unsigned int high_queue,
			       unsigned int token_words,
			       unsigned int queue_words,
			       unsigned int total_mem_words)
{
	u32 queue_mask;

	if (!msp_ready(msp) || normal_queue >= 32 || high_queue >= 32)
		return;

	queue_mask = BIT(normal_queue) | BIT(high_queue);

	writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK) &
		  ~queue_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK) &
		  ~queue_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);
	writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_LOW_WTMK_IRQ_MASK) &
		  ~queue_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_LOW_WTMK_IRQ_MASK);
	writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_LOW_WTMK_IRQ_MASK) &
		  ~queue_mask,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_LOW_WTMK_IRQ_MASK);
	writel_be(queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_LOW_WTMK_IRQ_STS);
	writel_be(queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS);
	writel_be(total_mem_words << 16,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_CFG);

	msp_dqm_config_queue(msp, normal_queue, token_words, queue_words, 0, 0);
	msp_dqm_config_queue(msp, high_queue, token_words, queue_words, queue_words, 0);

	dev_info(msp->dev,
		 "DQM init: DqmCfg=0x%08x NotEmptySts=0x%08x q%u_sts=0x%08x q%u_sts=0x%08x\n",
		 readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_CFG),
		 msp_dqm_not_empty_status(msp),
		 normal_queue, msp_dqm_queue_status(msp, normal_queue),
		 high_queue, msp_dqm_queue_status(msp, high_queue));
}
EXPORT_SYMBOL_GPL(msp_dqm_test_init);

bool msp_dqm_queue_not_empty(struct bcm3380_msp *msp,
				     unsigned int queue)
{
	if (!msp_ready(msp) || queue >= 32)
		return false;

	return msp_dqm_not_empty_status(msp) & BIT(queue);
}
EXPORT_SYMBOL_GPL(msp_dqm_queue_not_empty);

bool msp_dqm_queue_has_space(struct bcm3380_msp *msp,
				     unsigned int queue)
{
	if (!msp_ready(msp) || queue >= 32)
		return false;

	return msp_dqm_queue_status(msp, queue) &
	       MSP_DQM_Q_AVAIL_MASK;
}
EXPORT_SYMBOL_GPL(msp_dqm_queue_has_space);

u32 msp_dqm_not_empty_status(struct bcm3380_msp *msp)
{
	if (!msp_ready(msp))
		return 0;

	return readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_STS);
}
EXPORT_SYMBOL_GPL(msp_dqm_not_empty_status);

u32 msp_dqm_queue_status(struct bcm3380_msp *msp,
				 unsigned int queue)
{
	if (!msp_ready(msp) || queue >= 32)
		return 0;

	return readl_be(msp_qctrl(msp, queue) + MSP_DQM_QCTRL_STS);
}
EXPORT_SYMBOL_GPL(msp_dqm_queue_status);

u32 msp_dqm_read_word(struct bcm3380_msp *msp, unsigned int queue,
			      unsigned int word)
{
	if (!msp_ready(msp) || queue >= 32 || word >= 4)
		return 0;

	return readl_be(msp_qdata(msp, queue) + word * MSP_DQM_QDATA_WORD_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_dqm_read_word);

void msp_dqm_write_word(struct bcm3380_msp *msp, unsigned int queue,
				unsigned int word, u32 value)
{
	if (!msp_ready(msp) || queue >= 32 || word >= 4)
		return;

	writel_be(value, msp_qdata(msp, queue) + word * MSP_DQM_QDATA_WORD_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_dqm_write_word);

static int msp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3380_msp *msp;
	struct resource *res;
	struct property *memory_4ke_prop;
	int firmware_size;
	int ret;

	msp = devm_kzalloc(dev, sizeof(*msp), GFP_KERNEL);
	if (!msp)
		return -ENOMEM;

	msp->dev = dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	msp->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(msp->base))
		return PTR_ERR(msp->base);

	ret = of_property_read_u32(dev->of_node, "brcm,producer-base", &msp->producer_base);
	if (ret == -EINVAL) {
		msp->producer_base = ((u32)res->start & ~0x0f000000) | 0x05000000;
	} else if (ret) {
		dev_err(dev, "invalid brcm,producer-base\n");
		return ret;
	}

	msp->firmware_4ke = of_get_property(dev->of_node,
					    "brcm,4ke-firmware",
					    &firmware_size);
	memory_4ke_prop = of_find_property(dev->of_node,
					   "brcm,4ke-memory-size", NULL);
	if (!!msp->firmware_4ke != !!memory_4ke_prop) {
		dev_err(dev, "brcm,4ke-firmware and brcm,4ke-memory-size must be specified together\n");
		return -EINVAL;
	}

	if (msp->firmware_4ke) {
		if (firmware_size <= 0) {
			dev_err(dev, "invalid brcm,4ke-firmware size: %d\n", firmware_size);
			return -EINVAL;
		}

		msp->firmware_4ke_size = firmware_size;

		ret = of_property_read_u32(dev->of_node,
					   "brcm,4ke-memory-size",
					   &msp->memory_4ke_size);
		if (ret) {
			dev_err(dev, "invalid brcm,4ke-memory-size\n");
			return ret;
		}
	}

	msp->clk = devm_clk_get_optional(dev, "msp");
	if (IS_ERR(msp->clk))
		return PTR_ERR(msp->clk);

	if (msp->clk) {
		ret = clk_prepare_enable(msp->clk);
		if (ret)
			return ret;
	}

	msp->reset = devm_reset_control_get_optional_exclusive(dev, "msp");
	if (IS_ERR(msp->reset)) {
		ret = PTR_ERR(msp->reset);
		goto disable_clk;
	}

	if (msp->reset) {
		ret = reset_control_reset(msp->reset);
		if (ret)
			goto disable_clk;
	}

	writel_be(MSP_SMISB_CTRL_ENABLE, (void __iomem *)MSP_SMISB_CTRL);
	mdelay(10);

	ret = msp_start_4ke_firmware(msp);
	if (ret)
		goto assert_reset;

	msp->ready = true;
	platform_set_drvdata(pdev, msp);

	if (msp->firmware_4ke)
		dev_info(dev,
			 "BCM3380 MSP ready: inmsg_data_bus=0x%08x 4ke_fw=%zu 4ke_mem=%u\n",
			 msp_in_msg_data_bus_addr(msp),
			 msp->firmware_4ke_size, msp->memory_4ke_size);
	else
		dev_info(dev, "BCM3380 MSP ready: inmsg_data_bus=0x%08x\n",
			 msp_in_msg_data_bus_addr(msp));

	return 0;

assert_reset:
	msp_assert_4ke_reset(msp);
	msp_free_4ke_firmware(msp);
	if (msp->reset)
		reset_control_assert(msp->reset);
disable_clk:
	if (msp->clk)
		clk_disable_unprepare(msp->clk);
	return ret;
}

static void msp_remove(struct platform_device *pdev)
{
	struct bcm3380_msp *msp = platform_get_drvdata(pdev);

	msp->ready = false;
	msp_assert_4ke_reset(msp);
	msp_free_4ke_firmware(msp);

	if (msp->reset)
		reset_control_assert(msp->reset);

	if (msp->clk)
		clk_disable_unprepare(msp->clk);
}

static const struct of_device_id msp_of_match[] = {
	{ .compatible = "brcm,bcm3380-msp" },
	{ }
};
MODULE_DEVICE_TABLE(of, msp_of_match);

static struct platform_driver msp_driver = {
	.probe = msp_probe,
	.remove = msp_remove,
	.driver = {
		.name = "bcm3380-msp",
		.of_match_table = msp_of_match,
	},
};
module_platform_driver(msp_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3380 MSP IOPROC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bcm3380-msp");
