// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012, 2013 DSP Group
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/firmware.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/kfifo.h>
#include <linux/mailbox_client.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/remoteproc.h>
#include <linux/reset.h>
#include <linux/rpmsg/dvf99_coma.h>
#include <linux/workqueue.h>

#define CSS_MAGIC		0xdadadada
#define CSS_IMG_VERSION		0x0100
#define CSS_ABI_VERSION		0x0206
#define CSS_PARTS_MIN		3
#define CSS_PARTS_MAX		4

#define CSS_PM_REQ		GENMASK(7, 0)
#define CSS_PM_INDEX		GENMASK(15, 8)

#define CSS_PM_UNSUPPORTED	(-38)

#define CSS_ARM_RESET_US	2000
#define CSS_SHUTDOWN_US		1000

enum css_mem {
	CSS_MEM_ITCM,
	CSS_MEM_DTCM,
	CSS_MEM_AHBRAM,
	CSS_MEM_NUM,
};

enum css_part {
	CSS_PART_ITCM,
	CSS_PART_AHBRAM,
	CSS_PART_DRAM,
	CSS_PART_ELF,
};

enum css_mbox_id {
	CSS_MBOX_PANIC,
	CSS_MBOX_SHUTDOWN,
	CSS_MBOX_COUNTER,
	CSS_MBOX_CLK_CMD,
	CSS_MBOX_CLK_ARG,
	CSS_MBOX_RST_CMD,
	CSS_MBOX_NUM,
};

enum css_clk_req {
	CSS_CLK_REQ_ENABLE,
	CSS_CLK_REQ_DISABLE,
	CSS_CLK_REQ_SET_RATE,
	CSS_CLK_REQ_GET_RATE,
	CSS_CLK_REQ_ROUND_RATE,
};

enum css_rst_req {
	CSS_RST_REQ_ASSERT,
	CSS_RST_REQ_DEASSERT,
};

struct css_img_hdr {
	__le32 magic;
	__le32 parts;
	__le32 dc_addr;
	__le32 boot_config;
	__le32 elf_md5_addr;
	__le16 abi_version;
	__le16 img_version;
	char info[52];
} __packed;

struct css_part_hdr {
	__le32 size;
	__le32 rsize;
	__le32 dest_addr;
	u8 md5[16];
} __packed;

struct css_boot_config {
	__le32 dram_addr;
	__le32 dram_size;
	__le32 uart;
	__le32 cfifo_l2c;
	__le32 cfifo_c2l;
} __packed;

struct css_mem_region {
	void __iomem *va;
	phys_addr_t pa;
	size_t size;
};

struct css_mbox {
	struct mbox_client client;
	struct mbox_chan *chan;
	u32 data;
};

static const char * const css_mem_names[CSS_MEM_NUM] = {
	[CSS_MEM_ITCM] = "itcm",
	[CSS_MEM_DTCM] = "dtcm",
	[CSS_MEM_AHBRAM] = "ahbram",
};

static const char * const css_mbox_names[CSS_MBOX_NUM] = {
	[CSS_MBOX_PANIC] = "panic",
	[CSS_MBOX_SHUTDOWN] = "shutdown",
	[CSS_MBOX_COUNTER] = "counter",
	[CSS_MBOX_CLK_CMD] = "clk-cmd",
	[CSS_MBOX_CLK_ARG] = "clk-arg",
	[CSS_MBOX_RST_CMD] = "rst-cmd",
};

static const char * const css_pm_clk_names[] = {
	"adpcm", "bmp", "drt", "rfapu", "gdmac", "timer1", "timer2", "uart",
	"rtc", "clkout", "dclass", "tdm1", "tdm2", "apu", "pmu",
};

static const char * const css_pm_rst_names[] = {
	"adpcm", "bmp", "drt", "rfapu", "gdmac", "timers", "uart", "dclass",
	"tdm1", "tdm2",
};

#define CSS_PM_CLK_NUM		ARRAY_SIZE(css_pm_clk_names)
#define CSS_PM_CLK_RESET_NUM	4
#define CSS_PM_RST_NUM		ARRAY_SIZE(css_pm_rst_names)
#define CSS_RST_CMDS		16

