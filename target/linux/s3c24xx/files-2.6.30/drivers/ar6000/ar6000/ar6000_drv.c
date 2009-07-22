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

/*
 * This driver is a pseudo ethernet driver to access the Atheros AR6000
 * WLAN Device
 */
static const char athId[] __attribute__ ((unused)) = "$Id: //depot/sw/releases/olca2.0-GPL/host/os/linux/ar6000_drv.c#2 $";

#include "ar6000_drv.h"
#include "htc.h"

MODULE_LICENSE("GPL and additional rights");

#ifndef REORG_APTC_HEURISTICS
#undef ADAPTIVE_POWER_THROUGHPUT_CONTROL
#endif /* REORG_APTC_HEURISTICS */

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
#define APTC_TRAFFIC_SAMPLING_INTERVAL     100  /* msec */
#define APTC_UPPER_THROUGHPUT_THRESHOLD    3000 /* Kbps */
#define APTC_LOWER_THROUGHPUT_THRESHOLD    2000 /* Kbps */

typedef struct aptc_traffic_record {
    A_BOOL timerScheduled;
    struct timeval samplingTS;
    unsigned long bytesReceived;
    unsigned long bytesTransmitted;
} APTC_TRAFFIC_RECORD;

A_TIMER aptcTimer;
APTC_TRAFFIC_RECORD aptcTR;
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

unsigned int bypasswmi = 0;
unsigned int debuglevel = 0;
int tspecCompliance = 1;
unsigned int busspeedlow = 0;
unsigned int onebitmode = 0;
unsigned int skipflash = 0;
unsigned int wmitimeout = 2;
unsigned int wlanNodeCaching = 1;
unsigned int enableuartprint = 0;
unsigned int logWmiRawMsgs = 0;
unsigned int enabletimerwar = 0;
unsigned int mbox_yield_limit = 99;
int reduce_credit_dribble = 1 + HTC_CONNECT_FLAGS_THRESHOLD_LEVEL_ONE_HALF;
int allow_trace_signal = 0;
#ifdef CONFIG_HOST_TCMD_SUPPORT
unsigned int testmode =0;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param(bypasswmi, int, 0644);
module_param(debuglevel, int, 0644);
module_param(tspecCompliance, int, 0644);
module_param(onebitmode, int, 0644);
module_param(busspeedlow, int, 0644);
module_param(skipflash, int, 0644);
module_param(wmitimeout, int, 0644);
module_param(wlanNodeCaching, int, 0644);
module_param(logWmiRawMsgs, int, 0644);
module_param(enableuartprint, int, 0644);
module_param(enabletimerwar, int, 0644);
module_param(mbox_yield_limit, int, 0644);
module_param(reduce_credit_dribble, int, 0644);
module_param(allow_trace_signal, int, 0644);
#ifdef CONFIG_HOST_TCMD_SUPPORT
module_param(testmode, int, 0644);
#endif
#else

#define __user
/* for linux 2.4 and lower */
MODULE_PARM(bypasswmi,"i");
MODULE_PARM(debuglevel, "i");
MODULE_PARM(onebitmode,"i");
MODULE_PARM(busspeedlow, "i");
MODULE_PARM(skipflash, "i");
MODULE_PARM(wmitimeout, "i");
MODULE_PARM(wlanNodeCaching, "i");
MODULE_PARM(enableuartprint,"i");
MODULE_PARM(logWmiRawMsgs, "i");
MODULE_PARM(enabletimerwar,"i");
MODULE_PARM(mbox_yield_limit,"i");
MODULE_PARM(reduce_credit_dribble,"i");
MODULE_PARM(allow_trace_signal,"i");
#ifdef CONFIG_HOST_TCMD_SUPPORT
MODULE_PARM(testmode, "i");
#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,10)
/* in 2.6.10 and later this is now a pointer to a uint */
unsigned int _mboxnum = HTC_MAILBOX_NUM_MAX;
#define mboxnum &_mboxnum
#else
unsigned int mboxnum = HTC_MAILBOX_NUM_MAX;
#endif

#ifdef CONFIG_AR6000_WLAN_RESET
unsigned int resetok = 1;
#else
unsigned int resetok = 0;
#endif

