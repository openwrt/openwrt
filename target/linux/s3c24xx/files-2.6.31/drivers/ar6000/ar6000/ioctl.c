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
static A_UINT8 null_mac[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
extern USER_RSSI_THOLD rssi_map[12];
extern unsigned int wmitimeout;
extern A_WAITQUEUE_HEAD arEvent;
extern int tspecCompliance;
extern int bypasswmi;

static int
ar6000_ioctl_get_roam_tbl(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if(wmi_get_roam_tbl_cmd(ar->arWmi) != A_OK) {
        return -EIO;
    }

    return 0;
}

static int
ar6000_ioctl_get_roam_data(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }


    /* currently assume only roam times are required */
    if(wmi_get_roam_data_cmd(ar->arWmi, ROAM_DATA_TIME) != A_OK) {
        return -EIO;
    }


    return 0;
}

static int
ar6000_ioctl_set_roam_ctrl(struct net_device *dev, char *userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_ROAM_CTRL_CMD cmd;
    A_UINT8 size = sizeof(cmd);

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }


    if (copy_from_user(&cmd, userdata, size)) {
        return -EFAULT;
    }

    if (cmd.roamCtrlType == WMI_SET_HOST_BIAS) {
        if (cmd.info.bssBiasInfo.numBss > 1) {
            size += (cmd.info.bssBiasInfo.numBss - 1) * sizeof(WMI_BSS_BIAS);
        }
    }

    if (copy_from_user(&cmd, userdata, size)) {
        return -EFAULT;
    }

    if(wmi_set_roam_ctrl_cmd(ar->arWmi, &cmd, size) != A_OK) {
        return -EIO;
    }

    return 0;
}

static int
ar6000_ioctl_set_powersave_timers(struct net_device *dev, char *userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_POWERSAVE_TIMERS_POLICY_CMD cmd;
    A_UINT8 size = sizeof(cmd);

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, userdata, size)) {
        return -EFAULT;
    }

    if (copy_from_user(&cmd, userdata, size)) {
        return -EFAULT;
    }

    if(wmi_set_powersave_timers_cmd(ar->arWmi, &cmd, size) != A_OK) {
        return -EIO;
    }

    return 0;
}

static int
ar6000_ioctl_set_wmm(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_WMM_CMD cmd;
    A_STATUS ret;

    if ((dev->flags & IFF_UP) != IFF_UP) {
        return -EIO;
    }
    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, (char *)((unsigned int*)rq->ifr_data + 1),
                                sizeof(cmd)))
    {
        return -EFAULT;
    }

    if (cmd.status == WMI_WMM_ENABLED) {
        ar->arWmmEnabled = TRUE;
    } else {
        ar->arWmmEnabled = FALSE;
    }

    ret = wmi_set_wmm_cmd(ar->arWmi, cmd.status);

    switch (ret) {
        case A_OK:
            return 0;
        case A_EBUSY :
            return -EBUSY;
        case A_NO_MEMORY:
            return -ENOMEM;
        case A_EINVAL:
        default:
            return -EFAULT;
    }
}

static int
ar6000_ioctl_set_txop(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_WMM_TXOP_CMD cmd;
    A_STATUS ret;

    if ((dev->flags & IFF_UP) != IFF_UP) {
        return -EIO;
    }
    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, (char *)((unsigned int*)rq->ifr_data + 1),
                                sizeof(cmd)))
    {
        return -EFAULT;
    }

    ret = wmi_set_wmm_txop(ar->arWmi, cmd.txopEnable);

    switch (ret) {
        case A_OK:
            return 0;
        case A_EBUSY :
            return -EBUSY;
        case A_NO_MEMORY:
            return -ENOMEM;
        case A_EINVAL:
        default:
            return -EFAULT;
    }
}

static int
ar6000_ioctl_get_rd(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_STATUS ret = 0;

    if ((dev->flags & IFF_UP) != IFF_UP || ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if(copy_to_user((char *)((unsigned int*)rq->ifr_data + 1),
                            &ar->arRegCode, sizeof(ar->arRegCode)))
        ret = -EFAULT;

    return ret;
}


/* Get power mode command */
static int
ar6000_ioctl_get_power_mode(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_POWER_MODE_CMD power_mode;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    power_mode.powerMode = wmi_get_power_mode_cmd(ar->arWmi);
    if (copy_to_user(rq->ifr_data, &power_mode, sizeof(WMI_POWER_MODE_CMD))) {
        ret = -EFAULT;
    }

    return ret;
}


static int
ar6000_ioctl_set_channelParams(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_CHANNEL_PARAMS_CMD cmd, *cmdp;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }


    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if (cmd.numChannels > 1) {
        cmdp = A_MALLOC(130);
        if (copy_from_user(cmdp, rq->ifr_data,
                           sizeof (*cmdp) +
                           ((cmd.numChannels - 1) * sizeof(A_UINT16))))
        {
            kfree(cmdp);
            return -EFAULT;
        }
    } else {
        cmdp = &cmd;
    }

    if ((ar->arPhyCapability == WMI_11G_CAPABILITY) &&
        ((cmdp->phyMode == WMI_11A_MODE) || (cmdp->phyMode == WMI_11AG_MODE)))
    {
        ret = -EINVAL;
    }

    if (!ret &&
        (wmi_set_channelParams_cmd(ar->arWmi, cmdp->scanParam, cmdp->phyMode,
                                   cmdp->numChannels, cmdp->channelList)
         != A_OK))
    {
        ret = -EIO;
    }

    if (cmd.numChannels > 1) {
        kfree(cmdp);
    }

    return ret;
}

static int
ar6000_ioctl_set_snr_threshold(struct net_device *dev, struct ifreq *rq)
{

    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SNR_THRESHOLD_PARAMS_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if( wmi_set_snr_threshold_params(ar->arWmi, &cmd) != A_OK ) {
        ret = -EIO;
    }

    return ret;
}

static int
ar6000_ioctl_set_rssi_threshold(struct net_device *dev, struct ifreq *rq)
{
#define SWAP_THOLD(thold1, thold2) do { \
    USER_RSSI_THOLD tmpThold;           \
    tmpThold.tag = thold1.tag;          \
    tmpThold.rssi = thold1.rssi;        \
    thold1.tag = thold2.tag;            \
    thold1.rssi = thold2.rssi;          \
    thold2.tag = tmpThold.tag;          \
    thold2.rssi = tmpThold.rssi;        \
} while (0)

    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_RSSI_THRESHOLD_PARAMS_CMD cmd;
    USER_RSSI_PARAMS rssiParams;
    A_INT32 i, j;

    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user((char *)&rssiParams, (char *)((unsigned int *)rq->ifr_data + 1), sizeof(USER_RSSI_PARAMS))) {
        return -EFAULT;
    }
    cmd.weight = rssiParams.weight;
    cmd.pollTime = rssiParams.pollTime;

    A_MEMCPY(rssi_map, &rssiParams.tholds, sizeof(rssi_map));
    /*
     *  only 6 elements, so use bubble sorting, in ascending order
     */
    for (i = 5; i > 0; i--) {
        for (j = 0; j < i; j++) { /* above tholds */
            if (rssi_map[j+1].rssi < rssi_map[j].rssi) {
                SWAP_THOLD(rssi_map[j+1], rssi_map[j]);
            } else if (rssi_map[j+1].rssi == rssi_map[j].rssi) {
                return EFAULT;
            }
        }
    }
    for (i = 11; i > 6; i--) {
        for (j = 6; j < i; j++) { /* below tholds */
            if (rssi_map[j+1].rssi < rssi_map[j].rssi) {
                SWAP_THOLD(rssi_map[j+1], rssi_map[j]);
            } else if (rssi_map[j+1].rssi == rssi_map[j].rssi) {
                return EFAULT;
            }
        }
    }

#ifdef DEBUG
    for (i = 0; i < 12; i++) {
        AR_DEBUG2_PRINTF("thold[%d].tag: %d, thold[%d].rssi: %d \n",
                i, rssi_map[i].tag, i, rssi_map[i].rssi);
    }
