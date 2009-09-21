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

#include "ar6000_drv.h"

static A_UINT8 bcast_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static void ar6000_set_quality(struct iw_quality *iq, A_INT8 rssi);
extern unsigned int wmitimeout;
extern A_WAITQUEUE_HEAD arEvent;
extern wait_queue_head_t ar6000_scan_queue;

/*
 * Encode a WPA or RSN information element as a custom
 * element using the hostap format.
 */
static u_int
encode_ie(void *buf, size_t bufsize,
    const u_int8_t *ie, size_t ielen,
    const char *leader, size_t leader_len)
{
    u_int8_t *p;
    int i;

    if (bufsize < leader_len)
        return 0;
    p = buf;
    memcpy(p, leader, leader_len);
    bufsize -= leader_len;
    p += leader_len;
    for (i = 0; i < ielen && bufsize > 2; i++)
        p += sprintf(p, "%02x", ie[i]);
    return (i == ielen ? p - (u_int8_t *)buf : 0);
}

void
ar6000_scan_node(void *arg, bss_t *ni)
{
    struct iw_event iwe;
#if WIRELESS_EXT > 14
    char buf[64*2 + 30];
#endif
    struct ar_giwscan_param *param;
    A_CHAR *current_ev;
    A_CHAR *end_buf;
    struct ieee80211_common_ie  *cie;
	struct iw_request_info info;

	info.cmd = 0;
	info.flags = 0;

    param = (struct ar_giwscan_param *)arg;

    if (param->current_ev >= param->end_buf) {
        return;
    }
    if ((param->firstPass == TRUE) &&
        ((ni->ni_cie.ie_wpa == NULL) && (ni->ni_cie.ie_rsn == NULL))) {
        /*
         * Only forward wpa bss's in first pass
         */
        return;
    }

     if ((param->firstPass == FALSE) &&
        ((ni->ni_cie.ie_wpa != NULL) || (ni->ni_cie.ie_rsn != NULL))) {
        /*
         * Only forward non-wpa bss's in 2nd pass
         */
        return;
    }

    current_ev = param->current_ev;
    end_buf = param->end_buf;

    cie = &ni->ni_cie;

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = SIOCGIWAP;
    iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
    A_MEMCPY(iwe.u.ap_addr.sa_data, ni->ni_macaddr, 6);
    current_ev = iwe_stream_add_event(&info, current_ev, end_buf, &iwe,
                                      IW_EV_ADDR_LEN);

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = SIOCGIWESSID;
    iwe.u.data.flags = 1;
    iwe.u.data.length = cie->ie_ssid[1];
    current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe,
                                      &cie->ie_ssid[2]);

    if (cie->ie_capInfo & (IEEE80211_CAPINFO_ESS|IEEE80211_CAPINFO_IBSS)) {
        A_MEMZERO(&iwe, sizeof(iwe));
        iwe.cmd = SIOCGIWMODE;
        iwe.u.mode = cie->ie_capInfo & IEEE80211_CAPINFO_ESS ?
                IW_MODE_MASTER : IW_MODE_ADHOC;
        current_ev = iwe_stream_add_event(&info, current_ev, end_buf, &iwe,
                                          IW_EV_UINT_LEN);
    }

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = SIOCGIWFREQ;
    iwe.u.freq.m = cie->ie_chan * 100000;
    iwe.u.freq.e = 1;
    current_ev = iwe_stream_add_event(&info, current_ev, end_buf, &iwe,
                                      IW_EV_FREQ_LEN);

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = IWEVQUAL;
    ar6000_set_quality(&iwe.u.qual, ni->ni_snr);
    current_ev = iwe_stream_add_event(&info, current_ev, end_buf, &iwe,
                                      IW_EV_QUAL_LEN);

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = SIOCGIWENCODE;
    if (cie->ie_capInfo & IEEE80211_CAPINFO_PRIVACY) {
        iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
    } else {
        iwe.u.data.flags = IW_ENCODE_DISABLED;
    }
    iwe.u.data.length = 0;
    current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe, "");

    A_MEMZERO(&iwe, sizeof(iwe));
    iwe.cmd = IWEVCUSTOM;
    snprintf(buf, sizeof(buf), "bcn_int=%d", cie->ie_beaconInt);
    iwe.u.data.length = strlen(buf);
    current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe, buf);

    if (cie->ie_wpa != NULL) {
        static const char wpa_leader[] = "wpa_ie=";

        A_MEMZERO(&iwe, sizeof(iwe));
        iwe.cmd = IWEVCUSTOM;
        iwe.u.data.length = encode_ie(buf, sizeof(buf), cie->ie_wpa,
                                      cie->ie_wpa[1]+2,
                                      wpa_leader, sizeof(wpa_leader)-1);

        if (iwe.u.data.length != 0) {
            current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe,
									   buf);
        }
    }

    if (cie->ie_rsn != NULL && cie->ie_rsn[0] == IEEE80211_ELEMID_RSN) {
        static const char rsn_leader[] = "rsn_ie=";

        A_MEMZERO(&iwe, sizeof(iwe));
        iwe.cmd = IWEVCUSTOM;
        iwe.u.data.length = encode_ie(buf, sizeof(buf), cie->ie_rsn,
                                      cie->ie_rsn[1]+2,
                                      rsn_leader, sizeof(rsn_leader)-1);

        if (iwe.u.data.length != 0) {
            current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe,
									   buf);
        }
    }

    if (cie->ie_wmm != NULL) {
        static const char wmm_leader[] = "wmm_ie=";

        A_MEMZERO(&iwe, sizeof(iwe));
        iwe.cmd = IWEVCUSTOM;
        iwe.u.data.length = encode_ie(buf, sizeof(buf), cie->ie_wmm,
                                      cie->ie_wmm[1]+2,
                                      wmm_leader, sizeof(wmm_leader)-1);
        if (iwe.u.data.length != 0) {
            current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe,
									   buf);
        }
    }

    if (cie->ie_ath != NULL) {
        static const char ath_leader[] = "ath_ie=";

        A_MEMZERO(&iwe, sizeof(iwe));
        iwe.cmd = IWEVCUSTOM;
        iwe.u.data.length = encode_ie(buf, sizeof(buf), cie->ie_ath,
                                      cie->ie_ath[1]+2,
                                      ath_leader, sizeof(ath_leader)-1);
        if (iwe.u.data.length != 0) {
            current_ev = iwe_stream_add_point(&info, current_ev, end_buf, &iwe,
									   buf);
        }
    }

    param->current_ev = current_ev;
}

int
ar6000_ioctl_giwscan(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_point *data, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ar_giwscan_param param;
    int i;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    param.current_ev = extra;
    param.end_buf = extra + IW_SCAN_MAX_DATA;
    param.firstPass = TRUE;

    /*
     * Do two passes to insure WPA scan candidates
     * are sorted to the front.  This is a hack to deal with
     * the wireless extensions capping scan results at
     * IW_SCAN_MAX_DATA bytes.  In densely populated environments
     * it's easy to overflow this buffer (especially with WPA/RSN
     * information elements).  Note this sorting hack does not
     * guarantee we won't overflow anyway.
     */
    for (i = 0; i < 2; i++) {
        /*
         * Translate data to WE format.
         */
        wmi_iterate_nodes(ar->arWmi, ar6000_scan_node, &param);
        param.firstPass = FALSE;
        if (param.current_ev >= param.end_buf) {
            data->length = param.current_ev - extra;
            return -E2BIG;
        }
    }

    data->length = param.current_ev - extra;
    return 0;
}

