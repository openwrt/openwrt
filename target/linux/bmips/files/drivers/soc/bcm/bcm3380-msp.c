// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 MSP(Message Processing Processor) Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/firmware.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/types.h>

#include <soc/bcm/bcm3380-msp-fw.h>
#include <soc/bcm/bcm3380-msp.h>

#define MSP_CTRL_OFFSET			0x1000
#define MSP_OG_OFFSET			0x1100
#define MSP_IN_OFFSET			0x1200
#define MSP_MSGID_OFFSET		0x1700
#define MSP_DQM_OFFSET			0x1800
#define MSP_DQM_QCTRL_OFFSET		0x1a00
#define MSP_DQM_QDATA_OFFSET		0x1c00

// IoprocBlockIncoming:
// IoprocIoprocInMsgCtl InMsgCtl
#define MSP_IN_MSG_CTL			0x0000
#define MSP_IN_MSG_CTL_LOW_WM_WORDS_MASK	0x3f
// IoprocIoprocInMsgSts InMsgSts
#define MSP_IN_MSG_STS			0x0004
// u32 InMsgData
#define MSP_IN_MSG_DATA			0x0040

// IoprocBlockOutgoing:
// IoprocIoprocOgMsgCtl OgMsgCtl
#define MSP_OG_MSG_CTL			0x0000
// IoprocIoprocOgMsgSts OgMsgSts
#define MSP_OG_MSG_STS			0x0004

#define MSP_MSGID_STRIDE		0x0004
#define MSP_MSGID_MSG_WD_SZ_ID_MASK	0x3f

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

#define MSP_DQM_QDATA_STRIDE		0x0010
#define MSP_DQM_QDATA_WORD_STRIDE	0x0004

// IoprocIoprocControlRegs:
// IoprocIoprocIrq4KeMask L1Irq4keMask
#define MSP_CTRL_L1_IRQ_4KE_MASK	0x0000
// IoprocIoprocIrq4KeStatus L1Irq4keStatus
#define MSP_CTRL_L1_IRQ_4KE_STATUS	0x0004
// IoprocIoprocIrqMipsMask L1IrqMipsMask
#define MSP_CTRL_L1_IRQ_MIPS_MASK	0x0008
// IoprocIoprocIrqMipsStatus L1IrqMipsStatus
#define MSP_CTRL_L1_IRQ_MIPS_STATUS	0x000c
// IoprocIoprocGpMsk L2IrqGpMsk
#define MSP_CTRL_L2_IRQ_GP_MASK		0x0010
// IoprocIoprocGpSts L2IrqGpSts
#define MSP_CTRL_L2_IRQ_GP_STATUS	0x0014
// u32 HostMboxIn
#define MSP_CTRL_HOST_MBOX_IN		0x0028
// u32 HostMboxOut
#define MSP_CTRL_HOST_MBOX_OUT		0x002c
// IoprocIoprocGeneralPurposeIrqMask GpInIrqMask
#define MSP_CTRL_GP_IN_IRQ_MASK		0x0038
// IoprocIoprocGeneralPurposeInputIrqStatus GpInIrqStatus
#define MSP_CTRL_GP_IN_IRQ_STATUS	0x003c
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

#define MSP_CTRL_MIPS_DQM_IRQ		0x00000008

#define MSP_IOP4KE_RESET_VECTOR_PHYS	0x1fc00000
#define MSP_IOP_BUS_ADDR_MASK		0x1fffffff
#define MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS	4

struct variant_data;

struct msp_dqm_host_not_empty_irq_slot {
	u32 queue_mask;
	msp_dqm_host_not_empty_irq_callback_t callback;
	void *data;
};

struct bcm3380_msp {
	struct device *dev;
	void __iomem *base;
	u32 producer_base;
	const struct variant_data *variant;

	const struct firmware *firmware_4ke;
	u32 memory_4ke_size;
	void *firmware_4ke_mem;
	dma_addr_t firmware_4ke_dma;

	struct clk *clk;
	struct reset_control *reset;
	int irq; // MSP's IRQ line to the host CPU

	// DQM
	struct device_node *dqm_node;
	u32 dqm_memory_words;
	u32 dqm_enabled_queues;
	spinlock_t dqm_host_not_empty_irq_lock;
	struct msp_dqm_host_not_empty_irq_slot dqm_host_not_empty_irq_slots[MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS];

	bool ready;
};

