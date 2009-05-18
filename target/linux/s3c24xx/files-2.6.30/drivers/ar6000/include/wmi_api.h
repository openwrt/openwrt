#ifndef _WMI_API_H_
#define _WMI_API_H_
/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 *
 * This file contains the definitions for the Wireless Module Interface (WMI).
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/include/wmi_api.h#2 $
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

/*
 * IP QoS Field definitions according to 802.1p
 */
#define BEST_EFFORT_PRI         0
#define BACKGROUND_PRI          1
#define EXCELLENT_EFFORT_PRI    3
#define CONTROLLED_LOAD_PRI     4
#define VIDEO_PRI               5
#define VOICE_PRI               6
#define NETWORK_CONTROL_PRI     7
#define MAX_NUM_PRI             8

#define UNDEFINED_PRI           (0xff)

/* simple mapping of IP TOS field to a WMI priority stream
 * this mapping was taken from the original linux driver implementation
 * The operation maps the following
 *
 *  */
#define IP_TOS_TO_WMI_PRI(tos)  \
    ((WMI_PRI_STREAM_ID)(((tos) >> 1) & 0x03))

#define WMI_IMPLICIT_PSTREAM_INACTIVITY_INT 5000 /* 5 seconds */


struct wmi_t;

void *wmi_init(void *devt);

void wmi_qos_state_init(struct wmi_t *wmip);
void wmi_shutdown(struct wmi_t *wmip);
A_UINT16  wmi_get_mapped_qos_queue(struct wmi_t *, A_UINT8);
A_STATUS wmi_dix_2_dot3(struct wmi_t *wmip, void *osbuf);
A_STATUS wmi_data_hdr_add(struct wmi_t *wmip, void *osbuf, A_UINT8 msgType);
A_STATUS wmi_dot3_2_dix(struct wmi_t *wmip, void *osbuf);
A_STATUS wmi_data_hdr_remove(struct wmi_t *wmip, void *osbuf);
A_STATUS wmi_syncpoint(struct wmi_t *wmip);
A_STATUS wmi_syncpoint_reset(struct wmi_t *wmip);
WMI_PRI_STREAM_ID wmi_get_stream_id(struct wmi_t *wmip, A_UINT8 trafficClass);
A_UINT8 wmi_implicit_create_pstream(struct wmi_t *wmip, void *osbuf, A_UINT8 dir, A_UINT8 up);

A_STATUS wmi_control_rx(struct wmi_t *wmip, void *osbuf);
void wmi_iterate_nodes(struct wmi_t *wmip, wlan_node_iter_func *f, void *arg);
void wmi_free_allnodes(struct wmi_t *wmip);
bss_t *wmi_find_node(struct wmi_t *wmip, const A_UINT8 *macaddr);


typedef enum {
    NO_SYNC_WMIFLAG = 0,
    SYNC_BEFORE_WMIFLAG,            /* transmit all queued data before cmd */
    SYNC_AFTER_WMIFLAG,             /* any new data waits until cmd execs */
    SYNC_BOTH_WMIFLAG,
    END_WMIFLAG                     /* end marker */
} WMI_SYNC_FLAG;

A_STATUS wmi_cmd_send(struct wmi_t *wmip, void *osbuf, WMI_COMMAND_ID cmdId,
                      WMI_SYNC_FLAG flag);
A_STATUS wmi_connect_cmd(struct wmi_t *wmip,
                         NETWORK_TYPE netType,
                         DOT11_AUTH_MODE dot11AuthMode,
                         AUTH_MODE authMode,
                         CRYPTO_TYPE pairwiseCrypto,
                         A_UINT8 pairwiseCryptoLen,
                         CRYPTO_TYPE groupCrypto,
                         A_UINT8 groupCryptoLen,
                         int ssidLength,
                         A_UCHAR *ssid,
                         A_UINT8 *bssid,
                         A_UINT16 channel,
                         A_UINT32 ctrl_flags);
A_STATUS wmi_reconnect_cmd(struct wmi_t *wmip,
                           A_UINT8 *bssid,
                           A_UINT16 channel);