extern int reconnect_flag;
/* SIOCSIWESSID */
static int
ar6000_ioctl_siwessid(struct net_device *dev,
                     struct iw_request_info *info,
                     struct iw_point *data, char *ssid)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_STATUS status;
    A_UINT8     arNetworkType;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    /*
     * iwconfig passes a string with length excluding any trailing NUL.
     * FIXME: we should be able to set an ESSID of 32 bytes, yet things fall
     * over badly if we do. So we limit the ESSID to 31 bytes.
     */
    if (data->flags && (!data->length || data->length >= sizeof(ar->arSsid))) {
        /*
         * ssid is invalid
         */
        return -EINVAL;
    }
    /* Added for bug 25178, return an IOCTL error instead of target returning
       Illegal parameter error when either the BSSID or channel is missing
       and we cannot scan during connect.
     */
    if (data->flags) {
        if (ar->arSkipScan == TRUE &&
            (ar->arChannelHint == 0 ||
             (!ar->arReqBssid[0] && !ar->arReqBssid[1] && !ar->arReqBssid[2] &&
              !ar->arReqBssid[3] && !ar->arReqBssid[4] && !ar->arReqBssid[5])))
        {
            return -EINVAL;
        }
    }

    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }

    if (ar->arTxPending[WMI_CONTROL_PRI]) {
        /*
         * sleep until the command queue drains
         */
        wait_event_interruptible_timeout(arEvent,
            ar->arTxPending[WMI_CONTROL_PRI] == 0, wmitimeout * HZ);
        if (signal_pending(current)) {
            return -EINTR;
        }
    }

    if (!data->flags) {
        arNetworkType = ar->arNetworkType;
        ar6000_init_profile_info(ar);
        ar->arNetworkType = arNetworkType;
    }

    /*
     * The original logic here prevented a disconnect if issuing an "essid off"
     * if no ESSID was set, presumably to prevent sending multiple disconnects
     * to the WMI.
     *
     * Unfortunately, this also meant that no disconnect was sent when we were
     * already connected, but the profile has been changed since (which also
     * clears the ESSID as a reminder that the WMI needs updating.)
     *
     * The "1 ||" makes sure we always disconnect or reconnect. The WMI doesn't
     * seem to mind being sent multiple disconnects.
     */
    if (1 || (ar->arSsidLen) || (!data->flags))
    {
        if ((!data->flags) ||
            (A_MEMCMP(ar->arSsid, ssid, ar->arSsidLen) != 0) ||
            (ar->arSsidLen != (data->length)))
        {
            /*
             * SSID set previously or essid off has been issued.
             *
             * Disconnect Command is issued in two cases after wmi is ready
             * (1) ssid is different from the previous setting
             * (2) essid off has been issued
             *
             */
            if (ar->arWmiReady == TRUE) {
                reconnect_flag = 0;
                status = wmi_disconnect_cmd(ar->arWmi);
                A_MEMZERO(ar->arSsid, sizeof(ar->arSsid));
                ar->arSsidLen = 0;
                if (ar->arSkipScan == FALSE) {
                    A_MEMZERO(ar->arReqBssid, sizeof(ar->arReqBssid));
                }
                if (!data->flags) {
                    up(&ar->arSem);
                    return 0;
                }
            } else {
                 up(&ar->arSem);
            }
        }
        else
        {
            /*
             * SSID is same, so we assume profile hasn't changed.
             * If the interface is up and wmi is ready, we issue
             * a reconnect cmd. Issue a reconnect only we are already
             * connected.
             */
            if((ar->arConnected == TRUE) && (ar->arWmiReady == TRUE))
            {
                reconnect_flag = TRUE;
                status = wmi_reconnect_cmd(ar->arWmi,ar->arReqBssid,
                                           ar->arChannelHint);
                up(&ar->arSem);
                if (status != A_OK) {
                    return -EIO;
                }
                return 0;
            }
            else{
                /*
                 * Dont return if connect is pending.
                 */
                if(!(ar->arConnectPending)) {
                    up(&ar->arSem);
                    return 0;
                }
            }
        }
    }

    ar->arSsidLen = data->length;
    A_MEMCPY(ar->arSsid, ssid, ar->arSsidLen);

    /* The ssid length check prevents second "essid off" from the user,
       to be treated as a connect cmd. The second "essid off" is ignored.
    */
    if((ar->arWmiReady == TRUE) && (ar->arSsidLen > 0) )
    {
        AR6000_SPIN_LOCK(&ar->arLock, 0);
        if (SHARED_AUTH == ar->arDot11AuthMode) {
            ar6000_install_static_wep_keys(ar);
        }
        AR_DEBUG_PRINTF("Connect called with authmode %d dot11 auth %d"\
                        " PW crypto %d PW crypto Len %d GRP crypto %d"\
                        " GRP crypto Len %d\n",
                        ar->arAuthMode, ar->arDot11AuthMode,
                        ar->arPairwiseCrypto, ar->arPairwiseCryptoLen,
                        ar->arGroupCrypto, ar->arGroupCryptoLen);
        reconnect_flag = 0;
        AR6000_SPIN_UNLOCK(&ar->arLock, 0);
        status = wmi_connect_cmd(ar->arWmi, ar->arNetworkType,
                                 ar->arDot11AuthMode, ar->arAuthMode,
                                 ar->arPairwiseCrypto, ar->arPairwiseCryptoLen,
                                 ar->arGroupCrypto,ar->arGroupCryptoLen,
                                 ar->arSsidLen, ar->arSsid,
                                 ar->arReqBssid, ar->arChannelHint,
                                 ar->arConnectCtrlFlags);


        up(&ar->arSem);

        if (status != A_OK) {
            return -EIO;
        }
        ar->arConnectPending = TRUE;
    }else{
      up(&ar->arSem);
    }
    return 0;
}

/* SIOCGIWESSID */
static int
ar6000_ioctl_giwessid(struct net_device *dev,
                     struct iw_request_info *info,
                     struct iw_point *data, char *essid)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    data->flags = 1;
    data->length = ar->arSsidLen;
    A_MEMCPY(essid, ar->arSsid, ar->arSsidLen);

    return 0;
}


void ar6000_install_static_wep_keys(AR_SOFTC_T *ar)
{
    A_UINT8 index;
    A_UINT8 keyUsage;

    for (index = WMI_MIN_KEY_INDEX; index <= WMI_MAX_KEY_INDEX; index++) {
        if (ar->arWepKeyList[index].arKeyLen) {
            keyUsage = GROUP_USAGE;
            if (index == ar->arDefTxKeyIndex) {
                keyUsage |= TX_USAGE;
            }
            wmi_addKey_cmd(ar->arWmi,
                           index,
                           WEP_CRYPT,
                           keyUsage,
                           ar->arWepKeyList[index].arKeyLen,
                           NULL,
                           ar->arWepKeyList[index].arKey, KEY_OP_INIT_VAL,
                           NO_SYNC_WMIFLAG);
        }
    }
}

int
ar6000_ioctl_delkey(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    return 0;
}

int
ar6000_ioctl_setmlme(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ieee80211req_mlme *mlme = (struct ieee80211req_mlme *)extra;

    if ((ar->arWmiReady == FALSE) || (ar->arConnected != TRUE))
		return -EIO;

    switch (mlme->im_op) {
        case IEEE80211_MLME_DISASSOC:
        case IEEE80211_MLME_DEAUTH:
            /* Not Supported */
            break;
        default:
            break;
    }
    return 0;
}


