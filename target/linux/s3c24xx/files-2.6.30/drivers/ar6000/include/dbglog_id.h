/*
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
 * All rights reserved.
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 * This file contains the definitions of the debug identifiers for different
 * modules.
 *
 */

#ifndef _DBGLOG_ID_H_
#define _DBGLOG_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The nomenclature for the debug identifiers is MODULE_DESCRIPTION.
 * Please ensure that the definition of any new debugid introduced is captured
 * between the <MODULE>_DBGID_DEFINITION_START and
 * <MODULE>_DBGID_DEFINITION_END defines. The structure is required for the
 * parser to correctly pick up the values for different debug identifiers.
 */

/* INF debug identifier definitions */
#define INF_DBGID_DEFINITION_START
#define INF_ASSERTION_FAILED                          1
#define INF_TARGET_ID                                 2
#define INF_DBGID_DEFINITION_END

/* WMI debug identifier definitions */
#define WMI_DBGID_DEFINITION_START
#define WMI_CMD_RX_XTND_PKT_TOO_SHORT                 1
#define WMI_EXTENDED_CMD_NOT_HANDLED                  2
#define WMI_CMD_RX_PKT_TOO_SHORT                      3
#define WMI_CALLING_WMI_EXTENSION_FN                  4
#define WMI_CMD_NOT_HANDLED                           5
#define WMI_IN_SYNC                                   6
#define WMI_TARGET_WMI_SYNC_CMD                       7
#define WMI_SET_SNR_THRESHOLD_PARAMS                  8
#define WMI_SET_RSSI_THRESHOLD_PARAMS                 9
#define WMI_SET_LQ_TRESHOLD_PARAMS                   10
#define WMI_TARGET_CREATE_PSTREAM_CMD                11
#define WMI_WI_DTM_INUSE                             12
#define WMI_TARGET_DELETE_PSTREAM_CMD                13
#define WMI_TARGET_IMPLICIT_DELETE_PSTREAM_CMD       14
#define WMI_TARGET_GET_BIT_RATE_CMD                  15
#define WMI_GET_RATE_MASK_CMD_FIX_RATE_MASK_IS       16
#define WMI_TARGET_GET_AVAILABLE_CHANNELS_CMD        17
#define WMI_TARGET_GET_TX_PWR_CMD                    18
#define WMI_FREE_EVBUF_WMIBUF                        19
#define WMI_FREE_EVBUF_DATABUF                       20
#define WMI_FREE_EVBUF_BADFLAG                       21
#define WMI_HTC_RX_ERROR_DATA_PACKET                 22
#define WMI_HTC_RX_SYNC_PAUSING_FOR_MBOX             23
#define WMI_INCORRECT_WMI_DATA_HDR_DROPPING_PKT      24
#define WMI_SENDING_READY_EVENT                      25
#define WMI_SETPOWER_MDOE_TO_MAXPERF                 26
#define WMI_SETPOWER_MDOE_TO_REC                     27
#define WMI_BSSINFO_EVENT_FROM                       28
#define WMI_TARGET_GET_STATS_CMD                     29
#define WMI_SENDING_SCAN_COMPLETE_EVENT              30
#define WMI_SENDING_RSSI_INDB_THRESHOLD_EVENT        31
#define WMI_SENDING_RSSI_INDBM_THRESHOLD_EVENT       32
#define WMI_SENDING_LINK_QUALITY_THRESHOLD_EVENT     33
#define WMI_SENDING_ERROR_REPORT_EVENT               34
#define WMI_SENDING_CAC_EVENT                        35
#define WMI_TARGET_GET_ROAM_TABLE_CMD                36
#define WMI_TARGET_GET_ROAM_DATA_CMD                 37
#define WMI_SENDING_GPIO_INTR_EVENT                  38
#define WMI_SENDING_GPIO_ACK_EVENT                   39
#define WMI_SENDING_GPIO_DATA_EVENT                  40
#define WMI_CMD_RX                                   41
#define WMI_CMD_RX_XTND                              42
#define WMI_EVENT_SEND                               43
#define WMI_EVENT_SEND_XTND                          44
#define WMI_DBGID_DEFINITION_END