#endif
    cmd.thresholdAbove1_Val = rssi_map[0].rssi;
    cmd.thresholdAbove2_Val = rssi_map[1].rssi;
    cmd.thresholdAbove3_Val = rssi_map[2].rssi;
    cmd.thresholdAbove4_Val = rssi_map[3].rssi;
    cmd.thresholdAbove5_Val = rssi_map[4].rssi;
    cmd.thresholdAbove6_Val = rssi_map[5].rssi;
    cmd.thresholdBelow1_Val = rssi_map[6].rssi;
    cmd.thresholdBelow2_Val = rssi_map[7].rssi;
    cmd.thresholdBelow3_Val = rssi_map[8].rssi;
    cmd.thresholdBelow4_Val = rssi_map[9].rssi;
    cmd.thresholdBelow5_Val = rssi_map[10].rssi;
    cmd.thresholdBelow6_Val = rssi_map[11].rssi;

    if( wmi_set_rssi_threshold_params(ar->arWmi, &cmd) != A_OK ) {
        ret = -EIO;
    }

    return ret;
}

static int
ar6000_ioctl_set_lq_threshold(struct net_device *dev, struct ifreq *rq)
{

    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_LQ_THRESHOLD_PARAMS_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, (char *)((unsigned int *)rq->ifr_data + 1), sizeof(cmd))) {
        return -EFAULT;
    }

    if( wmi_set_lq_threshold_params(ar->arWmi, &cmd) != A_OK ) {
        ret = -EIO;
    }

    return ret;
}


static int
ar6000_ioctl_set_probedSsid(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_PROBED_SSID_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_probedSsid_cmd(ar->arWmi, cmd.entryIndex, cmd.flag, cmd.ssidLength,
                                  cmd.ssid) != A_OK)
    {
        ret = -EIO;
    }

    return ret;
}

static int
ar6000_ioctl_set_badAp(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_ADD_BAD_AP_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }


    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if (cmd.badApIndex > WMI_MAX_BAD_AP_INDEX) {
        return -EIO;
    }

    if (A_MEMCMP(cmd.bssid, null_mac, AR6000_ETH_ADDR_LEN) == 0) {
        /*
         * This is a delete badAP.
         */
        if (wmi_deleteBadAp_cmd(ar->arWmi, cmd.badApIndex) != A_OK) {
            ret = -EIO;
        }
    } else {
        if (wmi_addBadAp_cmd(ar->arWmi, cmd.badApIndex, cmd.bssid) != A_OK) {
            ret = -EIO;
        }
    }

    return ret;
}

static int
ar6000_ioctl_create_qos(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_CREATE_PSTREAM_CMD cmd;
    A_STATUS ret;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }


    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    ret = wmi_verify_tspec_params(&cmd, tspecCompliance);
    if (ret == A_OK)
        ret = wmi_create_pstream_cmd(ar->arWmi, &cmd);

    switch (ret) {
        case A_OK:
            return 0;
        case A_EBUSY :
            return -EBUSY;
        case A_NO_MEMORY:
            return -ENOMEM;
        case A_EINVAL:
        default:
            return -EFAULT;
    }
}

static int
ar6000_ioctl_delete_qos(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_DELETE_PSTREAM_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    ret = wmi_delete_pstream_cmd(ar->arWmi, cmd.trafficClass, cmd.tsid);

    switch (ret) {
        case A_OK:
            return 0;
        case A_EBUSY :
            return -EBUSY;
        case A_NO_MEMORY:
            return -ENOMEM;
        case A_EINVAL:
        default:
            return -EFAULT;
    }
}

static int
ar6000_ioctl_get_qos_queue(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    struct ar6000_queuereq qreq;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if( copy_from_user(&qreq, rq->ifr_data,
                  sizeof(struct ar6000_queuereq)))
        return -EFAULT;

    qreq.activeTsids = wmi_get_mapped_qos_queue(ar->arWmi, qreq.trafficClass);

    if (copy_to_user(rq->ifr_data, &qreq,
                 sizeof(struct ar6000_queuereq)))
    {
        ret = -EFAULT;
    }

    return ret;
}

#ifdef CONFIG_HOST_TCMD_SUPPORT
static A_STATUS
ar6000_ioctl_tcmd_get_rx_report(struct net_device *dev,
                                 struct ifreq *rq, A_UINT8 *data, A_UINT32 len)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_UINT32    buf[2];
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }
    ar->tcmdRxReport = 0;
    if (wmi_test_cmd(ar->arWmi, data, len) != A_OK) {
        up(&ar->arSem);
        return -EIO;
    }

    wait_event_interruptible_timeout(arEvent, ar->tcmdRxReport != 0, wmitimeout * HZ);

    if (signal_pending(current)) {
        ret = -EINTR;
    }

    buf[0] = ar->tcmdRxTotalPkt;
    buf[1] = ar->tcmdRxRssi;
    if (!ret && copy_to_user(rq->ifr_data, buf, sizeof(buf))) {
        ret = -EFAULT;
    }

    up(&ar->arSem);

    return ret;
}

void
ar6000_tcmd_rx_report_event(void *devt, A_UINT8 * results, int len)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;
    TCMD_CONT_RX * rx_rep = (TCMD_CONT_RX *)results;

    ar->tcmdRxTotalPkt = rx_rep->u.report.totalPkt;
    ar->tcmdRxRssi = rx_rep->u.report.rssiInDBm;
    ar->tcmdRxReport = 1;

    wake_up(&arEvent);
}
#endif /* CONFIG_HOST_TCMD_SUPPORT*/

static int
ar6000_ioctl_set_error_report_bitmask(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_TARGET_ERROR_REPORT_BITMASK cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    ret = wmi_set_error_report_bitmask(ar->arWmi, cmd.bitmask);

    return  (ret==0 ? ret : -EINVAL);
}

static int
ar6000_clear_target_stats(struct net_device *dev)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    TARGET_STATS *pStats = &ar->arTargetStats;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
       return -EIO;
    }
    AR6000_SPIN_LOCK(&ar->arLock, 0);
    A_MEMZERO(pStats, sizeof(TARGET_STATS));
    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
    return ret;
}

static int
ar6000_ioctl_get_target_stats(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    TARGET_STATS_CMD cmd;
    TARGET_STATS *pStats = &ar->arTargetStats;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }
    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }
    if (down_interruptible(&ar->arSem)) {
        return -ERESTARTSYS;
    }

    ar->statsUpdatePending = TRUE;

    if(wmi_get_stats_cmd(ar->arWmi) != A_OK) {
        up(&ar->arSem);
        return -EIO;
    }

    wait_event_interruptible_timeout(arEvent, ar->statsUpdatePending == FALSE, wmitimeout * HZ);

    if (signal_pending(current)) {
        ret = -EINTR;
    }

    if (!ret && copy_to_user(rq->ifr_data, pStats, sizeof(*pStats))) {
        ret = -EFAULT;
    }

    if (cmd.clearStats == 1) {
        ret = ar6000_clear_target_stats(dev);
    }

    up(&ar->arSem);

    return ret;
}

static int
ar6000_ioctl_set_access_params(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_ACCESS_PARAMS_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_set_access_params_cmd(ar->arWmi, cmd.txop, cmd.eCWmin, cmd.eCWmax,
                                  cmd.aifsn) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }

    return (ret);
}

static int
ar6000_ioctl_set_disconnect_timeout(struct net_device *dev, struct ifreq *rq)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_DISC_TIMEOUT_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, rq->ifr_data, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_disctimeout_cmd(ar->arWmi, cmd.disconnectTimeout) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }

    return (ret);
}

static int
ar6000_xioctl_set_voice_pkt_size(struct net_device *dev, char * userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_VOICE_PKT_SIZE_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, userdata, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_set_voice_pkt_size_cmd(ar->arWmi, cmd.voicePktSize) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }


    return (ret);
}

static int
ar6000_xioctl_set_max_sp_len(struct net_device *dev, char * userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_MAX_SP_LEN_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, userdata, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_set_max_sp_len_cmd(ar->arWmi, cmd.maxSPLen) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }

    return (ret);
}


static int
ar6000_xioctl_set_bt_status_cmd(struct net_device *dev, char * userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_BT_STATUS_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, userdata, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_set_bt_status_cmd(ar->arWmi, cmd.streamType, cmd.status) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }

    return (ret);
}

static int
ar6000_xioctl_set_bt_params_cmd(struct net_device *dev, char * userdata)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_SET_BT_PARAMS_CMD cmd;
    int ret = 0;

    if (ar->arWmiReady == FALSE) {
        return -EIO;
    }

    if (copy_from_user(&cmd, userdata, sizeof(cmd))) {
        return -EFAULT;
    }

    if (wmi_set_bt_params_cmd(ar->arWmi, &cmd) == A_OK)
    {
        ret = 0;
    } else {
        ret = -EINVAL;
    }

    return (ret);
}

