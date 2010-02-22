/*
 * sound/ubicom32/ubi32-pcm.c
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

#include <linux/interrupt.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <asm/ip5000.h>
#include <asm/ubi32-pcm.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include "ubi32.h"

struct ubi32_snd_runtime_data {
	dma_addr_t dma_buffer;		/* Physical address of DMA buffer */
	dma_addr_t dma_buffer_end;	/* First address beyond end of DMA buffer */
	size_t period_size;
	dma_addr_t period_ptr;		/* Physical address of next period */
	unsigned int flags;
};

static void snd_ubi32_vp_int_set(struct snd_pcm *pcm)
{
	struct ubi32_snd_priv *ubi32_priv = pcm->private_data;
	ubi32_priv->ar->int_req |= (1 << ubi32_priv->irq_idx);
	ubicom32_set_interrupt(ubi32_priv->tx_irq);
}

static snd_pcm_uframes_t snd_ubi32_pcm_pointer(struct snd_pcm_substream *substream)
{

	struct ubi32_snd_priv *ubi32_priv = snd_pcm_substream_chip(substream);
	struct audio_dev_regs *adr = ubi32_priv->adr;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct ubi32_snd_runtime_data *ubi32_rd = substream->runtime->private_data;

	dma_addr_t read_pos;

	snd_pcm_uframes_t frames;
	if (!adr->primary_os_buffer_ptr) {
		/*
		 * If primary_os_buffer_ptr is NULL (e.g. right after the HW is started or
		 * when the HW is stopped), then handle this case separately.
		 */
		return 0;
	}

	read_pos = (dma_addr_t)adr->primary_os_buffer_ptr;
	frames = bytes_to_frames(runtime, read_pos - ubi32_rd->dma_buffer);
	if (frames == runtime->buffer_size) {
		frames = 0;
	}
	return frames;
}

/*
 * Audio trigger
 */
static int snd_ubi32_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct ubi32_snd_priv *ubi32_priv = substream->pcm->private_data;
	struct audio_dev_regs *adr = ubi32_priv->adr;
	struct ubi32_snd_runtime_data *ubi32_rd = substream->runtime->private_data;
	int ret = 0;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_trigger cmd=%d=", cmd);
#endif

	if (adr->command != AUDIO_CMD_NONE) {
		snd_printk(KERN_WARNING "Can't send command to audio device at this time\n");
		// Set a timer to call this function back later.  How to do this?
		return 0;
	}

	/*
	 * Set interrupt flag to indicate that we interrupted audio device
	 * to send a command
	 */

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "START\n");
#endif
		/*
		 * Ready the DMA transfer
		 */
		ubi32_rd->period_ptr = ubi32_rd->dma_buffer;

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "trigger period_ptr=%lx\n", (unsigned long)ubi32_rd->period_ptr);
#endif
		adr->dma_xfer_requests[0].ptr = (void *)ubi32_rd->period_ptr;
		adr->dma_xfer_requests[0].ctr = ubi32_rd->period_size;
		adr->dma_xfer_requests[0].active = 1;

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "xfer_request 0 ptr=0x%x ctr=%u\n", ubi32_rd->period_ptr, ubi32_rd->period_size);
#endif

		ubi32_rd->period_ptr += ubi32_rd->period_size;
		adr->dma_xfer_requests[1].ptr = (void *)ubi32_rd->period_ptr;
		adr->dma_xfer_requests[1].ctr = ubi32_rd->period_size;
		adr->dma_xfer_requests[1].active = 1;

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "xfer_request 1 ptr=0x%x ctr=%u\n", ubi32_rd->period_ptr, ubi32_rd->period_size);
#endif

		/*
		 * Tell the VP that we want to begin playback by filling in the
		 * command field and then interrupting the audio VP
		 */
		adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
		adr->command = AUDIO_CMD_START;
		snd_ubi32_vp_int_set(substream->pcm);
		break;

	case SNDRV_PCM_TRIGGER_STOP:

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "STOP\n");
#endif

		/*
		 * Tell the VP that we want to stop playback by filling in the
		 * command field and then interrupting the audio VP
		 */
		adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
		adr->command = AUDIO_CMD_STOP;
		snd_ubi32_vp_int_set(substream->pcm);
		break;

	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "PAUSE_PUSH\n");
#endif

		/*
		 * Tell the VP that we want to pause playback by filling in the
		 * command field and then interrupting the audio VP
		 */
		adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
		adr->command = AUDIO_CMD_PAUSE;
		snd_ubi32_vp_int_set(substream->pcm);
		break;

	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:

