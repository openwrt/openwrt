/*
 * drivers/crypto/al_crypto_core.c
 *
 * Annapurna Labs Crypto driver - core
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
#ifndef DEBUG
#define DEBUG
#endif
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <crypto/scatterwalk.h>

#include <mach/al_hal_ssm_crypto.h>
#include <mach/al_hal_udma_iofic.h>
#include <mach/al_hal_udma_config.h>

#include "al_crypto.h"
#include "al_crypto_module_params.h"

#define smp_read_barrier_depends() do {} while(0)

static void al_crypto_free_chan_resources(
		struct al_crypto_chan *chan);

static int al_crypto_alloc_chan_resources(
		struct al_crypto_chan *chan);

static void al_crypto_free_channels(
		struct al_crypto_device *device);

static int al_crypto_alloc_channels(
		struct al_crypto_device *device);

static int al_crypto_setup_interrupts(
		struct al_crypto_device *device);

static irqreturn_t al_crypto_do_interrupt_msix(
		int	irq,
		void	*data);

static irqreturn_t al_crypto_do_interrupt_group_d(
		int	irq,
		void	*data);

static irqreturn_t al_crypto_do_interrupt_msix_rx(
		int	irq,
		void	*data);

static irqreturn_t al_crypto_do_interrupt_legacy(
		int	irq,
		void	*data);

static int al_crypto_init_channels(
		struct al_crypto_device	*device,
		int			max_channels);

static void al_crypto_init_channel(
		struct al_crypto_device	*device,
		struct al_crypto_chan	*chan,
		int			idx);

static struct al_crypto_sw_desc **al_crypto_alloc_sw_ring(
		struct al_crypto_chan	*chan,
		int			order,
		gfp_t		flags);

static void al_crypto_free_sw_ring(
		struct al_crypto_sw_desc **ring,
		struct al_crypto_chan	*chan,
		int size);

static struct al_crypto_sw_desc *al_crypto_alloc_ring_ent(
		struct al_crypto_chan	*chan,
		gfp_t			flags);

static void al_crypto_free_ring_ent(
		struct al_crypto_sw_desc	*desc,
		struct al_crypto_chan		*chan);

static int al_crypto_iofic_config(struct al_crypto_device *device,
		bool single_msix);

static void al_crypto_cleanup_tasklet(
		unsigned long data);

static void al_crypto_cleanup_tasklet_msix_rx(
		unsigned long data);

static void al_crypto_cleanup_tasklet_legacy(
		unsigned long data);

static void al_crypto_unmask_interrupts(struct al_crypto_device *device,
		bool single_interrupt);

static void al_crypto_group_d_errors_handler(struct al_crypto_device *device);
/******************************************************************************
 *****************************************************************************/