int
ar6000_ioctl_setwmmparams(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    return -EIO;            /* for now */
}

int
ar6000_ioctl_getwmmparams(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    return -EIO;            /* for now */
}

int ar6000_ioctl_setoptie(struct net_device *dev, struct iw_request_info *info,
			  struct iw_point *data, char *extra)
{
	/* The target generates the WPA/RSN IE */
	return 0;
}

int
ar6000_ioctl_setauthalg(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ieee80211req_authalg *req = (struct ieee80211req_authalg *)extra;
    int ret = 0;


    AR6000_SPIN_LOCK(&ar->arLock, 0);

    if (req->auth_alg == AUTH_ALG_OPEN_SYSTEM) {
        ar->arDot11AuthMode  = OPEN_AUTH;
    } else if (req->auth_alg == AUTH_ALG_LEAP) {
        ar->arDot11AuthMode   = LEAP_AUTH;
        ar->arPairwiseCrypto  = WEP_CRYPT;
        ar->arGroupCrypto     = WEP_CRYPT;
    } else {
        ret = -EIO;
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    return ret;
}
static int
ar6000_ioctl_addpmkid(struct net_device *dev, struct iw_request_info *info,
             void *w, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ieee80211req_addpmkid  *req = (struct ieee80211req_addpmkid *)extra;
    A_STATUS status;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    AR_DEBUG_PRINTF("Add pmkid for %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x en=%d\n",
                    req->pi_bssid[0], req->pi_bssid[1], req->pi_bssid[2],
                    req->pi_bssid[3], req->pi_bssid[4], req->pi_bssid[5],
                    req->pi_enable);

    status = wmi_setPmkid_cmd(ar->arWmi, req->pi_bssid, req->pi_pmkid,
                              req->pi_enable);

    if (status != A_OK) {
        return -EIO;
    }

    return 0;
}

/*
 * SIOCSIWRATE
 */
int
ar6000_ioctl_siwrate(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_UINT32  kbps;

    if (rrq->fixed) {
        kbps = rrq->value / 1000;           /* rrq->value is in bps */
    } else {
        kbps = -1;                          /* -1 indicates auto rate */
    }
    if(kbps != -1 && wmi_validate_bitrate(ar->arWmi, kbps) == A_EINVAL)
    {
        AR_DEBUG_PRINTF("BitRate is not Valid %d\n", kbps);
        return -EINVAL;
    }
    ar->arBitRate = kbps;
    if(ar->arWmiReady == TRUE)
    {
        if (wmi_set_bitrate_cmd(ar->arWmi, kbps) != A_OK) {
            return -EINVAL;
        }
    }
    return 0;
}

/*
 * SIOCGIWRATE
 */
int
ar6000_ioctl_giwrate(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    int ret = 0;

    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }
    if(ar->arWmiReady == TRUE)
    {
        ar->arBitRate = 0xFFFF;
        if (wmi_get_bitrate_cmd(ar->arWmi) != A_OK) {
            up(&ar->arSem);
            return -EIO;
        }
        wait_event_interruptible_timeout(arEvent, ar->arBitRate != 0xFFFF, wmitimeout * HZ);
        if (signal_pending(current)) {
            ret = -EINTR;
        }
    }
    /* If the interface is down or wmi is not ready or the target is not
       connected - return the value stored in the device structure */
    if (!ret) {
        if (ar->arBitRate == -1) {
            rrq->fixed = TRUE;
            rrq->value = 0;
        } else {
            rrq->value = ar->arBitRate * 1000;
        }
    }

    up(&ar->arSem);

    return ret;
}

/*
 * SIOCSIWTXPOW
 */
static int
ar6000_ioctl_siwtxpow(struct net_device *dev,
             struct iw_request_info *info,
             struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_UINT8 dbM;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arRadioSwitch == WLAN_ENABLED
	&& rrq->disabled) {
	    if (wmi_switch_radio(ar->arWmi, WLAN_DISABLED) < 0)
		    return -EIO;
	    ar->arRadioSwitch = WLAN_DISABLED;
    } else if (ar->arRadioSwitch == WLAN_DISABLED
	       && !rrq->disabled) {
	    if (wmi_switch_radio(ar->arWmi, WLAN_ENABLED) < 0)
		    return -EIO;
	    ar->arRadioSwitch = WLAN_ENABLED;
    }

    if (rrq->fixed) {
        if (rrq->flags != IW_TXPOW_DBM) {
            return -EOPNOTSUPP;
        }
        ar->arTxPwr= dbM = rrq->value;
        ar->arTxPwrSet = TRUE;
    } else {
        ar->arTxPwr = dbM = 0;
        ar->arTxPwrSet = FALSE;
    }
    if(ar->arWmiReady == TRUE)
    {
        AR_DEBUG_PRINTF("Set tx pwr cmd %d dbM\n", dbM);
        wmi_set_txPwr_cmd(ar->arWmi, dbM);
    }
    return 0;
}

/*
 * SIOCGIWTXPOW
 */
int
ar6000_ioctl_giwtxpow(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    int ret = 0;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arRadioSwitch == WLAN_DISABLED) {
	    rrq->disabled = 1;
	    return 0;
    }

    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }
    if((ar->arWmiReady == TRUE) && (ar->arConnected == TRUE))
    {
        ar->arTxPwr = 0;

        if (wmi_get_txPwr_cmd(ar->arWmi) != A_OK) {
            up(&ar->arSem);
            return -EIO;
        }

        wait_event_interruptible_timeout(arEvent, ar->arTxPwr != 0, wmitimeout * HZ);

        if (signal_pending(current)) {
            ret = -EINTR;
         }
    }
   /* If the interace is down or wmi is not ready or target is not connected
      then return value stored in the device structure */

    if (!ret) {
         if (ar->arTxPwrSet == TRUE) {
            rrq->fixed = TRUE;
        }
        rrq->value = ar->arTxPwr;
        rrq->flags = IW_TXPOW_DBM;
    }

    up(&ar->arSem);

    return ret;
}

/*
 * SIOCSIWRETRY
 * since iwconfig only provides us with one max retry value, we use it
 * to apply to data frames of the BE traffic class.
 */
static int
ar6000_ioctl_siwretry(struct net_device *dev,
             struct iw_request_info *info,
             struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (rrq->disabled) {
        return -EOPNOTSUPP;
    }

    if ((rrq->flags & IW_RETRY_TYPE) != IW_RETRY_LIMIT) {
        return -EOPNOTSUPP;
    }

    if ( !(rrq->value >= WMI_MIN_RETRIES) || !(rrq->value <= WMI_MAX_RETRIES)) {
            return - EINVAL;
    }
    if(ar->arWmiReady == TRUE)
    {
        if (wmi_set_retry_limits_cmd(ar->arWmi, DATA_FRAMETYPE, WMM_AC_BE,
                                     rrq->value, 0) != A_OK){
            return -EINVAL;
        }
    }
    ar->arMaxRetries = rrq->value;
    return 0;
}

/*
 * SIOCGIWRETRY
 */
