/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _PHL_TYPES_H_
#define _PHL_TYPES_H_

#define _ALIGN(_len, _align) (((_len) + (_align) - 1) & ~(_align - 1))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

enum lock_type {
	_ps,
	_bh,
	_irq
};

#ifndef PHL_PLATFORM_LINUX
	#ifndef inline
	#define inline __inline
	#endif

	#ifndef NULL
	#define NULL	((void *)0)
	#endif

	#ifndef __cplusplus
	typedef unsigned char bool;
	#endif

	#ifndef false
	#define false 0
	#endif

	#ifndef true
	#define true 1
	#endif

	#define RTW_PRINT_SEL(x,...) do {} while (0)

	#ifndef BIT
	#define BIT(x)	(1UL << (x))
	#endif

	#define _FAIL				0
	#define _SUCCESS			1

	#define BUG_ON

	#define DMA_TO_DEVICE 0
	#define DMA_FROM_DEVICE 1
#endif /*#ifndef PHL_PLATFORM_LINUX*/


#ifdef PHL_PLATFORM_WINDOWS

	#define MAC_ALEN 6
	#define _os_timer RT_TIMER
	#define _os_lock RT_SPIN_LOCK
	#define _os_mutex PlatformMutex
	#define _os_sema PlatformSemaphore
	#define _os_event PlatformEvent
	#define _os_list struct list_head

	#define _os_atomic volatile long
	#define _os_dbgdump DbgPrint
	#define _os_dbgdump_c DbgPrint
	#define _os_dbgdump_lmt DbgPrint
	#define _os_dbgdump_c_lmt DbgPrint
	#define _os_assert ASSERT
	#define _os_warn_on

	/*#define _os_completion unsigned long*/
	#define _os_tasklet RT_THREAD
	#define _os_thread RT_THREAD
	#define _os_spinlockfg unsigned int
	#define _os_workitem RT_WORK_ITEM

	#define _os_path_sep "\\"
	#define HAL_FILE_CONFIG_PATH ""
	#define FW_FILE_CONFIG_PATH ""
	#define PLATFOM_IS_LITTLE_ENDIAN 1

#elif defined(PHL_PLATFORM_LINUX)
	typedef struct rtw_timer_list _os_timer;

	/* taskletw is wrapper for callback function prototype is void (*func)(void *) */
	typedef struct rtw_taskletw _taskletw;
	struct rtw_taskletw {
		struct tasklet_struct tasklet;
		void (*func)(void *);
		void *data;
	};

	#define _os_lock _lock
	#define _os_mutex _mutex
	#define _os_sema _sema
	#define _os_event struct completion
	#define _os_list _list
	#define _os_atomic ATOMIC_T
	#define MAC_ALEN ETH_ALEN
	#define _os_dbgdump _dbgdump
	#ifdef _dbgdump_lmt
		#define _os_dbgdump_lmt _dbgdump_lmt
	#else
		#define _os_dbgdump_lmt _dbgdump
	#endif
	#ifdef _dbgdump_c
		#define _os_dbgdump_c _dbgdump_c
		#ifdef _dbgdump_c_lmt
			#define _os_dbgdump_c_lmt _dbgdump_c_lmt
		#else
			#define _os_dbgdump_c_lmt _dbgdump_c
		#endif
	#else
		#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
			#define KERN_CONT
		#endif
		#define _os_dbgdump_c(fmt, arg...)	_dbgdump(KERN_CONT fmt, ##arg)
		#define _os_dbgdump_c_lmt(fmt, arg...)	_dbgdump(KERN_CONT fmt, ##arg)
	#endif
	#define _os_assert(_expr) 0/*rtw_bug_on(_expr)*/
	#define _os_warn_on(_cond) rtw_warn_on(_cond)

	#define _os_tasklet _taskletw
	#define _os_thread struct thread_hdl
	#define _os_workitem _workitem
	#define _os_spinlockfg unsigned long

	#define	_os_path_sep "/"

	#ifndef REALTEK_CONFIG_PATH
	#define REALTEK_CONFIG_PATH	""
	#endif
	#define HAL_FILE_CONFIG_PATH	REALTEK_CONFIG_PATH
	#ifndef CONFIG_FIRMWARE_PATH
	#define CONFIG_FIRMWARE_PATH	""
	#endif
	#define FW_FILE_CONFIG_PATH	CONFIG_FIRMWARE_PATH

	#ifdef CONFIG_LITTLE_ENDIAN
	#define PLATFOM_IS_LITTLE_ENDIAN 1
	#else
	#define PLATFOM_IS_LITTLE_ENDIAN 0
	#endif