struct variant_data {
	u32 (*dqm_queue_status)(struct bcm3380_msp *msp, unsigned int queue);
	u32 (*dqm_queue_status_offset)(unsigned int queue);
	u32 ioproc_base_4ke;
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

static bool msp_dt_node_enabled(struct device_node *np)
{
	const char *status;

	if (of_property_read_string(np, "status", &status))
		return false;

	return !strcmp(status, "okay") || !strcmp(status, "ok");
}

static u32 msp_dqm_host_not_empty_irq_queue_mask(struct bcm3380_msp *msp)
{
	u32 queue_mask = 0;

	for (unsigned int i = 0; i < MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS; i++) {
		struct msp_dqm_host_not_empty_irq_slot *slot =
			&msp->dqm_host_not_empty_irq_slots[i];

		if (slot->callback)
			queue_mask |= slot->queue_mask;
	}

	return queue_mask;
}

static irqreturn_t msp_dqm_host_not_empty_irq(int irq, void *data)
{
	struct bcm3380_msp *msp = data;
	msp_dqm_host_not_empty_irq_callback_t callbacks[MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS];
	void *callback_data[MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS];
	unsigned int callback_count = 0;
	unsigned long flags;
	u32 l1_pending;
	u32 dqm_mask;
	u32 dqm_status;
	u32 pending;

	spin_lock_irqsave(&msp->dqm_host_not_empty_irq_lock, flags);

	if (!msp_ready(msp)) {
		spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
		return IRQ_NONE;
	}

	l1_pending = readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_STATUS)) & readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));
	if (!(l1_pending & MSP_CTRL_MIPS_DQM_IRQ)) {
		spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
		return IRQ_NONE;
	}

	dqm_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	dqm_status = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS);
	pending = dqm_status & dqm_mask & msp_dqm_host_not_empty_irq_queue_mask(msp);
	if (!pending) {
		spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
		return IRQ_HANDLED;
	}

	writel_be(dqm_mask & ~pending, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	for (unsigned int i = 0; i < MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS; i++) {
		struct msp_dqm_host_not_empty_irq_slot *slot =
			&msp->dqm_host_not_empty_irq_slots[i];

		if (slot->callback && (pending & slot->queue_mask)) {
			callbacks[callback_count] = slot->callback;
			callback_data[callback_count] = slot->data;
			callback_count++;
		}
	}

	spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);

	for (unsigned int i = 0; i < callback_count; i++)
		callbacks[i](callback_data[i]);

	return IRQ_HANDLED;
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

	if (!msp->firmware_4ke) {
		msp_assert_4ke_reset(msp);
		return 0;
	}

	if (!msp->memory_4ke_size ||
	    (msp->memory_4ke_size & (msp->memory_4ke_size - 1)) ||
	    msp->firmware_4ke->size > MSP_4KE_CONFIG_OFFSET ||
	    msp->memory_4ke_size < MSP_4KE_CONFIG_OFFSET +
				   sizeof(struct msp_4ke_config)) {
		dev_err(msp->dev,
			"invalid MSP 4KE memory: firmware=%zu config=0x%x memory=%u\n",
			msp->firmware_4ke->size, MSP_4KE_CONFIG_OFFSET,
			msp->memory_4ke_size);
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
	memcpy(msp->firmware_4ke_mem, msp->firmware_4ke->data,
	       msp->firmware_4ke->size);

	struct msp_4ke_config *config =
		(void *)((u8 *)msp->firmware_4ke_mem + MSP_4KE_CONFIG_OFFSET);

	*config = (struct msp_4ke_config) {
		.magic = MSP_4KE_CONFIG_MAGIC,
		.version = MSP_4KE_CONFIG_VERSION,
		.ioproc_base = msp->variant->ioproc_base_4ke,
		.host_mbox_in_offset = MSP_CTRL_OFFSET + MSP_CTRL_HOST_MBOX_IN,
		.dqm_cfg_offset = MSP_DQM_OFFSET + MSP_DQM_CFG,
		.dqm_cfg_value = MSP_4KE_DQM_CFG_VALUE,
		.in_msg_status_offset = MSP_IN_OFFSET + MSP_IN_MSG_STS,
		.in_msg_data_offset = MSP_IN_OFFSET + MSP_IN_MSG_DATA,
		.dqm_not_empty_status_offset = MSP_DQM_OFFSET +
					       MSP_DQM_NOT_EMPTY_STS,
	};

	dev_dbg(msp->dev,
		"MSP 4KE config: ioproc=0x%08x in_sts=0x%04x in_data=0x%04x dqm_not_empty=0x%04x\n",
		config->ioproc_base, config->in_msg_status_offset,
		config->in_msg_data_offset,
		config->dqm_not_empty_status_offset);

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
		    MSP_4KE_ALIVE_MAGIC) {
			dev_info(msp->dev,
				"MSP 4KE RX/TX DQM firmware alive: fw_dma=0x%08x bus=0x%08x HostMboxIn=0x%08x CoreStatus=0x%08x\n",
				 (u32)msp->firmware_4ke_dma, fw_bus_base,
				 readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN)),
				 readl_be(msp_ctrl(msp, MSP_CTRL_M4KE_CORE_STATUS)));
			return 0;
		}
		mdelay(1);
	}

	dev_err(msp->dev,
		"MSP 4KE RX/TX DQM firmware did not report alive: fw_dma=0x%08x bus=0x%08x HostMboxIn=0x%08x SoftResets=0x%08x CoreStatus=0x%08x A1Mask=0x%08x A1In=0x%08x A1Out=0x%08x\n",
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
		return -EPROBE_DEFER;
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

void msp_set_msgid_word_size(struct bcm3380_msp *msp, u8 msgid, u8 msg_wd_sz_id)
{
	if (!msp_ready(msp) || msgid >= 64 || msg_wd_sz_id > MSP_MSGID_MSG_WD_SZ_ID_MASK)
		return;

	writel_be(msg_wd_sz_id, msp->base + MSP_MSGID_OFFSET + msgid * MSP_MSGID_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_set_msgid_word_size);

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

int msp_dqm_get_queue(struct bcm3380_msp *msp, struct device *consumer,
		      const char *property, unsigned int *queue)
{
	struct device_node *parent;
	struct device_node *np;
	u32 queue_id;
	int ret;

	if (!msp_ready(msp) || !consumer || !consumer->of_node ||
	    !property || !queue)
		return -EINVAL;

	np = of_parse_phandle(consumer->of_node, property, 0);
	if (!np)
		return -ENODEV;

	parent = of_get_parent(np);
	if (parent != msp->dqm_node) {
		ret = -EINVAL;
		goto out_put_parent;
	}

	ret = of_property_read_u32(np, "reg", &queue_id);
	if (ret)
		goto out_put_parent;
	if (queue_id >= 32 || !(msp->dqm_enabled_queues & BIT(queue_id))) {
		ret = -EINVAL;
		goto out_put_parent;
	}

	*queue = queue_id;
	ret = 0;

out_put_parent:
	of_node_put(parent);
	of_node_put(np);
	return ret;
}
EXPORT_SYMBOL_GPL(msp_dqm_get_queue);

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

static ssize_t status_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct bcm3380_msp *msp = dev_get_drvdata(dev);
	ssize_t len = 0;

	if (!msp_ready(msp))
		return -ENODEV;

	len += sysfs_emit_at(buf, len, "host_mbox_in: 0x%08x\n",
			     readl_be(msp_ctrl(msp, MSP_CTRL_HOST_MBOX_IN)));
	len += sysfs_emit_at(buf, len, "core_status: 0x%08x\n",
			     readl_be(msp_ctrl(msp, MSP_CTRL_M4KE_CORE_STATUS)));
	len += sysfs_emit_at(buf, len, "dqm_not_empty: 0x%08x\n",
			     readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_STS));
	len += sysfs_emit_at(buf, len, "dqm_not_empty_irq_mask: 0x%08x\n",
			     readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK));
	len += sysfs_emit_at(buf, len, "dqm_not_empty_irq_status: 0x%08x\n",
			     readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS));

	return len;
}
static DEVICE_ATTR_RO(status);

