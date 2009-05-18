/*
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * This module implements the hardware independent layer of the
 * Wireless Module Interface (WMI) protocol.
 *
 * $Id: //depot/sw/releases/olca2.0-GPL/host/wmi/wmi.c#3 $
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

#include <a_config.h>
#include <athdefs.h>
#include <a_types.h>
#include <a_osapi.h>
#include "htc.h"
#include "htc_api.h"
#include "wmi.h"
#include <ieee80211.h>
#include <ieee80211_node.h>
#include <wlan_api.h>
#include <wmi_api.h>
#include "dset_api.h"
#include "gpio_api.h"
#include "wmi_host.h"
#include "a_drv.h"
#include "a_drv_api.h"
#include "a_debug.h"
#include "dbglog_api.h"

static A_STATUS wmi_ready_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_STATUS wmi_connect_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_disconnect_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                        int len);
static A_STATUS wmi_tkip_micerr_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                        int len);
static A_STATUS wmi_bssInfo_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_opt_frame_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                       int len);
static A_STATUS wmi_pstream_timeout_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_sync_point(struct wmi_t *wmip);

static A_STATUS wmi_bitrate_reply_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_ratemask_reply_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_channelList_reply_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                         int len);
static A_STATUS wmi_regDomain_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                       int len);
static A_STATUS wmi_txPwr_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_neighborReport_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                             int len);

static A_STATUS wmi_dset_open_req_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
#ifdef CONFIG_HOST_DSET_SUPPORT
static A_STATUS wmi_dset_close_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_dset_data_req_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
#endif /* CONFIG_HOST_DSET_SUPPORT */


static A_STATUS wmi_scanComplete_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                     int len);
static A_STATUS wmi_errorEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_statsEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_rssiThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_hbChallengeResp_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_reportErrorEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_cac_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_roam_tbl_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                      int len);
static A_STATUS wmi_roam_data_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                      int len);
static A_STATUS wmi_get_wow_list_event_rx(struct wmi_t *wmip, A_UINT8 *datap,
                                      int len);
static A_STATUS
wmi_get_pmkid_list_event_rx(struct wmi_t *wmip, A_UINT8 *datap, A_UINT32 len);

#ifdef CONFIG_HOST_GPIO_SUPPORT
static A_STATUS wmi_gpio_intr_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_gpio_data_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
static A_STATUS wmi_gpio_ack_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
#endif /* CONFIG_HOST_GPIO_SUPPORT */

#ifdef CONFIG_HOST_TCMD_SUPPORT
static A_STATUS
wmi_tcmd_test_report_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);
#endif

static A_STATUS
wmi_txRetryErrEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_STATUS
wmi_snrThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_STATUS
wmi_lqThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_BOOL
wmi_is_bitrate_index_valid(struct wmi_t *wmip, A_UINT32 rateIndex);

static A_STATUS
wmi_aplistEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_STATUS
wmi_dbglog_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

static A_STATUS wmi_keepalive_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len);

int wps_enable;
static const A_INT32 wmi_rateTable[] = {
    1000,
    2000,
    5500,
    11000,
    6000,
    9000,
    12000,
    18000,
    24000,
    36000,
    48000,
    54000,
    0};

#define MODE_A_SUPPORT_RATE_START       4
#define MODE_A_SUPPORT_RATE_STOP        11

#define MODE_GONLY_SUPPORT_RATE_START   MODE_A_SUPPORT_RATE_START
#define MODE_GONLY_SUPPORT_RATE_STOP    MODE_A_SUPPORT_RATE_STOP

#define MODE_B_SUPPORT_RATE_START       0
#define MODE_B_SUPPORT_RATE_STOP        3

#define MODE_G_SUPPORT_RATE_START       0
#define MODE_G_SUPPORT_RATE_STOP        11

#define MAX_NUMBER_OF_SUPPORT_RATES     (MODE_G_SUPPORT_RATE_STOP + 1)

/* 802.1d to AC mapping. Refer pg 57 of WMM-test-plan-v1.2 */
const A_UINT8 up_to_ac[]= {
                WMM_AC_BE,
                WMM_AC_BK,
                WMM_AC_BK,
                WMM_AC_BE,
                WMM_AC_VI,
                WMM_AC_VI,
                WMM_AC_VO,
                WMM_AC_VO,
            };

void *
wmi_init(void *devt)
{
    struct wmi_t *wmip;

    wmip = A_MALLOC(sizeof(struct wmi_t));
    if (wmip == NULL) {
        return (NULL);
    }
    A_MEMZERO(wmip, sizeof(*wmip));
    A_MUTEX_INIT(&wmip->wmi_lock);
    wmip->wmi_devt = devt;
    wlan_node_table_init(wmip, &wmip->wmi_scan_table);
    wmi_qos_state_init(wmip);
    wmip->wmi_powerMode = REC_POWER;
    wmip->wmi_phyMode = WMI_11G_MODE;

    return (wmip);
}

void
wmi_qos_state_init(struct wmi_t *wmip)
{
    A_UINT8 i;

    if (wmip == NULL) {
        return;
    }
    LOCK_WMI(wmip);

    /* Initialize QoS States */
    wmip->wmi_numQoSStream = 0;

    wmip->wmi_fatPipeExists = 0;

    for (i=0; i < WMM_NUM_AC; i++) {
        wmip->wmi_streamExistsForAC[i]=0;
    }

        /* Initialize the static Wmi stream Pri to WMM AC mappings Arrays */
    WMI_INIT_WMISTREAM_AC_MAP(wmip);

    UNLOCK_WMI(wmip);

    A_WMI_SET_NUMDATAENDPTS(wmip->wmi_devt, 1);
}

void
wmi_shutdown(struct wmi_t *wmip)
{
    if (wmip != NULL) {
        wlan_node_table_cleanup(&wmip->wmi_scan_table);
        if (A_IS_MUTEX_VALID(&wmip->wmi_lock)) {
            A_MUTEX_DELETE(&wmip->wmi_lock);
        }
        A_FREE(wmip);
    }
}

/*
 *  performs DIX to 802.3 encapsulation for transmit packets.
 *  uses passed in buffer.  Returns buffer or NULL if failed.
 *  Assumes the entire DIX header is contigous and that there is
 *  enough room in the buffer for a 802.3 mac header and LLC+SNAP headers.
 */