struct dvf99_css {
	struct device *dev;
	struct rproc *rproc;
	struct css_mem_region mem[CSS_MEM_NUM];

	void *dram_va;
	dma_addr_t dram_pa;
	size_t dram_size;
	u32 boot_config;

	struct clk *clk_css;
	struct clk *clk_arm;
	struct clk *clk_etm;
	struct reset_control *rst_css;
	struct reset_control *rst_arm;
	struct reset_control *rst_etm;
	struct gpio_desc *rf_reset;
	s32 uart;

	struct clk *pm_clk[CSS_PM_CLK_NUM];
	unsigned int pm_clk_count[CSS_PM_CLK_NUM];
	struct reset_control *pm_rst[CSS_PM_RST_NUM];
	struct mutex pm_lock;
	spinlock_t pm_clk_lock;

	struct css_mbox mbox[CSS_MBOX_NUM];
	struct work_struct panic_work;
	struct work_struct clk_work;
	struct work_struct rst_work;
	u32 clk_cmd;
	DECLARE_KFIFO(rst_cmds, u32, CSS_RST_CMDS);

	struct dvf99_coma *coma;
	struct rproc_subdev coma_subdev;
};

static int css_mbox_send(struct dvf99_css *css, enum css_mbox_id id, u32 data)
{
	struct mbox_chan *chan = css->mbox[id].chan;
	int ret;

	ret = mbox_send_message(chan, &data);
	if (ret < 0)
		return ret;

	mbox_client_txdone(chan, 0);

	return 0;
}

static u32 css_mbox_read(struct dvf99_css *css, enum css_mbox_id id)
{
	mbox_client_peek_data(css->mbox[id].chan);

	return READ_ONCE(css->mbox[id].data);
}

static bool css_pm_clk_atomic(struct dvf99_css *css, u32 cmd)
{
	unsigned int idx = FIELD_GET(CSS_PM_INDEX, cmd);
	unsigned int req = FIELD_GET(CSS_PM_REQ, cmd);
	unsigned long flags;
	int ret = 0;

	if (req != CSS_CLK_REQ_ENABLE && req != CSS_CLK_REQ_DISABLE)
		return false;

	if (idx >= CSS_PM_CLK_NUM || !css->pm_clk[idx]) {
		css_mbox_send(css, CSS_MBOX_CLK_CMD, (u32)CSS_PM_UNSUPPORTED);
		return true;
	}

	spin_lock_irqsave(&css->pm_clk_lock, flags);
	if (req == CSS_CLK_REQ_ENABLE) {
		ret = clk_enable(css->pm_clk[idx]);
		if (!ret)
			css->pm_clk_count[idx]++;
	} else if (css->pm_clk_count[idx]) {
		clk_disable(css->pm_clk[idx]);
		css->pm_clk_count[idx]--;
	} else {
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&css->pm_clk_lock, flags);

	css_mbox_send(css, CSS_MBOX_CLK_CMD, (u32)ret);

	return true;
}

static void css_mbox_rx(struct mbox_client *client, void *msg)
{
	struct css_mbox *mbox = container_of(client, struct css_mbox, client);
	struct dvf99_css *css = dev_get_drvdata(client->dev);
	enum css_mbox_id id = mbox - css->mbox;
	u32 data = *(u32 *)msg;

	WRITE_ONCE(mbox->data, data);

	switch (id) {
	case CSS_MBOX_PANIC:
		schedule_work(&css->panic_work);
		break;
	case CSS_MBOX_CLK_CMD:
		if (css_pm_clk_atomic(css, data))
			break;
		css->clk_cmd = data;
		schedule_work(&css->clk_work);
		break;
	case CSS_MBOX_RST_CMD:
		if (!kfifo_put(&css->rst_cmds, data))
			dev_err_ratelimited(client->dev, "reset request lost\n");
		schedule_work(&css->rst_work);
		break;
	default:
		break;
	}
}

static void css_panic_work(struct work_struct *work)
{
	struct dvf99_css *css = container_of(work, struct dvf99_css,
					     panic_work);

	gpiod_set_value_cansleep(css->rf_reset, 1);
	rproc_report_crash(css->rproc, RPROC_FATAL_ERROR);
}