/* CSERV debug identifier definitions */
#define CSERV_DBGID_DEFINITION_START
#define CSERV_BEGIN_SCAN1                             1
#define CSERV_BEGIN_SCAN2                             2
#define CSERV_END_SCAN1                               3
#define CSERV_END_SCAN2                               4
#define CSERV_CHAN_SCAN_START                         5
#define CSERV_CHAN_SCAN_STOP                          6
#define CSERV_CHANNEL_OPPPORTUNITY                    7
#define CSERV_NC_TIMEOUT                              8
#define CSERV_BACK_HOME                              10
#define CSERV_CHMGR_CH_CALLBACK1                     11
#define CSERV_CHMGR_CH_CALLBACK2                     12
#define CSERV_CHMGR_CH_CALLBACK3                     13
#define CSERV_SET_SCAN_PARAMS1                       14
#define CSERV_SET_SCAN_PARAMS2                       15
#define CSERV_SET_SCAN_PARAMS3                       16
#define CSERV_SET_SCAN_PARAMS4                       17
#define CSERV_ABORT_SCAN                             18
#define CSERV_NEWSTATE                               19
#define CSERV_MINCHMGR_OP_END                        20
#define CSERV_CHMGR_OP_END                           21
#define CSERV_DISCONNECT_TIMEOUT                     22
#define CSERV_ROAM_TIMEOUT                           23
#define CSERV_FORCE_SCAN1                            24
#define CSERV_FORCE_SCAN2                            25
#define CSERV_FORCE_SCAN3                            26
#define CSERV_UTIL_TIMEOUT                           27
#define CSERV_RSSIPOLLER                             28
#define CSERV_RETRY_CONNECT_TIMEOUT                  29
#define CSERV_RSSIINDBMPOLLER                        30
#define CSERV_BGSCAN_ENABLE                          31
#define CSERV_BGSCAN_DISABLE                         32
#define CSERV_WLAN_START_SCAN_CMD1                   33
#define CSERV_WLAN_START_SCAN_CMD2                   34
#define CSERV_WLAN_START_SCAN_CMD3                   35
#define CSERV_START_SCAN_CMD                         36
#define CSERV_START_FORCE_SCAN                       37
#define CSERV_NEXT_CHAN                              38
#define CSERV_SET_REGCODE                            39
#define CSERV_START_ADHOC                            40
#define CSERV_ADHOC_AT_HOME                          41
#define CSERV_OPT_AT_HOME                            42
#define CSERV_WLAN_CONNECT_CMD                       43
#define CSERV_WLAN_RECONNECT_CMD                     44
#define CSERV_WLAN_DISCONNECT_CMD                    45
#define CSERV_BSS_CHANGE_CHANNEL                     46
#define CSERV_BEACON_RX                              47
#define CSERV_KEEPALIVE_CHECK                        48
#define CSERV_RC_BEGIN_SCAN                          49
#define CSERV_RC_SCAN_START                          50
#define CSERV_RC_SCAN_STOP                           51
#define CSERV_RC_NEXT                                52
#define CSERV_RC_SCAN_END                            53
#define CSERV_PROBE_CALLBACK                         54
#define CSERV_ROAM1                                  55
#define CSERV_ROAM2                                  56
#define CSERV_ROAM3                                  57
#define CSERV_CONNECT_EVENT                          58
#define CSERV_DISCONNECT_EVENT                       59
#define CSERV_BMISS_HANDLER1                         60
#define CSERV_BMISS_HANDLER2                         61
#define CSERV_BMISS_HANDLER3                         62
#define CSERV_LOWRSSI_HANDLER                        63
#define CSERV_WLAN_SET_PMKID_CMD                     64
#define CSERV_RECONNECT_REQUEST                      65
#define CSERV_KEYSPLUMBED_EVENT                      66
#define CSERV_NEW_REG                                67
#define CSERV_SET_RSSI_THOLD                         68
#define CSERV_RSSITHRESHOLDCHECK                     69
#define CSERV_RSSIINDBMTHRESHOLDCHECK                70
#define CSERV_WLAN_SET_OPT_CMD1                      71
#define CSERV_WLAN_SET_OPT_CMD2                      72
#define CSERV_WLAN_SET_OPT_CMD3                      73
#define CSERV_WLAN_SET_OPT_CMD4                      74
#define CSERV_SCAN_CONNECT_STOP                      75
#define CSERV_BMISS_HANDLER4                         76
#define CSERV_INITIALIZE_TIMER                       77
#define CSERV_ARM_TIMER                              78
#define CSERV_DISARM_TIMER                           79
#define CSERV_UNINITIALIZE_TIMER                     80
#define CSERV_DISCONNECT_EVENT2                      81
#define CSERV_SCAN_CONNECT_START                     82
#define CSERV_BSSINFO_MEMORY_ALLOC_FAILED            83
#define CSERV_SET_SCAN_PARAMS5                       84
#define CSERV_DBGID_DEFINITION_END

