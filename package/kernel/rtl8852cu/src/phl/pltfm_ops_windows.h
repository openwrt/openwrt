/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef _HAL_PLTFM_WINDOWS_H_
#define _HAL_PLTFM_WINDOWS_H_
#include <ntdef.h>
#include <ndis.h>
#include <stdio.h>
#include <stdarg.h>
#include <ntstrsafe.h>
#include "Core_Includes.h"  //WDK header files

#include "StatusCode.h"
#include "EndianFree.h"
#include "LinkList.h"		//for N6C_pltfmdef - _RT_TIMER_HANDLE
#include "Ms_pltfmdef.h"	//defined u8 :: XXX_pltfmdef.h must before pltfm_def.h
#ifdef WIFICX_BASED
	#include "CX_pltfmdef.h"
#else
	#include "N6C_pltfmdef.h"
#endif
#include "pltfm_def.h"

#if defined(CONFIG_USB_HCI)
#include <wdftypes.h> //for WDFUSBPIPE
#include "core_util.h"
#include "core_usb.h"
#include "pltfm_usb.h"
#endif

#include "phl_config.h"
#ifdef CONFIG_PHL_WPP
//#include "hal_g6\phy\rf\halrf_wpp.h"
#include "hal_g6\mac\halmac_wpp.h"
#include "hal_g6\phy\bb\halbb_wpp.h"
#include "phl_wpp.h"
#include <rtwlanwpp.h>
#endif
#include "phl_types.h"
#include "phl_util.h"
#include "phl_def.h"
//#include "phl_types.h"
//#include "PlatformDef.h"

#define WriteLE4Byte(_ptr, _val)	WriteEF4Byte(_ptr,_val)
#define WriteLE2Byte(_ptr, _val)	WriteEF2Byte(_ptr,_val)
#define WriteLE1Byte(_ptr, _val)	WriteEF1Byte(_ptr,_val)

#define WriteBE4Byte(_ptr, _val)	WriteEF4Byte(_ptr,H2N1BYTE(_val))
#define WriteBE2Byte(_ptr, _val)	WriteEF2Byte(_ptr,H2N2BYTE(_val))
#define WriteBE1Byte(_ptr, _val)	WriteEF1Byte(_ptr,H2N4BYTE(_val))

static inline char *_os_strpbrk(const char *cs, const char *ct)
{
	const char *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1) {
		for (sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (char *)sc1;
		}
	}
	return NULL;
}

static inline char *_os_strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = _os_strpbrk((const char *)sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}


static inline int _os_strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}
static inline int _os_strncmp(const char *s1, const char *s2, size_t n)
{
	return strncmp(s1, s2, n);
}
static __inline char *_os_strcpy(char *dest, const char *src)
{
	return strcpy(dest, src);
}
static inline char *_os_strncpy(char *dest, const char *src, size_t n)
{
	return strncpy(dest, src, n);
}
static __inline char *_os_strchr(const char *s, int c)
{
	return strchr(s, c);
}

#define _os_snprintf(s, sz, fmt, ...) _snprintf(s, sz, fmt, ##__VA_ARGS__)
#define _os_vsnprintf(str, size, fmt, args) RtlStringCbVPrintfA(str, size, fmt, args)

#define _os_strncat strncat

static __inline u32 _os_strlen(u8 *buf)
{
	return (u32)strlen((const char *)buf);
}

#define _os_sscanf(buf, fmt, ...) sscanf_s(buf, fmt, ##__VA_ARGS__)

/* delay */
static __inline void _os_delay_ms(void * drv_priv, u32 ms)
{
	PlatformForceStallExecution(1000 * ms);
}
static __inline void _os_delay_us(void *h, u32 us)
{
	PlatformForceStallExecution(us);
}
static __inline void _os_sleep_ms(void *h, u32 ms)
{
	PlatformForceStallExecution(1000 * ms);
}
static __inline void _os_sleep_us(void *h, u32 us)
{
	PlatformForceStallExecution(us);
}

static inline u32 _os_get_cur_time_us(void)
{
	u64 ret;
	ret = PlatformGetCurrentTime();
	return (u32)ret;
}

static inline u32 _os_get_cur_time_ms(void)
{
	u64 ret;
	ret = PlatformGetCurrentTime() / 1000;
	return (u32)ret;
}

