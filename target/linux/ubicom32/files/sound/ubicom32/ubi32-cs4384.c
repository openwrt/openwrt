/*
 * sound/ubicom32/ubi32-cs4384.c
 *	Interface to ubicom32 virtual audio peripheral - using CS4384 DAC
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
#include <linux/delay.h>
#include <sound/core.h>
#include <sound/tlv.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <asm/ip5000.h>
#include <asm/gpio.h>
#include <asm/audio.h>
#include <asm/ubi32-cs4384.h>
#include "ubi32.h"

#define DRIVER_NAME "snd-ubi32-cs4384"

/*
 * Module properties
 */
static const struct i2c_device_id snd_ubi32_cs4384_id[] = {
	{"cs4384", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ubicom32audio_id);

static int index = SNDRV_DEFAULT_IDX1; /* Index 0-MAX */

/*
 * Mixer properties
 */
enum {
	/*
	 * Be careful of changing the order of these IDs, they
	 * are used to index the volume array.
	 */
	SND_UBI32_CS4384_FRONT_ID,
	SND_UBI32_CS4384_SURROUND_ID,
	SND_UBI32_CS4384_CENTER_ID,
	SND_UBI32_CS4384_LFE_ID,
	SND_UBI32_CS4384_REAR_ID,

	/*
	 * This should be the last ID
	 */
	SND_UBI32_CS4384_LAST_ID,
};
static const u8_t snd_ubi32_cs4384_ch_ofs[] = {0, 2, 4, 5, 6};

static const DECLARE_TLV_DB_SCALE(snd_ubi32_cs4384_db, -12750, 50, 0);

#define snd_ubi32_cs4384_info_mute 	snd_ctl_boolean_stereo_info
#define snd_ubi32_cs4384_info_mute_mono	snd_ctl_boolean_mono_info

/*
 * Mixer controls
 */
static int snd_ubi32_cs4384_info_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo);
static int snd_ubi32_cs4384_get_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int snd_ubi32_cs4384_put_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int snd_ubi32_cs4384_get_mute(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);
static int snd_ubi32_cs4384_put_mute(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol);

/*
 * Make sure to update these if the structure below is changed
 */
#define SND_UBI32_MUTE_CTL_START	5
#define SND_UBI32_MUTE_CTL_END		9
static struct snd_kcontrol_new snd_ubi32_cs4384_controls[] __devinitdata = {
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Front Playback Volume",
		.info = snd_ubi32_cs4384_info_volume,
		.get = snd_ubi32_cs4384_get_volume,
		.put = snd_ubi32_cs4384_put_volume,
		.private_value = SND_UBI32_CS4384_FRONT_ID,
		.tlv = {
			.p = snd_ubi32_cs4384_db,
		},
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Surround Playback Volume",
		.info = snd_ubi32_cs4384_info_volume,
		.get = snd_ubi32_cs4384_get_volume,
		.put = snd_ubi32_cs4384_put_volume,
		.private_value = SND_UBI32_CS4384_SURROUND_ID,
		.tlv = {
			.p = snd_ubi32_cs4384_db,
		},
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Center Playback Volume",
		.info = snd_ubi32_cs4384_info_volume,
		.get = snd_ubi32_cs4384_get_volume,
		.put = snd_ubi32_cs4384_put_volume,
		.private_value = SND_UBI32_CS4384_CENTER_ID,
		.tlv = {
			.p = snd_ubi32_cs4384_db,
		},
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "LFE Playback Volume",
		.info = snd_ubi32_cs4384_info_volume,
		.get = snd_ubi32_cs4384_get_volume,
		.put = snd_ubi32_cs4384_put_volume,
		.private_value = SND_UBI32_CS4384_LFE_ID,
		.tlv = {
			.p = snd_ubi32_cs4384_db,
		},
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Rear Playback Volume",
		.info = snd_ubi32_cs4384_info_volume,
		.get = snd_ubi32_cs4384_get_volume,
		.put = snd_ubi32_cs4384_put_volume,
		.private_value = SND_UBI32_CS4384_REAR_ID,
		.tlv = {
			.p = snd_ubi32_cs4384_db,
		},
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Front Playback Switch",
		.info = snd_ubi32_cs4384_info_mute,
		.get = snd_ubi32_cs4384_get_mute,
		.put = snd_ubi32_cs4384_put_mute,
		.private_value = SND_UBI32_CS4384_FRONT_ID,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Surround Playback Switch",
		.info = snd_ubi32_cs4384_info_mute,
		.get = snd_ubi32_cs4384_get_mute,
		.put = snd_ubi32_cs4384_put_mute,
		.private_value = SND_UBI32_CS4384_SURROUND_ID,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Center Playback Switch",
		.info = snd_ubi32_cs4384_info_mute_mono,
		.get = snd_ubi32_cs4384_get_mute,
		.put = snd_ubi32_cs4384_put_mute,
		.private_value = SND_UBI32_CS4384_CENTER_ID,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "LFE Playback Switch",
		.info = snd_ubi32_cs4384_info_mute_mono,
		.get = snd_ubi32_cs4384_get_mute,
		.put = snd_ubi32_cs4384_put_mute,
		.private_value = SND_UBI32_CS4384_LFE_ID,
	},
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.access = SNDRV_CTL_ELEM_ACCESS_READWRITE |
			  SNDRV_CTL_ELEM_ACCESS_TLV_READ,
		.name = "Rear Playback Switch",
		.info = snd_ubi32_cs4384_info_mute,
		.get = snd_ubi32_cs4384_get_mute,
		.put = snd_ubi32_cs4384_put_mute,
		.private_value = SND_UBI32_CS4384_REAR_ID,
	},
};

