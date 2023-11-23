/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef __OSDEP_SERVICE_H_
#define __OSDEP_SERVICE_H_

#define RTW_RX_HANDLED			2
#define RTW_RFRAME_UNAVAIL		3
#define RTW_RFRAME_PKT_UNAVAIL		4
#define RTW_RBUF_UNAVAIL		5
#define RTW_RBUF_PKT_UNAVAIL		6
#define RTW_SDIO_RECV_FAIL	7
#define RTW_ALREADY			8
#define RTW_RA_RESOLVING		9
#define RTW_ORI_NO_NEED			10

/* #define RTW_STATUS_TIMEDOUT -110 */



#ifdef PLATFORM_FREEBSD
	#include <osdep_service_bsd.h>
#endif

#ifdef PLATFORM_LINUX
	#include <linux/version.h>
#if defined(CONFIG_RTW_ANDROID_GKI)
	#include <linux/firmware.h>
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
	#include <linux/sched/signal.h>
	#include <linux/sched/types.h>
#endif
	#include <osdep_service_linux.h>
#endif

/* #include <rtw_byteorder.h> */

#ifndef BIT
	#define BIT(x)	(1 << (x))
#endif
#ifndef BIT_ULL
#define BIT_ULL(x)	(1ULL << (x))
#endif

#define CHECK_BIT(a, b) (!!((a) & (b)))

#define BIT0	0x00000001
#define BIT1	0x00000002
#define BIT2	0x00000004
#define BIT3	0x00000008
#define BIT4	0x00000010
#define BIT5	0x00000020
#define BIT6	0x00000040
#define BIT7	0x00000080
#define BIT8	0x00000100
#define BIT9	0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000
#define BIT32	0x0100000000
#define BIT33	0x0200000000
#define BIT34	0x0400000000
#define BIT35	0x0800000000
#define BIT36	0x1000000000

#ifndef GENMASK
#define GENMASK(h, l) \
	(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#endif

extern int RTW_STATUS_CODE(int error_code);

#ifndef RTK_DMP_PLATFORM
	#define CONFIG_USE_VMALLOC
#endif

/* flags used for rtw_mstat_update() */
enum mstat_f {
	/* type: 0x00ff */
	MSTAT_TYPE_VIR = 0x00,
	MSTAT_TYPE_PHY = 0x01,
	MSTAT_TYPE_SKB = 0x02,
	MSTAT_TYPE_USB = 0x03,
	MSTAT_TYPE_MAX = 0x04,

	/* func: 0xff00 */
	MSTAT_FUNC_UNSPECIFIED = 0x00 << 8,
	MSTAT_FUNC_IO = 0x01 << 8,
	MSTAT_FUNC_TX_IO = 0x02 << 8,
	MSTAT_FUNC_RX_IO = 0x03 << 8,
	MSTAT_FUNC_TX = 0x04 << 8,
	MSTAT_FUNC_RX = 0x05 << 8,
	MSTAT_FUNC_CFG_VENDOR = 0x06 << 8,
	MSTAT_FUNC_MAX = 0x07 << 8,
};

#define mstat_tf_idx(flags) ((flags) & 0xff)
#define mstat_ff_idx(flags) (((flags) & 0xff00) >> 8)

typedef enum mstat_status {
	MSTAT_ALLOC_SUCCESS = 0,
	MSTAT_ALLOC_FAIL,
	MSTAT_FREE
} MSTAT_STATUS;


#ifdef DBG_MEM_ALLOC
void rtw_mstat_update(const enum mstat_f flags, const MSTAT_STATUS status, u32 sz);
void rtw_mstat_dump(void *sel);
bool match_mstat_sniff_rules(const enum mstat_f flags, const size_t size);
void *dbg_rtw_vmalloc(u32 sz, const enum mstat_f flags, const char *func, const int line);
void *dbg_rtw_zvmalloc(u32 sz, const enum mstat_f flags, const char *func, const int line);
void dbg_rtw_vmfree(void *pbuf, const enum mstat_f flags, u32 sz, const char *func, const int line);
void *dbg_rtw_malloc(u32 sz, const enum mstat_f flags, const char *func, const int line);
void *dbg_rtw_zmalloc(u32 sz, const enum mstat_f flags, const char *func, const int line);
void dbg_rtw_mfree(void *pbuf, const enum mstat_f flags, u32 sz, const char *func, const int line);

struct sk_buff *dbg_rtw_skb_alloc(unsigned int size, const enum mstat_f flags, const char *func, const int line);
void dbg_rtw_skb_free(struct sk_buff *skb, const enum mstat_f flags, const char *func, const int line);
struct sk_buff *dbg_rtw_skb_copy(const struct sk_buff *skb, const enum mstat_f flags, const char *func, const int line);
struct sk_buff *dbg_rtw_skb_clone(struct sk_buff *skb, const enum mstat_f flags, const char *func, const int line);
int dbg_rtw_skb_linearize(struct sk_buff *skb, const enum mstat_f flags, const char *func, int line);
int dbg_rtw_netif_rx(_nic_hdl ndev, struct sk_buff *skb, const enum mstat_f flags, const char *func, int line);
#ifdef CONFIG_RTW_NAPI
int dbg_rtw_netif_receive_skb(_nic_hdl ndev, struct sk_buff *skb, const enum mstat_f flags, const char *func, int line);
#ifdef CONFIG_RTW_GRO
gro_result_t dbg_rtw_napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb, const enum mstat_f flags, const char *func, int line);
#endif
#endif /* CONFIG_RTW_NAPI */
void dbg_rtw_skb_queue_purge(struct sk_buff_head *list, enum mstat_f flags, const char *func, int line);
#ifdef CONFIG_USB_HCI
void *dbg_rtw_usb_buffer_alloc(struct usb_device *dev, size_t size, dma_addr_t *dma, const enum mstat_f flags, const char *func, const int line);
void dbg_rtw_usb_buffer_free(struct usb_device *dev, size_t size, void *addr, dma_addr_t dma, const enum mstat_f flags, const char *func, const int line);
#endif /* CONFIG_USB_HCI */