static int
ar6000_ioctl_giwretry(struct net_device *dev,
             struct iw_request_info *info,
             struct iw_param *rrq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    rrq->disabled = 0;
    switch (rrq->flags & IW_RETRY_TYPE) {
    case IW_RETRY_LIFETIME:
        return -EOPNOTSUPP;
        break;
    case IW_RETRY_LIMIT:
        rrq->flags = IW_RETRY_LIMIT;
        switch (rrq->flags & IW_RETRY_MODIFIER) {
        case IW_RETRY_MIN:
            rrq->flags |= IW_RETRY_MIN;
            rrq->value = WMI_MIN_RETRIES;
            break;
        case IW_RETRY_MAX:
            rrq->flags |= IW_RETRY_MAX;
            rrq->value = ar->arMaxRetries;
            break;
        }
        break;
    }
    return 0;
}

/*
 * SIOCSIWENCODE
 */
static int
ar6000_ioctl_siwencode(struct net_device *dev,
              struct iw_request_info *info,
              struct iw_point *erq, char *keybuf)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    int index;
    A_INT32 auth = ar->arDot11AuthMode;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    index = erq->flags & IW_ENCODE_INDEX;

    if (index && (((index - 1) < WMI_MIN_KEY_INDEX) ||
                  ((index - 1) > WMI_MAX_KEY_INDEX)))
    {
        return -EIO;
    }

    if (erq->flags & IW_ENCODE_DISABLED) {
        /*
         * Encryption disabled
         */
        if (index) {
            /*
             * If key index was specified then clear the specified key
             */
            index--;
            A_MEMZERO(ar->arWepKeyList[index].arKey,
                      sizeof(ar->arWepKeyList[index].arKey));
            ar->arWepKeyList[index].arKeyLen = 0;
        }
        ar->arDot11AuthMode       = OPEN_AUTH;
        ar->arPairwiseCrypto      = NONE_CRYPT;
        ar->arGroupCrypto         = NONE_CRYPT;
        ar->arAuthMode            = NONE_AUTH;
    } else {
        /*
         * Enabling WEP encryption
         */
        if (index) {
            index--;                /* keyindex is off base 1 in iwconfig */
        }

        if (erq->flags & IW_ENCODE_OPEN) {
            auth = OPEN_AUTH;
        } else if (erq->flags & IW_ENCODE_RESTRICTED) {
            auth = SHARED_AUTH;
        }

        if (erq->length) {
            if (!IEEE80211_IS_VALID_WEP_CIPHER_LEN(erq->length)) {
                return -EIO;
            }

            A_MEMZERO(ar->arWepKeyList[index].arKey,
                      sizeof(ar->arWepKeyList[index].arKey));
            A_MEMCPY(ar->arWepKeyList[index].arKey, keybuf, erq->length);
            ar->arWepKeyList[index].arKeyLen = erq->length;
        } else {
            if (ar->arWepKeyList[index].arKeyLen == 0) {
                return -EIO;
            }
            ar->arDefTxKeyIndex = index;
        }

        ar->arPairwiseCrypto      = WEP_CRYPT;
        ar->arGroupCrypto         = WEP_CRYPT;
        ar->arDot11AuthMode       = auth;
        ar->arAuthMode            = NONE_AUTH;
    }

    /*
     * profile has changed.  Erase ssid to signal change
     */
    A_MEMZERO(ar->arSsid, sizeof(ar->arSsid));
    ar->arSsidLen = 0;

    return 0;
}

static int
ar6000_ioctl_giwencode(struct net_device *dev,
              struct iw_request_info *info,
              struct iw_point *erq, char *key)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_UINT8 keyIndex;
    struct ar_wep_key *wk;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arPairwiseCrypto == NONE_CRYPT) {
        erq->length = 0;
        erq->flags = IW_ENCODE_DISABLED;
    } else {
        /* get the keyIndex */
        keyIndex = erq->flags & IW_ENCODE_INDEX;
        if (0 == keyIndex) {
            keyIndex = ar->arDefTxKeyIndex;
        } else if ((keyIndex - 1 < WMI_MIN_KEY_INDEX) ||
                   (keyIndex - 1 > WMI_MAX_KEY_INDEX))
        {
            keyIndex = WMI_MIN_KEY_INDEX;
        } else {
            keyIndex--;
        }
        erq->flags = keyIndex + 1;
        erq->flags |= IW_ENCODE_ENABLED;
        wk = &ar->arWepKeyList[keyIndex];
        if (erq->length > wk->arKeyLen) {
            erq->length = wk->arKeyLen;
        }
        if (wk->arKeyLen) {
            A_MEMCPY(key, wk->arKey, erq->length);
        }
        if (ar->arDot11AuthMode == OPEN_AUTH) {
            erq->flags |= IW_ENCODE_OPEN;
        } else if (ar->arDot11AuthMode == SHARED_AUTH) {
            erq->flags |= IW_ENCODE_RESTRICTED;
        }
    }

    return 0;
}

static int ar6000_ioctl_siwpower(struct net_device *dev,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu, char *extra)
{
	AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
	WMI_POWER_MODE power_mode;

	if (wrqu->power.disabled)
		power_mode = MAX_PERF_POWER;
	else
		power_mode = REC_POWER;

	if (wmi_powermode_cmd(ar->arWmi, power_mode) < 0)
		return -EIO;

	return 0;
}

static int ar6000_ioctl_giwpower(struct net_device *dev,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu, char *extra)
{
	AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

	/*
	 * FIXME:
	 * https://docs.openmoko.org/trac/ticket/2267
	 * When starting wpa_supplicant the kernel oopses.
	 * The following condition avoids the oops.
	 * Remove this comment to bless this solution.
	 */
	if (ar->arWlanState == WLAN_DISABLED || ar->arWmiReady == FALSE)
		return -EIO;

	return wmi_get_power_mode_cmd(ar->arWmi);
}

static int ar6000_ioctl_siwgenie(struct net_device *dev,
				 struct iw_request_info *info,
				 struct iw_point *dwrq,
				 char *extra)
{
	/* The target does that for us */
	return 0;
}

static int ar6000_ioctl_giwgenie(struct net_device *dev,
				 struct iw_request_info *info,
				 struct iw_point *dwrq,
				 char *extra)
{
	return 0;
}