/*
 * Our private data
 */
struct snd_ubi32_cs4384_priv {
	/*
	 * Array of current volumes
	 *	(L, R, SL, SR, C, LFE, RL, RR)
	 */
	uint8_t	volume[8];

	/*
	 * Bitmask of mutes
	 *	MSB (RR, RL, LFE, C, SR, SL, R, L) LSB
	 */
	uint8_t mute;

	/*
	 * Array of controls
	 */
	struct snd_kcontrol *kctls[ARRAY_SIZE(snd_ubi32_cs4384_controls)];

	/*
	 * Lock to protect our card
	 */
	spinlock_t lock;
};

/*
 * snd_ubi32_cs4384_info_volume
 */
static int snd_ubi32_cs4384_info_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	unsigned int id = (unsigned int)kcontrol->private_value;

	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	if ((id != SND_UBI32_CS4384_LFE_ID) &&
	    (id != SND_UBI32_CS4384_CENTER_ID)) {
		uinfo->count = 2;
	}
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 255;
	return 0;
}

/*
 * snd_ubi32_cs4384_get_volume
 */
static int snd_ubi32_cs4384_get_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *priv = snd_kcontrol_chip(kcontrol);
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	unsigned int id = (unsigned int)kcontrol->private_value;
	int ch = snd_ubi32_cs4384_ch_ofs[id];
	unsigned long flags;

	if (id >= SND_UBI32_CS4384_LAST_ID) {
		return -EINVAL;
	}

	cs4384_priv = snd_ubi32_priv_get_drv(priv);

	spin_lock_irqsave(&cs4384_priv->lock, flags);

	ucontrol->value.integer.value[0] = cs4384_priv->volume[ch];
	if ((id != SND_UBI32_CS4384_LFE_ID) &&
	    (id != SND_UBI32_CS4384_CENTER_ID)) {
		ch++;
		ucontrol->value.integer.value[1] = cs4384_priv->volume[ch];
	}

	spin_unlock_irqrestore(&cs4384_priv->lock, flags);

	return 0;
}

/*
 * snd_ubi32_cs4384_put_volume
 */