static struct attribute *msp_attrs[] = {
	&dev_attr_status.attr,
	NULL,
};

static const struct attribute_group msp_attr_group = {
	.attrs = msp_attrs,
};

// Returns the 4ke firmware config section seen by the host
static struct msp_4ke_config *msp_4ke_config(struct bcm3380_msp *msp)
{
	if (!msp->firmware_4ke_mem)
		return NULL;

	return (void *)((u8 *)msp->firmware_4ke_mem + MSP_4KE_CONFIG_OFFSET);
}

static bool msp_dqm_queue_enabled(struct bcm3380_msp *msp, unsigned int queue)
{
	return queue < 32 && (msp->dqm_enabled_queues & BIT(queue));
}

int msp_4ke_register_enet_port(struct bcm3380_msp *msp, u32 mac_id,
			       unsigned int rx_normal_queue,
			       unsigned int rx_high_queue,
			       unsigned int tx_high_queue,
			       unsigned int tx_normal_queue,
			       u32 tx_fifo_addr)
{
	if (!msp_ready(msp))
		return -ENODEV;
	if (mac_id >= MSP_4KE_MAX_ENET_PORTS || !tx_fifo_addr)
		return -EINVAL;
	if (!msp_dqm_queue_enabled(msp, rx_normal_queue) ||
	    !msp_dqm_queue_enabled(msp, rx_high_queue) ||
	    !msp_dqm_queue_enabled(msp, tx_high_queue) ||
	    !msp_dqm_queue_enabled(msp, tx_normal_queue))
		return -EINVAL;

	struct msp_4ke_config *config = msp_4ke_config(msp);
	if (!config)
		return -ENODEV;

	struct msp_4ke_port_config *port = &config->enet_ports[mac_id];
	port->valid = 0;
	wmb();
	port->mac_id = mac_id;
	port->rx_normal_queue = rx_normal_queue;
	port->rx_high_queue = rx_high_queue;
	port->rx_queue_status_offset = msp->variant->dqm_queue_status_offset(rx_normal_queue);
	port->rx_queue_data_offset = MSP_DQM_QDATA_OFFSET + rx_normal_queue * MSP_DQM_QDATA_STRIDE;
	port->tx_high_queue = tx_high_queue;
	port->tx_normal_queue = tx_normal_queue;
	port->tx_fifo_addr = tx_fifo_addr;
	port->tx_header = MSP_4KE_LAN_TX_HEADER | (mac_id << MSP_4KE_LAN_TX_MAC_ID_SHIFT);
	if (config->enet_port_count < mac_id + 1)
		config->enet_port_count = mac_id + 1;
	wmb();
	port->valid = 1;
	wmb();

	dev_info(msp->dev,
		 "MSP 4KE ENET port%u: rx_normal=%u rx_high=%u tx_high=%u tx_normal=%u tx_fifo=0x%08x\n",
		 mac_id, rx_normal_queue, rx_high_queue, tx_high_queue,
		 tx_normal_queue, tx_fifo_addr);

	return 0;
}
EXPORT_SYMBOL_GPL(msp_4ke_register_enet_port);

