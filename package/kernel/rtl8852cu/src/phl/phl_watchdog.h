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
#ifndef _PHL_WATCHDOG_H_
#define _PHL_WATCHDOG_H_

#define WDOG_PERIOD 2000

enum watchdog_state{
	WD_STATE_INIT,
	WD_STATE_STARTED,
	WD_STATE_STOP,
};

struct phl_watchdog {
	_os_timer wdog_timer;
	enum watchdog_state state;

	/* Only sw statistics or sw behavior or trigger FG cmd */
	void (*core_sw_wdog)(void *drv_priv);

	/* I/O, tx behavior, request power, ... */
	void (*core_hw_wdog)(void *drv_priv);
#ifdef PHL_WATCHDOG_REFINE
	/* Only sw statistics or sw behavior, this is will be call before trigger next watchdog */
	void (*core_post_sw_wdog)(void *drv_priv);
#endif
	u16 period;
};
void
phl_watchdog_cmd_complete_hdl(struct phl_info_t *phl_info);
enum rtw_phl_status
phl_watchdog_hw_cmd_hdl(struct phl_info_t *phl_info, enum rtw_phl_status psts);
enum rtw_phl_status
phl_watchdog_sw_cmd_hdl(struct phl_info_t *phl_info, enum rtw_phl_status psts);
void rtw_phl_watchdog_start(void *phl);
void rtw_phl_watchdog_stop(void *phl);

#ifdef CONFIG_FSM
void rtw_phl_watchdog_callback(void *phl);
#endif

#endif /*_PHL_WATCHDOG_H_*/