static int snd_ubi32_cs4384_put_volume(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)priv->client;
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	unsigned int id = (unsigned int)kcontrol->private_value;
	int ch = snd_ubi32_cs4384_ch_ofs[id];
	unsigned long flags;
	unsigned char send[3];
	int nch;
	int ret = -EINVAL;

	if (id >= SND_UBI32_CS4384_LAST_ID) {
		return -EINVAL;
	}

	cs4384_priv = snd_ubi32_priv_get_drv(priv);

	spin_lock_irqsave(&cs4384_priv->lock, flags);

	send[0] = 0;
	switch (id) {
	case SND_UBI32_CS4384_REAR_ID:
		send[0] = 0x06;

		/*
		 * Fall through
		 */

	case SND_UBI32_CS4384_SURROUND_ID:
		send[0] += 0x03;

		/*
		 * Fall through
		 */

	case SND_UBI32_CS4384_FRONT_ID:
		send[0] += 0x8B;
		nch = 2;
		send[1] = 255 - (ucontrol->value.integer.value[0] & 0xFF);
		send[2] = 255 - (ucontrol->value.integer.value[1] & 0xFF);
		cs4384_priv->volume[ch++] = send[1];
		cs4384_priv->volume[ch] = send[2];
		break;

	case SND_UBI32_CS4384_LFE_ID:
		send[0] = 0x81;

		/*
		 * Fall through
		 */

	case SND_UBI32_CS4384_CENTER_ID:
		send[0] += 0x11;
		nch = 1;
		send[1] = 255 - (ucontrol->value.integer.value[0] & 0xFF);
		cs4384_priv->volume[ch] = send[1];
		break;

	default:
		spin_unlock_irqrestore(&cs4384_priv->lock, flags);
		goto done;

	}

	/*
	 * Send the volume to the chip
	 */
	nch++;
	ret = i2c_master_send(client, send, nch);
	if (ret != nch) {
		snd_printk(KERN_ERR "Failed to set volume on CS4384\n");
	}

done:
	spin_unlock_irqrestore(&cs4384_priv->lock, flags);

	return ret;
}

/*
 * snd_ubi32_cs4384_get_mute
 */
static int snd_ubi32_cs4384_get_mute(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *priv = snd_kcontrol_chip(kcontrol);
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	unsigned int id = (unsigned int)kcontrol->private_value;
	int ch = snd_ubi32_cs4384_ch_ofs[id];
	unsigned long flags;

	if (id >= SND_UBI32_CS4384_LAST_ID) {
		return -EINVAL;
	}

	cs4384_priv = snd_ubi32_priv_get_drv(priv);

	spin_lock_irqsave(&cs4384_priv->lock, flags);

	ucontrol->value.integer.value[0] = !(cs4384_priv->mute & (1 << ch));

	if ((id != SND_UBI32_CS4384_LFE_ID) &&
	    (id != SND_UBI32_CS4384_CENTER_ID)) {
		ch++;
		ucontrol->value.integer.value[1] = !(cs4384_priv->mute & (1 << ch));
	}

	spin_unlock_irqrestore(&cs4384_priv->lock, flags);

	return 0;
}

/*
 * snd_ubi32_cs4384_put_mute
 */
static int snd_ubi32_cs4384_put_mute(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct ubi32_snd_priv *priv = snd_kcontrol_chip(kcontrol);
	struct i2c_client *client = (struct i2c_client *)priv->client;
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	unsigned int id = (unsigned int)kcontrol->private_value;
	int ch = snd_ubi32_cs4384_ch_ofs[id];
	unsigned long flags;
	unsigned char send[2];
	int ret = -EINVAL;

	if (id >= SND_UBI32_CS4384_LAST_ID) {
		return -EINVAL;
	}

	cs4384_priv = snd_ubi32_priv_get_drv(priv);

	spin_lock_irqsave(&cs4384_priv->lock, flags);

	if (ucontrol->value.integer.value[0]) {
		cs4384_priv->mute &= ~(1 << ch);
	} else {
		cs4384_priv->mute |= (1 << ch);
	}

	if ((id != SND_UBI32_CS4384_LFE_ID) && (id != SND_UBI32_CS4384_CENTER_ID)) {
		ch++;
		if (ucontrol->value.integer.value[1]) {
			cs4384_priv->mute &= ~(1 << ch);
		} else {
			cs4384_priv->mute |= (1 << ch);
		}
	}

	/*
	 * Update the chip's mute reigster
	 */
	send[0] = 0x09;
	send[1] = cs4384_priv->mute;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set mute on CS4384\n");
	}

	spin_unlock_irqrestore(&cs4384_priv->lock, flags);

	return ret;
}