int al_crypto_core_init(struct al_crypto_device *device,
			void __iomem *iobase_udma,
			void __iomem *iobase_app)
{
	int32_t rc;
	int err;
	int max_channels;
	int crc_channels;

	dev_dbg(&device->pdev->dev, "%s\n", __func__);

	device->cache = kmem_cache_create("al_crypto",
					  sizeof(struct al_crypto_sw_desc),
					  0, SLAB_HWCACHE_ALIGN, NULL);
	if (!device->cache)
		return -ENOMEM;

	max_channels = device->max_channels;
	crc_channels = device->crc_channels;

	if ((crc_channels > max_channels) || (max_channels > DMA_MAX_Q)) {
		dev_err(&device->pdev->dev, "invalid number of channels\n");
		err = -EINVAL;
		goto done;
	}

	device->udma_regs_base = iobase_udma;
	/* The crypto regs exists only for the PF.
	 * The VF uses the same configs/ error reporting as the PF */
	device->crypto_regs_base = iobase_app ? iobase_app +
			AL_CRYPTO_APP_REGS_BASE_OFFSET : NULL;

	device->ssm_dma_params.udma_regs_base = device->udma_regs_base;

	device->ssm_dma_params.name =
		kmalloc(strlen(dev_name(&device->pdev->dev)) + 1, GFP_KERNEL);
	if (device->ssm_dma_params.name == NULL) {
		dev_err(&device->pdev->dev, "kmalloc failed\n");
		err = -ENOMEM;
		goto done;
	}

	strcpy(device->ssm_dma_params.name, dev_name(&device->pdev->dev));

	device->ssm_dma_params.num_of_queues = max_channels;

	err = al_ssm_dma_init(&device->hal_crypto,
			&device->ssm_dma_params);
	if (err) {
		dev_err(&device->pdev->dev, "al_crypto_dma_init failed\n");
		goto err_no_chan;
	}

	/* enumerate and initialize channels (queues) */
	al_crypto_init_channels(device, max_channels);

	err = al_crypto_alloc_channels(device);
	if (err) {
		dev_err(&device->pdev->dev,
				"failed to alloc channel resources\n");
		goto err_no_irq;
	}

	/* enable Crypto DMA engine */
	rc = al_ssm_dma_state_set(&device->hal_crypto, UDMA_NORMAL);

	err = al_crypto_setup_interrupts(device);

	if (err) {
		dev_err(&device->pdev->dev, "failed to setup interrupts\n");
		goto err_no_irq;
	}

	goto done;

err_no_irq:
	al_crypto_free_channels(device);
err_no_chan:
	kfree(device->ssm_dma_params.name);
done:
	return err;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_core_terminate(struct al_crypto_device *device)
{
	int status = 0;

	dev_dbg(&device->pdev->dev, "%s\n", __func__);

	al_crypto_free_channels(device);

	kfree(device->ssm_dma_params.name);

	kmem_cache_destroy(device->cache);

	return status;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_init_channels(struct al_crypto_device *device,
				   int max_channels)
{
	int i;
	struct al_crypto_chan *chan;

	for (i = 0; i < max_channels; i++) {
		chan = kzalloc(sizeof(struct al_crypto_chan), GFP_KERNEL);
		if (!chan)
			break;

		al_crypto_init_channel(device, chan, i);
	}

	device->num_channels = i;

	return i;
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_init_channel(struct al_crypto_device *device,
				   struct al_crypto_chan *chan, int idx)
{
	unsigned long data = (unsigned long)chan;

	dev_dbg(&device->pdev->dev, "%s: idx=%d\n",
		__func__, idx);

	chan->device = device;
	chan->idx = idx;
	chan->hal_crypto = &device->hal_crypto;

	AL_CRYPTO_STATS_INIT_LOCK(&chan->stats_gen_lock);
	spin_lock_init(&chan->prep_lock);
	spin_lock_init(&chan->cleanup_lock);

	device->channels[idx] = chan;

	INIT_LIST_HEAD(&chan->cache_lru_list);
	chan->cache_lru_count = 0;

	tasklet_init(&chan->cleanup_task, al_crypto_cleanup_tasklet, data);

	crypto_init_queue(&chan->sw_queue, 1);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_unmask_interrupts(struct al_crypto_device *device,
					bool single_interrupt)
{
	/* enable group D summary */
	u32 group_a_mask = AL_INT_GROUP_A_GROUP_D_SUM;
	u32 group_b_mask = (1 << device->num_channels) - 1; /* bit per Rx q*/
	u32 group_d_mask = AL_INT_GROUP_D_ALL;

	struct unit_regs __iomem *regs_base = (struct unit_regs __iomem *)device->udma_regs_base;

	if (single_interrupt)
		group_a_mask |= AL_INT_GROUP_A_GROUP_B_SUM;

	al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A, group_a_mask);
	al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_B, group_b_mask);
	al_udma_iofic_unmask(regs_base, AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_D, group_d_mask);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_config_crypto_app_interrupts
	(struct al_crypto_device *device)
{
	if (!device->crypto_regs_base)
		return;

	al_iofic_clear_cause(
		device->crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET,
		AL_INT_GROUP_A,
		AL_CRYPTO_APP_INT_A_ALL);

	al_iofic_unmask(
		device->crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET,
		AL_INT_GROUP_A,
		AL_CRYPTO_APP_INT_A_ALL);

	al_iofic_config(
		device->crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET,
		AL_INT_GROUP_A,
		INT_CONTROL_GRP_CLEAR_ON_READ |
		INT_CONTROL_GRP_MASK_MSI_X);

	/* Clear the interrupt reg */
	al_iofic_read_cause(
		device->crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET,
		AL_INT_GROUP_A);
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_iofic_config(struct al_crypto_device *device,
				  bool single_msix)
{
	enum al_iofic_mode iofic_mode;
	int int_moderation_group;

	if (single_msix) {
		iofic_mode = AL_IOFIC_MODE_MSIX_PER_GROUP;
		int_moderation_group = AL_INT_GROUP_A;
	} else {
		iofic_mode = AL_IOFIC_MODE_MSIX_PER_Q;
		int_moderation_group = AL_INT_GROUP_B;
	}

	if (al_udma_iofic_config(
		(struct unit_regs *)device->udma_regs_base,
		iofic_mode, 0x480, 0x480, 0x1E0, 0x1E0)) {

		dev_err(&device->pdev->dev, "al_udma_iofic_config failed!.\n");
		return -EIO;
	}

	al_iofic_moder_res_config(
		&((struct unit_regs *)(device->udma_regs_base))->gen.
						interrupt_regs.main_iofic,
						int_moderation_group, 15);

	al_crypto_config_crypto_app_interrupts(device);
	al_crypto_unmask_interrupts(device, single_msix);

	return 0;
}

/******************************************************************************
 *****************************************************************************/
static int al_crypto_setup_interrupts(struct al_crypto_device *device)
{
	struct al_crypto_chan *chan;
	struct pci_dev *pdev = device->pdev;
	struct device *dev = &pdev->dev;
	struct msix_entry *msix;
	int i, msixcnt;
	unsigned int cpu;
	int err = -EINVAL;
	int devm_alloc_num = 0;

	if (al_crypto_get_use_single_msix())
		goto msix_single_vector;

	/* The number of MSI-X vectors should equal the number of channels + 1
	 * for group D */
	msixcnt = device->num_channels + 1;

	for (i = 0; i < device->num_channels; i++)
		device->msix_entries[i].entry =
			AL_INT_MSIX_RX_COMPLETION_START + i;

	device->msix_entries[device->num_channels].entry =
			AL_INT_MSIX_GROUP_A_SUM_D_IDX;

	err = pci_enable_msix_exact(pdev, device->msix_entries, msixcnt);

	if (err < 0) {
		dev_err(dev, "pci_enable_msix failed! using intx instead.\n");
		goto intx;
	}

	if (err > 0) {
		dev_err(dev, "pci_enable_msix failed! msix_single_vector.\n");
		goto msix_single_vector;
	}

	for (i = 0; i < device->num_channels; i++) {
		msix = &device->msix_entries[i];

		chan = device->channels[i];

		dev_dbg(dev, "%s: requesting irq %d\n", __func__, msix->vector);

		snprintf(device->irq_tbl[i].name, AL_CRYPTO_IRQNAME_SIZE,
			 "al-crypto-comp-%d@pci:%s", i,
			 pci_name(pdev));

		err = devm_request_irq(
			dev,
			msix->vector,
			al_crypto_do_interrupt_msix,
			0,
			device->irq_tbl[i].name,
			chan);

		if (err) {
			dev_err(dev, "devm_request_irq failed!.\n");
			goto err_free_devm;
		}

		devm_alloc_num++;

		cpu = cpumask_next((i % num_online_cpus() - 1), cpu_online_mask);
		cpumask_set_cpu(cpu, &chan->affinity_hint_mask);

		irq_set_affinity_hint(msix->vector, &chan->affinity_hint_mask);
	}

	snprintf(device->irq_tbl[device->num_channels].name,
		AL_CRYPTO_IRQNAME_SIZE,
		 "al-crypto-interrupt-group-d@pci:%s",
		 pci_name(pdev));

	err = devm_request_irq(
		dev,
		device->msix_entries[device->num_channels].vector,
		al_crypto_do_interrupt_group_d,
		0,
		device->irq_tbl[device->num_channels].name,
		device);

	if (err) {
		dev_err(dev, "devm_request_irq failed!.\n");
		goto err_free_devm;
	}

	err = al_crypto_iofic_config(device, false);
	if (err)
		return err;

	goto done;

msix_single_vector:
	msix = &device->msix_entries[0];

	msix->entry = 0;

	tasklet_init(&device->cleanup_task,
			al_crypto_cleanup_tasklet_msix_rx,
			(unsigned long)device);

	err = pci_enable_msix_exact(pdev, device->msix_entries, 1);

	if (err) {
		pci_disable_msix(pdev);
		goto intx;
	}

	snprintf(device->irq_tbl[0].name, AL_CRYPTO_IRQNAME_SIZE,
		 "al-crypto-msix-all@pci:%s", pci_name(pdev));

	err = devm_request_irq(
		dev,
		msix->vector,
		al_crypto_do_interrupt_msix_rx,
		0,
		device->irq_tbl[0].name,
		device);

	if (err) {
		dev_err(dev, "devm_request_irq failed!.\n");
		pci_disable_msix(pdev);
		goto intx;
	}

	devm_alloc_num = 1;

	err = al_crypto_iofic_config(device, true);
	if (err)
		return err;
	goto done;

intx:
	tasklet_init(&device->cleanup_task,
			al_crypto_cleanup_tasklet_legacy,
			(unsigned long)device);

	snprintf(device->irq_tbl[0].name, AL_CRYPTO_IRQNAME_SIZE,
		 "al-crypto-intx-all@pci:%s", pci_name(pdev));

	err = devm_request_irq(dev, pdev->irq, al_crypto_do_interrupt_legacy,
			       IRQF_SHARED, device->irq_tbl[0].name, device);
	if (err)
		goto err_no_irq;

	if (al_udma_iofic_config(
		(struct unit_regs *)device->udma_regs_base,
		AL_IOFIC_MODE_LEGACY, 0x480, 0x480, 0x1E0, 0x1E0)) {
		dev_err(dev, "al_udma_iofic_config failed!.\n");
		return -EIO;
	}

	al_crypto_config_crypto_app_interrupts(device);
	al_crypto_unmask_interrupts(device, true);

done:
	device->num_irq_used = devm_alloc_num;
	return 0;

err_free_devm:

	for (i = 0; i < devm_alloc_num; i++) {
		msix = &device->msix_entries[i];
		chan = device->channels[i];
		irq_set_affinity_hint(msix->vector, NULL);
		devm_free_irq(dev, msix->vector, chan);
		return -EIO;
	}

err_no_irq:
	/* Disable all interrupt generation */

	dev_err(dev, "no usable interrupts\n");
	return err;
}

/******************************************************************************
 *****************************************************************************/
/* Free tx and rx descriptor rings for all channels
 */
static void al_crypto_free_channels(struct al_crypto_device *device)
{
	int i;

	for (i = 0; i < device->num_channels; i++) {
		al_crypto_free_chan_resources(device->channels[i]);
		al_crypto_free_channel(device->channels[i]);
	}

	for (i = 0; i < device->num_irq_used; i++)
		irq_set_affinity_hint(device->msix_entries[i].vector, NULL);
}

/******************************************************************************
 *****************************************************************************/
/* Allocate/initialize tx and rx descriptor rings for all channels
 */
static int al_crypto_alloc_channels(struct al_crypto_device *device)
{
	int i, j;
	int err = -EINVAL;

	for (i = 0; i < device->num_channels; i++) {
		err = al_crypto_alloc_chan_resources(device->channels[i]);

		if (err < 0) {
			dev_err(
				&device->pdev->dev,
				"failed to alloc resources for channel %d\n",
				i);

			for (j = 0; j < i; j++) {
				al_crypto_free_chan_resources(
						device->channels[j]);
			}
			return err;
		}
	}

	return 0;
}

/******************************************************************************
 *****************************************************************************/
static inline bool al_crypto_is_crypt_auth_chan(struct al_crypto_chan *chan)
{
	struct al_crypto_device *device = chan->device;
	return (chan->idx < device->num_channels - device->crc_channels);
}

/******************************************************************************
 *****************************************************************************/
/* Allocate/initialize tx and rx descriptor rings for one channel
 */
static int al_crypto_alloc_chan_resources(struct al_crypto_chan *chan)
{
	struct al_crypto_device *device = chan->device;
	struct device *dev = to_dev(chan);
	struct al_crypto_sw_desc **sw_ring;
	struct al_udma_q_params tx_params;
	struct al_udma_q_params rx_params;
	struct al_udma_m2s_pkt_len_conf conf;
	struct al_udma *crypto_udma;

	int rc = 0;
	int tx_descs_order;
	int rx_descs_order;
	int ring_alloc_order;

	dev_dbg(dev, "%s: idx=%d\n", __func__, chan->idx);

	/* have we already been set up? */
	if (chan->sw_ring)
		return 1 << chan->alloc_order;

	tx_descs_order = al_crypto_get_tx_descs_order();
	rx_descs_order = al_crypto_get_rx_descs_order();
	ring_alloc_order = al_crypto_get_ring_alloc_order();

	chan->tx_descs_num = 1 << tx_descs_order;
	chan->rx_descs_num = 1 << rx_descs_order;

	/* allocate coherent memory for Tx submission descriptors */
	chan->tx_dma_desc_virt = dma_alloc_coherent(dev,
						    chan->tx_descs_num *
						    sizeof(union al_udma_desc),
						    &chan->tx_dma_desc,
						    GFP_KERNEL);
	if (chan->tx_dma_desc_virt == NULL) {
		dev_err(dev, "failed to allocate %d bytes of coherent memory for Tx submission descriptors\n",
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
		dev_err(dev, "failed to allocate %d bytes of coherent memory for Rx submission descriptors\n",
			chan->rx_descs_num * sizeof(union al_udma_desc));

		al_crypto_free_chan_resources(chan);
		return -ENOMEM;
	}
	dev_dbg(dev, "allocted rx descriptor ring: virt 0x%p phys 0x%llx\n",
		chan->rx_dma_desc_virt, (u64)chan->rx_dma_desc);

	/* allocate coherent memory for Rx completion descriptors */
	chan->rx_dma_cdesc_virt = dma_alloc_coherent(dev,
						     chan->rx_descs_num *
						     AL_CRYPTO_RX_CDESC_SIZE,
						     &chan->rx_dma_cdesc,
						     GFP_KERNEL);
	if (chan->rx_dma_cdesc_virt == NULL) {
		dev_err(dev, "failed to allocate %d bytes of coherent memory for Rx completion descriptors\n",
			chan->rx_descs_num * AL_CRYPTO_RX_CDESC_SIZE);

		al_crypto_free_chan_resources(chan);
		return -ENOMEM;
	}

	/* clear the Rx completion descriptors to avoid false positive */
	memset(
		chan->rx_dma_cdesc_virt,
		0,
		chan->rx_descs_num * AL_CRYPTO_RX_CDESC_SIZE);

	dev_dbg(
		dev,
		"allocted rx completion desc ring: virt 0x%p phys 0x%llx\n",
		chan->rx_dma_cdesc_virt, (u64)chan->rx_dma_cdesc);

	rc = al_ssm_dma_handle_get(
		&device->hal_crypto,
		UDMA_TX,
		&crypto_udma);
	if (rc) {
		dev_err(to_dev(chan), "al_crypto_dma_handle_get failed\n");
		al_crypto_free_chan_resources(chan);
		return rc;
	}

	conf.encode_64k_as_zero = true;
	conf.max_pkt_size = 0xfffff;
	al_udma_m2s_packet_size_cfg_set(crypto_udma, &conf);

	tx_params.size = chan->tx_descs_num;
	tx_params.desc_base = chan->tx_dma_desc_virt;
	tx_params.desc_phy_base = chan->tx_dma_desc;
	tx_params.cdesc_base = NULL; /* don't use Tx completion ring */
	tx_params.cdesc_phy_base = 0;
	tx_params.cdesc_size = AL_CRYPTO_TX_CDESC_SIZE; /* size is needed */

	rx_params.size = chan->rx_descs_num;
	rx_params.desc_base = chan->rx_dma_desc_virt;
	rx_params.desc_phy_base = chan->rx_dma_desc;
	rx_params.cdesc_base = chan->rx_dma_cdesc_virt;
	rx_params.cdesc_phy_base = chan->rx_dma_cdesc;
	rx_params.cdesc_size = AL_CRYPTO_RX_CDESC_SIZE;

	/* alloc sw descriptors */
	if (ring_alloc_order < AL_CRYPTO_SW_RING_MIN_ORDER) {
		dev_err(
			dev,
			"%s: ring_alloc_order = %d < %d!\n",
			__func__,
			ring_alloc_order,
			AL_CRYPTO_SW_RING_MIN_ORDER);

		al_crypto_free_chan_resources(chan);
		return -EINVAL;
	} else if (ring_alloc_order > AL_CRYPTO_SW_RING_MAX_ORDER) {
		dev_err(
			dev,
			"%s: ring_alloc_order = %d > %d!\n",
			__func__,
			ring_alloc_order,
			AL_CRYPTO_SW_RING_MAX_ORDER);

		al_crypto_free_chan_resources(chan);
		return -EINVAL;
	} else if (ring_alloc_order > rx_descs_order) {
		dev_warn(
			dev,
			"%s: ring_alloc_order > rx_descs_order (%d>%d)!\n",
			__func__,
			ring_alloc_order,
			rx_descs_order);

	}

	sw_ring = al_crypto_alloc_sw_ring(chan, ring_alloc_order, GFP_KERNEL);
	if (!sw_ring) {
		dev_err(
			dev,
			"%s: sw ring alloc failed! ring_alloc_order = %d\n",
			__func__,
			ring_alloc_order);

		al_crypto_free_chan_resources(chan);
		return -ENOMEM;
	}

	spin_lock_bh(&chan->cleanup_lock);
	spin_lock_bh(&chan->prep_lock);
	chan->sw_ring = sw_ring;
	chan->head = 0;
	chan->tail = 0;
	chan->alloc_order = ring_alloc_order;
	chan->type = al_crypto_is_crypt_auth_chan(chan) ?
			AL_CRYPT_AUTH_Q : AL_MEM_CRC_MEMCPY_Q;

	chan->cache_entries_num = al_crypto_is_crypt_auth_chan(chan) ?
			(CACHED_SAD_SIZE / (device->num_channels -
					device->crc_channels)) :
			(CRC_IV_CACHE_SIZE / device->crc_channels);

	rc = al_ssm_dma_q_init(&device->hal_crypto, chan->idx,
				&tx_params, &rx_params, chan->type);
	if (rc) {
		dev_err(dev, "failed to initialize hal q %d. rc %d\n",
			chan->idx, rc);

		spin_unlock_bh(&chan->prep_lock);
		spin_unlock_bh(&chan->cleanup_lock);
		al_crypto_free_chan_resources(chan);
		return -ENOMEM;
	}

	spin_unlock_bh(&chan->prep_lock);
	spin_unlock_bh(&chan->cleanup_lock);

	/* should we return less ?*/
	return  1 << chan->alloc_order;
}

/******************************************************************************
 *****************************************************************************/
/* Free tx and rx descriptor rings for one channel
 */
static void al_crypto_free_chan_resources(struct al_crypto_chan *chan)
{
	struct device *dev = to_dev(chan);

	dev_dbg(dev, "%s: idx=%d\n", __func__, chan->idx);

	tasklet_disable(&chan->cleanup_task);

	al_crypto_cleanup_fn(chan, 0);

	spin_lock_bh(&chan->cleanup_lock);

	al_crypto_free_sw_ring(chan->sw_ring, chan , 1 << chan->alloc_order);

	if (chan->tx_dma_desc_virt != NULL) {
		dma_free_coherent(dev,
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
		dma_free_coherent(dev, chan->rx_descs_num * AL_CRYPTO_RX_CDESC_SIZE,
				  chan->rx_dma_cdesc_virt, chan->rx_dma_cdesc);
		chan->rx_dma_desc_virt = NULL;
	}

	spin_unlock_bh(&chan->cleanup_lock);

	return;
}

/******************************************************************************
 *****************************************************************************/
/* Alloc sw descriptors ring
 */
static struct al_crypto_sw_desc **al_crypto_alloc_sw_ring(struct al_crypto_chan *chan,
							  int order, gfp_t flags)
{
	struct al_crypto_sw_desc **ring;
	int descs = 1 << order;
	int i;

	/* allocate the array to hold the software ring */
	ring = kcalloc(descs, sizeof(*ring), flags);
	if (!ring)
		return NULL;

	for (i = 0; i < descs; i++) {
		ring[i] = al_crypto_alloc_ring_ent(chan, flags);
		if (!ring[i]) {
			al_crypto_free_sw_ring(ring, chan , i);
			return NULL;
		}
		/* set_desc_id(ring[i], i); */
	}

	return ring;
}

/******************************************************************************
 *****************************************************************************/
/* Free sw descriptors ring
*/
static void al_crypto_free_sw_ring(struct al_crypto_sw_desc **ring,
				   struct al_crypto_chan *chan,
				   int size)
{
	int i;

	for (i = 0; i < size; i++)
		al_crypto_free_ring_ent(ring[i], chan);

	kfree(ring);
}

/******************************************************************************
 *****************************************************************************/
/* Alloc sw descriptor
 */
static struct al_crypto_sw_desc *al_crypto_alloc_ring_ent(struct al_crypto_chan *chan,
							  gfp_t flags)
{
	struct al_crypto_sw_desc *desc;

	desc = kmem_cache_zalloc(chan->device->cache, flags);
	if (!desc)
		return NULL;

	return desc;
}

/******************************************************************************
 *****************************************************************************/
/* Free sw descriptor
 */
static void al_crypto_free_ring_ent(struct al_crypto_sw_desc *desc,
				    struct al_crypto_chan *chan)
{
	kmem_cache_free(chan->device->cache, desc);
}

/******************************************************************************
 *****************************************************************************/
/* Get sw desc
 */
int al_crypto_get_sw_desc(struct al_crypto_chan *chan, int num)
{
	dev_dbg(to_dev(chan), "%s: idx=%d num=%d ring_space=%d ring_size=%d\n",
		__func__, chan->idx, num, al_crypto_ring_space(chan),
		al_crypto_ring_size(chan));

	if (likely(al_crypto_ring_space(chan) >= num)) {
		dev_dbg(to_dev(chan), "%s: head=%x tail=%x\n",
			__func__, chan->head, chan->tail);
		return 0;
	}

	return -ENOMEM;
}

/******************************************************************************
 *****************************************************************************/
/* Handler used for vector-per-channel interrupt mode
 */
static irqreturn_t al_crypto_do_interrupt_msix(int irq, void *data)
{
	struct al_crypto_chan *chan = data;

	dev_dbg(to_dev(chan), "%s: irq=%d idx=%d\n",
		__func__, irq, chan->idx);

	tasklet_schedule(&chan->cleanup_task);

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
/* Handler for interrupt group d
 */
static irqreturn_t al_crypto_do_interrupt_group_d(int irq, void *data)
{
	struct al_crypto_device *device = data;

	dev_dbg(&device->pdev->dev, "%s: irq=%d\n", __func__, irq);

	al_crypto_group_d_errors_handler(device);

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
/* Handler used for vector-per-group interrupt mode
 */
static irqreturn_t al_crypto_do_interrupt_msix_rx(int irq, void *data)
{
	struct al_crypto_device *device = data;

	dev_dbg(&device->pdev->dev, "%s: irq=%d\n", __func__, irq);

	tasklet_schedule(&device->cleanup_task);

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
/* Handler used for legacy interrupt mode
 */
static irqreturn_t al_crypto_do_interrupt_legacy(int irq, void *data)
{
	struct al_crypto_device *device = data;

	dev_dbg(&device->pdev->dev, "%s: irq=%d\n", __func__, irq);

	al_udma_iofic_mask((struct unit_regs *)device->udma_regs_base,
			   AL_UDMA_IOFIC_LEVEL_PRIMARY, AL_INT_GROUP_A,
			   AL_INT_GROUP_A_GROUP_B_SUM | AL_INT_GROUP_A_GROUP_D_SUM);

	tasklet_schedule(&device->cleanup_task);

	return IRQ_HANDLED;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_cleanup_fn(struct al_crypto_chan *chan, int from_tasklet)
{
	struct al_crypto_sw_desc *desc;
	uint32_t comp_status;
	u16 active;
	int idx, i, rc;

	spin_lock_bh(&chan->cleanup_lock);
	idx = chan->tail;

	active = al_crypto_ring_active(chan);

	dev_dbg(to_dev(chan), "%s: idx=%d head=%#x tail=%#x from_tasklet=%d active=%d\n",
		__func__, chan->idx, chan->head, chan->tail, from_tasklet, active);

	AL_CRYPTO_STATS_SET(chan->stats_comp.max_active_descs,
			    (active > chan->stats_comp.max_active_descs) ?
			    active : chan->stats_comp.max_active_descs);

	for (i = 0; i < active; i++) {
		rc = al_crypto_dma_completion(chan->hal_crypto, chan->idx,
					      &comp_status);

		/* if no completed transaction found -> exit */
		if (rc == 0) {
			dev_dbg(to_dev(chan), "%s: idx=%d no completion\n",
				__func__, chan->idx);
			break;
		}

		dev_dbg(to_dev(chan), "%s: idx=%d entry=%d comp_status=%u\n",
			__func__, chan->idx, idx + i, comp_status);

		/* This will instruct the CPU to make sure the index is up to
		   date before reading the new item */
		smp_read_barrier_depends();

		desc = al_crypto_get_ring_ent(chan, idx + i);

		if (desc->req_type == AL_CRYPTO_REQ_SKCIPHER)
			al_crypto_skcipher_cleanup_single(chan, desc, comp_status);
		else if (desc->req_type == AL_CRYPTO_REQ_AEAD)
			al_crypto_aead_cleanup_single(chan, desc, comp_status);
		else if (desc->req_type == AL_CRYPTO_REQ_AHASH)
			al_crypto_cleanup_single_ahash(chan, desc, comp_status);
		else if (desc->req_type == AL_CRYPTO_REQ_CRC)
			al_crypto_cleanup_single_crc(chan, desc, comp_status);
		else {
			dev_err(to_dev(chan), "alg type %d is not supported\n",
				desc->req_type);
			BUG();
		}
	}

	/* This will make sure the CPU has finished reading the item
	   before it writes the new tail pointer, which will erase the item */
	smp_mb(); /* finish all descriptor reads before incrementing tail */

	chan->tail = idx + i;

	dev_dbg(to_dev(chan), "%s: idx=%d head=%x tail=%x\n",
		__func__, chan->idx, chan->head, chan->tail);

	/* Keep track of redundant interrupts - interrupts that doesn't
	   yield completions */
	if (unlikely(from_tasklet && (!i)))
		AL_CRYPTO_STATS_INC(chan->stats_comp.redundant_int_cnt, 1);

	spin_unlock_bh(&chan->cleanup_lock);

	/* Currently only skcipher reqs can be backlogged */
	if (i && chan->sw_queue.qlen)
		al_crypto_skcipher_process_queue(chan);

	dev_dbg(to_dev(chan), "%s: idx=%d\n", __func__, chan->idx);

	return i;
};

/******************************************************************************
 *****************************************************************************/
static void al_crypto_group_d_errors_handler(struct al_crypto_device *device)
{
	u32 read_cause_group_d, read_cause_crypto_reg_a;

	read_cause_group_d = al_udma_iofic_read_cause(device->udma_regs_base,
						      AL_UDMA_IOFIC_LEVEL_PRIMARY,
						      AL_INT_GROUP_D);

	dev_err(&device->pdev->dev,
		"%s: got error - %08x from group D\n",
		__func__, read_cause_group_d);

	if (read_cause_group_d & AL_INT_GROUP_D_APP_EXT_INT) {
		read_cause_crypto_reg_a =
			al_iofic_read_cause(
				device->crypto_regs_base +
				AL_CRYPTO_APP_IOFIC_OFFSET,
				AL_INT_GROUP_A);

		al_iofic_unmask(
			device->crypto_regs_base + AL_CRYPTO_APP_IOFIC_OFFSET,
			AL_INT_GROUP_A,
			AL_CRYPTO_APP_INT_A_ALL);

		al_udma_iofic_unmask(
			(struct unit_regs *)device->udma_regs_base,
			AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_A,
			AL_INT_GROUP_A_GROUP_D_SUM);

		dev_err(&device->pdev->dev,
				"got error - %08x from APP group A\n",
				read_cause_crypto_reg_a);
	}
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_cleanup_tasklet(unsigned long data)
{
	struct al_crypto_chan *chan = (struct al_crypto_chan *)data;
	int num_completed;

	dev_dbg(to_dev(chan), "%s: idx=%d head=%x tail=%x sw_desc_num_locked=%d tx_desc_produced=%d\n",
		__func__, chan->idx, chan->head, chan->tail,
		chan->sw_desc_num_locked, chan->tx_desc_produced);

	num_completed = al_crypto_cleanup_fn(chan, 1);

	dev_dbg(to_dev(chan), "%s: idx=%d num_completed=%d\n",
		__func__, chan->idx, num_completed);

	if (unlikely(num_completed < 0))
		dev_err(to_dev(chan), "al_crypto_cleanup_fn failed\n");

	al_udma_iofic_unmask((struct unit_regs *)chan->device->udma_regs_base,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_B,
			     1 << chan->idx);
}

/******************************************************************************
 *****************************************************************************/
static inline void al_crypto_cleanup_q_group_fn(struct al_crypto_device *device,
						int group)
{
	int num_completed;
	unsigned int status;
	int i;

	status = al_udma_iofic_read_cause((struct unit_regs *)device->udma_regs_base,
					  AL_UDMA_IOFIC_LEVEL_PRIMARY,
					  group);

	for (i = 0; i < device->num_channels; i++) {
		if (status & AL_BIT(i)) {
			num_completed = al_crypto_cleanup_fn(device->channels[i], 1);

			if (unlikely(num_completed < 0))
				dev_err(to_dev(device->channels[i]),
					"al_crypto_cleanup_fn failed\n");
		}
	}
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_cleanup_tasklet_msix_rx(unsigned long data)
{
	struct al_crypto_device *device = (struct al_crypto_device *)data;
	unsigned int status;

	status = al_udma_iofic_read_cause(
			(struct unit_regs *)device->udma_regs_base,
			AL_UDMA_IOFIC_LEVEL_PRIMARY,
			AL_INT_GROUP_D);

	if (unlikely(status))
		al_crypto_group_d_errors_handler(device);

	al_crypto_cleanup_q_group_fn(device, AL_INT_GROUP_B);

	al_udma_iofic_unmask((struct unit_regs *)device->udma_regs_base,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_A,
			     AL_INT_GROUP_A_GROUP_B_SUM);
}

/******************************************************************************
 *****************************************************************************/
static void al_crypto_cleanup_tasklet_legacy(unsigned long data)
{
	struct al_crypto_device *device = (struct al_crypto_device *)data;
	unsigned int status;

	status = al_udma_iofic_read_cause((struct unit_regs *)device->udma_regs_base,
					  AL_UDMA_IOFIC_LEVEL_PRIMARY,
					  AL_INT_GROUP_A);

	if (unlikely(status & AL_INT_GROUP_A_GROUP_D_SUM))
		al_crypto_group_d_errors_handler(device);
	else if (status & AL_INT_GROUP_A_GROUP_B_SUM)
		al_crypto_cleanup_q_group_fn(device, AL_INT_GROUP_B);

	al_udma_iofic_unmask((struct unit_regs *)device->udma_regs_base,
			     AL_UDMA_IOFIC_LEVEL_PRIMARY,
			     AL_INT_GROUP_A,
			     AL_INT_GROUP_A_GROUP_B_SUM);
}

/******************************************************************************
 *****************************************************************************/
/* Update the LRU list according to the currently accessed entry
 */
void al_crypto_cache_update_lru(struct al_crypto_chan *chan,
				struct al_crypto_cache_state *ctx)
{
	struct list_head *ptr;
	struct al_crypto_cache_lru_entry *lru_entry = NULL;
	uint32_t list_idx = 0;

	/* skip update if cache not yet populated */
	if (unlikely(chan->cache_lru_count <= 1))
		return;

	list_for_each(ptr, &chan->cache_lru_list) {
		lru_entry = list_entry(ptr,
				       struct al_crypto_cache_lru_entry,
				       list);
		if (lru_entry->ctx == ctx)
			break;
		list_idx++;
	}

	/* The entry has to be in the list */
	BUG_ON(lru_entry->ctx != ctx);

	/* move to tail only if needed */
	if (list_idx != (chan->cache_lru_count - 1))
		list_move_tail(ptr, &chan->cache_lru_list);
}

/******************************************************************************
 *****************************************************************************/
/* Translate cache entry index in ring[0..3] to global index: [0..15] in sa
 * sa cache, [0..7] in crc iv cache
 */
static inline uint32_t al_crypto_ring_cache_idx(struct al_crypto_chan *chan, int cache_idx)
{
	struct al_crypto_device *device = chan->device;
	int chan_idx = chan->idx;

	if (chan->idx >= (device->num_channels - device->crc_channels))
		chan_idx = chan->idx -
			(device->num_channels - device->crc_channels);

	return (chan_idx * chan->cache_entries_num) + cache_idx;
}

/******************************************************************************
 *****************************************************************************/
/* Replace least recently used cache entry with current entry
 */
uint32_t al_crypto_cache_replace_lru(struct al_crypto_chan *chan,
				     struct al_crypto_cache_state *ctx,
				     struct al_crypto_cache_state **old_ctx)
{
	struct al_crypto_cache_lru_entry *lru_entry = NULL;

	if (chan->cache_lru_count < chan->cache_entries_num) {
		/* find a free entry */
		int i;
		for (i = 0; i < chan->cache_entries_num; i++) {
			lru_entry = &chan->cache_lru_entries[i];
			if (lru_entry->ctx && lru_entry->ctx->cached)
				continue;
			else
				break;
		}

		BUG_ON(!lru_entry);
		BUG_ON(i >= chan->cache_entries_num);

		lru_entry->cache_idx =
			al_crypto_ring_cache_idx(chan, i);
		lru_entry->ctx = ctx;

		list_add_tail(&lru_entry->list,
			      &chan->cache_lru_list);
		chan->cache_lru_count++;
		if (old_ctx)
			*old_ctx = NULL;
	} else {
		AL_CRYPTO_STATS_INC(chan->stats_prep.cache_misses, 1);
		lru_entry = list_first_entry(&chan->cache_lru_list,
					struct al_crypto_cache_lru_entry,
					list);
		/* Invalidate old ctx */
		lru_entry->ctx->cached = false;
		/* Return old ctx if needed */
		if (old_ctx)
			*old_ctx = lru_entry->ctx;
		/* Connect new ctx */
		lru_entry->ctx = ctx;
		/* Move current entry to end of LRU list */
		list_rotate_left(&chan->cache_lru_list);
	}

	ctx->cached = true;
	ctx->idx = lru_entry->cache_idx;
	return lru_entry->cache_idx;
}

/******************************************************************************
 *****************************************************************************/
/* Remove the entry from LRU list
 */
void al_crypto_cache_remove_lru(struct al_crypto_chan *chan,
				struct al_crypto_cache_state *ctx)
{
	struct list_head *ptr;
	struct al_crypto_cache_lru_entry *lru_entry = NULL;
	uint32_t list_idx = 0;

	/* lru list is empty */
	if (chan->cache_lru_count == 0)
		return;

	list_for_each(ptr, &chan->cache_lru_list) {
		lru_entry = list_entry(ptr,
				       struct al_crypto_cache_lru_entry,
				       list);
		if (lru_entry->ctx == ctx)
			break;
		list_idx++;
	}

	/* The entry has to be in the list */
	BUG_ON(lru_entry->ctx != ctx);

	list_del(ptr);
	lru_entry->ctx = NULL;
	chan->cache_lru_count--;
	ctx->cached = false;
}

/******************************************************************************
 *****************************************************************************/
/* Move ring tail to process prepared descriptors
 */
void al_crypto_tx_submit(struct al_crypto_chan *chan)
{
	dev_dbg(to_dev(chan), "%s: idx=%d head=%x tail=%x sw_desc_num_locked=%d tx_desc_produced=%d\n",
		__func__, chan->idx, chan->head, chan->tail,
		chan->sw_desc_num_locked, chan->tx_desc_produced);

	/* according to Documentation/circular-buffers.txt we should have */
	/* smp_wmb before intcrementing the head, however, the */
	/* al_crypto_dma_action contains writel() which implies dmb on ARM */
	/* so this smp_wmb() can be omitted on ARM platforms */
	/*smp_wmb();*/ /* commit the item before incrementing the head */
	chan->head += chan->sw_desc_num_locked;
	/* in our case the consumer (interrupt handler) will be waken up by */
	/* the hw, so we send the transaction to the hw after incrementing */
	/* the head */

	al_crypto_dma_action(chan->hal_crypto,
			     chan->idx, chan->tx_desc_produced);
}

/******************************************************************************
 *****************************************************************************/
/* Set interrupt moderation interval, each tick ~= 1.5usecs
 */
void al_crypto_set_int_moderation(struct al_crypto_device *device, int interval)
{
	int i;

	for (i = 0; i < device->num_channels; i++)
		al_iofic_msix_moder_interval_config(
			&((struct unit_regs *)(device->udma_regs_base))->gen.
						interrupt_regs.main_iofic,
						AL_INT_GROUP_B,
						i,
						interval);

	device->int_moderation = interval;
}

/******************************************************************************
 *****************************************************************************/
/* Get interrupt moderation interval
 */
int al_crypto_get_int_moderation(struct al_crypto_device *device)
{
	return device->int_moderation;
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_keylen_to_sa_aes_ksize(unsigned int keylen,
				     enum al_crypto_sa_aes_ksize *ksize)
{
	switch (keylen) {
	case 16: /* 128 bit */
		*ksize = AL_CRYPT_AES_128;
		return 0;
	case 24: /* 192 bit */
		*ksize = AL_CRYPT_AES_192;
		return 0;
	case 32: /* 256 bit */
		*ksize = AL_CRYPT_AES_256;
		return 0;
	default: /* Invalid key size */
		return 1;
	}
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_sa_aes_ksize_to_keylen(enum al_crypto_sa_aes_ksize ksize,
				     unsigned int *keylen)
{
	switch (ksize) {
	case AL_CRYPT_AES_128:
		*keylen = 16;
		return 0;
	case AL_CRYPT_AES_192:
		*keylen = 24;
		return 0;
	case AL_CRYPT_AES_256:
		*keylen = 32;
		return 0;
	default:
		return 1;
	}
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_hexdump_sgl(const char *level, struct scatterlist *sgl,
			   const char *name, off_t skip, int len, gfp_t gfp_flags)
{
	char prefix[64];
	void *buf;

	buf = kmalloc(len, gfp_flags);
	if (!buf)
		return;

	snprintf(prefix, sizeof(prefix), KBUILD_MODNAME ": %s: ", name);
	scatterwalk_map_and_copy(buf, sgl, skip, len, 0);
	print_hex_dump(KERN_INFO, prefix, DUMP_PREFIX_OFFSET, 16, 1, buf, len, false);

	kfree(buf);
}
