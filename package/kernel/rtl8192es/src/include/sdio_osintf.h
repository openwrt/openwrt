/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 ******************************************************************************/
#ifndef __SDIO_OSINTF_H__
#define __SDIO_OSINTF_H__

/* Compatibility helpers removed by the newer USB-only branch. */
#define rtw_zvmalloc(sz) rtw_zmalloc(sz)
#define rtw_vmfree(ptr, sz) rtw_mfree((ptr), (sz))
#define rtw_get_current_time() jiffies
#define _rtw_memset(ptr, value, len) memset((ptr), (value), (len))
#define _rtw_memcpy(dst, src, len) memcpy((dst), (src), (len))
#define _rtw_spinlock_free(lock) do { } while (0)
#define rtw_end_of_queue_search(head, pos) ((head) == (pos))
#define rtw_msleep_os(ms) msleep(ms)
#define rtw_yield_os() cond_resched()
#define IS_MCAST(addr) is_multicast_ether_addr(addr)
#define WIFI_BEACON IEEE80211_STYPE_BEACON
#define MAX_AMPDU_FACTOR_16K 1

#endif