static inline u64 _os_modular64(u64 x, u64 y)
{
	return x % y;
}
static inline u64 _os_division64(u64 x, u64 y)
{
	return x / y;
}

static inline u64 _os_minus64(u64 x, u64 y)

{
	return x - y;
}

static inline u64 _os_add64(u64 x, u64 y)

{
	return x + y;
}

static inline u32 _os_div_round_up(u32 x, u32 y)
{
	return (x + y - 1) / y;
}

static inline void *_os_pkt_buf_unmap_rx(void *d, u32 bus_addr_l, u32 bus_addr_h, u32 buf_sz)
{
	return NULL;
}

static inline void *_os_pkt_buf_map_rx(void *d, u32 *bus_addr_l, u32 *bus_addr_h,
					u32 buf_sz, void *os_priv)
{
	return NULL;
}

static inline void *_os_pkt_buf_alloc_rx(void *d, u32 *bus_addr_l,
			u32 *bus_addr_h, u32 buf_sz, void **os_priv)
{
	struct _SHARED_MEMORY share_mem;

	PlatformZeroMemory(&share_mem, sizeof(share_mem));

	if (PlatformAllocateSharedMemory(d, &share_mem, buf_sz) != RT_STATUS_SUCCESS) {
		share_mem.VirtualAddress = NULL;
		*bus_addr_l = 0;
		*bus_addr_h = 0;
		*os_priv = NULL;
	} else {
		PlatformZeroMemory(share_mem.VirtualAddress, buf_sz);
#ifdef CONFIG_PCI_HCI
		*bus_addr_l = share_mem.PhysicalAddressLow;
		*bus_addr_h = share_mem.PhysicalAddressHigh;
#endif
#if WIFICX_BASED
		*os_priv = share_mem.pltfm_rsvd[0];
#endif
	}
	return (u8 *)share_mem.VirtualAddress;
}
static inline void _os_pkt_buf_free_rx(void *d, u8 *vir_addr, u32 bus_addr_l,
			u32 bus_addr_h, u32 buf_sz, void *os_priv)
{
	struct _SHARED_MEMORY share_mem;

	if (NULL != vir_addr) {
		share_mem.VirtualAddress = (pu1Byte)vir_addr;
		share_mem.PhysicalAddressLow = (u4Byte)bus_addr_l;
		share_mem.PhysicalAddressHigh = (u4Byte)bus_addr_h;
		share_mem.Length = (u4Byte)buf_sz;
#if WIFICX_BASED
		share_mem.pltfm_rsvd[0] = os_priv;
#endif
		PlatformFreeSharedMemory(d, &share_mem);
	}
}

/* phl pre-alloc network layer buffer */
static inline void * _os_alloc_netbuf(void *d, u32 buf_sz, void **os_priv)
{
	return NULL; // windows never do this.
}

/* Free netbuf for error case. (ex. drop rx-reorder packet) */
static inline void _os_free_netbuf(void *d, u8 *vir_addr, u32 buf_sz, void *os_priv)
{
}

#ifdef CONFIG_PCI_HCI
static inline void _os_cache_inv(void *d, u32 *bus_addr_l, u32 *bus_addr_h,
					u32 buf_sz, u8 direction)
{
}
static inline void _os_cache_wback(void *d, u32 *bus_addr_l,
			u32 *bus_addr_h, u32 buf_sz, u8 direction)
{
}

