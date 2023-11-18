/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _PHL_RX_AGG_H_
#define _PHL_RX_AGG_H_

/*
 * This value is used when removing a @phl_tid_ampdu_rx as a timeout value
 * wating for an event passed as an argument to _os_event_wait().  Note that the
 * timeout value 0 has a different meaning on Windows and Linux.  See the
 * implementation of _os_event_wait().
 */
#define PHL_REORDER_TIMER_SYNC_TO_MS 50

void phl_tid_ampdu_rx_free(struct phl_tid_ampdu_rx *r);
void phl_free_rx_reorder(struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta);
struct phl_tid_ampdu_rx *phl_tid_ampdu_rx_alloc(struct phl_info_t *phl_info,
                                                struct rtw_phl_stainfo_t *sta,
                                                u16 timeout, u16 ssn, u16 tid,
                                                u16 buf_size);
void phl_notify_reorder_sleep(void *phl, struct rtw_phl_stainfo_t *sta);
#endif /*_PHL_RX_AGG_H_*/
