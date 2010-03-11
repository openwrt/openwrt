/*
 * metronomefb.h - definitions for the metronome framebuffer driver
 *
 * Copyright (C) 2008 by Jaya Kumar
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file COPYING in the main directory of this archive for
 * more details.
 *
 */

#ifndef _LINUX_METRONOMEFB_H_
#define _LINUX_METRONOMEFB_H_

/* command structure used by metronome controller */
struct metromem_cmd {
	u16 opcode;
	u16 args[((64-2)/2)];
	u16 csum;
} __attribute__((packed));

struct epd_frame;

/* struct used by metronome. board specific stuff comes from *board */
struct metronomefb_par {
	struct metromem_cmd *metromem_cmd;
	unsigned char *metromem_wfm;
	unsigned char *metromem_img;
	u16 *metromem_img_csum;
	u16 *csum_table;
	dma_addr_t metromem_dma;
	const struct firmware *firmware;
	struct fb_info *info;
	struct metronome_board *board;
	struct platform_device *pdev;
	wait_queue_head_t waitq;
	u8 frame_count;
	int extra_size;
	int current_wf_mode;
	int current_wf_temp;
	unsigned int manual_refresh_threshold;
	unsigned int partial_autorefresh_interval;
	const struct epd_frame *epd_frame;
	u32 *fxbuckets;
	u32 *fybuckets;

	unsigned int partial_updates_count;
	unsigned is_first_update:1;

	struct mutex lock;
};

#define METRONOME_POWER_OFF	0
#define METRONOME_POWER_ON	1

/* board specific routines and data */
struct metronome_board {
	struct module *owner; /* the platform device */
	void (*power_ctl)(struct metronomefb_par *, int);
	void (*set_rst)(struct metronomefb_par *, int);
	void (*set_stdby)(struct metronomefb_par *, int);
	int (*get_err)(struct metronomefb_par *);
	int (*get_rdy)(struct metronomefb_par *);
	void (*cleanup)(struct metronomefb_par *);
	int (*met_wait_event)(struct metronomefb_par *);
	int (*met_wait_event_intr)(struct metronomefb_par *);
	int (*setup_irq)(struct fb_info *);
	int (*setup_fb)(struct metronomefb_par *);
	int (*setup_io)(struct metronomefb_par *);
	int (*get_panel_type)(void);
};

#endif
