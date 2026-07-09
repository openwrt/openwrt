/*
 * Annapurna Labs DMA Linux driver core
 * Copyright(c) 2011 Annapurna Labs.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include <mach/al_fabric.h>
#include <mach/al_hal_udma_iofic.h>
#include <mach/al_hal_udma_config.h>
#include <mach/al_hal_ssm_crc_memcpy.h>
#include <mach/al_hal_udma_fast.h>
#include <mach/al_hal_udma_debug.h>
#include <mach/al_hal_iomap.h>
#include <mach/al_hal_plat_services.h>
#include <mach/al_hal_unit_adapter_regs.h>

#include "../dmaengine.h"
#include "al_dma.h"
#include "al_dma_prep.h"
#include "al_dma_sysfs.h"
#include "al_dma_module_params.h"

MODULE_LICENSE("GPL");

static dma_cookie_t al_dma_tx_submit_unlock(
	struct dma_async_tx_descriptor *tx);

static void al_dma_free_chan_resources(
	struct dma_chan *c);

static int al_dma_alloc_chan_resources(
	struct dma_chan *c);

static enum dma_status al_dma_tx_status(
	struct dma_chan *c,
	dma_cookie_t cookie,
	struct dma_tx_state *txstate);

static void al_dma_issue_pending(
	struct dma_chan *c);

static int al_dma_device_terminate_all(struct dma_chan *dchan);

static int al_dma_device_config(struct dma_chan *dchan,
				 struct dma_slave_config *config);


static int al_dma_setup_interrupts(
	struct al_dma_device *device);

static void al_dma_free_interrupts(
	struct al_dma_device *device);

static irqreturn_t al_dma_do_interrupt(
	int	irq,
	void	*data);

static irqreturn_t al_dma_do_interrupt_msix(
	int	irq,
	void	*data);

static int al_dma_init_channels(
	struct al_dma_device	*device,
	int			max_channels);

static void al_dma_init_channel(
	struct al_dma_device	*device,
	struct al_dma_chan	*chan,
	int			idx);

static struct al_dma_sw_desc **al_dma_alloc_sw_ring(
	struct al_dma_chan	*chan,
	int			order,
	gfp_t			flags);

static struct al_dma_sw_desc *al_dma_alloc_ring_ent(
	struct al_dma_chan	*chan,
	gfp_t			flags);

static void al_dma_free_ring_ent(
	struct al_dma_sw_desc	*desc,
	struct al_dma_chan	*chan);

static void al_dma_cleanup_tasklet(unsigned long data);

/******************************************************************************
 *****************************************************************************/
int al_dma_core_init(
	struct al_dma_device	*device,
	void __iomem		*iobase_udma,
	void __iomem		*iobase_app)
{
	int32_t rc;

	struct dma_device *dma = &device->common;
	int err;
	struct al_udma_m2s_pkt_len_conf pkt_len_conf;
	struct al_udma *tx_udma;
	int max_channels = al_dma_get_max_channels();

	dev_dbg(
		dma->dev,
		"%s(%p, %p, %p)\n",
		__func__,
		device,
		iobase_udma,
		iobase_app);

	device->cache = kmem_cache_create(
		"al_dma",
		sizeof(struct al_dma_sw_desc),
		0,
		SLAB_HWCACHE_ALIGN,
		NULL);
	if (!device->cache)
		return -ENOMEM;

	device->max_channels = max_channels;

	device->udma_regs_base = iobase_udma;
	device->app_regs_base = iobase_app;

	memset(&device->ssm_dma_params, 0, sizeof(struct al_ssm_dma_params));
	device->ssm_dma_params.dev_id = device->dev_id;
	device->ssm_dma_params.rev_id = device->rev_id;
	device->ssm_dma_params.udma_regs_base = device->udma_regs_base;

	device->ssm_dma_params.name =
		kmalloc(strlen(dev_name(device->common.dev)) + 1, GFP_KERNEL);
	if (device->ssm_dma_params.name == NULL) {
		dev_err(device->common.dev, "kmalloc failed\n");
		return -1;
	}

	memcpy(
		device->ssm_dma_params.name,
		dev_name(device->common.dev),
		strlen(dev_name(device->common.dev)) + 1);

	device->ssm_dma_params.num_of_queues = max_channels;

	rc = al_ssm_dma_init(&device->hal_raid, &device->ssm_dma_params);
	if (rc) {
		dev_err(device->common.dev, "al_raid_dma_init failed\n");
		return rc;
	}

	al_raid_init(device->app_regs_base);

	/* set max packet size to 512k (XOR with 32 sources) */
	rc = al_ssm_dma_handle_get(
		&device->hal_raid,
		UDMA_TX,
		&tx_udma);
	if (rc) {
		dev_err(device->common.dev, "al_raid_dma_handle_get failed\n");
		return rc;
	}

	pkt_len_conf.encode_64k_as_zero = AL_FALSE;
	pkt_len_conf.max_pkt_size = SZ_512K;
	rc = al_udma_m2s_packet_size_cfg_set(tx_udma, &pkt_len_conf);
	if (rc) {
		dev_err(device->common.dev,
			"al_udma_m2s_packet_size_cfg_set failed\n");
		return rc;
	}

	/* enumerate and initialize channels (queues) */
	al_dma_init_channels(device, max_channels);

	/* enable RAID DMA engine */
	rc = al_ssm_dma_state_set(&device->hal_raid, UDMA_NORMAL);

	dma->dev = &device->pdev->dev;

	dma->device_alloc_chan_resources = al_dma_alloc_chan_resources;
	dma->device_free_chan_resources = al_dma_free_chan_resources;
	dma->device_tx_status = al_dma_tx_status;
	dma->device_issue_pending = al_dma_issue_pending;
	dma->device_config = al_dma_device_config;
	dma->device_terminate_all = al_dma_device_terminate_all;

	if (al_dma_get_op_support_interrupt()) {
		dma_cap_set(DMA_INTERRUPT, dma->cap_mask);
		dma->device_prep_dma_interrupt = al_dma_prep_interrupt_lock;
	}

	if (al_dma_get_op_support_memcpy()) {
		dma_cap_set(DMA_MEMCPY, dma->cap_mask);
		dma->device_prep_dma_memcpy = al_dma_prep_memcpy_lock;
	}

	if (al_dma_get_op_support_memset()) {
		dma_cap_set(DMA_MEMSET, dma->cap_mask);
		dma->device_prep_dma_memset = al_dma_prep_memset_lock;
	}

	if (al_dma_get_op_support_xor()) {
		dma_cap_set(DMA_XOR, dma->cap_mask);
		dma->device_prep_dma_xor = al_dma_prep_xor_lock;
		dma->max_xor = AL_DMA_MAX_XOR;
	}

	if (al_dma_get_op_support_pq()) {
		dma_cap_set(DMA_PQ, dma->cap_mask);
		dma->device_prep_dma_pq = al_dma_prep_pq_lock;
	}

	if (al_dma_get_op_support_pq_val()) {
#ifndef CONFIG_ASYNC_TX_DISABLE_PQ_VAL_DMA
		dma_cap_set(DMA_PQ_VAL, dma->cap_mask);
		dma->device_prep_dma_pq_val = al_dma_prep_pq_val_lock;
#endif
	}

	if (al_dma_get_op_support_pq())
		dma_set_maxpq(dma, AL_DMA_MAX_XOR - 2, 0);
	else if (al_dma_get_op_support_pq_val()) {
#ifndef CONFIG_ASYNC_TX_DISABLE_PQ_VAL_DMA
		dma_set_maxpq(dma, AL_DMA_MAX_XOR - 2, 0);
#endif
	}

	if (al_dma_get_op_support_xor_val()) {
#ifndef CONFIG_ASYNC_TX_DISABLE_XOR_VAL_DMA
		dma_cap_set(DMA_XOR_VAL, dma->cap_mask);
		dma->device_prep_dma_xor_val = al_dma_prep_xor_val_lock;
#endif
	}

#ifdef CONFIG_ALPINE_VP_WA
	dma->copy_align = AL_DMA_ALIGN_SHIFT;
	dma->xor_align = AL_DMA_ALIGN_SHIFT;
	dma->pq_align = AL_DMA_ALIGN_SHIFT;
	dma->fill_align = AL_DMA_ALIGN_SHIFT;
#endif

#ifdef CONFIG_DMATEST
	/* Reserve for DMA test */
	dma_cap_set(DMA_PRIVATE, dma->cap_mask);
#endif

	err = al_dma_setup_interrupts(device);

	if (err) {
		dev_err(device->common.dev, "failed to setup interrupts\n");
		return err;
	}

	err = dma_async_device_register(&device->common);

	if (err)
		dev_err(device->common.dev, "failed to register dma device\n");

	return err;
}


