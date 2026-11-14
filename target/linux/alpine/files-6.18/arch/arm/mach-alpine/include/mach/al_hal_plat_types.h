/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2015 Annapurna Labs Ltd. */

/**
 * @defgroup group_services Platform Services API
 *  Platform Services API
 *  @{
  * @file   al_hal_plat_types.h
 *
 * @brief  platform dependent data types
 *
 *
 */

#ifndef __PLAT_TYPES_H__
#define __PLAT_TYPES_H__

#include <linux/types.h>

/* Basic data types */
typedef int al_bool;		/*! boolean */
#define AL_TRUE			1
#define AL_FALSE		0

/*! in LPAE mode, the address address is 40 bit, we extend it to 64 bit */
typedef dma_addr_t al_phys_addr_t;

/** @} end of Platform Services API group */

#endif				/* __PLAT_TYPES_H__ */