#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "PAUSE_RELEASE\n");
#endif
		/*
		 * Tell the VP that we want to resume paused playback by filling
		 * in the command field and then interrupting the audio VP
		 */
		adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
		adr->command = AUDIO_CMD_RESUME;
		snd_ubi32_vp_int_set(substream->pcm);
		break;

	default:
		snd_printk(KERN_WARNING "Unhandled trigger\n");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/*
 * Prepare to transfer an audio stream to the codec
 */
static int snd_ubi32_pcm_prepare(struct snd_pcm_substream *substream)
{
	/*
	 * Configure registers and setup the runtime instance for DMA transfers
	 */
	struct ubi32_snd_priv *ubi32_priv = substream->pcm->private_data;
	struct audio_dev_regs *adr = ubi32_priv->adr;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_prepare: sending STOP command to audio device\n");
#endif

	/*
	 * Make sure the audio device is stopped
	 */

	/*
	 * Set interrupt flag to indicate that we interrupted audio device
	 * to send a command
	 */
	adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
	adr->command = AUDIO_CMD_STOP;
	snd_ubi32_vp_int_set(substream->pcm);

	return 0;
}

/*
 * Allocate DMA buffers from preallocated memory.
 * Preallocation was done in snd_ubi32_pcm_new()
 */
static int snd_ubi32_pcm_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *hw_params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct ubi32_snd_priv *ubi32_priv = substream->pcm->private_data;
	struct audio_dev_regs *adr = ubi32_priv->adr;
	struct ubi32_snd_runtime_data *ubi32_rd = substream->runtime->private_data;

	/*
	 * Use pre-allocated memory from ubi32_snd_pcm_new() to satisfy
	 * this memory request.
	 */
	int ret = snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
	if (ret < 0) {
		return ret;
	}

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_hw_params\n");
#endif

	if (!(adr->channel_mask & (1 << params_channels(hw_params)))) {
		snd_printk(KERN_INFO "snd_ubi32_pcm_hw_params unsupported number of channels %d mask %08x\n", params_channels(hw_params), adr->channel_mask);
		return -EINVAL;
	}

	if (ubi32_priv->set_channels) {
		int ret = ubi32_priv->set_channels(ubi32_priv, params_channels(hw_params));
		if (ret) {
			snd_printk(KERN_WARNING "Unable to set channels to %d, ret=%d\n", params_channels(hw_params), ret);
			return ret;
		}
	}

	if (ubi32_priv->set_rate) {
		int ret = ubi32_priv->set_rate(ubi32_priv, params_rate(hw_params));
		if (ret) {
			snd_printk(KERN_WARNING "Unable to set rate to %d, ret=%d\n", params_rate(hw_params), ret);
			return ret;
		}
	}

	if (ubi32_priv->pdata->set_rate) {
		int ret = ubi32_priv->pdata->set_rate(ubi32_priv->pdata->appdata, params_rate(hw_params));
		if (ret) {
			snd_printk(KERN_WARNING "Unable to set rate to %d, ret=%d\n", params_rate(hw_params), ret);
			return ret;
		}
	}

	if (adr->command != AUDIO_CMD_NONE) {
		snd_printk(KERN_WARNING "snd_ubi32_pcm_hw_params: tio busy\n");
		return -EAGAIN;
	}

	if (params_format(hw_params) == SNDRV_PCM_FORMAT_S16_LE) {
		adr->flags |= CMD_START_FLAG_LE;
	} else {
		adr->flags &= ~CMD_START_FLAG_LE;
	}
	adr->channels = params_channels(hw_params);
	adr->sample_rate = params_rate(hw_params);
	adr->command = AUDIO_CMD_SETUP;
	adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
	snd_ubi32_vp_int_set(substream->pcm);

	/*
	 * Wait for the command to complete
	 */
	while (adr->command != AUDIO_CMD_NONE) {
		udelay(1);
	}

	/*
	 * Put the DMA info into the DMA descriptor that we will
	 * use to do transfers to our audio VP "hardware"
	 */

	/*
	 * Mark both DMA transfers as not ready/inactive
	 */
	adr->dma_xfer_requests[0].active = 0;
	adr->dma_xfer_requests[1].active = 0;

	/*
	 * Put the location of the buffer into the runtime data instance
	 */
	ubi32_rd->dma_buffer = (dma_addr_t)runtime->dma_area;
	ubi32_rd->dma_buffer_end = (dma_addr_t)(runtime->dma_area + runtime->dma_bytes);

	/*
	 * Get the period size
	 */
	ubi32_rd->period_size = params_period_bytes(hw_params);

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "DMA for ubi32 audio initialized dma_area=0x%x dma_bytes=%d, period_size=%d\n", (unsigned int)runtime->dma_area, (unsigned int)runtime->dma_bytes, ubi32_rd->period_size);
	snd_printk(KERN_INFO "Private buffer ubi32_rd: dma_buffer=0x%x dma_buffer_end=0x%x ret=%d\n", ubi32_rd->dma_buffer, ubi32_rd->dma_buffer_end, ret);