static int ar6000_ioctl_siwauth(struct net_device *dev,
				struct iw_request_info *info,
				struct iw_param *param,
				char *extra)
{
	AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
	int reset = 0;

	switch (param->flags & IW_AUTH_INDEX) {
	case IW_AUTH_WPA_VERSION:
		if (param->value & IW_AUTH_WPA_VERSION_DISABLED) {
			ar->arAuthMode = NONE_AUTH;
		}
		if (param->value & IW_AUTH_WPA_VERSION_WPA) {
			ar->arAuthMode = WPA_AUTH;
		}
		if (param->value & IW_AUTH_WPA_VERSION_WPA2) {
			ar->arAuthMode = WPA2_AUTH;
		}

		reset = 1;
		break;
	case IW_AUTH_CIPHER_PAIRWISE:
		if (param->value & IW_AUTH_CIPHER_NONE) {
			ar->arPairwiseCrypto = NONE_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_WEP40) {
			ar->arPairwiseCrypto = WEP_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_TKIP) {
			ar->arPairwiseCrypto = TKIP_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_CCMP) {
			ar->arPairwiseCrypto = AES_CRYPT;
		}

		reset = 1;
		break;
	case IW_AUTH_CIPHER_GROUP:
		if (param->value & IW_AUTH_CIPHER_NONE) {
			ar->arGroupCrypto = NONE_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_WEP40) {
			ar->arGroupCrypto = WEP_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_TKIP) {
			ar->arGroupCrypto = TKIP_CRYPT;
		}
		if (param->value & IW_AUTH_CIPHER_CCMP) {
			ar->arGroupCrypto = AES_CRYPT;
		}

		reset = 1;
		break;
	case IW_AUTH_KEY_MGMT:
		if (param->value & IW_AUTH_KEY_MGMT_PSK) {
			if (ar->arAuthMode == WPA_AUTH) {
				ar->arAuthMode = WPA_PSK_AUTH;
			} else if (ar->arAuthMode == WPA2_AUTH) {
				ar->arAuthMode = WPA2_PSK_AUTH;
			}

			reset = 1;
		}
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		if (ar->arWmiReady == FALSE) {
			return -EIO;
		}
		wmi_set_tkip_countermeasures_cmd(ar->arWmi, param->value);
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		break;

	case IW_AUTH_80211_AUTH_ALG:
		if (param->value & IW_AUTH_ALG_OPEN_SYSTEM) {
			ar->arDot11AuthMode  = OPEN_AUTH;
		}
		if (param->value & IW_AUTH_ALG_SHARED_KEY) {
			ar->arDot11AuthMode  = SHARED_AUTH;
		}
		if (param->value & IW_AUTH_ALG_LEAP) {
			ar->arDot11AuthMode   = LEAP_AUTH;
			ar->arPairwiseCrypto  = WEP_CRYPT;
			ar->arGroupCrypto     = WEP_CRYPT;
		}

		reset = 1;
		break;

	case IW_AUTH_WPA_ENABLED:
		reset = 1;
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		break;

	default:
		printk("%s(): Unknown flag 0x%x\n", __FUNCTION__, param->flags);
		return -EOPNOTSUPP;
	}

	if (reset) {
		A_MEMZERO(ar->arSsid, sizeof(ar->arSsid));
		ar->arSsidLen = 0;
	}

	return 0;
}

static int ar6000_ioctl_giwauth(struct net_device *dev,
				struct iw_request_info *info,
				struct iw_param *dwrq,
				char *extra)
{
	return 0;
}

static int ar6000_ioctl_siwencodeext(struct net_device *dev,
				     struct iw_request_info *info,
				     union iwreq_data *wrqu,
				     char *extra)
{
	AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
	struct iw_point *encoding = &wrqu->encoding;
	struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
	int alg = ext->alg, idx;

	if (ar->arWlanState == WLAN_DISABLED) {
		return -EIO;
	}

	/* Determine and validate the key index */
	idx = (encoding->flags & IW_ENCODE_INDEX) - 1;
	if (idx) {
		if (idx < 0 || idx > 3)
			return -EINVAL;
	}

	if ((alg == IW_ENCODE_ALG_TKIP) || (alg == IW_ENCODE_ALG_CCMP)) {
		struct ieee80211req_key ik;
		KEY_USAGE key_usage;
		CRYPTO_TYPE key_type = NONE_CRYPT;
		int status;

		ar->user_saved_keys.keyOk = FALSE;

		if (alg == IW_ENCODE_ALG_TKIP) {
			key_type = TKIP_CRYPT;
			ik.ik_type = IEEE80211_CIPHER_TKIP;
		} else {
			key_type = AES_CRYPT;
			ik.ik_type = IEEE80211_CIPHER_AES_CCM;
		}

		ik.ik_keyix = idx;
		ik.ik_keylen = ext->key_len;
		ik.ik_flags = IEEE80211_KEY_RECV;
		if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			ik.ik_flags |= IEEE80211_KEY_XMIT
				| IEEE80211_KEY_DEFAULT;
		}

		if (ext->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
			memcpy(&ik.ik_keyrsc, ext->rx_seq, 8);
		}

		memcpy(ik.ik_keydata, ext->key, ext->key_len);

		ar->user_saved_keys.keyType = key_type;
		if (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) {
			key_usage = GROUP_USAGE;
			memset(ik.ik_macaddr, 0, ETH_ALEN);
			memcpy(&ar->user_saved_keys.bcast_ik, &ik,
			       sizeof(struct ieee80211req_key));
		} else {
			key_usage = PAIRWISE_USAGE;
			memcpy(ik.ik_macaddr, ext->addr.sa_data, ETH_ALEN);
			memcpy(&ar->user_saved_keys.ucast_ik, &ik,
			       sizeof(struct ieee80211req_key));
		}

		status = wmi_addKey_cmd(ar->arWmi, ik.ik_keyix, key_type,
					key_usage, ik.ik_keylen,
					(A_UINT8 *)&ik.ik_keyrsc,
					ik.ik_keydata,
					KEY_OP_INIT_VAL, SYNC_BEFORE_WMIFLAG);

		if (status < 0)
			return -EIO;

		ar->user_saved_keys.keyOk = TRUE;

		return 0;

	} else {
		/* WEP falls back to SIWENCODE */
		return -EOPNOTSUPP;
	}

	return 0;
}


static int ar6000_ioctl_giwencodeext(struct net_device *dev,
				     struct iw_request_info *info,
				     struct iw_point *dwrq,
				     char *extra)
{
	return 0;
}


static int
ar6000_ioctl_setparam(struct net_device *dev,
                      struct iw_request_info *info,
                      void *erq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    int *i = (int *)extra;
    int param = i[0];
    int value = i[1];
    int ret = 0;
    A_BOOL profChanged = FALSE;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    switch (param) {
        case IEEE80211_PARAM_WPA:
            switch (value) {
                case WPA_MODE_WPA1:
                    ar->arAuthMode = WPA_AUTH;
                    profChanged    = TRUE;
                    break;
                case WPA_MODE_WPA2:
                    ar->arAuthMode = WPA2_AUTH;
                    profChanged    = TRUE;
                    break;
                case WPA_MODE_NONE:
                    ar->arAuthMode = NONE_AUTH;
                    profChanged    = TRUE;
                    break;
	    default:
		    printk("IEEE80211_PARAM_WPA: Unknown value %d\n", value);
            }
            break;
        case IEEE80211_PARAM_AUTHMODE:
            switch(value) {
                case IEEE80211_AUTH_WPA_PSK:
                    if (WPA_AUTH == ar->arAuthMode) {
                        ar->arAuthMode = WPA_PSK_AUTH;
                        profChanged    = TRUE;
                    } else if (WPA2_AUTH == ar->arAuthMode) {
                        ar->arAuthMode = WPA2_PSK_AUTH;
                        profChanged    = TRUE;
                    } else {
                        AR_DEBUG_PRINTF("Error -  Setting PSK mode when WPA "\
                                        "param was set to %d\n",
                                        ar->arAuthMode);
                        ret = -1;
                    }
                    break;
                case IEEE80211_AUTH_WPA_CCKM:
                    if (WPA2_AUTH == ar->arAuthMode) {
                        ar->arAuthMode = WPA2_AUTH_CCKM;
                    } else {
                        ar->arAuthMode = WPA_AUTH_CCKM;
                    }
                    break;
                default:
                    break;
            }
            break;
        case IEEE80211_PARAM_UCASTCIPHER:
            switch (value) {
                case IEEE80211_CIPHER_AES_CCM:
                    ar->arPairwiseCrypto = AES_CRYPT;
                    profChanged          = TRUE;
                    break;
                case IEEE80211_CIPHER_TKIP:
                    ar->arPairwiseCrypto = TKIP_CRYPT;
                    profChanged          = TRUE;
                    break;
                case IEEE80211_CIPHER_WEP:
                    ar->arPairwiseCrypto = WEP_CRYPT;
                    profChanged          = TRUE;
                    break;
                case IEEE80211_CIPHER_NONE:
                    ar->arPairwiseCrypto = NONE_CRYPT;
                    profChanged          = TRUE;
                    break;
            }
            break;
        case IEEE80211_PARAM_UCASTKEYLEN:
            if (!IEEE80211_IS_VALID_WEP_CIPHER_LEN(value)) {
                ret = -EIO;
            } else {
                ar->arPairwiseCryptoLen = value;
            }
            break;
        case IEEE80211_PARAM_MCASTCIPHER:
            switch (value) {
                case IEEE80211_CIPHER_AES_CCM:
                    ar->arGroupCrypto = AES_CRYPT;
                    profChanged       = TRUE;
                    break;
                case IEEE80211_CIPHER_TKIP:
                    ar->arGroupCrypto = TKIP_CRYPT;
                    profChanged       = TRUE;
                    break;
                case IEEE80211_CIPHER_WEP:
                    ar->arGroupCrypto = WEP_CRYPT;
                    profChanged       = TRUE;
                    break;
                case IEEE80211_CIPHER_NONE:
                    ar->arGroupCrypto = NONE_CRYPT;
                    profChanged       = TRUE;
                    break;
            }
            break;
        case IEEE80211_PARAM_MCASTKEYLEN:
            if (!IEEE80211_IS_VALID_WEP_CIPHER_LEN(value)) {
                ret = -EIO;
            } else {
                ar->arGroupCryptoLen = value;
            }
            break;
        case IEEE80211_PARAM_COUNTERMEASURES:
            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            wmi_set_tkip_countermeasures_cmd(ar->arWmi, value);
            break;
        default:
            break;
    }

    if (profChanged == TRUE) {
        /*
         * profile has changed.  Erase ssid to signal change
         */
	A_MEMZERO(ar->arSsid, sizeof(ar->arSsid));
	ar->arSsidLen = 0;
    }

    return ret;
}