/*
 * snd_ubi32_cs4384_mixer
 *	Setup the mixer controls
 */
static int __devinit snd_ubi32_cs4384_mixer(struct ubi32_snd_priv *priv)
{
	struct snd_card *card = priv->card;
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	int i;

	cs4384_priv = snd_ubi32_priv_get_drv(priv);
	for (i = 0; i < ARRAY_SIZE(snd_ubi32_cs4384_controls); i++) {
		int err;

		cs4384_priv->kctls[i] = snd_ctl_new1(&snd_ubi32_cs4384_controls[i], priv);
		err = snd_ctl_add(card, cs4384_priv->kctls[i]);
		if (err) {
			snd_printk(KERN_WARNING "Failed to add control %d\n", i);
			return err;
		}
	}
	return 0;
}

/*
 * snd_ubi32_cs4384_free
 *	Card private data free function
 */
void snd_ubi32_cs4384_free(struct snd_card *card)
{
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	struct ubi32_snd_priv *ubi32_priv;

	ubi32_priv = card->private_data;
	cs4384_priv = snd_ubi32_priv_get_drv(ubi32_priv);
	if (cs4384_priv) {
		kfree(cs4384_priv);
	}
}

/*
 * snd_ubi32_cs4384_setup_mclk
 */
static int snd_ubi32_cs4384_setup_mclk(struct ubi32_cs4384_platform_data *pdata)
{
	struct ubicom32_io_port *ioa = (struct ubicom32_io_port *)RA;
	struct ubicom32_io_port *ioc = (struct ubicom32_io_port *)RC;
	struct ubicom32_io_port *iod = (struct ubicom32_io_port *)RD;
	struct ubicom32_io_port *ioe = (struct ubicom32_io_port *)RE;
	struct ubicom32_io_port *ioh = (struct ubicom32_io_port *)RH;
	unsigned int ctl0;
	unsigned int ctlx;
	unsigned int div;

	div = pdata->mclk_entries[0].div;

	ctl0 = (1 << 13);
	ctlx = ((div - 1) << 16) | (div / 2);

	switch (pdata->mclk_src) {
	case UBI32_CS4384_MCLK_PWM_0:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0;
		ioc->ctl1 = ctlx;
		if (!ioa->function) {
			ioa->function = 3;
		}
		return 0;

	case UBI32_CS4384_MCLK_PWM_1:
		ioc->function |= 2;
		ioc->ctl0 |= ctl0 << 16;
		ioc->ctl2 = ctlx;
		if (!ioe->function) {
			ioe->function = 3;
		}
		return 0;

	case UBI32_CS4384_MCLK_PWM_2:
		ioh->ctl0 |= ctl0;
		ioh->ctl1 = ctlx;
		if (!iod->function) {
			iod->function = 3;
		}
		return 0;

	case UBI32_CS4384_MCLK_CLKDIV_1:
		ioa->gpio_mask &= (1 << 7);
		ioa->ctl1 &= ~(0x7F << 14);
		ioa->ctl1 |= ((div - 1) << 14);
		return 0;

	case UBI32_CS4384_MCLK_OTHER:
		return 0;
	}

	return 1;
}

/*
 * snd_ubi32_cs4384_set_rate
 */