/* txbd, rxbd, wd */
static inline void *_os_shmem_alloc(void *d, u32 *bus_addr_l,
				    u32 *bus_addr_h, u32 buf_sz,
				    u8 cache, u8 direction, void **os_rsvd)
{
	struct _SHARED_MEMORY share_mem;

	PlatformZeroMemory(&share_mem, sizeof(share_mem));

	if (PlatformAllocateSharedMemory(d, &share_mem, buf_sz) != RT_STATUS_SUCCESS) {
		share_mem.VirtualAddress = NULL;
		*bus_addr_l = 0;
		*bus_addr_h = 0;
	} else {
		PlatformZeroMemory(share_mem.VirtualAddress, buf_sz);
		*bus_addr_l = share_mem.PhysicalAddressLow;
		*bus_addr_h = share_mem.PhysicalAddressHigh;
#if WIFICX_BASED
		*os_rsvd = share_mem.pltfm_rsvd[0];
#endif
	}
	return (u8 *)share_mem.VirtualAddress;
}
static inline void _os_shmem_free(void *d, u8 *vir_addr, u32 *bus_addr_l,
				  u32 *bus_addr_h, u32 buf_sz,
				  u8 cache, u8 direction, void *os_rsvd)
{
	struct _SHARED_MEMORY share_mem;

	PlatformZeroMemory(&share_mem, sizeof(share_mem));

	if (NULL != vir_addr) {
		share_mem.VirtualAddress = (pu1Byte)vir_addr;
		share_mem.PhysicalAddressLow = (u4Byte)*bus_addr_l;
		share_mem.PhysicalAddressHigh = (u4Byte)*bus_addr_h;
		share_mem.Length = (u4Byte)buf_sz;
#if WIFICX_BASED
		share_mem.pltfm_rsvd[0] = os_rsvd;
#endif
		PlatformFreeSharedMemory(d, &share_mem);
	}
}
#endif /*CONFIG_PCI_HCI*/

/* Generate an unsigned 32-bit random number */
static inline u32 _os_random32(void *d)
{
	return 0;
}

#define _os_mem_alloc(h, buf_sz) _os_mem_alloc_with_tag(h, GenTag(__func__), buf_sz)
/* memory */
static __inline void *_os_mem_alloc_with_tag(void *h, u32 tag, u32 buf_sz)
{
	PVOID ptr = NULL;
	if (PlatformAllocateMemoryWithTag(tag, &ptr, buf_sz) != RT_STATUS_SUCCESS)
		return NULL;

	PlatformZeroMemory(ptr, buf_sz);
	return ptr;
}
static __inline void _os_mem_free(void *h, void *buf, u32 buf_sz)
{
	if(buf)
		PlatformFreeMemory(buf, buf_sz);
}

#define _os_kmem_alloc(h, buf_sz) _os_kmem_alloc_with_tag(h, GenTag(__func__), buf_sz)
/*physically contiguous memory if the buffer will be accessed by a DMA device*/
static __inline void *_os_kmem_alloc_with_tag(void *h, u32 tag, u32 buf_sz)
{
	PVOID ptr = NULL;
	if (PlatformAllocateMemoryWithTag(tag, &ptr, buf_sz) != RT_STATUS_SUCCESS)
		return NULL;

	PlatformZeroMemory(ptr, buf_sz);
	return ptr;
}

/*physically contiguous memory if the buffer will be accessed by a DMA device*/
static __inline void _os_kmem_free(void *h, void *buf, u32 buf_sz)
{
	if(buf)
		PlatformFreeMemory(buf, buf_sz);
}
/*static __inline void *_os_aligment_mem_alloc(void *h, u32 buf_sz)
{
	PALIGNED_SHARED_MEMORY  pAlignedSharedMemory=NULL;

	PlatformAllocateAlignedSharedMemory()
}*/

static __inline void _os_mem_set(void *h, void *buf, s8 value, u32 buf_sz)
{
	PlatformFillMemory(buf, buf_sz, value);
}
static __inline void _os_mem_cpy(void *h, void *dest, const void *src, u32 buf_sz)
{
	PlatformMoveMemory(dest, (void *)src, buf_sz);
}
static __inline int _os_mem_cmp(void *h, const void *ptr1, const void *ptr2, u32 buf_sz)
{

	return PlatformCompareMemory((void *)ptr1, (void *)ptr2, buf_sz);
}

/* timer */
static __inline void _os_init_timer(void *drv_priv, _os_timer *timer,
		void (*call_back_func)(void *context), void *context,
		const char *sz_id)
{
	PlatformInitializeTimer(drv_priv, timer, (RT_TIMER_CALL_BACK)call_back_func, context, sz_id);
}
static __inline void _os_set_timer(void *drv_priv, _os_timer *timer, u32 ms_delay)
{
	PlatformSetTimer(drv_priv, timer, ms_delay);
}
static __inline void _os_cancel_timer(void *drv_priv, _os_timer *timer)
{
	PlatformCancelTimer(drv_priv, timer);
}
static inline void _os_cancel_timer_async(void *drv_priv, _os_timer *timer)
{
	PlatformCancelTimer(drv_priv, timer);
}
static __inline void _os_release_timer(void *drv_priv, _os_timer *timer)
{
	PlatformReleaseTimer(drv_priv, timer);
}


