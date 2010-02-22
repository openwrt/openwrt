/*
 * audionode.h
 *	audionode and DMA descriptors
 *
 * Copyright Å© 2009 Ubicom Inc. <www.ubicom.com>.  All rights reserved.
 *
 * This file contains confidential information of Ubicom, Inc. and your use of
 * this file is subject to the Ubicom Software License Agreement distributed with
 * this file. If you are uncertain whether you are an authorized user or to report
 * any unauthorized use, please contact Ubicom, Inc. at +1-408-789-2200.
 * Unauthorized reproduction or distribution of this file is subject to civil and
 * criminal penalties.
 *
 */
#ifndef _AUDIONODE_H_
#define _AUDIONODE_H_

#define AUDIO_INT_FLAG_MORE_SAMPLES 0x00000001
#define AUDIO_INT_FLAG_COMMAND      0x00000002

/*
 * Commands the Primary OS sends to the audio device
 */
enum audio_command {
	AUDIO_CMD_NONE,
	AUDIO_CMD_START,
	AUDIO_CMD_STOP,
	AUDIO_CMD_PAUSE,
	AUDIO_CMD_RESUME,
	AUDIO_CMD_MUTE,
	AUDIO_CMD_UNMUTE,
	AUDIO_CMD_SETUP,
	AUDIO_CMD_ENABLE,
	AUDIO_CMD_DISABLE,
};

/*
 * Flag bits passed in the registers
 */
#define CMD_START_FLAG_LE		(1 << 0)	/* Use Little Endian Mode */

/*
 * Status bits that audio device can set to indicate reason
 * for interrupting the Primary OS
 */
#define AUDIO_STATUS_PLAY_DMA0_REQUEST (1 << 0) /* Audio device needs samples in DMA0 for playback */
#define AUDIO_STATUS_PLAY_DMA1_REQUEST (1 << 1) /* Audio device needs samples in DMA1 for playback */

struct audio_dma {
	/*
	 * NOTE: The active flag shall only be SET by the producer and CLEARED
	 * by the consumer, NEVER the other way around.  For playback, the
	 * Primary OS sets this flag and ipAudio clears it.
	 *
	 * The producer shall not modify the ptr or ctr fields when the transfer
	 * is marked as active, as these are used by the consumer to do the
	 * transfer.
	 */
	volatile u32_t active; 		/* Nonzero if data in ptr/ctr ready to be transferred */
	volatile void *ptr;		/* Pointer to data to be transferred */
	volatile u32_t ctr; 		/* Counter: number of data units to transfer */
};

#define AUDIONODE_CAP_BE	(1 << 0)
#define AUDIONODE_CAP_LE	(1 << 1)

#define AUDIONODE_VERSION	7
struct audio_node {
	struct devtree_node dn;

	/*
	 * Version of this node
	 */
	u32_t			version;

	/*
	 * Pointer to the registers
	 */
	struct audio_regs	*regs;
};

/*
 * [OCM] Audio registers
 *	Registers exposed as part of our MMIO area
 */
#define AUDIO_REGS_VERSION	7
struct audio_regs {
	/*
	 * Version of this register set
	 */
	u32_t version;

	/*
	 * Interrupt status
	 */
	volatile u32_t int_status;

	/*
	 * Interrupt request
	 */
	volatile u32_t int_req;

	/*
	 * Current IRQ being serviced
	 */
	u32_t cur_irq;

	/*
	 * Maximum number of devices supported
	 */
	u32_t max_devs;

	/*
	 * [DDR] Device registers for each of the devices
	 */
	struct audio_dev_regs *adr;
};

#define AUDIO_DEV_REGS_VERSION 2
struct audio_dev_regs {
	u32_t version;					/* Version of this register set */

	u8_t name[32];					/* Name of this driver */
	u32_t caps;					/* Capabilities of this driver */
	const u32_t *sample_rates;			/* Sample Rates supported by this driver */
	u32_t n_sample_rates;				/* Number of sample rates supported by this driver */
	u32_t channel_mask;				/* A bit set in a particular position means we support this channel configuration */
	volatile u32_t int_flags;			/* Reason for interrupting audio device */
	volatile enum audio_command command;	/* Command from Primary OS */
	volatile u32_t flags;				/* Flag bits for this command */
	volatile u32_t channels;			/* Number of channels */
	volatile u32_t sample_rate;			/* Sample rate */
	volatile u32_t status;				/* Status bits sent from ipAudio to Primary OS */
	void *primary_os_buffer_ptr;			/*
							 * Playback: Pointer to next sample to be removed from
							 *           Primary OS sample buffer
							 * Capture: Pointer to where next sample will be inserted
							 *          into Primary OS sample buffer
							 */

	/*
	 * These are the transfer requests.  They are used in alternating
	 * order so that when ipAudio is processing one request, the
	 * Primary OS can fill in the other one.
	 *
	 * NOTE: The active bit shall always be SET by the producer and
	 * CLEARED by the consumer, NEVER the other way around.
	 */
	struct audio_dma dma_xfer_requests[2];
};

#endif