#else

	#ifdef _WIN64
	typedef unsigned long long size_t;
	#else
	typedef unsigned long size_t;
	#endif
	#define u8 unsigned char
	#define s8 char
	#define u16 unsigned short
	#define s16 short
	#define u32 unsigned int
	#define s32 int
	#define u64 unsigned long long
	#define s64 long long
	#define MAC_ALEN 6

	#ifndef fallthrough
	#if __GNUC__ >= 5 || defined(__clang__)
	#ifndef __has_attribute
	#define __has_attribute(x) 0
	#endif
	#if __has_attribute(__fallthrough__)
	#define fallthrough __attribute__((__fallthrough__))
	#endif
	#endif
	#ifndef fallthrough
	#define fallthrough do {} while (0) /* fallthrough */
	#endif
	#endif

	/* keep define name then delete if osdep ready */

	#define _os_timer unsigned long
	#define _os_lock unsigned long
	#define _os_mutex unsigned long
	#define _os_sema unsigned long
	#define _os_event unsigned long
	#define _os_list struct list_head
	#define _os_atomic int
	#define _os_dbgdump(_fmt, ...)
	#define _os_dbgdump_c(_fmt, ...)
	#define _os_dbgdump_lmt(...)
	#define _os_dbgdump_c_lmt(...)
	#define _os_assert(_expr)
	#define _os_warn_on(_cond)
	#define _os_spinlockfg unsigned int

	#define _os_tasklet unsigned long
	#define _os_thread unsigned long
	#define _os_workitem unsigned long

	#define	_os_path_sep "/"
	#define HAL_FILE_CONFIG_PATH	""
	#define FW_FILE_CONFIG_PATH	""

	#define PLATFOM_IS_LITTLE_ENDIAN 1
#endif

struct _os_handler {
	union {
		_os_tasklet	tasklet;
		_os_workitem	workitem;
		_os_thread thread;
	} u;
	_os_sema hdlr_sema;
	bool hdlr_created;
};

#ifndef PHL_PLATFORM_LINUX
#define SWAP32(x)                                                              \
	((u32)((((u32)(x) & (u32)0x000000ff) << 24) |                          \
	       (((u32)(x) & (u32)0x0000ff00) << 8) |                           \
	       (((u32)(x) & (u32)0x00ff0000) >> 8) |                           \
	       (((u32)(x) & (u32)0xff000000) >> 24)))

#define SWAP16(x)                                                              \
	((u16)((((u16)(x) & (u16)0x00ff) << 8) |                               \
	       (((u16)(x) & (u16)0xff00) >> 8)))

/* 1: the platform is Little Endian. */
/* 0: the platform is Big Endian. */
#if PLATFOM_IS_LITTLE_ENDIAN
#define cpu_to_le32(x)              ((u32)(x))
#define le32_to_cpu(x)              ((u32)(x))
#define cpu_to_le16(x)              ((u16)(x))
#define le16_to_cpu(x)              ((u16)(x))
#define cpu_to_be32(x)              SWAP32((x))
#define be32_to_cpu(x)              SWAP32((x))
#define cpu_to_be16(x)              SWAP16((x))
#define be16_to_cpu(x)              SWAP16((x))
#else
#define cpu_to_le32(x)              SWAP32((x))
#define le32_to_cpu(x)              SWAP32((x))
#define cpu_to_le16(x)              SWAP16((x))
#define le16_to_cpu(x)              SWAP16((x))
#define cpu_to_be32(x)              ((u32)(x))
#define be32_to_cpu(x)              ((u32)(x))
#define cpu_to_be16(x)              ((u16)(x))
#define be16_to_cpu(x)              ((u16)(x))
#endif /*PLATFOM_IS_LITTLE_ENDIAN*/

typedef u16 __le16;
typedef u32 __le32;
typedef u16 __be16;
typedef u32 __be32;
#endif /*#ifndef PHL_PLATFORM_LINUX*/

#endif /*_PHL_TYPES_H_*/