#ifdef CONFIG_USE_VMALLOC
#define rtw_vmalloc(sz)			dbg_rtw_vmalloc((sz), MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#define rtw_zvmalloc(sz)			dbg_rtw_zvmalloc((sz), MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#define rtw_vmfree(pbuf, sz)			dbg_rtw_vmfree((pbuf), (sz), MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#define rtw_vmalloc_f(sz, mstat_f)		dbg_rtw_vmalloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#define rtw_zvmalloc_f(sz, mstat_f)		dbg_rtw_zvmalloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#define rtw_vmfree_f(pbuf, sz, mstat_f)	dbg_rtw_vmfree((pbuf), (sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_VIR, __FUNCTION__, __LINE__)
#else /* CONFIG_USE_VMALLOC */
#define rtw_vmalloc(sz)			dbg_rtw_malloc((sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_zvmalloc(sz)			dbg_rtw_zmalloc((sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_vmfree(pbuf, sz)			dbg_rtw_mfree((pbuf), (sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_vmalloc_f(sz, mstat_f)		dbg_rtw_malloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_zvmalloc_f(sz, mstat_f)		dbg_rtw_zmalloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_vmfree_f(pbuf, sz, mstat_f)	dbg_rtw_mfree((pbuf), (sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#endif /* CONFIG_USE_VMALLOC */
#define rtw_malloc(sz)				dbg_rtw_malloc((sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_zmalloc(sz)			dbg_rtw_zmalloc((sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_mfree(pbuf, sz)			dbg_rtw_mfree((pbuf), (sz), MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_malloc_f(sz, mstat_f)		dbg_rtw_malloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_zmalloc_f(sz, mstat_f)		dbg_rtw_zmalloc((sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)
#define rtw_mfree_f(pbuf, sz, mstat_f)	dbg_rtw_mfree((pbuf), (sz), ((mstat_f) & 0xff00) | MSTAT_TYPE_PHY, __FUNCTION__, __LINE__)

