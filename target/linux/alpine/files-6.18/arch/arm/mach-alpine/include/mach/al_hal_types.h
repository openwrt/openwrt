/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_common HAL Common Layer
 *  @{
 * @file   al_hal_types.h
 *
 * @brief  macros used by HALs and platform layer
 *
 */

#ifndef __AL_HAL_TYPES_H__
#define __AL_HAL_TYPES_H__

#include <mach/al_hal_plat_types.h>
#include <mach/al_hal_plat_services.h>

/* Common defines */

typedef int AL_RETURN;

  /* packed and alinged types */
#define __packed_a4 __attribute__ ((packed, aligned(4)))
#define __packed_a8 __attribute__ ((packed, aligned(8)))
#define __packed_a16 __attribute__ ((packed, aligned(16)))

#if !defined(__cache_aligned)
#define __cache_aligned __attribute__ ((__aligned__(64)))
#endif

#if !defined(INLINE)
#define INLINE inline
#endif

/** @} end of Common group */
#endif				/* __TYPES_H__ */