void msp_4ke_unregister_enet_port(struct bcm3380_msp *msp, u32 mac_id)
{
	if (!msp_ready(msp) || mac_id >= MSP_4KE_MAX_ENET_PORTS)
		return;

	struct msp_4ke_config *config = msp_4ke_config(msp);
	if (!config)
		return;

	config->enet_ports[mac_id].valid = 0;
	wmb();
}
EXPORT_SYMBOL_GPL(msp_4ke_unregister_enet_port);

static int msp_dqm_config_queue_raw(struct bcm3380_msp *msp, unsigned int queue,
				    unsigned int token_words,
				    unsigned int mem_words,
				    unsigned int start_words,
				    unsigned int low_water_mark)
{
	if (queue >= 32 || !token_words || !mem_words)
		return -EINVAL;
	if (start_words > msp->dqm_memory_words ||
	    mem_words > msp->dqm_memory_words - start_words)
		return -EINVAL;

	void __iomem *qctrl = msp_qctrl(msp, queue);
	u32 queue_bit = BIT(queue);
	u32 mips_not_empty_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	u32 k4_not_empty_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);

	writel_be(mips_not_empty_mask & ~queue_bit,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(k4_not_empty_mask & ~queue_bit,
		  msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);

	// QHeadPtr = 0; QTailPtr = 0; QTknSize = token_words - 1;
	writel_be(token_words - 1, qctrl + MSP_DQM_QCTRL_SIZE);
	// QSize = mem_words; QStartAddr = start_words;
	writel_be((mem_words << 16) | start_words,
		  qctrl + MSP_DQM_QCTRL_CFGA);
	// QNumTkns = (mem_words/token_words); QLowWatermark = low_water_mark;
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
		 msp->variant->dqm_queue_status(msp, queue));

	return 0;
}

static int msp_dqm_config_queue_node(struct bcm3380_msp *msp,
				     struct device_node *np,
				     unsigned int *next_start_words)
{
	struct device *dev = msp->dev;
	unsigned int mem_words;
	u32 low_water_mark;
	u32 token_words = 1;
	u32 capacity;
	u32 queue;
	int ret;

	ret = of_property_read_u32(np, "reg", &queue);
	if (ret) {
		dev_err(dev, "%pOF: missing DQM queue reg\n", np);
		return ret;
	}
	if (queue >= 32) {
		dev_err(dev, "%pOF: invalid DQM queue id %u\n", np, queue);
		return -EINVAL;
	}
	if (msp->dqm_enabled_queues & BIT(queue)) {
		dev_err(dev, "%pOF: duplicate DQM queue id %u\n", np, queue);
		return -EINVAL;
	}

	ret = of_property_read_u32(np, "brcm,capacity", &capacity);
	if (ret) {
		dev_err(dev, "%pOF: missing brcm,capacity\n", np);
		return ret;
	}

	ret = of_property_read_u32(np, "brcm,low_water_mark",
				   &low_water_mark);
	if (ret) {
		dev_err(dev, "%pOF: missing brcm,low_water_mark\n", np);
		return ret;
	}

	ret = of_property_read_u32(np, "brcm,token_words", &token_words);
	if (ret && ret != -EINVAL) {
		dev_err(dev, "%pOF: invalid brcm,token_words\n", np);
		return ret;
	}

	if (!capacity || capacity > U16_MAX || !token_words ||
	    token_words > 4 || low_water_mark > capacity) {
		dev_err(dev,
			"%pOF: invalid capacity=%u token_words=%u low_water_mark=%u\n",
			np, capacity, token_words, low_water_mark);
		return -EINVAL;
	}

	mem_words = capacity * token_words;
	if (capacity != mem_words / token_words || mem_words > U16_MAX ||
	    *next_start_words > msp->dqm_memory_words ||
	    mem_words > msp->dqm_memory_words - *next_start_words) {
		dev_err(dev,
			"%pOF: DQM memory overflow start=%u mem=%u total=%u\n",
			np, *next_start_words, mem_words, msp->dqm_memory_words);
		return -EINVAL;
	}

	ret = msp_dqm_config_queue_raw(msp, queue, token_words, mem_words,
				       *next_start_words, low_water_mark);
	if (ret)
		return ret;

	msp->dqm_enabled_queues |= BIT(queue);
	*next_start_words += mem_words;

	return 0;
}