A_STATUS
wmi_dix_2_dot3(struct wmi_t *wmip, void *osbuf)
{
    A_UINT8          *datap;
    A_UINT16         typeorlen;
    ATH_MAC_HDR      macHdr;
    ATH_LLC_SNAP_HDR *llcHdr;

    A_ASSERT(osbuf != NULL);

    if (A_NETBUF_HEADROOM(osbuf) <
        (sizeof(ATH_LLC_SNAP_HDR) + sizeof(WMI_DATA_HDR)))
    {
        return A_NO_MEMORY;
    }

    datap = A_NETBUF_DATA(osbuf);

    typeorlen = *(A_UINT16 *)(datap + ATH_MAC_LEN + ATH_MAC_LEN);

    if (!IS_ETHERTYPE(A_BE2CPU16(typeorlen))) {
        /*
         * packet is already in 802.3 format - return success
         */
        A_DPRINTF(DBG_WMI, (DBGFMT "packet already 802.3\n", DBGARG));
        return (A_OK);
    }

    /*
     * Save mac fields and length to be inserted later
     */
    A_MEMCPY(macHdr.dstMac, datap, ATH_MAC_LEN);
    A_MEMCPY(macHdr.srcMac, datap + ATH_MAC_LEN, ATH_MAC_LEN);
    macHdr.typeOrLen = A_CPU2BE16(A_NETBUF_LEN(osbuf) - sizeof(ATH_MAC_HDR) +
                                  sizeof(ATH_LLC_SNAP_HDR));

    /*
     * Make room for LLC+SNAP headers
     */
    if (A_NETBUF_PUSH(osbuf, sizeof(ATH_LLC_SNAP_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    datap = A_NETBUF_DATA(osbuf);

    A_MEMCPY(datap, &macHdr, sizeof (ATH_MAC_HDR));

    llcHdr = (ATH_LLC_SNAP_HDR *)(datap + sizeof(ATH_MAC_HDR));
    llcHdr->dsap      = 0xAA;
    llcHdr->ssap      = 0xAA;
    llcHdr->cntl      = 0x03;
    llcHdr->orgCode[0] = 0x0;
    llcHdr->orgCode[1] = 0x0;
    llcHdr->orgCode[2] = 0x0;
    llcHdr->etherType = typeorlen;

    return (A_OK);
}

/*
 * Adds a WMI data header
 * Assumes there is enough room in the buffer to add header.
 */
A_STATUS
wmi_data_hdr_add(struct wmi_t *wmip, void *osbuf, A_UINT8 msgType)
{
    WMI_DATA_HDR     *dtHdr;

    A_ASSERT(osbuf != NULL);

    if (A_NETBUF_PUSH(osbuf, sizeof(WMI_DATA_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    dtHdr = (WMI_DATA_HDR *)A_NETBUF_DATA(osbuf);
    dtHdr->info = msgType;
    dtHdr->rssi = 0;

    return (A_OK);
}

A_UINT8 wmi_implicit_create_pstream(struct wmi_t *wmip, void *osbuf, A_UINT8 dir, A_UINT8 up)
{
    A_UINT8         *datap;
    A_UINT8         trafficClass = WMM_AC_BE, userPriority = up;
    ATH_LLC_SNAP_HDR *llcHdr;
    A_UINT16        ipType = IP_ETHERTYPE;
    WMI_DATA_HDR     *dtHdr;
    WMI_CREATE_PSTREAM_CMD  cmd;
    A_BOOL           streamExists = FALSE;

    A_ASSERT(osbuf != NULL);

    datap = A_NETBUF_DATA(osbuf);

    if (up == UNDEFINED_PRI) {
    llcHdr = (ATH_LLC_SNAP_HDR *)(datap + sizeof(WMI_DATA_HDR) +
                                  sizeof(ATH_MAC_HDR));

        if (llcHdr->etherType == A_CPU2BE16(ipType)) {
        /* Extract the endpoint info from the TOS field in the IP header */
        userPriority = A_WMI_IPTOS_TO_USERPRIORITY(((A_UINT8 *)llcHdr) + sizeof(ATH_LLC_SNAP_HDR));
        }
    }

    if (userPriority < MAX_NUM_PRI) {
        trafficClass = convert_userPriority_to_trafficClass(userPriority);
    }

    dtHdr = (WMI_DATA_HDR *)datap;
    if(dir==UPLINK_TRAFFIC)
        dtHdr->info |= (userPriority & WMI_DATA_HDR_UP_MASK) << WMI_DATA_HDR_UP_SHIFT;  /* lower 3-bits are 802.1d priority */

    LOCK_WMI(wmip);
    streamExists = wmip->wmi_fatPipeExists;
    UNLOCK_WMI(wmip);

    if (!(streamExists & (1 << trafficClass))) {

        A_MEMZERO(&cmd, sizeof(cmd));
	    cmd.trafficClass = trafficClass;
	    cmd.userPriority = userPriority;
		cmd.inactivityInt = WMI_IMPLICIT_PSTREAM_INACTIVITY_INT;
            /* Implicit streams are created with TSID 0xFF */
        cmd.tsid = WMI_IMPLICIT_PSTREAM;
        wmi_create_pstream_cmd(wmip, &cmd);
    }

    return trafficClass;
}

WMI_PRI_STREAM_ID
wmi_get_stream_id(struct wmi_t *wmip, A_UINT8 trafficClass)
{
    return WMI_ACCESSCATEGORY_WMISTREAM(wmip, trafficClass);
}

/*
 *  performs 802.3 to DIX encapsulation for received packets.
 *  Assumes the entire 802.3 header is contigous.
 */
A_STATUS
wmi_dot3_2_dix(struct wmi_t *wmip, void *osbuf)
{
    A_UINT8          *datap;
    ATH_MAC_HDR      macHdr;
    ATH_LLC_SNAP_HDR *llcHdr;

    A_ASSERT(osbuf != NULL);
    datap = A_NETBUF_DATA(osbuf);

    A_MEMCPY(&macHdr, datap, sizeof(ATH_MAC_HDR));
    llcHdr = (ATH_LLC_SNAP_HDR *)(datap + sizeof(ATH_MAC_HDR));
    macHdr.typeOrLen = llcHdr->etherType;

    if (A_NETBUF_PULL(osbuf, sizeof(ATH_LLC_SNAP_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    datap = A_NETBUF_DATA(osbuf);

    A_MEMCPY(datap, &macHdr, sizeof (ATH_MAC_HDR));

    return (A_OK);
}

/*
 * Removes a WMI data header
 */
A_STATUS
wmi_data_hdr_remove(struct wmi_t *wmip, void *osbuf)
{
    A_ASSERT(osbuf != NULL);

    return (A_NETBUF_PULL(osbuf, sizeof(WMI_DATA_HDR)));
}

void
wmi_iterate_nodes(struct wmi_t *wmip, wlan_node_iter_func *f, void *arg)
{
    wlan_iterate_nodes(&wmip->wmi_scan_table, f, arg);
}

/*
 * WMI Extended Event received from Target.
 */
A_STATUS
wmi_control_rx_xtnd(struct wmi_t *wmip, void *osbuf)
{
    WMIX_CMD_HDR *cmd;
    A_UINT16 id;
    A_UINT8 *datap;
    A_UINT32 len;
    A_STATUS status = A_OK;

    if (A_NETBUF_LEN(osbuf) < sizeof(WMIX_CMD_HDR)) {
        A_DPRINTF(DBG_WMI, (DBGFMT "bad packet 1\n", DBGARG));
        wmip->wmi_stats.cmd_len_err++;
        A_NETBUF_FREE(osbuf);
        return A_ERROR;
    }

    cmd = (WMIX_CMD_HDR *)A_NETBUF_DATA(osbuf);
    id = cmd->commandId;

    if (A_NETBUF_PULL(osbuf, sizeof(WMIX_CMD_HDR)) != A_OK) {
        A_DPRINTF(DBG_WMI, (DBGFMT "bad packet 2\n", DBGARG));
        wmip->wmi_stats.cmd_len_err++;
        A_NETBUF_FREE(osbuf);
        return A_ERROR;
    }

    datap = A_NETBUF_DATA(osbuf);
    len = A_NETBUF_LEN(osbuf);

    switch (id) {
    case (WMIX_DSETOPENREQ_EVENTID):
        status = wmi_dset_open_req_rx(wmip, datap, len);
        break;
#ifdef CONFIG_HOST_DSET_SUPPORT
    case (WMIX_DSETCLOSE_EVENTID):
        status = wmi_dset_close_rx(wmip, datap, len);
        break;
    case (WMIX_DSETDATAREQ_EVENTID):
        status = wmi_dset_data_req_rx(wmip, datap, len);
        break;
#endif /* CONFIG_HOST_DSET_SUPPORT */
#ifdef CONFIG_HOST_GPIO_SUPPORT
    case (WMIX_GPIO_INTR_EVENTID):
        wmi_gpio_intr_rx(wmip, datap, len);
        break;
    case (WMIX_GPIO_DATA_EVENTID):
        wmi_gpio_data_rx(wmip, datap, len);
        break;
    case (WMIX_GPIO_ACK_EVENTID):
        wmi_gpio_ack_rx(wmip, datap, len);
        break;
#endif /* CONFIG_HOST_GPIO_SUPPORT */
    case (WMIX_HB_CHALLENGE_RESP_EVENTID):
        wmi_hbChallengeResp_rx(wmip, datap, len);
        break;
    case (WMIX_DBGLOG_EVENTID):
        wmi_dbglog_event_rx(wmip, datap, len);
        break;
    default:
        A_DPRINTF(DBG_WMI|DBG_ERROR,
            (DBGFMT "Unknown id 0x%x\n", DBGARG, id));
        wmip->wmi_stats.cmd_id_err++;
        status = A_ERROR;
        break;
    }

    return status;
}

/*
 * Control Path
 */
A_UINT32 cmdRecvNum;

A_STATUS
wmi_control_rx(struct wmi_t *wmip, void *osbuf)
{
    WMI_CMD_HDR *cmd;
    A_UINT16 id;
    A_UINT8 *datap;
    A_UINT32 len, i, loggingReq;
    A_STATUS status = A_OK;

    A_ASSERT(osbuf != NULL);
    if (A_NETBUF_LEN(osbuf) < sizeof(WMI_CMD_HDR)) {
        A_DPRINTF(DBG_WMI, (DBGFMT "bad packet 1\n", DBGARG));
        wmip->wmi_stats.cmd_len_err++;
        A_NETBUF_FREE(osbuf);
        return A_ERROR;
    }

    cmd = (WMI_CMD_HDR *)A_NETBUF_DATA(osbuf);
    id = cmd->commandId;

    if (A_NETBUF_PULL(osbuf, sizeof(WMI_CMD_HDR)) != A_OK) {
        A_DPRINTF(DBG_WMI, (DBGFMT "bad packet 2\n", DBGARG));
        wmip->wmi_stats.cmd_len_err++;
        A_NETBUF_FREE(osbuf);
        return A_ERROR;
    }

    datap = A_NETBUF_DATA(osbuf);
    len = A_NETBUF_LEN(osbuf);

    ar6000_get_driver_cfg(wmip->wmi_devt,
                    AR6000_DRIVER_CFG_LOG_RAW_WMI_MSGS,
                    &loggingReq);

    if(loggingReq) {
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("WMI %d \n",id));
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("WMI recv, MsgNo %d : ", cmdRecvNum));
        for(i = 0; i < len; i++)
            AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("%x ", datap[i]));
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("\n"));
    }

    LOCK_WMI(wmip);
    cmdRecvNum++;
    UNLOCK_WMI(wmip);

    switch (id) {
    case (WMI_GET_BITRATE_CMDID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_BITRATE_CMDID\n", DBGARG));
        status = wmi_bitrate_reply_rx(wmip, datap, len);
        break;
    case (WMI_GET_CHANNEL_LIST_CMDID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_CHANNEL_LIST_CMDID\n", DBGARG));
        status = wmi_channelList_reply_rx(wmip, datap, len);
        break;
    case (WMI_GET_TX_PWR_CMDID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_TX_PWR_CMDID\n", DBGARG));
        status = wmi_txPwr_reply_rx(wmip, datap, len);
        break;
    case (WMI_READY_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_READY_EVENTID\n", DBGARG));
        status = wmi_ready_event_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        A_WMI_DBGLOG_INIT_DONE(wmip->wmi_devt);
        break;
    case (WMI_CONNECT_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_CONNECT_EVENTID\n", DBGARG));
        status = wmi_connect_event_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_DISCONNECT_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_DISCONNECT_EVENTID\n", DBGARG));
        status = wmi_disconnect_event_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_TKIP_MICERR_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_TKIP_MICERR_EVENTID\n", DBGARG));
        status = wmi_tkip_micerr_event_rx(wmip, datap, len);
        break;
    case (WMI_BSSINFO_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_BSSINFO_EVENTID\n", DBGARG));
        status = wmi_bssInfo_event_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_REGDOMAIN_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_REGDOMAIN_EVENTID\n", DBGARG));
        status = wmi_regDomain_event_rx(wmip, datap, len);
        break;
    case (WMI_PSTREAM_TIMEOUT_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_PSTREAM_TIMEOUT_EVENTID\n", DBGARG));
        status = wmi_pstream_timeout_event_rx(wmip, datap, len);
            /* pstreams are fatpipe abstractions that get implicitly created.
             * User apps only deal with thinstreams. creation of a thinstream
             * by the user or data traffic flow in an AC triggers implicit
             * pstream creation. Do we need to send this event to App..?
             * no harm in sending it.
             */
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_NEIGHBOR_REPORT_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_NEIGHBOR_REPORT_EVENTID\n", DBGARG));
        status = wmi_neighborReport_event_rx(wmip, datap, len);
        break;
    case (WMI_SCAN_COMPLETE_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_SCAN_COMPLETE_EVENTID\n", DBGARG));
        status = wmi_scanComplete_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_CMDERROR_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_CMDERROR_EVENTID\n", DBGARG));
        status = wmi_errorEvent_rx(wmip, datap, len);
        break;
    case (WMI_REPORT_STATISTICS_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_REPORT_STATISTICS_EVENTID\n", DBGARG));
        status = wmi_statsEvent_rx(wmip, datap, len);
        break;
    case (WMI_RSSI_THRESHOLD_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_RSSI_THRESHOLD_EVENTID\n", DBGARG));
        status = wmi_rssiThresholdEvent_rx(wmip, datap, len);
        break;
    case (WMI_ERROR_REPORT_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_ERROR_REPORT_EVENTID\n", DBGARG));
        status = wmi_reportErrorEvent_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_OPT_RX_FRAME_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_OPT_RX_FRAME_EVENTID\n", DBGARG));
        status = wmi_opt_frame_event_rx(wmip, datap, len);
        break;
    case (WMI_REPORT_ROAM_TBL_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_REPORT_ROAM_TBL_EVENTID\n", DBGARG));
        status = wmi_roam_tbl_event_rx(wmip, datap, len);
        break;
    case (WMI_EXTENSION_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_EXTENSION_EVENTID\n", DBGARG));
        status = wmi_control_rx_xtnd(wmip, osbuf);
        break;
    case (WMI_CAC_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_CAC_EVENTID\n", DBGARG));
        status = wmi_cac_event_rx(wmip, datap, len);
        break;
    case (WMI_REPORT_ROAM_DATA_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_REPORT_ROAM_DATA_EVENTID\n", DBGARG));
        status = wmi_roam_data_event_rx(wmip, datap, len);
        break;
#ifdef CONFIG_HOST_TCMD_SUPPORT
    case (WMI_TEST_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_TEST_EVENTID\n", DBGARG));
        status = wmi_tcmd_test_report_rx(wmip, datap, len);
        break;
#endif
    case (WMI_GET_FIXRATES_CMDID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_FIXRATES_CMDID\n", DBGARG));
        status = wmi_ratemask_reply_rx(wmip, datap, len);
        break;
    case (WMI_TX_RETRY_ERR_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_TX_RETRY_ERR_EVENTID\n", DBGARG));
        status = wmi_txRetryErrEvent_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_SNR_THRESHOLD_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_SNR_THRESHOLD_EVENTID\n", DBGARG));
        status = wmi_snrThresholdEvent_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_LQ_THRESHOLD_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_LQ_THRESHOLD_EVENTID\n", DBGARG));
        status = wmi_lqThresholdEvent_rx(wmip, datap, len);
        A_WMI_SEND_EVENT_TO_APP(wmip->wmi_devt, id, datap, len);
        break;
    case (WMI_APLIST_EVENTID):
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Received APLIST Event\n"));
        status = wmi_aplistEvent_rx(wmip, datap, len);
        break;
    case (WMI_GET_KEEPALIVE_CMDID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_KEEPALIVE_CMDID\n", DBGARG));
        status = wmi_keepalive_reply_rx(wmip, datap, len);
        break;
    case (WMI_GET_WOW_LIST_EVENTID):
        status = wmi_get_wow_list_event_rx(wmip, datap, len);
        break;
    case (WMI_GET_PMKID_LIST_EVENTID):
        A_DPRINTF(DBG_WMI, (DBGFMT "WMI_GET_PMKID_LIST Event\n", DBGARG));
        status = wmi_get_pmkid_list_event_rx(wmip, datap, len);
        break;
    default:
        A_DPRINTF(DBG_WMI|DBG_ERROR,
            (DBGFMT "Unknown id 0x%x\n", DBGARG, id));
        wmip->wmi_stats.cmd_id_err++;
        status = A_ERROR;
        break;
    }

    A_NETBUF_FREE(osbuf);

    return status;
}

static A_STATUS
wmi_ready_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_READY_EVENT *ev = (WMI_READY_EVENT *)datap;

    if (len < sizeof(WMI_READY_EVENT)) {
        return A_EINVAL;
    }
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));
    wmip->wmi_ready = TRUE;
    A_WMI_READY_EVENT(wmip->wmi_devt, ev->macaddr, ev->phyCapability);

    return A_OK;
}

static A_STATUS
wmi_connect_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_CONNECT_EVENT *ev;

    if (len < sizeof(WMI_CONNECT_EVENT)) {
        return A_EINVAL;
    }
    ev = (WMI_CONNECT_EVENT *)datap;
    A_DPRINTF(DBG_WMI,
        (DBGFMT "freq %d bssid %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
        DBGARG, ev->channel,
        ev->bssid[0], ev->bssid[1], ev->bssid[2],
        ev->bssid[3], ev->bssid[4], ev->bssid[5]));

    A_MEMCPY(wmip->wmi_bssid, ev->bssid, ATH_MAC_LEN);

    A_WMI_CONNECT_EVENT(wmip->wmi_devt, ev->channel, ev->bssid,
                         ev->listenInterval, ev->beaconInterval,
                         ev->networkType, ev->beaconIeLen,
                         ev->assocReqLen, ev->assocRespLen,
                         ev->assocInfo);

    return A_OK;
}

static A_STATUS
wmi_regDomain_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_REG_DOMAIN_EVENT *ev;

    if (len < sizeof(*ev)) {
        return A_EINVAL;
    }
    ev = (WMI_REG_DOMAIN_EVENT *)datap;

    A_WMI_REGDOMAIN_EVENT(wmip->wmi_devt, ev->regDomain);

    return A_OK;
}

static A_STATUS
wmi_neighborReport_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_NEIGHBOR_REPORT_EVENT *ev;
    int numAps;

    if (len < sizeof(*ev)) {
        return A_EINVAL;
    }
    ev = (WMI_NEIGHBOR_REPORT_EVENT *)datap;
    numAps = ev->numberOfAps;

    if (len < (int)(sizeof(*ev) + ((numAps - 1) * sizeof(WMI_NEIGHBOR_INFO)))) {
        return A_EINVAL;
    }

    A_WMI_NEIGHBORREPORT_EVENT(wmip->wmi_devt, numAps, ev->neighbor);

    return A_OK;
}

static A_STATUS
wmi_disconnect_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_DISCONNECT_EVENT *ev;

    if (len < sizeof(WMI_DISCONNECT_EVENT)) {
        return A_EINVAL;
    }
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    ev = (WMI_DISCONNECT_EVENT *)datap;

    A_MEMZERO(wmip->wmi_bssid, sizeof(wmip->wmi_bssid));

    A_WMI_DISCONNECT_EVENT(wmip->wmi_devt, ev->disconnectReason, ev->bssid,
                            ev->assocRespLen, ev->assocInfo, ev->protocolReasonStatus);

    return A_OK;
}

static A_STATUS
wmi_tkip_micerr_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TKIP_MICERR_EVENT *ev;

    if (len < sizeof(*ev)) {
        return A_EINVAL;
    }
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    ev = (WMI_TKIP_MICERR_EVENT *)datap;
    A_WMI_TKIP_MICERR_EVENT(wmip->wmi_devt, ev->keyid, ev->ismcast);

    return A_OK;
}

static A_STATUS
wmi_bssInfo_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    bss_t *bss;
    WMI_BSS_INFO_HDR *bih;
    A_UINT8 *buf;
    A_UINT32 nodeCachingAllowed;

    if (len <= sizeof(WMI_BSS_INFO_HDR)) {
        return A_EINVAL;
    }

    A_WMI_BSSINFO_EVENT_RX(wmip->wmi_devt, datap, len);
    /* What is driver config for wlan node caching? */
    if(ar6000_get_driver_cfg(wmip->wmi_devt,
                    AR6000_DRIVER_CFG_GET_WLANNODECACHING,
                    &nodeCachingAllowed) != A_OK) {
        return A_EINVAL;
    }

    if(!nodeCachingAllowed) {
        return A_OK;
    }


    bih = (WMI_BSS_INFO_HDR *)datap;
    buf = datap + sizeof(WMI_BSS_INFO_HDR);
    len -= sizeof(WMI_BSS_INFO_HDR);

    A_DPRINTF(DBG_WMI2, (DBGFMT "bssInfo event - ch %u, rssi %02x, "
              "bssid \"%02x:%02x:%02x:%02x:%02x:%02x\"\n", DBGARG,
              bih->channel, (unsigned char) bih->rssi, bih->bssid[0],
              bih->bssid[1], bih->bssid[2], bih->bssid[3], bih->bssid[4],
              bih->bssid[5]));

    if(wps_enable && (bih->frameType == PROBERESP_FTYPE) ) {
	    printk("%s() A_OK 2\n", __FUNCTION__);
		return A_OK;
    }

    bss = wlan_find_node(&wmip->wmi_scan_table, bih->bssid);
    if (bss != NULL) {
        /*
         * Free up the node.  Not the most efficient process given
         * we are about to allocate a new node but it is simple and should be
         * adequate.
         */
        wlan_node_reclaim(&wmip->wmi_scan_table, bss);
    }

    bss = wlan_node_alloc(&wmip->wmi_scan_table, len);
    if (bss == NULL) {
        return A_NO_MEMORY;
    }

    bss->ni_snr        = bih->snr;
    bss->ni_rssi       = bih->rssi;
    A_ASSERT(bss->ni_buf != NULL);
    A_MEMCPY(bss->ni_buf, buf, len);

    if (wlan_parse_beacon(bss->ni_buf, len, &bss->ni_cie) != A_OK) {
        wlan_node_free(bss);
        return A_EINVAL;
    }

    /*
     * Update the frequency in ie_chan, overwriting of channel number
     * which is done in wlan_parse_beacon
     */
    bss->ni_cie.ie_chan = bih->channel;
    wlan_setup_node(&wmip->wmi_scan_table, bss, bih->bssid);

    return A_OK;
}

static A_STATUS
wmi_opt_frame_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    bss_t *bss;
    WMI_OPT_RX_INFO_HDR *bih;
    A_UINT8 *buf;

    if (len <= sizeof(WMI_OPT_RX_INFO_HDR)) {
        return A_EINVAL;
    }

    bih = (WMI_OPT_RX_INFO_HDR *)datap;
    buf = datap + sizeof(WMI_OPT_RX_INFO_HDR);
    len -= sizeof(WMI_OPT_RX_INFO_HDR);

    A_DPRINTF(DBG_WMI2, (DBGFMT "opt frame event %2.2x:%2.2x\n", DBGARG,
        bih->bssid[4], bih->bssid[5]));

    bss = wlan_find_node(&wmip->wmi_scan_table, bih->bssid);
    if (bss != NULL) {
        /*
         * Free up the node.  Not the most efficient process given
         * we are about to allocate a new node but it is simple and should be
         * adequate.
         */
        wlan_node_reclaim(&wmip->wmi_scan_table, bss);
    }

    bss = wlan_node_alloc(&wmip->wmi_scan_table, len);
    if (bss == NULL) {
        return A_NO_MEMORY;
    }

    bss->ni_snr        = bih->snr;
    bss->ni_cie.ie_chan = bih->channel;
    A_ASSERT(bss->ni_buf != NULL);
    A_MEMCPY(bss->ni_buf, buf, len);
    wlan_setup_node(&wmip->wmi_scan_table, bss, bih->bssid);

    return A_OK;
}

    /* This event indicates inactivity timeout of a fatpipe(pstream)
     * at the target
     */
static A_STATUS
wmi_pstream_timeout_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_PSTREAM_TIMEOUT_EVENT *ev;

    if (len < sizeof(WMI_PSTREAM_TIMEOUT_EVENT)) {
        return A_EINVAL;
    }

    A_DPRINTF(DBG_WMI, (DBGFMT "wmi_pstream_timeout_event_rx\n", DBGARG));

    ev = (WMI_PSTREAM_TIMEOUT_EVENT *)datap;

        /* When the pstream (fat pipe == AC) timesout, it means there were no
         * thinStreams within this pstream & it got implicitly created due to
         * data flow on this AC. We start the inactivity timer only for
         * implicitly created pstream. Just reset the host state.
         */
        /* Set the activeTsids for this AC to 0 */
    LOCK_WMI(wmip);
    wmip->wmi_streamExistsForAC[ev->trafficClass]=0;
    wmip->wmi_fatPipeExists &= ~(1 << ev->trafficClass);
    UNLOCK_WMI(wmip);

        /*Indicate inactivity to driver layer for this fatpipe (pstream)*/
    A_WMI_STREAM_TX_INACTIVE(wmip->wmi_devt, ev->trafficClass);

    return A_OK;
}

static A_STATUS
wmi_bitrate_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_BIT_RATE_CMD *reply;
    A_INT32 rate;

    if (len < sizeof(WMI_BIT_RATE_CMD)) {
        return A_EINVAL;
    }
    reply = (WMI_BIT_RATE_CMD *)datap;
    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - rateindex %d\n", DBGARG, reply->rateIndex));

    if (reply->rateIndex == RATE_AUTO) {
        rate = RATE_AUTO;
    } else {
        rate = wmi_rateTable[(A_UINT32) reply->rateIndex];
    }

    A_WMI_BITRATE_RX(wmip->wmi_devt, rate);

    return A_OK;
}

static A_STATUS
wmi_ratemask_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_FIX_RATES_CMD *reply;

    if (len < sizeof(WMI_BIT_RATE_CMD)) {
        return A_EINVAL;
    }
    reply = (WMI_FIX_RATES_CMD *)datap;
    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - fixed rate mask %x\n", DBGARG, reply->fixRateMask));

    A_WMI_RATEMASK_RX(wmip->wmi_devt, reply->fixRateMask);

    return A_OK;
}

static A_STATUS
wmi_channelList_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_CHANNEL_LIST_REPLY *reply;

    if (len < sizeof(WMI_CHANNEL_LIST_REPLY)) {
        return A_EINVAL;
    }
    reply = (WMI_CHANNEL_LIST_REPLY *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_CHANNELLIST_RX(wmip->wmi_devt, reply->numChannels,
                          reply->channelList);

    return A_OK;
}

static A_STATUS
wmi_txPwr_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TX_PWR_REPLY *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TX_PWR_REPLY *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_TXPWR_RX(wmip->wmi_devt, reply->dbM);

    return A_OK;
}
static A_STATUS
wmi_keepalive_reply_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_GET_KEEPALIVE_CMD *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_GET_KEEPALIVE_CMD *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_KEEPALIVE_RX(wmip->wmi_devt, reply->configured);

    return A_OK;
}


static A_STATUS
wmi_dset_open_req_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_DSETOPENREQ_EVENT *dsetopenreq;

    if (len < sizeof(WMIX_DSETOPENREQ_EVENT)) {
        return A_EINVAL;
    }
    dsetopenreq = (WMIX_DSETOPENREQ_EVENT *)datap;
    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - dset_id=0x%x\n", DBGARG, dsetopenreq->dset_id));
    A_WMI_DSET_OPEN_REQ(wmip->wmi_devt,
                        dsetopenreq->dset_id,
                        dsetopenreq->targ_dset_handle,
                        dsetopenreq->targ_reply_fn,
                        dsetopenreq->targ_reply_arg);

    return A_OK;
}

#ifdef CONFIG_HOST_DSET_SUPPORT
static A_STATUS
wmi_dset_close_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_DSETCLOSE_EVENT *dsetclose;

    if (len < sizeof(WMIX_DSETCLOSE_EVENT)) {
        return A_EINVAL;
    }
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    dsetclose = (WMIX_DSETCLOSE_EVENT *)datap;
    A_WMI_DSET_CLOSE(wmip->wmi_devt, dsetclose->access_cookie);

    return A_OK;
}

static A_STATUS
wmi_dset_data_req_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_DSETDATAREQ_EVENT *dsetdatareq;

    if (len < sizeof(WMIX_DSETDATAREQ_EVENT)) {
        return A_EINVAL;
    }
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    dsetdatareq = (WMIX_DSETDATAREQ_EVENT *)datap;
    A_WMI_DSET_DATA_REQ(wmip->wmi_devt,
                         dsetdatareq->access_cookie,
                         dsetdatareq->offset,
                         dsetdatareq->length,
                         dsetdatareq->targ_buf,
                         dsetdatareq->targ_reply_fn,
                         dsetdatareq->targ_reply_arg);

    return A_OK;
}
#endif /* CONFIG_HOST_DSET_SUPPORT */

static A_STATUS
wmi_scanComplete_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_SCAN_COMPLETE_EVENT *ev;

    ev = (WMI_SCAN_COMPLETE_EVENT *)datap;
    A_WMI_SCANCOMPLETE_EVENT(wmip->wmi_devt, ev->status);

    return A_OK;
}

/*
 * Target is reporting a programming error.  This is for
 * developer aid only.  Target only checks a few common violations
 * and it is responsibility of host to do all error checking.
 * Behavior of target after wmi error event is undefined.
 * A reset is recommended.
 */
static A_STATUS
wmi_errorEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_CMD_ERROR_EVENT *ev;

    ev = (WMI_CMD_ERROR_EVENT *)datap;
    AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Programming Error: cmd=%d ", ev->commandId));
    switch (ev->errorCode) {
    case (INVALID_PARAM):
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Illegal Parameter\n"));
        break;
    case (ILLEGAL_STATE):
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Illegal State\n"));
        break;
    case (INTERNAL_ERROR):
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Internal Error\n"));
        break;
    }

    return A_OK;
}


static A_STATUS
wmi_statsEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TARGET_STATS *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TARGET_STATS *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_TARGETSTATS_EVENT(wmip->wmi_devt, reply);

    return A_OK;
}

static A_STATUS
wmi_rssiThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_RSSI_THRESHOLD_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_RSSI_THRESHOLD_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_RSSI_THRESHOLD_EVENT(wmip->wmi_devt, reply->range, reply->rssi);

    return A_OK;
}


static A_STATUS
wmi_reportErrorEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TARGET_ERROR_REPORT_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TARGET_ERROR_REPORT_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_REPORT_ERROR_EVENT(wmip->wmi_devt, reply->errorVal);

    return A_OK;
}

static A_STATUS
wmi_cac_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_CAC_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_CAC_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_CAC_EVENT(wmip->wmi_devt, reply->ac,
                reply->cac_indication, reply->statusCode,
                reply->tspecSuggestion);

    return A_OK;
}

