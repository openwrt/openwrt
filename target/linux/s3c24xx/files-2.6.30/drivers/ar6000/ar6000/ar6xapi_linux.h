#ifndef _AR6XAPI_LINUX_H
#define _AR6XAPI_LINUX_H
/*
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
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

struct ar6_softc;

void ar6000_ready_event(void *devt, A_UINT8 *datap, A_UINT8 phyCap);
A_UINT8 ar6000_iptos_to_userPriority(A_UINT8 *pkt);
A_STATUS ar6000_control_tx(void *devt, void *osbuf, WMI_PRI_STREAM_ID streamID);
void ar6000_connect_event(struct ar6_softc *ar, A_UINT16 channel,
                          A_UINT8 *bssid, A_UINT16 listenInterval,
                          A_UINT16 beaconInterval, NETWORK_TYPE networkType,
                          A_UINT8 beaconIeLen, A_UINT8 assocReqLen,
                          A_UINT8 assocRespLen,A_UINT8 *assocInfo);
void ar6000_disconnect_event(struct ar6_softc *ar, A_UINT8 reason,
                             A_UINT8 *bssid, A_UINT8 assocRespLen,
                             A_UINT8 *assocInfo, A_UINT16 protocolReasonStatus);
void ar6000_tkip_micerr_event(struct ar6_softc *ar, A_UINT8 keyid,
                              A_BOOL ismcast);
void ar6000_bitrate_rx(void *devt, A_INT32 rateKbps);
void ar6000_channelList_rx(void *devt, A_INT8 numChan, A_UINT16 *chanList);
void ar6000_regDomain_event(struct ar6_softc *ar, A_UINT32 regCode);
void ar6000_txPwr_rx(void *devt, A_UINT8 txPwr);
void ar6000_keepalive_rx(void *devt, A_UINT8 configured);
void ar6000_neighborReport_event(struct ar6_softc *ar, int numAps,
                                 WMI_NEIGHBOR_INFO *info);
void ar6000_set_numdataendpts(struct ar6_softc *ar, A_UINT32 num);
void ar6000_scanComplete_event(struct ar6_softc *ar, A_STATUS status);
void ar6000_targetStats_event(struct ar6_softc *ar,  WMI_TARGET_STATS *pStats);
void ar6000_rssiThreshold_event(struct ar6_softc *ar,
                                WMI_RSSI_THRESHOLD_VAL newThreshold,
                                A_INT16 rssi);
void ar6000_reportError_event(struct ar6_softc *, WMI_TARGET_ERROR_VAL errorVal);
void ar6000_cac_event(struct ar6_softc *ar, A_UINT8 ac, A_UINT8 cac_indication,
                                A_UINT8 statusCode, A_UINT8 *tspecSuggestion);
void ar6000_hbChallengeResp_event(struct ar6_softc *, A_UINT32 cookie, A_UINT32 source);
void
ar6000_roam_tbl_event(struct ar6_softc *ar, WMI_TARGET_ROAM_TBL *pTbl);

void
ar6000_roam_data_event(struct ar6_softc *ar, WMI_TARGET_ROAM_DATA *p);

void
ar6000_wow_list_event(struct ar6_softc *ar, A_UINT8 num_filters,
                      WMI_GET_WOW_LIST_REPLY *wow_reply);

void ar6000_pmkid_list_event(void *devt, A_UINT8 numPMKID,
                             WMI_PMKID *pmkidList);

void ar6000_gpio_intr_rx(A_UINT32 intr_mask, A_UINT32 input_values);
void ar6000_gpio_data_rx(A_UINT32 reg_id, A_UINT32 value);
void ar6000_gpio_ack_rx(void);

void ar6000_dbglog_init_done(struct ar6_softc *ar);

#ifdef SEND_EVENT_TO_APP
void ar6000_send_event_to_app(struct ar6_softc *ar, A_UINT16 eventId, A_UINT8 *datap, int len);
#endif

#ifdef CONFIG_HOST_TCMD_SUPPORT
void ar6000_tcmd_rx_report_event(void *devt, A_UINT8 * results, int len);
#endif

void ar6000_tx_retry_err_event(void *devt);

void ar6000_snrThresholdEvent_rx(void *devt,
                                 WMI_SNR_THRESHOLD_VAL newThreshold,
                                 A_UINT8 snr);

void ar6000_lqThresholdEvent_rx(void *devt, WMI_LQ_THRESHOLD_VAL range, A_UINT8 lqVal);


void ar6000_ratemask_rx(void *devt, A_UINT16 ratemask);

A_STATUS ar6000_get_driver_cfg(struct net_device *dev,
                                A_UINT16 cfgParam,
                                void *result);
void ar6000_bssInfo_event_rx(struct ar6_softc *ar, A_UINT8 *data, int len);

void ar6000_dbglog_event(struct ar6_softc *ar, A_UINT32 dropped,
                         A_INT8 *buffer, A_UINT32 length);

int ar6000_dbglog_get_debug_logs(struct ar6_softc *ar);

void ar6000_indicate_tx_activity(void *devt, A_UINT8 trafficClass, A_BOOL Active);

void ar6000_dset_open_req(void *devt,
                          A_UINT32 id,
                          A_UINT32 targ_handle,
                          A_UINT32 targ_reply_fn,
                          A_UINT32 targ_reply_arg);
void ar6000_dset_close(void *devt, A_UINT32 access_cookie);
void ar6000_dset_data_req(void *devt,
                          A_UINT32 access_cookie,
                          A_UINT32 offset,
                          A_UINT32 length,
                          A_UINT32 targ_buf,
                          A_UINT32 targ_reply_fn,
                          A_UINT32 targ_reply_arg);



#ifdef __cplusplus
}
#endif

#endif
