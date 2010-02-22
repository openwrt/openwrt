/*
 * sound/ubicom32/ubi32-cs4350.c
 *	Interface to ubicom32 virtual audio peripheral - using CS4350 DAC
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

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <sound/core.h>
#include <sound/tlv.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include "ubi32.h"

#define DRIVER_NAME "snd-ubi32-cs4350"

/*
 * Module properties
 */
static const struct i2c_device_id snd_ubi32_cs4350_id[] = {
	{"cs4350", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32audio_id);

static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * The dB scale for the Cirrus Logic cs4350.  The output range is from
 * -127.5 dB to 0 dB.
 */
static const DECLARE_TLV_DB_SCALE(snd_ubi32_cs4350_db, -12750, 50, 0);

#define ubi32_cs4350_mute_info	snd_ctl_boolean_stereo_info

/*
 * Private data for cs4350 chip
 */
struct ubi32_cs4350_priv {
	/*
	 * The current volume settings
	 */
	uint8_t volume[2];

	/*
	 * Bitmask of mutes MSB (unused, ..., unused, right_ch, left_ch) LSB
	 */
	uint8_t mute;

	/*
	 * Lock to protect this struct because callbacks are not atomic.
	 */
	spinlock_t lock;
};

/*
 * The info for the cs4350.  The volume currently has one channel,
 * and 255 possible settings.
 */
static int ubi32_cs4350_volume_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 255; // 8 bits in cirrus logic cs4350 volume register
	return 0;
}

static int ubi32_cs4350_volume_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_cs4350_priv *cs4350_priv;
	unsigned long flags;

	cs4350_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4350_priv->lock, flags);

	ucontrol->value.integer.value[0] = cs4350_priv->volume[0];
	ucontrol->value.integer.value[1] = cs4350_priv->volume[1];

	spin_unlock_irqrestore(&cs4350_priv->lock, flags);

	return 0;
}

static int ubi32_cs4350_volume_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_cs4350_priv *cs4350_priv;
	unsigned long flags;
	int ret, changed;
	char send[2];
	uint8_t volume_reg_value_left, volume_reg_value_right;

	changed = 0;

	cs4350_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	volume_reg_value_left = 255 - (ucontrol->value.integer.value[0] & 0xFF);
	volume_reg_value_right = 255 - (ucontrol->value.integer.value[1] & 0xFF);

#if SND_UBI32_DEBUG
	snd_printk(KERN_INFO "Setting volume: writing %d,%d to CS4350 volume registers\n", volume_reg_value_left, volume_reg_value_right);
#endif
	spin_lock_irqsave(&cs4350_priv->lock, flags);

	if (cs4350_priv->volume[0] != ucontrol->value.integer.value[0]) {
		send[0] = 0x05; // left channel
		send[1] = volume_reg_value_left;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel A volume on CS4350\n");
			return changed;
		}
		cs4350_priv->volume[0] = ucontrol->value.integer.value[0];
		changed = 1;
	}

	if (cs4350_priv->volume[1] != ucontrol->value.integer.value[1]) {
		send[0] = 0x06; // right channel
		send[1] = volume_reg_value_right;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel B volume on CS4350\n");
			return changed;
		}
		cs4350_priv->volume[1] = ucontrol->value.integer.value[1];
		changed = 1;
	}

	spin_unlock_irqrestore(&cs4350_priv->lock, flags);

	return changed;
}

static struct snd_kcontrol_new ubi32_cs4350_volume __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
		  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
	.name = "PCM Playback Volume",
	.info = ubi32_cs4350_volume_info,
	.get = ubi32_cs4350_volume_get,
	.put = ubi32_cs4350_volume_put,
	.tlv.p = snd_ubi32_cs4350_db,
};

static int ubi32_cs4350_mute_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct ubi32_cs4350_priv *cs4350_priv;
	unsigned long flags;

	cs4350_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4350_priv->lock, flags);

	ucontrol->value.integer.value[0] = cs4350_priv->mute & 1;
	ucontrol->value.integer.value[1] = (cs4350_priv->mute & (1 << 1)) ? 1 : 0;

	spin_unlock_irqrestore(&cs4350_priv->lock, flags);

	return 0;
}