A_STATUS wmi_disconnect_cmd(struct wmi_t *wmip);
A_STATUS wmi_getrev_cmd(struct wmi_t *wmip);
A_STATUS wmi_startscan_cmd(struct wmi_t *wmip, WMI_SCAN_TYPE scanType,
                           A_BOOL forceFgScan, A_BOOL isLegacy,
                           A_UINT32 homeDwellTime, A_UINT32 forceScanInterval);
A_STATUS wmi_scanparams_cmd(struct wmi_t *wmip, A_UINT16 fg_start_sec,
                            A_UINT16 fg_end_sec, A_UINT16 bg_sec,
                            A_UINT16 minact_chdw_msec,
                            A_UINT16 maxact_chdw_msec, A_UINT16 pas_chdw_msec,
                            A_UINT8 shScanRatio, A_UINT8 scanCtrlFlags,
                            A_UINT32 max_dfsch_act_time);
A_STATUS wmi_bssfilter_cmd(struct wmi_t *wmip, A_UINT8 filter, A_UINT32 ieMask);
A_STATUS wmi_probedSsid_cmd(struct wmi_t *wmip, A_UINT8 index, A_UINT8 flag,
                            A_UINT8 ssidLength, A_UCHAR *ssid);
A_STATUS wmi_listeninterval_cmd(struct wmi_t *wmip, A_UINT16 listenInterval, A_UINT16 listenBeacons);
A_STATUS wmi_bmisstime_cmd(struct wmi_t *wmip, A_UINT16 bmisstime, A_UINT16 bmissbeacons);
A_STATUS wmi_associnfo_cmd(struct wmi_t *wmip, A_UINT8 ieType,
                           A_UINT8 ieLen, A_UINT8 *ieInfo);
A_STATUS wmi_powermode_cmd(struct wmi_t *wmip, A_UINT8 powerMode);
A_STATUS wmi_ibsspmcaps_cmd(struct wmi_t *wmip, A_UINT8 pmEnable, A_UINT8 ttl,
                            A_UINT16 atim_windows, A_UINT16 timeout_value);
A_STATUS wmi_pmparams_cmd(struct wmi_t *wmip, A_UINT16 idlePeriod,
                           A_UINT16 psPollNum, A_UINT16 dtimPolicy);
A_STATUS wmi_disctimeout_cmd(struct wmi_t *wmip, A_UINT8 timeout);
A_STATUS wmi_sync_cmd(struct wmi_t *wmip, A_UINT8 syncNumber);
A_STATUS wmi_create_pstream_cmd(struct wmi_t *wmip, WMI_CREATE_PSTREAM_CMD *pstream);
A_STATUS wmi_delete_pstream_cmd(struct wmi_t *wmip, A_UINT8 trafficClass, A_UINT8 streamID);
A_STATUS wmi_set_bitrate_cmd(struct wmi_t *wmip, A_INT32 rate);
A_STATUS wmi_get_bitrate_cmd(struct wmi_t *wmip);
A_INT8   wmi_validate_bitrate(struct wmi_t *wmip, A_INT32 rate);
A_STATUS wmi_get_regDomain_cmd(struct wmi_t *wmip);
A_STATUS wmi_get_channelList_cmd(struct wmi_t *wmip);
A_STATUS wmi_set_channelParams_cmd(struct wmi_t *wmip, A_UINT8 scanParam,
                                   WMI_PHY_MODE mode, A_INT8 numChan,
                                   A_UINT16 *channelList);

A_STATUS wmi_set_snr_threshold_params(struct wmi_t *wmip,
                                       WMI_SNR_THRESHOLD_PARAMS_CMD *snrCmd);
A_STATUS wmi_set_rssi_threshold_params(struct wmi_t *wmip,
                                        WMI_RSSI_THRESHOLD_PARAMS_CMD *rssiCmd);
A_STATUS wmi_clr_rssi_snr(struct wmi_t *wmip);
A_STATUS wmi_set_lq_threshold_params(struct wmi_t *wmip,
                                      WMI_LQ_THRESHOLD_PARAMS_CMD *lqCmd);
