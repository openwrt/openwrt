/*
 * drivers/uio/uio_ubicom32ring.c
 *
 * Userspace I/O platform driver for Ubicom32 ring buffers
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * Based on uio_ubicom32ring.c by Magnus Damm
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
#include <linux/uio_driver.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/stringify.h>

#include <asm/ip5000.h>
#include <asm/ubicom32ring.h>

#define DRIVER_NAME "uio_ubicom32ring"

struct uio_ubicom32ring_data {
	struct uio_info			*uioinfo;

	struct uio_ubicom32ring_regs	*regs;

	/*
	 * IRQ used to kick the ring buffer
	 */
	int				irq_tx;
	int				irq_rx;

	spinlock_t			lock;

	unsigned long			flags;

	char				name[0];
};

static irqreturn_t uio_ubicom32ring_handler(int irq, struct uio_info *dev_info)
{
	struct uio_ubicom32ring_data *priv = dev_info->priv;

	/* Just disable the interrupt in the interrupt controller, and
	 * remember the state so we can allow user space to enable it later.
	 */

	if (!test_and_set_bit(0, &priv->flags))
		disable_irq_nosync(irq);

	return IRQ_HANDLED;
}

static int uio_ubicom32ring_irqcontrol(struct uio_info *dev_info, s32 irq_on)
{
	struct uio_ubicom32ring_data *priv = dev_info->priv;
	unsigned long flags;

	/* Allow user space to enable and disable the interrupt
	 * in the interrupt controller, but keep track of the
	 * state to prevent per-irq depth damage.
	 *
	 * Serialize this operation to support multiple tasks.
	 */

	spin_lock_irqsave(&priv->lock, flags);

	if (irq_on & 2) {
		/*
		 * Kick the ring buffer (if we can)
		 */
		if (priv->irq_tx != 0xFF) {
			ubicom32_set_interrupt(priv->irq_tx);
		}
	}

	if (priv->irq_rx != 0xFF) {
		if (irq_on & 1) {
			if (test_and_clear_bit(0, &priv->flags))
				enable_irq(dev_info->irq);
		} else {
			if (!test_and_set_bit(0, &priv->flags))
				disable_irq(dev_info->irq);
		}
	}

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int uio_ubicom32ring_probe(struct platform_device *pdev)
{
	struct uio_info *uioinfo;
	struct uio_mem *uiomem;
	struct uio_ubicom32ring_data *priv;
	struct uio_ubicom32ring_regs *regs;
	struct resource *mem_resource;
	struct resource *irqtx_resource;
	struct resource *irqrx_resource;
	int ret = -EINVAL;
	int i;

	uioinfo = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
	if (!uioinfo) {
		dev_err(&pdev->dev, "unable to kmalloc\n");
		return -ENOMEM;
	}

	/*
	 * Allocate private data with some string space after
	 */
	i = sizeof(DRIVER_NAME) + 1;
	i += pdev->dev.platform_data ? strlen(pdev->dev.platform_data) : 0;
	priv = kzalloc(sizeof(struct uio_ubicom32ring_data) + i, GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "unable to kmalloc\n");
		kfree(uioinfo);
		return -ENOMEM;
	}

	strcpy(priv->name, DRIVER_NAME ":");
	if (pdev->dev.platform_data) {
		strcat(priv->name, pdev->dev.platform_data);
	}
	uioinfo->priv = priv;
	uioinfo->name = priv->name;
	uioinfo->version = "0.1";

	priv->uioinfo = uioinfo;
	spin_lock_init(&priv->lock);
	priv->flags = 0; /* interrupt is enabled to begin with */

	/*
	 * Get our resources, the IRQ_TX and IRQ_RX are optional.
	 */
	priv->irq_tx = 0xFF;
	irqtx_resource = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (irqtx_resource) {
		priv->irq_tx = irqtx_resource->start;
	}

	uioinfo->irq = -1;
	priv->irq_rx = 0xFF;
	irqrx_resource = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (irqrx_resource) {
		priv->irq_rx = irqrx_resource->start;
		uioinfo->irq = priv->irq_rx;
		uioinfo->handler = uio_ubicom32ring_handler;
	}

	mem_resource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem_resource || !mem_resource->start) {
		dev_err(&pdev->dev, "No valid memory resource found\n");
		ret = -ENODEV;
		goto fail;
	}
	regs = (struct uio_ubicom32ring_regs *)mem_resource->start;
	priv->regs = regs;

	if (regs->version != UIO_UBICOM32RING_REG_VERSION) {
		dev_err(&pdev->dev, "version %d not supported\n", regs->version);
		ret = -ENODEV;
		goto fail;
	}

	/*
	 * First range is the shared register space, if we have any
	 */
	uiomem = &uioinfo->mem[0];
	if (regs->regs_size) {
		uiomem->memtype = UIO_MEM_PHYS;
		uiomem->addr = (u32_t)regs->regs;
		uiomem->size = regs->regs_size;
		++uiomem;
		dev_info(&pdev->dev, "regs:%p (%u) / rings: %d found\n", regs->regs, regs->regs_size, regs->num_rings);
	} else {
		dev_info(&pdev->dev, "rings: %d found\n", regs->num_rings);
	}

	/*
	 * The rest of the range correspond to the rings
	 */
	for (i = 0; i < regs->num_rings; i++) {
		dev_info(&pdev->dev, "\t%d: entries:%d ring:%p\n",
			 i, regs->rings[i]->entries, &(regs->rings[i]->ring));
		if (uiomem >= &uioinfo->mem[MAX_UIO_MAPS]) {
			dev_warn(&pdev->dev, "device has more than "
					__stringify(MAX_UIO_MAPS)
					" I/O memory resources.\n");
			break;
		}

		uiomem->memtype = UIO_MEM_PHYS;
		uiomem->addr = (u32_t)&(regs->rings[i]->head);
		uiomem->size = (regs->rings[i]->entries * sizeof(u32_t)) +
				sizeof(struct uio_ubicom32ring_desc);
		++uiomem;
	}

	while (uiomem < &uioinfo->mem[MAX_UIO_MAPS]) {
		uiomem->size = 0;
		++uiomem;
	}

	/* This driver requires no hardware specific kernel code to handle
	 * interrupts. Instead, the interrupt handler simply disables the
	 * interrupt in the interrupt controller. User space is responsible
	 * for performing hardware specific acknowledge and re-enabling of
	 * the interrupt in the interrupt controller.
	 *
	 * Interrupt sharing is not supported.
	 */
	uioinfo->irq_flags = IRQF_DISABLED;
	uioinfo->irqcontrol = uio_ubicom32ring_irqcontrol;

	ret = uio_register_device(&pdev->dev, priv->uioinfo);
	if (ret) {
		dev_err(&pdev->dev, "unable to register uio device\n");
		goto fail;
	}

	platform_set_drvdata(pdev, priv);

	dev_info(&pdev->dev, "'%s' using irq: rx %d tx %d, regs %p\n",
		 priv->name, priv->irq_rx, priv->irq_tx, priv->regs);

	return 0;

fail:
	kfree(uioinfo);
	kfree(priv);
	return ret;
}

static int uio_ubicom32ring_remove(struct platform_device *pdev)
{
	struct uio_ubicom32ring_data *priv = platform_get_drvdata(pdev);

	uio_unregister_device(priv->uioinfo);
	kfree(priv->uioinfo);
	kfree(priv);
	return 0;
}

static struct platform_driver uio_ubicom32ring = {
	.probe = uio_ubicom32ring_probe,
	.remove = uio_ubicom32ring_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init uio_ubicom32ring_init(void)
{
	return platform_driver_register(&uio_ubicom32ring);
}

static void __exit uio_ubicom32ring_exit(void)
{
	platform_driver_unregister(&uio_ubicom32ring);
}

module_init(uio_ubicom32ring_init);
module_exit(uio_ubicom32ring_exit);

MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Userspace I/O driver for Ubicom32 ring buffers");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