static long css_pm_clk_handle(struct dvf99_css *css, unsigned int idx,
			      unsigned int req)
{
	struct clk *clk;

	if (idx >= CSS_PM_CLK_NUM || !css->pm_clk[idx])
		return CSS_PM_UNSUPPORTED;

	clk = css->pm_clk[idx];

	switch (req) {
	case CSS_CLK_REQ_SET_RATE:
		return clk_set_rate(clk, css_mbox_read(css, CSS_MBOX_CLK_ARG));
	case CSS_CLK_REQ_GET_RATE:
		return clk_get_rate(clk);
	case CSS_CLK_REQ_ROUND_RATE:
		return clk_round_rate(clk, css_mbox_read(css, CSS_MBOX_CLK_ARG));
	}

	return CSS_PM_UNSUPPORTED;
}

static void css_clk_work(struct work_struct *work)
{
	struct dvf99_css *css = container_of(work, struct dvf99_css, clk_work);
	u32 cmd = READ_ONCE(css->clk_cmd);
	long ret;

	mutex_lock(&css->pm_lock);
	ret = css_pm_clk_handle(css, FIELD_GET(CSS_PM_INDEX, cmd),
				FIELD_GET(CSS_PM_REQ, cmd));
	mutex_unlock(&css->pm_lock);

	css_mbox_send(css, CSS_MBOX_CLK_CMD, (u32)ret);
}

static void css_pm_rst_handle(struct dvf99_css *css, u32 cmd)
{
	unsigned int idx = FIELD_GET(CSS_PM_INDEX, cmd);
	struct reset_control *rst;

	if (idx >= CSS_PM_RST_NUM || !css->pm_rst[idx])
		return;

	rst = css->pm_rst[idx];

	mutex_lock(&css->pm_lock);
	switch (FIELD_GET(CSS_PM_REQ, cmd)) {
	case CSS_RST_REQ_ASSERT:
		reset_control_assert(rst);
		break;
	case CSS_RST_REQ_DEASSERT:
		reset_control_deassert(rst);
		break;
	}
	mutex_unlock(&css->pm_lock);
}

static void css_rst_work(struct work_struct *work)
{
	struct dvf99_css *css = container_of(work, struct dvf99_css, rst_work);
	u32 cmd;

	while (kfifo_get(&css->rst_cmds, &cmd))
		css_pm_rst_handle(css, cmd);
}

static int css_pm_prepare(struct dvf99_css *css)
{
	unsigned int i;
	int ret;

	for (i = 0; i < CSS_PM_CLK_NUM; i++) {
		ret = clk_prepare(css->pm_clk[i]);
		if (ret)
			goto err;
	}

	return 0;

err:
	while (i--)
		clk_unprepare(css->pm_clk[i]);

	return ret;
}

static void css_pm_release(struct dvf99_css *css)
{
	unsigned long flags;
	unsigned int i;

	mutex_lock(&css->pm_lock);

	spin_lock_irqsave(&css->pm_clk_lock, flags);
	for (i = 0; i < CSS_PM_CLK_NUM; i++)
		for (; css->pm_clk_count[i]; css->pm_clk_count[i]--)
			clk_disable(css->pm_clk[i]);
	spin_unlock_irqrestore(&css->pm_clk_lock, flags);

	for (i = 0; i < CSS_PM_RST_NUM; i++)
		reset_control_assert(css->pm_rst[i]);

	for (i = 0; i < CSS_PM_CLK_RESET_NUM; i++)
		clk_enable(css->pm_clk[i]);

	usleep_range(CSS_SHUTDOWN_US, 2 * CSS_SHUTDOWN_US);

	for (i = 0; i < CSS_PM_CLK_RESET_NUM; i++)
		clk_disable(css->pm_clk[i]);

	mutex_unlock(&css->pm_lock);
}

static void css_pm_unprepare(struct dvf99_css *css)
{
	unsigned int i;

	for (i = 0; i < CSS_PM_CLK_NUM; i++)
		clk_unprepare(css->pm_clk[i]);
}

