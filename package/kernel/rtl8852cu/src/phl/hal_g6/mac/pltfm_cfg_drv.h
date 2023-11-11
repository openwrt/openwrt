/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_PLTFM_H_
#define _MAC_AX_PLTFM_H_

/* Include header file which contain the following definitions, */
/* or modify this file to meet your platform */
/*[Driver] use their own header files*/
#include "../hal_headers_le.h"

/*[Driver] provide the define of NULL, u8, u16, u32*/
#ifndef NULL
#define NULL		((void *)0)
#endif

/*[Driver] provide the type mutex*/
/* Mutex type */
#define mac_ax_mutex _os_mutex

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))
#endif

#define _ASSERT_ BUG_ON

typedef u16 __le16;
typedef u32 __le32;
typedef u16 __be16;
typedef u32 __be32;

/* 1: enable MAC debug messages */
/* 0: disable MAC debug messages */
#define MAC_AX_DBG_MSG_EN	1

/* Set debug message level */
#define MAC_AX_MSG_LEVEL_TRACE		3
#define MAC_AX_MSG_LEVEL_WARNING	2
#define MAC_AX_MSG_LEVEL_ERR		1
#define MAC_AX_MSG_LEVEL_ALWAYS		0
#define MAC_AX_MSG_LEVEL		MAC_AX_MSG_LEVEL_TRACE

#define SET_CLR_WORD(_w, _v, _f)                                               \
				(((_w) & ~((_f##_MSK) << (_f##_SH))) |         \
				(((_v) & (_f##_MSK)) << (_f##_SH)))
#define SET_WORD(_v, _f)	(((_v) & (_f##_MSK)) << (_f##_SH))
#define GET_FIELD(_w, _f)         (((_w) >> (_f##_SH)) & (_f##_MSK))

#define SET_CLR_WOR2(_w, _v, _sh, _msk) (((_w) & ~(_msk << _sh)) |             \
					(((_v) & _msk) << _sh))
#define SET_WOR2(_v, _sh, _msk)         (((_v) & _msk) << _sh)
#define GET_FIEL2(_w, _sh, _msk)        (((_w) >> _sh) & _msk)

#define GET_MSK(_f)         ((_f##_MSK) << (_f##_SH))

#endif