static int ubi32_cs4350_mute_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *ubi32_priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)ubi32_priv->client;
	struct ubi32_cs4350_priv *cs4350_priv;
	unsigned long flags;
	int ret, changed;
	char send[2];
	char recv[1];
	uint8_t mute;

        changed = 0;

	cs4350_priv = snd_ubi32_priv_get_drv(ubi32_priv);

	spin_lock_irqsave(&cs4350_priv->lock, flags);

	if ((cs4350_priv->mute & 1) != ucontrol->value.integer.value[0]) {
		send[0] = 0x04;
		ret = i2c_master_send(client, send, 1);
		if (ret != 1) {
			snd_printk(KERN_ERR "Failed to write to mute register: channel 0\n");
			return changed;
		}

		ret = i2c_master_recv(client, recv, 1);
		if (ret != 1) {
			snd_printk(KERN_ERR "Failed to read mute register: channel 0\n");
			return changed;
		}

		mute = recv[0];

		if (ucontrol->value.integer.value[0]) {
			cs4350_priv->mute |= 1;
			mute &= ~(1 << 4);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Unmuted channel A\n");
#endif
		} else {
			cs4350_priv->mute &= ~1;
			mute |= (1 << 4);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Muted channel A\n");
#endif
		}

		send[0] = 0x04;
		send[1] = mute;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel A mute on CS4350\n");
			return changed;
		}
		changed = 1;
	}

	if (((cs4350_priv->mute & 2) >> 1) != ucontrol->value.integer.value[1]) {
		send[0] = 0x04;
		ret = i2c_master_send(client, send, 1);
		if (ret != 1) {
			snd_printk(KERN_ERR "Failed to write to mute register: channel 1\n");
			return changed;
		}

		ret = i2c_master_recv(client, recv, 1);
		if (ret != 1) {
			snd_printk(KERN_ERR "Failed to read mute register: channel 1\n");
			return changed;
		}

		mute = recv[0];

		if (ucontrol->value.integer.value[1]) {
			cs4350_priv->mute |= (1 << 1);
			mute &= ~(1 << 3);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Unmuted channel B\n");
#endif
		} else {
			cs4350_priv->mute &= ~(1 << 1);
			mute |= (1 << 3);
#if SND_UBI32_DEBUG
			snd_printk(KERN_INFO "Muted channel B\n");
#endif
		}

		send[0] = 0x04;
		send[1] = mute;
		ret = i2c_master_send(client, send, 2);
		if (ret != 2) {
			snd_printk(KERN_ERR "Failed to set channel A mute on CS4350\n");
			return changed;
		}
		changed = 1;
	}

	spin_unlock_irqrestore(&cs4350_priv->lock, flags);

	return changed;
}

static struct snd_kcontrol_new ubi32_cs4350_mute __devinitdata = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE,
	.name = "PCM Playback Switch",
	.info = ubi32_cs4350_mute_info,
	.get = ubi32_cs4350_mute_get,
	.put = ubi32_cs4350_mute_put,
};

/*
 * snd_ubi32_cs4350_free
 *	Card private data free function
 */
void snd_ubi32_cs4350_free(struct snd_card *card)
{
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_cs4350_priv *cs4350_priv;

	ubi32_priv = card->private_data;
	cs4350_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	if (cs4350_priv) {
		kfree(cs4350_priv);
	}
}

/*
 * snd_ubi32_cs4350_dac_init
 */
static int snd_ubi32_cs4350_dac_init(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	char send[2];
	char recv[8];

	/*
	 * Initialize the CS4350 DAC over the I2C interface
	 */
	snd_printk(KERN_INFO "Initializing CS4350 DAC\n");

	/*
	 * Register 0x01: device/revid
	 */
	send[0] = 0x01;
	ret = i2c_master_send(client, send, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed 1st attempt to write to CS4350 register 0x01\n");
		goto fail;
	}
	ret = i2c_master_recv(client, recv, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed initial read of CS4350 registers\n");
		goto fail;
	}
	snd_printk(KERN_INFO "CS4350 DAC Device/Rev: %08x\n", recv[0]);

	/*
	 * Register 0x02: Mode control
	 *	I2S DIF[2:0] = 001, no De-Emphasis, Auto speed mode
	 */
	send[0] = 0x02;
	send[1] = 0x10;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set CS4350 to I2S mode\n");
		goto fail;
	}

	/*
	 * Register 0x05/0x06: Volume control
	 *	Channel A volume set to 0 dB
	 *	Channel B volume set to 0 dB
	 */
	send[0] = 0x05;
	send[1] = 0x00;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set channel A volume on CS4350\n");
		goto fail;
	}

	send[0] = 0x06;
	send[1] = 0x00;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set channel A volume on CS4350\n");
		goto fail;
	}

	/*
	 * Make sure the changes took place, this helps verify we are talking to
	 * the correct chip.
	 */
	send[0] = 0x81;
	ret = i2c_master_send(client, send, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed to initiate readback\n");
		goto fail;
	}

	ret = i2c_master_recv(client, recv, 8);
	if (ret != 8) {
		snd_printk(KERN_ERR "Failed second read of CS4350 registers\n");
		goto fail;
	}

	if ((recv[1] != 0x10) || (recv[4] != 0x00) || (recv[5] != 0x00)) {
		snd_printk(KERN_ERR "Failed to initialize CS4350 DAC\n");
		goto fail;
	}

	snd_printk(KERN_INFO "CS4350 DAC Initialized\n");
	return 0;