static int css_tcm_write_begin(struct dvf99_css *css, unsigned long *rate)
{
	*rate = clk_get_rate(css->clk_css);

	return clk_set_rate(css->clk_css,
			    clk_round_rate(css->clk_css, *rate / 2));
}

static void css_tcm_write_end(struct dvf99_css *css, unsigned long rate)
{
	clk_set_rate(css->clk_css, rate);
}

static int dvf99_css_prepare(struct rproc *rproc)
{
	struct dvf99_css *css = rproc->priv;
	int ret;

	ret = reset_control_assert(css->rst_arm);
	if (ret)
		return ret;

	ret = css_pm_prepare(css);
	if (ret)
		return ret;

	ret = reset_control_deassert(css->rst_css);
	if (ret)
		goto err_pm;

	ret = clk_prepare_enable(css->clk_css);
	if (ret)
		goto err_rst_css;

	ret = reset_control_deassert(css->rst_etm);
	if (ret)
		goto err_clk_css;

	ret = clk_prepare_enable(css->clk_etm);
	if (ret)
		goto err_rst_etm;

	ret = clk_prepare_enable(css->clk_arm);
	if (ret)
		goto err_clk_etm;

	ret = css_mbox_send(css, CSS_MBOX_SHUTDOWN, 0);
	if (ret)
		goto err_clk_arm;

	gpiod_set_value_cansleep(css->rf_reset, 0);

	return 0;

err_clk_arm:
	clk_disable_unprepare(css->clk_arm);
err_clk_etm:
	clk_disable_unprepare(css->clk_etm);
err_rst_etm:
	reset_control_assert(css->rst_etm);
err_clk_css:
	clk_disable_unprepare(css->clk_css);
err_rst_css:
	reset_control_assert(css->rst_css);
err_pm:
	css_pm_unprepare(css);

	return ret;
}

static int dvf99_css_unprepare(struct rproc *rproc)
{
	struct dvf99_css *css = rproc->priv;

	gpiod_set_value_cansleep(css->rf_reset, 1);
	css_pm_unprepare(css);

	clk_disable_unprepare(css->clk_arm);
	clk_disable_unprepare(css->clk_etm);
	reset_control_assert(css->rst_etm);
	clk_disable_unprepare(css->clk_css);
	reset_control_assert(css->rst_css);

	clk_prepare_enable(css->clk_css);
	usleep_range(CSS_ARM_RESET_US, 2 * CSS_ARM_RESET_US);
	clk_disable_unprepare(css->clk_css);

	return 0;
}

static const struct css_part_hdr *css_part_get(const struct firmware *fw,
					       size_t *offset)
{
	const struct css_part_hdr *part;

	if (fw->size - *offset < sizeof(*part))
		return NULL;

	part = (const struct css_part_hdr *)(fw->data + *offset);
	if (fw->size - *offset - sizeof(*part) < le32_to_cpu(part->size))
		return NULL;

	*offset += sizeof(*part) + le32_to_cpu(part->size);

	return part;
}

static bool css_range_valid(u32 addr, u32 size, phys_addr_t base,
			    size_t limit)
{
	return addr >= base && addr - base <= limit &&
	       size <= limit - (addr - base);
}