#define rtw_skb_alloc(size)			dbg_rtw_skb_alloc((size), MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_free(skb)			dbg_rtw_skb_free((skb), MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_alloc_f(size, mstat_f)	dbg_rtw_skb_alloc((size), ((mstat_f) & 0xff00) | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_free_f(skb, mstat_f)		dbg_rtw_skb_free((skb), ((mstat_f) & 0xff00) | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_copy(skb)			dbg_rtw_skb_copy((skb), MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_clone(skb)			dbg_rtw_skb_clone((skb), MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_copy_f(skb, mstat_f)		dbg_rtw_skb_copy((skb), ((mstat_f) & 0xff00) | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_clone_f(skb, mstat_f)		dbg_rtw_skb_clone((skb), ((mstat_f) & 0xff00) | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#define rtw_skb_linearize(skb)			dbg_rtw_skb_linearize((skb), MSTAT_TYPE_SKB, __func__, __LINE__)
#define rtw_netif_rx(ndev, skb)		dbg_rtw_netif_rx(ndev, skb, MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#ifdef CONFIG_RTW_NAPI
#define rtw_netif_receive_skb(ndev, skb)	dbg_rtw_netif_receive_skb(ndev, skb, MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#ifdef CONFIG_RTW_GRO
#define rtw_napi_gro_receive(napi, skb)	dbg_rtw_napi_gro_receive(napi, skb, MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#endif
#endif /* CONFIG_RTW_NAPI */
#define rtw_skb_queue_purge(sk_buff_head)	dbg_rtw_skb_queue_purge(sk_buff_head, MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#ifdef CONFIG_USB_HCI
#define rtw_usb_buffer_alloc(dev, size, dma)		dbg_rtw_usb_buffer_alloc((dev), (size), (dma), MSTAT_TYPE_USB, __FUNCTION__, __LINE__)
#define rtw_usb_buffer_free(dev, size, addr, dma)	dbg_rtw_usb_buffer_free((dev), (size), (addr), (dma), MSTAT_TYPE_USB, __FUNCTION__, __LINE__)
#define rtw_usb_buffer_alloc_f(dev, size, dma, mstat_f)			dbg_rtw_usb_buffer_alloc((dev), (size), (dma), ((mstat_f) & 0xff00) | MSTAT_TYPE_USB, __FUNCTION__, __LINE__)
#define rtw_usb_buffer_free_f(dev, size, addr, dma, mstat_f)	dbg_rtw_usb_buffer_free((dev), (size), (addr), (dma), ((mstat_f) & 0xff00) | MSTAT_TYPE_USB, __FUNCTION__, __LINE__)
#endif /* CONFIG_USB_HCI */

#else /* DBG_MEM_ALLOC */
#define rtw_mstat_update(flag, status, sz) do {} while (0)
#define rtw_mstat_dump(sel) do {} while (0)
#define match_mstat_sniff_rules(flags, size) _FALSE
void *_rtw_vmalloc(u32 sz);
void *_rtw_zvmalloc(u32 sz);
void _rtw_vmfree(void *pbuf, u32 sz);
void *_rtw_zmalloc(u32 sz);
void *_rtw_malloc(u32 sz);
void _rtw_mfree(void *pbuf, u32 sz);

struct sk_buff *_rtw_skb_alloc(u32 sz);
void _rtw_skb_free(struct sk_buff *skb);
#ifdef CONFIG_RTW_NAPI
int _rtw_netif_receive_skb(_nic_hdl ndev, struct sk_buff *skb);
#ifdef CONFIG_RTW_GRO
gro_result_t _rtw_napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb);
#endif
#endif /* CONFIG_RTW_NAPI */
void _rtw_skb_queue_purge(struct sk_buff_head *list);

