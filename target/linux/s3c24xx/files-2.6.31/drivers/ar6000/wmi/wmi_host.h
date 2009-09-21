#ifndef _WMI_HOST_H_
#define _WMI_HOST_H_
/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 *
 * This file contains local definitios for the wmi host module.
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/wmi/wmi_host.h#1 $
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

#ifdef __cplusplus
extern "C" {
#endif

struct wmi_stats {
    A_UINT32    cmd_len_err;
    A_UINT32    cmd_id_err;
};

struct wmi_t {
    A_BOOL                          wmi_ready;
    A_BOOL                          wmi_numQoSStream;
    A_UINT8                         wmi_wmiStream2AcMapping[WMI_PRI_MAX_COUNT];
    WMI_PRI_STREAM_ID               wmi_ac2WmiStreamMapping[WMM_NUM_AC];
    A_UINT16                        wmi_streamExistsForAC[WMM_NUM_AC];
    A_UINT8                         wmi_fatPipeExists;
    void                           *wmi_devt;
    struct wmi_stats                wmi_stats;
    struct ieee80211_node_table     wmi_scan_table;
    A_UINT8                         wmi_bssid[ATH_MAC_LEN];
    A_UINT8                         wmi_powerMode;
    A_UINT8                         wmi_phyMode;
    A_UINT8                         wmi_keepaliveInterval;
    A_MUTEX_T                       wmi_lock;
};

#define WMI_INIT_WMISTREAM_AC_MAP(w) \
{  (w)->wmi_wmiStream2AcMapping[WMI_BEST_EFFORT_PRI] = WMM_AC_BE; \
   (w)->wmi_wmiStream2AcMapping[WMI_LOW_PRI] = WMM_AC_BK; \
   (w)->wmi_wmiStream2AcMapping[WMI_HIGH_PRI] = WMM_AC_VI; \
   (w)->wmi_wmiStream2AcMapping[WMI_HIGHEST_PRI] = WMM_AC_VO; \
   (w)->wmi_ac2WmiStreamMapping[WMM_AC_BE] = WMI_BEST_EFFORT_PRI; \
   (w)->wmi_ac2WmiStreamMapping[WMM_AC_BK] = WMI_LOW_PRI; \
   (w)->wmi_ac2WmiStreamMapping[WMM_AC_VI] = WMI_HIGH_PRI; \
   (w)->wmi_ac2WmiStreamMapping[WMM_AC_VO] = WMI_HIGHEST_PRI; }

#define WMI_WMISTREAM_ACCESSCATEGORY(w,s)      (w)->wmi_wmiStream2AcMapping[s]
#define WMI_ACCESSCATEGORY_WMISTREAM(w,ac)       (w)->wmi_ac2WmiStreamMapping[ac]

#define LOCK_WMI(w)     A_MUTEX_LOCK(&(w)->wmi_lock);
#define UNLOCK_WMI(w)   A_MUTEX_UNLOCK(&(w)->wmi_lock);

#ifdef __cplusplus
}
#endif

#endif /* _WMI_HOST_H_ */