static int dvf99_css_parse(struct dvf99_css *css, const struct firmware *fw,
			   const struct css_part_hdr **parts)
{
	const struct css_img_hdr *hdr = (const void *)fw->data;
	struct css_mem_region *itcm = &css->mem[CSS_MEM_ITCM];
	struct css_mem_region *ahbram = &css->mem[CSS_MEM_AHBRAM];
	struct device *dev = css->dev;
	size_t offset = sizeof(*hdr);
	u32 nparts, dram_size, i;

	if (fw->size < sizeof(*hdr) || le32_to_cpu(hdr->magic) != CSS_MAGIC) {
		dev_err(dev, "invalid image magic\n");
		return -EINVAL;
	}

	if (le16_to_cpu(hdr->img_version) != CSS_IMG_VERSION ||
	    le16_to_cpu(hdr->abi_version) != CSS_ABI_VERSION) {
		dev_err(dev, "unsupported image v%x abi v%x\n",
			le16_to_cpu(hdr->img_version),
			le16_to_cpu(hdr->abi_version));
		return -EINVAL;
	}

	nparts = le32_to_cpu(hdr->parts);
	if (nparts < CSS_PARTS_MIN || nparts > CSS_PARTS_MAX) {
		dev_err(dev, "invalid number of parts: %u\n", nparts);
		return -EINVAL;
	}

	for (i = 0; i < CSS_PARTS_MAX; i++)
		parts[i] = NULL;

	for (i = 0; i < nparts; i++) {
		parts[i] = css_part_get(fw, &offset);
		if (!parts[i]) {
			dev_err(dev, "part %u is truncated\n", i);
			return -EINVAL;
		}
	}

	if (!css_range_valid(le32_to_cpu(parts[CSS_PART_ITCM]->dest_addr),
			     le32_to_cpu(parts[CSS_PART_ITCM]->size),
			     itcm->pa, itcm->size)) {
		dev_err(dev, "ITCM part out of range\n");
		return -EINVAL;
	}

	if (le32_to_cpu(parts[CSS_PART_AHBRAM]->size) &&
	    !css_range_valid(le32_to_cpu(parts[CSS_PART_AHBRAM]->dest_addr),
			     le32_to_cpu(parts[CSS_PART_AHBRAM]->size),
			     ahbram->pa, ahbram->size)) {
		dev_err(dev, "AHB RAM part out of range\n");
		return -EINVAL;
	}

	dram_size = le32_to_cpu(parts[CSS_PART_DRAM]->rsize);
	if (!css_range_valid(le32_to_cpu(parts[CSS_PART_DRAM]->dest_addr),
			     le32_to_cpu(parts[CSS_PART_DRAM]->size),
			     0, dram_size) ||
	    !css_range_valid(le32_to_cpu(hdr->dc_addr), sizeof(u32), 0,
			     dram_size) ||
	    !css_range_valid(le32_to_cpu(hdr->elf_md5_addr),
			     sizeof(parts[0]->md5), 0, dram_size)) {
		dev_err(dev, "DRAM part out of range\n");
		return -EINVAL;
	}

	if (!css_range_valid(itcm->pa + le32_to_cpu(hdr->boot_config),
			     sizeof(struct css_boot_config), itcm->pa,
			     itcm->size)) {
		dev_err(dev, "boot config out of range\n");
		return -EINVAL;
	}

	return 0;
}

static int dvf99_css_sanity_check(struct rproc *rproc,
				  const struct firmware *fw)
{
	const struct css_part_hdr *parts[CSS_PARTS_MAX];

	return dvf99_css_parse(rproc->priv, fw, parts);
}

static int css_dram_alloc(struct dvf99_css *css, size_t size)
{
	if (css->dram_va && css->dram_size >= size)
		return 0;

	if (css->dram_va)
		dma_free_coherent(css->dev, css->dram_size, css->dram_va,
				  css->dram_pa);

	css->dram_va = dma_alloc_coherent(css->dev, size, &css->dram_pa,
					  GFP_KERNEL);
	if (!css->dram_va) {
		css->dram_size = 0;
		return -ENOMEM;
	}

	css->dram_size = size;

	return 0;
}

static int dvf99_css_load(struct rproc *rproc, const struct firmware *fw)
{
	const struct css_img_hdr *hdr = (const void *)fw->data;
	struct dvf99_css *css = rproc->priv;
	struct css_mem_region *itcm = &css->mem[CSS_MEM_ITCM];
	struct css_mem_region *ahbram = &css->mem[CSS_MEM_AHBRAM];
	const struct css_part_hdr *parts[CSS_PARTS_MAX];
	const struct css_part_hdr *part;
	unsigned long rate;
	int ret;

	ret = dvf99_css_parse(css, fw, parts);
	if (ret)
		return ret;

	dev_info(css->dev, "loading %.*s\n", (int)sizeof(hdr->info),
		 hdr->info);

	ret = css_dram_alloc(css, le32_to_cpu(parts[CSS_PART_DRAM]->rsize));
	if (ret)
		return ret;

	ret = css_tcm_write_begin(css, &rate);
	if (ret)
		return ret;

	memset_io(itcm->va, 0, itcm->size);
	memset_io(ahbram->va, 0, ahbram->size);
	memset(css->dram_va, 0, css->dram_size);

	part = parts[CSS_PART_ITCM];
	memcpy_toio(itcm->va + le32_to_cpu(part->dest_addr) - itcm->pa,
		    part + 1, le32_to_cpu(part->size));

	part = parts[CSS_PART_AHBRAM];
	if (le32_to_cpu(part->size))
		memcpy_toio(ahbram->va + le32_to_cpu(part->dest_addr) -
			    ahbram->pa, part + 1, le32_to_cpu(part->size));

	css_tcm_write_end(css, rate);

	part = parts[CSS_PART_DRAM];
	memcpy(css->dram_va + le32_to_cpu(part->dest_addr), part + 1,
	       le32_to_cpu(part->size));

	part = parts[CSS_PART_ELF];
	if (part)
		memcpy(css->dram_va + le32_to_cpu(hdr->elf_md5_addr),
		       part->md5, sizeof(part->md5));

	css->boot_config = le32_to_cpu(hdr->boot_config);

	return 0;
}