/******************************************************************************
 ***************************** Fast DMA **************************************/
#define FAST_DMA_NUM_OF_QUEUES		4
#define FAST_DMA_MEMCPY_TIMEOUT		1000 /* in uSec */
#define FAST_DMA_DESCS_COUNT		8
#define FAST_DMA_TX_CDESCS_COUNT	8
#define FAST_DMA_RX_CDESCS_COUNT	8

DEFINE_PER_CPU(struct al_udma_q *, tx_udma_q_percpu);
DEFINE_PER_CPU(struct al_udma_q *, rx_udma_q_percpu);
DEFINE_PER_CPU(uint32_t *, temp_percpu);
DEFINE_PER_CPU(al_phys_addr_t, temp_phys_addr_percpu);

al_phys_addr_t tx_dma_desc_phys[FAST_DMA_NUM_OF_QUEUES];
al_phys_addr_t rx_dma_desc_phys[FAST_DMA_NUM_OF_QUEUES];
al_phys_addr_t rx_dma_cdesc_phys[FAST_DMA_NUM_OF_QUEUES];
void *tx_dma_desc_virt[FAST_DMA_NUM_OF_QUEUES];
void *rx_dma_desc_virt[FAST_DMA_NUM_OF_QUEUES];
void *rx_dma_cdesc_virt[FAST_DMA_NUM_OF_QUEUES];

uint64_t	al_pcie_read_addr_start[AL_SB_PCIE_NUM];
uint64_t	al_pcie_read_addr_end[AL_SB_PCIE_NUM];
uint64_t	al_pcie_write_addr_start[AL_SB_PCIE_NUM];
uint64_t	al_pcie_write_addr_end[AL_SB_PCIE_NUM];
bool		al_pcie_address_valid[AL_SB_PCIE_NUM] = {0};

bool		fast_dma_init = false;

/******************************************************************************
 *****************************************************************************/
/* Prepare queue for fast mode */
static void ssm_udma_fast_init(struct al_ssm_dma *ssm_dma, struct device* dev)
{
	struct al_memcpy_transaction xaction;
	struct al_udma_q *tx_udma_q, *rx_udma_q;
	uint32_t *temp;
	al_phys_addr_t temp_phys_addr;
	int cpu;

	for_each_possible_cpu(cpu) {
		tx_udma_q = al_ssm_dma_tx_queue_handle_get(ssm_dma, cpu);
		rx_udma_q = al_ssm_dma_rx_queue_handle_get(ssm_dma, cpu);

		memset(&xaction, 0, sizeof(struct al_memcpy_transaction));
		al_udma_fast_memcpy_q_prepare(tx_udma_q, rx_udma_q, &xaction);

		/* Allocate temp memory */
		temp = dma_alloc_coherent(dev,
					  sizeof(uint32_t),
					  &temp_phys_addr,
					  GFP_KERNEL);

		per_cpu(tx_udma_q_percpu, cpu) = tx_udma_q;
		per_cpu(rx_udma_q_percpu, cpu) = rx_udma_q;
		per_cpu(temp_percpu, cpu) = temp;
		per_cpu(temp_phys_addr_percpu, cpu) = temp_phys_addr;
	}
}

static void ssm_udma_fast_terminate(struct device* dev)
{
	uint32_t *temp;
	al_phys_addr_t temp_phys_addr;
	int cpu;

	for_each_possible_cpu(cpu) {
		temp = per_cpu(temp_percpu, cpu);
		temp_phys_addr = per_cpu(temp_phys_addr_percpu, cpu);

		/* if not set, don't free */
		if (!temp)
			continue;

		dma_free_coherent(dev,
				  sizeof(uint32_t),
				  temp,
				  temp_phys_addr);

	}
}

/******************************************************************************
 *****************************************************************************/