#endif

	return ret;
}

/*
 * This is the reverse of snd_ubi32_pcm_hw_params
 */
static int snd_ubi32_pcm_hw_free(struct snd_pcm_substream *substream)
{
#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_hw_free\n");
#endif
	return snd_pcm_lib_free_pages(substream);
}

/*
 * Audio virtual peripheral capabilities (capture and playback are identical)
 */
static struct snd_pcm_hardware snd_ubi32_pcm_hw =
{
	.info = (SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER |
		 SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME),
	.buffer_bytes_max = (64*1024),
	.period_bytes_min	= 64,
	.period_bytes_max	= 8184,//8184,//8176,
	.periods_min = 2,
	.periods_max = 255,
	.fifo_size = 0, // THIS IS IGNORED BY ALSA
};

/*
 * We fill this in later
 */
static struct snd_pcm_hw_constraint_list ubi32_pcm_rates;

/*
 * snd_ubi32_pcm_close
 */
static int snd_ubi32_pcm_close(struct snd_pcm_substream *substream)
{
	/* Disable codec, stop DMA, free private data structures */
	//struct ubi32_snd_priv *ubi32_priv = snd_pcm_substream_chip(substream);
	struct ubi32_snd_runtime_data *ubi32_rd = substream->runtime->private_data;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_close\n");
#endif

	substream->runtime->private_data = NULL;

	kfree(ubi32_rd);

	return 0;
}

/*
 * snd_ubi32_pcm_open
 */
static int snd_ubi32_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct ubi32_snd_runtime_data *ubi32_rd;
	int ret = 0;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "ubi32 pcm open\n");
#endif

	/* Associate capabilities with component */
	runtime->hw = snd_ubi32_pcm_hw;

	/*
	 * Inform ALSA about constraints of the audio device
	 */
	ret = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &ubi32_pcm_rates);
	if (ret < 0) {
		snd_printk(KERN_INFO "invalid rate\n");
		goto out;
	}

	/* Force the buffer size to be an integer multiple of period size */
	ret = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0) {
		snd_printk(KERN_INFO "invalid period\n");
		goto out;
	}
	/* Initialize structures/registers */
	ubi32_rd = kzalloc(sizeof(struct ubi32_snd_runtime_data), GFP_KERNEL);
	if (ubi32_rd == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	runtime->private_data = ubi32_rd;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_open returned 0\n");
#endif

	return 0;
out:
#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "snd_ubi32_pcm_open returned %d\n", ret);
#endif

	return ret;
}

static struct snd_pcm_ops snd_ubi32_pcm_ops = {
	.open = 	snd_ubi32_pcm_open, /* Open */
	.close = 	snd_ubi32_pcm_close, /* Close */
	.ioctl = 	snd_pcm_lib_ioctl, /* Generic IOCTL handler */
	.hw_params = 	snd_ubi32_pcm_hw_params, /* Hardware parameters/capabilities */
	.hw_free = 	snd_ubi32_pcm_hw_free, /* Free function for hw_params */
	.prepare = 	snd_ubi32_pcm_prepare,
	.trigger = 	snd_ubi32_pcm_trigger,
	.pointer = 	snd_ubi32_pcm_pointer,
};

/*
 * Interrupt handler that gets called when the audio device
 * interrupts Linux
 */