static int dvf99_css_start(struct rproc *rproc)
{
	struct dvf99_css *css = rproc->priv;
	struct css_boot_config cfg;
	dma_addr_t l2c, c2l;
	unsigned long rate;
	int ret;

	dvf99_coma_fifos_get(css->coma, &l2c, &c2l);

	cfg.dram_addr = cpu_to_le32(css->dram_pa);
	cfg.dram_size = cpu_to_le32(css->dram_size);
	cfg.uart = cpu_to_le32(css->uart);
	cfg.cfifo_l2c = cpu_to_le32(l2c);
	cfg.cfifo_c2l = cpu_to_le32(c2l);

	ret = css_tcm_write_begin(css, &rate);
	if (ret)
		return ret;

	memcpy_toio(css->mem[CSS_MEM_ITCM].va + css->boot_config, &cfg,
		    sizeof(cfg));

	css_tcm_write_end(css, rate);

	clk_disable_unprepare(css->clk_arm);
	ret = reset_control_deassert(css->rst_arm);
	if (!ret)
		ret = clk_prepare_enable(css->clk_arm);
	if (ret)
		return ret;

	usleep_range(CSS_ARM_RESET_US, 2 * CSS_ARM_RESET_US);

	return 0;
}

static int dvf99_css_stop(struct rproc *rproc)
{
	struct dvf99_css *css = rproc->priv;

	css_mbox_send(css, CSS_MBOX_SHUTDOWN, 1);
	usleep_range(CSS_SHUTDOWN_US, 2 * CSS_SHUTDOWN_US);

	clk_disable_unprepare(css->clk_arm);
	clk_disable_unprepare(css->clk_etm);
	reset_control_assert(css->rst_arm);
	reset_control_assert(css->rst_etm);
	clk_prepare_enable(css->clk_etm);
	clk_prepare_enable(css->clk_arm);
	usleep_range(CSS_SHUTDOWN_US, 2 * CSS_SHUTDOWN_US);

	css_pm_release(css);

	return 0;
}

static void *dvf99_css_da_to_va(struct rproc *rproc, u64 da, size_t len,
				bool *is_iomem)
{
	struct dvf99_css *css = rproc->priv;
	unsigned int i;

	for (i = 0; i < CSS_MEM_NUM; i++) {
		struct css_mem_region *mem = &css->mem[i];

		if (!css_range_valid(da, len, mem->pa, mem->size))
			continue;

		if (is_iomem)
			*is_iomem = true;

		return (void __force *)mem->va + (da - mem->pa);
	}

	return NULL;
}

static const struct rproc_ops dvf99_css_ops = {
	.prepare = dvf99_css_prepare,
	.unprepare = dvf99_css_unprepare,
	.start = dvf99_css_start,
	.stop = dvf99_css_stop,
	.da_to_va = dvf99_css_da_to_va,
	.sanity_check = dvf99_css_sanity_check,
	.load = dvf99_css_load,
};

static int css_coma_prepare(struct rproc_subdev *subdev)
{
	struct dvf99_css *css = container_of(subdev, struct dvf99_css,
					     coma_subdev);

	return dvf99_coma_prepare(css->coma);
}