#ifdef CONFIG_USE_VMALLOC
#define rtw_vmalloc(sz)			_rtw_vmalloc((sz))
#define rtw_zvmalloc(sz)			_rtw_zvmalloc((sz))
#define rtw_vmfree(pbuf, sz)			_rtw_vmfree((pbuf), (sz))
#define rtw_vmalloc_f(sz, mstat_f)		_rtw_vmalloc((sz))
#define rtw_zvmalloc_f(sz, mstat_f)		_rtw_zvmalloc((sz))
#define rtw_vmfree_f(pbuf, sz, mstat_f)	_rtw_vmfree((pbuf), (sz))
#else /* CONFIG_USE_VMALLOC */
#define rtw_vmalloc(sz)			_rtw_malloc((sz))
#define rtw_zvmalloc(sz)			_rtw_zmalloc((sz))
#define rtw_vmfree(pbuf, sz)			_rtw_mfree((pbuf), (sz))
#define rtw_vmalloc_f(sz, mstat_f)		_rtw_malloc((sz))
#define rtw_zvmalloc_f(sz, mstat_f)		_rtw_zmalloc((sz))
#define rtw_vmfree_f(pbuf, sz, mstat_f)	_rtw_mfree((pbuf), (sz))
#endif /* CONFIG_USE_VMALLOC */
#define rtw_malloc(sz)				_rtw_malloc((sz))
#define rtw_zmalloc(sz)			_rtw_zmalloc((sz))
#define rtw_mfree(pbuf, sz)			_rtw_mfree((pbuf), (sz))
#define rtw_malloc_f(sz, mstat_f)		_rtw_malloc((sz))
#define rtw_zmalloc_f(sz, mstat_f)		_rtw_zmalloc((sz))
#define rtw_mfree_f(pbuf, sz, mstat_f)	_rtw_mfree((pbuf), (sz))

#define rtw_skb_alloc(size) 			_rtw_skb_alloc((size))
#define rtw_skb_free(skb) 			_rtw_skb_free((skb))
#define rtw_skb_alloc_f(size, mstat_f)	_rtw_skb_alloc((size))
#define rtw_skb_free_f(skb, mstat_f)		_rtw_skb_free((skb))
#define rtw_skb_copy(skb)			_rtw_skb_copy((skb))
#define rtw_skb_clone(skb)			_rtw_skb_clone((skb))
#define rtw_skb_copy_f(skb, mstat_f)		_rtw_skb_copy((skb))
#define rtw_skb_clone_f(skb, mstat_f)		_rtw_skb_clone((skb))
#define rtw_skb_linearize(skb)			_rtw_skb_linearize(skb)
#define rtw_netif_rx(ndev, skb)		_rtw_netif_rx(ndev, skb)
#ifdef CONFIG_RTW_NAPI
#define rtw_netif_receive_skb(ndev, skb) _rtw_netif_receive_skb(ndev, skb)
#ifdef CONFIG_RTW_GRO
#define rtw_napi_gro_receive(napi, skb) _rtw_napi_gro_receive(napi, skb)
#endif
#endif /* CONFIG_RTW_NAPI */
#define rtw_skb_queue_purge(sk_buff_head) _rtw_skb_queue_purge(sk_buff_head)
#ifdef CONFIG_USB_HCI
#define rtw_usb_buffer_alloc(dev, size, dma) _rtw_usb_buffer_alloc((dev), (size), (dma))
#define rtw_usb_buffer_free(dev, size, addr, dma) _rtw_usb_buffer_free((dev), (size), (addr), (dma))
#define rtw_usb_buffer_alloc_f(dev, size, dma, mstat_f) _rtw_usb_buffer_alloc((dev), (size), (dma))
#define rtw_usb_buffer_free_f(dev, size, addr, dma, mstat_f) _rtw_usb_buffer_free((dev), (size), (addr), (dma))
#endif /* CONFIG_USB_HCI */
#endif /* DBG_MEM_ALLOC */


void *rtw_malloc2d(int h, int w, size_t size);
void rtw_mfree2d(void *pbuf, int h, int w, int size);
void _rtw_memcpy(void *dec, const void *sour, u32 sz);
void _rtw_memmove(void *dst, const void *src, u32 sz);
int _rtw_memcmp(const void *dst, const void *src, u32 sz);
int _rtw_memcmp2(const void *dst, const void *src, u32 sz);
void _rtw_memset(void *pbuf, int c, u32 sz);

