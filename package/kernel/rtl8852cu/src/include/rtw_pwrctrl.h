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
#ifndef __RTW_PWRCTRL_H_
#define __RTW_PWRCTRL_H_

typedef _sema _pwrlock;
/*
	BIT[2:0] = HW state
	BIT[3] = Protocol PS state,   0: register active state , 1: register sleep state
	BIT[4] = sub-state
*/

struct pwrctrl_priv {
	_pwrlock	lock;
	u32	alives;

	_workitem dma_event; /*for handle un-synchronized tx dma*/
	u8		bInSuspend;
#ifdef CONFIG_BTC
	u8		bAutoResume;
	u8		autopm_cnt;
#endif
	u8		bSupportRemoteWakeup;
	u8		wowlan_ap_mode;
	u8		wowlan_mode;
	u8		wowlan_p2p_mode;
	u8		wowlan_pno_enable;
	u8		wowlan_in_resume;

#ifdef CONFIG_GPIO_WAKEUP
#endif /* CONFIG_GPIO_WAKEUP */
	u8		hst2dev_high_active;
#ifdef CONFIG_WOWLAN
#ifdef CONFIG_IPV6
	u8		wowlan_ns_offload_en;
#endif /*CONFIG_IPV6*/
	u8		wowlan_txpause_status;
	u8		wowlan_pattern_idx;
	u64		wowlan_fw_iv;
	struct rtl_priv_pattern	patterns[MAX_WKFM_CAM_NUM];
	_mutex	wowlan_pattern_cam_mutex;
	u8		wowlan_aoac_rpt_loc;
	struct aoac_report wowlan_aoac_rpt;
#endif /* CONFIG_WOWLAN */
	u8		bkeepfwalive;
#ifdef CONFIG_RESUME_IN_WORKQUEUE
	struct workqueue_struct *rtw_workqueue;
	_workitem resume_work;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
	u8 do_late_resume;
#endif /* CONFIG_HAS_EARLYSUSPEND */

#ifdef CONFIG_ANDROID_POWER
	android_early_suspend_t early_suspend;
	u8 do_late_resume;
#endif
#ifdef CONFIG_RTW_CFGVENDOR_LLSTATS
	systime radio_on_start_time;
	systime pwr_saving_start_time;
	u32 pwr_saving_time;
	u32 on_time;
	u32 tx_time;
	u32 rx_time;
#endif /* CONFIG_RTW_CFGVENDOR_LLSTATS */
};

#ifdef CONFIG_RTW_IPS
bool rtw_core_set_ips_state(void *drv_priv, enum rtw_rf_state state);
#endif
extern void rtw_init_pwrctrl_priv(_adapter *adapter);
extern void rtw_free_pwrctrl_priv(_adapter *adapter);

#ifdef CONFIG_RESUME_IN_WORKQUEUE
void rtw_resume_in_workqueue(struct pwrctrl_priv *pwrpriv);
#endif /* CONFIG_RESUME_IN_WORKQUEUE */

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
bool rtw_is_earlysuspend_registered(struct pwrctrl_priv *pwrpriv);
bool rtw_is_do_late_resume(struct pwrctrl_priv *pwrpriv);
void rtw_set_do_late_resume(struct pwrctrl_priv *pwrpriv, bool enable);
void rtw_register_early_suspend(struct pwrctrl_priv *pwrpriv);
void rtw_unregister_early_suspend(struct pwrctrl_priv *pwrpriv);
#else
#define rtw_is_earlysuspend_registered(pwrpriv) _FALSE
#define rtw_is_do_late_resume(pwrpriv) _FALSE
#define rtw_set_do_late_resume(pwrpriv, enable) do {} while (0)
#define rtw_register_early_suspend(pwrpriv) do {} while (0)
#define rtw_unregister_early_suspend(pwrpriv) do {} while (0)
#endif /* CONFIG_HAS_EARLYSUSPEND || CONFIG_ANDROID_POWER */
void rtw_ssmps_enter(_adapter *adapter, struct sta_info *sta);
void rtw_ssmps_leave(_adapter *adapter, struct sta_info *sta);
#endif /* __RTL871X_PWRCTRL_H_ */