static int css_coma_start(struct rproc_subdev *subdev)
{
	struct dvf99_css *css = container_of(subdev, struct dvf99_css,
					     coma_subdev);

	return dvf99_coma_start(css->coma);
}

static void css_coma_stop(struct rproc_subdev *subdev, bool crashed)
{
	struct dvf99_css *css = container_of(subdev, struct dvf99_css,
					     coma_subdev);

	dvf99_coma_stop(css->coma, crashed);
}

static void css_coma_unprepare(struct rproc_subdev *subdev)
{
	struct dvf99_css *css = container_of(subdev, struct dvf99_css,
					     coma_subdev);

	dvf99_coma_unprepare(css->coma);
}

static int css_mem_init(struct dvf99_css *css, struct platform_device *pdev)
{
	struct resource *res;
	unsigned int i;

	for (i = 0; i < CSS_MEM_NUM; i++) {
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
						   css_mem_names[i]);
		css->mem[i].va = devm_ioremap_resource(css->dev, res);
		if (IS_ERR(css->mem[i].va))
			return PTR_ERR(css->mem[i].va);

		css->mem[i].pa = res->start;
		css->mem[i].size = resource_size(res);
	}

	return 0;
}

static void css_mbox_free(void *data)
{
	struct dvf99_css *css = data;
	unsigned int i;

	for (i = 0; i < CSS_MBOX_NUM; i++)
		if (!IS_ERR_OR_NULL(css->mbox[i].chan))
			mbox_free_channel(css->mbox[i].chan);
}

static int css_mbox_init(struct dvf99_css *css)
{
	struct device *dev = css->dev;
	unsigned int i;
	int ret;

	ret = devm_add_action(dev, css_mbox_free, css);
	if (ret)
		return ret;

	for (i = 0; i < CSS_MBOX_NUM; i++) {
		struct css_mbox *mbox = &css->mbox[i];

		mbox->client.dev = dev;
		mbox->client.rx_callback = css_mbox_rx;
		mbox->client.knows_txdone = true;
		mbox->chan = mbox_request_channel_byname(&mbox->client,
							 css_mbox_names[i]);
		if (IS_ERR(mbox->chan))
			return dev_err_probe(dev, PTR_ERR(mbox->chan),
					     "no %s mailbox\n",
					     css_mbox_names[i]);
	}

	return 0;
}

static int css_pm_init(struct dvf99_css *css)
{
	struct device *dev = css->dev;
	unsigned int i;

	for (i = 0; i < CSS_PM_CLK_NUM; i++) {
		css->pm_clk[i] = devm_clk_get_optional(dev,
						       css_pm_clk_names[i]);
		if (IS_ERR(css->pm_clk[i]))
			return dev_err_probe(dev, PTR_ERR(css->pm_clk[i]),
					     "bad %s clock\n",
					     css_pm_clk_names[i]);
	}

	for (i = 0; i < CSS_PM_RST_NUM; i++) {
		css->pm_rst[i] = devm_reset_control_get_optional_exclusive(dev,
						css_pm_rst_names[i]);
		if (IS_ERR(css->pm_rst[i]))
			return dev_err_probe(dev, PTR_ERR(css->pm_rst[i]),
					     "bad %s reset\n",
					     css_pm_rst_names[i]);
	}

	return 0;
}

static void css_reserved_mem_release(void *data)
{
	of_reserved_mem_device_release(data);
}

static void css_dram_free(void *data)
{
	struct dvf99_css *css = data;

	if (css->dram_va)
		dma_free_coherent(css->dev, css->dram_size, css->dram_va,
				  css->dram_pa);
}

static void css_work_cancel(void *data)
{
	struct dvf99_css *css = data;

	cancel_work_sync(&css->panic_work);
	cancel_work_sync(&css->clk_work);
	cancel_work_sync(&css->rst_work);
}