int
ar6000_ioctl_getparam(struct net_device *dev, struct iw_request_info *info,
            void *w, char *extra)
{
    return -EIO;            /* for now */
}

int
ar6000_ioctl_setkey(struct net_device *dev, struct iw_request_info *info,
		    void *w, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ieee80211req_key *ik = (struct ieee80211req_key *)extra;
    KEY_USAGE keyUsage;
    A_STATUS status;
    CRYPTO_TYPE keyType = NONE_CRYPT;

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    ar->user_saved_keys.keyOk = FALSE;

    if ( 0 == memcmp(ik->ik_macaddr, "\x00\x00\x00\x00\x00\x00",
                     IEEE80211_ADDR_LEN)) {
        keyUsage = GROUP_USAGE;
        A_MEMCPY(&ar->user_saved_keys.bcast_ik, ik,
                 sizeof(struct ieee80211req_key));
    } else {
        keyUsage = PAIRWISE_USAGE;
        A_MEMCPY(&ar->user_saved_keys.ucast_ik, ik,
                 sizeof(struct ieee80211req_key));
    }

    switch (ik->ik_type) {
        case IEEE80211_CIPHER_WEP:
            keyType = WEP_CRYPT;
            break;
        case IEEE80211_CIPHER_TKIP:
            keyType = TKIP_CRYPT;
            break;
        case IEEE80211_CIPHER_AES_CCM:
            keyType = AES_CRYPT;
            break;
        default:
            break;
    }
    ar->user_saved_keys.keyType = keyType;

    if (IEEE80211_CIPHER_CCKM_KRK != ik->ik_type) {
        if (NONE_CRYPT == keyType) {
            return -EIO;
        }

        status = wmi_addKey_cmd(ar->arWmi, ik->ik_keyix, keyType, keyUsage,
                                ik->ik_keylen, (A_UINT8 *)&ik->ik_keyrsc,
                                ik->ik_keydata, KEY_OP_INIT_VAL,
                                SYNC_BEFORE_WMIFLAG);

        if (status != A_OK) {
            return -EIO;
        }
    } else {
        status = wmi_add_krk_cmd(ar->arWmi, ik->ik_keydata);
    }

    ar->user_saved_keys.keyOk = TRUE;

    return 0;
}


/*
 * SIOCGIWNAME
 */
int
ar6000_ioctl_giwname(struct net_device *dev,
           struct iw_request_info *info,
           char *name, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    switch (ar->arPhyCapability) {
    case (WMI_11A_CAPABILITY):
        strncpy(name, "AR6000 802.11a", IFNAMSIZ);
        break;
    case (WMI_11G_CAPABILITY):
        strncpy(name, "AR6000 802.11g", IFNAMSIZ);
        break;
    case (WMI_11AG_CAPABILITY):
        strncpy(name, "AR6000 802.11ag", IFNAMSIZ);
        break;
    default:
        strncpy(name, "AR6000 802.11", IFNAMSIZ);
        break;
    }

    return 0;
}

/*
 * SIOCSIWFREQ
 */
int
ar6000_ioctl_siwfreq(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_freq *freq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    /*
     * We support limiting the channels via wmiconfig.
     *
     * We use this command to configure the channel hint for the connect cmd
     * so it is possible the target will end up connecting to a different
     * channel.
     */
    if (freq->e > 1) {
        return -EINVAL;
    } else if (freq->e == 1) {
        ar->arChannelHint = freq->m / 100000;
    } else {
        ar->arChannelHint = wlan_ieee2freq(freq->m);
    }

    A_PRINTF("channel hint set to %d\n", ar->arChannelHint);
    return 0;
}

/*
 * SIOCGIWFREQ
 */
int
ar6000_ioctl_giwfreq(struct net_device *dev,
                struct iw_request_info *info,
                struct iw_freq *freq, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arConnected != TRUE) {
        return -EINVAL;
    }

    freq->m = ar->arBssChannel * 100000;
    freq->e = 1;

    return 0;
}

/*
 * SIOCSIWMODE
 */