static A_STATUS
wmi_hbChallengeResp_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_HB_CHALLENGE_RESP_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMIX_HB_CHALLENGE_RESP_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "wmi: challenge response event\n", DBGARG));

    A_WMI_HBCHALLENGERESP_EVENT(wmip->wmi_devt, reply->cookie, reply->source);

    return A_OK;
}

static A_STATUS
wmi_roam_tbl_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TARGET_ROAM_TBL *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TARGET_ROAM_TBL *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_ROAM_TABLE_EVENT(wmip->wmi_devt, reply);

    return A_OK;
}

static A_STATUS
wmi_roam_data_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TARGET_ROAM_DATA *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TARGET_ROAM_DATA *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_ROAM_DATA_EVENT(wmip->wmi_devt, reply);

    return A_OK;
}

static A_STATUS
wmi_txRetryErrEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_TX_RETRY_ERR_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_TX_RETRY_ERR_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_TX_RETRY_ERR_EVENT(wmip->wmi_devt);

    return A_OK;
}

static A_STATUS
wmi_snrThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_SNR_THRESHOLD_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_SNR_THRESHOLD_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_SNR_THRESHOLD_EVENT_RX(wmip->wmi_devt, reply->range, reply->snr);

    return A_OK;
}

static A_STATUS
wmi_lqThresholdEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_LQ_THRESHOLD_EVENT *reply;

    if (len < sizeof(*reply)) {
        return A_EINVAL;
    }
    reply = (WMI_LQ_THRESHOLD_EVENT *)datap;
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_LQ_THRESHOLD_EVENT_RX(wmip->wmi_devt, reply->range, reply->lq);

    return A_OK;
}