/* mutex */
static __inline void _os_mutex_init(void *h, _os_mutex *mutex)
{
	PlatformInitializeMutex(mutex);
}

static __inline void _os_mutex_deinit(void *h, _os_mutex *mutex)
{
	PlatformFreeMutex(mutex);
}

static __inline void _os_mutex_lock(void *h, _os_mutex *mutex)
{
	PlatformAcquireMutex(mutex);
}

static __inline void _os_mutex_unlock(void *h, _os_mutex *mutex)
{
	PlatformReleaseMutex(mutex);
}


/* Semaphore */
static inline void _os_sema_init(void *h, _os_sema *sema, int int_cnt)
{
	PlatformInitializeSemaphore(sema, int_cnt);
}

static inline void _os_sema_free(void *h, _os_sema *sema)
{
	PlatformFreeSemaphore(sema);
}

static inline void _os_sema_up(void *h, _os_sema *sema)
{

	PlatformReleaseSemaphore(sema);
}

static inline u8 _os_sema_down(void *h, _os_sema *sema)
{
	if(PlatformAcquireSemaphore(sema)==RT_STATUS_SUCCESS)
		return 0; //// RTW_PHL_STATUS_SUCCESS
	else
		return 1;

}

/* event */
static __inline void _os_event_init(void *h, _os_event *event)
{
	PlatformInitializeEvent(event);
}

static __inline void _os_event_free(void *h, _os_event *event)
{
	PlatformFreeEvent(event);
}

static __inline void _os_event_reset(void *h, _os_event *event)
{
	PlatformResetEvent(event);
}

static __inline void _os_event_set(void *h, _os_event *event)
{
	PlatformSetEvent(event);
}

/*
 * m_sec
 * 	== 0 : wait for completion
 * 	>  0 : wait for timeout or completion
 * return value
 * 	0:timeout
 * 	otherwise:success
 */
static __inline int _os_event_wait(void *h, _os_event *event, u32 m_sec)
{
	return PlatformWaitEvent(event, m_sec);
}




/* spinlock */
static __inline void _os_spinlock_init(void *d, _os_lock *plock)
{
	PLATFORM_INIT_RT_SPINLOCK(*plock);
}
static __inline void _os_spinlock_free(void *d, _os_lock *plock)
{
	PLATFORM_FREE_RT_SPINLOCK(*plock);
}

static inline void _os_spinlock(void *d, _os_lock *plock,
					enum lock_type type, _os_spinlockfg *flags)
{
	PLATFORM_ACQUIRE_RT_SPINLOCK(*plock);
}
static inline void _os_spinunlock(void *d, _os_lock *plock,
					enum lock_type type, _os_spinlockfg *flags)
{
	PLATFORM_RELEASE_RT_SPINLOCK(*plock);
}

/* tasklet/thread */
static __inline u8 _os_thread_init(	void *drv_priv, _os_thread *thread,
					int (*call_back_func)(void * context),
					void *context,
					const char namefmt[])
{
	if (PlatformInitializeThread(drv_priv,
			thread,
			(RT_THREAD_CALL_BACK)call_back_func,
			namefmt,
			0,
			context) == RT_STATUS_SUCCESS) //0: caller will wait for the event indefinitely.
		return 0;
	else
		return 1;
}

static __inline u8 _os_thread_deinit(void *drv_priv, _os_thread *thread)
{
	/* Terminate the thread */
	PlatformWaitThreadEnd(drv_priv, thread);
	PlatformCancelThread(drv_priv, thread);
	PlatformReleaseThread(drv_priv, thread);
	return 0;
}

static __inline enum rtw_phl_status _os_thread_schedule(void *drv_priv, _os_thread *thread)
{
	//PlatformSetEventTrigerThread(drv_priv, thread, PASSIVE_LEVEL, thread->pContext);
	PlatformRunThread(drv_priv, thread, PASSIVE_LEVEL);
	return RTW_PHL_STATUS_SUCCESS;
}
static inline void _os_thread_stop(void *drv_priv, _os_thread *thread)
{
	PlatformSetThreadEnd(drv_priv, thread);
}
static inline int _os_thread_check_stop(void *drv_priv, _os_thread *thread)
{
	return PlatformIsThreadEnd(drv_priv, thread);
}