static int msp_dqm_init_from_dt(struct bcm3380_msp *msp)
{
	struct device_node *np;
	unsigned int next_start_words = 0;
	int ret;

	ret = of_property_read_u32(msp->dqm_node, "brcm,dqm-memory-words",
				   &msp->dqm_memory_words);
	if (ret) {
		dev_err(msp->dev, "%pOF: missing or invalid brcm,dqm-memory-words\n",
			msp->dqm_node);
		return ret;
	}
	if (!msp->dqm_memory_words || msp->dqm_memory_words > U16_MAX) {
		dev_err(msp->dev, "%pOF: invalid brcm,dqm-memory-words: %u\n",
			msp->dqm_node, msp->dqm_memory_words);
		return -EINVAL;
	}

	// TotMemSz=msp->dqm_memory_words; StartAddr=0;
	writel_be(msp->dqm_memory_words << 16, msp->base + MSP_DQM_OFFSET + MSP_DQM_CFG);

	for_each_child_of_node(msp->dqm_node, np) {
		if (!msp_dt_node_enabled(np))
			continue;

		ret = msp_dqm_config_queue_node(msp, np, &next_start_words);
		if (ret) {
			of_node_put(np);
			return ret;
		}
	}

	return 0;
}

bool msp_dqm_queue_not_empty(struct bcm3380_msp *msp,
				     unsigned int queue)
{
	if (!msp_ready(msp) || queue >= 32 ||
	    !(msp->dqm_enabled_queues & BIT(queue)))
		return false;

	return msp_dqm_not_empty_status(msp) & BIT(queue);
}
EXPORT_SYMBOL_GPL(msp_dqm_queue_not_empty);

bool msp_dqm_queue_has_space(struct bcm3380_msp *msp,
				     unsigned int queue)
{
	if (!msp_ready(msp) || queue >= 32 ||
	    !(msp->dqm_enabled_queues & BIT(queue)))
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
	if (!msp_ready(msp) || queue >= 32 ||
	    !(msp->dqm_enabled_queues & BIT(queue)))
		return 0;

	return msp->variant->dqm_queue_status(msp, queue);
}
EXPORT_SYMBOL_GPL(msp_dqm_queue_status);

u32 msp_dqm_read_word(struct bcm3380_msp *msp, unsigned int queue,
			      unsigned int word)
{
	if (!msp_ready(msp) || queue >= 32 || word >= 4 ||
	    !(msp->dqm_enabled_queues & BIT(queue)))
		return 0;

	return readl_be(msp_qdata(msp, queue) + word * MSP_DQM_QDATA_WORD_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_dqm_read_word);

void msp_dqm_write_word(struct bcm3380_msp *msp, unsigned int queue,
				unsigned int word, u32 value)
{
	if (!msp_ready(msp) || queue >= 32 || word >= 4 ||
	    !(msp->dqm_enabled_queues & BIT(queue)))
		return;

	writel_be(value, msp_qdata(msp, queue) + word * MSP_DQM_QDATA_WORD_STRIDE);
}
EXPORT_SYMBOL_GPL(msp_dqm_write_word);

int msp_dqm_host_not_empty_irq_register(struct bcm3380_msp *msp,
					u32 queue_mask,
					msp_dqm_host_not_empty_irq_callback_t callback,
					void *data)
{
	if (!msp_ready(msp) || msp->irq < 0)
		return -ENODEV;
	if (!queue_mask || !callback ||
	    (queue_mask & ~msp->dqm_enabled_queues))
		return -EINVAL;

	unsigned long flags;
	int free_slot = -1;

	spin_lock_irqsave(&msp->dqm_host_not_empty_irq_lock, flags);

	for (unsigned int i = 0; i < MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS; i++) {
		struct msp_dqm_host_not_empty_irq_slot *slot =
			&msp->dqm_host_not_empty_irq_slots[i];

		if (!slot->callback) {
			if (free_slot < 0)
				free_slot = i;
			continue;
		}

		if (slot->callback == callback && slot->data == data) {
			spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
			return -EEXIST;
		}

		if (slot->queue_mask & queue_mask) {
			spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
			return -EBUSY;
		}
	}

	if (free_slot < 0) {
		spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
		return -ENOSPC;
	}

	u32 mips_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(mips_mask & ~queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	struct msp_dqm_host_not_empty_irq_slot *slot =
		&msp->dqm_host_not_empty_irq_slots[free_slot];
	slot->queue_mask = queue_mask;
	slot->callback = callback;
	slot->data = data;
	writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK)) | MSP_CTRL_MIPS_DQM_IRQ, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));

	spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);

	return 0;
}
EXPORT_SYMBOL_GPL(msp_dqm_host_not_empty_irq_register);

