/*
 * sound/ubicom32/ubi32.h
 *	Common header file for all ubi32- sound drivers
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

#ifndef _UBI32_H
#define _UBI32_H

#define SND_UBI32_DEBUG 0 // Debug flag

#include <linux/platform_device.h>
#include <asm/devtree.h>
#include <asm/audio.h>
#include <asm/ubi32-pcm.h>

struct ubi32_snd_priv;

typedef int (*set_channels_t)(struct ubi32_snd_priv *priv, int channels);
typedef int (*set_rate_t)(struct ubi32_snd_priv *priv, int rate);

struct ubi32_snd_priv {
	/*
	 * Any variables that are needed locally here but NOT in
	 * the VP itself should go in here.
	 */
	struct snd_card *card;
	struct snd_pcm *pcm;

	/*
	 * capture (1) or playback (0)
	 */
	int is_capture;
	/*
	 * DAC parameters.  These are the parameters for the specific
	 * DAC we are driving.  The I2S component can run at a range
	 * of frequencies, but the DAC may be limited.  We may want
	 * to make this an array of some sort in the future?
	 *
	 * min/max_sample_rate if set to 0 are ignored.
	 */
	int max_sample_rate;
	int min_sample_rate;

	/*
	 * The size a period (group) of audio samples.  The VP does
	 * not need to know this; each DMA transfer is made to be
	 * one period.
	 */
	u32_t period_size;

	spinlock_t ubi32_lock;

	struct audio_regs *ar;
	struct audio_dev_regs *adr;
	u32 irq_idx;
	u8 tx_irq;
	u8 rx_irq;

	void *client;

	/*
	 * Operations which the base DAC driver can implement
	 */
	set_channels_t set_channels;
	set_rate_t set_rate;

	/*
	 * platform data
	 */
	struct ubi32pcm_platform_data *pdata;

	/*
	 * Private driver data (used for DAC driver control, etc)
	 */
	void *drvdata;
};

#define snd_ubi32_priv_get_drv(priv) ((priv)->drvdata)
#define snd_ubi32_priv_set_drv(priv, data) (((priv)->drvdata) = (void *)(data))

extern int snd_ubi32_pcm_probe(struct ubi32_snd_priv *ubi32_priv, struct platform_device *pdev);
extern void snd_ubi32_pcm_remove(struct ubi32_snd_priv *ubi32_priv);

#endif