static inline int _os_thread_wait_stop(void *drv_priv, _os_thread *thread)
{
	return RTW_PHL_STATUS_SUCCESS;
}

/* Workitem */
static __inline u8 _os_workitem_init(void *drv_priv, _os_workitem *workitem, void (*call_back_func)(void* context), void *context)
{
	if (PlatformInitializeWorkItem(drv_priv, workitem, (RT_WORKITEM_CALL_BACK)call_back_func,
			context, "phl_workitem") == RT_STATUS_SUCCESS)
	{
		PlatformStartWorkItem(workitem);
		return 0; // RTW_PHL_STATUS_SUCCESS
	}
	else
		return 1;
}
static __inline u8 _os_workitem_deinit(void *drv_priv, _os_workitem *workitem)
{
	PlatformStopWorkItem(workitem);
	PlatformFreeWorkItem(workitem);
	return 0;
}
static __inline u8 _os_workitem_schedule(void *drv_priv, _os_workitem *workitem)
{
	if(PlatformScheduleWorkItem(workitem) == TRUE)
		return 0; // RTW_PHL_STATUS_SUCCESS
	else
		return 1;
}

/*
static __inline void _os_workitem_run(void *h, hal_thread *thread)
{
	//??
}
static __inline void _os_workitem_kill(void *h, hal_thread *thread)
{
	PlatformStopWorkItem(thread);
}
static __inline void _os_workitem_pause(void *h, hal_thread *thread)
{
}
static __inline void _os_workitem_resume(void *h, hal_thread *thread)
{
}
*/

/* tasklet */

static __inline int phl_notify_thread_callback(void *context)
{
	struct rtw_phl_handler *handler = (struct rtw_phl_handler *)context;
	_os_thread *thread = (_os_thread *) &(handler->os_handler.u.tasklet);
	do {
		if(_os_sema_down(handler->drv_priv, &(thread->sema)) != RTW_PHL_STATUS_SUCCESS )
			break;
		if(handler->status & RTW_PHL_HANDLER_STATUS_RELEASED)
			break;
		handler->callback(thread);
	} while (true);
	return 0;
}

static __inline u8 _os_tasklet_init(void *drv_priv, _os_tasklet *tasklet, void (*call_back_func)(void *context), void *context)
{
	_os_thread *actual_thread = (_os_thread *)tasklet;
	struct rtw_phl_handler *handler;

	_os_thread_init(drv_priv, actual_thread, phl_notify_thread_callback,
			context, "phl_tasklet");
	_os_thread_schedule(drv_priv, actual_thread);

	handler = (struct rtw_phl_handler *)actual_thread->pContext;
	handler->status &= RTW_PHL_HANDLER_STATUS_INITIALIZED;
	return 0;
}

static __inline u8 _os_tasklet_deinit(void *drv_priv, _os_tasklet *tasklet)
{
	_os_thread *actual_thread = (_os_thread *)tasklet;
	struct rtw_phl_handler *handler = (struct rtw_phl_handler *)actual_thread->pContext;
	handler->status |= RTW_PHL_HANDLER_STATUS_RELEASED;
	_os_sema_up(drv_priv, &(actual_thread->sema));
	_os_thread_deinit(drv_priv, actual_thread);
	return 0;
}

static __inline enum rtw_phl_status _os_tasklet_schedule(void *drv_priv, _os_tasklet *tasklet)
{
	_os_thread *actual_thread = (_os_thread *)tasklet;

	_os_sema_up(drv_priv, &(actual_thread->sema));
	return RTW_PHL_STATUS_SUCCESS;
}


static _inline int _os_test_and_clear_bit(int nr, unsigned long *addr)
{
	/*UNDO*/
	return 0;
}
static _inline int _os_test_and_set_bit(int nr, unsigned long *addr)
{
	/*UNDO*/
	return 1;
}


/* Atomic integer operations */
static __inline void _os_atomic_set(void *d, _os_atomic *v, int i)
{
	InterlockedExchange(v, i);
}