#ifdef CONFIG_HOST_GPIO_SUPPORT
struct ar6000_gpio_intr_wait_cmd_s  gpio_intr_results;
/* gpio_reg_results and gpio_data_available are protected by arSem */
static struct ar6000_gpio_register_cmd_s gpio_reg_results;
static A_BOOL gpio_data_available; /* Requested GPIO data available */
static A_BOOL gpio_intr_available; /* GPIO interrupt info available */
static A_BOOL gpio_ack_received;   /* GPIO ack was received */

/* Host-side initialization for General Purpose I/O support */
void ar6000_gpio_init(void)
{
    gpio_intr_available = FALSE;
    gpio_data_available = FALSE;
    gpio_ack_received   = FALSE;
}

/*
 * Called when a GPIO interrupt is received from the Target.
 * intr_values shows which GPIO pins have interrupted.
 * input_values shows a recent value of GPIO pins.
 */
void
ar6000_gpio_intr_rx(A_UINT32 intr_mask, A_UINT32 input_values)
{
    gpio_intr_results.intr_mask = intr_mask;
    gpio_intr_results.input_values = input_values;
    *((volatile A_BOOL *)&gpio_intr_available) = TRUE;
    wake_up(&arEvent);
}

/*
 * This is called when a response is received from the Target
 * for a previous or ar6000_gpio_input_get or ar6000_gpio_register_get
 * call.
 */
void
ar6000_gpio_data_rx(A_UINT32 reg_id, A_UINT32 value)
{
    gpio_reg_results.gpioreg_id = reg_id;
    gpio_reg_results.value = value;
    *((volatile A_BOOL *)&gpio_data_available) = TRUE;
    wake_up(&arEvent);
}

/*
 * This is called when an acknowledgement is received from the Target
 * for a previous or ar6000_gpio_output_set or ar6000_gpio_register_set
 * call.
 */
void
ar6000_gpio_ack_rx(void)
{
    gpio_ack_received = TRUE;
    wake_up(&arEvent);
}

A_STATUS
ar6000_gpio_output_set(struct net_device *dev,
                       A_UINT32 set_mask,
                       A_UINT32 clear_mask,
                       A_UINT32 enable_mask,
                       A_UINT32 disable_mask)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    gpio_ack_received = FALSE;
    return wmi_gpio_output_set(ar->arWmi,
                set_mask, clear_mask, enable_mask, disable_mask);
}

static A_STATUS
ar6000_gpio_input_get(struct net_device *dev)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    *((volatile A_BOOL *)&gpio_data_available) = FALSE;
    return wmi_gpio_input_get(ar->arWmi);
}

static A_STATUS
ar6000_gpio_register_set(struct net_device *dev,
                         A_UINT32 gpioreg_id,
                         A_UINT32 value)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    gpio_ack_received = FALSE;
    return wmi_gpio_register_set(ar->arWmi, gpioreg_id, value);
}

static A_STATUS
ar6000_gpio_register_get(struct net_device *dev,
                         A_UINT32 gpioreg_id)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    *((volatile A_BOOL *)&gpio_data_available) = FALSE;
    return wmi_gpio_register_get(ar->arWmi, gpioreg_id);
}

static A_STATUS
ar6000_gpio_intr_ack(struct net_device *dev,
                     A_UINT32 ack_mask)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

    gpio_intr_available = FALSE;
    return wmi_gpio_intr_ack(ar->arWmi, ack_mask);
}
#endif /* CONFIG_HOST_GPIO_SUPPORT */