int
ar6000_ioctl_siwmode(struct net_device *dev,
            struct iw_request_info *info,
            __u32 *mode, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    switch (*mode) {
    case IW_MODE_INFRA:
        ar->arNetworkType = INFRA_NETWORK;
        break;
    case IW_MODE_ADHOC:
        ar->arNetworkType = ADHOC_NETWORK;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

/*
 * SIOCGIWMODE
 */
int
ar6000_ioctl_giwmode(struct net_device *dev,
            struct iw_request_info *info,
            __u32 *mode, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    switch (ar->arNetworkType) {
    case INFRA_NETWORK:
        *mode = IW_MODE_INFRA;
        break;
    case ADHOC_NETWORK:
        *mode = IW_MODE_ADHOC;
        break;
    default:
        return -EIO;
    }
    return 0;
}

/*
 * SIOCSIWSENS
 */
int
ar6000_ioctl_siwsens(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_param *sens, char *extra)
{
    return 0;
}

/*
 * SIOCGIWSENS
 */
int
ar6000_ioctl_giwsens(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_param *sens, char *extra)
{
    sens->value = 0;
    sens->fixed = 1;

    return 0;
}

/*
 * SIOCGIWRANGE
 */
int
ar6000_ioctl_giwrange(struct net_device *dev,
             struct iw_request_info *info,
             struct iw_point *data, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct iw_range *range = (struct iw_range *) extra;
    int i, ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }
    ar->arNumChannels = -1;
    A_MEMZERO(ar->arChannelList, sizeof (ar->arChannelList));

    if (wmi_get_channelList_cmd(ar->arWmi) != A_OK) {
        up(&ar->arSem);
        return -EIO;
    }

    wait_event_interruptible_timeout(arEvent, ar->arNumChannels != -1, wmitimeout * HZ);

    if (signal_pending(current)) {
        up(&ar->arSem);
        return -EINTR;
    }

    data->length = sizeof(struct iw_range);
    A_MEMZERO(range, sizeof(struct iw_range));

    range->txpower_capa = IW_TXPOW_DBM;

    range->min_pmp = 1 * 1024;
    range->max_pmp = 65535 * 1024;
    range->min_pmt = 1 * 1024;
    range->max_pmt = 1000 * 1024;
    range->pmp_flags = IW_POWER_PERIOD;
    range->pmt_flags = IW_POWER_TIMEOUT;
    range->pm_capa = 0;

    range->we_version_compiled = WIRELESS_EXT;
    range->we_version_source = 13;

    range->retry_capa = IW_RETRY_LIMIT;
    range->retry_flags = IW_RETRY_LIMIT;
    range->min_retry = 0;
    range->max_retry = 255;

    range->num_frequency = range->num_channels = ar->arNumChannels;
    for (i = 0; i < ar->arNumChannels; i++) {
        range->freq[i].i = wlan_freq2ieee(ar->arChannelList[i]);
        range->freq[i].m = ar->arChannelList[i] * 100000;
        range->freq[i].e = 1;
         /*
         * Linux supports max of 32 channels, bail out once you
         * reach the max.
         */
        if (i == IW_MAX_FREQUENCIES) {
            break;
        }
    }

    /* Max quality is max field value minus noise floor */
    range->max_qual.qual  = 0xff - 161;

    /*
     * In order to use dBm measurements, 'level' must be lower
     * than any possible measurement (see iw_print_stats() in
     * wireless tools).  It's unclear how this is meant to be
     * done, but setting zero in these values forces dBm and
     * the actual numbers are not used.
     */
    range->max_qual.level = 0;
    range->max_qual.noise = 0;

    range->sensitivity = 3;

    range->max_encoding_tokens = 4;
    /* XXX query driver to find out supported key sizes */
    range->num_encoding_sizes = 3;
    range->encoding_size[0] = 5;        /* 40-bit */
    range->encoding_size[1] = 13;       /* 104-bit */
    range->encoding_size[2] = 16;       /* 128-bit */

    range->num_bitrates = 0;

    /* estimated maximum TCP throughput values (bps) */
    range->throughput = 22000000;

    range->min_rts = 0;
    range->max_rts = 2347;
    range->min_frag = 256;
    range->max_frag = 2346;

    up(&ar->arSem);

    return ret;
}


/*
 * SIOCSIWAP
 * This ioctl is used to set the desired bssid for the connect command.
 */
int
ar6000_ioctl_siwap(struct net_device *dev,
              struct iw_request_info *info,
              struct sockaddr *ap_addr, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ap_addr->sa_family != ARPHRD_ETHER) {
        return -EIO;
    }

    if (A_MEMCMP(&ap_addr->sa_data, bcast_mac, AR6000_ETH_ADDR_LEN) == 0) {
        A_MEMZERO(ar->arReqBssid, sizeof(ar->arReqBssid));
    } else {
        A_MEMCPY(ar->arReqBssid, &ap_addr->sa_data,  sizeof(ar->arReqBssid));
    }

    return 0;
}

/*
 * SIOCGIWAP
 */
int
ar6000_ioctl_giwap(struct net_device *dev,
              struct iw_request_info *info,
              struct sockaddr *ap_addr, char *extra)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    if (ar->arConnected != TRUE) {
        return -EINVAL;
    }

    A_MEMCPY(&ap_addr->sa_data, ar->arBssid, sizeof(ar->arBssid));
    ap_addr->sa_family = ARPHRD_ETHER;

    return 0;
}

/*
 * SIOCGIWAPLIST
 */
int
ar6000_ioctl_iwaplist(struct net_device *dev,
            struct iw_request_info *info,
            struct iw_point *data, char *extra)
{
    return -EIO;            /* for now */
}

/*
 * SIOCSIWSCAN
 */
int
ar6000_ioctl_siwscan(struct net_device *dev,
                     struct iw_request_info *info,
                     struct iw_point *data, char *extra)
{
#define ACT_DWELLTIME_DEFAULT   105
#define HOME_TXDRAIN_TIME       100
#define SCAN_INT                HOME_TXDRAIN_TIME + ACT_DWELLTIME_DEFAULT
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (ar->arWlanState == WLAN_DISABLED) {
        return -EIO;
    }

    /* We ask for everything from the target */
    if (wmi_bssfilter_cmd(ar->arWmi, ALL_BSS_FILTER, 0) != A_OK) {
	    printk("Couldn't set filtering\n");
	    ret = -EIO;
    }

    if (wmi_startscan_cmd(ar->arWmi, WMI_LONG_SCAN, FALSE, FALSE, \
                          HOME_TXDRAIN_TIME, SCAN_INT) != A_OK) {
        ret = -EIO;
    }

    ar->scan_complete = 0;
    wait_event_interruptible_timeout(ar6000_scan_queue, ar->scan_complete,
				     5 * HZ);

    if (wmi_bssfilter_cmd(ar->arWmi, NONE_BSS_FILTER, 0) != A_OK) {
	    printk("Couldn't set filtering\n");
	    ret = -EIO;
    }

    return ret;
#undef  ACT_DWELLTIME_DEFAULT
#undef HOME_TXDRAIN_TIME
#undef SCAN_INT
}


/*
 * Units are in db above the noise floor. That means the
 * rssi values reported in the tx/rx descriptors in the
 * driver are the SNR expressed in db.
 *
 * If you assume that the noise floor is -95, which is an
 * excellent assumption 99.5 % of the time, then you can
 * derive the absolute signal level (i.e. -95 + rssi).
 * There are some other slight factors to take into account
 * depending on whether the rssi measurement is from 11b,
 * 11g, or 11a.   These differences are at most 2db and
 * can be documented.
 *
 * NB: various calculations are based on the orinoco/wavelan
 *     drivers for compatibility
 */
static void
ar6000_set_quality(struct iw_quality *iq, A_INT8 rssi)
{
    if (rssi < 0) {
        iq->qual = 0;
    } else {
        iq->qual = rssi;
    }

    /* NB: max is 94 because noise is hardcoded to 161 */
    if (iq->qual > 94)
        iq->qual = 94;

    iq->noise = 161;        /* -95dBm */
    iq->level = iq->noise + iq->qual;
    iq->updated = 7;
}