void _rtw_init_listhead(_list *list);
u32 rtw_is_list_empty(_list *phead);
void rtw_list_insert_head(_list *plist, _list *phead);
void rtw_list_insert_tail(_list *plist, _list *phead);
void rtw_list_splice(_list *list, _list *head);
void rtw_list_splice_init(_list *list, _list *head);
void rtw_list_splice_tail(_list *list, _list *head);

void rtw_list_delete(_list *plist);

void rtw_hlist_head_init(rtw_hlist_head *h);
void rtw_hlist_add_head(rtw_hlist_node *n, rtw_hlist_head *h);
void rtw_hlist_del(rtw_hlist_node *n);
void rtw_hlist_add_head_rcu(rtw_hlist_node *n, rtw_hlist_head *h);
void rtw_hlist_del_rcu(rtw_hlist_node *n);

void _rtw_init_queue(_queue *pqueue);
void _rtw_deinit_queue(_queue *pqueue);
u32 _rtw_queue_empty(_queue	*pqueue);
u32 rtw_end_of_queue_search(_list *queue, _list *pelement);

systime _rtw_get_current_time(void);
u32 _rtw_systime_to_us(systime stime);
u32 _rtw_systime_to_ms(systime stime);
systime _rtw_ms_to_systime(u32 ms);
systime _rtw_us_to_systime(u32 us);
s32 _rtw_get_passing_time_ms(systime start);
s32 _rtw_get_remaining_time_ms(systime end);
s32 _rtw_get_time_interval_ms(systime start, systime end);
bool _rtw_time_after(systime a, systime b);
bool _rtw_time_after_eq(systime a, systime b);

#ifdef DBG_SYSTIME
#define rtw_get_current_time() ({systime __stime = _rtw_get_current_time(); __stime;})
#define rtw_systime_to_us(stime) ({u32 __us = _rtw_systime_to_us(stime); typecheck(systime, stime); __us;})
#define rtw_systime_to_ms(stime) ({u32 __ms = _rtw_systime_to_ms(stime); typecheck(systime, stime); __ms;})
#define rtw_ms_to_systime(ms) ({systime __stime = _rtw_ms_to_systime(ms); __stime;})
#define rtw_us_to_systime(us) ({systime __stime = _rtw_us_to_systime(us); __stime;})
#define rtw_get_passing_time_ms(start) ({u32 __ms = _rtw_get_passing_time_ms(start); typecheck(systime, start); __ms;})
#define rtw_get_remaining_time_ms(end) ({u32 __ms = _rtw_get_remaining_time_ms(end); typecheck(systime, end); __ms;})
#define rtw_get_time_interval_ms(start, end) ({u32 __ms = _rtw_get_time_interval_ms(start, end); typecheck(systime, start); typecheck(systime, end); __ms;})
#define rtw_time_after(a, b) ({bool __r = _rtw_time_after(a, b); typecheck(systime, a); typecheck(systime, b); __r;})
#define rtw_time_after_eq(a, b) ({bool __r = _rtw_time_after_eq(a, b); typecheck(systime, a); typecheck(systime, b); __r;})
#define rtw_time_before(a, b) ({bool __r = _rtw_time_after(b, a); typecheck(systime, a); typecheck(systime, b); __r;})
#define rtw_time_before_eq(a, b) ({bool __r = _rtw_time_after_eq(b, a); typecheck(systime, a); typecheck(systime, b); __r;})
#else
#define rtw_get_current_time() _rtw_get_current_time()
#define rtw_systime_to_us(stime) _rtw_systime_to_us(stime)
#define rtw_systime_to_ms(stime) _rtw_systime_to_ms(stime)
#define rtw_ms_to_systime(ms) _rtw_ms_to_systime(ms)
#define rtw_us_to_systime(us) _rtw_us_to_systime(us)
#define rtw_get_passing_time_ms(start) _rtw_get_passing_time_ms(start)
#define rtw_get_remaining_time_ms(end) _rtw_get_remaining_time_ms(end)
#define rtw_get_time_interval_ms(start, end) _rtw_get_time_interval_ms(start, end)
#define rtw_time_after(a, b) _rtw_time_after(a, b)
#define rtw_time_after_eq(a, b) _rtw_time_after(a, b)
#define rtw_time_before(a, b) _rtw_time_after(b, a)
#define rtw_time_before_eq(a, b) _rtw_time_after_eq(b, a)
#endif