#ifdef DEBUG
A_UINT32 g_dbg_flags = DBG_DEFAULTS;
unsigned int debugflags = 0;
int debugdriver = 1;
unsigned int debughtc = 128;
unsigned int debugbmi = 1;
unsigned int debughif = 2;
unsigned int txcreditsavailable[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int txcreditsconsumed[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int txcreditintrenable[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int txcreditintrenableaggregate[HTC_MAILBOX_NUM_MAX] = {0};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param(debugflags, int, 0644);
module_param(debugdriver, int, 0644);
module_param(debughtc, int, 0644);
module_param(debugbmi, int, 0644);
module_param(debughif, int, 0644);
module_param(resetok, int, 0644);
module_param_array(txcreditsavailable, int, mboxnum, 0644);
module_param_array(txcreditsconsumed, int, mboxnum, 0644);
module_param_array(txcreditintrenable, int, mboxnum, 0644);
module_param_array(txcreditintrenableaggregate, int, mboxnum, 0644);
#else
/* linux 2.4 and lower */
MODULE_PARM(debugflags,"i");
MODULE_PARM(debugdriver, "i");
MODULE_PARM(debughtc, "i");
MODULE_PARM(debugbmi, "i");
MODULE_PARM(debughif, "i");
MODULE_PARM(resetok, "i");
MODULE_PARM(txcreditsavailable, "0-3i");
MODULE_PARM(txcreditsconsumed, "0-3i");
MODULE_PARM(txcreditintrenable, "0-3i");
MODULE_PARM(txcreditintrenableaggregate, "0-3i");
#endif

#endif /* DEBUG */

unsigned int tx_attempt[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int tx_post[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int tx_complete[HTC_MAILBOX_NUM_MAX] = {0};
unsigned int hifBusRequestNumMax = 40;
unsigned int war23838_disabled = 0;
#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
unsigned int enableAPTCHeuristics = 1;
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param_array(tx_attempt, int, mboxnum, 0644);
module_param_array(tx_post, int, mboxnum, 0644);
module_param_array(tx_complete, int, mboxnum, 0644);
module_param(hifBusRequestNumMax, int, 0644);
module_param(war23838_disabled, int, 0644);
#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
module_param(enableAPTCHeuristics, int, 0644);
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */
#else
MODULE_PARM(tx_attempt, "0-3i");
MODULE_PARM(tx_post, "0-3i");
MODULE_PARM(tx_complete, "0-3i");
MODULE_PARM(hifBusRequestNumMax, "i");
MODULE_PARM(war23838_disabled, "i");
#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
MODULE_PARM(enableAPTCHeuristics, "i");
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */
#endif

#ifdef BLOCK_TX_PATH_FLAG
int blocktx = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param(blocktx, int, 0644);
#else
MODULE_PARM(blocktx, "i");
#endif
#endif /* BLOCK_TX_PATH_FLAG */

// TODO move to arsoft_c
USER_RSSI_THOLD rssi_map[12];

int reconnect_flag = 0;

DECLARE_WAIT_QUEUE_HEAD(ar6000_scan_queue);

/* Function declarations */
static int ar6000_init_module(void);
static void ar6000_cleanup_module(void);

int ar6000_init(struct net_device *dev);
static int ar6000_open(struct net_device *dev);
static int ar6000_close(struct net_device *dev);
static int ar6000_cleanup(struct net_device *dev);
static void ar6000_init_control_info(AR_SOFTC_T *ar);
static int ar6000_data_tx(struct sk_buff *skb, struct net_device *dev);

static void ar6000_destroy(struct net_device *dev, unsigned int unregister);
static void ar6000_detect_error(unsigned long ptr);
static struct net_device_stats *ar6000_get_stats(struct net_device *dev);
static struct iw_statistics *ar6000_get_iwstats(struct net_device * dev);

/*
 * HTC service connection handlers
 */
static void ar6000_avail_ev(HTC_HANDLE HTCHandle);

static void ar6000_unavail_ev(void *Instance);

static void ar6000_target_failure(void *Instance, A_STATUS Status);

static void ar6000_rx(void *Context, HTC_PACKET *pPacket);

static void ar6000_rx_refill(void *Context,HTC_ENDPOINT_ID Endpoint);

static void ar6000_tx_complete(void *Context, HTC_PACKET *pPacket);

static void ar6000_tx_queue_full(void *Context, HTC_ENDPOINT_ID Endpoint);

static void ar6000_tx_queue_avail(void *Context, HTC_ENDPOINT_ID Endpoint);

/*
 * Static variables
 */

static struct net_device *ar6000_devices[MAX_AR6000];
extern struct iw_handler_def ath_iw_handler_def;
DECLARE_WAIT_QUEUE_HEAD(arEvent);
static void ar6000_cookie_init(AR_SOFTC_T *ar);
static void ar6000_cookie_cleanup(AR_SOFTC_T *ar);
static void ar6000_free_cookie(AR_SOFTC_T *ar, struct ar_cookie * cookie);
static struct ar_cookie *ar6000_alloc_cookie(AR_SOFTC_T *ar);
static void ar6000_TxDataCleanup(AR_SOFTC_T *ar);

#ifdef USER_KEYS
static A_STATUS ar6000_reinstall_keys(AR_SOFTC_T *ar,A_UINT8 key_op_ctrl);
#endif


static struct ar_cookie s_ar_cookie_mem[MAX_COOKIE_NUM];

#define HOST_INTEREST_ITEM_ADDRESS(ar, item)    \
((ar->arTargetType == TARGET_TYPE_AR6001) ?     \
   AR6001_HOST_INTEREST_ITEM_ADDRESS(item) :    \
   AR6002_HOST_INTEREST_ITEM_ADDRESS(item))


/* Debug log support */

/*
 * Flag to govern whether the debug logs should be parsed in the kernel
 * or reported to the application.
 */
#ifdef DEBUG
#define REPORT_DEBUG_LOGS_TO_APP
#endif

A_STATUS
ar6000_set_host_app_area(AR_SOFTC_T *ar)
{
    A_UINT32 address, data;
    struct host_app_area_s host_app_area;

    /* Fetch the address of the host_app_area_s instance in the host interest area */
    address = HOST_INTEREST_ITEM_ADDRESS(ar, hi_app_host_interest);
    if (ar6000_ReadRegDiag(ar->arHifDevice, &address, &data) != A_OK) {
        return A_ERROR;
    }
    address = data;
    host_app_area.wmi_protocol_ver = WMI_PROTOCOL_VERSION;
    if (ar6000_WriteDataDiag(ar->arHifDevice, address,
                             (A_UCHAR *)&host_app_area,
                             sizeof(struct host_app_area_s)) != A_OK)
    {
        return A_ERROR;
    }

    return A_OK;
}

A_UINT32
dbglog_get_debug_hdr_ptr(AR_SOFTC_T *ar)
{
    A_UINT32 param;
    A_UINT32 address;
    A_STATUS status;

    address = HOST_INTEREST_ITEM_ADDRESS(ar, hi_dbglog_hdr);
    if ((status = ar6000_ReadDataDiag(ar->arHifDevice, address,
                                      (A_UCHAR *)&param, 4)) != A_OK)
    {
        param = 0;
    }

    return param;
}

/*
 * The dbglog module has been initialized. Its ok to access the relevant
 * data stuctures over the diagnostic window.
 */
void
ar6000_dbglog_init_done(AR_SOFTC_T *ar)
{
    ar->dbglog_init_done = TRUE;
}

A_UINT32
dbglog_get_debug_fragment(A_INT8 *datap, A_UINT32 len, A_UINT32 limit)
{
    A_INT32 *buffer;
    A_UINT32 count;
    A_UINT32 numargs;
    A_UINT32 length;
    A_UINT32 fraglen;

    count = fraglen = 0;
    buffer = (A_INT32 *)datap;
    length = (limit >> 2);

    if (len <= limit) {
        fraglen = len;
    } else {
        while (count < length) {
            numargs = DBGLOG_GET_NUMARGS(buffer[count]);
            fraglen = (count << 2);
            count += numargs + 1;
        }
    }

    return fraglen;
}

void
dbglog_parse_debug_logs(A_INT8 *datap, A_UINT32 len)
{
    A_INT32 *buffer;
    A_UINT32 count;
    A_UINT32 timestamp;
    A_UINT32 debugid;
    A_UINT32 moduleid;
    A_UINT32 numargs;
    A_UINT32 length;

    count = 0;
    buffer = (A_INT32 *)datap;
    length = (len >> 2);
    while (count < length) {
        debugid = DBGLOG_GET_DBGID(buffer[count]);
        moduleid = DBGLOG_GET_MODULEID(buffer[count]);
        numargs = DBGLOG_GET_NUMARGS(buffer[count]);
        timestamp = DBGLOG_GET_TIMESTAMP(buffer[count]);
        switch (numargs) {
            case 0:
            AR_DEBUG_PRINTF("%d %d (%d)\n", moduleid, debugid, timestamp);
            break;

            case 1:
            AR_DEBUG_PRINTF("%d %d (%d): 0x%x\n", moduleid, debugid,
                            timestamp, buffer[count+1]);
            break;

            case 2:
            AR_DEBUG_PRINTF("%d %d (%d): 0x%x, 0x%x\n", moduleid, debugid,
                            timestamp, buffer[count+1], buffer[count+2]);
            break;

            default:
            AR_DEBUG_PRINTF("Invalid args: %d\n", numargs);
        }
        count += numargs + 1;
    }
}

int
ar6000_dbglog_get_debug_logs(AR_SOFTC_T *ar)
{
    struct dbglog_hdr_s debug_hdr;
    struct dbglog_buf_s debug_buf;
    A_UINT32 address;
    A_UINT32 length;
    A_UINT32 dropped;
    A_UINT32 firstbuf;
    A_UINT32 debug_hdr_ptr;

    if (!ar->dbglog_init_done) return A_ERROR;

#ifndef CONFIG_AR6000_WLAN_DEBUG
    return 0;
#endif

    AR6000_SPIN_LOCK(&ar->arLock, 0);

    if (ar->dbgLogFetchInProgress) {
        AR6000_SPIN_UNLOCK(&ar->arLock, 0);
        return A_EBUSY;
    }

        /* block out others */
    ar->dbgLogFetchInProgress = TRUE;

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    debug_hdr_ptr = dbglog_get_debug_hdr_ptr(ar);
    printk("debug_hdr_ptr: 0x%x\n", debug_hdr_ptr);

    /* Get the contents of the ring buffer */
    if (debug_hdr_ptr) {
        address = debug_hdr_ptr;
        length = sizeof(struct dbglog_hdr_s);
        ar6000_ReadDataDiag(ar->arHifDevice, address,
                            (A_UCHAR *)&debug_hdr, length);
        address = (A_UINT32)debug_hdr.dbuf;
        firstbuf = address;
        dropped = debug_hdr.dropped;
        length = sizeof(struct dbglog_buf_s);
        ar6000_ReadDataDiag(ar->arHifDevice, address,
                            (A_UCHAR *)&debug_buf, length);

        do {
            address = (A_UINT32)debug_buf.buffer;
            length = debug_buf.length;
            if ((length) && (debug_buf.length <= debug_buf.bufsize)) {
                /* Rewind the index if it is about to overrun the buffer */
                if (ar->log_cnt > (DBGLOG_HOST_LOG_BUFFER_SIZE - length)) {
                    ar->log_cnt = 0;
                }
                if(A_OK != ar6000_ReadDataDiag(ar->arHifDevice, address,
                                    (A_UCHAR *)&ar->log_buffer[ar->log_cnt], length))
                {
                    break;
                }
                ar6000_dbglog_event(ar, dropped, &ar->log_buffer[ar->log_cnt], length);
                ar->log_cnt += length;
            } else {
                AR_DEBUG_PRINTF("Length: %d (Total size: %d)\n",
                                debug_buf.length, debug_buf.bufsize);
            }

            address = (A_UINT32)debug_buf.next;
            length = sizeof(struct dbglog_buf_s);
            if(A_OK != ar6000_ReadDataDiag(ar->arHifDevice, address,
                                (A_UCHAR *)&debug_buf, length))
            {
                break;
            }

        } while (address != firstbuf);
    }

    ar->dbgLogFetchInProgress = FALSE;

    return A_OK;
}

void
ar6000_dbglog_event(AR_SOFTC_T *ar, A_UINT32 dropped,
                    A_INT8 *buffer, A_UINT32 length)
{
#ifdef REPORT_DEBUG_LOGS_TO_APP
    #define MAX_WIRELESS_EVENT_SIZE 252
    /*
     * Break it up into chunks of MAX_WIRELESS_EVENT_SIZE bytes of messages.
     * There seems to be a limitation on the length of message that could be
     * transmitted to the user app via this mechanism.
     */
    A_UINT32 send, sent;

    sent = 0;
    send = dbglog_get_debug_fragment(&buffer[sent], length - sent,
                                     MAX_WIRELESS_EVENT_SIZE);
    while (send) {
        ar6000_send_event_to_app(ar, WMIX_DBGLOG_EVENTID, &buffer[sent], send);
        sent += send;
        send = dbglog_get_debug_fragment(&buffer[sent], length - sent,
                                         MAX_WIRELESS_EVENT_SIZE);
    }
#else
    AR_DEBUG_PRINTF("Dropped logs: 0x%x\nDebug info length: %d\n",
                    dropped, length);

    /* Interpret the debug logs */
    dbglog_parse_debug_logs(buffer, length);
#endif /* REPORT_DEBUG_LOGS_TO_APP */
}



static int __init
ar6000_init_module(void)
{
    static int probed = 0;
    A_STATUS status;
    HTC_INIT_INFO initInfo;

    A_MEMZERO(&initInfo,sizeof(initInfo));
    initInfo.AddInstance = ar6000_avail_ev;
    initInfo.DeleteInstance = ar6000_unavail_ev;
    initInfo.TargetFailure = ar6000_target_failure;


#ifdef DEBUG
    /* Set the debug flags if specified at load time */
    if(debugflags != 0)
    {
        g_dbg_flags = debugflags;
    }
#endif

    if (probed) {
        return -ENODEV;
    }
    probed++;

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
    memset(&aptcTR, 0, sizeof(APTC_TRAFFIC_RECORD));
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

#ifdef CONFIG_HOST_GPIO_SUPPORT
    ar6000_gpio_init();
#endif /* CONFIG_HOST_GPIO_SUPPORT */

    status = HTCInit(&initInfo);
    if(status != A_OK)
        return -ENODEV;

    return 0;
}

static void __exit
ar6000_cleanup_module(void)
{
    int i = 0;
    struct net_device *ar6000_netdev;

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
    /* Delete the Adaptive Power Control timer */
    if (timer_pending(&aptcTimer)) {
        del_timer_sync(&aptcTimer);
    }
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

    for (i=0; i < MAX_AR6000; i++) {
        if (ar6000_devices[i] != NULL) {
            ar6000_netdev = ar6000_devices[i];
            ar6000_devices[i] = NULL;
            ar6000_destroy(ar6000_netdev, 1);
        }
    }

        /* shutting down HTC will cause the HIF layer to detach from the
         * underlying bus driver which will cause the subsequent deletion of
         * all HIF and HTC instances */
    HTCShutDown();

    AR_DEBUG_PRINTF("ar6000_cleanup: success\n");
}

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
void
aptcTimerHandler(unsigned long arg)
{
    A_UINT32 numbytes;
    A_UINT32 throughput;
    AR_SOFTC_T *ar;
    A_STATUS status;

    ar = (AR_SOFTC_T *)arg;
    A_ASSERT(ar != NULL);
    A_ASSERT(!timer_pending(&aptcTimer));

    AR6000_SPIN_LOCK(&ar->arLock, 0);

    /* Get the number of bytes transferred */
    numbytes = aptcTR.bytesTransmitted + aptcTR.bytesReceived;
    aptcTR.bytesTransmitted = aptcTR.bytesReceived = 0;

    /* Calculate and decide based on throughput thresholds */
    throughput = ((numbytes * 8)/APTC_TRAFFIC_SAMPLING_INTERVAL); /* Kbps */
    if (throughput < APTC_LOWER_THROUGHPUT_THRESHOLD) {
        /* Enable Sleep and delete the timer */
        A_ASSERT(ar->arWmiReady == TRUE);
        AR6000_SPIN_UNLOCK(&ar->arLock, 0);
        status = wmi_powermode_cmd(ar->arWmi, REC_POWER);
        AR6000_SPIN_LOCK(&ar->arLock, 0);
        A_ASSERT(status == A_OK);
        aptcTR.timerScheduled = FALSE;
    } else {
        A_TIMEOUT_MS(&aptcTimer, APTC_TRAFFIC_SAMPLING_INTERVAL, 0);
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
}
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */



/* set HTC block size, assume BMI is already initialized */
A_STATUS ar6000_SetHTCBlockSize(AR_SOFTC_T *ar)
{
    A_STATUS status;
    A_UINT32 blocksizes[HTC_MAILBOX_NUM_MAX];

    do {
            /* get the block sizes */
        status = HIFConfigureDevice(ar->arHifDevice, HIF_DEVICE_GET_MBOX_BLOCK_SIZE,
                                    blocksizes, sizeof(blocksizes));

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF("Failed to get block size info from HIF layer...\n");
            break;
        }
            /* note: we actually get the block size for mailbox 1, for SDIO the block
             * size on mailbox 0 is artificially set to 1 */
            /* must be a power of 2 */
        A_ASSERT((blocksizes[1] & (blocksizes[1] - 1)) == 0);

            /* set the host interest area for the block size */
        status = BMIWriteMemory(ar->arHifDevice,
                                HOST_INTEREST_ITEM_ADDRESS(ar, hi_mbox_io_block_sz),
                                (A_UCHAR *)&blocksizes[1],
                                4);

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF("BMIWriteMemory for IO block size failed \n");
            break;
        }

        AR_DEBUG_PRINTF("Block Size Set: %d (target address:0x%X)\n",
                blocksizes[1], HOST_INTEREST_ITEM_ADDRESS(ar, hi_mbox_io_block_sz));

            /* set the host interest area for the mbox ISR yield limit */
        status = BMIWriteMemory(ar->arHifDevice,
                                HOST_INTEREST_ITEM_ADDRESS(ar, hi_mbox_isr_yield_limit),
                                (A_UCHAR *)&mbox_yield_limit,
                                4);

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF("BMIWriteMemory for yield limit failed \n");
            break;
        }

    } while (FALSE);

    return status;
}

static void free_raw_buffers(AR_SOFTC_T *ar)
{
    int i, j;

    for (i = 0; i != HTC_RAW_STREAM_NUM_MAX; i++) {
	for (j = 0; j != RAW_HTC_READ_BUFFERS_NUM; j++)
	    kfree(ar->raw_htc_read_buffer[i][j]);
	for (j = 0; j != RAW_HTC_WRITE_BUFFERS_NUM; j++)
	    kfree(ar->raw_htc_write_buffer[i][j]);
    }
}

static int alloc_raw_buffers(AR_SOFTC_T *ar)
{
    int i, j;
    raw_htc_buffer *b;

    for (i = 0; i != HTC_RAW_STREAM_NUM_MAX; i++) {
	for (j = 0; j != RAW_HTC_READ_BUFFERS_NUM; j++) {
	    b = kzalloc(sizeof(*b), GFP_KERNEL);
	    if (!b)
		return -ENOMEM;
	    ar->raw_htc_read_buffer[i][j] = b;
	}
	for (j = 0; j != RAW_HTC_WRITE_BUFFERS_NUM; j++) {
	    b = kzalloc(sizeof(*b), GFP_KERNEL);
	    if (!b)
		return -ENOMEM;
	    ar->raw_htc_write_buffer[i][j] = b;
	}
    }
    return 0;
}

static const struct net_device_ops ar6000_netdev_ops = {
    .ndo_init = &ar6000_init,
    .ndo_open = &ar6000_open,
    .ndo_stop = &ar6000_close,
    .ndo_start_xmit = &ar6000_data_tx,
    .ndo_get_stats = &ar6000_get_stats,
    .ndo_do_ioctl = &ar6000_ioctl,
};
/*
 * HTC Event handlers
 */
static void
ar6000_avail_ev(HTC_HANDLE HTCHandle)
{
    int i;
    struct net_device *dev;
    AR_SOFTC_T *ar;
    int device_index = 0;

    AR_DEBUG_PRINTF("ar6000_available\n");

    for (i=0; i < MAX_AR6000; i++) {
        if (ar6000_devices[i] == NULL) {
            break;
        }
    }

    if (i == MAX_AR6000) {
        AR_DEBUG_PRINTF("ar6000_available: max devices reached\n");
        return;
    }

    /* Save this. It gives a bit better readability especially since */
    /* we use another local "i" variable below.                      */
    device_index = i;

    A_ASSERT(HTCHandle != NULL);

    dev = alloc_etherdev(sizeof(AR_SOFTC_T));
    if (dev == NULL) {
        AR_DEBUG_PRINTF("ar6000_available: can't alloc etherdev\n");
        return;
    }

    ether_setup(dev);

    if (netdev_priv(dev) == NULL) {
        printk(KERN_CRIT "ar6000_available: Could not allocate memory\n");
        return;
    }

    A_MEMZERO(netdev_priv(dev), sizeof(AR_SOFTC_T));

    ar                       = (AR_SOFTC_T *)netdev_priv(dev);
    ar->arNetDev             = dev;
    ar->arHtcTarget          = HTCHandle;
    ar->arHifDevice          = HTCGetHifDevice(HTCHandle);
    ar->arWlanState          = WLAN_ENABLED;
    ar->arRadioSwitch        = WLAN_ENABLED;
    ar->arDeviceIndex        = device_index;

    A_INIT_TIMER(&ar->arHBChallengeResp.timer, ar6000_detect_error, dev);
    ar->arHBChallengeResp.seqNum = 0;
    ar->arHBChallengeResp.outstanding = FALSE;
    ar->arHBChallengeResp.missCnt = 0;
    ar->arHBChallengeResp.frequency = AR6000_HB_CHALLENGE_RESP_FREQ_DEFAULT;
    ar->arHBChallengeResp.missThres = AR6000_HB_CHALLENGE_RESP_MISS_THRES_DEFAULT;

    ar6000_init_control_info(ar);
    init_waitqueue_head(&arEvent);
    sema_init(&ar->arSem, 1);

    if (alloc_raw_buffers(ar)) {
	free_raw_buffers(ar);
	/*
	 * @@@ Clean up our own mess, but for anything else, cheerfully mimick
	 * the beautiful error non-handling of the rest of this function.
	 */
	return;
    }

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
    A_INIT_TIMER(&aptcTimer, aptcTimerHandler, ar);
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

    /*
     * If requested, perform some magic which requires no cooperation from
     * the Target.  It causes the Target to ignore flash and execute to the
     * OS from ROM.
     *
     * This is intended to support recovery from a corrupted flash on Targets
     * that support flash.
     */
    if (skipflash)
    {
        ar6000_reset_device_skipflash(ar->arHifDevice);
            }

    BMIInit();
    {
        struct bmi_target_info targ_info;

        if (BMIGetTargetInfo(ar->arHifDevice, &targ_info) != A_OK) {
            return;
        }

        ar->arVersion.target_ver = targ_info.target_ver;
        ar->arTargetType = targ_info.target_type;
    }

    if (enableuartprint) {
        A_UINT32 param;
        param = 1;
        if (BMIWriteMemory(ar->arHifDevice,
                           HOST_INTEREST_ITEM_ADDRESS(ar, hi_serial_enable),
                           (A_UCHAR *)&param,
                           4)!= A_OK)
        {
             AR_DEBUG_PRINTF("BMIWriteMemory for enableuartprint failed \n");
             return ;
        }
        AR_DEBUG_PRINTF("Serial console prints enabled\n");
    }
#ifdef CONFIG_HOST_TCMD_SUPPORT
    if(testmode) {
        ar->arTargetMode = AR6000_TCMD_MODE;
    }else {
        ar->arTargetMode = AR6000_WLAN_MODE;
    }
#endif
    if (enabletimerwar) {
        A_UINT32 param;

        if (BMIReadMemory(ar->arHifDevice,
            HOST_INTEREST_ITEM_ADDRESS(ar, hi_option_flag),
            (A_UCHAR *)&param,
            4)!= A_OK)
        {
            AR_DEBUG_PRINTF("BMIReadMemory for enabletimerwar failed \n");
            return;
        }

        param |= HI_OPTION_TIMER_WAR;

        if (BMIWriteMemory(ar->arHifDevice,
            HOST_INTEREST_ITEM_ADDRESS(ar, hi_option_flag),
            (A_UCHAR *)&param,
            4) != A_OK)
        {
            AR_DEBUG_PRINTF("BMIWriteMemory for enabletimerwar failed \n");
            return;
        }
        AR_DEBUG_PRINTF("Timer WAR enabled\n");
    }


        /* since BMIInit is called in the driver layer, we have to set the block
         * size here for the target */

    if (A_FAILED(ar6000_SetHTCBlockSize(ar))) {
        return;
    }

    spin_lock_init(&ar->arLock);

    dev->netdev_ops = &ar6000_netdev_ops;
    dev->watchdog_timeo = AR6000_TX_TIMEOUT;
    ar6000_ioctl_iwsetup(&ath_iw_handler_def);
    dev->wireless_handlers = &ath_iw_handler_def;
    ath_iw_handler_def.get_wireless_stats = ar6000_get_iwstats; /*Displayed via proc fs */

    /*
     * We need the OS to provide us with more headroom in order to
     * perform dix to 802.3, WMI header encap, and the HTC header
     */
    dev->hard_header_len = ETH_HLEN + sizeof(ATH_LLC_SNAP_HDR) +
        sizeof(WMI_DATA_HDR) + HTC_HEADER_LEN;

    /* This runs the init function */
    SET_NETDEV_DEV(dev, HIFGetOSDevice(ar->arHifDevice));
    if (register_netdev(dev)) {
        AR_DEBUG_PRINTF("ar6000_avail: register_netdev failed\n");
        ar6000_destroy(dev, 0);
        return;
    }

    HTCSetInstance(ar->arHtcTarget, ar);

    /* We only register the device in the global list if we succeed. */
    /* If the device is in the global list, it will be destroyed     */
    /* when the module is unloaded.                                  */
    ar6000_devices[device_index] = dev;

    AR_DEBUG_PRINTF("ar6000_avail: name=%s htcTarget=0x%x, dev=0x%x (%d), ar=0x%x\n",
                    dev->name, (A_UINT32)HTCHandle, (A_UINT32)dev, device_index,
                    (A_UINT32)ar);
}

static void ar6000_target_failure(void *Instance, A_STATUS Status)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)Instance;
    WMI_TARGET_ERROR_REPORT_EVENT errEvent;
    static A_BOOL sip = FALSE;

    if (Status != A_OK) {
        if (timer_pending(&ar->arHBChallengeResp.timer)) {
            A_UNTIMEOUT(&ar->arHBChallengeResp.timer);
        }

        /* try dumping target assertion information (if any) */
        ar6000_dump_target_assert_info(ar->arHifDevice,ar->arTargetType);

        /*
         * Fetch the logs from the target via the diagnostic
         * window.
         */
        ar6000_dbglog_get_debug_logs(ar);

        /* Report the error only once */
        if (!sip) {
            sip = TRUE;
            errEvent.errorVal = WMI_TARGET_COM_ERR |
                                WMI_TARGET_FATAL_ERR;
#ifdef SEND_EVENT_TO_APP
            ar6000_send_event_to_app(ar, WMI_ERROR_REPORT_EVENTID,
                                     (A_UINT8 *)&errEvent,
                                     sizeof(WMI_TARGET_ERROR_REPORT_EVENT));
#endif
        }
    }
}

static void
ar6000_unavail_ev(void *Instance)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)Instance;
        /* NULL out it's entry in the global list */
    ar6000_devices[ar->arDeviceIndex] = NULL;
    ar6000_destroy(ar->arNetDev, 1);
}