static int dvf99_css_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const char *fw_name = "css-loader";
	struct dvf99_css *css;
	struct rproc *rproc;
	u32 uart;
	int ret;

	of_property_read_string(dev->of_node, "firmware-name", &fw_name);

	rproc = devm_rproc_alloc(dev, dev_name(dev), &dvf99_css_ops, fw_name,
				 sizeof(*css));
	if (!rproc)
		return -ENOMEM;

	rproc->auto_boot = false;
	rproc->recovery_disabled = true;

	css = rproc->priv;
	css->dev = dev;
	css->rproc = rproc;
	css->uart = of_property_read_u32(dev->of_node, "dspg,uart", &uart) ?
		    -1 : uart;
	platform_set_drvdata(pdev, css);

	INIT_WORK(&css->panic_work, css_panic_work);
	INIT_WORK(&css->clk_work, css_clk_work);
	INIT_WORK(&css->rst_work, css_rst_work);
	INIT_KFIFO(css->rst_cmds);

	spin_lock_init(&css->pm_clk_lock);

	ret = devm_mutex_init(dev, &css->pm_lock);
	if (ret)
		return ret;

	ret = css_mem_init(css, pdev);
	if (ret)
		return ret;

	ret = of_reserved_mem_device_init(dev);
	if (ret)
		return dev_err_probe(dev, ret, "no reserved memory\n");

	ret = devm_add_action_or_reset(dev, css_reserved_mem_release, dev);
	if (ret)
		return ret;

	ret = devm_add_action(dev, css_dram_free, css);
	if (ret)
		return ret;

	css->clk_css = devm_clk_get(dev, "css");
	if (IS_ERR(css->clk_css))
		return dev_err_probe(dev, PTR_ERR(css->clk_css), "no css clock\n");

	css->clk_arm = devm_clk_get(dev, "arm");
	if (IS_ERR(css->clk_arm))
		return dev_err_probe(dev, PTR_ERR(css->clk_arm), "no arm clock\n");

	css->clk_etm = devm_clk_get(dev, "etm");
	if (IS_ERR(css->clk_etm))
		return dev_err_probe(dev, PTR_ERR(css->clk_etm), "no etm clock\n");

	css->rst_css = devm_reset_control_get_exclusive(dev, "css");
	if (IS_ERR(css->rst_css))
		return dev_err_probe(dev, PTR_ERR(css->rst_css), "no css reset\n");

	css->rst_arm = devm_reset_control_get_exclusive(dev, "arm");
	if (IS_ERR(css->rst_arm))
		return dev_err_probe(dev, PTR_ERR(css->rst_arm), "no arm reset\n");

	css->rst_etm = devm_reset_control_get_exclusive(dev, "etm");
	if (IS_ERR(css->rst_etm))
		return dev_err_probe(dev, PTR_ERR(css->rst_etm), "no etm reset\n");

	css->rf_reset = devm_gpiod_get_optional(dev, "rf-reset",
						GPIOD_OUT_HIGH);
	if (IS_ERR(css->rf_reset))
		return dev_err_probe(dev, PTR_ERR(css->rf_reset),
				     "bad rf-reset gpio\n");

	ret = css_pm_init(css);
	if (ret)
		return ret;

	ret = css_mbox_init(css);
	if (ret)
		return ret;

	ret = devm_add_action(dev, css_work_cancel, css);
	if (ret)
		return ret;

	css->coma = devm_dvf99_coma_create(dev);
	if (IS_ERR(css->coma))
		return dev_err_probe(dev, PTR_ERR(css->coma), "no coma\n");

	css->coma_subdev.prepare = css_coma_prepare;
	css->coma_subdev.start = css_coma_start;
	css->coma_subdev.stop = css_coma_stop;
	css->coma_subdev.unprepare = css_coma_unprepare;
	rproc_add_subdev(rproc, &css->coma_subdev);

	return devm_rproc_add(dev, rproc);
}

static const struct of_device_id dvf99_css_of_match[] = {
	{ .compatible = "dspg,dvf99-css" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_css_of_match);

static struct platform_driver dvf99_css_driver = {
	.probe = dvf99_css_probe,
	.driver = {
		.name = "dvf99-css",
		.of_match_table = dvf99_css_of_match,
	},
};
module_platform_driver(dvf99_css_driver);

MODULE_DESCRIPTION("DSPG DVF99 cordless subsystem remoteproc driver");
MODULE_LICENSE("GPL");