static A_STATUS
wmi_aplistEvent_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    A_UINT16 ap_info_entry_size;
    WMI_APLIST_EVENT *ev = (WMI_APLIST_EVENT *)datap;
    WMI_AP_INFO_V1 *ap_info_v1;
    A_UINT8 i;

    if (len < sizeof(WMI_APLIST_EVENT)) {
        return A_EINVAL;
    }

    if (ev->apListVer == APLIST_VER1) {
        ap_info_entry_size = sizeof(WMI_AP_INFO_V1);
        ap_info_v1 = (WMI_AP_INFO_V1 *)ev->apList;
    } else {
        return A_EINVAL;
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("Number of APs in APLIST Event is %d\n", ev->numAP));
    if (len < (int)(sizeof(WMI_APLIST_EVENT) +
              (ev->numAP - 1) * ap_info_entry_size))
    {
        return A_EINVAL;
    }

    /*
     * AP List Ver1 Contents
     */
    for (i = 0; i < ev->numAP; i++) {
        AR_DEBUG_PRINTF(ATH_DEBUG_WMI, ("AP#%d BSSID %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x "\
                    "Channel %d\n", i,
                   ap_info_v1->bssid[0], ap_info_v1->bssid[1],
                   ap_info_v1->bssid[2], ap_info_v1->bssid[3],
                   ap_info_v1->bssid[4], ap_info_v1->bssid[5],
                   ap_info_v1->channel));
        ap_info_v1++;
    }
    return A_OK;
}

static A_STATUS
wmi_dbglog_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    A_UINT32 dropped;

    dropped = *((A_UINT32 *)datap);
    datap += sizeof(dropped);
    len -= sizeof(dropped);
    A_WMI_DBGLOG_EVENT(wmip->wmi_devt, dropped, datap, len);
    return A_OK;
}

#ifdef CONFIG_HOST_GPIO_SUPPORT
static A_STATUS
wmi_gpio_intr_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_GPIO_INTR_EVENT *gpio_intr = (WMIX_GPIO_INTR_EVENT *)datap;

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - intrmask=0x%x input=0x%x.\n", DBGARG,
        gpio_intr->intr_mask, gpio_intr->input_values));

    A_WMI_GPIO_INTR_RX(gpio_intr->intr_mask, gpio_intr->input_values);

    return A_OK;
}

static A_STATUS
wmi_gpio_data_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMIX_GPIO_DATA_EVENT *gpio_data = (WMIX_GPIO_DATA_EVENT *)datap;

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - reg=%d value=0x%x\n", DBGARG,
        gpio_data->reg_id, gpio_data->value));

    A_WMI_GPIO_DATA_RX(gpio_data->reg_id, gpio_data->value);

    return A_OK;
}

static A_STATUS
wmi_gpio_ack_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    A_WMI_GPIO_ACK_RX();

    return A_OK;
}
#endif /* CONFIG_HOST_GPIO_SUPPORT */

/*
 * Called to send a wmi command. Command specific data is already built
 * on osbuf and current osbuf->data points to it.
 */
