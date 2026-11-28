/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * This file contains the default I/O mappings for Annapurna Labs
 */

#ifndef __AL_HAL_IOMAP_H__
#define __AL_HAL_IOMAP_H__

#define AL_NB_BASE		0xfb000000

/******************/
/* NB Sub Windows */
/******************/

/* NB service registers */
#define AL_NB_SERVICE_BASE	(AL_NB_BASE + 0x00070000)

#endif
