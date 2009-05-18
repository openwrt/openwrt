/*
 *  linux/drivers/mmc/host/glamo-mmc.h - GLAMO MCI driver
 *
 *  Copyright (C) 2007-2008 Openmoko, Inc, Andy Green <andy@openmoko.com>
 *   based on S3C MMC driver -->
 *  Copyright (C) 2004-2006 Thomas Kleffel, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/regulator/consumer.h>

enum glamo_mci_waitfor {
	COMPLETION_NONE,
	COMPLETION_FINALIZE,
	COMPLETION_CMDSENT,
	COMPLETION_RSPFIN,
	COMPLETION_XFERFINISH,
	COMPLETION_XFERFINISH_RSPFIN,
};

struct glamo_mci_host {
	struct platform_device	*pdev;
	struct glamo_mci_pdata  *pdata;
	struct mmc_host		*mmc;
	struct resource		*mem;
	struct resource		*mem_data;
	struct clk		*clk;
	void __iomem		*base;
	u16 __iomem		*base_data;
	int			irq;
	int			irq_cd;
	int			dma;
	int 			data_max_size;

	int 			suspending;

	int			power_mode_current;
	unsigned int		vdd_current;

	unsigned long		clk_rate;
	unsigned long		clk_div;
	unsigned long		real_rate;
	u8			prescaler;

	int			force_slow_during_powerup;

	unsigned		sdiimsk;
	int			dodma;

	volatile int		dmatogo;

	struct mmc_request	*mrq;
	int			cmd_is_stop;
	struct work_struct	irq_work;

	spinlock_t		complete_lock;
	volatile enum 		glamo_mci_waitfor
				complete_what;

	volatile int		dma_complete;

	volatile u32		pio_sgptr;
	volatile u32		pio_words;
	volatile u32		pio_count;
	volatile u16		*pio_ptr;
#define XFER_NONE 0
#define XFER_READ 1
#define XFER_WRITE 2
	volatile u32		pio_active;

	int			bus_width;

	char 			dbgmsg_cmd[301];
	char 			dbgmsg_dat[301];
	volatile char		*status;

	unsigned int		ccnt, dcnt;
	struct tasklet_struct	pio_tasklet;

	struct regulator 	*regulator;
};