static irqreturn_t snd_ubi32_pcm_interrupt(int irq, void *appdata)
{
	struct snd_pcm *pcm = (struct snd_pcm *)appdata;
	struct ubi32_snd_priv *ubi32_priv = pcm->private_data;
	struct audio_dev_regs *adr = ubi32_priv->adr;
	struct snd_pcm_substream *substream;
	struct ubi32_snd_runtime_data *ubi32_rd;
	int dma_to_fill = 0;

	/*
	 * Check to see if the interrupt is for us
	 */
	if (!(ubi32_priv->ar->int_status & (1 << ubi32_priv->irq_idx))) {
		return IRQ_NONE;
	}

	/*
	 * Clear the interrupt
	 */
	ubi32_priv->ar->int_status &= ~(1 << ubi32_priv->irq_idx);

	/*
	 * We only have one stream since we don't mix.  Therefore
	 * we don't need to search through substreams.
	 */
	if (ubi32_priv->is_capture) {
		substream = pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream;
	} else {
		substream = pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream;
	}

	if (!substream->runtime) {
		snd_printk(KERN_WARNING "No runtime data\n");
		return IRQ_NONE;
	}

	ubi32_rd = substream->runtime->private_data;

#ifdef CONFIG_SND_DEBUG
	snd_printk(KERN_INFO "Ubi32 ALSA interrupt\n");
#endif

	if (ubi32_rd == NULL) {
		snd_printk(KERN_WARNING "No private data\n");
		return IRQ_NONE;
	}

	// Check interrupt cause
	if (0) {
		// Handle the underflow case
	} else if ((adr->status & AUDIO_STATUS_PLAY_DMA0_REQUEST) ||
		   (adr->status & AUDIO_STATUS_PLAY_DMA1_REQUEST)) {
		if (adr->status & AUDIO_STATUS_PLAY_DMA0_REQUEST) {
			dma_to_fill = 0;
			adr->status &= ~AUDIO_STATUS_PLAY_DMA0_REQUEST;
		} else if (adr->status & AUDIO_STATUS_PLAY_DMA1_REQUEST) {
			dma_to_fill = 1;
			adr->status &= ~AUDIO_STATUS_PLAY_DMA1_REQUEST;
		}
		ubi32_rd->period_ptr += ubi32_rd->period_size;
		if (ubi32_rd->period_ptr >= ubi32_rd->dma_buffer_end) {
			ubi32_rd->period_ptr = ubi32_rd->dma_buffer;
		}
		adr->dma_xfer_requests[dma_to_fill].ptr = (void *)ubi32_rd->period_ptr;
		adr->dma_xfer_requests[dma_to_fill].ctr = ubi32_rd->period_size;
		adr->dma_xfer_requests[dma_to_fill].active = 1;
#ifdef CONFIG_SND_DEBUG
		snd_printk(KERN_INFO "xfer_request %d ptr=0x%x ctr=%u\n", dma_to_fill, ubi32_rd->period_ptr, ubi32_rd->period_size);
#endif
		adr->int_flags |= AUDIO_INT_FLAG_MORE_SAMPLES;
		snd_ubi32_vp_int_set(substream->pcm);
	}
	// If we are interrupted by the VP, that means we completed
	// processing one period of audio.  We need to inform the upper
	// layers of ALSA of this.
	snd_pcm_period_elapsed(substream);

	return IRQ_HANDLED;
}

void __devexit snd_ubi32_pcm_remove(struct ubi32_snd_priv *ubi32_priv)
{
	struct snd_pcm *pcm = ubi32_priv->pcm;
	free_irq(ubi32_priv->rx_irq, pcm);
}

#if SNDRV_PCM_RATE_5512 != 1 << 0 || SNDRV_PCM_RATE_192000 != 1 << 12
#error "Change this table to match pcm.h"
#endif
static unsigned int rates[] __initdata = {5512, 8000, 11025, 16000, 22050,
					  32000, 44100, 48000, 64000, 88200,
					  96000, 176400, 192000};

/*
 * snd_ubi32_pcm_probe
 */