static int snd_ubi32_cs4384_set_rate(struct ubi32_snd_priv *priv, int rate)
{
	struct ubi32_cs4384_platform_data *cpd = priv->pdata->priv_data;
	struct ubicom32_io_port *ioa = (struct ubicom32_io_port *)RA;
	struct ubicom32_io_port *ioc = (struct ubicom32_io_port *)RC;
	struct ubicom32_io_port *ioh = (struct ubicom32_io_port *)RH;
	unsigned int ctl;
	unsigned int div = 0;
	const u16_t mult[] = {64, 96, 128, 192, 256, 384, 512, 768, 1024};
	int i;
	int j;


	for (i = 0; i < sizeof(mult) / sizeof(u16_t); i++) {
		for (j = 0; j < cpd->n_mclk; j++) {
			if (((unsigned int)rate * (unsigned int)mult[i]) ==
			     cpd->mclk_entries[j].rate) {
				div = cpd->mclk_entries[j].div;
				break;
			}
		}
	}

	ctl = ((div - 1) << 16) | (div / 2);

	switch (cpd->mclk_src) {
	case UBI32_CS4384_MCLK_PWM_0:
		ioc->ctl1 = ctl;
		return 0;

	case UBI32_CS4384_MCLK_PWM_1:
		ioc->ctl2 = ctl;
		return 0;

	case UBI32_CS4384_MCLK_PWM_2:
		ioh->ctl1 = ctl;
		return 0;

	case UBI32_CS4384_MCLK_CLKDIV_1:
		ioa->ctl1 &= ~(0x7F << 14);
		ioa->ctl1 |= ((div - 1) << 14);
		return 0;

	case UBI32_CS4384_MCLK_OTHER:
		return 0;
	}

	return 1;
}

/*
 * snd_ubi32_cs4384_set_channels
 *	Mute unused channels
 */
static int snd_ubi32_cs4384_set_channels(struct ubi32_snd_priv *priv, int channels)
{
	struct i2c_client *client = (struct i2c_client *)priv->client;
	struct snd_ubi32_cs4384_priv *cs4384_priv;
	unsigned char send[2];
	int ret;
	int i;
	unsigned long flags;

	/*
	 * Only support 0, 2, 4, 6, 8 channels
	 */
	if ((channels > 8) || (channels & 1)) {
		return -EINVAL;
	}

	cs4384_priv = snd_ubi32_priv_get_drv(priv);
	spin_lock_irqsave(&cs4384_priv->lock, flags);

	/*
	 * Address 09h, Mute control
	 */
	send[0] = 0x09;
	send[1] = (unsigned char)(0xFF << channels);

	ret = i2c_master_send(client, send, 2);

	spin_unlock_irqrestore(&cs4384_priv->lock, flags);

	/*
	 * Notify the system that we changed the mutes
	 */
	cs4384_priv->mute = (unsigned char)(0xFF << channels);

	for (i = SND_UBI32_MUTE_CTL_START; i < SND_UBI32_MUTE_CTL_END; i++) {
		snd_ctl_notify(priv->card, SNDRV_CTL_EVENT_MASK_VALUE,
			       &cs4384_priv->kctls[i]->id);
	}

	if (ret != 2) {
		return -ENXIO;
	}

	return 0;
}

/*
 * snd_ubi32_cs4384_dac_init
 */