int al_dma_fast_init(
	struct al_dma_device	*device,
	void __iomem		*iobase_udma)
{
	int32_t rc;
	int i;

	struct dma_device *dma = &device->common;
	struct al_udma_m2s_pkt_len_conf pkt_len_conf;
	struct al_udma *tx_udma;

	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;

	int max_channels = al_dma_get_max_channels();

	dev_dbg(
		dma->dev,
		"%s(%p, %p)\n",
		__func__,
		device,
		iobase_udma);

	al_assert(FAST_DMA_NUM_OF_QUEUES >= NR_CPUS);

	device->max_channels = max_channels;

	device->udma_regs_base = iobase_udma;
	device->app_regs_base = NULL;

	memset(&device->ssm_dma_params, 0, sizeof(struct al_ssm_dma_params));
	device->ssm_dma_params.dev_id = device->dev_id;
	device->ssm_dma_params.rev_id = device->rev_id;
	device->ssm_dma_params.udma_regs_base = device->udma_regs_base;

	device->ssm_dma_params.name =
		kmalloc(strlen(dev_name(device->common.dev)) + 1, GFP_KERNEL);
	if (device->ssm_dma_params.name == NULL) {
		dev_err(device->common.dev, "kmalloc failed\n");
		return -1;
	}

	memcpy(
		device->ssm_dma_params.name,
		dev_name(device->common.dev),
		strlen(dev_name(device->common.dev)) + 1);

	device->ssm_dma_params.num_of_queues = max_channels;

	rc = al_ssm_dma_init(&device->hal_raid, &device->ssm_dma_params);
	if (rc) {
		dev_err(device->common.dev, "al_raid_dma_init failed\n");
		return rc;
	}

	rc = al_ssm_dma_handle_get(
		&device->hal_raid,
		UDMA_TX,
		&tx_udma);
	if (rc) {
		dev_err(device->common.dev, "al_raid_dma_handle_get failed\n");
		return rc;
	}

	/* set max packet size to 128 (XOR with 32 sources) */
	/* TODO reduce max pkt size to 32 */
	pkt_len_conf.encode_64k_as_zero = AL_FALSE;
	pkt_len_conf.max_pkt_size = SZ_128;
	rc = al_udma_m2s_packet_size_cfg_set(tx_udma, &pkt_len_conf);
	if (rc) {
		dev_err(device->common.dev,
			"al_udma_m2s_packet_size_cfg_set failed\n");
		return rc;
	}

	/* enable RAID DMA engine */
	rc = al_ssm_dma_state_set(&device->hal_raid, UDMA_NORMAL);

	dma->dev = &device->pdev->dev;

	/* Init dma queue using the params below */
	for (i = 0; i < FAST_DMA_NUM_OF_QUEUES; i++) {
		/* Allocate dma queue memory */
		/* allocate coherent memory for Tx submission descriptors */
		tx_dma_desc_virt[i] = dma_alloc_coherent(
			dma->dev,
			FAST_DMA_DESCS_COUNT * sizeof(union al_udma_desc),
			&tx_dma_desc_phys[i],
			GFP_KERNEL);

		/* allocate coherent memory for Rx submission descriptors */
		rx_dma_desc_virt[i] = dma_alloc_coherent(
			dma->dev,
			FAST_DMA_DESCS_COUNT * sizeof(union al_udma_desc),
			&rx_dma_desc_phys[i],
			GFP_KERNEL);

		/* Allocate memory for Rx completion descriptors */
		/* allocate coherent memory for Rx submission descriptors */
		rx_dma_cdesc_virt[i] = dma_alloc_coherent(
			dma->dev,
			FAST_DMA_RX_CDESCS_COUNT * sizeof(union al_udma_cdesc),
			&rx_dma_cdesc_phys[i],
			GFP_KERNEL);

		/* Fill in dma queue params */
		tx_params.size = FAST_DMA_DESCS_COUNT;
		tx_params.desc_base = tx_dma_desc_virt[i];
		tx_params.desc_phy_base = tx_dma_desc_phys[i];
		tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
		tx_params.cdesc_phy_base = 0;
		tx_params.cdesc_size = FAST_DMA_TX_CDESCS_COUNT;

		rx_params.size = FAST_DMA_DESCS_COUNT;
		rx_params.desc_base = rx_dma_desc_virt[i];
		rx_params.desc_phy_base = rx_dma_desc_phys[i];
		rx_params.cdesc_base = rx_dma_cdesc_virt[i];
		rx_params.cdesc_phy_base = rx_dma_cdesc_phys[i];
		rx_params.cdesc_size = FAST_DMA_RX_CDESCS_COUNT;

		rc += al_ssm_dma_q_init(&device->hal_raid, i,
				&tx_params, &rx_params, AL_MEM_CRC_MEMCPY_Q);
	}

	ssm_udma_fast_init(&device->hal_raid, dma->dev);

	fast_dma_init = true;

	return rc;
}
int al_dma_fast_terminate(struct al_dma_device	*device)
{
	int i;
	struct dma_device *dma = &device->common;

	dev_dbg(
		dma->dev,
		"%s(%p)\n",
		__func__,
		device);

	fast_dma_init = false;

	ssm_udma_fast_terminate(dma->dev);

	for (i=0; i < FAST_DMA_NUM_OF_QUEUES; i++) {
		dma_free_coherent(
			dma->dev,
			FAST_DMA_RX_CDESCS_COUNT * sizeof(union al_udma_cdesc),
			rx_dma_cdesc_virt[i],
			rx_dma_cdesc_phys[i]);

		dma_free_coherent(
			dma->dev,
			FAST_DMA_DESCS_COUNT * sizeof(union al_udma_desc),
			rx_dma_desc_virt[i],
			rx_dma_desc_phys[i]);

		dma_free_coherent(
			dma->dev,
			FAST_DMA_DESCS_COUNT * sizeof(union al_udma_desc),
			tx_dma_desc_virt[i],
			tx_dma_desc_phys[i]);
	}

	kfree(device->ssm_dma_params.name);

	return 0;
}
/******************************************************************************
 *****************************************************************************/