void msp_dqm_host_not_empty_irq_unregister(struct bcm3380_msp *msp,
					   msp_dqm_host_not_empty_irq_callback_t callback,
					   void *data)
{
	unsigned long flags;

	if (!msp_ready(msp) || !callback)
		return;

	spin_lock_irqsave(&msp->dqm_host_not_empty_irq_lock, flags);

	for (unsigned int i = 0; i < MSP_DQM_HOST_NOT_EMPTY_IRQ_SLOTS; i++) {
		struct msp_dqm_host_not_empty_irq_slot *slot =
			&msp->dqm_host_not_empty_irq_slots[i];

		if (slot->callback != callback || slot->data != data)
			continue;

		if (slot->queue_mask)
			writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK) & ~slot->queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
		memset(slot, 0, sizeof(*slot));
		break;
	}

	if (!msp_dqm_host_not_empty_irq_queue_mask(msp))
		writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK)) & ~MSP_CTRL_MIPS_DQM_IRQ, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));

	spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
}
EXPORT_SYMBOL_GPL(msp_dqm_host_not_empty_irq_unregister);

static void msp_dqm_host_not_empty_irq_unregister_all(struct bcm3380_msp *msp)
{
	unsigned long flags;
	u32 queue_mask = 0;

	if (!msp_ready(msp))
		return;

	spin_lock_irqsave(&msp->dqm_host_not_empty_irq_lock, flags);

	queue_mask = msp_dqm_host_not_empty_irq_queue_mask(msp);
	if (queue_mask)
		writel_be(readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK) & ~queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	memset(msp->dqm_host_not_empty_irq_slots, 0,
	       sizeof(msp->dqm_host_not_empty_irq_slots));
	writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK)) & ~MSP_CTRL_MIPS_DQM_IRQ, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));

	spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
}

void msp_dqm_host_not_empty_irq_rearm(struct bcm3380_msp *msp, u32 queue_mask)
{
	unsigned long flags;

	if (!msp_ready(msp) || !queue_mask)
		return;

	spin_lock_irqsave(&msp->dqm_host_not_empty_irq_lock, flags);

	queue_mask &= msp_dqm_host_not_empty_irq_queue_mask(msp);
	if (queue_mask) {
		writel_be(queue_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS);
		u32 irq_mask = readl_be(msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK) | queue_mask;
		writel_be(irq_mask, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
		writel_be(readl_be(msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK)) | MSP_CTRL_MIPS_DQM_IRQ, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));
	}

	spin_unlock_irqrestore(&msp->dqm_host_not_empty_irq_lock, flags);
}
EXPORT_SYMBOL_GPL(msp_dqm_host_not_empty_irq_rearm);