void rtw_sleep_schedulable(int ms);

void rtw_msleep_os(int ms);
void rtw_usleep_os(int us);

u32 rtw_atoi(u8 *s);

#ifdef DBG_DELAY_OS
#define rtw_mdelay_os(ms) _rtw_mdelay_os((ms), __FUNCTION__, __LINE__)
#define rtw_udelay_os(ms) _rtw_udelay_os((ms), __FUNCTION__, __LINE__)
void _rtw_mdelay_os(int ms, const char *func, const int line);
void _rtw_udelay_os(int us, const char *func, const int line);
#else
void rtw_mdelay_os(int ms);
void rtw_udelay_os(int us);
#endif

void rtw_yield_os(void);

void rtw_init_timer(_timer *ptimer, void *pfunc, void *ctx);
__inline static unsigned char _cancel_timer_ex(_timer *ptimer)
{
	u8 bcancelled;

	_cancel_timer(ptimer, &bcancelled);

	return bcancelled;
}

__inline static void _cancel_timer_nowait(_timer *ptimer)
{
	_cancel_timer_async(ptimer);
}

#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ? 0 : 1)) << 2)

__inline static u32 _RND4(u32 sz)
{

	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;

}

__inline static u32 _RND8(u32 sz)
{

	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;

}

__inline static u32 _RND128(u32 sz)
{

	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;

}

__inline static u32 _RND256(u32 sz)
{

	u32	val;

	val = ((sz >> 8) + ((sz & 255) ? 1 : 0)) << 8;

	return val;

}

__inline static u32 _RND512(u32 sz)
{

	u32	val;

	val = ((sz >> 9) + ((sz & 511) ? 1 : 0)) << 9;

	return val;

}

__inline static u32 bitshift(u32 bitmask)
{
	u32 i;

	for (i = 0; i <= 31; i++)
		if (((bitmask >> i) &  0x1) == 1)
			break;

	return i;
}

static inline int largest_bit(u32 bitmask)
{
	int i;

	for (i = 31; i >= 0; i--)
		if (bitmask & BIT(i))
			break;

	return i;
}

static inline int largest_bit_64(u64 bitmask)
{
	int i;

	for (i = 63; i >= 0; i--)
		if (bitmask & BIT_ULL(i))
			break;

	return i;
}

#define rtw_abs(a) ((a) < 0 ? -(a) : (a))
#define rtw_min(a, b) (((a) > (b)) ? (b) : (a))
#define rtw_max(a, b) (((a) > (b)) ? (a) : (b))

#define rtw_is_range_empty(hi, lo) ((hi) == (lo))
#define rtw_is_range_a_in_b(a_hi, a_lo, b_hi, b_lo) (((a_hi) <= (b_hi)) && ((a_lo) >= (b_lo)))
#define rtw_is_range_adjacent(a_hi, a_lo, b_hi, b_lo) (((a_hi) == (b_lo)) || ((a_lo) == (b_hi)))
#define rtw_is_range_overlap(a_hi, a_lo, b_hi, b_lo) (((a_hi) > (b_lo)) && ((a_lo) < (b_hi)))