static int snd_ubi32_cs4384_dac_init(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	unsigned char send[2];
	unsigned char recv[2];

	/*
	 * Initialize the CS4384 DAC over the I2C interface
	 */
	snd_printk(KERN_INFO "Initializing CS4384 DAC\n");

	/*
	 * Register 0x01: device/revid
	 */
	send[0] = 0x01;
	ret = i2c_master_send(client, send, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed 1st attempt to write to CS4384 register 0x01\n");
		goto fail;
	}
	ret = i2c_master_recv(client, recv, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed initial read of CS4384 registers\n");
		goto fail;
	}
	snd_printk(KERN_INFO "CS4384 DAC Device/Rev: %08x\n", recv[0]);

	/*
	 * Register 0x02: Mode Control 1
	 *	Control Port Enable, PCM, All DACs enabled, Power Down
	 */
	send[0] = 0x02;
	send[1] = 0x81;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set CPEN CS4384\n");
		goto fail;
	}

	/*
	 * Register 0x08: Ramp and Mute
	 *	RMP_UP, RMP_DN, PAMUTE, DAMUTE
	 */
	send[0] = 0x08;
	send[1] = 0xBC;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set CPEN CS4384\n");
		goto fail;
	}

	/*
	 * Register 0x03: PCM Control
	 *	I2S DIF[3:0] = 0001, no De-Emphasis, Auto speed mode
	 */
	send[0] = 0x03;
	send[1] = 0x13;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to set CS4384 to I2S mode\n");
		goto fail;
	}

	/*
	 * Register 0x0B/0x0C: Volume control A1/B1
	 * Register 0x0E/0x0F: Volume control A2/B2
	 * Register 0x11/0x12: Volume control A3/B3
	 * Register 0x14/0x15: Volume control A4/B4
	 */
	send[0] = 0x80 | 0x0B;
	send[1] = 0x00;
	send[2] = 0x00;
	ret = i2c_master_send(client, send, 3);
	if (ret != 3) {
		snd_printk(KERN_ERR "Failed to set ch1 volume on CS4384\n");
		goto fail;
	}

	send[0] = 0x80 | 0x0E;
	send[1] = 0x00;
	send[2] = 0x00;
	ret = i2c_master_send(client, send, 3);
	if (ret != 3) {
		snd_printk(KERN_ERR "Failed to set ch2 volume on CS4384\n");
		goto fail;
	}

	send[0] = 0x80 | 0x11;
	send[1] = 0x00;
	send[2] = 0x00;
	ret = i2c_master_send(client, send, 3);
	if (ret != 3) {
		snd_printk(KERN_ERR "Failed to set ch3 volume on CS4384\n");
		goto fail;
	}

	send[0] = 0x80 | 0x14;
	send[1] = 0x00;
	send[2] = 0x00;
	ret = i2c_master_send(client, send, 3);
	if (ret != 3) {
		snd_printk(KERN_ERR "Failed to set ch4 volume on CS4384\n");
		goto fail;
	}

	/*
	 * Register 09h: Mute control
	 *	Mute all (we will unmute channels as needed)
	 */
	send[0] = 0x09;
	send[1] = 0xFF;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to power up CS4384\n");
		goto fail;
	}

	/*
	 * Register 0x02: Mode Control 1
	 *	Control Port Enable, PCM, All DACs enabled, Power Up
	 */
	send[0] = 0x02;
	send[1] = 0x80;
	ret = i2c_master_send(client, send, 2);
	if (ret != 2) {
		snd_printk(KERN_ERR "Failed to power up CS4384\n");
		goto fail;
	}

	/*
	 * Make sure the changes took place, this helps verify we are talking to
	 * the correct chip.
	 */
	send[0] = 0x80 | 0x03;
	ret = i2c_master_send(client, send, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed to initiate readback\n");
		goto fail;
	}

	ret = i2c_master_recv(client, recv, 1);
	if (ret != 1) {
		snd_printk(KERN_ERR "Failed second read of CS4384 registers\n");
		goto fail;
	}

	if (recv[0] != 0x13) {
		snd_printk(KERN_ERR "Failed to initialize CS4384 DAC\n");
		goto fail;
	}

	snd_printk(KERN_INFO "CS4384 DAC Initialized\n");
	return 0;

fail:
	return -ENODEV;
}

/*
 * snd_ubi32_cs4384_i2c_probe
 */