static int msp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3380_msp *msp;
	u32 in_low_water_mark;
	int ret;

	msp = devm_kzalloc(dev, sizeof(*msp), GFP_KERNEL);
	if (!msp)
		return -ENOMEM;

	msp->dev = dev;
	msp->variant = of_device_get_match_data(dev);
	if (!msp->variant)
		return dev_err_probe(dev, -EINVAL, "missing MSP variant data\n");
	spin_lock_init(&msp->dqm_host_not_empty_irq_lock);

	struct resource *msp_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "msp");
	msp->base = devm_ioremap_resource(dev, msp_res);
	if (IS_ERR(msp->base))
		return PTR_ERR(msp->base);

	struct resource *res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "smisb-ctrl");
	void __iomem *smisb_ctrl = devm_ioremap_resource(dev, res);
	if (IS_ERR(smisb_ctrl))
		return PTR_ERR(smisb_ctrl);

	u32 smisb_ctrl_value;
	ret = of_property_read_u32(dev->of_node, "brcm,smisb-ctrl-value",
				   &smisb_ctrl_value);
	if (ret) {
		dev_err(dev, "missing or invalid brcm,smisb-ctrl-value\n");
		return ret;
	}

	msp->irq = platform_get_irq_optional(pdev, 0);
	if (msp->irq < 0 && msp->irq != -ENXIO)
		return msp->irq;

	ret = of_property_read_u32(dev->of_node, "brcm,producer-base", &msp->producer_base);
	if (ret == -EINVAL) {
		msp->producer_base = ((u32)msp_res->start & ~0x0f000000) | 0x05000000;
	} else if (ret) {
		dev_err(dev, "invalid brcm,producer-base\n");
		return ret;
	}

	ret = of_property_read_u32(dev->of_node, "brcm,in-low-water-mark", &in_low_water_mark);
	if (ret) {
		dev_err(dev, "missing or invalid brcm,in-low-water-mark\n");
		return ret;
	}
	if (in_low_water_mark > MSP_IN_MSG_CTL_LOW_WM_WORDS_MASK) {
		dev_err(dev, "invalid brcm,in-low-water-mark: %u\n", in_low_water_mark);
		return -EINVAL;
	}

	msp->dqm_node = of_get_child_by_name(dev->of_node, "dqm");
	if (!msp->dqm_node) {
		dev_err(dev, "missing dqm child node\n");
		return -EINVAL;
	}

	ret = of_property_read_u32(dev->of_node,
				   "brcm,4ke-memory-size",
				   &msp->memory_4ke_size);
	if (ret) {
		dev_err(dev, "missing or invalid brcm,4ke-memory-size\n");
		goto put_dqm_node;
	}

	size_t firmware_4ke_size = 0;
	if (msp->memory_4ke_size) {
		const char *firmware_4ke_name;

		ret = of_property_read_string(dev->of_node, "firmware-name",
					      &firmware_4ke_name);
		if (ret) {
			dev_err(dev, "missing or invalid firmware-name\n");
			goto put_dqm_node;
		}

		ret = request_firmware_direct(&msp->firmware_4ke,
					      firmware_4ke_name, dev);
		if (ret) {
			if (ret == -ENOENT)
				ret = -EPROBE_DEFER;
			dev_err_probe(dev, ret, "failed to load MSP 4KE firmware %s\n",
				      firmware_4ke_name);
			goto put_dqm_node;
		}

		if (!msp->firmware_4ke->size) {
			dev_err(dev, "empty MSP 4KE firmware %s\n",
				firmware_4ke_name);
			ret = -EINVAL;
			goto release_firmware;
		}
		firmware_4ke_size = msp->firmware_4ke->size;
	} else {
		dev_info(dev, "MSP 4KE firmware loading disabled\n");
	}

	msp->clk = devm_clk_get_optional(dev, "msp");
	if (IS_ERR(msp->clk)) {
		ret = PTR_ERR(msp->clk);
		goto release_firmware;
	}

	if (msp->clk) {
		ret = clk_prepare_enable(msp->clk);
		if (ret)
			goto release_firmware;
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

	writel_be(smisb_ctrl_value, smisb_ctrl);
	mdelay(10);

	// Disable all MSP interrupts and clear all pending interrupt status bits
	writel_be(0, msp_ctrl(msp, MSP_CTRL_L1_IRQ_4KE_MASK));
	writel_be(~0u, msp_ctrl(msp, MSP_CTRL_L1_IRQ_4KE_STATUS));
	writel_be(0, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_MASK));
	writel_be(~0u, msp_ctrl(msp, MSP_CTRL_L1_IRQ_MIPS_STATUS));
	writel_be(0, msp_ctrl(msp, MSP_CTRL_L2_IRQ_GP_MASK));
	writel_be(~0u, msp_ctrl(msp, MSP_CTRL_L2_IRQ_GP_STATUS));
	writel_be(0, msp_ctrl(msp, MSP_CTRL_GP_IN_IRQ_MASK));
	writel_be(~0u, msp_ctrl(msp, MSP_CTRL_GP_IN_IRQ_STATUS));

	writel_be(0, msp->base + MSP_IN_OFFSET + MSP_IN_MSG_CTL);
	writel_be(~0u, msp->base + MSP_IN_OFFSET + MSP_IN_MSG_STS);
	writel_be(in_low_water_mark, msp->base + MSP_IN_OFFSET + MSP_IN_MSG_CTL);

	writel_be(0, msp->base + MSP_OG_OFFSET + MSP_OG_MSG_CTL);
	writel_be(~0u, msp->base + MSP_OG_OFFSET + MSP_OG_MSG_STS);

	writel_be(0, msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_LOW_WTMK_IRQ_MASK);
	writel_be(0, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_LOW_WTMK_IRQ_MASK);
	writel_be(~0u, msp->base + MSP_DQM_OFFSET + MSP_DQM_LOW_WTMK_IRQ_STS);
	writel_be(0, msp->base + MSP_DQM_OFFSET + MSP_DQM_4KE_NOT_EMPTY_IRQ_MASK);
	writel_be(0, msp->base + MSP_DQM_OFFSET + MSP_DQM_MIPS_NOT_EMPTY_IRQ_MASK);
	writel_be(~0u, msp->base + MSP_DQM_OFFSET + MSP_DQM_NOT_EMPTY_IRQ_STS);

	ret = msp_dqm_init_from_dt(msp);
	if (ret)
		goto assert_reset;

	ret = msp_start_4ke_firmware(msp);
	if (ret)
		goto assert_reset;
	if (msp->firmware_4ke) {
		release_firmware(msp->firmware_4ke);
		msp->firmware_4ke = NULL;
	}

	msp->ready = true;
	platform_set_drvdata(pdev, msp);

	ret = devm_device_add_group(dev, &msp_attr_group);
	if (ret)
		goto clear_ready;

	if (msp->irq >= 0) {
		ret = devm_request_irq(dev, msp->irq, msp_dqm_host_not_empty_irq, 0, dev_name(dev), msp);
		if (ret)
			goto clear_ready;
	}

	dev_info(dev,
		 "BCM3380 MSP ready: inmsg_data_bus=0x%08x dqm_words=%u 4ke_fw=%zu 4ke_mem=%u\n",
		 msp_in_msg_data_bus_addr(msp),
		 msp->dqm_memory_words,
		 firmware_4ke_size, msp->memory_4ke_size);

	return 0;