/*
 * We need to differentiate between the surprise and planned removal of the
 * device because of the following consideration:
 * - In case of surprise removal, the hcd already frees up the pending
 *   for the device and hence there is no need to unregister the function
 *   driver inorder to get these requests. For planned removal, the function
 *   driver has to explictly unregister itself to have the hcd return all the
 *   pending requests before the data structures for the devices are freed up.
 *   Note that as per the current implementation, the function driver will
 *   end up releasing all the devices since there is no API to selectively
 *   release a particular device.
 * - Certain commands issued to the target can be skipped for surprise
 *   removal since they will anyway not go through.
 */
static void
ar6000_destroy(struct net_device *dev, unsigned int unregister)
{
    AR_SOFTC_T *ar;

    AR_DEBUG_PRINTF("+ar6000_destroy \n");

    if((dev == NULL) || ((ar = netdev_priv(dev)) == NULL))
    {
        AR_DEBUG_PRINTF("%s(): Failed to get device structure.\n", __func__);
        return;
    }

    /* Clear the tx counters */
    memset(tx_attempt, 0, sizeof(tx_attempt));
    memset(tx_post, 0, sizeof(tx_post));
    memset(tx_complete, 0, sizeof(tx_complete));

    /* Free up the device data structure */
    if (unregister) {
	unregister_netdev(dev);
    } else {
	ar6000_close(dev);
    ar6000_cleanup(dev);
    }

    free_raw_buffers(ar);

#ifndef free_netdev
    kfree(dev);
#else
    free_netdev(dev);
#endif

    AR_DEBUG_PRINTF("-ar6000_destroy \n");
}

static void ar6000_detect_error(unsigned long ptr)
{
    struct net_device *dev = (struct net_device *)ptr;
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_TARGET_ERROR_REPORT_EVENT errEvent;

    AR6000_SPIN_LOCK(&ar->arLock, 0);

    if (ar->arHBChallengeResp.outstanding) {
        ar->arHBChallengeResp.missCnt++;
    } else {
        ar->arHBChallengeResp.missCnt = 0;
    }

    if (ar->arHBChallengeResp.missCnt > ar->arHBChallengeResp.missThres) {
        /* Send Error Detect event to the application layer and do not reschedule the error detection module timer */
        ar->arHBChallengeResp.missCnt = 0;
        ar->arHBChallengeResp.seqNum = 0;
        errEvent.errorVal = WMI_TARGET_COM_ERR | WMI_TARGET_FATAL_ERR;
        AR6000_SPIN_UNLOCK(&ar->arLock, 0);
#ifdef SEND_EVENT_TO_APP
        ar6000_send_event_to_app(ar, WMI_ERROR_REPORT_EVENTID,
                                 (A_UINT8 *)&errEvent,
                                 sizeof(WMI_TARGET_ERROR_REPORT_EVENT));
#endif
        return;
    }

    /* Generate the sequence number for the next challenge */
    ar->arHBChallengeResp.seqNum++;
    ar->arHBChallengeResp.outstanding = TRUE;

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    /* Send the challenge on the control channel */
    if (wmi_get_challenge_resp_cmd(ar->arWmi, ar->arHBChallengeResp.seqNum, DRV_HB_CHALLENGE) != A_OK) {
        AR_DEBUG_PRINTF("Unable to send heart beat challenge\n");
    }


    /* Reschedule the timer for the next challenge */
    A_TIMEOUT_MS(&ar->arHBChallengeResp.timer, ar->arHBChallengeResp.frequency * 1000, 0);
}

void ar6000_init_profile_info(AR_SOFTC_T *ar)
{
    ar->arSsidLen            = 0;
    A_MEMZERO(ar->arSsid, sizeof(ar->arSsid));
    ar->arNetworkType        = INFRA_NETWORK;
    ar->arDot11AuthMode      = OPEN_AUTH;
    ar->arAuthMode           = NONE_AUTH;
    ar->arPairwiseCrypto     = NONE_CRYPT;
    ar->arPairwiseCryptoLen  = 0;
    ar->arGroupCrypto        = NONE_CRYPT;
    ar->arGroupCryptoLen     = 0;
    A_MEMZERO(ar->arWepKeyList, sizeof(ar->arWepKeyList));
    A_MEMZERO(ar->arReqBssid, sizeof(ar->arReqBssid));
    A_MEMZERO(ar->arBssid, sizeof(ar->arBssid));
    ar->arBssChannel = 0;
}

static void
ar6000_init_control_info(AR_SOFTC_T *ar)
{
    ar->arWmiEnabled         = FALSE;
    ar6000_init_profile_info(ar);
    ar->arDefTxKeyIndex      = 0;
    A_MEMZERO(ar->arWepKeyList, sizeof(ar->arWepKeyList));
    ar->arChannelHint        = 0;
    ar->arListenInterval     = MAX_LISTEN_INTERVAL;
    ar->arVersion.host_ver   = AR6K_SW_VERSION;
    ar->arRssi               = 0;
    ar->arTxPwr              = 0;
    ar->arTxPwrSet           = FALSE;
    ar->arSkipScan           = 0;
    ar->arBeaconInterval     = 0;
    ar->arBitRate            = 0;
    ar->arMaxRetries         = 0;
    ar->arWmmEnabled         = TRUE;
}

static int
ar6000_open(struct net_device *dev)
{
    /* Wake up the queues */
    netif_start_queue(dev);

    return 0;
}

static int
ar6000_close(struct net_device *dev)
{
    /* Stop the transmit queues */ 
    netif_stop_queue(dev); 
    return 0; 
}

static int 
ar6000_cleanup(struct net_device *dev)
{
    AR_SOFTC_T *ar = netdev_priv(dev);

    /* Stop the transmit queues */
    netif_stop_queue(dev);

    /* Disable the target and the interrupts associated with it */
    if (ar->arWmiReady == TRUE)
    {
        if (!bypasswmi)
        {
            if (ar->arConnected == TRUE || ar->arConnectPending == TRUE)
            {
                AR_DEBUG_PRINTF("%s(): Disconnect\n", __func__);
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                ar6000_init_profile_info(ar);
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                wmi_disconnect_cmd(ar->arWmi);
            }

            ar6000_dbglog_get_debug_logs(ar);
            ar->arWmiReady  = FALSE;
            ar->arConnected = FALSE;
            ar->arConnectPending = FALSE;
            wmi_shutdown(ar->arWmi);
            ar->arWmiEnabled = FALSE;
            ar->arWmi = NULL;
            ar->arWlanState = WLAN_ENABLED;
#ifdef USER_KEYS
            ar->user_savedkeys_stat = USER_SAVEDKEYS_STAT_INIT;
            ar->user_key_ctrl      = 0;
#endif
        }

         AR_DEBUG_PRINTF("%s(): WMI stopped\n", __func__);
    }
    else
    {
        AR_DEBUG_PRINTF("%s(): WMI not ready 0x%08x 0x%08x\n",
            __func__, (unsigned int) ar, (unsigned int) ar->arWmi);

        /* Shut down WMI if we have started it */
        if(ar->arWmiEnabled == TRUE)
        {
            AR_DEBUG_PRINTF("%s(): Shut down WMI\n", __func__);
            wmi_shutdown(ar->arWmi);
            ar->arWmiEnabled = FALSE;
            ar->arWmi = NULL;
        }
    }

    /* stop HTC */
    HTCStop(ar->arHtcTarget);

    /* set the instance to NULL so we do not get called back on remove incase we
     * we're explicity destroyed by module unload */
    HTCSetInstance(ar->arHtcTarget, NULL);

    if (resetok) {
        /* try to reset the device if we can
         * The driver may have been configure NOT to reset the target during
         * a debug session */
        AR_DEBUG_PRINTF(" Attempting to reset target on instance destroy.... \n");
        ar6000_reset_device(ar->arHifDevice, ar->arTargetType);
    } else {
        AR_DEBUG_PRINTF(" Host does not want target reset. \n");
    }

       /* Done with cookies */
    ar6000_cookie_cleanup(ar);

    /* Cleanup BMI */
    BMIInit();

    return 0;
}

/* connect to a service */
static A_STATUS ar6000_connectservice(AR_SOFTC_T               *ar,
                                      HTC_SERVICE_CONNECT_REQ  *pConnect,
                                      WMI_PRI_STREAM_ID        WmiStreamID,
                                      char                     *pDesc)
{
    A_STATUS                 status;
    HTC_SERVICE_CONNECT_RESP response;

    do {

        A_MEMZERO(&response,sizeof(response));

        status = HTCConnectService(ar->arHtcTarget,
                                   pConnect,
                                   &response);

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF(" Failed to connect to %s service status:%d \n", pDesc, status);
            break;
        }

        if (WmiStreamID == WMI_NOT_MAPPED) {
                /* done */
            break;
        }

            /* set endpoint mapping for the WMI stream in the driver layer */
        arSetWMIStream2EndpointIDMap(ar,WmiStreamID,response.Endpoint);

    } while (FALSE);

    return status;
}

static void ar6000_TxDataCleanup(AR_SOFTC_T *ar)
{
        /* flush all the data (non-control) streams
         * we only flush packets that are tagged as data, we leave any control packets that
         * were in the TX queues alone */
    HTCFlushEndpoint(ar->arHtcTarget,
                     arWMIStream2EndpointID(ar,WMI_BEST_EFFORT_PRI),
                     AR6K_DATA_PKT_TAG);
    HTCFlushEndpoint(ar->arHtcTarget,
                     arWMIStream2EndpointID(ar,WMI_LOW_PRI),
                     AR6K_DATA_PKT_TAG);
    HTCFlushEndpoint(ar->arHtcTarget,
                     arWMIStream2EndpointID(ar,WMI_HIGH_PRI),
                     AR6K_DATA_PKT_TAG);
    HTCFlushEndpoint(ar->arHtcTarget,
                     arWMIStream2EndpointID(ar,WMI_HIGHEST_PRI),
                     AR6K_DATA_PKT_TAG);
}