static int snd_ubi32_cs4384_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct snd_card *card;
	struct ubi32_snd_priv *ubi32_priv;
	int err, ret;
	struct platform_device *pdev;
	struct ubi32_cs4384_platform_data *pdata;
	struct snd_ubi32_cs4384_priv *cs4384_priv;

	/*
	 * pdev is audio device
	 */
	pdev = client->dev.platform_data;
	if (!pdev) {
		return -ENODEV;
	}

	/*
	 * pdev->dev.platform_data is ubi32-pcm platform_data
	 */
	pdata = audio_device_priv(pdev);
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Initialize the CS4384 DAC
	 */
	ret = snd_ubi32_cs4384_dac_init(client, id);
	if (ret < 0) {
		/*
		 * Initialization failed.  Propagate the error.
		 */
		return ret;
	}

	if (snd_ubi32_cs4384_setup_mclk(pdata)) {
		return -EINVAL;
	}

	/*
	 * Create a snd_card structure
	 */
	card = snd_card_new(index, "Ubi32-CS4384", THIS_MODULE, sizeof(struct ubi32_snd_priv));
	if (card == NULL) {
		return -ENOMEM;
	}

	card->private_free = snd_ubi32_cs4384_free;
	ubi32_priv = card->private_data;

	/*
	 * Initialize the snd_card's private data structure
	 */
	ubi32_priv->card = card;
	ubi32_priv->client = client;
	ubi32_priv->set_channels = snd_ubi32_cs4384_set_channels;
	ubi32_priv->set_rate = snd_ubi32_cs4384_set_rate;

	/*
	 * CS4384 DAC has a minimum sample rate of 4khz and an
	 * upper limit of 216khz for it's auto-detect.
	 */
	ubi32_priv->min_sample_rate = 4000;
	ubi32_priv->max_sample_rate = 216000;

	/*
	 * Create our private data (to manage volume, etc)
	 */
	cs4384_priv = kzalloc(sizeof(struct snd_ubi32_cs4384_priv), GFP_KERNEL);
	if (!cs4384_priv) {
		snd_card_free(card);
		return -ENOMEM;
	}
	snd_ubi32_priv_set_drv(ubi32_priv, cs4384_priv);
	spin_lock_init(&cs4384_priv->lock);

	/*
	 * We start off all muted and max volume
	 */
	cs4384_priv->mute = 0xFF;
	memset(cs4384_priv->volume, 0xFF, 8);

	/*
	 * Create the new PCM instance
	 */
	err = snd_ubi32_pcm_probe(ubi32_priv, pdev);
	if (err < 0) {
		snd_card_free(card);
		return err; /* What is err?  Need to include correct file */
	}

	strcpy(card->driver, "Ubi32-CS4384");
	strcpy(card->shortname, "Ubi32-CS4384");
	snprintf(card->longname, sizeof(card->longname),
		"%s at sendirq=%d.%d recvirq=%d.%d regs=%p",
		card->shortname, ubi32_priv->tx_irq, ubi32_priv->irq_idx,
		ubi32_priv->rx_irq, ubi32_priv->irq_idx, ubi32_priv->adr);

	snd_card_set_dev(card, &client->dev);

	/*
	 * Set up the mixer
	 */
	snd_ubi32_cs4384_mixer(ubi32_priv);

	/*
	 * Register the sound card
	 */
	if ((err = snd_card_register(card)) != 0) {
		snd_printk(KERN_INFO "snd_card_register error\n");
	}

	/*
	 * Store card for access from other methods
	 */
	i2c_set_clientdata(client, card);

	return 0;
}

/*
 * snd_ubi32_cs4384_i2c_remove
 */
static int __devexit snd_ubi32_cs4384_i2c_remove(struct i2c_client *client)
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
static struct i2c_driver snd_ubi32_cs4384_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.id_table	= snd_ubi32_cs4384_id,
	.probe		= snd_ubi32_cs4384_i2c_probe,
	.remove		= __devexit_p(snd_ubi32_cs4384_i2c_remove),
};

/*
 * Driver init
 */
static int __init snd_ubi32_cs4384_init(void)
{
	return i2c_add_driver(&snd_ubi32_cs4384_driver);
}
module_init(snd_ubi32_cs4384_init);

/*
 * snd_ubi32_cs4384_exit
 */
static void __exit snd_ubi32_cs4384_exit(void)
{
	i2c_del_driver(&snd_ubi32_cs4384_driver);
}
module_exit(snd_ubi32_cs4384_exit);

/*
 * Module properties
 */
MODULE_ALIAS("i2c:" DRIVER_NAME);
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Driver for Ubicom32 audio devices CS4384");
MODULE_LICENSE("GPL");