A_STATUS wmi_set_rts_cmd(struct wmi_t *wmip, A_UINT16 threshold);
A_STATUS wmi_set_lpreamble_cmd(struct wmi_t *wmip, A_UINT8 status);

A_STATUS wmi_set_error_report_bitmask(struct wmi_t *wmip, A_UINT32 bitmask);

A_STATUS wmi_get_challenge_resp_cmd(struct wmi_t *wmip, A_UINT32 cookie,
                                    A_UINT32 source);
A_STATUS wmi_config_debug_module_cmd(struct wmi_t *wmip, A_UINT16 mmask,
                                     A_UINT16 tsr, A_BOOL rep, A_UINT16 size,
                                     A_UINT32 valid);
A_STATUS wmi_get_stats_cmd(struct wmi_t *wmip);
A_STATUS wmi_addKey_cmd(struct wmi_t *wmip, A_UINT8 keyIndex,
                        CRYPTO_TYPE keyType, A_UINT8 keyUsage,
                        A_UINT8 keyLength,A_UINT8 *keyRSC,
                        A_UINT8 *keyMaterial, A_UINT8 key_op_ctrl,
                        WMI_SYNC_FLAG sync_flag);
A_STATUS wmi_add_krk_cmd(struct wmi_t *wmip, A_UINT8 *krk);
A_STATUS wmi_delete_krk_cmd(struct wmi_t *wmip);
A_STATUS wmi_deleteKey_cmd(struct wmi_t *wmip, A_UINT8 keyIndex);
A_STATUS wmi_set_akmp_params_cmd(struct wmi_t *wmip,
                                 WMI_SET_AKMP_PARAMS_CMD *akmpParams);
A_STATUS wmi_get_pmkid_list_cmd(struct wmi_t *wmip);
A_STATUS wmi_set_pmkid_list_cmd(struct wmi_t *wmip,
                                WMI_SET_PMKID_LIST_CMD *pmkInfo);
A_STATUS wmi_set_txPwr_cmd(struct wmi_t *wmip, A_UINT8 dbM);
A_STATUS wmi_get_txPwr_cmd(struct wmi_t *wmip);
A_STATUS wmi_switch_radio(struct wmi_t *wmip, A_UINT8 on);
A_STATUS wmi_addBadAp_cmd(struct wmi_t *wmip, A_UINT8 apIndex, A_UINT8 *bssid);
A_STATUS wmi_deleteBadAp_cmd(struct wmi_t *wmip, A_UINT8 apIndex);
A_STATUS wmi_set_tkip_countermeasures_cmd(struct wmi_t *wmip, A_BOOL en);
A_STATUS wmi_setPmkid_cmd(struct wmi_t *wmip, A_UINT8 *bssid, A_UINT8 *pmkId,
                          A_BOOL set);
A_STATUS wmi_set_access_params_cmd(struct wmi_t *wmip, A_UINT16 txop,
                                   A_UINT8 eCWmin, A_UINT8 eCWmax,
                                   A_UINT8 aifsn);
A_STATUS wmi_set_retry_limits_cmd(struct wmi_t *wmip, A_UINT8 frameType,
                                  A_UINT8 trafficClass, A_UINT8 maxRetries,
                                  A_UINT8 enableNotify);

void wmi_get_current_bssid(struct wmi_t *wmip, A_UINT8 *bssid);

A_STATUS wmi_get_roam_tbl_cmd(struct wmi_t *wmip);
A_STATUS wmi_get_roam_data_cmd(struct wmi_t *wmip, A_UINT8 roamDataType);
A_STATUS wmi_set_roam_ctrl_cmd(struct wmi_t *wmip, WMI_SET_ROAM_CTRL_CMD *p,
                               A_UINT8 size);
A_STATUS wmi_set_powersave_timers_cmd(struct wmi_t *wmip,
                            WMI_POWERSAVE_TIMERS_POLICY_CMD *pCmd,
                            A_UINT8 size);

A_STATUS wmi_set_opt_mode_cmd(struct wmi_t *wmip, A_UINT8 optMode);
A_STATUS wmi_opt_tx_frame_cmd(struct wmi_t *wmip,
                              A_UINT8 frmType,
                              A_UINT8 *dstMacAddr,
                              A_UINT8 *bssid,
                              A_UINT16 optIEDataLen,
                              A_UINT8 *optIEData);