/* This function does one time initialization for the lifetime of the device */
int ar6000_init(struct net_device *dev)
{
    AR_SOFTC_T *ar;
    A_STATUS    status;
    A_INT32     timeleft;

    if((ar = netdev_priv(dev)) == NULL)
    {
        return(-EIO);
    }

    /* Do we need to finish the BMI phase */
    if(BMIDone(ar->arHifDevice) != A_OK)
    {
        return -EIO;
    }

    if (!bypasswmi)
    {
#if 0 /* TBDXXX */
        if (ar->arVersion.host_ver != ar->arVersion.target_ver) {
            A_PRINTF("WARNING: Host version 0x%x does not match Target "
                    " version 0x%x!\n",
                    ar->arVersion.host_ver, ar->arVersion.target_ver);
        }
#endif

        /* Indicate that WMI is enabled (although not ready yet) */
        ar->arWmiEnabled = TRUE;
        if ((ar->arWmi = wmi_init((void *) ar)) == NULL)
        {
            AR_DEBUG_PRINTF("%s() Failed to initialize WMI.\n", __func__);
            return(-EIO);
        }

        AR_DEBUG_PRINTF("%s() Got WMI @ 0x%08x.\n", __func__,
            (unsigned int) ar->arWmi);
    }

    do {
        HTC_SERVICE_CONNECT_REQ connect;

            /* the reason we have to wait for the target here is that the driver layer
             * has to init BMI in order to set the host block size,
             */
        status = HTCWaitTarget(ar->arHtcTarget);

        if (A_FAILED(status)) {
            break;
        }

        A_MEMZERO(&connect,sizeof(connect));
            /* meta data is unused for now */
        connect.pMetaData = NULL;
        connect.MetaDataLength = 0;
            /* these fields are the same for all service endpoints */
        connect.EpCallbacks.pContext = ar;
        connect.EpCallbacks.EpTxComplete = ar6000_tx_complete;
        connect.EpCallbacks.EpRecv = ar6000_rx;
        connect.EpCallbacks.EpRecvRefill = ar6000_rx_refill;
        connect.EpCallbacks.EpSendFull = ar6000_tx_queue_full;
        connect.EpCallbacks.EpSendAvail = ar6000_tx_queue_avail;
            /* set the max queue depth so that our ar6000_tx_queue_full handler gets called.
             * Linux has the peculiarity of not providing flow control between the
             * NIC and the network stack. There is no API to indicate that a TX packet
             * was sent which could provide some back pressure to the network stack.
             * Under linux you would have to wait till the network stack consumed all sk_buffs
             * before any back-flow kicked in. Which isn't very friendly.
             * So we have to manage this ourselves */
        connect.MaxSendQueueDepth = 32;

            /* connect to control service */
        connect.ServiceID = WMI_CONTROL_SVC;
        status = ar6000_connectservice(ar,
                                       &connect,
                                       WMI_CONTROL_PRI,
                                       "WMI CONTROL");
        if (A_FAILED(status)) {
            break;
        }

            /* for the remaining data services set the connection flag to reduce dribbling,
             * if configured to do so */
        if (reduce_credit_dribble) {
            connect.ConnectionFlags |= HTC_CONNECT_FLAGS_REDUCE_CREDIT_DRIBBLE;
            /* the credit dribble trigger threshold is (reduce_credit_dribble - 1) for a value
             * of 0-3 */
            connect.ConnectionFlags &= ~HTC_CONNECT_FLAGS_THRESHOLD_LEVEL_MASK;
            connect.ConnectionFlags |=
                        ((A_UINT16)reduce_credit_dribble - 1) & HTC_CONNECT_FLAGS_THRESHOLD_LEVEL_MASK;
        }
            /* connect to best-effort service */
        connect.ServiceID = WMI_DATA_BE_SVC;

        status = ar6000_connectservice(ar,
                                       &connect,
                                       WMI_BEST_EFFORT_PRI,
                                       "WMI DATA BE");
        if (A_FAILED(status)) {
            break;
        }

            /* connect to back-ground
             * map this to WMI LOW_PRI */
        connect.ServiceID = WMI_DATA_BK_SVC;
        status = ar6000_connectservice(ar,
                                       &connect,
                                       WMI_LOW_PRI,
                                       "WMI DATA BK");
        if (A_FAILED(status)) {
            break;
        }

            /* connect to Video service, map this to
             * to HI PRI */
        connect.ServiceID = WMI_DATA_VI_SVC;
        status = ar6000_connectservice(ar,
                                       &connect,
                                       WMI_HIGH_PRI,
                                       "WMI DATA VI");
        if (A_FAILED(status)) {
            break;
        }

            /* connect to VO service, this is currently not
             * mapped to a WMI priority stream due to historical reasons.
             * WMI originally defined 3 priorities over 3 mailboxes
             * We can change this when WMI is reworked so that priorities are not
             * dependent on mailboxes */
        connect.ServiceID = WMI_DATA_VO_SVC;
        status = ar6000_connectservice(ar,
                                       &connect,
                                       WMI_HIGHEST_PRI,
                                       "WMI DATA VO");
        if (A_FAILED(status)) {
            break;
        }

        A_ASSERT(arWMIStream2EndpointID(ar,WMI_CONTROL_PRI) != 0);
        A_ASSERT(arWMIStream2EndpointID(ar,WMI_BEST_EFFORT_PRI) != 0);
        A_ASSERT(arWMIStream2EndpointID(ar,WMI_LOW_PRI) != 0);
        A_ASSERT(arWMIStream2EndpointID(ar,WMI_HIGH_PRI) != 0);
        A_ASSERT(arWMIStream2EndpointID(ar,WMI_HIGHEST_PRI) != 0);
    } while (FALSE);

    if (A_FAILED(status)) {
        return (-EIO);
    }

    /*
     * give our connected endpoints some buffers
     */
    ar6000_rx_refill(ar, arWMIStream2EndpointID(ar,WMI_CONTROL_PRI));

    ar6000_rx_refill(ar, arWMIStream2EndpointID(ar,WMI_BEST_EFFORT_PRI));

    /*
     * We will post the receive buffers only for SPE testing and so we are
     * making it conditional on the 'bypasswmi' flag.
     */
    if (bypasswmi) {
        ar6000_rx_refill(ar,arWMIStream2EndpointID(ar,WMI_LOW_PRI));
        ar6000_rx_refill(ar,arWMIStream2EndpointID(ar,WMI_HIGH_PRI));
    }

        /* setup credit distribution */
    ar6000_setup_credit_dist(ar->arHtcTarget, &ar->arCreditStateInfo);

    /* Since cookies are used for HTC transports, they should be */
    /* initialized prior to enabling HTC.                        */
    ar6000_cookie_init(ar);

    /* start HTC */
    status = HTCStart(ar->arHtcTarget);

    if (status != A_OK) {
        if (ar->arWmiEnabled == TRUE) {
            wmi_shutdown(ar->arWmi);
            ar->arWmiEnabled = FALSE;
            ar->arWmi = NULL;
        }
        ar6000_cookie_cleanup(ar);
        return -EIO;
    }

    if (!bypasswmi) {
        /* Wait for Wmi event to be ready */
        timeleft = wait_event_interruptible_timeout(arEvent,
            (ar->arWmiReady == TRUE), wmitimeout * HZ);

        if(!timeleft || signal_pending(current))
        {
            AR_DEBUG_PRINTF("WMI is not ready or wait was interrupted\n");
#if defined(DWSIM) /* TBDXXX */
            AR_DEBUG_PRINTF(".....but proceed anyway.\n");
#else
            return -EIO;
#endif
        }

        AR_DEBUG_PRINTF("%s() WMI is ready\n", __func__);

        /* Communicate the wmi protocol verision to the target */
        if ((ar6000_set_host_app_area(ar)) != A_OK) {
            AR_DEBUG_PRINTF("Unable to set the host app area\n");
        }
    }

    ar->arNumDataEndPts = 1;

    return(0);
}


void
ar6000_bitrate_rx(void *devt, A_INT32 rateKbps)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;

    ar->arBitRate = rateKbps;
    wake_up(&arEvent);
}

void
ar6000_ratemask_rx(void *devt, A_UINT16 ratemask)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;

    ar->arRateMask = ratemask;
    wake_up(&arEvent);
}

void
ar6000_txPwr_rx(void *devt, A_UINT8 txPwr)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;

    ar->arTxPwr = txPwr;
    wake_up(&arEvent);
}


void
ar6000_channelList_rx(void *devt, A_INT8 numChan, A_UINT16 *chanList)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;

    A_MEMCPY(ar->arChannelList, chanList, numChan * sizeof (A_UINT16));
    ar->arNumChannels = numChan;

    wake_up(&arEvent);
}

A_UINT8
ar6000_ibss_map_epid(struct sk_buff *skb, struct net_device *dev, A_UINT32 * mapNo)
{
    AR_SOFTC_T      *ar = (AR_SOFTC_T *)netdev_priv(dev);
    A_UINT8         *datap;
    ATH_MAC_HDR     *macHdr;
    A_UINT32         i, eptMap;

    (*mapNo) = 0;
    datap = A_NETBUF_DATA(skb);
    macHdr = (ATH_MAC_HDR *)(datap + sizeof(WMI_DATA_HDR));
    if (IEEE80211_IS_MULTICAST(macHdr->dstMac)) {
        return ENDPOINT_2;
    }

    eptMap = -1;
    for (i = 0; i < ar->arNodeNum; i ++) {
        if (IEEE80211_ADDR_EQ(macHdr->dstMac, ar->arNodeMap[i].macAddress)) {
            (*mapNo) = i + 1;
            ar->arNodeMap[i].txPending ++;
            return ar->arNodeMap[i].epId;
        }

        if ((eptMap == -1) && !ar->arNodeMap[i].txPending) {
            eptMap = i;
        }
    }

    if (eptMap == -1) {
        eptMap = ar->arNodeNum;
        ar->arNodeNum ++;
        A_ASSERT(ar->arNodeNum <= MAX_NODE_NUM);
    }

    A_MEMCPY(ar->arNodeMap[eptMap].macAddress, macHdr->dstMac, IEEE80211_ADDR_LEN);

    for (i = ENDPOINT_2; i <= ENDPOINT_5; i ++) {
        if (!ar->arTxPending[i]) {
            ar->arNodeMap[eptMap].epId = i;
            break;
        }
        // No free endpoint is available, start redistribution on the inuse endpoints.
        if (i == ENDPOINT_5) {
            ar->arNodeMap[eptMap].epId = ar->arNexEpId;
            ar->arNexEpId ++;
            if (ar->arNexEpId > ENDPOINT_5) {
                ar->arNexEpId = ENDPOINT_2;
            }
        }
    }

    (*mapNo) = eptMap + 1;
    ar->arNodeMap[eptMap].txPending ++;

    return ar->arNodeMap[eptMap].epId;
}

#ifdef DEBUG
static void ar6000_dump_skb(struct sk_buff *skb)
{
   u_char *ch;
   for (ch = A_NETBUF_DATA(skb);
        (A_UINT32)ch < ((A_UINT32)A_NETBUF_DATA(skb) +
        A_NETBUF_LEN(skb)); ch++)
    {
         AR_DEBUG_PRINTF("%2.2x ", *ch);
    }
    AR_DEBUG_PRINTF("\n");
}
#endif

static int
ar6000_data_tx(struct sk_buff *skb, struct net_device *dev)
{
    AR_SOFTC_T        *ar = (AR_SOFTC_T *)netdev_priv(dev);
    WMI_PRI_STREAM_ID streamID = WMI_NOT_MAPPED;
    A_UINT32          mapNo = 0;
    int               len;
    struct ar_cookie *cookie;
    A_BOOL            checkAdHocPsMapping = FALSE;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,13)
    skb->list = NULL;
#endif

    AR_DEBUG2_PRINTF("ar6000_data_tx start - skb=0x%x, data=0x%x, len=0x%x\n",
                     (A_UINT32)skb, (A_UINT32)A_NETBUF_DATA(skb),
                     A_NETBUF_LEN(skb));
#ifdef CONFIG_HOST_TCMD_SUPPORT
     /* TCMD doesnt support any data, free the buf and return */
    if(ar->arTargetMode == AR6000_TCMD_MODE) {
        A_NETBUF_FREE(skb);
        return 0;
    }
#endif
    do {

        if (ar->arWmiReady == FALSE && bypasswmi == 0) {
            break;
        }

#ifdef BLOCK_TX_PATH_FLAG
        if (blocktx) {
            break;
        }
#endif /* BLOCK_TX_PATH_FLAG */

        if (ar->arWmiEnabled) {
            if (A_NETBUF_HEADROOM(skb) < dev->hard_header_len) {
                struct sk_buff  *newbuf;
                /*
                 * We really should have gotten enough headroom but sometimes
                 * we still get packets with not enough headroom.  Copy the packet.
                 */
                len = A_NETBUF_LEN(skb);
                newbuf = A_NETBUF_ALLOC(len);
                if (newbuf == NULL) {
                    break;
                }
                A_NETBUF_PUT(newbuf, len);
                A_MEMCPY(A_NETBUF_DATA(newbuf), A_NETBUF_DATA(skb), len);
                A_NETBUF_FREE(skb);
                skb = newbuf;
                /* fall through and assemble header */
            }

            if (wmi_dix_2_dot3(ar->arWmi, skb) != A_OK) {
                AR_DEBUG_PRINTF("ar6000_data_tx - wmi_dix_2_dot3 failed\n");
                break;
            }

            if (wmi_data_hdr_add(ar->arWmi, skb, DATA_MSGTYPE) != A_OK) {
                AR_DEBUG_PRINTF("ar6000_data_tx - wmi_data_hdr_add failed\n");
                break;
            }

            if ((ar->arNetworkType == ADHOC_NETWORK) &&
                ar->arIbssPsEnable && ar->arConnected) {
                    /* flag to check adhoc mapping once we take the lock below: */
                checkAdHocPsMapping = TRUE;

            } else {
                    /* get the stream mapping */
                if (ar->arWmmEnabled) {
                    streamID = wmi_get_stream_id(ar->arWmi,
                                    wmi_implicit_create_pstream(ar->arWmi, skb, UPLINK_TRAFFIC, UNDEFINED_PRI));
                } else {
                    streamID = WMI_BEST_EFFORT_PRI;
                }
            }

        } else {
            struct iphdr    *ipHdr;
            /*
             * the endpoint is directly based on the TOS field in the IP
             * header **** only for testing ******
             */
            ipHdr = A_NETBUF_DATA(skb) + sizeof(ATH_MAC_HDR);
                /* here we map the TOS field to an endpoint number, this is for
                 * the endpointping test application */
            streamID = IP_TOS_TO_WMI_PRI(ipHdr->tos);
        }

    } while (FALSE);

        /* did we succeed ? */
    if ((streamID == WMI_NOT_MAPPED) && !checkAdHocPsMapping) {
            /* cleanup and exit */
        A_NETBUF_FREE(skb);
        AR6000_STAT_INC(ar, tx_dropped);
        AR6000_STAT_INC(ar, tx_aborted_errors);
        return 0;
    }

    cookie = NULL;

        /* take the lock to protect driver data */
    AR6000_SPIN_LOCK(&ar->arLock, 0);

    do {

        if (checkAdHocPsMapping) {
            streamID = ar6000_ibss_map_epid(skb, dev, &mapNo);
        }

        A_ASSERT(streamID != WMI_NOT_MAPPED);

            /* validate that the endpoint is connected */
        if (arWMIStream2EndpointID(ar,streamID) == 0) {
            AR_DEBUG_PRINTF("Stream %d is NOT mapped!\n",streamID);
            break;
        }
            /* allocate resource for this packet */
        cookie = ar6000_alloc_cookie(ar);

        if (cookie != NULL) {
                /* update counts while the lock is held */
            ar->arTxPending[streamID]++;
            ar->arTotalTxDataPending++;
        }

    } while (FALSE);

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    if (cookie != NULL) {
        cookie->arc_bp[0] = (A_UINT32)skb;
        cookie->arc_bp[1] = mapNo;
        SET_HTC_PACKET_INFO_TX(&cookie->HtcPkt,
                               cookie,
                               A_NETBUF_DATA(skb),
                               A_NETBUF_LEN(skb),
                               arWMIStream2EndpointID(ar,streamID),
                               AR6K_DATA_PKT_TAG);

#ifdef DEBUG
        if (debugdriver >= 3) {
            ar6000_dump_skb(skb);
        }
#endif
            /* HTC interface is asynchronous, if this fails, cleanup will happen in
             * the ar6000_tx_complete callback */
        HTCSendPkt(ar->arHtcTarget, &cookie->HtcPkt);
    } else {
            /* no packet to send, cleanup */
        A_NETBUF_FREE(skb);
        AR6000_STAT_INC(ar, tx_dropped);
        AR6000_STAT_INC(ar, tx_aborted_errors);
    }

    return 0;
}

