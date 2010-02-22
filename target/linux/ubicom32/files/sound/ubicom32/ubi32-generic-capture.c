/*
 * sound/ubicom32/ubi32-generic-capture.c
 *	Interface to ubicom32 virtual audio peripheral
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include "ubi32.h"

#define DRIVER_NAME "snd-ubi32-generic-capture"

/*
 * Module properties
 */
static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * Card private data free function
 */
void snd_ubi32_generic_capture_free(struct snd_card *card)
{
	/*
	 * Free all the fields in the snd_ubi32_priv struct
	 */
	// Nothing to free at this time because ubi32_priv just maintains pointers
}

/*
 * Ubicom audio driver probe() method.  Args change depending on whether we use
 * platform_device or i2c_device.
 */
static int snd_ubi32_generic_capture_probe(struct platform_device *dev)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	int err;

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-Generic-C", THIS_MODULE, sizeof(struct ubi32_snd_priv));

	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_generic_capture_free; /* Not sure if correct */
	ubi32_priv = card->private_data;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;
	ubi32_priv->is_capture = 1;

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, dev);
	if (err < 0) {
		snd_card_free(card);
		return err;
	}

	strcpy(card->driver, "Ubi32-Generic-C");
	strcpy(card->shortname, "Ubi32-Generic-C");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p",
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &dev->dev);

	/* Register the sound card */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_INFO "snd_card_register error\n");
	}

	/* Store card for access from other methods */
	platform_set_drvdata(dev, card);

	return 0;
}

/*
 * Ubicom audio driver remove() method
 */
static int __devexit snd_ubi32_generic_capture_remove(struct platform_device *dev)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;

	card = platform_get_drvdata(dev);
	ubi32_priv = card->private_data;
	snd_ubi32_pcm_remove(ubi32_priv);

	snd_card_free(platform_get_drvdata(dev));
	platform_set_drvdata(dev, NULL);
	return 0;
}

/*
 * Platform driver definition
 */
static struct platform_driver snd_ubi32_generic_capture_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = snd_ubi32_generic_capture_probe,
	.remove = __devexit_p(snd_ubi32_generic_capture_remove),
};

/*
 * snd_ubi32_generic_capture_init
 */
static int __init snd_ubi32_generic_capture_init(void)
{
	return platform_driver_register(&snd_ubi32_generic_capture_driver);
}
module_init(snd_ubi32_generic_capture_init);

/*
 * snd_ubi32_generic_capture_exit
 */
static void __exit snd_ubi32_generic_capture_exit(void)
{
	platform_driver_unregister(&snd_ubi32_generic_capture_driver);
}
module_exit(snd_ubi32_generic_capture_exit);

/*
 * Module properties
 */
//#if defined(CONFIG_SND_UBI32_AUDIO_I2C)
//MODULE_ALIAS("i2c:snd-ubi32");
//#endif
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices");
MODULE_LICENSE("GPL");