/* TXRX debug identifier definitions */
#define TXRX_TXBUF_DBGID_DEFINITION_START
#define TXRX_TXBUF_ALLOCATE_BUF                      1
#define TXRX_TXBUF_QUEUE_BUF_TO_MBOX                 2
#define TXRX_TXBUF_QUEUE_BUF_TO_TXQ                  3
#define TXRX_TXBUF_TXQ_DEPTH                         4
#define TXRX_TXBUF_IBSS_QUEUE_TO_SFQ                 5
#define TXRX_TXBUF_IBSS_QUEUE_TO_TXQ_FRM_SFQ         6
#define TXRX_TXBUF_INITIALIZE_TIMER                  7
#define TXRX_TXBUF_ARM_TIMER                         8
#define TXRX_TXBUF_DISARM_TIMER                      9
#define TXRX_TXBUF_UNINITIALIZE_TIMER                10
#define TXRX_TXBUF_DBGID_DEFINITION_END

#define TXRX_RXBUF_DBGID_DEFINITION_START
#define TXRX_RXBUF_ALLOCATE_BUF                      1
#define TXRX_RXBUF_QUEUE_TO_HOST                     2
#define TXRX_RXBUF_QUEUE_TO_WLAN                     3
#define TXRX_RXBUF_ZERO_LEN_BUF                      4
#define TXRX_RXBUF_QUEUE_TO_HOST_LASTBUF_IN_RXCHAIN  5
#define TXRX_RXBUF_LASTBUF_IN_RXCHAIN_ZEROBUF        6
#define TXRX_RXBUF_QUEUE_EMPTY_QUEUE_TO_WLAN         7
#define TXRX_RXBUF_SEND_TO_RECV_MGMT                 8
#define TXRX_RXBUF_SEND_TO_IEEE_LAYER                9
#define TXRX_RXBUF_DBGID_DEFINITION_END

#define TXRX_MGMTBUF_DBGID_DEFINITION_START
#define TXRX_MGMTBUF_ALLOCATE_BUF                    1
#define TXRX_MGMTBUF_ALLOCATE_SM_BUF                 2
#define TXRX_MGMTBUF_ALLOCATE_RMBUF                  3
#define TXRX_MGMTBUF_GET_BUF                         4
#define TXRX_MGMTBUF_GET_SM_BUF                      5
#define TXRX_MGMTBUF_QUEUE_BUF_TO_TXQ                6
#define TXRX_MGMTBUF_REAPED_BUF                      7
#define TXRX_MGMTBUF_REAPED_SM_BUF                   8
#define TXRX_MGMTBUF_WAIT_FOR_TXQ_DRAIN              9
#define TXRX_MGMTBUF_WAIT_FOR_TXQ_SFQ_DRAIN          10
#define TXRX_MGMTBUF_ENQUEUE_INTO_SFQ                11
#define TXRX_MGMTBUF_DEQUEUE_FROM_SFQ                12
#define TXRX_MGMTBUF_PAUSE_TXQ                       13
#define TXRX_MGMTBUF_RESUME_TXQ                      14
#define TXRX_MGMTBUF_WAIT_FORTXQ_DRAIN_TIMEOUT       15
#define TXRX_MGMTBUF_DRAINQ                          16
#define TXRX_MGMTBUF_INDICATE_Q_DRAINED              17
#define TXRX_MGMTBUF_DBGID_DEFINITION_END

/* PM (Power Module) debug identifier definitions */
#define PM_DBGID_DEFINITION_START
#define PM_INIT                                      1
#define PM_ENABLE                                    2
#define PM_SET_STATE                                 3
#define PM_SET_POWERMODE                             4
#define PM_CONN_NOTIFY                               5
#define PM_REF_COUNT_NEGATIVE                        6
#define PM_APSD_ENABLE                               7
#define PM_UPDATE_APSD_STATE                         8
#define PM_CHAN_OP_REQ                               9
#define PM_SET_MY_BEACON_POLICY                      10
#define PM_SET_ALL_BEACON_POLICY                     11
#define PM_SET_PM_PARAMS1                            12
#define PM_SET_PM_PARAMS2                            13
#define PM_ADHOC_SET_PM_CAPS_FAIL                    14
#define PM_ADHOC_UNKNOWN_IBSS_ATTRIB_ID              15
#define PM_DBGID_DEFINITION_END