/*
* Combine two ranges if possible (hilo_s is empty or adjcent/overlap with hilo)
* @hi_s, @lo_s: range parameters to store combined range
* @hi, @lo: range parameters to be combined, if combined, set to 0 (empty)
*/
#define rtw_range_combine(hi_s, lo_s, hi, lo) \
	do { \
		if (rtw_is_range_empty(hi, lo)) {} \
		else if (rtw_is_range_empty(hi_s, lo_s)) { \
			(hi_s) = (hi); \
			(lo_s) = (lo); \
			(hi) = 0; (lo) = 0; \
		} else if (rtw_is_range_adjacent(hi_s, lo_s, hi, lo) \
			|| rtw_is_range_overlap(hi_s, lo_s, hi, lo) \
		) { \
			(hi_s) = rtw_max(hi_s, hi); \
			(lo_s) = rtw_min(lo_s, lo); \
			(hi) = 0; (lo) = 0; \
		} \
	} while (0)

/*
* Merge two ranges (no need to adjcent/overlap with each other)
* @hi_s, @lo_s: range parameters to store merged range
* @hi, @lo: range parameters to be merged
*/
#define rtw_range_merge(hi_s, lo_s, hi, lo) \
	do { \
		if (rtw_is_range_empty(hi, lo)) {} \
		else if (rtw_is_range_empty(hi_s, lo_s)) { \
			(hi_s) = (hi); \
			(lo_s) = (lo); \
		} else { \
			(hi_s) = rtw_max(hi_s, hi); \
			(lo_s) = rtw_min(lo_s, lo); \
		} \
	} while (0)

#ifndef MAC_FMT
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
#ifndef MAC_ARG
#define MAC_ARG(x) ((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3], ((u8 *)(x))[4], ((u8 *)(x))[5]
#endif

bool rtw_macaddr_is_larger(const u8 *a, const u8 *b);

void rtw_suspend_lock_init(void);
void rtw_suspend_lock_uninit(void);
void rtw_lock_suspend(void);
void rtw_unlock_suspend(void);
void rtw_lock_suspend_timeout(u32 timeout_ms);
void rtw_lock_traffic_suspend_timeout(u32 timeout_ms);
void rtw_resume_lock_suspend(void);
void rtw_resume_unlock_suspend(void);
#ifdef CONFIG_AP_WOWLAN
void rtw_softap_lock_suspend(void);
void rtw_softap_unlock_suspend(void);
#endif

void rtw_set_bit(int nr, unsigned long *addr);
void rtw_clear_bit(int nr, unsigned long *addr);
int rtw_test_and_clear_bit(int nr, unsigned long *addr);
int rtw_test_and_set_bit(int nr, unsigned long *addr);

/* File operation APIs, just for linux now */
#if !defined(CONFIG_RTW_ANDROID_GKI)
int rtw_is_dir_readable(const char *path);
int rtw_store_to_file(const char *path, u8 *buf, u32 sz);
#endif /* !defined(CONFIG_RTW_ANDROID_GKI) */
int rtw_is_file_readable(const char *path);
int rtw_is_file_readable_with_size(const char *path, u32 *sz);
int rtw_readable_file_sz_chk(const char *path, u32 sz);
int rtw_retrieve_from_file(const char *path, u8 *buf, u32 sz);
void rtw_free_netdev(struct net_device *netdev);

u64 rtw_modular64(u64 x, u64 y);
u64 rtw_division64(u64 x, u64 y);
 u32 rtw_random32(void);

/* Macros for handling unaligned memory accesses */

#define RTW_GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))
#define RTW_PUT_BE16(a, val)			\
	do {					\
		(a)[0] = ((u16) (val)) >> 8;	\
		(a)[1] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_LE16(a) ((u16) (((a)[1] << 8) | (a)[0]))
#define RTW_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define RTW_GET_BE24(a) ((((u32) (a)[0]) << 16) | (((u32) (a)[1]) << 8) | \
			 ((u32) (a)[2]))
#define RTW_PUT_BE24(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[2] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))
#define RTW_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_LE32(a) ((((u32) (a)[3]) << 24) | (((u32) (a)[2]) << 16) | \
			 (((u32) (a)[1]) << 8) | ((u32) (a)[0]))
#define RTW_PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define RTW_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))
#define RTW_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)