static __inline int _os_atomic_read(void *d, _os_atomic *v)
{
	return *v;
}

static __inline void _os_atomic_add(void *d, _os_atomic *v, int i)
{
	InterlockedExchangeAdd(v, i);
}
static __inline void _os_atomic_sub(void *d, _os_atomic *v, int i)
{
}

static __inline void _os_atomic_inc(void *d, _os_atomic *v)
{
	InterlockedIncrement(v);
}

static __inline void _os_atomic_dec(void *d, _os_atomic *v)
{
	InterlockedDecrement(v);
}

static __inline int _os_atomic_add_return(void *d, _os_atomic *v, int i)
{
	return 0;
}

static __inline int _os_atomic_sub_return(void *d, _os_atomic *v, int i)
{
	return 0;
}

static __inline int _os_atomic_inc_return(void *d, _os_atomic *v)
{
	return InterlockedIncrement(v);
}

static __inline int _os_atomic_dec_return(void *d, _os_atomic *v)
{
	return InterlockedDecrement(v);
}

/* OS handler extension */
static inline u8 _os_init_handler_ext(void *drv_priv,
                                      void *phl_handler)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static inline u8 _os_deinit_handler_ext(void *drv_priv,
                                        void *phl_handler)
{
	return RTW_PHL_STATUS_SUCCESS;
}

/* File Operation */
static inline u32 _os_read_file(const char *path, u8 *buf, u32 sz)
{
	/* OS Dependent API */
	return platform_read_file(path, buf, sz);
}

/* Network Function */
#ifdef CONFIG_RTW_MIRROR_DUMP
static inline u32 _os_mirror_dump(u8 *hdr, u32 hdr_len, u8 *buf, u32 sz)
{
	return 0;
}
#endif

/*
static __inline bool _os_atomic_inc_unless(void *d, _os_atomic *v, int u)
{
	return 0;
}
*/

#ifdef CONFIG_PCI_HCI
static __inline u8 _os_read8_pcie(void *drv_priv, u32 addr)
{
	return PlatformEFIORead1Byte(drv_priv, addr);
}
static __inline u16 _os_read16_pcie(void *drv_priv, u32 addr)
{
	return PlatformEFIORead2Byte(drv_priv, addr);
}
static __inline u32 _os_read32_pcie(void *drv_priv, u32 addr)
{
	return PlatformEFIORead4Byte(drv_priv, addr);
}

static __inline u32 _os_write8_pcie(void *drv_priv, u32 addr, u8 val)
{
	PlatformEFIOWrite1Byte(drv_priv, addr, val);
	return 0;
}
static __inline u32 _os_write16_pcie(void *drv_priv, u32 addr, u16 val)
{
	PlatformEFIOWrite2Byte(drv_priv, addr, val);
	return 0;
}
static __inline u32 _os_write32_pcie(void *drv_priv, u32 addr, u32 val)
{
	PlatformEFIOWrite4Byte(drv_priv, addr, val);
	return 0;
}
#endif/*#ifdef CONFIG_PCI_HCI*/

#ifdef CONFIG_USB_HCI

static __inline u32 _os_usbctrl_vendorreq(void *drv_priv, u8 request, u16 value,
			u16 index, void *pdata, u16 len, u8 requesttype)
{
	// return value ?? RTW_PHL_STATUS or boolean??
	return pltfm_usb_ctrl_vendor_request(
			drv_priv,
			request,
			value,
			index,
			pdata,
			len,
			requesttype);

}

static inline int os_usb_tx(void *h, u8 *tx_buf_ptr,
			u8 bulk_id, u32 len, u8 *pkt_data_buf)
{
	if(pltfm_usb_out_token_send(h, tx_buf_ptr, pkt_data_buf, len, bulk_id) == TRUE)
		return 0; // RTW_PHL_STATUS_SUCCESS
	else
		return 1;
}

static __inline void os_enable_usb_out_pipes(void *drv_priv)
{
	pltfm_usb_out_pipes_start(drv_priv);
}

static __inline void os_disable_usb_out_pipes(void *drv_priv)
{
	pltfm_usb_out_pipes_stop(drv_priv);
}