/* Fast memcopy submission */
int udma_fast_memcpy(int len, al_phys_addr_t src, al_phys_addr_t dst)
{
	struct al_udma_q *tx_udma_q, *rx_udma_q;

	union al_udma_desc *tx_desc;
	union al_udma_desc *rx_desc;
	int completed = 0;
	int timeout = FAST_DMA_MEMCPY_TIMEOUT;
	uint32_t flags;
	/* prepare rx desc */

	rx_udma_q = __this_cpu_read(rx_udma_q_percpu);
	tx_udma_q = __this_cpu_read(tx_udma_q_percpu);

	rx_desc = al_udma_desc_get(rx_udma_q);

	flags = al_udma_ring_id_get(rx_udma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(rx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(rx_desc, len);
	al_udma_fast_desc_buf_set(rx_desc, dst, 0);

	/* submit rx desc */
	al_udma_desc_action_add(rx_udma_q, 1);

	/* prepare tx desc */
	tx_desc = al_udma_desc_get(tx_udma_q);

	flags = al_udma_ring_id_get(tx_udma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

	al_udma_fast_desc_flags_set(tx_desc, flags, AL_M2S_DESC_RING_ID_MASK);
	al_udma_fast_desc_len_set(tx_desc, len);
	al_udma_fast_desc_buf_set(tx_desc, src, 0);

	/* submit tx desc */
	al_udma_desc_action_add(tx_udma_q, 1);

	/* wait for completion using polling */
	while(1) {
		completed = al_udma_fast_completion(rx_udma_q, 1, 0);
		if ((completed > 0) || (timeout == 0))
			break;

		udelay(1);
		timeout--;
	}

	if (timeout == 0) {
		pr_err("%s: Didn't receive completion in %d uSec",
		       __func__, FAST_DMA_MEMCPY_TIMEOUT);

		return -ETIME;
	}

	return 0;
}
EXPORT_SYMBOL(udma_fast_memcpy);

static inline al_phys_addr_t virt_to_physical_address(const volatile void __iomem *address)
{
	al_phys_addr_t phys_addr;
	uint32_t phys_addr_h, phys_addr_l;

	/*
	 * write a virt. address to ATS1CPR:
	 * perform H/W stage1 address translation (meaning, to IPA)
	 * translate as current security state, privileged read accesses
	 * read PAR: (physical address register)
	 * lower 12-bit have some flags, the rest holds upper bits
	 * of the physical address
	 */
	asm volatile( "mcr p15, 0, %0, c7, c8, 0" :: "r"(address));

	/*
	 * according to ARM ABI, in Little Endian systems r0 will contain the
	 * low 32 bits, while in Big Endian systems r0 will contain the high 32
	 * bits
	 * TODO: assumes LE need to change to BE mode
	 */

#ifdef CONFIG_CPU_BIG_ENDIAN
#error "virt_to_physical_address assumes LE!"
#endif
	asm volatile("mrrc p15, 0, %0, %1, c7" : "=r"(phys_addr_l), "=r"(phys_addr_h));

	/* Take the lower 12-bit from the virtual address. */
	phys_addr = phys_addr_l & ~(((uint32_t)1<<12) - 1UL);
	phys_addr |= (uintptr_t)address & AL_BIT_MASK(12);

	return phys_addr;
}

#ifdef	CONFIG_AL_PCIE_DEADLOCK_WA_VALIDATE
#define _al_dma_dma_read_validate(type, val)			\
{								\
	type _cpu_val;						\
	switch (sizeof(type)) {						\
	case sizeof(uint8_t):					\
		_cpu_val = __raw_readb(address);		\
		break;						\
	case sizeof(uint16_t):					\
		_cpu_val = le16_to_cpu((__force __le16)__raw_readw(address)); \
		break;						\
	default:						\
	case sizeof(uint32_t):					\
		_cpu_val = le32_to_cpu((__force __le32)__raw_readl(address)); \
		break;						\
	}							\
								\
	if (memcmp(&_cpu_val, &val, sizeof(type))) {		\
		al_info("[%s] Potential Error: DMA read value isn't the same as CPU read addr: "	\
			"%p phys addr %x DMA read: %x cpu read: %x\n"					\
			"This register might be clear on read or status register so different values"	\
			"doesn't guarantee we have a problem, Please check the spec\n",			\
			__func__, address, phys_addr, val, _cpu_val);					\
			val = _cpu_val;				\
	}							\
}
#else
#define _al_dma_dma_read_validate(type, val)
#endif

static inline uint32_t _al_dma_read_reg(const volatile void __iomem *address, int size)
{
	unsigned long flags;
	al_phys_addr_t phys_addr;
	uint32_t val_32;
	uint16_t val_16;
	uint8_t val_8;
	int i;

	/* Use DMA read only if the fast DMA was initialized and HW CC */
	if (likely((al_fabric_hwcc_enabled()) && (fast_dma_init))) {
		local_irq_save(flags);

		phys_addr = virt_to_physical_address(address);

		for (i = 0; i < AL_SB_PCIE_NUM; i++) {
			if (likely(al_pcie_address_valid[i] == false))
				continue;

			if (unlikely(phys_addr >= al_pcie_read_addr_start[i] &&
				     phys_addr <= al_pcie_read_addr_end[i]))
				goto pcie_mem_read;
		}

		local_irq_restore(flags);
	}


	switch (size) {
	case sizeof(uint8_t):
		val_8 = __raw_readb(address);
		return val_8;
	case sizeof(uint16_t):
		val_16 = le16_to_cpu((__force __le16)__raw_readw(address));
		return val_16;
	default:
	case sizeof(uint32_t):
		val_32 = le32_to_cpu((__force __le32)__raw_readl(address));
		return val_32;
	}

pcie_mem_read:
	udma_fast_memcpy(size,
			phys_addr,
			__this_cpu_read(temp_phys_addr_percpu));

	switch (size) {
	default:
	case sizeof(uint32_t):
		val_32 = *__this_cpu_read(temp_percpu);
		_al_dma_dma_read_validate(uint32_t, val_32);
		local_irq_restore(flags);
		return val_32;
	case sizeof(uint16_t):
		val_16 = *__this_cpu_read(temp_percpu);
		_al_dma_dma_read_validate(uint16_t, val_16);
		local_irq_restore(flags);
		return val_16;
	case sizeof(uint8_t):
		val_8 = *__this_cpu_read(temp_percpu);
		_al_dma_dma_read_validate(uint8_t, val_8);
		local_irq_restore(flags);
		return val_8;
	}
}

uint32_t al_dma_read_reg32(const volatile void __iomem *address)
{
	return _al_dma_read_reg(address, sizeof(uint32_t));
}
EXPORT_SYMBOL(al_dma_read_reg32);

uint16_t al_dma_read_reg16(const volatile void __iomem *address)
{
	return _al_dma_read_reg(address, sizeof(uint16_t));
}
EXPORT_SYMBOL(al_dma_read_reg16);

uint8_t al_dma_read_reg8(const volatile void __iomem *address)
{
	return _al_dma_read_reg(address, sizeof(uint8_t));
}
EXPORT_SYMBOL(al_dma_read_reg8);


void al_dma_write_reg32(volatile void __iomem *address, u32 val)
{
	unsigned long flags;
	al_phys_addr_t phys_addr;
	int i;

	/* Use DMA write only if the fast DMA was initialized and HW CC */
	if (likely((al_fabric_hwcc_enabled()) && (fast_dma_init))) {
		local_irq_save(flags);

		phys_addr = virt_to_physical_address(address);

		for (i = 0; i < AL_SB_PCIE_NUM; i++) {
			if (likely(al_pcie_address_valid[i] == false))
				continue;

			if (unlikely(phys_addr >= al_pcie_write_addr_start[i] &&
				     phys_addr <= al_pcie_write_addr_end[i]))
				goto pcie_mem_write;
		}

		local_irq_restore(flags);
	}

	__raw_writel((__force u32) cpu_to_le32(val), address);

	return;

pcie_mem_write:
	__this_cpu_write(temp_percpu,&val);

	udma_fast_memcpy(sizeof(uint32_t),
			__this_cpu_read(temp_phys_addr_percpu),
			phys_addr);

	local_irq_restore(flags);
}
EXPORT_SYMBOL(al_dma_write_reg32);

/******************************************************************************
 *****************************************************************************/
int al_dma_core_terminate(
	struct al_dma_device	*device)
{
	int status = 0;

	struct dma_device *dma = &device->common;

	dev_dbg(
		dma->dev,
		"%s(%p)\n",
		__func__,
		device);

	dma_async_device_unregister(&device->common);

	al_dma_free_interrupts(device);

	kfree(device->ssm_dma_params.name);

	kmem_cache_destroy(device->cache);

	return status;
}

/******************************************************************************
 *****************************************************************************/
static int al_dma_init_channels(struct al_dma_device *device, int max_channels)
{
	int i;
	struct al_dma_chan *chan;
	struct device *dev = &device->pdev->dev;
	struct dma_device *dma = &device->common;

	INIT_LIST_HEAD(&dma->channels);
	dma->chancnt = max_channels;

	if (dma->chancnt > ARRAY_SIZE(device->channels)) {
		dev_warn(dev, "(%d) exceeds max supported channels (%zu)\n",
			 dma->chancnt, ARRAY_SIZE(device->channels));
		dma->chancnt = ARRAY_SIZE(device->channels);
	}

	for (i = 0; i < dma->chancnt; i++) {
		chan = devm_kzalloc(dev, sizeof(*chan), GFP_KERNEL);
		if (!chan)
			break;

		al_dma_init_channel(device, chan, i);

	}
	dma->chancnt = i;
	return i;
}

/******************************************************************************
 *****************************************************************************/
static void al_dma_init_channel(struct al_dma_device *device,
			 struct al_dma_chan *chan, int idx)
{
	struct dma_device *dma = &device->common;
	struct dma_chan *c = &chan->common;
	unsigned long data = (unsigned long) c;

	dev_dbg(
		dma->dev,
		"%s(%p, %p, %d): %p\n",
		__func__,
		device,
		chan,
		idx,
		c);

	chan->device = device;
	chan->idx = idx;
	chan->hal_raid = &device->hal_raid;

	spin_lock_init(&chan->prep_lock);

	spin_lock_init(&chan->cleanup_lock);
	chan->common.device = dma;
	list_add_tail(&chan->common.device_node, &dma->channels);
	device->channels[idx] = chan;

	tasklet_init(&chan->cleanup_task, al_dma_cleanup_tasklet, data);
}

/******************************************************************************
 *****************************************************************************/
static int al_dma_setup_interrupts(struct al_dma_device *device)
{
	struct al_dma_chan *chan;
	struct pci_dev *pdev = device->pdev;
	struct device *dev = &pdev->dev;
	struct msix_entry *msix;
	int i, j, msixcnt;
	int err = -EINVAL;

	/* The number of MSI-X vectors should equal the number of channels */
	msixcnt = device->common.chancnt;

	for (i = 0; i < msixcnt; i++)
		device->msix_entries[i].entry = 3 + i;

	err = pci_enable_msix_exact(pdev, device->msix_entries, msixcnt);

	if (err < 0) {
		dev_err(dev, "pci_enable_msix_exact failed! using intx instead.\n");
		goto intx;
	}

	if (err > 0) {
		dev_err(dev, "pci_enable_msix_exact failed! msix_single_vector.\n");
		goto msix_single_vector;
	}

	for (i = 0; i < msixcnt; i++) {
		msix = &device->msix_entries[i];

		chan = al_dma_chan_by_index(device, i);

		dev_dbg(dev, "%s: requesting irq %d\n", __func__, msix->vector);

		snprintf(device->irq_tbl[i].name, AL_DMA_IRQNAME_SIZE,
			"al-dma-comp-%d@pci:%s", i,
			pci_name(pdev));

		err = devm_request_irq(
			dev,
			msix->vector,
			al_dma_do_interrupt_msix,
			0,
			device->irq_tbl[i].name,
			chan);

		if (err) {
			dev_err(dev, "devm_request_irq failed!.\n");

			for (j = 0; j < i; j++) {
				msix = &device->msix_entries[j];
				chan = al_dma_chan_by_index(device, j);
				devm_free_irq(dev, msix->vector, chan);
			}

			/* goto msix_single_vector; */
			return -EIO;
		}

		/* setup interrupt affinity */
		if (cpu_online(chan->idx))
			cpumask_set_cpu(chan->idx, &chan->affinity_mask);
		else
			cpumask_copy(&chan->affinity_mask, cpu_online_mask);

		dev_dbg(
			dev,
			"Setting affinity of channel %d to %lx\n",
			chan->idx,
			chan->affinity_mask.bits[0]);

		err = irq_set_affinity_hint(msix->vector, &chan->affinity_mask);
		if (err) {
			dev_err(dev, "irq_set_affinity_hint failed!\n");
			return err;
		}

		err = irq_set_affinity(msix->vector, &chan->affinity_mask);
		if (err) {
			dev_err(dev, "irq_set_affinity failed!\n");
			return err;
		}
	}

	err = al_udma_iofic_config(
		(struct unit_regs *)device->udma_regs_base,
		AL_IOFIC_MODE_MSIX_PER_Q, 0x480, 0x480, 0x1E0, 0x1E0);
	if (err) {
		dev_err(dev, "al_udma_iofic_config failed!.\n");
		return err;
	}

	al_udma_iofic_unmask(
		(struct unit_regs *)device->udma_regs_base,
		AL_UDMA_IOFIC_LEVEL_PRIMARY,
		AL_INT_GROUP_B,
		((1 << (device->common.chancnt)) - 1));

	goto done;

msix_single_vector:
	msix = &device->msix_entries[0];

	msix->entry = 0;

	err = pci_enable_msix_exact(pdev, device->msix_entries, 1);

	if (err)
		goto intx;

	snprintf(device->irq_tbl[0].name, AL_DMA_IRQNAME_SIZE,
		"al-dma-msix-all@pci:%s", pci_name(pdev));

	err = devm_request_irq(
		dev,
		msix->vector,
		al_dma_do_interrupt,
		IRQF_TRIGGER_RISING,
		device->irq_tbl[0].name, device);

	if (err) {
		pci_disable_msix(pdev);
		goto intx;
	}

	goto done;

intx:
	snprintf(device->irq_tbl[0].name, AL_DMA_IRQNAME_SIZE,
		"al-dma-intx-all@pci:%s", pci_name(pdev));

	err = devm_request_irq(dev, pdev->irq, al_dma_do_interrupt,
			       IRQF_SHARED, device->irq_tbl[0].name, device);
	if (err)
		goto err_no_irq;

done:
	return 0;

err_no_irq:
	/* Disable all interrupt generation */

	dev_err(dev, "no usable interrupts\n");
	return err;
}

/******************************************************************************
 *****************************************************************************/
static void al_dma_free_interrupts(struct al_dma_device *device)
{
	struct al_dma_chan *chan;
	struct pci_dev *pdev = device->pdev;
	struct device *dev = &pdev->dev;
	struct msix_entry *msix;
	int i, msixcnt;

	/* The number of MSI-X vectors should equal the number of channels */
	msixcnt = device->common.chancnt;

	if (pdev->msix_enabled) {
		msix = &device->msix_entries[0];
		if (msix->entry == 0) {
			devm_free_irq(dev, msix->vector, device);
			pci_disable_msix(pdev);
			return;
		}

		for (i = 0; i < msixcnt; i++) {
			msix = &device->msix_entries[i];
			chan = al_dma_chan_by_index(device, i);
			irq_set_affinity_hint(msix->vector, NULL);
			devm_free_irq(dev, msix->vector, chan);
		}

		pci_disable_msix(pdev);
	} else {
		devm_free_irq(dev, pdev->irq, device);
	}
}

/******************************************************************************
 *****************************************************************************/
/* al_dma_alloc_chan_resources - allocate/initialize tx and rx descriptor rings
 */
static int al_dma_alloc_chan_resources(struct dma_chan *c)
{
	struct al_dma_chan *chan = to_al_dma_chan(c);
	struct device *dev = chan->device->common.dev;
	struct al_dma_sw_desc **sw_ring;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	int ring_alloc_order = al_dma_get_ring_alloc_order();
	int tx_descs_order = al_dma_get_tx_descs_order();
	int rx_descs_order = al_dma_get_rx_descs_order();
	uint32_t rc = 0;

	dev_dbg(dev, "al_dma_alloc_chan_resources: channel %d\n",
		chan->idx);

	/* have we already been set up? */
	if (chan->sw_ring)
		return 1 << chan->alloc_order;

	chan->tx_descs_num = 1 << tx_descs_order;
	chan->rx_descs_num = 1 << rx_descs_order;

	/* allocate coherent memory for Tx submission descriptors */
	chan->tx_dma_desc_virt = dma_alloc_coherent(dev,
						    chan->tx_descs_num *
						    sizeof(union al_udma_desc),
						    &chan->tx_dma_desc,
						    GFP_KERNEL);
	if (chan->tx_dma_desc_virt == NULL) {
		dev_err(dev, "failed to allocate %d bytes of coherent "
			"memory for Tx submission descriptors\n",
			chan->tx_descs_num * sizeof(union al_udma_desc));
		return -ENOMEM;
	}
	dev_dbg(dev, "allocted tx descriptor ring: virt 0x%p phys 0x%llx\n",
		chan->tx_dma_desc_virt, (u64)chan->tx_dma_desc);

	/* allocate coherent memory for Rx submission descriptors */
	chan->rx_dma_desc_virt = dma_alloc_coherent(dev,
						    chan->rx_descs_num *
						    sizeof(union al_udma_desc),
						    &chan->rx_dma_desc,
						    GFP_KERNEL);
	if (chan->rx_dma_desc_virt == NULL) {
		dev_err(dev, "failed to allocate %d bytes of coherent "
			"memory for Rx submission descriptors\n",
			chan->rx_descs_num * sizeof(union al_udma_desc));

		al_dma_free_chan_resources(c);
		return -ENOMEM;
	}
	dev_dbg(dev, "allocted rx descriptor ring: virt 0x%p phys 0x%llx\n",
		chan->rx_dma_desc_virt, (u64)chan->rx_dma_desc);

	/* allocate coherent memory for Rx completion descriptors */
	chan->rx_dma_cdesc_virt = dma_alloc_coherent(dev,
						     chan->rx_descs_num *
						     AL_DMA_RAID_RX_CDESC_SIZE,
						     &chan->rx_dma_cdesc,
						     GFP_KERNEL);
	if (chan->rx_dma_cdesc_virt == NULL) {
		dev_err(dev, "failed to allocate %d bytes of coherent "
			"memory for Rx completion descriptors\n",
			chan->rx_descs_num * AL_DMA_RAID_RX_CDESC_SIZE);

		al_dma_free_chan_resources(c);
		return -ENOMEM;
	}

	/* clear the Rx completion descriptors to avoid false positive */
	memset(
		chan->rx_dma_cdesc_virt,
		0,
		chan->rx_descs_num * AL_DMA_RAID_RX_CDESC_SIZE);

	dev_dbg(
		dev,
		"allocted rx completion desc ring: virt 0x%p phys 0x%llx\n",
		chan->rx_dma_cdesc_virt, (u64)chan->rx_dma_cdesc);

	tx_params.size = chan->tx_descs_num;
	tx_params.desc_base = chan->tx_dma_desc_virt;
	tx_params.desc_phy_base = chan->tx_dma_desc;
	tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
	tx_params.cdesc_phy_base = 0;
	tx_params.cdesc_size = AL_DMA_RAID_TX_CDESC_SIZE; /* size is needed */

	rx_params.size = chan->rx_descs_num;
	rx_params.desc_base = chan->rx_dma_desc_virt;
	rx_params.desc_phy_base = chan->rx_dma_desc;
	rx_params.cdesc_base = chan->rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = chan->rx_dma_cdesc;
	rx_params.cdesc_size = AL_DMA_RAID_RX_CDESC_SIZE;

	/* alloc sw descriptors */
	if (ring_alloc_order < AL_DMA_SW_RING_MIN_ORDER) {
		dev_err(
			dev,
			"%s: ring_alloc_order = %d < %d!\n",
			__func__,
			ring_alloc_order,
			AL_DMA_SW_RING_MIN_ORDER);

		al_dma_free_chan_resources(c);
		return -EINVAL;
	} else if (ring_alloc_order > AL_DMA_SW_RING_MAX_ORDER) {
		dev_err(
			dev,
			"%s: ring_alloc_order = %d > %d!\n",
			__func__,
			ring_alloc_order,
			AL_DMA_SW_RING_MAX_ORDER);

		al_dma_free_chan_resources(c);
		return -EINVAL;
	} else if (ring_alloc_order > rx_descs_order) {
		dev_warn(
			dev,
			"%s: ring_alloc_order > rx_descs_order (%d>%d)!\n",
			__func__,
			ring_alloc_order,
			rx_descs_order);

	}

	sw_ring = al_dma_alloc_sw_ring(chan, ring_alloc_order, GFP_KERNEL);
	if (!sw_ring)
		return -ENOMEM;

	spin_lock_bh(&chan->cleanup_lock);
	spin_lock_bh(&chan->prep_lock);
	chan->sw_ring = sw_ring;
	chan->head = 0;
	chan->tail = 0;
	chan->alloc_order = ring_alloc_order;
	chan->tx_desc_produced = 0;
	spin_unlock_bh(&chan->prep_lock);
	spin_unlock_bh(&chan->cleanup_lock);

	rc = al_ssm_dma_q_init(&chan->device->hal_raid, chan->idx,
				&tx_params, &rx_params, AL_RAID_Q);
	if (rc) {
		dev_err(dev, "failed to initialize hal q %d. rc %d\n",
			chan->idx, rc);
		al_dma_free_chan_resources(c);
		return rc;
	}

	/* should we return less ?*/
	return  1 << chan->alloc_order;
}

/******************************************************************************
 *****************************************************************************/
/* al_dma_free_chan_resources - free tx and rx descriptor rings
 * @chan: channel to be free
 */
static void al_dma_free_chan_resources(struct dma_chan *c)
{
	struct al_dma_chan *chan = to_al_dma_chan(c);
	struct device *dev = chan->device->common.dev;
	struct al_dma_sw_desc **sw_ring;
	int i;

	dev_dbg(dev, "%s(%p): %p\n", __func__, c, chan);

	tasklet_disable(&chan->cleanup_task);

	al_dma_cleanup_fn(chan, 0);

	spin_lock_bh(&chan->cleanup_lock);
	sw_ring = chan->sw_ring;
	for (i = 0; i < (1 << chan->alloc_order); i++)
		al_dma_free_ring_ent(sw_ring[i], chan);

	kfree(chan->sw_ring);

	spin_unlock_bh(&chan->cleanup_lock);
	if (chan->tx_dma_desc_virt != NULL) {
		dma_free_coherent(
			dev,
			chan->tx_descs_num * sizeof(union al_udma_desc),
			chan->tx_dma_desc_virt, chan->tx_dma_desc);
		chan->tx_dma_desc_virt = NULL;
	}

	if (chan->rx_dma_desc_virt != NULL) {
		dma_free_coherent(
			dev,
			chan->rx_descs_num * sizeof(union al_udma_desc),
			chan->rx_dma_desc_virt,
			chan->rx_dma_desc);
		chan->rx_dma_desc_virt = NULL;
	}

	if (chan->rx_dma_cdesc_virt != NULL) {
		dma_free_coherent(dev, chan->rx_descs_num *
				  AL_DMA_RAID_RX_CDESC_SIZE,
				  chan->rx_dma_cdesc_virt, chan->rx_dma_cdesc);
		chan->rx_dma_desc_virt = NULL;
	}

	return;
}

/******************************************************************************
 *****************************************************************************/
static struct al_dma_sw_desc **al_dma_alloc_sw_ring(
	struct al_dma_chan	*chan,
	int			order,
	gfp_t			flags)
{
	struct al_dma_sw_desc **ring;
	int descs = 1 << order;
	int i;

	/* allocate the array to hold the software ring */
	ring = kcalloc(descs, sizeof(*ring), flags);
	if (!ring)
		return NULL;
	for (i = 0; i < descs; i++) {
		ring[i] = al_dma_alloc_ring_ent(chan, flags);
		if (!ring[i]) {
			while (i--)
				al_dma_free_ring_ent(ring[i], chan);
			kfree(ring);
			return NULL;
		}
		set_desc_id(ring[i], i);
	}

	return ring;
}

/******************************************************************************
 *****************************************************************************/
static struct al_dma_sw_desc *al_dma_alloc_ring_ent(
	struct al_dma_chan	*chan,
	gfp_t			flags)
{
	struct al_dma_sw_desc *desc;

	desc = kmem_cache_zalloc(chan->device->cache, flags);
	if (!desc)
		return NULL;

	dma_async_tx_descriptor_init(&desc->txd, &chan->common);
	desc->txd.tx_submit = al_dma_tx_submit_unlock;
	return desc;
}

/******************************************************************************
 *****************************************************************************/
static void al_dma_free_ring_ent(
	struct al_dma_sw_desc	*desc,
	struct al_dma_chan	*chan)
{
	kmem_cache_free(chan->device->cache, desc);
}

/* wrappers for accessing PCI configuration space */
static int al_dma_read_pcie_config(void *handle, int where, uint32_t *val)
{
	/* handle is a pointer to the pci_dev */
	pci_read_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

static int al_dma_write_pcie_config(void *handle, int where, uint32_t val)
{
	/* handle is a pointer to the pci_dev */
	pci_write_config_dword((struct pci_dev *)handle, where, val);
	return 0;
}

/* wrapper for PCI function level reset */
static int al_dma_write_pcie_flr(void *handle)
{
	/* handle is a pointer to the pci_dev */
	__pci_reset_function_locked((struct pci_dev *)handle);
	udelay(1000);
	return 0;
}

/**
 * al_dma_flr - perform Function Level Reset
 * @pdev: PCI device to reset
 */
void al_dma_flr(struct pci_dev *pdev)
{
	al_pcie_perform_flr(al_dma_read_pcie_config,
			al_dma_write_pcie_config,
			al_dma_write_pcie_flr,
			pdev);
}

/******************************************************************************
 *****************************************************************************/
/**
 * al_dma_get_sw_desc_lock - get sw desc and grab ring producer lock
 * @chan: dma channel to operate on
 * @num: the number of required sw descriptos
 */
int al_dma_get_sw_desc_lock(struct al_dma_chan *chan, int num)
{
	spin_lock_bh(&chan->prep_lock);

	/* never allow the last descriptor to be consumed, we need at
	 * least one free at all times to allow for on-the-fly ring
	 * resizing.
	 */
	if (likely(al_dma_ring_space(chan) >= num)) {
		dev_dbg(to_dev(chan), "%s: (%x:%x)\n",
			__func__, chan->head, chan->tail);
		return 0;  /* with chan->prep_lock held */
	}

	spin_unlock_bh(&chan->prep_lock);

	return -ENOMEM;
}

/******************************************************************************
 *****************************************************************************/
/**
 * al_dma_do_interrupt - handler used for single vector interrupt mode
 * @irq: interrupt id
 * @data: interrupt data
 */
static irqreturn_t al_dma_do_interrupt(int irq, void *data)
{
	pr_debug("%s(%d, %p)\n", __func__, irq, data);

	/* TODO: handle interrupt registers */

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
/**
 * al_dma_do_interrupt_msix - handler used for vector-per-channel interrupt mode
 * @irq: interrupt id
 * @data: interrupt data
 */
static irqreturn_t al_dma_do_interrupt_msix(int irq, void *data)
{
	struct al_dma_chan *chan = data;

	pr_debug("%s(%d, %p)\n", __func__, irq, data);

	tasklet_schedule(&chan->cleanup_task);

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
/**
 * al_dma_tx_status - poll the status of an DMA transaction
 * @c: channel handle
 * @cookie: transaction identifier
 * @txstate: if set, updated with the transaction state
 */
static enum dma_status al_dma_tx_status(
	struct dma_chan *c,
	dma_cookie_t cookie,
	struct dma_tx_state *txstate)
{
	struct al_dma_chan *chan = to_al_dma_chan(c);
	enum dma_status ret;

	dev_dbg(
		to_dev(chan),
		"%s(%d)\n",
		__func__,
		cookie);

	ret = dma_cookie_status(c, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;

	al_dma_cleanup_fn(chan, 0);

	return dma_cookie_status(c, cookie, txstate);
}

/******************************************************************************
 *****************************************************************************/
static inline int al_dma_issue_pending_raw(struct al_dma_chan *chan)
{
	int err = 0;

	if (chan->tx_desc_produced) {
		dev_dbg(
			chan->device->common.dev,
			"%s(%p): issuing %u descriptors\n",
			__func__,
			chan,
			chan->tx_desc_produced);

		err = al_raid_dma_action(
				chan->hal_raid,
				chan->idx,
				chan->tx_desc_produced);
		if (err)
			dev_err(
				chan->device->common.dev,
				"al_raid_dma_action failed\n");

		chan->tx_desc_produced = 0;
	}

	return err;
}

/******************************************************************************
 *****************************************************************************/
void al_dma_tx_submit_sw_cond_unlock(
	struct al_dma_chan		*chan,
	struct dma_async_tx_descriptor	*tx)
{
	if (tx) {
		struct dma_chan *c = tx->chan;
		dma_cookie_t cookie = dma_cookie_assign(tx);

		c->cookie = cookie;

		dev_dbg(
			chan->device->common.dev,
			"%s: cookie = %d\n",
			__func__,
			cookie);

		/**
		 * according to Documentation/circular-buffers.txt we should
		 * have smp_wmb before intcrementing the head, however, the
		 * al_raid_dma_action contains writel() which implies dmb on
		 * ARM so this smp_wmb() can be omitted on ARM platforms
		 */
		/*smp_wmb();*/ /* commit the item before updating the head */
		chan->head += chan->sw_desc_num_locked;
		/**
		 * in our case the consumer (interrupt handler) will be waken up
		 * by the hw, so we send the transaction to the hw after
		 * incrementing the head
		 **/
	}

#if !AL_DMA_ISSUE_PNDNG_UPON_SUBMIT
	spin_unlock_bh(&chan->prep_lock);
#endif
}

/******************************************************************************
 *****************************************************************************/
static dma_cookie_t al_dma_tx_submit_unlock(struct dma_async_tx_descriptor *tx)
{
#if AL_DMA_ISSUE_PNDNG_UPON_SUBMIT
	int err;

	struct dma_chan *c = tx->chan;
	struct al_dma_chan *chan = to_al_dma_chan(c);

	dev_dbg(
		chan->device->common.dev,
		"%s(%p): %p, %p\n",
		__func__,
		tx,
		chan,
		c);

	err = al_dma_issue_pending_raw(chan);
	if (err)
		dev_err(
			chan->device->common.dev,
			"%s: al_dma_issue_pending\n",
			__func__);

	spin_unlock_bh(&chan->prep_lock);
#endif

	return tx->cookie;
}

/******************************************************************************
 *****************************************************************************/
static void al_dma_issue_pending(struct dma_chan *c)
{
#if !AL_DMA_ISSUE_PNDNG_UPON_SUBMIT
	int err;

	struct al_dma_chan *chan = to_al_dma_chan(c);

	spin_lock_bh(&chan->prep_lock);

	dev_dbg(
		chan->device->common.dev,
		"%s(%p)\n",
		__func__,
		chan);

	err = al_dma_issue_pending_raw(chan);
	if (err)
		dev_err(
			chan->device->common.dev,
			"%s: al_dma_issue_pending\n",
			__func__);

	spin_unlock_bh(&chan->prep_lock);
#endif
}

static int al_dma_device_terminate_all(struct dma_chan *dchan)
{
	struct al_dma_chan *chan = to_al_dma_chan(dchan);
	int err;
		dev_err(
			chan->device->common.dev,
			"%s: Unexpected cmd (terminate_all)!\n",
			__func__);
		err = -ENXIO;
		
	return err;

}

static int al_dma_device_config(struct dma_chan *dchan,
				 struct dma_slave_config *config)
{

	struct al_dma_chan *chan = to_al_dma_chan(dchan);
	int err;
		dev_err(
			chan->device->common.dev,
			"%s: Unexpected cmd (device_config)!\n",
			__func__);
		err = -ENXIO;
		
	return err;

}

static void al_dma_cleanup_tasklet(unsigned long data)
{
	struct al_dma_chan *chan = to_al_dma_chan((void *) data);
	int num_completed;

	num_completed = al_dma_cleanup_fn(chan, 1);

	if (unlikely(num_completed < 0))
		dev_err(
			chan->device->common.dev,
			"al_dma_cleanup_fn failed\n");

	al_udma_iofic_unmask(
		(struct unit_regs *)chan->device->udma_regs_base,
		AL_UDMA_IOFIC_LEVEL_PRIMARY,
		AL_INT_GROUP_B,
		1 << chan->idx);
}