#define RTW_GET_LE64(a) ((((u64) (a)[7]) << 56) | (((u64) (a)[6]) << 48) | \
			 (((u64) (a)[5]) << 40) | (((u64) (a)[4]) << 32) | \
			 (((u64) (a)[3]) << 24) | (((u64) (a)[2]) << 16) | \
			 (((u64) (a)[1]) << 8) | ((u64) (a)[0]))
#define RTW_PUT_LE64(a, val)					\
	do {							\
		(a)[7] = (u8) ((((u64) (val)) >> 56) & 0xff);	\
		(a)[6] = (u8) ((((u64) (val)) >> 48) & 0xff);	\
		(a)[5] = (u8) ((((u64) (val)) >> 40) & 0xff);	\
		(a)[4] = (u8) ((((u64) (val)) >> 32) & 0xff);	\
		(a)[3] = (u8) ((((u64) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u64) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u64) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u64) (val)) & 0xff);		\
	} while (0)

void rtw_buf_free(u8 **buf, u32 *buf_len);
void rtw_buf_update(u8 **buf, u32 *buf_len, const u8 *src, u32 src_len);

struct rtw_cbuf {
	u32 write;
	u32 read;
	u32 size;
	void *bufs[0];
};

bool rtw_cbuf_full(struct rtw_cbuf *cbuf);
bool rtw_cbuf_empty(struct rtw_cbuf *cbuf);
bool rtw_cbuf_push(struct rtw_cbuf *cbuf, void *buf);
void *rtw_cbuf_pop(struct rtw_cbuf *cbuf);
struct rtw_cbuf *rtw_cbuf_alloc(u32 size);
void rtw_cbuf_free(struct rtw_cbuf *cbuf);

struct map_seg_t {
	u16 sa;
	u16 len;
	u8 *c;
};

struct map_t {
	u16 len;
	u16 seg_num;
	u8 init_value;
	struct map_seg_t *segs;
};

#define MAPSEG_ARRAY_ENT(_sa, _len, _c, arg...) \
	{ .sa = _sa, .len = _len, .c = (u8[_len]){ _c, ##arg}, }

#define MAPSEG_PTR_ENT(_sa, _len, _p) \
	{ .sa = _sa, .len = _len, .c = _p, }

#define MAP_ENT(_len, _seg_num, _init_v, _seg, arg...) \
	{ .len = _len, .seg_num = _seg_num, .init_value = _init_v, .segs = (struct map_seg_t[_seg_num]){ _seg, ##arg}, }

int map_readN(const struct map_t *map, u16 offset, u16 len, u8 *buf);
u8 map_read8(const struct map_t *map, u16 offset);

struct blacklist_ent {
	_list list;
	u8 addr[ETH_ALEN];
	systime exp_time;
};

int rtw_blacklist_add(_queue *blist, const u8 *addr, u32 timeout_ms);
int rtw_blacklist_del(_queue *blist, const u8 *addr);
int rtw_blacklist_search(_queue *blist, const u8 *addr);
void rtw_blacklist_flush(_queue *blist);
void dump_blacklist(void *sel, _queue *blist, const char *title);

/* String handler */

BOOLEAN is_null(char c);
BOOLEAN is_all_null(char *c, int len);
BOOLEAN is_eol(char c);
BOOLEAN is_space(char c);
BOOLEAN is_decimal(char chTmp);
BOOLEAN IsHexDigit(char chTmp);
BOOLEAN is_alpha(char chTmp);
char alpha_to_upper(char c);

int hex2num_i(char c);
int hex2byte_i(const char *hex);
int hexstr2bin(const char *hex, u8 *buf, size_t len);

/*
* ustrs
* str_0      str_1      str_2      str_3
* |          |          |          |          |
* |---------------- ustrs_len ----------------|
*/
#define ustrs_for_each_str(ustrs, ustrs_len, str) \
	for ((str) = (ustrs); (str) < (ustrs) + (ustrs_len); (str) += strlen(str) + 1)

void ustrs_add(char **ustrs, int *ustrs_len, const char *str);

/*
 * Write formatted output to sized buffer
 */

#endif