static __inline u8 os_out_token_alloc(void *drv_priv)
{
	if(core_usb_out_token_init(drv_priv)== TRUE)
		return 0; // RTW_PHL_STATUS_SUCCESS
	else
		return 1;
}

static __inline void os_out_token_free(void *drv_priv)
{
	core_usb_out_token_deinit(drv_priv);
}

static __inline u8 os_in_token_alloc(void *drv_priv)
{
	if(core_usb_in_token_init(drv_priv)== TRUE)
		return 0; // RTW_PHL_STATUS_SUCCESS
	else
		return 1;
}

static __inline void os_in_token_free(void *drv_priv)
{
	core_usb_in_token_deinit(drv_priv);
}

static __inline u8 os_send_usb_in_token(void *drv_priv, void *rxobj, u8 *inbuf, u32 inbuf_len, u8 pipe_idx, u8 minLen)
{
	if(pltfm_usb_in_token_send(drv_priv, rxobj, inbuf, inbuf_len, pipe_idx, minLen) == TRUE)
		return 0;// RTW_PHL_STATUS_SUCCESS
	else
		return 1;
}

static __inline void os_enable_usb_in_pipes(void *drv_priv)
{
	pltfm_usb_in_pipes_start(drv_priv);
}

static __inline void os_disable_usb_in_pipes(void *drv_priv)
{
	pltfm_usb_in_pipes_stop(drv_priv);
}

#endif /*CONFIG_USB_HCI*/

#ifdef CONFIG_SDIO_HCI
static __inline u8 _os_sdio_cmd52_r8(void *d, u32 offset)
{
	return PlatformEFSdioLocalCmd52Read1Byte(d, offset);
}

static __inline u8 _os_sdio_cmd53_r8(void *d, u32 offset)
{
	return PlatformEFSdioLocalCmd53Read1Byte(d, offset);
}

static __inline u16 _os_sdio_cmd53_r16(void *d, u32 offset)
{
	return PlatformEFSdioLocalCmd53Read2Byte(d, offset);
}

static __inline u32 _os_sdio_cmd53_r32(void *d, u32 offset)
{
	return PlatformEFSdioLocalCmd53Read4Byte(d, offset);
}

static __inline u8 _os_sdio_cmd53_rn(void *d, u32 offset, u32 size, u8 *data)
{
	if (!data){
		return _FAIL;
	}

	PlatformEFSdioLocalCmd53ReadNByte(d, offset, size, (pu1Byte)data);

	return _SUCCESS;
}

static __inline u8 _os_sdio_cmd53_r(void *d, u32 offset, u32 size, u8 *data)
{

	PlatformEFSdioLocalCmd53ReadNByte(d, offset, size, data);

	return _SUCCESS;
}

static __inline void _os_sdio_cmd52_w8(void *d, u32 offset, u8 val)
{
	PlatformEFSdioLocalCmd52Write1Byte(d, offset, val);
}

static __inline void _os_sdio_cmd53_w8(void *d, u32 offset, u8 val)
{
	PlatformEFSdioLocalCmd53Write1Byte(d, offset, val);
}

static __inline void _os_sdio_cmd53_w16(void *d, u32 offset, u16 val)
{
	PlatformEFSdioLocalCmd53Write2Byte(d, offset, val);
}

static __inline void _os_sdio_cmd53_w32(void *d, u32 offset, u32 val)
{
	PlatformEFSdioLocalCmd53Write4Byte(d, offset, val);
}

static __inline void _os_sdio_cmd53_wn(void *d, u32 offset, u32 size, u8 *data)
{
	PlatformEFSdioLocalCmd53WriteNByte(d, offset, size, (pu1Byte)data);
}

static __inline void _os_sdio_cmd53_w(void *d, u32 offset, u32 size, u8 *data)
{
	PlatformEFSdioLocalCmd53WriteNByte(d, offset, size, (pu1Byte)data);
}

static __inline u8 _os_sdio_f0_read(void *d, u32 addr, void *buf, size_t len)
{
	return 0;
}

static __inline u8 _os_sdio_read_cia_r8(void *d, u32 addr)
{
	return 0;
}

#endif /*CONFIG_SDIO_HCI*/


#endif /*_HAL_PLTFM_WINDOWS_H_*/