clear_ready:
	msp->ready = false;
assert_reset:
	msp_assert_4ke_reset(msp);
	msp_free_4ke_firmware(msp);
	if (msp->reset)
		reset_control_assert(msp->reset);
disable_clk:
	if (msp->clk)
		clk_disable_unprepare(msp->clk);
release_firmware:
	if (msp->firmware_4ke) {
		release_firmware(msp->firmware_4ke);
		msp->firmware_4ke = NULL;
	}
put_dqm_node:
	of_node_put(msp->dqm_node);
	return ret;
}

static void msp_remove(struct platform_device *pdev)
{
	struct bcm3380_msp *msp = platform_get_drvdata(pdev);

	msp_dqm_host_not_empty_irq_unregister_all(msp);
	msp->ready = false;
	msp_assert_4ke_reset(msp);
	msp_free_4ke_firmware(msp);
	of_node_put(msp->dqm_node);

	if (msp->reset)
		reset_control_assert(msp->reset);

	if (msp->clk)
		clk_disable_unprepare(msp->clk);
}

static u32 bcm3380_dqm_queue_status(struct bcm3380_msp *msp,
					 unsigned int queue)
{
	// IoprocIoprocDqmQStatus Sts, a single register, offset 0x0C
	return readl_be(msp_qctrl(msp, queue) + 0x0C);
}

static u32 bcm3380_dqm_queue_status_offset(unsigned int queue)
{
	return MSP_DQM_QCTRL_OFFSET + queue * MSP_DQM_QCTRL_STRIDE + 0x0C;
}

static const struct variant_data bcm3380_msp_data = {
	.dqm_queue_status = bcm3380_dqm_queue_status,
	.dqm_queue_status_offset = bcm3380_dqm_queue_status_offset,
	.ioproc_base_4ke = 0xe0000000,
};

static u32 bcm3383_dqm_queue_status(struct bcm3380_msp *msp,
					 unsigned int queue)
{
	// IoprocBlockDqmQueueStatus Dqmqsts starts at offset 0x1f00
	// There are 32 4-byte status registers
	return readl_be(msp->base + 0x1f00 + queue * 0x04);
}

static u32 bcm3383_dqm_queue_status_offset(unsigned int queue)
{
	return 0x1f00 + queue * 0x04;
}

static const struct variant_data bcm3383_msp_data = {
	.dqm_queue_status = bcm3383_dqm_queue_status,
	.dqm_queue_status_offset = bcm3383_dqm_queue_status_offset,
	.ioproc_base_4ke = 0xb6000000,
};

static const struct of_device_id msp_of_match[] = {
	{ .compatible = "brcm,bcm3383-msp", .data = &bcm3383_msp_data },
	{ .compatible = "brcm,bcm3380-msp", .data = &bcm3380_msp_data },
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