int ar6000_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    HIF_DEVICE *hifDevice = ar->arHifDevice;
    int ret, param, param2;
    unsigned int address = 0;
    unsigned int length = 0;
    unsigned char *buffer;
    char *userdata;
    A_UINT32 connectCtrlFlags;


    static WMI_SCAN_PARAMS_CMD scParams = {0, 0, 0, 0, 0,
                                           WMI_SHORTSCANRATIO_DEFAULT,
                                           DEFAULT_SCAN_CTRL_FLAGS,
                                           0};
    WMI_SET_AKMP_PARAMS_CMD  akmpParams;
    WMI_SET_PMKID_LIST_CMD   pmkidInfo;

    if (cmd == AR6000_IOCTL_EXTENDED)
    {
        /*
         * This allows for many more wireless ioctls than would otherwise
         * be available.  Applications embed the actual ioctl command in
         * the first word of the parameter block, and use the command
         * AR6000_IOCTL_EXTENDED_CMD on the ioctl call.
         */
        get_user(cmd, (int *)rq->ifr_data);
        userdata = (char *)(((unsigned int *)rq->ifr_data)+1);
    }
    else
    {
        userdata = (char *)rq->ifr_data;
    }

    if ((ar->arWlanState == WLAN_DISABLED) &&
        ((cmd != AR6000_XIOCTRL_WMI_SET_WLAN_STATE) &&
         (cmd != AR6000_XIOCTL_DIAG_READ) &&
         (cmd != AR6000_XIOCTL_DIAG_WRITE)))
    {
        return -EIO;
    }

    ret = 0;
    switch(cmd)
    {
#ifdef CONFIG_HOST_TCMD_SUPPORT
        case AR6000_XIOCTL_TCMD_CONT_TX:
            {
                TCMD_CONT_TX txCmd;

                if (ar->tcmdPm == TCMD_PM_SLEEP) {
                    A_PRINTF("Can NOT send tx tcmd when target is asleep! \n");
                    return -EFAULT;
                }

                if(copy_from_user(&txCmd, userdata, sizeof(TCMD_CONT_TX)))
                    return -EFAULT;
                wmi_test_cmd(ar->arWmi,(A_UINT8 *)&txCmd, sizeof(TCMD_CONT_TX));
            }
            break;
        case AR6000_XIOCTL_TCMD_CONT_RX:
            {
                TCMD_CONT_RX rxCmd;

                if (ar->tcmdPm == TCMD_PM_SLEEP) {
                    A_PRINTF("Can NOT send rx tcmd when target is asleep! \n");
                    return -EFAULT;
                }
                if(copy_from_user(&rxCmd, userdata, sizeof(TCMD_CONT_RX)))
                    return -EFAULT;
                switch(rxCmd.act)
                {
                    case TCMD_CONT_RX_PROMIS:
                    case TCMD_CONT_RX_FILTER:
                    case TCMD_CONT_RX_SETMAC:
                         wmi_test_cmd(ar->arWmi,(A_UINT8 *)&rxCmd,
                                                sizeof(TCMD_CONT_RX));
                         break;
                    case TCMD_CONT_RX_REPORT:
                         ar6000_ioctl_tcmd_get_rx_report(dev, rq,
                         (A_UINT8 *)&rxCmd, sizeof(TCMD_CONT_RX));
                         break;
                    default:
                         A_PRINTF("Unknown Cont Rx mode: %d\n",rxCmd.act);
                         return -EINVAL;
                }
            }
            break;
        case AR6000_XIOCTL_TCMD_PM:
            {
                TCMD_PM pmCmd;

                if(copy_from_user(&pmCmd, userdata, sizeof(TCMD_PM)))
                    return -EFAULT;
                ar->tcmdPm = pmCmd.mode;
                wmi_test_cmd(ar->arWmi, (A_UINT8*)&pmCmd, sizeof(TCMD_PM));
            }
            break;
#endif /* CONFIG_HOST_TCMD_SUPPORT */

        case AR6000_XIOCTL_BMI_DONE:
            ret = BMIDone(hifDevice);
            break;

        case AR6000_XIOCTL_BMI_READ_MEMORY:
            get_user(address, (unsigned int *)userdata);
            get_user(length, (unsigned int *)userdata + 1);
            AR_DEBUG_PRINTF("Read Memory (address: 0x%x, length: %d)\n",
                             address, length);
            if ((buffer = (unsigned char *)A_MALLOC(length)) != NULL) {
                A_MEMZERO(buffer, length);
                ret = BMIReadMemory(hifDevice, address, buffer, length);
                if (copy_to_user(rq->ifr_data, buffer, length)) {
                    ret = -EFAULT;
                }
                A_FREE(buffer);
            } else {
                ret = -ENOMEM;
            }
            break;

        case AR6000_XIOCTL_BMI_WRITE_MEMORY:
            get_user(address, (unsigned int *)userdata);
            get_user(length, (unsigned int *)userdata + 1);
            AR_DEBUG_PRINTF("Write Memory (address: 0x%x, length: %d)\n",
                             address, length);
            if ((buffer = (unsigned char *)A_MALLOC(length)) != NULL) {
                A_MEMZERO(buffer, length);
                if (copy_from_user(buffer, &userdata[sizeof(address) +
                                   sizeof(length)], length))
                {
                    ret = -EFAULT;
                } else {
                    ret = BMIWriteMemory(hifDevice, address, buffer, length);
                }
                A_FREE(buffer);
            } else {
                ret = -ENOMEM;
            }
            break;

        case AR6000_XIOCTL_BMI_TEST:
           AR_DEBUG_PRINTF("No longer supported\n");
           ret = -EOPNOTSUPP;
           break;

        case AR6000_XIOCTL_BMI_EXECUTE:
            get_user(address, (unsigned int *)userdata);
            get_user(param, (unsigned int *)userdata + 1);
            AR_DEBUG_PRINTF("Execute (address: 0x%x, param: %d)\n",
                             address, param);
            ret = BMIExecute(hifDevice, address, &param);
            put_user(param, (unsigned int *)rq->ifr_data); /* return value */
            break;

        case AR6000_XIOCTL_BMI_SET_APP_START:
            get_user(address, (unsigned int *)userdata);
            AR_DEBUG_PRINTF("Set App Start (address: 0x%x)\n", address);
            ret = BMISetAppStart(hifDevice, address);
            break;

        case AR6000_XIOCTL_BMI_READ_SOC_REGISTER:
            get_user(address, (unsigned int *)userdata);
            ret = BMIReadSOCRegister(hifDevice, address, &param);
            put_user(param, (unsigned int *)rq->ifr_data); /* return value */
            break;

        case AR6000_XIOCTL_BMI_WRITE_SOC_REGISTER:
            get_user(address, (unsigned int *)userdata);
            get_user(param, (unsigned int *)userdata + 1);
            ret = BMIWriteSOCRegister(hifDevice, address, param);
            break;

#ifdef HTC_RAW_INTERFACE
        case AR6000_XIOCTL_HTC_RAW_OPEN:
            ret = A_OK;
            if (!arRawIfEnabled(ar)) {
                /* make sure block size is set in case the target was reset since last
                  * BMI phase (i.e. flashup downloads) */
                ret = ar6000_SetHTCBlockSize(ar);
                if (A_FAILED(ret)) {
                    break;
                }
                /* Terminate the BMI phase */
                ret = BMIDone(hifDevice);
                if (ret == A_OK) {
                    ret = ar6000_htc_raw_open(ar);
                }
            }
            break;

        case AR6000_XIOCTL_HTC_RAW_CLOSE:
            if (arRawIfEnabled(ar)) {
                ret = ar6000_htc_raw_close(ar);
                arRawIfEnabled(ar) = FALSE;
            } else {
                ret = A_ERROR;
            }
            break;

        case AR6000_XIOCTL_HTC_RAW_READ:
            if (arRawIfEnabled(ar)) {
                unsigned int streamID;
                get_user(streamID, (unsigned int *)userdata);
                get_user(length, (unsigned int *)userdata + 1);
                buffer = rq->ifr_data + sizeof(length);
                ret = ar6000_htc_raw_read(ar, (HTC_RAW_STREAM_ID)streamID,
                                          buffer, length);
                put_user(ret, (unsigned int *)rq->ifr_data);
            } else {
                ret = A_ERROR;
            }
            break;

        case AR6000_XIOCTL_HTC_RAW_WRITE:
            if (arRawIfEnabled(ar)) {
                unsigned int streamID;
                get_user(streamID, (unsigned int *)userdata);
                get_user(length, (unsigned int *)userdata + 1);
                buffer = userdata + sizeof(streamID) + sizeof(length);
                ret = ar6000_htc_raw_write(ar, (HTC_RAW_STREAM_ID)streamID,
                                           buffer, length);
                put_user(ret, (unsigned int *)rq->ifr_data);
            } else {
                ret = A_ERROR;
            }
            break;
#endif /* HTC_RAW_INTERFACE */

        case AR6000_IOCTL_WMI_GETREV:
        {
            if (copy_to_user(rq->ifr_data, &ar->arVersion,
                             sizeof(ar->arVersion)))
            {
                ret = -EFAULT;
            }
            break;
        }
        case AR6000_IOCTL_WMI_SETPWR:
        {
            WMI_POWER_MODE_CMD pwrModeCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&pwrModeCmd, userdata,
                                   sizeof(pwrModeCmd)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_powermode_cmd(ar->arWmi, pwrModeCmd.powerMode)
                       != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SET_IBSS_PM_CAPS:
        {
            WMI_IBSS_PM_CAPS_CMD ibssPmCaps;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&ibssPmCaps, userdata,
                                   sizeof(ibssPmCaps)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_ibsspmcaps_cmd(ar->arWmi, ibssPmCaps.power_saving, ibssPmCaps.ttl,
                    ibssPmCaps.atim_windows, ibssPmCaps.timeout_value) != A_OK)
                {
                    ret = -EIO;
                }
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                ar->arIbssPsEnable = ibssPmCaps.power_saving;
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            }
            break;
        }
        case AR6000_IOCTL_WMI_SET_PMPARAMS:
        {
            WMI_POWER_PARAMS_CMD pmParams;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&pmParams, userdata,
                                      sizeof(pmParams)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_pmparams_cmd(ar->arWmi, pmParams.idle_period,
                                     pmParams.pspoll_number,
                                     pmParams.dtim_policy) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SETSCAN:
        {
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&scParams, userdata,
                                      sizeof(scParams)))
            {
                ret = -EFAULT;
            } else {
                if (CAN_SCAN_IN_CONNECT(scParams.scanCtrlFlags)) {
                    ar->arSkipScan = FALSE;
                } else {
                    ar->arSkipScan = TRUE;
                }

                if (wmi_scanparams_cmd(ar->arWmi, scParams.fg_start_period,
                                       scParams.fg_end_period,
                                       scParams.bg_period,
                                       scParams.minact_chdwell_time,
                                       scParams.maxact_chdwell_time,
                                       scParams.pas_chdwell_time,
                                       scParams.shortScanRatio,
                                       scParams.scanCtrlFlags,
                                       scParams.max_dfsch_act_time) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SETLISTENINT:
        {
            WMI_LISTEN_INT_CMD listenCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&listenCmd, userdata,
                                      sizeof(listenCmd)))
            {
                ret = -EFAULT;
            } else {
                    if (wmi_listeninterval_cmd(ar->arWmi, listenCmd.listenInterval, listenCmd.numBeacons) != A_OK) {
                        ret = -EIO;
                    } else {
                        AR6000_SPIN_LOCK(&ar->arLock, 0);
                        ar->arListenInterval = param;
                        AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                    }

                }
            break;
        }
        case AR6000_IOCTL_WMI_SET_BMISS_TIME:
        {
            WMI_BMISS_TIME_CMD bmissCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&bmissCmd, userdata,
                                      sizeof(bmissCmd)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_bmisstime_cmd(ar->arWmi, bmissCmd.bmissTime, bmissCmd.numBeacons) != A_OK) {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SETBSSFILTER:
        {
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else {

                get_user(param, (unsigned char *)userdata);
                get_user(param2, (unsigned int *)(userdata + 1));
		printk("SETBSSFILTER: filter 0x%x, mask: 0x%x\n", param, param2);
                if (wmi_bssfilter_cmd(ar->arWmi, param, param2) != A_OK) {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SET_SNRTHRESHOLD:
        {
            ret = ar6000_ioctl_set_snr_threshold(dev, rq);
            break;
        }
        case AR6000_XIOCTL_WMI_SET_RSSITHRESHOLD:
        {
            ret = ar6000_ioctl_set_rssi_threshold(dev, rq);
            break;
        }
        case AR6000_XIOCTL_WMI_CLR_RSSISNR:
        {
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            }
            ret = wmi_clr_rssi_snr(ar->arWmi);
            break;
        }
        case AR6000_XIOCTL_WMI_SET_LQTHRESHOLD:
        {
            ret = ar6000_ioctl_set_lq_threshold(dev, rq);
            break;
        }
        case AR6000_XIOCTL_WMI_SET_LPREAMBLE:
        {
            WMI_SET_LPREAMBLE_CMD setLpreambleCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setLpreambleCmd, userdata,
                                   sizeof(setLpreambleCmd)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_lpreamble_cmd(ar->arWmi, setLpreambleCmd.status)
                       != A_OK)
                {
                    ret = -EIO;
                }
            }

            break;
        }
        case AR6000_XIOCTL_WMI_SET_RTS:
        {
            WMI_SET_RTS_CMD rtsCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&rtsCmd, userdata,
                                   sizeof(rtsCmd)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_rts_cmd(ar->arWmi, rtsCmd.threshold)
                       != A_OK)
                {
                    ret = -EIO;
                }
            }

            break;
        }
        case AR6000_XIOCTL_WMI_SET_WMM:
        {
            ret = ar6000_ioctl_set_wmm(dev, rq);
            break;
        }
        case AR6000_XIOCTL_WMI_SET_TXOP:
        {
            ret = ar6000_ioctl_set_txop(dev, rq);
            break;
        }
        case AR6000_XIOCTL_WMI_GET_RD:
        {
            ret = ar6000_ioctl_get_rd(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_CHANNELPARAMS:
        {
            ret = ar6000_ioctl_set_channelParams(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_PROBEDSSID:
        {
            ret = ar6000_ioctl_set_probedSsid(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_BADAP:
        {
            ret = ar6000_ioctl_set_badAp(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_CREATE_QOS:
        {
            ret = ar6000_ioctl_create_qos(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_DELETE_QOS:
        {
            ret = ar6000_ioctl_delete_qos(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_GET_QOS_QUEUE:
        {
            ret = ar6000_ioctl_get_qos_queue(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_GET_TARGET_STATS:
        {
            ret = ar6000_ioctl_get_target_stats(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_ERROR_REPORT_BITMASK:
        {
            ret = ar6000_ioctl_set_error_report_bitmask(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_ASSOC_INFO:
        {
            WMI_SET_ASSOC_INFO_CMD cmd;
            A_UINT8 assocInfo[WMI_MAX_ASSOC_INFO_LEN];

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else {
                get_user(cmd.ieType, userdata);
                if (cmd.ieType >= WMI_MAX_ASSOC_INFO_TYPE) {
                    ret = -EIO;
                } else {
                    get_user(cmd.bufferSize, userdata + 1);
                    if (cmd.bufferSize > WMI_MAX_ASSOC_INFO_LEN) {
                        ret = -EFAULT;
                        break;
                    }
                    if (copy_from_user(assocInfo, userdata + 2,
                                       cmd.bufferSize))
                    {
                        ret = -EFAULT;
                    } else {
                        if (wmi_associnfo_cmd(ar->arWmi, cmd.ieType,
                                                 cmd.bufferSize,
                                                 assocInfo) != A_OK)
                        {
                            ret = -EIO;
                        }
                    }
                }
            }
            break;
        }
        case AR6000_IOCTL_WMI_SET_ACCESS_PARAMS:
        {
            ret = ar6000_ioctl_set_access_params(dev, rq);
            break;
        }
        case AR6000_IOCTL_WMI_SET_DISC_TIMEOUT:
        {
            ret = ar6000_ioctl_set_disconnect_timeout(dev, rq);
            break;
        }
        case AR6000_XIOCTL_FORCE_TARGET_RESET:
        {
            if (ar->arHtcTarget)
            {
//                HTCForceReset(htcTarget);
            }
            else
            {
                AR_DEBUG_PRINTF("ar6000_ioctl cannot attempt reset.\n");
            }
            break;
        }
        case AR6000_XIOCTL_TARGET_INFO:
        case AR6000_XIOCTL_CHECK_TARGET_READY: /* backwards compatibility */
        {
            /* If we made it to here, then the Target exists and is ready. */

            if (cmd == AR6000_XIOCTL_TARGET_INFO) {
                if (copy_to_user((A_UINT32 *)rq->ifr_data, &ar->arVersion.target_ver,
                                 sizeof(ar->arVersion.target_ver)))
                {
                    ret = -EFAULT;
                }
                if (copy_to_user(((A_UINT32 *)rq->ifr_data)+1, &ar->arTargetType,
                                 sizeof(ar->arTargetType)))
                {
                    ret = -EFAULT;
                }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_HB_CHALLENGE_RESP_PARAMS:
        {
            WMI_SET_HB_CHALLENGE_RESP_PARAMS_CMD hbparam;

            if (copy_from_user(&hbparam, userdata, sizeof(hbparam)))
            {
                ret = -EFAULT;
            } else {
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                /* Start a cyclic timer with the parameters provided. */
                if (hbparam.frequency) {
                    ar->arHBChallengeResp.frequency = hbparam.frequency;
                }
                if (hbparam.threshold) {
                    ar->arHBChallengeResp.missThres = hbparam.threshold;
                }

                /* Delete the pending timer and start a new one */
                if (timer_pending(&ar->arHBChallengeResp.timer)) {
                    A_UNTIMEOUT(&ar->arHBChallengeResp.timer);
                }
                A_TIMEOUT_MS(&ar->arHBChallengeResp.timer, ar->arHBChallengeResp.frequency * 1000, 0);
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            }
            break;
        }
        case AR6000_XIOCTL_WMI_GET_HB_CHALLENGE_RESP:
        {
            A_UINT32 cookie;

            if (copy_from_user(&cookie, userdata, sizeof(cookie))) {
                return -EFAULT;
            }

            /* Send the challenge on the control channel */
            if (wmi_get_challenge_resp_cmd(ar->arWmi, cookie, APP_HB_CHALLENGE) != A_OK) {
                return -EIO;
            }
            break;
        }
#ifdef USER_KEYS
        case AR6000_XIOCTL_USER_SETKEYS:
        {

            ar->user_savedkeys_stat = USER_SAVEDKEYS_STAT_RUN;

            if (copy_from_user(&ar->user_key_ctrl, userdata,
                               sizeof(ar->user_key_ctrl)))
            {
                return -EFAULT;
            }

            A_PRINTF("ar6000 USER set key %x\n", ar->user_key_ctrl);
            break;
        }
#endif /* USER_KEYS */

#ifdef CONFIG_HOST_GPIO_SUPPORT
        case AR6000_XIOCTL_GPIO_OUTPUT_SET:
        {
            struct ar6000_gpio_output_set_cmd_s gpio_output_set_cmd;

            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }

            if (copy_from_user(&gpio_output_set_cmd, userdata,
                                sizeof(gpio_output_set_cmd)))
            {
                ret = -EFAULT;
            } else {
                ret = ar6000_gpio_output_set(dev,
                                             gpio_output_set_cmd.set_mask,
                                             gpio_output_set_cmd.clear_mask,
                                             gpio_output_set_cmd.enable_mask,
                                             gpio_output_set_cmd.disable_mask);
                if (ret != A_OK) {
                    ret = EIO;
                }
            }
            up(&ar->arSem);
            break;
        }
        case AR6000_XIOCTL_GPIO_INPUT_GET:
        {
            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }

            ret = ar6000_gpio_input_get(dev);
            if (ret != A_OK) {
                up(&ar->arSem);
                return -EIO;
            }

            /* Wait for Target to respond. */
            wait_event_interruptible(arEvent, gpio_data_available);
            if (signal_pending(current)) {
                ret = -EINTR;
            } else {
                A_ASSERT(gpio_reg_results.gpioreg_id == GPIO_ID_NONE);

                if (copy_to_user(userdata, &gpio_reg_results.value,
                                 sizeof(gpio_reg_results.value)))
                {
                    ret = -EFAULT;
                }
            }
            up(&ar->arSem);
            break;
        }
        case AR6000_XIOCTL_GPIO_REGISTER_SET:
        {
            struct ar6000_gpio_register_cmd_s gpio_register_cmd;

            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }

            if (copy_from_user(&gpio_register_cmd, userdata,
                                sizeof(gpio_register_cmd)))
            {
                ret = -EFAULT;
            } else {
                ret = ar6000_gpio_register_set(dev,
                                               gpio_register_cmd.gpioreg_id,
                                               gpio_register_cmd.value);
                if (ret != A_OK) {
                    ret = EIO;
                }

                /* Wait for acknowledgement from Target */
                wait_event_interruptible(arEvent, gpio_ack_received);
                if (signal_pending(current)) {
                    ret = -EINTR;
                }
            }
            up(&ar->arSem);
            break;
        }
        case AR6000_XIOCTL_GPIO_REGISTER_GET:
        {
            struct ar6000_gpio_register_cmd_s gpio_register_cmd;

            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }

            if (copy_from_user(&gpio_register_cmd, userdata,
                                sizeof(gpio_register_cmd)))
            {
                ret = -EFAULT;
            } else {
                ret = ar6000_gpio_register_get(dev, gpio_register_cmd.gpioreg_id);
                if (ret != A_OK) {
                    up(&ar->arSem);
                    return -EIO;
                }

                /* Wait for Target to respond. */
                wait_event_interruptible(arEvent, gpio_data_available);
                if (signal_pending(current)) {
                    ret = -EINTR;
                } else {
                    A_ASSERT(gpio_register_cmd.gpioreg_id == gpio_reg_results.gpioreg_id);
                    if (copy_to_user(userdata, &gpio_reg_results,
                                     sizeof(gpio_reg_results)))
                    {
                        ret = -EFAULT;
                    }
                }
            }
            up(&ar->arSem);
            break;
        }
        case AR6000_XIOCTL_GPIO_INTR_ACK:
        {
            struct ar6000_gpio_intr_ack_cmd_s gpio_intr_ack_cmd;

            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }

            if (copy_from_user(&gpio_intr_ack_cmd, userdata,
                                sizeof(gpio_intr_ack_cmd)))
            {
                ret = -EFAULT;
            } else {
                ret = ar6000_gpio_intr_ack(dev, gpio_intr_ack_cmd.ack_mask);
                if (ret != A_OK) {
                    ret = EIO;
                }
            }
            up(&ar->arSem);
            break;
        }
        case AR6000_XIOCTL_GPIO_INTR_WAIT:
        {
            /* Wait for Target to report an interrupt. */
            dev_hold(dev);
            rtnl_unlock();
            wait_event_interruptible(arEvent, gpio_intr_available);
            rtnl_lock();
            __dev_put(dev);

            if (signal_pending(current)) {
                ret = -EINTR;
            } else {
                if (copy_to_user(userdata, &gpio_intr_results,
                                 sizeof(gpio_intr_results)))
                {
                    ret = -EFAULT;
                }
            }
            break;
        }
#endif /* CONFIG_HOST_GPIO_SUPPORT */

        case AR6000_XIOCTL_DBGLOG_CFG_MODULE:
        {
            struct ar6000_dbglog_module_config_s config;

            if (copy_from_user(&config, userdata, sizeof(config))) {
                return -EFAULT;
            }

            /* Send the challenge on the control channel */
            if (wmi_config_debug_module_cmd(ar->arWmi, config.mmask,
                                            config.tsr, config.rep,
                                            config.size, config.valid) != A_OK)
            {
                return -EIO;
            }
            break;
        }

        case AR6000_XIOCTL_DBGLOG_GET_DEBUG_LOGS:
        {
            /* Send the challenge on the control channel */
            if (ar6000_dbglog_get_debug_logs(ar) != A_OK)
            {
                return -EIO;
            }
            break;
        }

        case AR6000_XIOCTL_SET_ADHOC_BSSID:
        {
            WMI_SET_ADHOC_BSSID_CMD adhocBssid;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&adhocBssid, userdata,
                                      sizeof(adhocBssid)))
            {
                ret = -EFAULT;
            } else if (A_MEMCMP(adhocBssid.bssid, bcast_mac,
                                AR6000_ETH_ADDR_LEN) == 0)
            {
                ret = -EFAULT;
            } else {

                A_MEMCPY(ar->arReqBssid, adhocBssid.bssid, sizeof(ar->arReqBssid));
        }
            break;
        }

        case AR6000_XIOCTL_SET_OPT_MODE:
        {
        WMI_SET_OPT_MODE_CMD optModeCmd;
            AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&optModeCmd, userdata,
                                      sizeof(optModeCmd)))
            {
                ret = -EFAULT;
            } else if (ar->arConnected && optModeCmd.optMode == SPECIAL_ON) {
                ret = -EFAULT;

            } else if (wmi_set_opt_mode_cmd(ar->arWmi, optModeCmd.optMode)
                       != A_OK)
            {
                ret = -EIO;
            }
            break;
        }

        case AR6000_XIOCTL_OPT_SEND_FRAME:
        {
        WMI_OPT_TX_FRAME_CMD optTxFrmCmd;
            A_UINT8 data[MAX_OPT_DATA_LEN];

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&optTxFrmCmd, userdata,
                                      sizeof(optTxFrmCmd)))
            {
                ret = -EFAULT;
            } else if (copy_from_user(data,
                                      userdata+sizeof(WMI_OPT_TX_FRAME_CMD)-1,
                                      optTxFrmCmd.optIEDataLen))
            {
                ret = -EFAULT;
            } else {
                ret = wmi_opt_tx_frame_cmd(ar->arWmi,
                                           optTxFrmCmd.frmType,
                                           optTxFrmCmd.dstAddr,
                                           optTxFrmCmd.bssid,
                                           optTxFrmCmd.optIEDataLen,
                                           data);
            }

            break;
        }
        case AR6000_XIOCTL_WMI_SETRETRYLIMITS:
        {
            WMI_SET_RETRY_LIMITS_CMD setRetryParams;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setRetryParams, userdata,
                                      sizeof(setRetryParams)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_retry_limits_cmd(ar->arWmi, setRetryParams.frameType,
                                          setRetryParams.trafficClass,
                                          setRetryParams.maxRetries,
                                          setRetryParams.enableNotify) != A_OK)
                {
                    ret = -EIO;
                }
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                ar->arMaxRetries = setRetryParams.maxRetries;
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            }
            break;
        }

        case AR6000_XIOCTL_SET_ADHOC_BEACON_INTVAL:
        {
            WMI_BEACON_INT_CMD bIntvlCmd;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&bIntvlCmd, userdata,
                       sizeof(bIntvlCmd)))
            {
                ret = -EFAULT;
            } else if (wmi_set_adhoc_bconIntvl_cmd(ar->arWmi, bIntvlCmd.beaconInterval)
                        != A_OK)
            {
                ret = -EIO;
            }
            break;
        }
        case IEEE80211_IOCTL_SETAUTHALG:
        {
            AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
            struct ieee80211req_authalg req;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&req, userdata,
                       sizeof(struct ieee80211req_authalg)))
            {
                ret = -EFAULT;
            } else if (req.auth_alg == AUTH_ALG_OPEN_SYSTEM) {
                ar->arDot11AuthMode  = OPEN_AUTH;
                ar->arPairwiseCrypto  = NONE_CRYPT;
                ar->arGroupCrypto     = NONE_CRYPT;
            } else if (req.auth_alg == AUTH_ALG_LEAP) {
                ar->arDot11AuthMode   = LEAP_AUTH;
            } else {
                ret = -EIO;
            }
            break;
        }

        case AR6000_XIOCTL_SET_VOICE_PKT_SIZE:
            ret = ar6000_xioctl_set_voice_pkt_size(dev, userdata);
            break;

        case AR6000_XIOCTL_SET_MAX_SP:
            ret = ar6000_xioctl_set_max_sp_len(dev, userdata);
            break;

        case AR6000_XIOCTL_WMI_GET_ROAM_TBL:
            ret = ar6000_ioctl_get_roam_tbl(dev, rq);
            break;
        case AR6000_XIOCTL_WMI_SET_ROAM_CTRL:
            ret = ar6000_ioctl_set_roam_ctrl(dev, userdata);
            break;
        case AR6000_XIOCTRL_WMI_SET_POWERSAVE_TIMERS:
            ret = ar6000_ioctl_set_powersave_timers(dev, userdata);
            break;
        case AR6000_XIOCTRL_WMI_GET_POWER_MODE:
            ret = ar6000_ioctl_get_power_mode(dev, rq);
            break;
        case AR6000_XIOCTRL_WMI_SET_WLAN_STATE:
            get_user(ar->arWlanState, (unsigned int *)userdata);
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
                break;
            }

            if (ar->arWlanState == WLAN_ENABLED) {
                /* Enable foreground scanning */
                if (wmi_scanparams_cmd(ar->arWmi, scParams.fg_start_period,
                                       scParams.fg_end_period,
                                       scParams.bg_period,
                                       scParams.minact_chdwell_time,
                                       scParams.maxact_chdwell_time,
                                       scParams.pas_chdwell_time,
                                       scParams.shortScanRatio,
                                       scParams.scanCtrlFlags,
                                       scParams.max_dfsch_act_time) != A_OK)
                {
                    ret = -EIO;
                }
                if (ar->arSsidLen) {
                    ar->arConnectPending = TRUE;
                    if (wmi_connect_cmd(ar->arWmi, ar->arNetworkType,
                                        ar->arDot11AuthMode, ar->arAuthMode,
                                        ar->arPairwiseCrypto,
                                        ar->arPairwiseCryptoLen,
                                        ar->arGroupCrypto, ar->arGroupCryptoLen,
                                        ar->arSsidLen, ar->arSsid,
                                        ar->arReqBssid, ar->arChannelHint,
                                        ar->arConnectCtrlFlags) != A_OK)
                    {
                        ret = -EIO;
                        ar->arConnectPending = FALSE;
                    }
                }
            } else {
                /* Disconnect from the AP and disable foreground scanning */
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                if (ar->arConnected == TRUE || ar->arConnectPending == TRUE) {
                    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                    wmi_disconnect_cmd(ar->arWmi);
                } else {
                    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                }

                if (wmi_scanparams_cmd(ar->arWmi, 0xFFFF, 0, 0, 0, 0, 0, 0, 0xFF, 0) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        case AR6000_XIOCTL_WMI_GET_ROAM_DATA:
            ret = ar6000_ioctl_get_roam_data(dev, rq);
            break;
        case AR6000_XIOCTL_WMI_SET_BT_STATUS:
            ret = ar6000_xioctl_set_bt_status_cmd(dev, userdata);
            break;
        case AR6000_XIOCTL_WMI_SET_BT_PARAMS:
            ret = ar6000_xioctl_set_bt_params_cmd(dev, userdata);
            break;
        case AR6000_XIOCTL_WMI_STARTSCAN:
        {
            WMI_START_SCAN_CMD setStartScanCmd;

            if (ar->arWmiReady == FALSE) {
                    ret = -EIO;
                } else if (copy_from_user(&setStartScanCmd, userdata,
                                          sizeof(setStartScanCmd)))
                {
                    ret = -EFAULT;
                } else {
                    if (wmi_startscan_cmd(ar->arWmi, setStartScanCmd.scanType,
                                    setStartScanCmd.forceFgScan,
                                    setStartScanCmd.isLegacy,
                                    setStartScanCmd.homeDwellTime,
                                    setStartScanCmd.forceScanInterval) != A_OK)
                    {
                        ret = -EIO;
                    }
                }
            break;
        }
        case AR6000_XIOCTL_WMI_SETFIXRATES:
        {
            WMI_FIX_RATES_CMD setFixRatesCmd;
            A_STATUS returnStatus;

            if (ar->arWmiReady == FALSE) {
                    ret = -EIO;
                } else if (copy_from_user(&setFixRatesCmd, userdata,
                                          sizeof(setFixRatesCmd)))
                {
                    ret = -EFAULT;
                } else {
                    returnStatus = wmi_set_fixrates_cmd(ar->arWmi, setFixRatesCmd.fixRateMask);
                    if (returnStatus == A_EINVAL)
                    {
                        ret = -EINVAL;
                    }
                    else if(returnStatus != A_OK) {
                        ret = -EIO;
                    }
                }
            break;
        }

        case AR6000_XIOCTL_WMI_GETFIXRATES:
        {
            WMI_FIX_RATES_CMD getFixRatesCmd;
            AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
            int ret = 0;

            if (ar->arWmiReady == FALSE) {
                return -EIO;
            }

            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }
            /* Used copy_from_user/copy_to_user to access user space data */
            if (copy_from_user(&getFixRatesCmd, userdata, sizeof(getFixRatesCmd))) {
                ret = -EFAULT;
            } else {
                ar->arRateMask = 0xFFFF;

                if (wmi_get_ratemask_cmd(ar->arWmi) != A_OK) {
                    up(&ar->arSem);
                    return -EIO;
                }

                wait_event_interruptible_timeout(arEvent, ar->arRateMask != 0xFFFF, wmitimeout * HZ);

                if (signal_pending(current)) {
                    ret = -EINTR;
                }

                if (!ret) {
                    getFixRatesCmd.fixRateMask = ar->arRateMask;
                }

                if(copy_to_user(userdata, &getFixRatesCmd, sizeof(getFixRatesCmd))) {
                   ret = -EFAULT;
                }

                up(&ar->arSem);
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_AUTHMODE:
        {
            WMI_SET_AUTH_MODE_CMD setAuthMode;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setAuthMode, userdata,
                                      sizeof(setAuthMode)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_authmode_cmd(ar->arWmi, setAuthMode.mode) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_REASSOCMODE:
        {
            WMI_SET_REASSOC_MODE_CMD setReassocMode;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setReassocMode, userdata,
                                      sizeof(setReassocMode)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_reassocmode_cmd(ar->arWmi, setReassocMode.mode) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_DIAG_READ:
        {
            A_UINT32 addr, data;
            get_user(addr, (unsigned int *)userdata);
            if (ar6000_ReadRegDiag(ar->arHifDevice, &addr, &data) != A_OK) {
                ret = -EIO;
            }
            put_user(data, (unsigned int *)userdata + 1);
            break;
        }
        case AR6000_XIOCTL_DIAG_WRITE:
        {
            A_UINT32 addr, data;
            get_user(addr, (unsigned int *)userdata);
            get_user(data, (unsigned int *)userdata + 1);
            if (ar6000_WriteRegDiag(ar->arHifDevice, &addr, &data) != A_OK) {
                ret = -EIO;
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_KEEPALIVE:
        {
             WMI_SET_KEEPALIVE_CMD setKeepAlive;
             if (ar->arWmiReady == FALSE) {
                 return -EIO;
             } else if (copy_from_user(&setKeepAlive, userdata,
                        sizeof(setKeepAlive))){
                 ret = -EFAULT;
             } else {
                 if (wmi_set_keepalive_cmd(ar->arWmi, setKeepAlive.keepaliveInterval) != A_OK) {
                     ret = -EIO;
               }
             }
             break;
        }
        case AR6000_XIOCTL_WMI_GET_KEEPALIVE:
        {
            AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
            WMI_GET_KEEPALIVE_CMD getKeepAlive;
            int ret = 0;
            if (ar->arWmiReady == FALSE) {
               return -EIO;
            }
            if (down_interruptible(&ar->arSem)) {
                return -ERESTARTSYS;
            }
            if (copy_from_user(&getKeepAlive, userdata,sizeof(getKeepAlive))) {
               ret = -EFAULT;
            } else {
            getKeepAlive.keepaliveInterval = wmi_get_keepalive_cmd(ar->arWmi);
            ar->arKeepaliveConfigured = 0xFF;
            if (wmi_get_keepalive_configured(ar->arWmi) != A_OK){
                up(&ar->arSem);
                return -EIO;
            }
            wait_event_interruptible_timeout(arEvent, ar->arKeepaliveConfigured != 0xFF, wmitimeout * HZ);
            if (signal_pending(current)) {
                ret = -EINTR;
            }

            if (!ret) {
                getKeepAlive.configured = ar->arKeepaliveConfigured;
            }
            if (copy_to_user(userdata, &getKeepAlive, sizeof(getKeepAlive))) {
               ret = -EFAULT;
            }
            up(&ar->arSem);
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_APPIE:
        {
            WMI_SET_APPIE_CMD appIEcmd;
            A_UINT8           appIeInfo[IEEE80211_APPIE_FRAME_MAX_LEN];
            A_UINT32            fType,ieLen;

	    if (ar->arWmiReady == FALSE) {
		    return -EIO;
	    }
	    get_user(fType, (A_UINT32 *)userdata);
	    appIEcmd.mgmtFrmType = fType;
	    if (appIEcmd.mgmtFrmType >= IEEE80211_APPIE_NUM_OF_FRAME) {
		    ret = -EIO;
	    } else {
		    get_user(ieLen, (A_UINT32 *)(userdata + 4));
		    appIEcmd.ieLen = ieLen;
		    if (appIEcmd.ieLen > IEEE80211_APPIE_FRAME_MAX_LEN) {
			    ret = -EIO;
			    break;
		    }
		    if (copy_from_user(appIeInfo, userdata + 8, appIEcmd.ieLen)) {
			    ret = -EFAULT;
		    } else {
			    if (wmi_set_appie_cmd(ar->arWmi, appIEcmd.mgmtFrmType,
						  appIEcmd.ieLen,  appIeInfo) != A_OK)
			    {
				    ret = -EIO;
			    }
		    }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_MGMT_FRM_RX_FILTER:
        {
            WMI_BSS_FILTER_CMD cmd;
            A_UINT32    filterType;

            if (copy_from_user(&filterType, userdata, sizeof(A_UINT32)))
            {
                return -EFAULT;
            }
            if (filterType & (IEEE80211_FILTER_TYPE_BEACON |
                                    IEEE80211_FILTER_TYPE_PROBE_RESP))
            {
                cmd.bssFilter = ALL_BSS_FILTER;
            } else {
                cmd.bssFilter = NONE_BSS_FILTER;
            }
            if (wmi_bssfilter_cmd(ar->arWmi, cmd.bssFilter, 0) != A_OK) {
                ret = -EIO;
            }

            AR6000_SPIN_LOCK(&ar->arLock, 0);
            ar->arMgmtFilter = filterType;
            AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            break;
        }
        case AR6000_XIOCTL_WMI_SET_WSC_STATUS:
        {
            A_UINT32    wsc_status;

            if (copy_from_user(&wsc_status, userdata, sizeof(A_UINT32)))
            {
                return -EFAULT;
            }
            if (wmi_set_wsc_status_cmd(ar->arWmi, wsc_status) != A_OK) {
                ret = -EIO;
            }
            break;
        }
        case AR6000_XIOCTL_BMI_ROMPATCH_INSTALL:
        {
            A_UINT32 ROM_addr;
            A_UINT32 RAM_addr;
            A_UINT32 nbytes;
            A_UINT32 do_activate;
            A_UINT32 rompatch_id;

            get_user(ROM_addr, (A_UINT32 *)userdata);
            get_user(RAM_addr, (A_UINT32 *)userdata + 1);
            get_user(nbytes, (A_UINT32 *)userdata + 2);
            get_user(do_activate, (A_UINT32 *)userdata + 3);
            AR_DEBUG_PRINTF("Install rompatch from ROM: 0x%x to RAM: 0x%x  length: %d\n",
                             ROM_addr, RAM_addr, nbytes);
            ret = BMIrompatchInstall(hifDevice, ROM_addr, RAM_addr,
                                        nbytes, do_activate, &rompatch_id);
            if (ret == A_OK) {
                put_user(rompatch_id, (unsigned int *)rq->ifr_data); /* return value */
            }
            break;
        }

        case AR6000_XIOCTL_BMI_ROMPATCH_UNINSTALL:
        {
            A_UINT32 rompatch_id;

            get_user(rompatch_id, (A_UINT32 *)userdata);
            AR_DEBUG_PRINTF("UNinstall rompatch_id %d\n", rompatch_id);
            ret = BMIrompatchUninstall(hifDevice, rompatch_id);
            break;
        }

        case AR6000_XIOCTL_BMI_ROMPATCH_ACTIVATE:
        case AR6000_XIOCTL_BMI_ROMPATCH_DEACTIVATE:
        {
            A_UINT32 rompatch_count;

            get_user(rompatch_count, (A_UINT32 *)userdata);
            AR_DEBUG_PRINTF("Change rompatch activation count=%d\n", rompatch_count);
            length = sizeof(A_UINT32) * rompatch_count;
            if ((buffer = (unsigned char *)A_MALLOC(length)) != NULL) {
                A_MEMZERO(buffer, length);
                if (copy_from_user(buffer, &userdata[sizeof(rompatch_count)], length))
                {
                    ret = -EFAULT;
                } else {
                    if (cmd == AR6000_XIOCTL_BMI_ROMPATCH_ACTIVATE) {
                        ret = BMIrompatchActivate(hifDevice, rompatch_count, (A_UINT32 *)buffer);
                    } else {
                        ret = BMIrompatchDeactivate(hifDevice, rompatch_count, (A_UINT32 *)buffer);
                    }
                }
                A_FREE(buffer);
            } else {
                ret = -ENOMEM;
            }

            break;
        }

        case AR6000_XIOCTL_WMI_SET_HOST_SLEEP_MODE:
        {
            WMI_SET_HOST_SLEEP_MODE_CMD setHostSleepMode;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setHostSleepMode, userdata,
                                      sizeof(setHostSleepMode)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_host_sleep_mode_cmd(ar->arWmi,
                                &setHostSleepMode) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_SET_WOW_MODE:
        {
            WMI_SET_WOW_MODE_CMD setWowMode;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&setWowMode, userdata,
                                      sizeof(setWowMode)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_wow_mode_cmd(ar->arWmi,
                                &setWowMode) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_GET_WOW_LIST:
        {
            WMI_GET_WOW_LIST_CMD getWowList;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&getWowList, userdata,
                                      sizeof(getWowList)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_get_wow_list_cmd(ar->arWmi,
                                &getWowList) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_WMI_ADD_WOW_PATTERN:
        {
#define WOW_PATTERN_SIZE 64
#define WOW_MASK_SIZE 64

            WMI_ADD_WOW_PATTERN_CMD cmd;
            A_UINT8 mask_data[WOW_PATTERN_SIZE]={0};
            A_UINT8 pattern_data[WOW_PATTERN_SIZE]={0};

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else {

                if(copy_from_user(&cmd, userdata,
                            sizeof(WMI_ADD_WOW_PATTERN_CMD)))
                      return -EFAULT;
                if (copy_from_user(pattern_data,
                                      userdata + 3,
                                      cmd.filter_size)){
                        ret = -EFAULT;
                        break;
                }
                if (copy_from_user(mask_data,
                                      (userdata + 3 + cmd.filter_size),
                                      cmd.filter_size)){
                        ret = -EFAULT;
                        break;
                } else {
                    if (wmi_add_wow_pattern_cmd(ar->arWmi,
                                &cmd, pattern_data, mask_data, cmd.filter_size) != A_OK){
                        ret = -EIO;
                    }
                }
            }
#undef WOW_PATTERN_SIZE
#undef WOW_MASK_SIZE
            break;
        }
        case AR6000_XIOCTL_WMI_DEL_WOW_PATTERN:
        {
            WMI_DEL_WOW_PATTERN_CMD delWowPattern;

            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&delWowPattern, userdata,
                                      sizeof(delWowPattern)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_del_wow_pattern_cmd(ar->arWmi,
                                &delWowPattern) != A_OK)
                {
                    ret = -EIO;
                }
            }
            break;
        }
        case AR6000_XIOCTL_DUMP_HTC_CREDIT_STATE:
            if (ar->arHtcTarget != NULL) {
                HTCDumpCreditStates(ar->arHtcTarget);
            }
            break;
        case AR6000_XIOCTL_TRAFFIC_ACTIVITY_CHANGE:
            if (ar->arHtcTarget != NULL) {
                struct ar6000_traffic_activity_change data;

                if (copy_from_user(&data, userdata, sizeof(data)))
                {
                    return -EFAULT;
                }
                    /* note, this is used for testing (mbox ping testing), indicate activity
                     * change using the stream ID as the traffic class */
                ar6000_indicate_tx_activity(ar,
                                            (A_UINT8)data.StreamID,
                                            data.Active ? TRUE : FALSE);
            }
            break;
        case AR6000_XIOCTL_WMI_SET_CONNECT_CTRL_FLAGS:
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&connectCtrlFlags, userdata,
                                      sizeof(connectCtrlFlags)))
            {
                ret = -EFAULT;
            } else {
                ar->arConnectCtrlFlags = connectCtrlFlags;
            }
            break;
        case AR6000_XIOCTL_WMI_SET_AKMP_PARAMS:
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else if (copy_from_user(&akmpParams, userdata,
                                      sizeof(WMI_SET_AKMP_PARAMS_CMD)))
            {
                ret = -EFAULT;
            } else {
                if (wmi_set_akmp_params_cmd(ar->arWmi, &akmpParams) != A_OK) {
                    ret = -EIO;
                }
            }
            break;
        case AR6000_XIOCTL_WMI_SET_PMKID_LIST:
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else {
                if (copy_from_user(&pmkidInfo.numPMKID, userdata,
                                      sizeof(pmkidInfo.numPMKID)))
                {
                    ret = -EFAULT;
                    break;
                }
                if (copy_from_user(&pmkidInfo.pmkidList,
                                   userdata + sizeof(pmkidInfo.numPMKID),
                                   pmkidInfo.numPMKID * sizeof(WMI_PMKID)))
                {
                    ret = -EFAULT;
                    break;
                }
                if (wmi_set_pmkid_list_cmd(ar->arWmi, &pmkidInfo) != A_OK) {
                    ret = -EIO;
                }
            }
            break;
        case AR6000_XIOCTL_WMI_GET_PMKID_LIST:
            if (ar->arWmiReady == FALSE) {
                ret = -EIO;
            } else  {
                if (wmi_get_pmkid_list_cmd(ar->arWmi) != A_OK) {
                    ret = -EIO;
                }
            }
            break;
        default:
            ret = -EOPNOTSUPP;
    }
    return ret;
}