/* Structures to export the Wireless Handlers */
static const iw_handler ath_handlers[] = {
    (iw_handler) NULL,                          /* SIOCSIWCOMMIT */
    (iw_handler) ar6000_ioctl_giwname,          /* SIOCGIWNAME */
    (iw_handler) NULL,                          /* SIOCSIWNWID */
    (iw_handler) NULL,                          /* SIOCGIWNWID */
    (iw_handler) ar6000_ioctl_siwfreq,          /* SIOCSIWFREQ */
    (iw_handler) ar6000_ioctl_giwfreq,          /* SIOCGIWFREQ */
    (iw_handler) ar6000_ioctl_siwmode,          /* SIOCSIWMODE */
    (iw_handler) ar6000_ioctl_giwmode,          /* SIOCGIWMODE */
    (iw_handler) ar6000_ioctl_siwsens,          /* SIOCSIWSENS */
    (iw_handler) ar6000_ioctl_giwsens,          /* SIOCGIWSENS */
    (iw_handler) NULL /* not _used */,          /* SIOCSIWRANGE */
    (iw_handler) ar6000_ioctl_giwrange,         /* SIOCGIWRANGE */
    (iw_handler) NULL /* not used */,           /* SIOCSIWPRIV */
    (iw_handler) NULL /* kernel code */,        /* SIOCGIWPRIV */
    (iw_handler) NULL /* not used */,           /* SIOCSIWSTATS */
    (iw_handler) NULL /* kernel code */,        /* SIOCGIWSTATS */
    (iw_handler) NULL,                          /* SIOCSIWSPY */
    (iw_handler) NULL,                          /* SIOCGIWSPY */
    (iw_handler) NULL,                          /* SIOCSIWTHRSPY */
    (iw_handler) NULL,                          /* SIOCGIWTHRSPY */
    (iw_handler) ar6000_ioctl_siwap,            /* SIOCSIWAP */
    (iw_handler) ar6000_ioctl_giwap,            /* SIOCGIWAP */
    (iw_handler) NULL,                          /* -- hole -- */
    (iw_handler) ar6000_ioctl_iwaplist,         /* SIOCGIWAPLIST */
    (iw_handler) ar6000_ioctl_siwscan,          /* SIOCSIWSCAN */
    (iw_handler) ar6000_ioctl_giwscan,          /* SIOCGIWSCAN */
    (iw_handler) ar6000_ioctl_siwessid,         /* SIOCSIWESSID */
    (iw_handler) ar6000_ioctl_giwessid,         /* SIOCGIWESSID */
    (iw_handler) NULL,                          /* SIOCSIWNICKN */
    (iw_handler) NULL,                          /* SIOCGIWNICKN */
    (iw_handler) NULL,                          /* -- hole -- */
    (iw_handler) NULL,                          /* -- hole -- */
    (iw_handler) ar6000_ioctl_siwrate,          /* SIOCSIWRATE */
    (iw_handler) ar6000_ioctl_giwrate,          /* SIOCGIWRATE */
    (iw_handler) NULL,           /* SIOCSIWRTS */
    (iw_handler) NULL,           /* SIOCGIWRTS */
    (iw_handler) NULL,          /* SIOCSIWFRAG */
    (iw_handler) NULL,          /* SIOCGIWFRAG */
    (iw_handler) ar6000_ioctl_siwtxpow,         /* SIOCSIWTXPOW */
    (iw_handler) ar6000_ioctl_giwtxpow,         /* SIOCGIWTXPOW */
    (iw_handler) ar6000_ioctl_siwretry,         /* SIOCSIWRETRY */
    (iw_handler) ar6000_ioctl_giwretry,         /* SIOCGIWRETRY */
    (iw_handler) ar6000_ioctl_siwencode,        /* SIOCSIWENCODE */
    (iw_handler) ar6000_ioctl_giwencode,        /* SIOCGIWENCODE */
    (iw_handler) ar6000_ioctl_siwpower,         /* SIOCSIWPOWER */
    (iw_handler) ar6000_ioctl_giwpower,         /* SIOCGIWPOWER */
    (iw_handler) NULL,	/* -- hole -- */
    (iw_handler) NULL,	/* -- hole -- */
    (iw_handler) ar6000_ioctl_siwgenie,	/* SIOCSIWGENIE */
    (iw_handler) ar6000_ioctl_giwgenie,	/* SIOCGIWGENIE */
    (iw_handler) ar6000_ioctl_siwauth,	/* SIOCSIWAUTH */
    (iw_handler) ar6000_ioctl_giwauth,	/* SIOCGIWAUTH */
    (iw_handler) ar6000_ioctl_siwencodeext,/* SIOCSIWENCODEEXT */
    (iw_handler) ar6000_ioctl_giwencodeext,/* SIOCGIWENCODEEXT */
    (iw_handler) NULL,		/* SIOCSIWPMKSA */
};

static const iw_handler ath_priv_handlers[] = {
    (iw_handler) ar6000_ioctl_setparam,         /* SIOCWFIRSTPRIV+0 */
    (iw_handler) ar6000_ioctl_getparam,         /* SIOCWFIRSTPRIV+1 */
    (iw_handler) ar6000_ioctl_setkey,           /* SIOCWFIRSTPRIV+2 */
    (iw_handler) ar6000_ioctl_setwmmparams,     /* SIOCWFIRSTPRIV+3 */
    (iw_handler) ar6000_ioctl_delkey,           /* SIOCWFIRSTPRIV+4 */
    (iw_handler) ar6000_ioctl_getwmmparams,     /* SIOCWFIRSTPRIV+5 */
    (iw_handler) ar6000_ioctl_setoptie,         /* SIOCWFIRSTPRIV+6 */
    (iw_handler) ar6000_ioctl_setmlme,          /* SIOCWFIRSTPRIV+7 */
    (iw_handler) ar6000_ioctl_addpmkid,         /* SIOCWFIRSTPRIV+8 */
};

#define IW_PRIV_TYPE_KEY \
    (IW_PRIV_TYPE_BYTE | sizeof(struct ieee80211req_key))
#define IW_PRIV_TYPE_DELKEY \
    (IW_PRIV_TYPE_BYTE | sizeof(struct ieee80211req_del_key))
#define IW_PRIV_TYPE_MLME \
    (IW_PRIV_TYPE_BYTE | sizeof(struct ieee80211req_mlme))
#define IW_PRIV_TYPE_ADDPMKID \
    (IW_PRIV_TYPE_BYTE | sizeof(struct ieee80211req_addpmkid))

static const struct iw_priv_args ar6000_priv_args[] = {
    { IEEE80211_IOCTL_SETKEY,
      IW_PRIV_TYPE_KEY | IW_PRIV_SIZE_FIXED, 0,       "setkey"},
    { IEEE80211_IOCTL_DELKEY,
      IW_PRIV_TYPE_DELKEY | IW_PRIV_SIZE_FIXED, 0,    "delkey"},
    { IEEE80211_IOCTL_SETPARAM,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0,   "setparam"},
    { IEEE80211_IOCTL_GETPARAM,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,      "getparam"},
    { IEEE80211_IOCTL_SETWMMPARAMS,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 4, 0,   "setwmmparams"},
    { IEEE80211_IOCTL_GETWMMPARAMS,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 3,
      IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,      "getwmmparams"},
    { IEEE80211_IOCTL_SETOPTIE,
      IW_PRIV_TYPE_BYTE, 0,       "setie"},
    { IEEE80211_IOCTL_SETMLME,
      IW_PRIV_TYPE_MLME, 0,       "setmlme"},
    { IEEE80211_IOCTL_ADDPMKID,
      IW_PRIV_TYPE_ADDPMKID | IW_PRIV_SIZE_FIXED, 0,  "addpmkid"},
};

void ar6000_ioctl_iwsetup(struct iw_handler_def *def)
{
    def->private_args = (struct iw_priv_args *)ar6000_priv_args;
    def->num_private_args = ARRAY_SIZE(ar6000_priv_args);
}

struct iw_handler_def ath_iw_handler_def = {
    .standard         = (iw_handler *)ath_handlers,
    .num_standard     = ARRAY_SIZE(ath_handlers),
    .private          = (iw_handler *)ath_priv_handlers,
    .num_private      = ARRAY_SIZE(ath_priv_handlers),
};


