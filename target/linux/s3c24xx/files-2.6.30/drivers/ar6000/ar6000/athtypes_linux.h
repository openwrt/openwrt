/*
 * $Id: //depot/sw/releases/olca2.0-GPL/host/os/linux/include/athtypes_linux.h#1 $
 *
 * This file contains the definitions of the basic atheros data types.
 * It is used to map the data types in atheros files to a platform specific
 * type.
 *
 * Copyright 2003-2005 Atheros Communications, Inc.,  All Rights Reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

#ifndef _ATHTYPES_LINUX_H_
#define _ATHTYPES_LINUX_H_

#ifdef __KERNEL__
#include <linux/types.h>
#endif

typedef int8_t      A_INT8;
typedef int16_t     A_INT16;
typedef int32_t     A_INT32;
typedef int64_t     A_INT64;

typedef u_int8_t     A_UINT8;
typedef u_int16_t    A_UINT16;
typedef u_int32_t    A_UINT32;
typedef u_int64_t    A_UINT64;

typedef int             A_BOOL;
typedef char            A_CHAR;
typedef unsigned char   A_UCHAR;
typedef unsigned long   A_ATH_TIMER;


#endif /* _ATHTYPES_LINUX_H_ */