A_STATUS
wmi_cmd_send(struct wmi_t *wmip, void *osbuf, WMI_COMMAND_ID cmdId,
               WMI_SYNC_FLAG syncflag)
{
#define IS_LONG_CMD(cmdId) ((cmdId == WMI_OPT_TX_FRAME_CMDID) || (cmdId == WMI_ADD_WOW_PATTERN_CMDID))
    WMI_CMD_HDR         *cHdr;
    WMI_PRI_STREAM_ID   streamID = WMI_CONTROL_PRI;

    A_ASSERT(osbuf != NULL);

    if (syncflag >= END_WMIFLAG) {
        return A_EINVAL;
    }

    if ((syncflag == SYNC_BEFORE_WMIFLAG) || (syncflag == SYNC_BOTH_WMIFLAG)) {
        /*
         * We want to make sure all data currently queued is transmitted before
         * the cmd execution.  Establish a new sync point.
         */
        wmi_sync_point(wmip);
    }

    if (A_NETBUF_PUSH(osbuf, sizeof(WMI_CMD_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    cHdr = (WMI_CMD_HDR *)A_NETBUF_DATA(osbuf);
    cHdr->commandId = cmdId;

    /*
     * Send cmd, some via control pipe, others via data pipe
     */
    if (IS_LONG_CMD(cmdId)) {
        wmi_data_hdr_add(wmip, osbuf, CNTL_MSGTYPE);
        // TODO ... these can now go through the control endpoint via HTC 2.0
        streamID = WMI_BEST_EFFORT_PRI;
    }
    A_WMI_CONTROL_TX(wmip->wmi_devt, osbuf, streamID);

    if ((syncflag == SYNC_AFTER_WMIFLAG) || (syncflag == SYNC_BOTH_WMIFLAG)) {
        /*
         * We want to make sure all new data queued waits for the command to
         * execute. Establish a new sync point.
         */
        wmi_sync_point(wmip);
    }
    return (A_OK);
#undef IS_LONG_CMD
}

A_STATUS
wmi_cmd_send_xtnd(struct wmi_t *wmip, void *osbuf, WMI_COMMAND_ID cmdId,
                  WMI_SYNC_FLAG syncflag)
{
    WMIX_CMD_HDR     *cHdr;

    if (A_NETBUF_PUSH(osbuf, sizeof(WMIX_CMD_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    cHdr = (WMIX_CMD_HDR *)A_NETBUF_DATA(osbuf);
    cHdr->commandId = cmdId;

    return wmi_cmd_send(wmip, osbuf, WMI_EXTENSION_CMDID, syncflag);
}

A_STATUS
wmi_connect_cmd(struct wmi_t *wmip, NETWORK_TYPE netType,
                DOT11_AUTH_MODE dot11AuthMode, AUTH_MODE authMode,
                CRYPTO_TYPE pairwiseCrypto, A_UINT8 pairwiseCryptoLen,
                CRYPTO_TYPE groupCrypto,A_UINT8 groupCryptoLen,
                int ssidLength, A_UCHAR *ssid,
                A_UINT8 *bssid, A_UINT16 channel, A_UINT32 ctrl_flags)
{
    void *osbuf;
    WMI_CONNECT_CMD *cc;

    if ((pairwiseCrypto == NONE_CRYPT) && (groupCrypto != NONE_CRYPT)) {
        return A_EINVAL;
    }
    if ((pairwiseCrypto != NONE_CRYPT) && (groupCrypto == NONE_CRYPT)) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(WMI_CONNECT_CMD));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(WMI_CONNECT_CMD));

    cc = (WMI_CONNECT_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cc, sizeof(*cc));

    A_MEMCPY(cc->ssid, ssid, ssidLength);
    cc->ssidLength          = ssidLength;
    cc->networkType         = netType;
    cc->dot11AuthMode       = dot11AuthMode;
    cc->authMode            = authMode;
    cc->pairwiseCryptoType  = pairwiseCrypto;
    cc->pairwiseCryptoLen   = pairwiseCryptoLen;
    cc->groupCryptoType     = groupCrypto;
    cc->groupCryptoLen      = groupCryptoLen;
    cc->channel             = channel;
    cc->ctrl_flags          = ctrl_flags;

    if (bssid != NULL) {
        A_MEMCPY(cc->bssid, bssid, ATH_MAC_LEN);
    }
    if (wmi_set_keepalive_cmd(wmip, wmip->wmi_keepaliveInterval) != A_OK) {
        return(A_ERROR);
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_CONNECT_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_reconnect_cmd(struct wmi_t *wmip, A_UINT8 *bssid, A_UINT16 channel)
{
    void *osbuf;
    WMI_RECONNECT_CMD *cc;

    osbuf = A_NETBUF_ALLOC(sizeof(WMI_RECONNECT_CMD));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(WMI_RECONNECT_CMD));

    cc = (WMI_RECONNECT_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cc, sizeof(*cc));

    cc->channel = channel;

    if (bssid != NULL) {
        A_MEMCPY(cc->bssid, bssid, ATH_MAC_LEN);
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_RECONNECT_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_disconnect_cmd(struct wmi_t *wmip)
{
    void *osbuf;
    A_STATUS status;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    /* Bug fix for 24817(elevator bug) - the disconnect command does not
       need to do a SYNC before.*/
    status = (wmi_cmd_send(wmip, osbuf, WMI_DISCONNECT_CMDID,
                         NO_SYNC_WMIFLAG));

    return status;
}

A_STATUS
wmi_startscan_cmd(struct wmi_t *wmip, WMI_SCAN_TYPE scanType,
                  A_BOOL forceFgScan, A_BOOL isLegacy,
                  A_UINT32 homeDwellTime, A_UINT32 forceScanInterval)
{
    void *osbuf;
    WMI_START_SCAN_CMD *sc;

    if ((scanType != WMI_LONG_SCAN) && (scanType != WMI_SHORT_SCAN)) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*sc));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*sc));

    sc = (WMI_START_SCAN_CMD *)(A_NETBUF_DATA(osbuf));
    sc->scanType = scanType;
    sc->forceFgScan = forceFgScan;
    sc->isLegacy = isLegacy;
    sc->homeDwellTime = homeDwellTime;
    sc->forceScanInterval = forceScanInterval;

    return (wmi_cmd_send(wmip, osbuf, WMI_START_SCAN_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_scanparams_cmd(struct wmi_t *wmip, A_UINT16 fg_start_sec,
                   A_UINT16 fg_end_sec, A_UINT16 bg_sec,
                   A_UINT16 minact_chdw_msec, A_UINT16 maxact_chdw_msec,
                   A_UINT16 pas_chdw_msec,
                   A_UINT8 shScanRatio, A_UINT8 scanCtrlFlags,
                   A_UINT32 max_dfsch_act_time)
{
    void *osbuf;
    WMI_SCAN_PARAMS_CMD *sc;

    osbuf = A_NETBUF_ALLOC(sizeof(*sc));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*sc));

    sc = (WMI_SCAN_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(sc, sizeof(*sc));
    sc->fg_start_period  = fg_start_sec;
    sc->fg_end_period    = fg_end_sec;
    sc->bg_period        = bg_sec;
    sc->minact_chdwell_time = minact_chdw_msec;
    sc->maxact_chdwell_time = maxact_chdw_msec;
    sc->pas_chdwell_time = pas_chdw_msec;
    sc->shortScanRatio   = shScanRatio;
    sc->scanCtrlFlags    = scanCtrlFlags;
    sc->max_dfsch_act_time = max_dfsch_act_time;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_SCAN_PARAMS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_bssfilter_cmd(struct wmi_t *wmip, A_UINT8 filter, A_UINT32 ieMask)
{
    void *osbuf;
    WMI_BSS_FILTER_CMD *cmd;

    if (filter >= LAST_BSS_FILTER) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_BSS_FILTER_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->bssFilter = filter;
    cmd->ieMask = ieMask;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BSS_FILTER_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_probedSsid_cmd(struct wmi_t *wmip, A_UINT8 index, A_UINT8 flag,
                   A_UINT8 ssidLength, A_UCHAR *ssid)
{
    void *osbuf;
    WMI_PROBED_SSID_CMD *cmd;

    if (index > MAX_PROBED_SSID_INDEX) {
        return A_EINVAL;
    }
    if (ssidLength > sizeof(cmd->ssid)) {
        return A_EINVAL;
    }
    if ((flag & (DISABLE_SSID_FLAG | ANY_SSID_FLAG)) && (ssidLength > 0)) {
        return A_EINVAL;
    }
    if ((flag & SPECIFIC_SSID_FLAG) && !ssidLength) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_PROBED_SSID_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->entryIndex = index;
    cmd->flag       = flag;
    cmd->ssidLength = ssidLength;
    A_MEMCPY(cmd->ssid, ssid, ssidLength);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_PROBED_SSID_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_listeninterval_cmd(struct wmi_t *wmip, A_UINT16 listenInterval, A_UINT16 listenBeacons)
{
    void *osbuf;
    WMI_LISTEN_INT_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_LISTEN_INT_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->listenInterval = listenInterval;
    cmd->numBeacons = listenBeacons;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_LISTEN_INT_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_bmisstime_cmd(struct wmi_t *wmip, A_UINT16 bmissTime, A_UINT16 bmissBeacons)
{
    void *osbuf;
    WMI_BMISS_TIME_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_BMISS_TIME_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->bmissTime = bmissTime;
    cmd->numBeacons =  bmissBeacons;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BMISS_TIME_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_associnfo_cmd(struct wmi_t *wmip, A_UINT8 ieType,
                     A_UINT8 ieLen, A_UINT8 *ieInfo)
{
    void *osbuf;
    WMI_SET_ASSOC_INFO_CMD *cmd;
    A_UINT16 cmdLen;

    cmdLen = sizeof(*cmd) + ieLen - 1;
    osbuf = A_NETBUF_ALLOC(cmdLen);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, cmdLen);

    cmd = (WMI_SET_ASSOC_INFO_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, cmdLen);
    cmd->ieType = ieType;
    cmd->bufferSize = ieLen;
    A_MEMCPY(cmd->assocInfo, ieInfo, ieLen);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_ASSOC_INFO_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_powermode_cmd(struct wmi_t *wmip, A_UINT8 powerMode)
{
    void *osbuf;
    WMI_POWER_MODE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_POWER_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->powerMode = powerMode;
    wmip->wmi_powerMode = powerMode;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_POWER_MODE_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_ibsspmcaps_cmd(struct wmi_t *wmip, A_UINT8 pmEnable, A_UINT8 ttl,
                   A_UINT16 atim_windows, A_UINT16 timeout_value)
{
    void *osbuf;
    WMI_IBSS_PM_CAPS_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_IBSS_PM_CAPS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->power_saving = pmEnable;
    cmd->ttl = ttl;
    cmd->atim_windows = atim_windows;
    cmd->timeout_value = timeout_value;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_IBSS_PM_CAPS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_pmparams_cmd(struct wmi_t *wmip, A_UINT16 idlePeriod,
                 A_UINT16 psPollNum, A_UINT16 dtimPolicy)
{
    void *osbuf;
    WMI_POWER_PARAMS_CMD *pm;

    osbuf = A_NETBUF_ALLOC(sizeof(*pm));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*pm));

    pm = (WMI_POWER_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(pm, sizeof(*pm));
    pm->idle_period   = idlePeriod;
    pm->pspoll_number = psPollNum;
    pm->dtim_policy   = dtimPolicy;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_POWER_PARAMS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_disctimeout_cmd(struct wmi_t *wmip, A_UINT8 timeout)
{
    void *osbuf;
    WMI_DISC_TIMEOUT_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_DISC_TIMEOUT_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->disconnectTimeout = timeout;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_DISC_TIMEOUT_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_addKey_cmd(struct wmi_t *wmip, A_UINT8 keyIndex, CRYPTO_TYPE keyType,
               A_UINT8 keyUsage, A_UINT8 keyLength, A_UINT8 *keyRSC,
               A_UINT8 *keyMaterial, A_UINT8 key_op_ctrl,
               WMI_SYNC_FLAG sync_flag)
{
    void *osbuf;
    WMI_ADD_CIPHER_KEY_CMD *cmd;

    if ((keyIndex > WMI_MAX_KEY_INDEX) || (keyLength > WMI_MAX_KEY_LEN) ||
        (keyMaterial == NULL))
    {
        return A_EINVAL;
    }

    if ((WEP_CRYPT != keyType) && (NULL == keyRSC)) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_ADD_CIPHER_KEY_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->keyIndex = keyIndex;
    cmd->keyType  = keyType;
    cmd->keyUsage = keyUsage;
    cmd->keyLength = keyLength;
    A_MEMCPY(cmd->key, keyMaterial, keyLength);
    if (NULL != keyRSC) {
        A_MEMCPY(cmd->keyRSC, keyRSC, sizeof(cmd->keyRSC));
    }
    cmd->key_op_ctrl = key_op_ctrl;

    return (wmi_cmd_send(wmip, osbuf, WMI_ADD_CIPHER_KEY_CMDID, sync_flag));
}

A_STATUS
wmi_add_krk_cmd(struct wmi_t *wmip, A_UINT8 *krk)
{
    void *osbuf;
    WMI_ADD_KRK_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));

    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_ADD_KRK_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    A_MEMCPY(cmd->krk, krk, WMI_KRK_LEN);

    return (wmi_cmd_send(wmip, osbuf, WMI_ADD_KRK_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_delete_krk_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);

    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_DELETE_KRK_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_deleteKey_cmd(struct wmi_t *wmip, A_UINT8 keyIndex)
{
    void *osbuf;
    WMI_DELETE_CIPHER_KEY_CMD *cmd;

    if (keyIndex > WMI_MAX_KEY_INDEX) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_DELETE_CIPHER_KEY_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->keyIndex = keyIndex;

    return (wmi_cmd_send(wmip, osbuf, WMI_DELETE_CIPHER_KEY_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_setPmkid_cmd(struct wmi_t *wmip, A_UINT8 *bssid, A_UINT8 *pmkId,
                 A_BOOL set)
{
    void *osbuf;
    WMI_SET_PMKID_CMD *cmd;

    if (bssid == NULL) {
        return A_EINVAL;
    }

    if ((set == TRUE) && (pmkId == NULL)) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_PMKID_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMCPY(cmd->bssid, bssid, sizeof(cmd->bssid));
    if (set == TRUE) {
        A_MEMCPY(cmd->pmkid, pmkId, sizeof(cmd->pmkid));
        cmd->enable = PMKID_ENABLE;
    } else {
        A_MEMZERO(cmd->pmkid, sizeof(cmd->pmkid));
        cmd->enable = PMKID_DISABLE;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_PMKID_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_tkip_countermeasures_cmd(struct wmi_t *wmip, A_BOOL en)
{
    void *osbuf;
    WMI_SET_TKIP_COUNTERMEASURES_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_TKIP_COUNTERMEASURES_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->cm_en = (en == TRUE)? WMI_TKIP_CM_ENABLE : WMI_TKIP_CM_DISABLE;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_TKIP_COUNTERMEASURES_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_akmp_params_cmd(struct wmi_t *wmip,
                        WMI_SET_AKMP_PARAMS_CMD *akmpParams)
{
    void *osbuf;
    WMI_SET_AKMP_PARAMS_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));
    cmd = (WMI_SET_AKMP_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->akmpInfo = akmpParams->akmpInfo;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_AKMP_PARAMS_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_pmkid_list_cmd(struct wmi_t *wmip,
                       WMI_SET_PMKID_LIST_CMD *pmkInfo)
{
    void *osbuf;
    WMI_SET_PMKID_LIST_CMD *cmd;
    A_UINT16 cmdLen;
    A_UINT8 i;

    cmdLen = sizeof(pmkInfo->numPMKID) +
             pmkInfo->numPMKID * sizeof(WMI_PMKID);

    osbuf = A_NETBUF_ALLOC(cmdLen);

    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, cmdLen);
    cmd = (WMI_SET_PMKID_LIST_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->numPMKID = pmkInfo->numPMKID;

    for (i = 0; i < cmd->numPMKID; i++) {
        A_MEMCPY(&cmd->pmkidList[i], &pmkInfo->pmkidList[i],
                 WMI_PMKID_LEN);
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_PMKID_LIST_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_pmkid_list_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_PMKID_LIST_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_dataSync_send(struct wmi_t *wmip, void *osbuf, WMI_PRI_STREAM_ID streamID)
{
    WMI_DATA_HDR     *dtHdr;

    A_ASSERT(streamID != WMI_CONTROL_PRI);
    A_ASSERT(osbuf != NULL);

    if (A_NETBUF_PUSH(osbuf, sizeof(WMI_DATA_HDR)) != A_OK) {
        return A_NO_MEMORY;
    }

    dtHdr = (WMI_DATA_HDR *)A_NETBUF_DATA(osbuf);
    dtHdr->info =
      (SYNC_MSGTYPE & WMI_DATA_HDR_MSG_TYPE_MASK) << WMI_DATA_HDR_MSG_TYPE_SHIFT;

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter - streamID %d\n", DBGARG, streamID));

    return (A_WMI_CONTROL_TX(wmip->wmi_devt, osbuf, streamID));
}

typedef struct _WMI_DATA_SYNC_BUFS {
    A_UINT8            trafficClass;
    void               *osbuf;
}WMI_DATA_SYNC_BUFS;

static A_STATUS
wmi_sync_point(struct wmi_t *wmip)
{
	void *cmd_osbuf;
    WMI_DATA_SYNC_BUFS dataSyncBufs[WMM_NUM_AC];
	A_UINT8 i,numPriStreams=0;
	A_STATUS status;

	A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    memset(dataSyncBufs,0,sizeof(dataSyncBufs));

    /* lock out while we walk through the priority list and assemble our local array */
    LOCK_WMI(wmip);

    for (i=0; i < WMM_NUM_AC ; i++) {
		if (wmip->wmi_fatPipeExists & (1 << i)) {
            numPriStreams++;
            dataSyncBufs[numPriStreams-1].trafficClass = i;
        }
    }

    UNLOCK_WMI(wmip);

    /* dataSyncBufs is now filled with entries (starting at index 0) containing valid streamIDs */

    do {
	    /*
	     * We allocate all network buffers needed so we will be able to
	     * send all required frames.
	     */
	    cmd_osbuf = A_NETBUF_ALLOC(0);      /* no payload */
	    if (cmd_osbuf == NULL) {
		    status = A_NO_MEMORY;
            break;
	    }

	    for (i=0; i < numPriStreams ; i++) {
	        dataSyncBufs[i].osbuf = A_NETBUF_ALLOC(0);
            if (dataSyncBufs[i].osbuf == NULL) {
                status = A_NO_MEMORY;
                break;
            }
	    } //end for

	    /*
	     * Send sync cmd followed by sync data messages on all endpoints being
	     * used
	     */
	    status = wmi_cmd_send(wmip, cmd_osbuf, WMI_SYNCHRONIZE_CMDID,
						  NO_SYNC_WMIFLAG);

        if (A_FAILED(status)) {
            break;
        }
            /* cmd buffer sent, we no longer own it */
        cmd_osbuf = NULL;

		for(i=0; i < numPriStreams; i++) {
            A_ASSERT(dataSyncBufs[i].osbuf != NULL);

            status = wmi_dataSync_send(wmip, dataSyncBufs[i].osbuf,
                        WMI_ACCESSCATEGORY_WMISTREAM(wmip,dataSyncBufs[i].trafficClass));

            if (A_FAILED(status)) {
                break;
            }
            /* we don't own this buffer anymore, NULL it out of the array so it
             * won't get cleaned up */
            dataSyncBufs[i].osbuf = NULL;
		} //end for

    } while(FALSE);

    /* free up any resources left over (possibly due to an error) */

    if (cmd_osbuf != NULL) {
        A_NETBUF_FREE(cmd_osbuf);
    }

    for (i = 0; i < numPriStreams; i++) {
        if (dataSyncBufs[i].osbuf != NULL) {
            A_NETBUF_FREE(dataSyncBufs[i].osbuf);
        }
    }

	return (status);
}

A_STATUS
wmi_create_pstream_cmd(struct wmi_t *wmip, WMI_CREATE_PSTREAM_CMD *params)
{
    void *osbuf;
    WMI_CREATE_PSTREAM_CMD *cmd;
	A_UINT16 activeTsids=0;
    A_UINT8 fatPipeExistsForAC=0;

    /* Validate all the parameters. */
    if( !((params->userPriority < 8) &&
         (params->userPriority <= 0x7) &&
         (convert_userPriority_to_trafficClass(params->userPriority) == params->trafficClass)  &&
         (params->trafficDirection == UPLINK_TRAFFIC ||
            params->trafficDirection == DNLINK_TRAFFIC ||
            params->trafficDirection == BIDIR_TRAFFIC) &&
         (params->trafficType == TRAFFIC_TYPE_APERIODIC ||
            params->trafficType == TRAFFIC_TYPE_PERIODIC ) &&
         (params->voicePSCapability == DISABLE_FOR_THIS_AC  ||
            params->voicePSCapability == ENABLE_FOR_THIS_AC ||
            params->voicePSCapability == ENABLE_FOR_ALL_AC) &&
         (params->tsid == WMI_IMPLICIT_PSTREAM || params->tsid <= WMI_MAX_THINSTREAM)) )
    {
        return  A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Sending create_pstream_cmd: ac=%d    tsid:%d\n", DBGARG,
        params->trafficClass, params->tsid));

    cmd = (WMI_CREATE_PSTREAM_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    A_MEMCPY(cmd, params, sizeof(*cmd));

        /* this is an implicitly created Fat pipe */
    if (params->tsid == WMI_IMPLICIT_PSTREAM) {
        LOCK_WMI(wmip);
        fatPipeExistsForAC = (wmip->wmi_fatPipeExists & (1 << params->trafficClass));
        wmip->wmi_fatPipeExists |= (1<<params->trafficClass);
        UNLOCK_WMI(wmip);
    } else {
            /* this is an explicitly created thin stream within a fat pipe */
        LOCK_WMI(wmip);
        fatPipeExistsForAC = (wmip->wmi_fatPipeExists & (1 << params->trafficClass));
        activeTsids = wmip->wmi_streamExistsForAC[params->trafficClass];
        wmip->wmi_streamExistsForAC[params->trafficClass] |= (1<<params->tsid);
            /* if a thinstream becomes active, the fat pipe automatically
            * becomes active
            */
        wmip->wmi_fatPipeExists |= (1<<params->trafficClass);
        UNLOCK_WMI(wmip);
    }

        /* Indicate activty change to driver layer only if this is the
         * first TSID to get created in this AC explicitly or an implicit
         * fat pipe is getting created.
         */
    if (!fatPipeExistsForAC) {
        A_WMI_STREAM_TX_ACTIVE(wmip->wmi_devt, params->trafficClass);
    }

    /* mike: should be SYNC_BEFORE_WMIFLAG */
    return (wmi_cmd_send(wmip, osbuf, WMI_CREATE_PSTREAM_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_delete_pstream_cmd(struct wmi_t *wmip, A_UINT8 trafficClass, A_UINT8 tsid)
{
    void *osbuf;
    WMI_DELETE_PSTREAM_CMD *cmd;
    A_STATUS status;
	A_UINT16 activeTsids=0;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_DELETE_PSTREAM_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));

    cmd->trafficClass = trafficClass;
    cmd->tsid = tsid;

    LOCK_WMI(wmip);
    activeTsids = wmip->wmi_streamExistsForAC[trafficClass];
    UNLOCK_WMI(wmip);

        /* Check if the tsid was created & exists */
    if (!(activeTsids & (1<<tsid))) {

        A_DPRINTF(DBG_WMI,
        (DBGFMT "TSID %d does'nt exist for trafficClass: %d\n", DBGARG, tsid, trafficClass));
            /* TODO: return a more appropriate err code */
        return A_ERROR;
    }

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Sending delete_pstream_cmd: trafficClass: %d tsid=%d\n", DBGARG, trafficClass, tsid));

    status = (wmi_cmd_send(wmip, osbuf, WMI_DELETE_PSTREAM_CMDID,
                         SYNC_BEFORE_WMIFLAG));

    LOCK_WMI(wmip);
    wmip->wmi_streamExistsForAC[trafficClass] &= ~(1<<tsid);
    activeTsids = wmip->wmi_streamExistsForAC[trafficClass];
    UNLOCK_WMI(wmip);


        /* Indicate stream inactivity to driver layer only if all tsids
         * within this AC are deleted.
         */
    if(!activeTsids) {
        A_WMI_STREAM_TX_INACTIVE(wmip->wmi_devt, trafficClass);
        wmip->wmi_fatPipeExists &= ~(1<<trafficClass);
    }

    return status;
}

/*
 * used to set the bit rate.  rate is in Kbps.  If rate == -1
 * then auto selection is used.
 */
A_STATUS
wmi_set_bitrate_cmd(struct wmi_t *wmip, A_INT32 rate)
{
    void *osbuf;
    WMI_BIT_RATE_CMD *cmd;
    A_INT8 index;

    if (rate != -1) {
        index = wmi_validate_bitrate(wmip, rate);
        if(index == A_EINVAL){
            return A_EINVAL;
        }
    } else {
        index = -1;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_BIT_RATE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));

    cmd->rateIndex = index;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BITRATE_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_bitrate_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_BITRATE_CMDID, NO_SYNC_WMIFLAG));
}

/*
 * Returns TRUE iff the given rate index is legal in the current PHY mode.
 */
A_BOOL
wmi_is_bitrate_index_valid(struct wmi_t *wmip, A_UINT32 rateIndex)
{
    WMI_PHY_MODE phyMode = wmip->wmi_phyMode;
    A_BOOL isValid = TRUE;
    switch(phyMode) {
        case WMI_11A_MODE:
            if ((rateIndex < MODE_A_SUPPORT_RATE_START) || (rateIndex > MODE_A_SUPPORT_RATE_STOP)) {
                isValid = FALSE;
            }
            break;

        case WMI_11B_MODE:
            if ((rateIndex < MODE_B_SUPPORT_RATE_START) || (rateIndex > MODE_B_SUPPORT_RATE_STOP)) {
                isValid = FALSE;
            }
            break;

        case WMI_11GONLY_MODE:
            if ((rateIndex < MODE_GONLY_SUPPORT_RATE_START) || (rateIndex > MODE_GONLY_SUPPORT_RATE_STOP)) {
                isValid = FALSE;
            }
            break;

        case WMI_11G_MODE:
        case WMI_11AG_MODE:
            if ((rateIndex < MODE_G_SUPPORT_RATE_START) || (rateIndex > MODE_G_SUPPORT_RATE_STOP)) {
                isValid = FALSE;
            }
            break;

        default:
            A_ASSERT(FALSE);
            break;
    }

    return isValid;
}

A_INT8
wmi_validate_bitrate(struct wmi_t *wmip, A_INT32 rate)
{
    A_INT8 i;
    if (rate != -1)
    {
        for (i=0;;i++)
        {
            if (wmi_rateTable[(A_UINT32) i] == 0) {
                return A_EINVAL;
            }
            if (wmi_rateTable[(A_UINT32) i] == rate) {
                break;
            }
        }
    }
    else{
     i = -1;
    }

    if(wmi_is_bitrate_index_valid(wmip, i) != TRUE) {
        return A_EINVAL;
    }

    return i;
}

A_STATUS
wmi_set_fixrates_cmd(struct wmi_t *wmip, A_INT16 fixRatesMask)
{
    void *osbuf;
    WMI_FIX_RATES_CMD *cmd;
    A_UINT32 rateIndex;

    /* Make sure all rates in the mask are valid in the current PHY mode */
    for(rateIndex = 0; rateIndex < MAX_NUMBER_OF_SUPPORT_RATES; rateIndex++) {
       if((1 << rateIndex) & (A_UINT32)fixRatesMask) {
            if(wmi_is_bitrate_index_valid(wmip, rateIndex) != TRUE) {
                A_DPRINTF(DBG_WMI, (DBGFMT "Set Fix Rates command failed: Given rate is illegal in current PHY mode\n", DBGARG));
                return A_EINVAL;
            }
       }
    }


    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_FIX_RATES_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));

    cmd->fixRateMask = fixRatesMask;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_FIXRATES_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_ratemask_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_FIXRATES_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_channelList_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_CHANNEL_LIST_CMDID,
                         NO_SYNC_WMIFLAG));
}

/*
 * used to generate a wmi sey channel Parameters cmd.
 * mode should always be specified and corresponds to the phy mode of the
 * wlan.
 * numChan should alway sbe specified. If zero indicates that all available
 * channels should be used.
 * channelList is an array of channel frequencies (in Mhz) which the radio
 * should limit its operation to.  It should be NULL if numChan == 0.  Size of
 * array should correspond to numChan entries.
 */
A_STATUS
wmi_set_channelParams_cmd(struct wmi_t *wmip, A_UINT8 scanParam,
                          WMI_PHY_MODE mode, A_INT8 numChan,
                          A_UINT16 *channelList)
{
    void *osbuf;
    WMI_CHANNEL_PARAMS_CMD *cmd;
    A_INT8 size;

    size = sizeof (*cmd);

    if (numChan) {
        if (numChan > WMI_MAX_CHANNELS) {
            return A_EINVAL;
        }
        size += sizeof(A_UINT16) * (numChan - 1);
    }

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_CHANNEL_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);

    wmip->wmi_phyMode = mode;
    cmd->scanParam   = scanParam;
    cmd->phyMode     = mode;
    cmd->numChannels = numChan;
    A_MEMCPY(cmd->channelList, channelList, numChan * sizeof(A_UINT16));

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_CHANNEL_PARAMS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_rssi_threshold_params(struct wmi_t *wmip,
                              WMI_RSSI_THRESHOLD_PARAMS_CMD *rssiCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_RSSI_THRESHOLD_PARAMS_CMD *cmd;
    /* These values are in ascending order */
    if( rssiCmd->thresholdAbove6_Val <= rssiCmd->thresholdAbove5_Val ||
        rssiCmd->thresholdAbove5_Val <= rssiCmd->thresholdAbove4_Val ||
        rssiCmd->thresholdAbove4_Val <= rssiCmd->thresholdAbove3_Val ||
        rssiCmd->thresholdAbove3_Val <= rssiCmd->thresholdAbove2_Val ||
        rssiCmd->thresholdAbove2_Val <= rssiCmd->thresholdAbove1_Val ||
        rssiCmd->thresholdBelow6_Val <= rssiCmd->thresholdBelow5_Val ||
        rssiCmd->thresholdBelow5_Val <= rssiCmd->thresholdBelow4_Val ||
        rssiCmd->thresholdBelow4_Val <= rssiCmd->thresholdBelow3_Val ||
        rssiCmd->thresholdBelow3_Val <= rssiCmd->thresholdBelow2_Val ||
        rssiCmd->thresholdBelow2_Val <= rssiCmd->thresholdBelow1_Val) {

        return A_EINVAL;
    }

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_RSSI_THRESHOLD_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, rssiCmd, sizeof(WMI_RSSI_THRESHOLD_PARAMS_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_RSSI_THRESHOLD_PARAMS_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_host_sleep_mode_cmd(struct wmi_t *wmip,
                              WMI_SET_HOST_SLEEP_MODE_CMD *hostModeCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_SET_HOST_SLEEP_MODE_CMD *cmd;

    if( hostModeCmd->awake == hostModeCmd->asleep) {
        return A_EINVAL;
    }

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_SET_HOST_SLEEP_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, hostModeCmd, sizeof(WMI_SET_HOST_SLEEP_MODE_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_HOST_SLEEP_MODE_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_wow_mode_cmd(struct wmi_t *wmip,
                              WMI_SET_WOW_MODE_CMD *wowModeCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_SET_WOW_MODE_CMD *cmd;

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_SET_WOW_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, wowModeCmd, sizeof(WMI_SET_WOW_MODE_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_WOW_MODE_CMDID,
                            NO_SYNC_WMIFLAG));

}

A_STATUS
wmi_get_wow_list_cmd(struct wmi_t *wmip,
                              WMI_GET_WOW_LIST_CMD *wowListCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_GET_WOW_LIST_CMD *cmd;

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_GET_WOW_LIST_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, wowListCmd, sizeof(WMI_GET_WOW_LIST_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_WOW_LIST_CMDID,
                            NO_SYNC_WMIFLAG));

}

static A_STATUS
wmi_get_wow_list_event_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{
    WMI_GET_WOW_LIST_REPLY *reply;

    if (len < sizeof(WMI_GET_WOW_LIST_REPLY)) {
        return A_EINVAL;
    }
    reply = (WMI_GET_WOW_LIST_REPLY *)datap;

    A_WMI_WOW_LIST_EVENT(wmip->wmi_devt, reply->num_filters,
                          reply);

    return A_OK;
}

A_STATUS wmi_add_wow_pattern_cmd(struct wmi_t *wmip,
                                 WMI_ADD_WOW_PATTERN_CMD *addWowCmd,
                                 A_UINT8* pattern, A_UINT8* mask,
                                 A_UINT8 pattern_size)
{
    void    *osbuf;
    A_INT8  size;
    WMI_ADD_WOW_PATTERN_CMD *cmd;
    A_UINT8 *filter_mask = NULL;

    size = sizeof (*cmd);

    size += ((2 * addWowCmd->filter_size)* sizeof(A_UINT8));
    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_ADD_WOW_PATTERN_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->filter_list_id = addWowCmd->filter_list_id;
    cmd->filter_offset = addWowCmd->filter_offset;
    cmd->filter_size = addWowCmd->filter_size;

    A_MEMCPY(cmd->filter, pattern, addWowCmd->filter_size);

    filter_mask = (A_UINT8*)(cmd->filter + cmd->filter_size);
    A_MEMCPY(filter_mask, mask, addWowCmd->filter_size);


    return (wmi_cmd_send(wmip, osbuf, WMI_ADD_WOW_PATTERN_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_del_wow_pattern_cmd(struct wmi_t *wmip,
                              WMI_DEL_WOW_PATTERN_CMD *delWowCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_DEL_WOW_PATTERN_CMD *cmd;

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_DEL_WOW_PATTERN_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, delWowCmd, sizeof(WMI_DEL_WOW_PATTERN_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_DEL_WOW_PATTERN_CMDID,
                            NO_SYNC_WMIFLAG));

}

A_STATUS
wmi_set_snr_threshold_params(struct wmi_t *wmip,
                             WMI_SNR_THRESHOLD_PARAMS_CMD *snrCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_SNR_THRESHOLD_PARAMS_CMD *cmd;
    /* These values are in ascending order */
    if( snrCmd->thresholdAbove4_Val <= snrCmd->thresholdAbove3_Val ||
        snrCmd->thresholdAbove3_Val <= snrCmd->thresholdAbove2_Val ||
        snrCmd->thresholdAbove2_Val <= snrCmd->thresholdAbove1_Val ||
        snrCmd->thresholdBelow4_Val <= snrCmd->thresholdBelow3_Val ||
        snrCmd->thresholdBelow3_Val <= snrCmd->thresholdBelow2_Val ||
        snrCmd->thresholdBelow2_Val <= snrCmd->thresholdBelow1_Val) {

        return A_EINVAL;
    }

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_SNR_THRESHOLD_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, snrCmd, sizeof(WMI_SNR_THRESHOLD_PARAMS_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_SNR_THRESHOLD_PARAMS_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_clr_rssi_snr(struct wmi_t *wmip)
{
    void    *osbuf;

    osbuf = A_NETBUF_ALLOC(sizeof(int));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_CLR_RSSI_SNR_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_lq_threshold_params(struct wmi_t *wmip,
                             WMI_LQ_THRESHOLD_PARAMS_CMD *lqCmd)
{
    void    *osbuf;
    A_INT8  size;
    WMI_LQ_THRESHOLD_PARAMS_CMD *cmd;
    /* These values are in ascending order */
    if( lqCmd->thresholdAbove4_Val <= lqCmd->thresholdAbove3_Val ||
        lqCmd->thresholdAbove3_Val <= lqCmd->thresholdAbove2_Val ||
        lqCmd->thresholdAbove2_Val <= lqCmd->thresholdAbove1_Val ||
        lqCmd->thresholdBelow4_Val <= lqCmd->thresholdBelow3_Val ||
        lqCmd->thresholdBelow3_Val <= lqCmd->thresholdBelow2_Val ||
        lqCmd->thresholdBelow2_Val <= lqCmd->thresholdBelow1_Val ) {

        return A_EINVAL;
    }

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_LQ_THRESHOLD_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);
    A_MEMCPY(cmd, lqCmd, sizeof(WMI_LQ_THRESHOLD_PARAMS_CMD));

    return (wmi_cmd_send(wmip, osbuf, WMI_LQ_THRESHOLD_PARAMS_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_error_report_bitmask(struct wmi_t *wmip, A_UINT32 mask)
{
    void    *osbuf;
    A_INT8  size;
    WMI_TARGET_ERROR_REPORT_BITMASK *cmd;

    size = sizeof (*cmd);

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_TARGET_ERROR_REPORT_BITMASK *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);

    cmd->bitmask = mask;

    return (wmi_cmd_send(wmip, osbuf, WMI_TARGET_ERROR_REPORT_BITMASK_CMDID,
                            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_challenge_resp_cmd(struct wmi_t *wmip, A_UINT32 cookie, A_UINT32 source)
{
    void *osbuf;
    WMIX_HB_CHALLENGE_RESP_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMIX_HB_CHALLENGE_RESP_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->cookie = cookie;
    cmd->source = source;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_HB_CHALLENGE_RESP_CMDID,
                              NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_config_debug_module_cmd(struct wmi_t *wmip, A_UINT16 mmask,
                            A_UINT16 tsr, A_BOOL rep, A_UINT16 size,
                            A_UINT32 valid)
{
    void *osbuf;
    WMIX_DBGLOG_CFG_MODULE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMIX_DBGLOG_CFG_MODULE_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->config.cfgmmask = mmask;
    cmd->config.cfgtsr = tsr;
    cmd->config.cfgrep = rep;
    cmd->config.cfgsize = size;
    cmd->config.cfgvalid = valid;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_DBGLOG_CFG_MODULE_CMDID,
                              NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_stats_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_STATISTICS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_addBadAp_cmd(struct wmi_t *wmip, A_UINT8 apIndex, A_UINT8 *bssid)
{
    void *osbuf;
    WMI_ADD_BAD_AP_CMD *cmd;

    if ((bssid == NULL) || (apIndex > WMI_MAX_BAD_AP_INDEX)) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_ADD_BAD_AP_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->badApIndex = apIndex;
    A_MEMCPY(cmd->bssid, bssid, sizeof(cmd->bssid));

    return (wmi_cmd_send(wmip, osbuf, WMI_ADD_BAD_AP_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_deleteBadAp_cmd(struct wmi_t *wmip, A_UINT8 apIndex)
{
    void *osbuf;
    WMI_DELETE_BAD_AP_CMD *cmd;

    if (apIndex > WMI_MAX_BAD_AP_INDEX) {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_DELETE_BAD_AP_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->badApIndex = apIndex;

    return (wmi_cmd_send(wmip, osbuf, WMI_DELETE_BAD_AP_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_txPwr_cmd(struct wmi_t *wmip, A_UINT8 dbM)
{
    void *osbuf;
    WMI_SET_TX_PWR_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_TX_PWR_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->dbM = dbM;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_TX_PWR_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_txPwr_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_TX_PWR_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_switch_radio(struct wmi_t *wmip, A_UINT8 on)
{
	WMI_SCAN_PARAMS_CMD scParams = {0, 0, 0, 0, 0,
					WMI_SHORTSCANRATIO_DEFAULT,
					DEFAULT_SCAN_CTRL_FLAGS,
					0};

	if (on) {
		/* Enable foreground scanning */
                if (wmi_scanparams_cmd(wmip, scParams.fg_start_period,
                                       scParams.fg_end_period,
                                       scParams.bg_period,
                                       scParams.minact_chdwell_time,
                                       scParams.maxact_chdwell_time,
                                       scParams.pas_chdwell_time,
                                       scParams.shortScanRatio,
                                       scParams.scanCtrlFlags,
                                       scParams.max_dfsch_act_time) != A_OK) {
			return -EIO;
		}
	} else {
		wmi_disconnect_cmd(wmip);
		if (wmi_scanparams_cmd(wmip, 0xFFFF, 0, 0, 0,
				       0, 0, 0, 0xFF, 0) != A_OK) {
			return -EIO;
		}
	}

	return A_OK;
}


A_UINT16
wmi_get_mapped_qos_queue(struct wmi_t *wmip, A_UINT8 trafficClass)
{
	A_UINT16 activeTsids=0;

    LOCK_WMI(wmip);
    activeTsids = wmip->wmi_streamExistsForAC[trafficClass];
    UNLOCK_WMI(wmip);

    return activeTsids;
}

A_STATUS
wmi_get_roam_tbl_cmd(struct wmi_t *wmip)
{
    void *osbuf;

    osbuf = A_NETBUF_ALLOC(0);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_ROAM_TBL_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_roam_data_cmd(struct wmi_t *wmip, A_UINT8 roamDataType)
{
    void *osbuf;
    A_UINT32 size = sizeof(A_UINT8);
    WMI_TARGET_ROAM_DATA *cmd;

    osbuf = A_NETBUF_ALLOC(size);      /* no payload */
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_TARGET_ROAM_DATA *)(A_NETBUF_DATA(osbuf));
    cmd->roamDataType = roamDataType;

    return (wmi_cmd_send(wmip, osbuf, WMI_GET_ROAM_DATA_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_roam_ctrl_cmd(struct wmi_t *wmip, WMI_SET_ROAM_CTRL_CMD *p,
                      A_UINT8 size)
{
    void *osbuf;
    WMI_SET_ROAM_CTRL_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_SET_ROAM_CTRL_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);

    A_MEMCPY(cmd, p, size);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_ROAM_CTRL_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_powersave_timers_cmd(struct wmi_t *wmip,
                            WMI_POWERSAVE_TIMERS_POLICY_CMD *pCmd,
                            A_UINT8 size)
{
    void *osbuf;
    WMI_POWERSAVE_TIMERS_POLICY_CMD *cmd;

    /* These timers can't be zero */
    if(!pCmd->psPollTimeout || !pCmd->triggerTimeout ||
       !(pCmd->apsdTimPolicy == IGNORE_TIM_ALL_QUEUES_APSD ||
         pCmd->apsdTimPolicy == PROCESS_TIM_ALL_QUEUES_APSD) ||
       !(pCmd->simulatedAPSDTimPolicy == IGNORE_TIM_SIMULATED_APSD ||
         pCmd->simulatedAPSDTimPolicy == PROCESS_TIM_SIMULATED_APSD))
        return A_EINVAL;

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, size);

    cmd = (WMI_POWERSAVE_TIMERS_POLICY_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, size);

    A_MEMCPY(cmd, pCmd, size);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_POWERSAVE_TIMERS_POLICY_CMDID,
                         NO_SYNC_WMIFLAG));
}

#ifdef CONFIG_HOST_GPIO_SUPPORT
/* Send a command to Target to change GPIO output pins. */
A_STATUS
wmi_gpio_output_set(struct wmi_t *wmip,
                    A_UINT32 set_mask,
                    A_UINT32 clear_mask,
                    A_UINT32 enable_mask,
                    A_UINT32 disable_mask)
{
    void *osbuf;
    WMIX_GPIO_OUTPUT_SET_CMD *output_set;
    int size;

    size = sizeof(*output_set);

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - set=0x%x clear=0x%x enb=0x%x dis=0x%x\n", DBGARG,
        set_mask, clear_mask, enable_mask, disable_mask));

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, size);
    output_set = (WMIX_GPIO_OUTPUT_SET_CMD *)(A_NETBUF_DATA(osbuf));

    output_set->set_mask                   = set_mask;
    output_set->clear_mask                 = clear_mask;
    output_set->enable_mask                = enable_mask;
    output_set->disable_mask               = disable_mask;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_GPIO_OUTPUT_SET_CMDID,
                             NO_SYNC_WMIFLAG));
}

/* Send a command to the Target requesting state of the GPIO input pins */
A_STATUS
wmi_gpio_input_get(struct wmi_t *wmip)
{
    void *osbuf;

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    osbuf = A_NETBUF_ALLOC(0);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_GPIO_INPUT_GET_CMDID,
                             NO_SYNC_WMIFLAG));
}

/* Send a command to the Target that changes the value of a GPIO register. */
A_STATUS
wmi_gpio_register_set(struct wmi_t *wmip,
                      A_UINT32 gpioreg_id,
                      A_UINT32 value)
{
    void *osbuf;
    WMIX_GPIO_REGISTER_SET_CMD *register_set;
    int size;

    size = sizeof(*register_set);

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - reg=%d value=0x%x\n", DBGARG, gpioreg_id, value));

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, size);
    register_set = (WMIX_GPIO_REGISTER_SET_CMD *)(A_NETBUF_DATA(osbuf));

    register_set->gpioreg_id               = gpioreg_id;
    register_set->value                    = value;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_GPIO_REGISTER_SET_CMDID,
                             NO_SYNC_WMIFLAG));
}

/* Send a command to the Target to fetch the value of a GPIO register. */
A_STATUS
wmi_gpio_register_get(struct wmi_t *wmip,
                      A_UINT32 gpioreg_id)
{
    void *osbuf;
    WMIX_GPIO_REGISTER_GET_CMD *register_get;
    int size;

    size = sizeof(*register_get);

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter - reg=%d\n", DBGARG, gpioreg_id));

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, size);
    register_get = (WMIX_GPIO_REGISTER_GET_CMD *)(A_NETBUF_DATA(osbuf));

    register_get->gpioreg_id               = gpioreg_id;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_GPIO_REGISTER_GET_CMDID,
                             NO_SYNC_WMIFLAG));
}

/* Send a command to the Target acknowledging some GPIO interrupts. */
A_STATUS
wmi_gpio_intr_ack(struct wmi_t *wmip,
                  A_UINT32 ack_mask)
{
    void *osbuf;
    WMIX_GPIO_INTR_ACK_CMD *intr_ack;
    int size;

    size = sizeof(*intr_ack);

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter ack_mask=0x%x\n", DBGARG, ack_mask));

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, size);
    intr_ack = (WMIX_GPIO_INTR_ACK_CMD *)(A_NETBUF_DATA(osbuf));

    intr_ack->ack_mask               = ack_mask;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_GPIO_INTR_ACK_CMDID,
                             NO_SYNC_WMIFLAG));
}
#endif /* CONFIG_HOST_GPIO_SUPPORT */

A_STATUS
wmi_set_access_params_cmd(struct wmi_t *wmip, A_UINT16 txop, A_UINT8 eCWmin,
                          A_UINT8 eCWmax, A_UINT8 aifsn)
{
    void *osbuf;
    WMI_SET_ACCESS_PARAMS_CMD *cmd;

    if ((eCWmin > WMI_MAX_CW_ACPARAM) || (eCWmax > WMI_MAX_CW_ACPARAM) ||
        (aifsn > WMI_MAX_AIFSN_ACPARAM))
    {
        return A_EINVAL;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_ACCESS_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->txop   = txop;
    cmd->eCWmin = eCWmin;
    cmd->eCWmax = eCWmax;
    cmd->aifsn  = aifsn;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_ACCESS_PARAMS_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_retry_limits_cmd(struct wmi_t *wmip, A_UINT8 frameType,
                         A_UINT8 trafficClass, A_UINT8 maxRetries,
                         A_UINT8 enableNotify)
{
    void *osbuf;
    WMI_SET_RETRY_LIMITS_CMD *cmd;

    if ((frameType != MGMT_FRAMETYPE) && (frameType != CONTROL_FRAMETYPE) &&
        (frameType != DATA_FRAMETYPE))
    {
        return A_EINVAL;
    }

    if (maxRetries > WMI_MAX_RETRIES) {
        return A_EINVAL;
    }

    if (frameType != DATA_FRAMETYPE) {
        trafficClass = 0;
    }

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_RETRY_LIMITS_CMD *)(A_NETBUF_DATA(osbuf));
    cmd->frameType    = frameType;
    cmd->trafficClass = trafficClass;
    cmd->maxRetries   = maxRetries;
    cmd->enableNotify = enableNotify;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_RETRY_LIMITS_CMDID,
                         NO_SYNC_WMIFLAG));
}

void
wmi_get_current_bssid(struct wmi_t *wmip, A_UINT8 *bssid)
{
    if (bssid != NULL) {
        A_MEMCPY(bssid, wmip->wmi_bssid, ATH_MAC_LEN);
    }
}

A_STATUS
wmi_set_opt_mode_cmd(struct wmi_t *wmip, A_UINT8 optMode)
{
    void *osbuf;
    WMI_SET_OPT_MODE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_OPT_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->optMode = optMode;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_OPT_MODE_CMDID,
                         SYNC_BOTH_WMIFLAG));
}

A_STATUS
wmi_opt_tx_frame_cmd(struct wmi_t *wmip,
                      A_UINT8 frmType,
                      A_UINT8 *dstMacAddr,
                      A_UINT8 *bssid,
                      A_UINT16 optIEDataLen,
                      A_UINT8 *optIEData)
{
    void *osbuf;
    WMI_OPT_TX_FRAME_CMD *cmd;
    osbuf = A_NETBUF_ALLOC(optIEDataLen + sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, (optIEDataLen + sizeof(*cmd)));

    cmd = (WMI_OPT_TX_FRAME_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, (optIEDataLen + sizeof(*cmd)-1));

    cmd->frmType    = frmType;
    cmd->optIEDataLen   = optIEDataLen;
    //cmd->optIEData     = (A_UINT8 *)((int)cmd + sizeof(*cmd));
    A_MEMCPY(cmd->bssid, bssid, sizeof(cmd->bssid));
    A_MEMCPY(cmd->dstAddr, dstMacAddr, sizeof(cmd->dstAddr));
    A_MEMCPY(&cmd->optIEData[0], optIEData, optIEDataLen);

    return (wmi_cmd_send(wmip, osbuf, WMI_OPT_TX_FRAME_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_adhoc_bconIntvl_cmd(struct wmi_t *wmip, A_UINT16 intvl)
{
    void *osbuf;
    WMI_BEACON_INT_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_BEACON_INT_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->beaconInterval = intvl;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BEACON_INT_CMDID,
            NO_SYNC_WMIFLAG));
}


A_STATUS
wmi_set_voice_pkt_size_cmd(struct wmi_t *wmip, A_UINT16 voicePktSize)
{
    void *osbuf;
    WMI_SET_VOICE_PKT_SIZE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_VOICE_PKT_SIZE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->voicePktSize = voicePktSize;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_VOICE_PKT_SIZE_CMDID,
            NO_SYNC_WMIFLAG));
}


A_STATUS
wmi_set_max_sp_len_cmd(struct wmi_t *wmip, A_UINT8 maxSPLen)
{
    void *osbuf;
    WMI_SET_MAX_SP_LEN_CMD *cmd;

    /* maxSPLen is a two-bit value. If user trys to set anything
     * other than this, then its invalid
     */
    if(maxSPLen & ~0x03)
        return  A_EINVAL;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_MAX_SP_LEN_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->maxSPLen = maxSPLen;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_MAX_SP_LEN_CMDID,
            NO_SYNC_WMIFLAG));
}

A_UINT8
convert_userPriority_to_trafficClass(A_UINT8 userPriority)
{
        return  (up_to_ac[userPriority & 0x7]);
}

A_UINT8
wmi_get_power_mode_cmd(struct wmi_t *wmip)
{
    return wmip->wmi_powerMode;
}

A_STATUS
wmi_verify_tspec_params(WMI_CREATE_PSTREAM_CMD *pCmd, A_BOOL tspecCompliance)
{
    return A_OK;
}

#ifdef CONFIG_HOST_TCMD_SUPPORT
static A_STATUS
wmi_tcmd_test_report_rx(struct wmi_t *wmip, A_UINT8 *datap, int len)
{

   A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

   A_WMI_TCMD_RX_REPORT_EVENT(wmip->wmi_devt, datap, len);

   return A_OK;
}

#endif /* CONFIG_HOST_TCMD_SUPPORT*/

A_STATUS
wmi_set_authmode_cmd(struct wmi_t *wmip, A_UINT8 mode)
{
    void *osbuf;
    WMI_SET_AUTH_MODE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_AUTH_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->mode = mode;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_AUTH_MODE_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_reassocmode_cmd(struct wmi_t *wmip, A_UINT8 mode)
{
    void *osbuf;
    WMI_SET_REASSOC_MODE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_REASSOC_MODE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->mode = mode;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_REASSOC_MODE_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_lpreamble_cmd(struct wmi_t *wmip, A_UINT8 status)
{
    void *osbuf;
    WMI_SET_LPREAMBLE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_LPREAMBLE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->status = status;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_LPREAMBLE_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_rts_cmd(struct wmi_t *wmip, A_UINT16 threshold)
{
    void *osbuf;
    WMI_SET_RTS_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_RTS_CMD*)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->threshold = threshold;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_RTS_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_wmm_cmd(struct wmi_t *wmip, WMI_WMM_STATUS status)
{
    void *osbuf;
    WMI_SET_WMM_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_WMM_CMD*)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->status = status;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_WMM_CMDID,
            NO_SYNC_WMIFLAG));

}

A_STATUS
wmi_set_wmm_txop(struct wmi_t *wmip, WMI_TXOP_CFG cfg)
{
    void *osbuf;
    WMI_SET_WMM_TXOP_CMD *cmd;

    if( !((cfg == WMI_TXOP_DISABLED) || (cfg == WMI_TXOP_ENABLED)) )
        return A_EINVAL;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_WMM_TXOP_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->txopEnable = cfg;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_WMM_TXOP_CMDID,
            NO_SYNC_WMIFLAG));

}

#ifdef CONFIG_HOST_TCMD_SUPPORT
/* WMI  layer doesn't need to know the data type of the test cmd.
   This would be beneficial for customers like Qualcomm, who might
   have different test command requirements from differnt manufacturers
 */
A_STATUS
wmi_test_cmd(struct wmi_t *wmip, A_UINT8 *buf, A_UINT32  len)
{
    void *osbuf;
    char *data;

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter\n", DBGARG));

    osbuf= A_NETBUF_ALLOC(len);
    if(osbuf == NULL)
    {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, len);
    data = A_NETBUF_DATA(osbuf);
    A_MEMCPY(data, buf, len);

    return(wmi_cmd_send(wmip, osbuf, WMI_TEST_CMDID,
         NO_SYNC_WMIFLAG));
}

#endif

A_STATUS
wmi_set_bt_status_cmd(struct wmi_t *wmip, A_UINT8 streamType, A_UINT8 status)
{
    void *osbuf;
    WMI_SET_BT_STATUS_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_BT_STATUS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->streamType = streamType;
    cmd->status = status;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BT_STATUS_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_bt_params_cmd(struct wmi_t *wmip, WMI_SET_BT_PARAMS_CMD* cmd)
{
    void *osbuf;
    WMI_SET_BT_PARAMS_CMD* alloc_cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    alloc_cmd = (WMI_SET_BT_PARAMS_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(alloc_cmd, sizeof(*cmd));
    A_MEMCPY(alloc_cmd, cmd, sizeof(*cmd));

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_BT_PARAMS_CMDID,
            NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_get_keepalive_configured(struct wmi_t *wmip)
{
    void *osbuf;
    WMI_GET_KEEPALIVE_CMD *cmd;
    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, sizeof(*cmd));
    cmd = (WMI_GET_KEEPALIVE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    return (wmi_cmd_send(wmip, osbuf, WMI_GET_KEEPALIVE_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_UINT8
wmi_get_keepalive_cmd(struct wmi_t *wmip)
{
    return wmip->wmi_keepaliveInterval;
}

A_STATUS
wmi_set_keepalive_cmd(struct wmi_t *wmip, A_UINT8 keepaliveInterval)
{
    void *osbuf;
    WMI_SET_KEEPALIVE_CMD *cmd;

    osbuf = A_NETBUF_ALLOC(sizeof(*cmd));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*cmd));

    cmd = (WMI_SET_KEEPALIVE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, sizeof(*cmd));
    cmd->keepaliveInterval = keepaliveInterval;
    wmip->wmi_keepaliveInterval = keepaliveInterval;

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_KEEPALIVE_CMDID,
                         NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_appie_cmd(struct wmi_t *wmip, A_UINT8 mgmtFrmType, A_UINT8 ieLen,
                  A_UINT8 *ieInfo)
{
    void *osbuf;
    WMI_SET_APPIE_CMD *cmd;
    A_UINT16 cmdLen;

    if (ieLen > WMI_MAX_IE_LEN) {
        return A_ERROR;
    }
    cmdLen = sizeof(*cmd) + ieLen - 1;
    osbuf = A_NETBUF_ALLOC(cmdLen);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, cmdLen);

    cmd = (WMI_SET_APPIE_CMD *)(A_NETBUF_DATA(osbuf));
    A_MEMZERO(cmd, cmdLen);

    cmd->mgmtFrmType = mgmtFrmType;
    cmd->ieLen = ieLen;
    A_MEMCPY(cmd->ieInfo, ieInfo, ieLen);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_APPIE_CMDID, NO_SYNC_WMIFLAG));
}

A_STATUS
wmi_set_halparam_cmd(struct wmi_t *wmip, A_UINT8 *cmd, A_UINT16 dataLen)
{
    void *osbuf;
    A_UINT8 *data;

    osbuf = A_NETBUF_ALLOC(dataLen);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, dataLen);

    data = A_NETBUF_DATA(osbuf);

    A_MEMCPY(data, cmd, dataLen);

    return (wmi_cmd_send(wmip, osbuf, WMI_SET_WHALPARAM_CMDID, NO_SYNC_WMIFLAG));
}

A_INT32
wmi_get_rate(A_INT8 rateindex)
{
    if (rateindex == RATE_AUTO) {
        return 0;
    } else {
        return(wmi_rateTable[(A_UINT32) rateindex]);
    }
}

void
wmi_node_return (struct wmi_t *wmip, bss_t *bss)
{
	if (NULL != bss)
	{
		wlan_node_return (&wmip->wmi_scan_table, bss);
	}
}

bss_t *
wmi_find_Ssidnode (struct wmi_t *wmip, A_UCHAR *pSsid,
				   A_UINT32 ssidLength, A_BOOL bIsWPA2)
{
	bss_t *node = NULL;
    node = wlan_find_Ssidnode (&wmip->wmi_scan_table, pSsid,
							   ssidLength, bIsWPA2);
	return node;
}

void
wmi_free_allnodes(struct wmi_t *wmip)
{
	wlan_free_allnodes(&wmip->wmi_scan_table);
}

bss_t *
wmi_find_node(struct wmi_t *wmip, const A_UINT8 *macaddr)
{
	bss_t *ni=NULL;
	ni=wlan_find_node(&wmip->wmi_scan_table,macaddr);
	return ni;
}

A_STATUS
wmi_dset_open_reply(struct wmi_t *wmip,
                    A_UINT32 status,
                    A_UINT32 access_cookie,
                    A_UINT32 dset_size,
                    A_UINT32 dset_version,
                    A_UINT32 targ_handle,
                    A_UINT32 targ_reply_fn,
                    A_UINT32 targ_reply_arg)
{
    void *osbuf;
    WMIX_DSETOPEN_REPLY_CMD *open_reply;

    A_DPRINTF(DBG_WMI, (DBGFMT "Enter - wmip=0x%x\n", DBGARG, (int)wmip));

    osbuf = A_NETBUF_ALLOC(sizeof(*open_reply));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    A_NETBUF_PUT(osbuf, sizeof(*open_reply));
    open_reply = (WMIX_DSETOPEN_REPLY_CMD *)(A_NETBUF_DATA(osbuf));

    open_reply->status                   = status;
    open_reply->targ_dset_handle         = targ_handle;
    open_reply->targ_reply_fn            = targ_reply_fn;
    open_reply->targ_reply_arg           = targ_reply_arg;
    open_reply->access_cookie            = access_cookie;
    open_reply->size                     = dset_size;
    open_reply->version                  = dset_version;

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_DSETOPEN_REPLY_CMDID,
                             NO_SYNC_WMIFLAG));
}

static A_STATUS
wmi_get_pmkid_list_event_rx(struct wmi_t *wmip, A_UINT8 *datap, A_UINT32 len)
{
    WMI_PMKID_LIST_REPLY *reply;
    A_UINT32 expected_len;

    if (len < sizeof(WMI_PMKID_LIST_REPLY)) {
        return A_EINVAL;
    }
    reply = (WMI_PMKID_LIST_REPLY *)datap;
    expected_len = sizeof(reply->numPMKID) + reply->numPMKID * WMI_PMKID_LEN;

    if (len < expected_len) {
        return A_EINVAL;
    }

    A_WMI_PMKID_LIST_EVENT(wmip->wmi_devt, reply->numPMKID,
                           reply->pmkidList);

    return A_OK;
}

#ifdef CONFIG_HOST_DSET_SUPPORT
A_STATUS
wmi_dset_data_reply(struct wmi_t *wmip,
                    A_UINT32 status,
                    A_UINT8 *user_buf,
                    A_UINT32 length,
                    A_UINT32 targ_buf,
                    A_UINT32 targ_reply_fn,
                    A_UINT32 targ_reply_arg)
{
    void *osbuf;
    WMIX_DSETDATA_REPLY_CMD *data_reply;
    int size;

    size = sizeof(*data_reply) + length;

    A_DPRINTF(DBG_WMI,
        (DBGFMT "Enter - length=%d status=%d\n", DBGARG, length, status));

    osbuf = A_NETBUF_ALLOC(size);
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }
    A_NETBUF_PUT(osbuf, size);
    data_reply = (WMIX_DSETDATA_REPLY_CMD *)(A_NETBUF_DATA(osbuf));

    data_reply->status                     = status;
    data_reply->targ_buf                   = targ_buf;
    data_reply->targ_reply_fn              = targ_reply_fn;
    data_reply->targ_reply_arg             = targ_reply_arg;
    data_reply->length                     = length;

    if (status == A_OK) {
        if (a_copy_from_user(data_reply->buf, user_buf, length)) {
            return A_ERROR;
        }
    }

    return (wmi_cmd_send_xtnd(wmip, osbuf, WMIX_DSETDATA_REPLY_CMDID,
                             NO_SYNC_WMIFLAG));
}
#endif /* CONFIG_HOST_DSET_SUPPORT */

A_STATUS
wmi_set_wsc_status_cmd(struct wmi_t *wmip, A_UINT32 status)
{
    void *osbuf;
    char *cmd;

	wps_enable = status;

    osbuf = a_netbuf_alloc(sizeof(1));
    if (osbuf == NULL) {
        return A_NO_MEMORY;
    }

    a_netbuf_put(osbuf, sizeof(1));

    cmd = (char *)(a_netbuf_to_data(osbuf));

    A_MEMZERO(cmd, sizeof(*cmd));
    cmd[0] = (status?1:0);
    return (wmi_cmd_send(wmip, osbuf, WMI_SET_WSC_STATUS_CMDID,
                         NO_SYNC_WMIFLAG));
}

