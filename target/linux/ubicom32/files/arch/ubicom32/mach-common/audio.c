/*
 * arch/ubicom32/mach-common/audio.c
 *   Generic initialization for Ubicom32 Audio
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

#include <linux/platform_device.h>
#include <linux/types.h>

#include <asm/devtree.h>
#include <asm/audio.h>
#include <asm/ubi32-pcm.h>

/*
 * The number of audio devices currently allocated, used for .id
 */
static int __initdata audio_device_count;

/*
 * The maximum number of resources (cards) that the audio will have.
 * Currently 3, a register space, and up to 2 interrupts.
 */
#define AUDIO_MAX_RESOURCES	3

/*
 * audio_device_alloc
 *	Checks the device tree and allocates a platform_device if found
 */
struct platform_device * __init audio_device_alloc(const char *driver_name,
		const char *node_name, const char *inst_name, int priv_bytes)
{
	struct platform_device *pdev;
	struct resource *res;
	struct audio_node *audio_node;
	struct ubi32pcm_platform_data *pdata;
	struct audio_dev_regs *adr;
	int idx;

	/*
	 * Check the device tree for the audio node
	 */
	audio_node = (struct audio_node *)devtree_find_node(node_name);
	if (!audio_node) {
		printk(KERN_WARNING "audio device '%s' not found\n", node_name);
		return NULL;
	}

	if (audio_node->version != AUDIONODE_VERSION) {
		printk(KERN_WARNING "audio node not compatible\n");
		return NULL;
	}

	/*
	 * Find the instance in this node
	 */
	adr = audio_node->regs->adr;
	for (idx = 0; idx < audio_node->regs->max_devs; idx++) {
		if ((adr->version == AUDIO_DEV_REGS_VERSION) &&
		   (strcmp(adr->name, inst_name) == 0)) {
			break;
		}
		adr++;
	}
	if (idx == audio_node->regs->max_devs) {
		printk(KERN_WARNING "audio inst '%s' not found in device '%s'\n", inst_name, node_name);
		return NULL;
	}

	/*
	 * Dynamically create the platform_device structure and resources
	 */
	pdev = kzalloc(sizeof(struct platform_device) +
		       sizeof(struct ubi32pcm_platform_data) +
		       priv_bytes , GFP_KERNEL);
	if (!pdev) {
		printk(KERN_WARNING "audio could not alloc pdev\n");
		return NULL;
	}

	res = kzalloc(sizeof(struct resource) * AUDIO_MAX_RESOURCES,
			GFP_KERNEL);
	if (!res) {
		kfree(pdev);
		printk(KERN_WARNING "audio could not alloc res\n");
		return NULL;
	}

	pdev->name = driver_name;
	pdev->id = audio_device_count++;
	pdev->resource = res;

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	res[0].start = (u32_t)(audio_node->regs);
	res[0].end = (u32_t)(audio_node->regs);
	res[0].flags = IORESOURCE_MEM;
	res[1 + AUDIO_TX_IRQ_RESOURCE].start = audio_node->dn.sendirq;
	res[1 + AUDIO_TX_IRQ_RESOURCE].flags = IORESOURCE_IRQ;
	res[1 + AUDIO_RX_IRQ_RESOURCE].start = audio_node->dn.recvirq;
	res[1 + AUDIO_RX_IRQ_RESOURCE].flags = IORESOURCE_IRQ;
	pdev->num_resources = 3;

	printk(KERN_INFO "Audio.%d '%s':'%s' found irq=%d/%d.%d regs=%p pdev=%p/%p\n",
		pdev->id, node_name, inst_name, audio_node->dn.sendirq,
		audio_node->dn.recvirq, idx, audio_node->regs, pdev, res);
	pdata = (struct ubi32pcm_platform_data *)(pdev + 1);
	pdev->dev.platform_data = pdata;
	pdata->node_name = node_name;
	pdata->inst_name = inst_name;
	pdata->inst_num = idx;
	if (priv_bytes) {
		pdata->priv_data = pdata + 1;
	}

	return pdev;
}
