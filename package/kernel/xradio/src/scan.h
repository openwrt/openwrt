/*
 * Scan interfaces for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef SCAN_H_INCLUDED
#define SCAN_H_INCLUDED

#include <linux/semaphore.h>
#include "wsm.h"

/* external */ struct sk_buff;
/* external */ struct cfg80211_scan_request;
/* external */ struct ieee80211_channel;
/* external */ struct ieee80211_hw;
/* external */ struct work_struct;

#define SCAN_MAX_DELAY      (3*HZ)   //3s, add by yangfh for connect

struct xradio_scan {
	struct semaphore lock;
	struct work_struct work;
#ifdef ROAM_OFFLOAD
	struct work_struct swork; /* scheduled scan work */
	struct cfg80211_sched_scan_request *sched_req;
#endif /*ROAM_OFFLOAD*/
	struct delayed_work timeout;
	struct cfg80211_scan_request *req;
	struct ieee80211_channel **begin;
	struct ieee80211_channel **curr;
	struct ieee80211_channel **end;
	struct wsm_ssid ssids[WSM_SCAN_MAX_NUM_OF_SSIDS];
	int output_power;
	int n_ssids;
	//add by liwei, for h64 ping WS550 BUG
	struct semaphore status_lock;
	int status;
	atomic_t in_progress;
	/* Direct probe requests workaround */
	struct delayed_work probe_work;
	int direct_probe;
	u8 if_id;
};

int xradio_hw_scan(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
                   struct ieee80211_scan_request *req);
#ifdef ROAM_OFFLOAD
int xradio_hw_sched_scan_start(struct ieee80211_hw *hw,
                               struct ieee80211_vif *vif,
                               struct cfg80211_sched_scan_request *req,
                               struct ieee80211_sched_scan_ies *ies);
void xradio_hw_sched_scan_stop(struct xradio_common *priv);
void xradio_sched_scan_work(struct work_struct *work);
#endif /*ROAM_OFFLOAD*/
void xradio_scan_work(struct work_struct *work);
void xradio_scan_timeout(struct work_struct *work);
void xradio_scan_complete_cb(struct xradio_common *priv,
                             struct wsm_scan_complete *arg);

/* ******************************************************************** */
/* Raw probe requests TX workaround					*/
void xradio_probe_work(struct work_struct *work);

#endif