A_STATUS wmi_set_adhoc_bconIntvl_cmd(struct wmi_t *wmip, A_UINT16 intvl);
A_STATUS wmi_set_voice_pkt_size_cmd(struct wmi_t *wmip, A_UINT16 voicePktSize);
A_STATUS wmi_set_max_sp_len_cmd(struct wmi_t *wmip, A_UINT8 maxSpLen);
A_UINT8  convert_userPriority_to_trafficClass(A_UINT8 userPriority);
A_UINT8 wmi_get_power_mode_cmd(struct wmi_t *wmip);
A_STATUS wmi_verify_tspec_params(WMI_CREATE_PSTREAM_CMD *pCmd, A_BOOL tspecCompliance);

#ifdef CONFIG_HOST_TCMD_SUPPORT
A_STATUS wmi_test_cmd(struct wmi_t *wmip, A_UINT8 *buf, A_UINT32  len);
#endif

A_STATUS wmi_set_bt_status_cmd(struct wmi_t *wmip, A_UINT8 streamType, A_UINT8 status);
A_STATUS wmi_set_bt_params_cmd(struct wmi_t *wmip, WMI_SET_BT_PARAMS_CMD* cmd);


/*
 *  This function is used to configure the fix rates mask to the target.
 */
A_STATUS wmi_set_fixrates_cmd(struct wmi_t *wmip, A_INT16 fixRatesMask);
A_STATUS wmi_get_ratemask_cmd(struct wmi_t *wmip);

A_STATUS wmi_set_authmode_cmd(struct wmi_t *wmip, A_UINT8 mode);

A_STATUS wmi_set_reassocmode_cmd(struct wmi_t *wmip, A_UINT8 mode);

A_STATUS wmi_set_wmm_cmd(struct wmi_t *wmip, WMI_WMM_STATUS status);
A_STATUS wmi_set_wmm_txop(struct wmi_t *wmip, WMI_TXOP_CFG txEnable);

A_STATUS wmi_get_keepalive_configured(struct wmi_t *wmip);
A_UINT8 wmi_get_keepalive_cmd(struct wmi_t *wmip);
A_STATUS wmi_set_keepalive_cmd(struct wmi_t *wmip, A_UINT8 keepaliveInterval);

A_STATUS wmi_set_appie_cmd(struct wmi_t *wmip, A_UINT8 mgmtFrmType,
                           A_UINT8 ieLen,A_UINT8 *ieInfo);

A_STATUS wmi_set_halparam_cmd(struct wmi_t *wmip, A_UINT8 *cmd, A_UINT16 dataLen);
A_INT32 wmi_get_rate(A_INT8 rateindex);

/*Wake on Wireless WMI commands*/
A_STATUS wmi_set_host_sleep_mode_cmd(struct wmi_t *wmip, WMI_SET_HOST_SLEEP_MODE_CMD *cmd);
A_STATUS wmi_set_wow_mode_cmd(struct wmi_t *wmip, WMI_SET_WOW_MODE_CMD *cmd);
A_STATUS wmi_get_wow_list_cmd(struct wmi_t *wmip, WMI_GET_WOW_LIST_CMD *cmd);
A_STATUS wmi_add_wow_pattern_cmd(struct wmi_t *wmip,
                                 WMI_ADD_WOW_PATTERN_CMD *cmd, A_UINT8* pattern, A_UINT8* mask, A_UINT8 pattern_size);
A_STATUS wmi_del_wow_pattern_cmd(struct wmi_t *wmip,
                                 WMI_DEL_WOW_PATTERN_CMD *cmd);
A_STATUS wmi_set_wsc_status_cmd(struct wmi_t *wmip, A_UINT32 status);

bss_t *
wmi_find_Ssidnode (struct wmi_t *wmip, A_UCHAR *pSsid,
				   A_UINT32 ssidLength, A_BOOL bIsWPA2);

void
wmi_node_return (struct wmi_t *wmip, bss_t *bss);
#ifdef __cplusplus
}
#endif

#endif /* _WMI_API_H_ */