#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
static void
tvsub(register struct timeval *out, register struct timeval *in)
{
    if((out->tv_usec -= in->tv_usec) < 0) {
        out->tv_sec--;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

void
applyAPTCHeuristics(AR_SOFTC_T *ar)
{
    A_UINT32 duration;
    A_UINT32 numbytes;
    A_UINT32 throughput;
    struct timeval ts;
    A_STATUS status;

    AR6000_SPIN_LOCK(&ar->arLock, 0);

    if ((enableAPTCHeuristics) && (!aptcTR.timerScheduled)) {
        do_gettimeofday(&ts);
        tvsub(&ts, &aptcTR.samplingTS);
        duration = ts.tv_sec * 1000 + ts.tv_usec / 1000; /* ms */
        numbytes = aptcTR.bytesTransmitted + aptcTR.bytesReceived;

        if (duration > APTC_TRAFFIC_SAMPLING_INTERVAL) {
            /* Initialize the time stamp and byte count */
            aptcTR.bytesTransmitted = aptcTR.bytesReceived = 0;
            do_gettimeofday(&aptcTR.samplingTS);

            /* Calculate and decide based on throughput thresholds */
            throughput = ((numbytes * 8) / duration);
            if (throughput > APTC_UPPER_THROUGHPUT_THRESHOLD) {
                /* Disable Sleep and schedule a timer */
                A_ASSERT(ar->arWmiReady == TRUE);
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                status = wmi_powermode_cmd(ar->arWmi, MAX_PERF_POWER);
                AR6000_SPIN_LOCK(&ar->arLock, 0);
                A_TIMEOUT_MS(&aptcTimer, APTC_TRAFFIC_SAMPLING_INTERVAL, 0);
                aptcTR.timerScheduled = TRUE;
            }
        }
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
}
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

static void
ar6000_tx_queue_full(void *Context, HTC_ENDPOINT_ID Endpoint)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *) Context;

    if (Endpoint == arWMIStream2EndpointID(ar,WMI_CONTROL_PRI)) {
        if (!bypasswmi) {
                /* under normal WMI if this is getting full, then something is running rampant
                 * the host should not be exhausting the WMI queue with too many commands
                 * the only exception to this is during testing using endpointping */

            AR6000_SPIN_LOCK(&ar->arLock, 0);
                /* set flag to handle subsequent messages */
            ar->arWMIControlEpFull = TRUE;
            AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            AR_DEBUG_PRINTF("WMI Control Endpoint is FULL!!! \n");
        }
    } else {
        /* one of the data endpoints queues is getting full..need to stop network stack
         * the queue will resume after credits received */
        netif_stop_queue(ar->arNetDev);
    }
}

static void
ar6000_tx_queue_avail(void *Context, HTC_ENDPOINT_ID Endpoint)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)Context;

    if (Endpoint == arWMIStream2EndpointID(ar,WMI_CONTROL_PRI)) {
        /* FIXME: what do for it?  */
    } else {
        /* Wake up interface, rescheduling prevented.  */
        if (ar->arConnected == TRUE || bypasswmi)
            netif_wake_queue(ar->arNetDev);
    }
}

static void
ar6000_tx_complete(void *Context, HTC_PACKET *pPacket)
{
    AR_SOFTC_T     *ar = (AR_SOFTC_T *)Context;
    void           *cookie = (void *)pPacket->pPktContext;
    struct sk_buff *skb = NULL;
    A_UINT32        mapNo = 0;
    A_STATUS        status;
    struct ar_cookie * ar_cookie;
    WMI_PRI_STREAM_ID streamID;
    A_BOOL          wakeEvent = FALSE;

    status = pPacket->Status;
    ar_cookie = (struct ar_cookie *)cookie;
    skb = (struct sk_buff *)ar_cookie->arc_bp[0];
    streamID = arEndpoint2WMIStreamID(ar,pPacket->Endpoint);
    mapNo = ar_cookie->arc_bp[1];

    A_ASSERT(skb);
    A_ASSERT(pPacket->pBuffer == A_NETBUF_DATA(skb));

    if (A_SUCCESS(status)) {
        A_ASSERT(pPacket->ActualLength == A_NETBUF_LEN(skb));
    }

    AR_DEBUG2_PRINTF("ar6000_tx_complete skb=0x%x data=0x%x len=0x%x sid=%d ",
                     (A_UINT32)skb, (A_UINT32)pPacket->pBuffer,
                     pPacket->ActualLength,
                     streamID);

        /* lock the driver as we update internal state */
    AR6000_SPIN_LOCK(&ar->arLock, 0);

    ar->arTxPending[streamID]--;

    if ((streamID != WMI_CONTROL_PRI) || bypasswmi) {
        ar->arTotalTxDataPending--;
    }

    if (streamID == WMI_CONTROL_PRI)
    {
        if (ar->arWMIControlEpFull) {
                /* since this packet completed, the WMI EP is no longer full */
            ar->arWMIControlEpFull = FALSE;
        }

        if (ar->arTxPending[streamID] == 0) {
            wakeEvent = TRUE;
        }
    }

    if (A_FAILED(status)) {
        AR_DEBUG_PRINTF("%s() -TX ERROR, status: 0x%x\n", __func__,
                        status);
        AR6000_STAT_INC(ar, tx_errors);
    } else {
        AR_DEBUG2_PRINTF("OK\n");
        AR6000_STAT_INC(ar, tx_packets);
        ar->arNetStats.tx_bytes += A_NETBUF_LEN(skb);
#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
        aptcTR.bytesTransmitted += a_netbuf_to_len(skb);
        applyAPTCHeuristics(ar);
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */
    }

    // TODO this needs to be looked at
    if ((ar->arNetworkType == ADHOC_NETWORK) && ar->arIbssPsEnable
        && (streamID != WMI_CONTROL_PRI) && mapNo)
    {
        mapNo --;
        ar->arNodeMap[mapNo].txPending --;

        if (!ar->arNodeMap[mapNo].txPending && (mapNo == (ar->arNodeNum - 1))) {
            A_UINT32 i;
            for (i = ar->arNodeNum; i > 0; i --) {
                if (!ar->arNodeMap[i - 1].txPending) {
                    A_MEMZERO(&ar->arNodeMap[i - 1], sizeof(struct ar_node_mapping));
                    ar->arNodeNum --;
                } else {
                    break;
                }
            }
        }
    }

    /* Freeing a cookie should not be contingent on either of */
    /* these flags, just if we have a cookie or not.           */
    /* Can we even get here without a cookie? Fix later.       */
    if (ar->arWmiReady == TRUE || (bypasswmi))
    {
        ar6000_free_cookie(ar, cookie);
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    /* lock is released, we can freely call other kernel APIs */

        /* this indirectly frees the HTC_PACKET */
    A_NETBUF_FREE(skb);

    if (wakeEvent) {
        wake_up(&arEvent);
    }
}

/*
 * Receive event handler.  This is called by HTC when a packet is received
 */
int pktcount;
static void
ar6000_rx(void *Context, HTC_PACKET *pPacket)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)Context;
    struct sk_buff *skb = (struct sk_buff *)pPacket->pPktContext;
    int minHdrLen;
    A_STATUS        status = pPacket->Status;
    WMI_PRI_STREAM_ID streamID = arEndpoint2WMIStreamID(ar,pPacket->Endpoint);
    HTC_ENDPOINT_ID   ept = pPacket->Endpoint;

    A_ASSERT((status != A_OK) || (pPacket->pBuffer == (A_NETBUF_DATA(skb) + HTC_HEADER_LEN)));

    AR_DEBUG2_PRINTF("ar6000_rx ar=0x%x sid=%d, skb=0x%x, data=0x%x, len=0x%x ",
                    (A_UINT32)ar, streamID, (A_UINT32)skb, (A_UINT32)pPacket->pBuffer,
                    pPacket->ActualLength);
    if (status != A_OK) {
        AR_DEBUG2_PRINTF("ERR\n");
    } else {
        AR_DEBUG2_PRINTF("OK\n");
    }

        /* take lock to protect buffer counts
         * and adaptive power throughput state */
    AR6000_SPIN_LOCK(&ar->arLock, 0);

    ar->arRxBuffers[streamID]--;

    if (A_SUCCESS(status)) {
        AR6000_STAT_INC(ar, rx_packets);
        ar->arNetStats.rx_bytes += pPacket->ActualLength;
#ifdef ADAPTIVE_POWER_THROUGHPUT_CONTROL
        aptcTR.bytesReceived += a_netbuf_to_len(skb);
        applyAPTCHeuristics(ar);
#endif /* ADAPTIVE_POWER_THROUGHPUT_CONTROL */

        A_NETBUF_PUT(skb, pPacket->ActualLength +  HTC_HEADER_LEN);
        A_NETBUF_PULL(skb, HTC_HEADER_LEN);

#ifdef DEBUG
        if (debugdriver >= 2) {
            ar6000_dump_skb(skb);
        }
#endif /* DEBUG */
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    if (status != A_OK) {
        AR6000_STAT_INC(ar, rx_errors);
        A_NETBUF_FREE(skb);
    } else if (ar->arWmiEnabled == TRUE) {
        if (streamID == WMI_CONTROL_PRI) {
           /*
            * this is a wmi control msg
            */
            wmi_control_rx(ar->arWmi, skb);
        } else {
            WMI_DATA_HDR *dhdr = (WMI_DATA_HDR *)A_NETBUF_DATA(skb);
            if (WMI_DATA_HDR_IS_MSG_TYPE(dhdr, CNTL_MSGTYPE)) {
                /*
                 * this is a wmi control msg
                 */
                /* strip off WMI hdr */
                wmi_data_hdr_remove(ar->arWmi, skb);
                wmi_control_rx(ar->arWmi, skb);
            } else {
                /*
                 * this is a wmi data packet
                 */
                minHdrLen = sizeof (WMI_DATA_HDR) + sizeof(ATH_MAC_HDR) +
                            sizeof(ATH_LLC_SNAP_HDR);

                if ((pPacket->ActualLength < minHdrLen) ||
                    (pPacket->ActualLength > AR6000_BUFFER_SIZE))
                {
                    /*
                     * packet is too short or too long
                     */
                    AR_DEBUG_PRINTF("TOO SHORT or TOO LONG\n");
                    AR6000_STAT_INC(ar, rx_errors);
                    AR6000_STAT_INC(ar, rx_length_errors);
                    A_NETBUF_FREE(skb);
                } else {
                    if (ar->arWmmEnabled) {
                        wmi_implicit_create_pstream(ar->arWmi, skb,
                            DNLINK_TRAFFIC, UNDEFINED_PRI);
                    }
#if 0
                    /* Access RSSI values here */
                    AR_DEBUG_PRINTF("RSSI %d\n",
                        ((WMI_DATA_HDR *) A_NETBUF_DATA(skb))->rssi);
#endif
                    wmi_data_hdr_remove(ar->arWmi, skb);
                    wmi_dot3_2_dix(ar->arWmi, skb);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
                    /*
                     * extra push and memcpy, for eth_type_trans() of 2.4 kernel
                     * will pull out hard_header_len bytes of the skb.
                     */
                    A_NETBUF_PUSH(skb, sizeof(WMI_DATA_HDR) + sizeof(ATH_LLC_SNAP_HDR) + HTC_HEADER_LEN);
                    A_MEMCPY(A_NETBUF_DATA(skb), A_NETBUF_DATA(skb) + sizeof(WMI_DATA_HDR) +
                             sizeof(ATH_LLC_SNAP_HDR) + HTC_HEADER_LEN, sizeof(ATH_MAC_HDR));
#endif
                    if ((ar->arNetDev->flags & IFF_UP) == IFF_UP)
                    {
                        skb->dev = ar->arNetDev;
                        skb->protocol = eth_type_trans(skb, ar->arNetDev);
                        netif_rx(skb);
                    }
                    else
                    {
                        A_NETBUF_FREE(skb);
                    }
                }
            }
        }
    } else {
        if ((ar->arNetDev->flags & IFF_UP) == IFF_UP)
        {
            skb->dev = ar->arNetDev;
            skb->protocol = eth_type_trans(skb, ar->arNetDev);
            netif_rx(skb);
        }
        else
        {
            A_NETBUF_FREE(skb);
        }
    }

    if (status != A_ECANCELED) {
        /*
         * HTC provides A_ECANCELED status when it doesn't want to be refilled
         * (probably due to a shutdown)
         */
        ar6000_rx_refill(Context, ept);
    }


}