int __devinit snd_ubi32_pcm_probe(struct ubi32_snd_priv *ubi32_priv, struct platform_device *pdev)
{
	struct snd_pcm *pcm;
	int ret, err;
	int i;
	int j;
	int nrates;
	unsigned int rate_max = 0;
	unsigned int rate_min = 0xFFFFFFFF;
	unsigned int rate_mask = 0;
	struct audio_dev_regs *adr;
	struct resource *res_adr;
	struct resource *res_irq_tx;
	struct resource *res_irq_rx;
	struct ubi32pcm_platform_data *pdata;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Get our resources, adr is the hardware driver base address
	 * and the tx and rx irqs are used to communicate with the
	 * hardware driver.
	 */
	res_adr = platform_get_resource(pdev, IORESOURCE_MEM, AUDIO_MEM_RESOURCE);
	res_irq_tx = platform_get_resource(pdev, IORESOURCE_IRQ, AUDIO_TX_IRQ_RESOURCE);
	res_irq_rx = platform_get_resource(pdev, IORESOURCE_IRQ, AUDIO_RX_IRQ_RESOURCE);
	if (!res_adr || !res_irq_tx || !res_irq_rx) {
		snd_printk(KERN_WARNING "Could not get resources");
		return -ENODEV;
	}

	ubi32_priv->ar = (struct audio_regs *)res_adr->start;
	ubi32_priv->tx_irq = res_irq_tx->start;
	ubi32_priv->rx_irq = res_irq_rx->start;
	ubi32_priv->irq_idx = pdata->inst_num;
	ubi32_priv->adr = &(ubi32_priv->ar->adr[pdata->inst_num]);

	/*
	 * Check the version
	 */
	adr = ubi32_priv->adr;
	if (adr->version != AUDIO_DEV_REGS_VERSION) {
		snd_printk(KERN_WARNING "This audio_dev_reg is not compatible with this driver\n");
		return -ENODEV;
	}

	/*
	 * Find out the standard rates, also find max and min rates
	 */
	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		int found = 0;
		for (j = 0; j < adr->n_sample_rates; j++) {
			if (rates[i] == adr->sample_rates[j]) {
				/*
				 * Check to see if it is supported by the dac
				 */
				if ((rates[i] >= ubi32_priv->min_sample_rate) &&
				    (!ubi32_priv->max_sample_rate ||
				     (ubi32_priv->max_sample_rate && (rates[i] <= ubi32_priv->max_sample_rate)))) {
					found = 1;
					rate_mask |= (1 << i);
					nrates++;
					if (rates[i] < rate_min) {
						rate_min = rates[i];
					}
					if (rates[i] > rate_max) {
						rate_max = rates[i];
					}
					break;
				}
			}
		}
		if (!found) {
			rate_mask |= SNDRV_PCM_RATE_KNOT;
		}
	}

	snd_ubi32_pcm_hw.rates = rate_mask;
	snd_ubi32_pcm_hw.rate_min = rate_min;
	snd_ubi32_pcm_hw.rate_max = rate_max;
	ubi32_pcm_rates.count = adr->n_sample_rates;
	ubi32_pcm_rates.list = (unsigned int *)adr->sample_rates;
	ubi32_pcm_rates.mask = 0;

	for (i = 0; i < 32; i++) {
		if (adr->channel_mask & (1 << i)) {
			if (!snd_ubi32_pcm_hw.channels_min) {
				snd_ubi32_pcm_hw.channels_min = i;
			}
			snd_ubi32_pcm_hw.channels_max = i;
		}
	}
	snd_printk(KERN_INFO "Ubi32PCM: channels_min:%u channels_max:%u\n",
		   snd_ubi32_pcm_hw.channels_min,
		   snd_ubi32_pcm_hw.channels_max);

	if (adr->caps & AUDIONODE_CAP_BE) {
		snd_ubi32_pcm_hw.formats |= SNDRV_PCM_FMTBIT_S16_BE;
	}
	if (adr->caps & AUDIONODE_CAP_LE) {
		snd_ubi32_pcm_hw.formats |= SNDRV_PCM_FMTBIT_S16_LE;
	}

	snd_printk(KERN_INFO "Ubi32PCM: rates:%08x min:%u max:%u count:%d fmts:%016llx (%s)\n",
		   snd_ubi32_pcm_hw.rates,
		   snd_ubi32_pcm_hw.rate_min,
		   snd_ubi32_pcm_hw.rate_max,
		   ubi32_pcm_rates.count,
		   snd_ubi32_pcm_hw.formats,
		   ubi32_priv->is_capture ? "capture" : "playback");

	if (ubi32_priv->is_capture) {
		ret = snd_pcm_new(ubi32_priv->card, "Ubi32 PCM", 0, 0, 1, &pcm);
	} else {
		ret = snd_pcm_new(ubi32_priv->card, "Ubi32 PCM", 0, 1, 0, &pcm);
	}

	if (ret < 0) {
		return ret;
	}

	pcm->private_data = ubi32_priv;
	ubi32_priv->pcm = pcm;
	ubi32_priv->pdata = pdata;

	pcm->info_flags = 0;

	strcpy(pcm->name, "Ubi32-PCM");

	snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS,
					      snd_dma_continuous_data(GFP_KERNEL),
					      45*1024, 64*1024);

	if (ubi32_priv->is_capture) {
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_ubi32_pcm_ops);
	} else {
		snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_ubi32_pcm_ops);
	}

	/*
	 * Start up the audio device
	 */
	adr->int_flags |= AUDIO_INT_FLAG_COMMAND;
	adr->command = AUDIO_CMD_ENABLE;
	snd_ubi32_vp_int_set(pcm);

	/*
	 * Request IRQ
	 */
	err = request_irq(ubi32_priv->rx_irq, snd_ubi32_pcm_interrupt, IRQF_SHARED | IRQF_DISABLED, pcm->name, pcm);
	if (err) {
		snd_printk(KERN_WARNING "request_irq failed: irq=%d err=%d\n", ubi32_priv->rx_irq, err);
		return -ENODEV;
	}

	return ret;

}