fail:
	return -ENODEV;
}

/*
 * snd_ubi32_cs4350_i2c_probe
 */
static int snd_ubi32_cs4350_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	struct ubi32_cs4350_priv *cs4350_priv;
	int err, ret;
	struct platform_device *pdev;

	pdev = client->dev.platform_data;
	if (!pdev) {
		return -ENODEV;
	}

	/*
	 * Initialize the CS4350 DAC
	 */
	ret = snd_ubi32_cs4350_dac_init(client, id);
	if (ret < 0) {
		/*
		 * Initialization failed.  Propagate the error.
		 */
		return ret;
	}

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-CS4350", THIS_MODULE, sizeof(struct ubi32_snd_priv));
	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_cs4350_free; /* Not sure if correct */
	ubi32_priv = card->private_data;

	/*
	 * CS4350 DAC has a minimum sample rate of 30khz and an
	 * upper limit of 216khz for it's auto-detect.
	 */
	ubi32_priv->min_sample_rate = 30000;
	ubi32_priv->max_sample_rate = 216000;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;
	ubi32_priv->client = client;

	/*
	 * Create our private data structure
	 */
	cs4350_priv = kzalloc(sizeof(struct ubi32_cs4350_priv), GFP_KERNEL);
	if (!cs4350_priv) {
		snd_card_free(card);
		return -ENOMEM;
	}
	snd_ubi32_priv_set_drv(ubi32_priv, cs4350_priv);
	spin_lock_init(&cs4350_priv->lock);

	/*
	 * Initial volume is set to max by probe function
	 */
	cs4350_priv->volume[0] = 0xFF;
	cs4350_priv->volume[1] = 0xFF;

	/*
	 * The CS4350 starts off unmuted (bit set = not muted)
	 */
	cs4350_priv->mute = 3;

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, pdev);
	if (err < 0) {
		snd_card_free(card);
		return err; /* What is err?  Need to include correct file */
	}

	strcpy(card->driver, "Ubi32-CS4350");
	strcpy(card->shortname, "Ubi32-CS4350");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p",
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &client->dev);

	/*
	 * Set up the mixer components
	 */
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_cs4350_volume, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "Failed to add volume mixer control\n");
	}
	err = snd_ctl_add(card, snd_ctl_new1(&ubi32_cs4350_mute, ubi32_priv));
	if (err) {
		snd_printk(KERN_WARNING "Failed to add mute mixer control\n");
	}

	/*
	 * Register the sound card
	 */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_WARNING "snd_card_register error\n");
	}

	/*
	 * Store card for access from other methods
	 */
	i2c_set_clientdata(client, card);

	return 0;
}

/*
 * snd_ubi32_cs4350_i2c_remove
 */
static int __devexit snd_ubi32_cs4350_i2c_remove(struct i2c_client *client)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;

	card = i2c_get_clientdata(client);

	ubi32_priv = card->private_data;
	snd_ubi32_pcm_remove(ubi32_priv);

	snd_card_free(i2c_get_clientdata(client));
	i2c_set_clientdata(client, NULL);

	return 0;
}

/*
 * I2C driver description
 */
static struct i2c_driver snd_ubi32_cs4350_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.id_table	= snd_ubi32_cs4350_id,
	.probe		= snd_ubi32_cs4350_i2c_probe,
	.remove		= __devexit_p(snd_ubi32_cs4350_i2c_remove),
};

/*
 * Driver init
 */
static int __init snd_ubi32_cs4350_init(void)
{
	return i2c_add_driver(&snd_ubi32_cs4350_driver);
}
module_init(snd_ubi32_cs4350_init);

/*
 * snd_ubi32_cs4350_exit
 */
static void __exit snd_ubi32_cs4350_exit(void)
{
	i2c_del_driver(&snd_ubi32_cs4350_driver);
}
module_exit(snd_ubi32_cs4350_exit);

/*
 * Module properties
 */
MODULE_ALIAS("i2c:" DRIVER_NAME);
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices CS4350");
MODULE_LICENSE("GPL");