/* Wake on Wireless debug identifier definitions */
#define WOW_DBGID_DEFINITION_START
#define WOW_INIT                                        1
#define WOW_GET_CONFIG_DSET                             2
#define WOW_NO_CONFIG_DSET                              3
#define WOW_INVALID_CONFIG_DSET                         4
#define WOW_USE_DEFAULT_CONFIG                          5
#define WOW_SETUP_GPIO                                  6
#define WOW_INIT_DONE                                   7
#define WOW_SET_GPIO_PIN                                8
#define WOW_CLEAR_GPIO_PIN                              9
#define WOW_SET_WOW_MODE_CMD                            10
#define WOW_SET_HOST_MODE_CMD                           11
#define WOW_ADD_WOW_PATTERN_CMD                         12
#define WOW_NEW_WOW_PATTERN_AT_INDEX                    13
#define WOW_DEL_WOW_PATTERN_CMD                         14
#define WOW_LIST_CONTAINS_PATTERNS                      15
#define WOW_GET_WOW_LIST_CMD                            16
#define WOW_INVALID_FILTER_ID                           17
#define WOW_INVALID_FILTER_LISTID                       18
#define WOW_NO_VALID_FILTER_AT_ID                       19
#define WOW_NO_VALID_LIST_AT_ID                         20
#define WOW_NUM_PATTERNS_EXCEEDED                       21
#define WOW_NUM_LISTS_EXCEEDED                          22
#define WOW_GET_WOW_STATS                               23
#define WOW_CLEAR_WOW_STATS                             24
#define WOW_WAKEUP_HOST                                 25
#define WOW_EVENT_WAKEUP_HOST                           26
#define WOW_EVENT_DISCARD                               27
#define WOW_PATTERN_MATCH                               28
#define WOW_PATTERN_NOT_MATCH                           29
#define WOW_PATTERN_NOT_MATCH_OFFSET                    30
#define WOW_DISABLED_HOST_ASLEEP                        31
#define WOW_ENABLED_HOST_ASLEEP_NO_PATTERNS             32
#define WOW_ENABLED_HOST_ASLEEP_NO_MATCH_FOUND          33
#define WOW_DBGID_DEFINITION_END

/* WHAL debug identifier definitions */
#define WHAL_DBGID_DEFINITION_START
#define WHAL_ERROR_ANI_CONTROL                      1
#define WHAL_ERROR_CHIP_TEST1                       2
#define WHAL_ERROR_CHIP_TEST2                       3
#define WHAL_ERROR_EEPROM_CHECKSUM                  4
#define WHAL_ERROR_EEPROM_MACADDR                   5
#define WHAL_ERROR_INTERRUPT_HIU                    6
#define WHAL_ERROR_KEYCACHE_RESET                   7
#define WHAL_ERROR_KEYCACHE_SET                     8
#define WHAL_ERROR_KEYCACHE_TYPE                    9
#define WHAL_ERROR_KEYCACHE_TKIPENTRY              10
#define WHAL_ERROR_KEYCACHE_WEPLENGTH              11
#define WHAL_ERROR_PHY_INVALID_CHANNEL             12
#define WHAL_ERROR_POWER_AWAKE                     13
#define WHAL_ERROR_POWER_SET                       14
#define WHAL_ERROR_RECV_STOPDMA                    15
#define WHAL_ERROR_RECV_STOPPCU                    16
#define WHAL_ERROR_RESET_CHANNF1                   17
#define WHAL_ERROR_RESET_CHANNF2                   18
#define WHAL_ERROR_RESET_PM                        19
#define WHAL_ERROR_RESET_OFFSETCAL                 20
#define WHAL_ERROR_RESET_RFGRANT                   21
#define WHAL_ERROR_RESET_RXFRAME                   22
#define WHAL_ERROR_RESET_STOPDMA                   23
#define WHAL_ERROR_RESET_RECOVER                   24
#define WHAL_ERROR_XMIT_COMPUTE                    25
#define WHAL_ERROR_XMIT_NOQUEUE                    26
#define WHAL_ERROR_XMIT_ACTIVEQUEUE                27
#define WHAL_ERROR_XMIT_BADTYPE                    28
#define WHAL_DBGID_DEFINITION_END

#ifdef __cplusplus
}
#endif

#endif /* _DBGLOG_ID_H_ */