static void
ar6000_rx_refill(void *Context, HTC_ENDPOINT_ID Endpoint)
{
    AR_SOFTC_T  *ar = (AR_SOFTC_T *)Context;
    void        *osBuf;
    int         RxBuffers;
    int         buffersToRefill;
    HTC_PACKET  *pPacket;
    WMI_PRI_STREAM_ID streamId = arEndpoint2WMIStreamID(ar,Endpoint);

    buffersToRefill = (int)AR6000_MAX_RX_BUFFERS -
                                    (int)ar->arRxBuffers[streamId];

    if (buffersToRefill <= 0) {
            /* fast return, nothing to fill */
        return;
    }

    AR_DEBUG2_PRINTF("ar6000_rx_refill: providing htc with %d buffers at eid=%d\n",
                    buffersToRefill, Endpoint);

    for (RxBuffers = 0; RxBuffers < buffersToRefill; RxBuffers++) {
        osBuf = A_NETBUF_ALLOC(AR6000_BUFFER_SIZE);
        if (NULL == osBuf) {
            break;
        }
            /* the HTC packet wrapper is at the head of the reserved area
             * in the skb */
        pPacket = (HTC_PACKET *)(A_NETBUF_HEAD(osBuf));
            /* set re-fill info */
        SET_HTC_PACKET_INFO_RX_REFILL(pPacket,osBuf,A_NETBUF_DATA(osBuf),AR6000_BUFFER_SIZE,Endpoint);
            /* add this packet */
        HTCAddReceivePkt(ar->arHtcTarget, pPacket);
    }

        /* update count */
    AR6000_SPIN_LOCK(&ar->arLock, 0);
    ar->arRxBuffers[streamId] += RxBuffers;
    AR6000_SPIN_UNLOCK(&ar->arLock, 0);
}

static struct net_device_stats *
ar6000_get_stats(struct net_device *dev)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    return &ar->arNetStats;
}

static struct iw_statistics *
ar6000_get_iwstats(struct net_device * dev)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)netdev_priv(dev);
    TARGET_STATS *pStats = &ar->arTargetStats;
    struct iw_statistics * pIwStats = &ar->arIwStats;

    if ((ar->arWmiReady == FALSE)
    /*
     * The in_atomic function is used to determine if the scheduling is
     * allowed in the current context or not. This was introduced in 2.6
     * From what I have read on the differences between 2.4 and 2.6, the
     * 2.4 kernel did not support preemption and so this check might not
     * be required for 2.4 kernels.
     */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
        || (in_atomic())
#endif
       )
    {
        pIwStats->status = 0;
        pIwStats->qual.qual = 0;
        pIwStats->qual.level =0;
        pIwStats->qual.noise = 0;
        pIwStats->discard.code =0;
        pIwStats->discard.retries=0;
        pIwStats->miss.beacon =0;
        return pIwStats;
    }
    if (down_interruptible(&ar->arSem)) {
        pIwStats->status = 0;
        return pIwStats;
    }


    ar->statsUpdatePending = TRUE;

    if(wmi_get_stats_cmd(ar->arWmi) != A_OK) {
        up(&ar->arSem);
        pIwStats->status = 0;
        return pIwStats;
    }

    wait_event_interruptible_timeout(arEvent, ar->statsUpdatePending == FALSE, wmitimeout * HZ);

    if (signal_pending(current)) {
        AR_DEBUG_PRINTF("ar6000 : WMI get stats timeout \n");
        up(&ar->arSem);
        pIwStats->status = 0;
        return pIwStats;
    }
    pIwStats->status = 1 ;
    pIwStats->qual.qual = pStats->cs_aveBeacon_rssi;
    pIwStats->qual.level =pStats->cs_aveBeacon_rssi + 161;  /* noise is -95 dBm */
    pIwStats->qual.noise = pStats->noise_floor_calibation;
    pIwStats->discard.code = pStats->rx_decrypt_err;
    pIwStats->discard.retries = pStats->tx_retry_cnt;
    pIwStats->miss.beacon = pStats->cs_bmiss_cnt;
    up(&ar->arSem);
    return pIwStats;
}

void
ar6000_ready_event(void *devt, A_UINT8 *datap, A_UINT8 phyCap)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;
    struct net_device *dev = ar->arNetDev;

    ar->arWmiReady = TRUE;
    wake_up(&arEvent);
    A_MEMCPY(dev->dev_addr, datap, AR6000_ETH_ADDR_LEN);
    AR_DEBUG_PRINTF("mac address = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
        dev->dev_addr[0], dev->dev_addr[1],
        dev->dev_addr[2], dev->dev_addr[3],
        dev->dev_addr[4], dev->dev_addr[5]);

    ar->arPhyCapability = phyCap;
}

A_UINT8
ar6000_iptos_to_userPriority(A_UINT8 *pkt)
{
    struct iphdr *ipHdr = (struct iphdr *)pkt;
    A_UINT8 userPriority;

    /*
     * IP Tos format :
     *      (Refer Pg 57 WMM-test-plan-v1.2)
     * IP-TOS - 8bits
     *          : DSCP(6-bits) ECN(2-bits)
     *          : DSCP - P2 P1 P0 X X X
     *              where (P2 P1 P0) form 802.1D
     */
    userPriority = ipHdr->tos >> 5;
    return (userPriority & 0x7);
}

void
ar6000_connect_event(AR_SOFTC_T *ar, A_UINT16 channel, A_UINT8 *bssid,
                     A_UINT16 listenInterval, A_UINT16 beaconInterval,
                     NETWORK_TYPE networkType, A_UINT8 beaconIeLen,
                     A_UINT8 assocReqLen, A_UINT8 assocRespLen,
                     A_UINT8 *assocInfo)
{
    union iwreq_data wrqu;
    int i, beacon_ie_pos, assoc_resp_ie_pos, assoc_req_ie_pos;
    static const char *tag1 = "ASSOCINFO(ReqIEs=";
    static const char *tag2 = "ASSOCRESPIE=";
    static const char *beaconIetag = "BEACONIE=";
    char buf[WMI_CONTROL_MSG_MAX_LEN * 2 + sizeof(tag1)];
    char *pos;
    A_UINT8 key_op_ctrl;

    A_MEMCPY(ar->arBssid, bssid, sizeof(ar->arBssid));
    ar->arBssChannel = channel;

    A_PRINTF("AR6000 connected event on freq %d ", channel);
    A_PRINTF("with bssid %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x "
            " listenInterval=%d, beaconInterval = %d, beaconIeLen = %d assocReqLen=%d"
            " assocRespLen =%d\n",
             bssid[0], bssid[1], bssid[2],
             bssid[3], bssid[4], bssid[5],
             listenInterval, beaconInterval,
             beaconIeLen, assocReqLen, assocRespLen);
    if (networkType & ADHOC_NETWORK) {
        if (networkType & ADHOC_CREATOR) {
            A_PRINTF("Network: Adhoc (Creator)\n");
        } else {
            A_PRINTF("Network: Adhoc (Joiner)\n");
        }
    } else {
        A_PRINTF("Network: Infrastructure\n");
    }

    if (beaconIeLen && (sizeof(buf) > (9 + beaconIeLen * 2))) {
        AR_DEBUG_PRINTF("\nBeaconIEs= ");

        beacon_ie_pos = 0;
        A_MEMZERO(buf, sizeof(buf));
        sprintf(buf, "%s", beaconIetag);
        pos = buf + 9;
        for (i = beacon_ie_pos; i < beacon_ie_pos + beaconIeLen; i++) {
            AR_DEBUG_PRINTF("%2.2x ", assocInfo[i]);
            sprintf(pos, "%2.2x", assocInfo[i]);
            pos += 2;
        }
        AR_DEBUG_PRINTF("\n");

        A_MEMZERO(&wrqu, sizeof(wrqu));
        wrqu.data.length = strlen(buf);
        wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
    }

    if (assocRespLen && (sizeof(buf) > (12 + (assocRespLen * 2))))
    {
        assoc_resp_ie_pos = beaconIeLen + assocReqLen +
                            sizeof(A_UINT16)  +  /* capinfo*/
                            sizeof(A_UINT16)  +  /* status Code */
                            sizeof(A_UINT16)  ;  /* associd */
        A_MEMZERO(buf, sizeof(buf));
        sprintf(buf, "%s", tag2);
        pos = buf + 12;
        AR_DEBUG_PRINTF("\nAssocRespIEs= ");
        /*
         * The Association Response Frame w.o. the WLAN header is delivered to
         * the host, so skip over to the IEs
         */
        for (i = assoc_resp_ie_pos; i < assoc_resp_ie_pos + assocRespLen - 6; i++)
        {
            AR_DEBUG_PRINTF("%2.2x ", assocInfo[i]);
            sprintf(pos, "%2.2x", assocInfo[i]);
            pos += 2;
        }
        AR_DEBUG_PRINTF("\n");

        A_MEMZERO(&wrqu, sizeof(wrqu));
        wrqu.data.length = strlen(buf);
        wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
    }

    if (assocReqLen && (sizeof(buf) > (17 + (assocReqLen * 2)))) {
        /*
         * assoc Request includes capability and listen interval. Skip these.
         */
        assoc_req_ie_pos =  beaconIeLen +
                            sizeof(A_UINT16)  +  /* capinfo*/
                            sizeof(A_UINT16);    /* listen interval */

        A_MEMZERO(buf, sizeof(buf));
        sprintf(buf, "%s", tag1);
        pos = buf + 17;
        AR_DEBUG_PRINTF("AssocReqIEs= ");
        for (i = assoc_req_ie_pos; i < assoc_req_ie_pos + assocReqLen - 4; i++) {
            AR_DEBUG_PRINTF("%2.2x ", assocInfo[i]);
            sprintf(pos, "%2.2x", assocInfo[i]);
            pos += 2;;
        }
        AR_DEBUG_PRINTF("\n");

        A_MEMZERO(&wrqu, sizeof(wrqu));
        wrqu.data.length = strlen(buf);
        wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
    }

#ifdef USER_KEYS
    if (ar->user_savedkeys_stat == USER_SAVEDKEYS_STAT_RUN &&
        ar->user_saved_keys.keyOk == TRUE)
    {

        key_op_ctrl = KEY_OP_VALID_MASK & ~KEY_OP_INIT_TSC;
        if (ar->user_key_ctrl & AR6000_USER_SETKEYS_RSC_UNCHANGED) {
            key_op_ctrl &= ~KEY_OP_INIT_RSC;
        } else {
            key_op_ctrl |= KEY_OP_INIT_RSC;
        }
        ar6000_reinstall_keys(ar, key_op_ctrl);
    }
#endif /* USER_KEYS */

        /* flush data queues */
    ar6000_TxDataCleanup(ar);

    netif_start_queue(ar->arNetDev);

    if ((OPEN_AUTH == ar->arDot11AuthMode) &&
        (NONE_AUTH == ar->arAuthMode)      &&
        (WEP_CRYPT == ar->arPairwiseCrypto))
    {
        if (!ar->arConnected) {
            ar6000_install_static_wep_keys(ar);
        }
    }

    ar->arConnected  = TRUE;
    ar->arConnectPending = FALSE;

    reconnect_flag = 0;

    A_MEMZERO(&wrqu, sizeof(wrqu));
    A_MEMCPY(wrqu.addr.sa_data, bssid, IEEE80211_ADDR_LEN);
    wrqu.addr.sa_family = ARPHRD_ETHER;
    wireless_send_event(ar->arNetDev, SIOCGIWAP, &wrqu, NULL);
    if ((ar->arNetworkType == ADHOC_NETWORK) && ar->arIbssPsEnable) {
        A_MEMZERO(ar->arNodeMap, sizeof(ar->arNodeMap));
        ar->arNodeNum = 0;
        ar->arNexEpId = ENDPOINT_2;
    }

}

void ar6000_set_numdataendpts(AR_SOFTC_T *ar, A_UINT32 num)
{
    A_ASSERT(num <= (HTC_MAILBOX_NUM_MAX - 1));
    ar->arNumDataEndPts = num;
}

void
ar6000_disconnect_event(AR_SOFTC_T *ar, A_UINT8 reason, A_UINT8 *bssid,
                        A_UINT8 assocRespLen, A_UINT8 *assocInfo, A_UINT16 protocolReasonStatus)
{
    A_UINT8 i;

    A_PRINTF("AR6000 disconnected");
    if (bssid[0] || bssid[1] || bssid[2] || bssid[3] || bssid[4] || bssid[5]) {
        A_PRINTF(" from %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ",
                 bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    }
    A_PRINTF("\n");

    AR_DEBUG_PRINTF("\nDisconnect Reason is %d", reason);
    AR_DEBUG_PRINTF("\nProtocol Reason/Status Code is %d", protocolReasonStatus);
    AR_DEBUG_PRINTF("\nAssocResp Frame = %s",
                    assocRespLen ? " " : "NULL");
    for (i = 0; i < assocRespLen; i++) {
        if (!(i % 0x10)) {
            AR_DEBUG_PRINTF("\n");
        }
        AR_DEBUG_PRINTF("%2.2x ", assocInfo[i]);
    }
    AR_DEBUG_PRINTF("\n");
    /*
     * If the event is due to disconnect cmd from the host, only they the target
     * would stop trying to connect. Under any other condition, target would
     * keep trying to connect.
     *
     */
    if( reason == DISCONNECT_CMD)
    {
        ar->arConnectPending = FALSE;
    } else {
        ar->arConnectPending = TRUE;
        if (((reason == ASSOC_FAILED) && (protocolReasonStatus == 0x11)) ||
            ((reason == ASSOC_FAILED) && (protocolReasonStatus == 0x0) && (reconnect_flag == 1))) {
            ar->arConnected = TRUE;
            return;
        }
    }
    ar->arConnected = FALSE;

    if( (reason != CSERV_DISCONNECT) || (reconnect_flag != 1) ) {
        reconnect_flag = 0;
    }

#ifdef USER_KEYS
    if (reason != CSERV_DISCONNECT)
    {
        ar->user_savedkeys_stat = USER_SAVEDKEYS_STAT_INIT;
        ar->user_key_ctrl      = 0;
    }
#endif /* USER_KEYS */

    netif_stop_queue(ar->arNetDev);
    A_MEMZERO(ar->arBssid, sizeof(ar->arBssid));
    ar->arBssChannel = 0;
    ar->arBeaconInterval = 0;

    ar6000_TxDataCleanup(ar);
}

void
ar6000_regDomain_event(AR_SOFTC_T *ar, A_UINT32 regCode)
{
    A_PRINTF("AR6000 Reg Code = 0x%x\n", regCode);
    ar->arRegCode = regCode;
}

void
ar6000_neighborReport_event(AR_SOFTC_T *ar, int numAps, WMI_NEIGHBOR_INFO *info)
{
    static const char *tag = "PRE-AUTH";
    char buf[128];
    union iwreq_data wrqu;
    int i;

    AR_DEBUG_PRINTF("AR6000 Neighbor Report Event\n");
    for (i=0; i < numAps; info++, i++) {
        AR_DEBUG_PRINTF("bssid %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ",
            info->bssid[0], info->bssid[1], info->bssid[2],
            info->bssid[3], info->bssid[4], info->bssid[5]);
        if (info->bssFlags & WMI_PREAUTH_CAPABLE_BSS) {
            AR_DEBUG_PRINTF("preauth-cap");
        }
        if (info->bssFlags & WMI_PMKID_VALID_BSS) {
            AR_DEBUG_PRINTF(" pmkid-valid\n");
            continue;           /* we skip bss if the pmkid is already valid */
        }
        AR_DEBUG_PRINTF("\n");
        snprintf(buf, sizeof(buf), "%s%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                 tag,
                 info->bssid[0], info->bssid[1], info->bssid[2],
                 info->bssid[3], info->bssid[4], info->bssid[5],
                 i, info->bssFlags);
        A_MEMZERO(&wrqu, sizeof(wrqu));
        wrqu.data.length = strlen(buf);
        wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
    }
}

void
ar6000_tkip_micerr_event(AR_SOFTC_T *ar, A_UINT8 keyid, A_BOOL ismcast)
{
    static const char *tag = "MLME-MICHAELMICFAILURE.indication";
    char buf[128];
    union iwreq_data wrqu;

    A_PRINTF("AR6000 TKIP MIC error received for keyid %d %scast\n",
             keyid, ismcast ? "multi": "uni");
    snprintf(buf, sizeof(buf), "%s(keyid=%d %scat)", tag, keyid,
             ismcast ? "multi" : "uni");
    memset(&wrqu, 0, sizeof(wrqu));
    wrqu.data.length = strlen(buf);
    wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
}

void
ar6000_scanComplete_event(AR_SOFTC_T *ar, A_STATUS status)
{
	AR_DEBUG_PRINTF("AR6000 scan complete: %d\n", status);

	ar->scan_complete = 1;
	wake_up_interruptible(&ar6000_scan_queue);
}

void
ar6000_targetStats_event(AR_SOFTC_T *ar,  WMI_TARGET_STATS *pTarget)
{
    TARGET_STATS *pStats = &ar->arTargetStats;
    A_UINT8 ac;

    /*A_PRINTF("AR6000 updating target stats\n");*/
    pStats->tx_packets          += pTarget->txrxStats.tx_stats.tx_packets;
    pStats->tx_bytes            += pTarget->txrxStats.tx_stats.tx_bytes;
    pStats->tx_unicast_pkts     += pTarget->txrxStats.tx_stats.tx_unicast_pkts;
    pStats->tx_unicast_bytes    += pTarget->txrxStats.tx_stats.tx_unicast_bytes;
    pStats->tx_multicast_pkts   += pTarget->txrxStats.tx_stats.tx_multicast_pkts;
    pStats->tx_multicast_bytes  += pTarget->txrxStats.tx_stats.tx_multicast_bytes;
    pStats->tx_broadcast_pkts   += pTarget->txrxStats.tx_stats.tx_broadcast_pkts;
    pStats->tx_broadcast_bytes  += pTarget->txrxStats.tx_stats.tx_broadcast_bytes;
    pStats->tx_rts_success_cnt  += pTarget->txrxStats.tx_stats.tx_rts_success_cnt;
    for(ac = 0; ac < WMM_NUM_AC; ac++)
        pStats->tx_packet_per_ac[ac] += pTarget->txrxStats.tx_stats.tx_packet_per_ac[ac];
    pStats->tx_errors           += pTarget->txrxStats.tx_stats.tx_errors;
    pStats->tx_failed_cnt       += pTarget->txrxStats.tx_stats.tx_failed_cnt;
    pStats->tx_retry_cnt        += pTarget->txrxStats.tx_stats.tx_retry_cnt;
    pStats->tx_rts_fail_cnt     += pTarget->txrxStats.tx_stats.tx_rts_fail_cnt;
    pStats->tx_unicast_rate      = wmi_get_rate(pTarget->txrxStats.tx_stats.tx_unicast_rate);

    pStats->rx_packets          += pTarget->txrxStats.rx_stats.rx_packets;
    pStats->rx_bytes            += pTarget->txrxStats.rx_stats.rx_bytes;
    pStats->rx_unicast_pkts     += pTarget->txrxStats.rx_stats.rx_unicast_pkts;
    pStats->rx_unicast_bytes    += pTarget->txrxStats.rx_stats.rx_unicast_bytes;
    pStats->rx_multicast_pkts   += pTarget->txrxStats.rx_stats.rx_multicast_pkts;
    pStats->rx_multicast_bytes  += pTarget->txrxStats.rx_stats.rx_multicast_bytes;
    pStats->rx_broadcast_pkts   += pTarget->txrxStats.rx_stats.rx_broadcast_pkts;
    pStats->rx_broadcast_bytes  += pTarget->txrxStats.rx_stats.rx_broadcast_bytes;
    pStats->rx_fragment_pkt     += pTarget->txrxStats.rx_stats.rx_fragment_pkt;
    pStats->rx_errors           += pTarget->txrxStats.rx_stats.rx_errors;
    pStats->rx_crcerr           += pTarget->txrxStats.rx_stats.rx_crcerr;
    pStats->rx_key_cache_miss   += pTarget->txrxStats.rx_stats.rx_key_cache_miss;
    pStats->rx_decrypt_err      += pTarget->txrxStats.rx_stats.rx_decrypt_err;
    pStats->rx_duplicate_frames += pTarget->txrxStats.rx_stats.rx_duplicate_frames;
    pStats->rx_unicast_rate      = wmi_get_rate(pTarget->txrxStats.rx_stats.rx_unicast_rate);


    pStats->tkip_local_mic_failure
                                += pTarget->txrxStats.tkipCcmpStats.tkip_local_mic_failure;
    pStats->tkip_counter_measures_invoked
                                += pTarget->txrxStats.tkipCcmpStats.tkip_counter_measures_invoked;
    pStats->tkip_replays        += pTarget->txrxStats.tkipCcmpStats.tkip_replays;
    pStats->tkip_format_errors  += pTarget->txrxStats.tkipCcmpStats.tkip_format_errors;
    pStats->ccmp_format_errors  += pTarget->txrxStats.tkipCcmpStats.ccmp_format_errors;
    pStats->ccmp_replays        += pTarget->txrxStats.tkipCcmpStats.ccmp_replays;


    pStats->power_save_failure_cnt += pTarget->pmStats.power_save_failure_cnt;
    pStats->noise_floor_calibation = pTarget->noise_floor_calibation;

    pStats->cs_bmiss_cnt        += pTarget->cservStats.cs_bmiss_cnt;
    pStats->cs_lowRssi_cnt      += pTarget->cservStats.cs_lowRssi_cnt;
    pStats->cs_connect_cnt      += pTarget->cservStats.cs_connect_cnt;
    pStats->cs_disconnect_cnt   += pTarget->cservStats.cs_disconnect_cnt;
    pStats->cs_aveBeacon_snr    = pTarget->cservStats.cs_aveBeacon_snr;
    pStats->cs_aveBeacon_rssi   = pTarget->cservStats.cs_aveBeacon_rssi;
    pStats->cs_lastRoam_msec    = pTarget->cservStats.cs_lastRoam_msec;
    pStats->cs_snr              = pTarget->cservStats.cs_snr;
    pStats->cs_rssi             = pTarget->cservStats.cs_rssi;

    pStats->lq_val              = pTarget->lqVal;

    pStats->wow_num_pkts_dropped += pTarget->wowStats.wow_num_pkts_dropped;
    pStats->wow_num_host_pkt_wakeups += pTarget->wowStats.wow_num_host_pkt_wakeups;
    pStats->wow_num_host_event_wakeups += pTarget->wowStats.wow_num_host_event_wakeups;
    pStats->wow_num_events_discarded += pTarget->wowStats.wow_num_events_discarded;

    ar->statsUpdatePending = FALSE;
    wake_up(&arEvent);
}

void
ar6000_rssiThreshold_event(AR_SOFTC_T *ar,  WMI_RSSI_THRESHOLD_VAL newThreshold, A_INT16 rssi)
{
    USER_RSSI_THOLD userRssiThold;

    userRssiThold.tag = rssi_map[newThreshold].tag;
    userRssiThold.rssi = rssi;
    AR_DEBUG2_PRINTF("rssi Threshold range = %d tag = %d  rssi = %d\n", newThreshold, userRssiThold.tag, rssi);
#ifdef SEND_EVENT_TO_APP
    ar6000_send_event_to_app(ar, WMI_RSSI_THRESHOLD_EVENTID,(A_UINT8 *)&userRssiThold, sizeof(USER_RSSI_THOLD));
#endif
}


void
ar6000_hbChallengeResp_event(AR_SOFTC_T *ar, A_UINT32 cookie, A_UINT32 source)
{
    if (source == APP_HB_CHALLENGE) {
        /* Report it to the app in case it wants a positive acknowledgement */
#ifdef SEND_EVENT_TO_APP
        ar6000_send_event_to_app(ar, WMIX_HB_CHALLENGE_RESP_EVENTID,
                                 (A_UINT8 *)&cookie, sizeof(cookie));
#endif
    } else {
        /* This would ignore the replys that come in after their due time */
        if (cookie == ar->arHBChallengeResp.seqNum) {
            ar->arHBChallengeResp.outstanding = FALSE;
        }
    }
}


void
ar6000_reportError_event(AR_SOFTC_T *ar, WMI_TARGET_ERROR_VAL errorVal)
{
    char    *errString[] = {
                [WMI_TARGET_PM_ERR_FAIL]    "WMI_TARGET_PM_ERR_FAIL",
                [WMI_TARGET_KEY_NOT_FOUND]  "WMI_TARGET_KEY_NOT_FOUND",
                [WMI_TARGET_DECRYPTION_ERR] "WMI_TARGET_DECRYPTION_ERR",
                [WMI_TARGET_BMISS]          "WMI_TARGET_BMISS",
                [WMI_PSDISABLE_NODE_JOIN]   "WMI_PSDISABLE_NODE_JOIN"
                };

    A_PRINTF("AR6000 Error on Target. Error = 0x%x\n", errorVal);

    /* One error is reported at a time, and errorval is a bitmask */
    if(errorVal & (errorVal - 1))
       return;

    A_PRINTF("AR6000 Error type = ");
    switch(errorVal)
    {
        case WMI_TARGET_PM_ERR_FAIL:
        case WMI_TARGET_KEY_NOT_FOUND:
        case WMI_TARGET_DECRYPTION_ERR:
        case WMI_TARGET_BMISS:
        case WMI_PSDISABLE_NODE_JOIN:
            A_PRINTF("%s\n", errString[errorVal]);
            break;
        default:
            A_PRINTF("INVALID\n");
            break;
    }

}


void
ar6000_cac_event(AR_SOFTC_T *ar, A_UINT8 ac, A_UINT8 cacIndication,
                 A_UINT8 statusCode, A_UINT8 *tspecSuggestion)
{
    WMM_TSPEC_IE    *tspecIe;

    /*
     * This is the TSPEC IE suggestion from AP.
     * Suggestion provided by AP under some error
     * cases, could be helpful for the host app.
     * Check documentation.
     */
    tspecIe = (WMM_TSPEC_IE *)tspecSuggestion;

    /*
     * What do we do, if we get TSPEC rejection? One thought
     * that comes to mind is implictly delete the pstream...
     */
    A_PRINTF("AR6000 CAC notification. "
                "AC = %d, cacIndication = 0x%x, statusCode = 0x%x\n",
                 ac, cacIndication, statusCode);
}

#define AR6000_PRINT_BSSID(_pBss)  do {     \
        A_PRINTF("%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ",\
                 (_pBss)[0],(_pBss)[1],(_pBss)[2],(_pBss)[3],\
                 (_pBss)[4],(_pBss)[5]);  \
} while(0)

void
ar6000_roam_tbl_event(AR_SOFTC_T *ar, WMI_TARGET_ROAM_TBL *pTbl)
{
    A_UINT8 i;

    A_PRINTF("ROAM TABLE NO OF ENTRIES is %d ROAM MODE is %d\n",
              pTbl->numEntries, pTbl->roamMode);
    for (i= 0; i < pTbl->numEntries; i++) {
        A_PRINTF("[%d]bssid %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x ", i,
            pTbl->bssRoamInfo[i].bssid[0], pTbl->bssRoamInfo[i].bssid[1],
            pTbl->bssRoamInfo[i].bssid[2],
            pTbl->bssRoamInfo[i].bssid[3],
            pTbl->bssRoamInfo[i].bssid[4],
            pTbl->bssRoamInfo[i].bssid[5]);
        A_PRINTF("RSSI %d RSSIDT %d LAST RSSI %d UTIL %d ROAM_UTIL %d"
                 " BIAS %d\n",
            pTbl->bssRoamInfo[i].rssi,
            pTbl->bssRoamInfo[i].rssidt,
            pTbl->bssRoamInfo[i].last_rssi,
            pTbl->bssRoamInfo[i].util,
            pTbl->bssRoamInfo[i].roam_util,
            pTbl->bssRoamInfo[i].bias);
    }
}

void
ar6000_wow_list_event(struct ar6_softc *ar, A_UINT8 num_filters, WMI_GET_WOW_LIST_REPLY *wow_reply)
{
    A_UINT8 i,j;

    /*Each event now contains exactly one filter, see bug 26613*/
    A_PRINTF("WOW pattern %d of %d patterns\n", wow_reply->this_filter_num,                 wow_reply->num_filters);
    A_PRINTF("wow mode = %s host mode = %s\n",
            (wow_reply->wow_mode == 0? "disabled":"enabled"),
            (wow_reply->host_mode == 1 ? "awake":"asleep"));


    /*If there are no patterns, the reply will only contain generic
      WoW information. Pattern information will exist only if there are
      patterns present. Bug 26716*/

   /* If this event contains pattern information, display it*/
    if (wow_reply->this_filter_num) {
        i=0;
        A_PRINTF("id=%d size=%d offset=%d\n",
                    wow_reply->wow_filters[i].wow_filter_id,
                    wow_reply->wow_filters[i].wow_filter_size,
                    wow_reply->wow_filters[i].wow_filter_offset);
       A_PRINTF("wow pattern = ");
       for (j=0; j< wow_reply->wow_filters[i].wow_filter_size; j++) {
             A_PRINTF("%2.2x",wow_reply->wow_filters[i].wow_filter_pattern[j]);
        }

        A_PRINTF("\nwow mask = ");
        for (j=0; j< wow_reply->wow_filters[i].wow_filter_size; j++) {
            A_PRINTF("%2.2x",wow_reply->wow_filters[i].wow_filter_mask[j]);
        }
        A_PRINTF("\n");
    }
}

/*
 * Report the Roaming related data collected on the target
 */
void
ar6000_display_roam_time(WMI_TARGET_ROAM_TIME *p)
{
    A_PRINTF("Disconnect Data : BSSID: ");
    AR6000_PRINT_BSSID(p->disassoc_bssid);
    A_PRINTF(" RSSI %d DISASSOC Time %d NO_TXRX_TIME %d\n",
             p->disassoc_bss_rssi,p->disassoc_time,
             p->no_txrx_time);
    A_PRINTF("Connect Data: BSSID: ");
    AR6000_PRINT_BSSID(p->assoc_bssid);
    A_PRINTF(" RSSI %d ASSOC Time %d TXRX_TIME %d\n",
             p->assoc_bss_rssi,p->assoc_time,
             p->allow_txrx_time);
    A_PRINTF("Last Data Tx Time (b4 Disassoc) %d "\
             "First Data Tx Time (after Assoc) %d\n",
             p->last_data_txrx_time, p->first_data_txrx_time);
}

void
ar6000_roam_data_event(AR_SOFTC_T *ar, WMI_TARGET_ROAM_DATA *p)
{
    switch (p->roamDataType) {
        case ROAM_DATA_TIME:
            ar6000_display_roam_time(&p->u.roamTime);
            break;
        default:
            break;
    }
}

void
ar6000_bssInfo_event_rx(AR_SOFTC_T *ar, A_UINT8 *datap, int len)
{
    struct sk_buff *skb;
    WMI_BSS_INFO_HDR *bih = (WMI_BSS_INFO_HDR *)datap;


    if (!ar->arMgmtFilter) {
        return;
    }
    if (((ar->arMgmtFilter & IEEE80211_FILTER_TYPE_BEACON) &&
        (bih->frameType != BEACON_FTYPE))  ||
        ((ar->arMgmtFilter & IEEE80211_FILTER_TYPE_PROBE_RESP) &&
        (bih->frameType != PROBERESP_FTYPE)))
    {
        return;
    }

    if ((skb = A_NETBUF_ALLOC_RAW(len)) != NULL) {

        A_NETBUF_PUT(skb, len);
        A_MEMCPY(A_NETBUF_DATA(skb), datap, len);
        skb->dev = ar->arNetDev;
	printk("MAC RAW...\n");
//        skb->mac.raw = A_NETBUF_DATA(skb);
        skb->ip_summed = CHECKSUM_NONE;
        skb->pkt_type = PACKET_OTHERHOST;
        skb->protocol = __constant_htons(0x0019);
        netif_rx(skb);
    }
}

A_UINT32 wmiSendCmdNum;

A_STATUS
ar6000_control_tx(void *devt, void *osbuf, WMI_PRI_STREAM_ID streamID)
{
    AR_SOFTC_T       *ar = (AR_SOFTC_T *)devt;
    A_STATUS         status = A_OK;
    struct ar_cookie *cookie = NULL;
    int i;

        /* take lock to protect ar6000_alloc_cookie() */
    AR6000_SPIN_LOCK(&ar->arLock, 0);

    do {

        AR_DEBUG2_PRINTF("ar_contrstatus = ol_tx: skb=0x%x, len=0x%x, sid=%d\n",
                         (A_UINT32)osbuf, A_NETBUF_LEN(osbuf), streamID);

        if ((streamID == WMI_CONTROL_PRI) && (ar->arWMIControlEpFull)) {
                /* control endpoint is full, don't allocate resources, we
                 * are just going to drop this packet */
            cookie = NULL;
            AR_DEBUG_PRINTF(" WMI Control EP full, dropping packet : 0x%X, len:%d \n",
                    (A_UINT32)osbuf, A_NETBUF_LEN(osbuf));
        } else {
            cookie = ar6000_alloc_cookie(ar);
        }

        if (cookie == NULL) {
            status = A_NO_MEMORY;
            break;
        }

        if(logWmiRawMsgs) {
            A_PRINTF("WMI cmd send, msgNo %d :", wmiSendCmdNum);
            for(i = 0; i < a_netbuf_to_len(osbuf); i++)
                A_PRINTF("%x ", ((A_UINT8 *)a_netbuf_to_data(osbuf))[i]);
            A_PRINTF("\n");
        }

        wmiSendCmdNum++;

    } while (FALSE);

    if (cookie != NULL) {
            /* got a structure to send it out on */
        ar->arTxPending[streamID]++;

        if (streamID != WMI_CONTROL_PRI) {
            ar->arTotalTxDataPending++;
        }
    }

    AR6000_SPIN_UNLOCK(&ar->arLock, 0);

    if (cookie != NULL) {
        cookie->arc_bp[0] = (A_UINT32)osbuf;
        cookie->arc_bp[1] = 0;
        SET_HTC_PACKET_INFO_TX(&cookie->HtcPkt,
                               cookie,
                               A_NETBUF_DATA(osbuf),
                               A_NETBUF_LEN(osbuf),
                               arWMIStream2EndpointID(ar,streamID),
                               AR6K_CONTROL_PKT_TAG);
            /* this interface is asynchronous, if there is an error, cleanup will happen in the
             * TX completion callback */
        HTCSendPkt(ar->arHtcTarget, &cookie->HtcPkt);
        status = A_OK;
    }

    return status;
}

/* indicate tx activity or inactivity on a WMI stream */
void ar6000_indicate_tx_activity(void *devt, A_UINT8 TrafficClass, A_BOOL Active)
{
    AR_SOFTC_T  *ar = (AR_SOFTC_T *)devt;
    WMI_PRI_STREAM_ID streamid;

    if (ar->arWmiEnabled) {
        streamid = wmi_get_stream_id(ar->arWmi, TrafficClass);
    } else {
            /* for mbox ping testing, the traffic class is mapped directly as a stream ID,
             * see handling of AR6000_XIOCTL_TRAFFIC_ACTIVITY_CHANGE in ioctl.c */
        streamid = (WMI_PRI_STREAM_ID)TrafficClass;
    }

        /* notify HTC, this may cause credit distribution changes */

    HTCIndicateActivityChange(ar->arHtcTarget,
                              arWMIStream2EndpointID(ar,streamid),
                              Active);

}

module_init(ar6000_init_module);
module_exit(ar6000_cleanup_module);

/* Init cookie queue */
static void
ar6000_cookie_init(AR_SOFTC_T *ar)
{
    A_UINT32    i;

    ar->arCookieList = NULL;
    A_MEMZERO(s_ar_cookie_mem, sizeof(s_ar_cookie_mem));

    for (i = 0; i < MAX_COOKIE_NUM; i++) {
        ar6000_free_cookie(ar, &s_ar_cookie_mem[i]);
    }
}

/* cleanup cookie queue */
static void
ar6000_cookie_cleanup(AR_SOFTC_T *ar)
{
    /* It is gone .... */
    ar->arCookieList = NULL;
}

/* Init cookie queue */
static void
ar6000_free_cookie(AR_SOFTC_T *ar, struct ar_cookie * cookie)
{
    /* Insert first */
    A_ASSERT(ar != NULL);
    A_ASSERT(cookie != NULL);
    cookie->arc_list_next = ar->arCookieList;
    ar->arCookieList = cookie;
}

/* cleanup cookie queue */
static struct ar_cookie *
ar6000_alloc_cookie(AR_SOFTC_T  *ar)
{
    struct ar_cookie   *cookie;

    cookie = ar->arCookieList;
    if(cookie != NULL)
    {
        ar->arCookieList = cookie->arc_list_next;
    }

    return cookie;
}

#ifdef SEND_EVENT_TO_APP
/*
 * This function is used to send event which come from taget to
 * the application. The buf which send to application is include
 * the event ID and event content.
 */
#define EVENT_ID_LEN   2
void ar6000_send_event_to_app(AR_SOFTC_T *ar, A_UINT16 eventId,
                              A_UINT8 *datap, int len)
{

#if (WIRELESS_EXT >= 15)

/* note: IWEVCUSTOM only exists in wireless extensions after version 15 */

    char *buf;
    A_UINT16 size;
    union iwreq_data wrqu;

    size = len + EVENT_ID_LEN;

    if (size > IW_CUSTOM_MAX) {
        AR_DEBUG_PRINTF("WMI event ID : 0x%4.4X, len = %d too big for IWEVCUSTOM (max=%d) \n",
                eventId, size, IW_CUSTOM_MAX);
        return;
    }

    buf = A_MALLOC_NOWAIT(size);
    A_MEMZERO(buf, size);
    A_MEMCPY(buf, &eventId, EVENT_ID_LEN);
    A_MEMCPY(buf+EVENT_ID_LEN, datap, len);

    //AR_DEBUG_PRINTF("event ID = %d,len = %d\n",*(A_UINT16*)buf, size);
    A_MEMZERO(&wrqu, sizeof(wrqu));
    wrqu.data.length = size;
    wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);

    A_FREE(buf);
#endif


}
#endif


void
ar6000_tx_retry_err_event(void *devt)
{
    AR_DEBUG2_PRINTF("Tx retries reach maximum!\n");
}

void
ar6000_snrThresholdEvent_rx(void *devt, WMI_SNR_THRESHOLD_VAL newThreshold, A_UINT8 snr)
{
    AR_DEBUG2_PRINTF("snr threshold range %d, snr %d\n", newThreshold, snr);
}

void
ar6000_lqThresholdEvent_rx(void *devt, WMI_LQ_THRESHOLD_VAL newThreshold, A_UINT8 lq)
{
    AR_DEBUG2_PRINTF("lq threshold range %d, lq %d\n", newThreshold, lq);
}



A_UINT32
a_copy_to_user(void *to, const void *from, A_UINT32 n)
{
    return(copy_to_user(to, from, n));
}

A_UINT32
a_copy_from_user(void *to, const void *from, A_UINT32 n)
{
    return(copy_from_user(to, from, n));
}


A_STATUS
ar6000_get_driver_cfg(struct net_device *dev,
                        A_UINT16 cfgParam,
                        void *result)
{

    A_STATUS    ret = 0;

    switch(cfgParam)
    {
        case AR6000_DRIVER_CFG_GET_WLANNODECACHING:
           *((A_UINT32 *)result) = wlanNodeCaching;
           break;
        case AR6000_DRIVER_CFG_LOG_RAW_WMI_MSGS:
           *((A_UINT32 *)result) = logWmiRawMsgs;
            break;
        default:
           ret = EINVAL;
           break;
    }

    return ret;
}

void
ar6000_keepalive_rx(void *devt, A_UINT8 configured)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)devt;

    ar->arKeepaliveConfigured = configured;
    wake_up(&arEvent);
}

void
ar6000_pmkid_list_event(void *devt, A_UINT8 numPMKID, WMI_PMKID *pmkidList)
{
    A_UINT8 i, j;

    A_PRINTF("Number of Cached PMKIDs is %d\n", numPMKID);

    for (i = 0; i < numPMKID; i++) {
        A_PRINTF("\nPMKID %d ", i);
            for (j = 0; j < WMI_PMKID_LEN; j++) {
                A_PRINTF("%2.2x", pmkidList->pmkid[j]);
            }
        pmkidList++;
    }
}

#ifdef USER_KEYS
static A_STATUS

ar6000_reinstall_keys(AR_SOFTC_T *ar, A_UINT8 key_op_ctrl)
{
    A_STATUS status = A_OK;
    struct ieee80211req_key *uik = &ar->user_saved_keys.ucast_ik;
    struct ieee80211req_key *bik = &ar->user_saved_keys.bcast_ik;
    CRYPTO_TYPE  keyType = ar->user_saved_keys.keyType;

    if (IEEE80211_CIPHER_CCKM_KRK != uik->ik_type) {
        if (NONE_CRYPT == keyType) {
            goto _reinstall_keys_out;
        }

        if (uik->ik_keylen) {
            status = wmi_addKey_cmd(ar->arWmi, uik->ik_keyix,
                    ar->user_saved_keys.keyType, PAIRWISE_USAGE,
                    uik->ik_keylen, (A_UINT8 *)&uik->ik_keyrsc,
                    uik->ik_keydata, key_op_ctrl, SYNC_BEFORE_WMIFLAG);
        }

    } else {
        status = wmi_add_krk_cmd(ar->arWmi, uik->ik_keydata);
    }

    if (IEEE80211_CIPHER_CCKM_KRK != bik->ik_type) {
        if (NONE_CRYPT == keyType) {
            goto _reinstall_keys_out;
        }

        if (bik->ik_keylen) {
            status = wmi_addKey_cmd(ar->arWmi, bik->ik_keyix,
                    ar->user_saved_keys.keyType, GROUP_USAGE,
                    bik->ik_keylen, (A_UINT8 *)&bik->ik_keyrsc,
                    bik->ik_keydata, key_op_ctrl, NO_SYNC_WMIFLAG);
        }
    } else {
        status = wmi_add_krk_cmd(ar->arWmi, bik->ik_keydata);
    }

_reinstall_keys_out:
    ar->user_savedkeys_stat = USER_SAVEDKEYS_STAT_INIT;
    ar->user_key_ctrl      = 0;

    return status;
}
#endif /* USER_KEYS */


void
ar6000_dset_open_req(
    void *context,
    A_UINT32 id,
    A_UINT32 targHandle,
    A_UINT32 targReplyFn,
    A_UINT32 targReplyArg)
{
}

void
ar6000_dset_close(
    void *context,
    A_UINT32 access_cookie)
{
    return;
}

void
ar6000_dset_data_req(
   void *context,
   A_UINT32 accessCookie,
   A_UINT32 offset,
   A_UINT32 length,
   A_UINT32 targBuf,
   A_UINT32 targReplyFn,
   A_UINT32 targReplyArg)
{
}
