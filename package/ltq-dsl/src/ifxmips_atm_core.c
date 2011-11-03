/******************************************************************************
**
** FILE NAME    : ifxmips_atm_core.c
** PROJECT      : UEIP
** MODULES      : ATM
**
** DATE         : 7 Jul 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : ATM driver common source file (core functions)
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/



/*
 * ####################################
 *              Version No.
 * ####################################
 */

#define IFX_ATM_VER_MAJOR               1
#define IFX_ATM_VER_MID                 0
#define IFX_ATM_VER_MINOR               19



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/atmdev.h>
#include <linux/atm.h>
#include <linux/clk.h>
#include <linux/interrupt.h>

/*
 *  Chip Specific Head File
 */
#include <lantiq_soc.h>
#include "ifxmips_compat.h"
#define IFX_MEI_BSP	1
#include "ifxmips_mei_interface.h"
#include "ifxmips_atm_core.h"



/*
 * ####################################
 *        Kernel Version Adaption
 * ####################################
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
  #define MODULE_PARM_ARRAY(a, b)   module_param_array(a, int, NULL, 0)
  #define MODULE_PARM(a, b)         module_param(a, int, 0)
#else
  #define MODULE_PARM_ARRAY(a, b)   MODULE_PARM(a, b)
#endif



/*!
  \addtogroup IFXMIPS_ATM_MODULE_PARAMS
 */
/*@{*/
/*
 * ####################################
 *   Parameters to Configure PPE
 * ####################################
 */
/*!
  \brief QSB cell delay variation due to concurrency
 */
static int qsb_tau   = 1;                       /*  QSB cell delay variation due to concurrency     */
/*!
  \brief QSB scheduler burst length
 */
static int qsb_srvm  = 0x0F;                    /*  QSB scheduler burst length                      */
/*!
  \brief QSB time step, all legal values are 1, 2, 4
 */
static int qsb_tstep = 4 ;                      /*  QSB time step, all legal values are 1, 2, 4     */

/*!
  \brief Write descriptor delay
 */
static int write_descriptor_delay  = 0x20;      /*  Write descriptor delay                          */

/*!
  \brief AAL5 padding byte ('~')
 */
static int aal5_fill_pattern       = 0x007E;    /*  AAL5 padding byte ('~')                         */
/*!
  \brief Max frame size for RX
 */
static int aal5r_max_packet_size   = 0x0700;    /*  Max frame size for RX                           */
/*!
  \brief Min frame size for RX
 */
static int aal5r_min_packet_size   = 0x0000;    /*  Min frame size for RX                           */
/*!
  \brief Max frame size for TX
 */
static int aal5s_max_packet_size   = 0x0700;    /*  Max frame size for TX                           */
/*!
  \brief Min frame size for TX
 */
static int aal5s_min_packet_size   = 0x0000;    /*  Min frame size for TX                           */
/*!
  \brief Drop error packet in RX path
 */
static int aal5r_drop_error_packet = 1;         /*  Drop error packet in RX path                    */

/*!
  \brief Number of descriptors per DMA RX channel
 */
static int dma_rx_descriptor_length = 128;      /*  Number of descriptors per DMA RX channel        */
/*!
  \brief Number of descriptors per DMA TX channel
 */
static int dma_tx_descriptor_length = 64;       /*  Number of descriptors per DMA TX channel        */
/*!
  \brief PPE core clock cycles between descriptor write and effectiveness in external RAM
 */
static int dma_rx_clp1_descriptor_threshold = 38;
/*@}*/

MODULE_PARM(qsb_tau, "i");
MODULE_PARM_DESC(qsb_tau, "Cell delay variation. Value must be > 0");
MODULE_PARM(qsb_srvm, "i");
MODULE_PARM_DESC(qsb_srvm, "Maximum burst size");
MODULE_PARM(qsb_tstep, "i");
MODULE_PARM_DESC(qsb_tstep, "n*32 cycles per sbs cycles n=1,2,4");

MODULE_PARM(write_descriptor_delay, "i");
MODULE_PARM_DESC(write_descriptor_delay, "PPE core clock cycles between descriptor write and effectiveness in external RAM");

MODULE_PARM(aal5_fill_pattern, "i");
MODULE_PARM_DESC(aal5_fill_pattern, "Filling pattern (PAD) for AAL5 frames");
MODULE_PARM(aal5r_max_packet_size, "i");
MODULE_PARM_DESC(aal5r_max_packet_size, "Max packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5r_min_packet_size, "i");
MODULE_PARM_DESC(aal5r_min_packet_size, "Min packet size in byte for downstream AAL5 frames");
MODULE_PARM(aal5s_max_packet_size, "i");
MODULE_PARM_DESC(aal5s_max_packet_size, "Max packet size in byte for upstream AAL5 frames");
MODULE_PARM(aal5s_min_packet_size, "i");
MODULE_PARM_DESC(aal5s_min_packet_size, "Min packet size in byte for upstream AAL5 frames");
MODULE_PARM(aal5r_drop_error_packet, "i");
MODULE_PARM_DESC(aal5r_drop_error_packet, "Non-zero value to drop error packet for downstream");

MODULE_PARM(dma_rx_descriptor_length, "i");
MODULE_PARM_DESC(dma_rx_descriptor_length, "Number of descriptor assigned to DMA RX channel (>16)");
MODULE_PARM(dma_tx_descriptor_length, "i");
MODULE_PARM_DESC(dma_tx_descriptor_length, "Number of descriptor assigned to DMA TX channel (>16)");
MODULE_PARM(dma_rx_clp1_descriptor_threshold, "i");
MODULE_PARM_DESC(dma_rx_clp1_descriptor_threshold, "Descriptor threshold for cells with cell loss priority 1");



/*
 * ####################################
 *              Definition
 * ####################################
 */

#define ENABLE_LED_FRAMEWORK                    1

#define DUMP_SKB_LEN                            ~0



/*
 * ####################################
 *             Declaration
 * ####################################
 */

/*
 *  Network Operations
 */
static int ppe_ioctl(struct atm_dev *, unsigned int, void *);
static int ppe_open(struct atm_vcc *);
static void ppe_close(struct atm_vcc *);
static int ppe_send(struct atm_vcc *, struct sk_buff *);
static int ppe_send_oam(struct atm_vcc *, void *, int);
static int ppe_change_qos(struct atm_vcc *, struct atm_qos *, int);

/*
 *  ADSL LED
 */
static INLINE void adsl_led_flash(void);

/*
 *  64-bit operation used by MIB calculation
 */
static INLINE void u64_add_u32(ppe_u64_t, unsigned int, ppe_u64_t *);

/*
 *  buffer manage functions
 */
static INLINE struct sk_buff* alloc_skb_rx(void);
static INLINE struct sk_buff* alloc_skb_tx(unsigned int);
struct sk_buff* atm_alloc_tx(struct atm_vcc *, unsigned int);
static INLINE void atm_free_tx_skb_vcc(struct sk_buff *, struct atm_vcc *);
static INLINE struct sk_buff *get_skb_rx_pointer(unsigned int);
static INLINE int get_tx_desc(unsigned int);

/*
 *  mailbox handler and signal function
 */
static INLINE void mailbox_oam_rx_handler(void);
static INLINE void mailbox_aal_rx_handler(void);
#if defined(ENABLE_TASKLET) && ENABLE_TASKLET
  static void do_ppe_tasklet(unsigned long);
#endif
static irqreturn_t mailbox_irq_handler(int, void *);
static INLINE void mailbox_signal(unsigned int, int);

/*
 *  QSB & HTU setting functions
 */
static void set_qsb(struct atm_vcc *, struct atm_qos *, unsigned int);
static void qsb_global_set(void);
static INLINE void set_htu_entry(unsigned int, unsigned int, unsigned int, int, int);
static INLINE void clear_htu_entry(unsigned int);
static void validate_oam_htu_entry(void);
static void invalidate_oam_htu_entry(void);

/*
 *  look up for connection ID
 */
static INLINE int find_vpi(unsigned int);
static INLINE int find_vpivci(unsigned int, unsigned int);
static INLINE int find_vcc(struct atm_vcc *);

/*
 *  ReTX functions
 */
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
  static void retx_polling_func(unsigned long);
  static int init_atm_tc_retrans_param(void);
  static void clear_atm_tc_retrans_param(void);
#endif


/*
 *  Debug Functions
 */
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
  static void dump_skb(struct sk_buff *, unsigned int, char *, int, int, int);
#else
  #define dump_skb(skb, len, title, port, ch, is_tx)    do {} while (0)
#endif
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static void skb_swap(struct sk_buff *, unsigned int);
#else
  #define skb_swap(skb, byteoff)                        do {} while (0)
#endif

/*
 *  Proc File Functions
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
static int proc_read_version(char *, char **, off_t, int, int *, void *);
static int proc_read_mib(char *, char **, off_t, int, int *, void *);
static int proc_write_mib(struct file *, const char *, unsigned long, void *);
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
  static int proc_read_retx_mib(char *, char **, off_t, int, int *, void *);
  static int proc_write_retx_mib(struct file *, const char *, unsigned long, void *);
#endif
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
  static int proc_write_dbg(struct file *, const char *, unsigned long, void *);
  static int proc_write_mem(struct file *, const char *, unsigned long, void *);
 #if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE)
  static int proc_read_pp32(char *, char **, off_t, int, int *, void *);
  static int proc_write_pp32(struct file *, const char *, unsigned long, void *);
 #endif
#endif
#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC
  static int proc_read_htu(char *, char **, off_t, int, int *, void *);
  static int proc_read_txq(char *, char **, off_t, int, int *, void *);
 #if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
  static int proc_read_retx_fw(char *, char **, off_t, int, int *, void *);
  static int proc_read_retx_stats(char *, char **, off_t, int, int *, void *);
  static int proc_write_retx_stats(struct file *, const char *, unsigned long, void *);
  static int proc_read_retx_cfg(char *, char **, off_t, int, int *, void *);
  static int proc_write_retx_cfg(struct file *, const char *, unsigned long, void *);
  static int proc_read_retx_dsl_param(char *, char **, off_t, int, int *, void *);
 #endif
#endif

/*
 *  Proc Help Functions
 */
static int stricmp(const char *, const char *);
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int strincmp(const char *, const char *, int);
  static int get_token(char **, char **, int *, int *);
  static unsigned int get_number(char **, int *, int);
  static void ignore_space(char **, int *);
#endif
static INLINE int ifx_atm_version(char *);

/*
 *  Init & clean-up functions
 */
static INLINE void check_parameters(void);
static INLINE int init_priv_data(void);
static INLINE void clear_priv_data(void);
static INLINE void init_rx_tables(void);
static INLINE void init_tx_tables(void);

/*
 *  Exteranl Function
 */
#if defined(CONFIG_IFX_OAM) || defined(CONFIG_IFX_OAM_MODULE)
  extern void ifx_push_oam(unsigned char *);
#else
  static inline void ifx_push_oam(unsigned char *dummy) {}
#endif
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
 #if !defined(ENABLE_LED_FRAMEWORK) || !ENABLE_LED_FRAMEWORK
  extern int ifx_mei_atm_led_blink(void) __attribute__ ((weak));
 #endif
  extern int ifx_mei_atm_showtime_check(int *is_showtime, struct port_cell_info *port_cell, void **xdata_addr) __attribute__ ((weak));
#else
 #if !defined(ENABLE_LED_FRAMEWORK) || !ENABLE_LED_FRAMEWORK
  static inline int ifx_mei_atm_led_blink(void) { return IFX_SUCCESS; }
 #endif
  static inline int ifx_mei_atm_showtime_check(int *is_showtime, struct port_cell_info *port_cell, void **xdata_addr)
  {
    if ( is_showtime != NULL )
        *is_showtime = 0;
    return IFX_SUCCESS;
  }
#endif

/*
 *  External variable
 */
struct sk_buff* (*ifx_atm_alloc_tx)(struct atm_vcc *, unsigned int) = NULL;


//extern struct sk_buff* (*ifx_atm_alloc_tx)(struct atm_vcc *, unsigned int);
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
  extern int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *) __attribute__ ((weak));
  extern int (*ifx_mei_atm_showtime_exit)(void) __attribute__ ((weak));
#else
  int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_enter);
  int (*ifx_mei_atm_showtime_exit)(void) = NULL;
  EXPORT_SYMBOL(ifx_mei_atm_showtime_exit);
#endif



/*
 * ####################################
 *            Local Variable
 * ####################################
 */

static struct atm_priv_data g_atm_priv_data;

static struct atmdev_ops g_ifx_atm_ops = {
    .open       = ppe_open,
    .close      = ppe_close,
    .ioctl      = ppe_ioctl,
    .send       = ppe_send,
    .send_oam   = ppe_send_oam,
    .change_qos = ppe_change_qos,
    .owner      = THIS_MODULE,
};

#if defined(ENABLE_TASKLET) && ENABLE_TASKLET
  DECLARE_TASKLET(g_dma_tasklet, do_ppe_tasklet, 0);
#endif

static int g_showtime = 0;
static void *g_xdata_addr = NULL;

#if 0 /*--- defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK ---*/
  static void *g_data_led_trigger = NULL;
#endif

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
  static unsigned long g_retx_playout_buffer = 0;

  static volatile int g_retx_htu = 1;
  static struct dsl_param g_dsl_param = {0};
  static int g_retx_polling_cnt = HZ;
  static struct timeval g_retx_polling_start = {0}, g_retx_polling_end = {0};
  static struct timer_list g_retx_polling_timer;
#endif

unsigned int ifx_atm_dbg_enable = 0;

static struct proc_dir_entry* g_atm_dir = NULL;



/*
 * ####################################
 *            Local Function
 * ####################################
 */

static int ppe_ioctl(struct atm_dev *dev, unsigned int cmd, void *arg)
{
    int ret = 0;
    atm_cell_ifEntry_t mib_cell;
    atm_aal5_ifEntry_t mib_aal5;
    atm_aal5_vcc_x_t mib_vcc;
    unsigned int value;
    int conn;

    if ( _IOC_TYPE(cmd) != PPE_ATM_IOC_MAGIC
        || _IOC_NR(cmd) >= PPE_ATM_IOC_MAXNR )
        return -ENOTTY;

    if ( _IOC_DIR(cmd) & _IOC_READ )
        ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
    else if ( _IOC_DIR(cmd) & _IOC_WRITE )
        ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
    if ( ret )
        return -EFAULT;

    switch ( cmd )
    {
    case PPE_ATM_MIB_CELL:  /*  cell level  MIB */
        /*  These MIB should be read at ARC side, now put zero only.    */
        mib_cell.ifHCInOctets_h = 0;
        mib_cell.ifHCInOctets_l = 0;
        mib_cell.ifHCOutOctets_h = 0;
        mib_cell.ifHCOutOctets_l = 0;
        mib_cell.ifInErrors = 0;
        mib_cell.ifInUnknownProtos = WAN_MIB_TABLE->wrx_drophtu_cell;
        mib_cell.ifOutErrors = 0;

        ret = sizeof(mib_cell) - copy_to_user(arg, &mib_cell, sizeof(mib_cell));
        break;

    case PPE_ATM_MIB_AAL5:  /*  AAL5 MIB    */
        value = WAN_MIB_TABLE->wrx_total_byte;
        u64_add_u32(g_atm_priv_data.wrx_total_byte, value - g_atm_priv_data.prev_wrx_total_byte, &g_atm_priv_data.wrx_total_byte);
        g_atm_priv_data.prev_wrx_total_byte = value;
        mib_aal5.ifHCInOctets_h = g_atm_priv_data.wrx_total_byte.h;
        mib_aal5.ifHCInOctets_l = g_atm_priv_data.wrx_total_byte.l;

        value = WAN_MIB_TABLE->wtx_total_byte;
        u64_add_u32(g_atm_priv_data.wtx_total_byte, value - g_atm_priv_data.prev_wtx_total_byte, &g_atm_priv_data.wtx_total_byte);
        g_atm_priv_data.prev_wtx_total_byte = value;
        mib_aal5.ifHCOutOctets_h = g_atm_priv_data.wtx_total_byte.h;
        mib_aal5.ifHCOutOctets_l = g_atm_priv_data.wtx_total_byte.l;

        mib_aal5.ifInUcastPkts  = g_atm_priv_data.wrx_pdu;
        mib_aal5.ifOutUcastPkts = WAN_MIB_TABLE->wtx_total_pdu;
        mib_aal5.ifInErrors     = WAN_MIB_TABLE->wrx_err_pdu;
        mib_aal5.ifInDiscards   = WAN_MIB_TABLE->wrx_dropdes_pdu + g_atm_priv_data.wrx_drop_pdu;
        mib_aal5.ifOutErros     = g_atm_priv_data.wtx_err_pdu;
        mib_aal5.ifOutDiscards  = g_atm_priv_data.wtx_drop_pdu;

        ret = sizeof(mib_aal5) - copy_to_user(arg, &mib_aal5, sizeof(mib_aal5));
        break;

    case PPE_ATM_MIB_VCC:   /*  VCC related MIB */
        copy_from_user(&mib_vcc, arg, sizeof(mib_vcc));
        conn = find_vpivci(mib_vcc.vpi, mib_vcc.vci);
        if ( conn >= 0 )
        {
            mib_vcc.mib_vcc.aal5VccCrcErrors     = g_atm_priv_data.conn[conn].aal5_vcc_crc_err;
            mib_vcc.mib_vcc.aal5VccOverSizedSDUs = g_atm_priv_data.conn[conn].aal5_vcc_oversize_sdu;
            mib_vcc.mib_vcc.aal5VccSarTimeOuts   = 0;   /*  no timer support    */
            ret = sizeof(mib_vcc) - copy_to_user(arg, &mib_vcc, sizeof(mib_vcc));
        }
        else
            ret = -EINVAL;
        break;

    default:
        ret = -ENOIOCTLCMD;
    }

    return ret;
}

static int ppe_open(struct atm_vcc *vcc)
{
    int ret;
    short vpi = vcc->vpi;
    int   vci = vcc->vci;
    struct port *port = &g_atm_priv_data.port[(int)vcc->dev->dev_data];
    int conn;
    int f_enable_irq = 0;
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    int sys_flag;
#endif

    if ( vcc->qos.aal != ATM_AAL5 && vcc->qos.aal != ATM_AAL0 )
        return -EPROTONOSUPPORT;

#if !defined(DISABLE_QOS_WORKAROUND) || !DISABLE_QOS_WORKAROUND
    /*  check bandwidth */

    if (vcc->qos.txtp.traffic_class == ATM_CBR && 
                vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
    {
        printk("CBR set. %s, line %d returns EINVAL\n", __FUNCTION__, __LINE__);
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }
    if(vcc->qos.txtp.traffic_class == ATM_VBR_RT && 
                vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
    {
        printk("VBR RT set. %s, line %d returns EINVAL\n", __FUNCTION__, __LINE__);
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    if (vcc->qos.txtp.traffic_class == ATM_VBR_NRT && 
                vcc->qos.txtp.scr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
    {
        printk("VBR NRT set. %s, line %d returns EINVAL\n", __FUNCTION__, __LINE__);
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    if (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS && 
                vcc->qos.txtp.min_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate)) 
    {
        printk("UBR PLUS set. %s, line %d returns EINVAL\n", __FUNCTION__, __LINE__);
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

#endif

    /*  check existing vpi,vci  */
    conn = find_vpivci(vpi, vci);
    if ( conn >= 0 ) {
        ret = -EADDRINUSE;
        goto PPE_OPEN_EXIT;
    }

    /*  check whether it need to enable irq */
    if ( g_atm_priv_data.conn_table == 0 )
        f_enable_irq = 1;

    /*  allocate connection */
    for ( conn = 0; conn < MAX_PVC_NUMBER; conn++ ) {
        if ( test_and_set_bit(conn, &g_atm_priv_data.conn_table) == 0 ) {
            g_atm_priv_data.conn[conn].vcc = vcc;
            break;
        }
    }
    if ( conn == MAX_PVC_NUMBER )
    {
        printk("max_pvc_number reached\n");
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    /*  reserve bandwidth   */
    switch ( vcc->qos.txtp.traffic_class ) {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_current_cell_rate += vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
        port->tx_current_cell_rate += vcc->qos.txtp.scr;
        break;
    case ATM_UBR_PLUS:
        port->tx_current_cell_rate += vcc->qos.txtp.min_pcr;
        break;
    }

    /*  set qsb */
    set_qsb(vcc, &vcc->qos, conn);

    /*  update atm_vcc structure    */
    vcc->itf = (int)vcc->dev->dev_data;
    vcc->vpi = vpi;
    vcc->vci = vci;
    set_bit(ATM_VF_READY, &vcc->flags);

    /*  enable irq  */
    if (f_enable_irq ) {
        ifx_atm_alloc_tx = atm_alloc_tx;

        *MBOX_IGU1_ISRC = (1 << RX_DMA_CH_AAL) | (1 << RX_DMA_CH_OAM);
        *MBOX_IGU1_IER  = (1 << RX_DMA_CH_AAL) | (1 << RX_DMA_CH_OAM);

        enable_irq(PPE_MAILBOX_IGU1_INT);
    }

    /*  set port    */
    WTX_QUEUE_CONFIG(conn + FIRST_QSB_QID)->sbid = (int)vcc->dev->dev_data;

    /*  set htu entry   */
    set_htu_entry(vpi, vci, conn, vcc->qos.aal == ATM_AAL5 ? 1 : 0, 0);

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    //  ReTX: occupy second QID
    local_irq_save(sys_flag);
    if ( g_retx_htu && vcc->qos.aal == ATM_AAL5 )
    {
        int retx_conn = (conn + 8) % 16;    //  ReTX queue

        if ( retx_conn < MAX_PVC_NUMBER && test_and_set_bit(retx_conn, &g_atm_priv_data.conn_table) == 0 ) {
            g_atm_priv_data.conn[retx_conn].vcc = vcc;
            set_htu_entry(vpi, vci, retx_conn, vcc->qos.aal == ATM_AAL5 ? 1 : 0, 1);
        }
    }
    local_irq_restore(sys_flag);
#endif

    ret = 0;

PPE_OPEN_EXIT:
    return ret;
}

static void ppe_close(struct atm_vcc *vcc)
{
    int conn;
    struct port *port;
    struct connection *connection;
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    int sys_flag;
#endif

    if ( vcc == NULL )
        return;

    /*  get connection id   */
    conn = find_vcc(vcc);
    if ( conn < 0 ) {
        err("can't find vcc");
        goto PPE_CLOSE_EXIT;
    }
    connection = &g_atm_priv_data.conn[conn];
    port = &g_atm_priv_data.port[connection->port];

    /*  clear htu   */
    clear_htu_entry(conn);

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    //  ReTX: release second QID
    local_irq_save(sys_flag);
    if ( g_retx_htu && vcc->qos.aal == ATM_AAL5 )
    {
        int retx_conn = (conn + 8) % 16;    //  ReTX queue

        if ( retx_conn < MAX_PVC_NUMBER && g_atm_priv_data.conn[retx_conn].vcc == vcc ) {
            clear_htu_entry(retx_conn);
            g_atm_priv_data.conn[retx_conn].vcc = NULL;
            g_atm_priv_data.conn[retx_conn].aal5_vcc_crc_err = 0;
            g_atm_priv_data.conn[retx_conn].aal5_vcc_oversize_sdu = 0;
            clear_bit(retx_conn, &g_atm_priv_data.conn_table);
        }
    }
    local_irq_restore(sys_flag);
#endif

    /*  release connection  */
    connection->vcc = NULL;
    connection->aal5_vcc_crc_err = 0;
    connection->aal5_vcc_oversize_sdu = 0;
    clear_bit(conn, &g_atm_priv_data.conn_table);

    /*  disable irq */
    if ( g_atm_priv_data.conn_table == 0 ) {
        disable_irq(PPE_MAILBOX_IGU1_INT);
        ifx_atm_alloc_tx = NULL;
    }

    /*  release bandwidth   */
    switch ( vcc->qos.txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_current_cell_rate -= vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
        port->tx_current_cell_rate -= vcc->qos.txtp.scr;
        break;
    case ATM_UBR_PLUS:
        port->tx_current_cell_rate -= vcc->qos.txtp.min_pcr;
        break;
    }

PPE_CLOSE_EXIT:
    return;
}

static int ppe_send(struct atm_vcc *vcc, struct sk_buff *skb)
{
    int ret;
    int conn;
    int desc_base;
    struct tx_descriptor reg_desc = {0};

    if ( vcc == NULL || skb == NULL )
        return -EINVAL;

    skb_orphan(skb);
    skb_get(skb);
    atm_free_tx_skb_vcc(skb, vcc);

    conn = find_vcc(vcc);
    if ( conn < 0 ) {
        ret = -EINVAL;
        goto FIND_VCC_FAIL;
    }

    if ( !g_showtime ) {
        err("not in showtime");
        ret = -EIO;
        goto PPE_SEND_FAIL;
    }

    if ( vcc->qos.aal == ATM_AAL5 ) {
        int byteoff;
        int datalen;
        struct tx_inband_header *header;

        datalen = skb->len;
        byteoff = (unsigned int)skb->data & (DATA_BUFFER_ALIGNMENT - 1);

        if ( skb_headroom(skb) < byteoff + TX_INBAND_HEADER_LENGTH ) {
            struct sk_buff *new_skb;

            new_skb = alloc_skb_tx(datalen);
            if ( new_skb == NULL ) {
                err("ALLOC_SKB_TX_FAIL");
                ret = -ENOMEM;
                goto PPE_SEND_FAIL;
            }
            skb_put(new_skb, datalen);
            memcpy(new_skb->data, skb->data, datalen);
            dev_kfree_skb_any(skb);
            skb = new_skb;
            byteoff = (unsigned int)skb->data & (DATA_BUFFER_ALIGNMENT - 1);
        }

        skb_push(skb, byteoff + TX_INBAND_HEADER_LENGTH);

        header = (struct tx_inband_header *)skb->data;

        /*  setup inband trailer    */
        header->uu   = 0;
        header->cpi  = 0;
        header->pad  = aal5_fill_pattern;
        header->res1 = 0;

        /*  setup cell header   */
        header->clp  = (vcc->atm_options & ATM_ATMOPT_CLP) ? 1 : 0;
        header->pti  = ATM_PTI_US0;
        header->vci  = vcc->vci;
        header->vpi  = vcc->vpi;
        header->gfc  = 0;

        /*  setup descriptor    */
        reg_desc.dataptr = (unsigned int)skb->data >> 2;
        reg_desc.datalen = datalen;
        reg_desc.byteoff = byteoff;
        reg_desc.iscell  = 0;
    }
    else {
        /*  if data pointer is not aligned, allocate new sk_buff    */
        if ( ((unsigned int)skb->data & (DATA_BUFFER_ALIGNMENT - 1)) != 0 ) {
            struct sk_buff *new_skb;

            err("skb->data not aligned");

            new_skb = alloc_skb_tx(skb->len);
            if ( new_skb == NULL ) {
                err("ALLOC_SKB_TX_FAIL");
                ret = -ENOMEM;
                goto PPE_SEND_FAIL;
            }
            skb_put(new_skb, skb->len);
            memcpy(new_skb->data, skb->data, skb->len);
            dev_kfree_skb_any(skb);
            skb = new_skb;
        }

        reg_desc.dataptr = (unsigned int)skb->data >> 2;
        reg_desc.datalen = skb->len;
        reg_desc.byteoff = 0;
        reg_desc.iscell  = 1;
    }

    reg_desc.own = 1;
    reg_desc.c = 1;
    reg_desc.sop = reg_desc.eop = 1;

    desc_base = get_tx_desc(conn);
    if ( desc_base < 0 ) {
        err("ALLOC_TX_CONNECTION_FAIL");
        ret = -EIO;
        goto PPE_SEND_FAIL;
    }

    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx);
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_pdu++;

    /*  update descriptor send pointer  */
    if ( g_atm_priv_data.conn[conn].tx_skb[desc_base] != NULL )
        dev_kfree_skb_any(g_atm_priv_data.conn[conn].tx_skb[desc_base]);
    g_atm_priv_data.conn[conn].tx_skb[desc_base] = skb;

    /*  write discriptor to memory and write back cache */
#ifdef CONFIG_DEBUG_SLAB
    /* be sure that "redzone 1" is written back to memory */
    dma_cache_wback((unsigned long)skb->head, 32);
#endif
    dma_cache_wback((unsigned long)skb_shinfo(skb), sizeof(struct skb_shared_info));
    dma_cache_wback((unsigned long)skb->data, skb->len);
    g_atm_priv_data.conn[conn].tx_desc[desc_base] = reg_desc;
     
    dump_skb(skb, DUMP_SKB_LEN, (char *)__func__, 0, conn, 1);

    mailbox_signal(conn, 1);

    adsl_led_flash();

    return 0;

FIND_VCC_FAIL:
    err("FIND_VCC_FAIL");
    g_atm_priv_data.wtx_err_pdu++;
    dev_kfree_skb_any(skb);
    return ret;

PPE_SEND_FAIL:
    if ( vcc->qos.aal == ATM_AAL5 )
        g_atm_priv_data.wtx_drop_pdu++;
    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx_err);
    dev_kfree_skb_any(skb);
    return ret;
}

static int ppe_send_oam(struct atm_vcc *vcc, void *cell, int flags)
{
    int conn;
    struct uni_cell_header *uni_cell_header = (struct uni_cell_header *)cell;
    int desc_base;
    struct sk_buff *skb;
    struct tx_descriptor reg_desc = {0};

    if ( ((uni_cell_header->pti == ATM_PTI_SEGF5 || uni_cell_header->pti == ATM_PTI_E2EF5)
        && find_vpivci(uni_cell_header->vpi, uni_cell_header->vci) < 0)
        || ((uni_cell_header->vci == 0x03 || uni_cell_header->vci == 0x04)
        && find_vpi(uni_cell_header->vpi) < 0) )
        return -EINVAL;

    if ( !g_showtime ) {
        err("not in showtime");
        return -EIO;
    }

    conn = find_vcc(vcc);
    if ( conn < 0 ) {
        err("FIND_VCC_FAIL");
        return -EINVAL;
    }

    skb = alloc_skb_tx(CELL_SIZE);
    if ( skb == NULL ) {
        err("ALLOC_SKB_TX_FAIL");
        return -ENOMEM;
    }
    memcpy(skb->data, cell, CELL_SIZE);

    reg_desc.dataptr = (unsigned int)skb->data >> 2;
    reg_desc.datalen = CELL_SIZE;
    reg_desc.byteoff = 0;
    reg_desc.iscell  = 1;

    reg_desc.own = 1;
    reg_desc.c = 1;
    reg_desc.sop = reg_desc.eop = 1;

    desc_base = get_tx_desc(conn);
    if ( desc_base < 0 ) {
        dev_kfree_skb_any(skb);
        err("ALLOC_TX_CONNECTION_FAIL");
        return -EIO;
    }

    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx);

    /*  update descriptor send pointer  */
    if ( g_atm_priv_data.conn[conn].tx_skb[desc_base] != NULL )
        dev_kfree_skb_any(g_atm_priv_data.conn[conn].tx_skb[desc_base]);
    g_atm_priv_data.conn[conn].tx_skb[desc_base] = skb;

    /*  write discriptor to memory and write back cache */
    g_atm_priv_data.conn[conn].tx_desc[desc_base] = reg_desc;
    dma_cache_wback((unsigned long)skb->data, CELL_SIZE);

    dump_skb(skb, DUMP_SKB_LEN, (char *)__func__, 0, conn, 1);

    if ( vcc->qos.aal == ATM_AAL5 && (ifx_atm_dbg_enable & DBG_ENABLE_MASK_MAC_SWAP) ) {
        skb_swap(skb, reg_desc.byteoff);
    }

    mailbox_signal(conn, 1);

    adsl_led_flash();

    return 0;
}

static int ppe_change_qos(struct atm_vcc *vcc, struct atm_qos *qos, int flags)
{
    int conn;

    if ( vcc == NULL || qos == NULL )
        return -EINVAL;

    conn = find_vcc(vcc);
    if ( conn < 0 )
        return -EINVAL;

    set_qsb(vcc, qos, conn);

    return 0;
}

static INLINE void adsl_led_flash(void)
{
#if 0
#if defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK
    if ( g_data_led_trigger != NULL )
        ifx_led_trigger_activate(g_data_led_trigger);
#else
    if (!IS_ERR(&ifx_mei_atm_led_blink) && &ifx_mei_atm_led_blink )
        ifx_mei_atm_led_blink();
#endif
#endif
}

/*
 *  Description:
 *    Add a 32-bit value to 64-bit value, and put result in a 64-bit variable.
 *  Input:
 *    opt1 --- ppe_u64_t, first operand, a 64-bit unsigned integer value
 *    opt2 --- unsigned int, second operand, a 32-bit unsigned integer value
 *    ret  --- ppe_u64_t, pointer to a variable to hold result
 *  Output:
 *    none
 */
static INLINE void u64_add_u32(ppe_u64_t opt1, unsigned int opt2, ppe_u64_t *ret)
{
    ret->l = opt1.l + opt2;
    if ( ret->l < opt1.l || ret->l < opt2 )
        ret->h++;
}

static INLINE struct sk_buff* alloc_skb_rx(void)
{
    struct sk_buff *skb;

    skb = dev_alloc_skb(RX_DMA_CH_AAL_BUF_SIZE + DATA_BUFFER_ALIGNMENT);
    if ( skb != NULL ) {
        /*  must be burst length alignment  */
        if ( ((unsigned int)skb->data & (DATA_BUFFER_ALIGNMENT - 1)) != 0 )
            skb_reserve(skb, ~((unsigned int)skb->data + (DATA_BUFFER_ALIGNMENT - 1)) & (DATA_BUFFER_ALIGNMENT - 1));
        /*  pub skb in reserved area "skb->data - 4"    */
        *((struct sk_buff **)skb->data - 1) = skb;
        /*  write back and invalidate cache */
        dma_cache_wback_inv((unsigned long)skb->data - sizeof(skb), sizeof(skb));
        /*  invalidate cache    */
        dma_cache_inv((unsigned long)skb->data, (unsigned int)skb->end - (unsigned int)skb->data);
    }

    return skb;
}

static INLINE struct sk_buff* alloc_skb_tx(unsigned int size)
{
    struct sk_buff *skb;

    /*  allocate memory including header and padding    */
    size += TX_INBAND_HEADER_LENGTH + MAX_TX_PACKET_ALIGN_BYTES + MAX_TX_PACKET_PADDING_BYTES;
    size &= ~(DATA_BUFFER_ALIGNMENT - 1);
    skb = dev_alloc_skb(size + DATA_BUFFER_ALIGNMENT);
    /*  must be burst length alignment  */
    if ( skb != NULL )
        skb_reserve(skb, (~((unsigned int)skb->data + (DATA_BUFFER_ALIGNMENT - 1)) & (DATA_BUFFER_ALIGNMENT - 1)) + TX_INBAND_HEADER_LENGTH);
    return skb;
}

struct sk_buff* atm_alloc_tx(struct atm_vcc *vcc, unsigned int size)
{
    int conn;
    struct sk_buff *skb;

    /*  oversize packet */
    if ( size > aal5s_max_packet_size ) {
        err("atm_alloc_tx: oversize packet");
        return NULL;
    }
    /*  send buffer overflow    */
    if ( atomic_read(&sk_atm(vcc)->sk_wmem_alloc) && !atm_may_send(vcc, size) ) {
        err("atm_alloc_tx: send buffer overflow");
        return NULL;
    }
    conn = find_vcc(vcc);
    if ( conn < 0 ) {
        err("atm_alloc_tx: unknown VCC");
        return NULL;
    }

    skb = dev_alloc_skb(size);
    if ( skb == NULL ) {
        err("atm_alloc_tx: sk buffer is used up");
        return NULL;
    }

    atomic_add(skb->truesize, &sk_atm(vcc)->sk_wmem_alloc);

    return skb;
}

static INLINE void atm_free_tx_skb_vcc(struct sk_buff *skb, struct atm_vcc *vcc)
{
    if ( vcc->pop != NULL )
        vcc->pop(vcc, skb);
    else
        dev_kfree_skb_any(skb);
}

static INLINE struct sk_buff *get_skb_rx_pointer(unsigned int dataptr)
{
    unsigned int skb_dataptr;
    struct sk_buff *skb;

    skb_dataptr = ((dataptr - 1) << 2) | KSEG1;
    skb = *(struct sk_buff **)skb_dataptr;

    ASSERT((unsigned int)skb >= KSEG0, "invalid skb - skb = %#08x, dataptr = %#08x", (unsigned int)skb, dataptr);
    ASSERT(((unsigned int)skb->data | KSEG1) == ((dataptr << 2) | KSEG1), "invalid skb - skb = %#08x, skb->data = %#08x, dataptr = %#08x", (unsigned int)skb, (unsigned int)skb->data, dataptr);

    return skb;
}

static INLINE int get_tx_desc(unsigned int conn)
{
    int desc_base = -1;
    struct connection *p_conn = &g_atm_priv_data.conn[conn];

    if ( p_conn->tx_desc[p_conn->tx_desc_pos].own == 0 ) {
        desc_base = p_conn->tx_desc_pos;
        if ( ++(p_conn->tx_desc_pos) == dma_tx_descriptor_length )
            p_conn->tx_desc_pos = 0;
    }

    return desc_base;
}

static INLINE void mailbox_oam_rx_handler(void)
{
    unsigned int vlddes = WRX_DMA_CHANNEL_CONFIG(RX_DMA_CH_OAM)->vlddes;
    struct rx_descriptor reg_desc;
    struct uni_cell_header *header;
    int conn;
    struct atm_vcc *vcc;
    unsigned int i;

    for ( i = 0; i < vlddes; i++ ) {
        do {
            reg_desc = g_atm_priv_data.oam_desc[g_atm_priv_data.oam_desc_pos];
        } while ( reg_desc.own || !reg_desc.c );    //  keep test OWN and C bit until data is ready

        header = (struct uni_cell_header *)&g_atm_priv_data.oam_buf[g_atm_priv_data.oam_desc_pos * RX_DMA_CH_OAM_BUF_SIZE];

        if ( header->pti == ATM_PTI_SEGF5 || header->pti == ATM_PTI_E2EF5 )
            conn = find_vpivci(header->vpi, header->vci);
        else if ( header->vci == 0x03 || header->vci == 0x04 )
            conn = find_vpi(header->vpi);
        else
            conn = -1;

        if ( conn >= 0 && g_atm_priv_data.conn[conn].vcc != NULL ) {
            vcc = g_atm_priv_data.conn[conn].vcc;

            if ( vcc->push_oam != NULL )
                vcc->push_oam(vcc, header);
            else
                ifx_push_oam((unsigned char *)header);
            adsl_led_flash();
        }

        reg_desc.byteoff = 0;
        reg_desc.datalen = RX_DMA_CH_OAM_BUF_SIZE;
        reg_desc.own = 1;
        reg_desc.c   = 0;

        g_atm_priv_data.oam_desc[g_atm_priv_data.oam_desc_pos] = reg_desc;
        if ( ++g_atm_priv_data.oam_desc_pos == RX_DMA_CH_OAM_DESC_LEN )
            g_atm_priv_data.oam_desc_pos = 0;

        mailbox_signal(RX_DMA_CH_OAM, 0);
    }
}

static INLINE void mailbox_aal_rx_handler(void)
{
    unsigned int vlddes = WRX_DMA_CHANNEL_CONFIG(RX_DMA_CH_AAL)->vlddes;
    struct rx_descriptor reg_desc;
    int conn;
    struct atm_vcc *vcc;
    struct sk_buff *skb, *new_skb;
    struct rx_inband_trailer *trailer;
    unsigned int i;

    for ( i = 0; i < vlddes; i++ ) {
        do {
            reg_desc = g_atm_priv_data.aal_desc[g_atm_priv_data.aal_desc_pos];
        } while ( reg_desc.own || !reg_desc.c );    //  keep test OWN and C bit until data is ready

        conn = reg_desc.id;

        if ( g_atm_priv_data.conn[conn].vcc != NULL ) {
            vcc = g_atm_priv_data.conn[conn].vcc;

            skb = get_skb_rx_pointer(reg_desc.dataptr);

            if ( reg_desc.err ) {
                if ( vcc->qos.aal == ATM_AAL5 ) {
                    trailer = (struct rx_inband_trailer *)((unsigned int)skb->data + ((reg_desc.byteoff + reg_desc.datalen + MAX_RX_PACKET_PADDING_BYTES) & ~MAX_RX_PACKET_PADDING_BYTES));
                    if ( trailer->stw_crc )
                        g_atm_priv_data.conn[conn].aal5_vcc_crc_err++;
                    if ( trailer->stw_ovz )
                        g_atm_priv_data.conn[conn].aal5_vcc_oversize_sdu++;
                    g_atm_priv_data.wrx_drop_pdu++;
                }
                if ( vcc->stats ) {
                    atomic_inc(&vcc->stats->rx_drop);
                    atomic_inc(&vcc->stats->rx_err);
                }
            }
            else if ( atm_charge(vcc, skb->truesize) ) {
                new_skb = alloc_skb_rx();
                if ( new_skb != NULL ) {
                    skb_reserve(skb, reg_desc.byteoff);
                    skb_put(skb, reg_desc.datalen);
                    ATM_SKB(skb)->vcc = vcc;

                    dump_skb(skb, DUMP_SKB_LEN, (char *)__func__, 0, conn, 0);

                    vcc->push(vcc, skb);

                    if ( vcc->qos.aal == ATM_AAL5 )
                        g_atm_priv_data.wrx_pdu++;
                    if ( vcc->stats )
                        atomic_inc(&vcc->stats->rx);
                    adsl_led_flash();

                    reg_desc.dataptr = (unsigned int)new_skb->data >> 2;
                }
                else {
                    atm_return(vcc, skb->truesize);
                    if ( vcc->qos.aal == ATM_AAL5 )
                        g_atm_priv_data.wrx_drop_pdu++;
                    if ( vcc->stats )
                        atomic_inc(&vcc->stats->rx_drop);
                }
            }
            else {
                if ( vcc->qos.aal == ATM_AAL5 )
                    g_atm_priv_data.wrx_drop_pdu++;
                if ( vcc->stats )
                    atomic_inc(&vcc->stats->rx_drop);
            }
        }
        else {
            g_atm_priv_data.wrx_drop_pdu++;
        }

        reg_desc.byteoff = 0;
        reg_desc.datalen = RX_DMA_CH_AAL_BUF_SIZE;
        reg_desc.own = 1;
        reg_desc.c   = 0;

        g_atm_priv_data.aal_desc[g_atm_priv_data.aal_desc_pos] = reg_desc;
        if ( ++g_atm_priv_data.aal_desc_pos == dma_rx_descriptor_length )
            g_atm_priv_data.aal_desc_pos = 0;

        mailbox_signal(RX_DMA_CH_AAL, 0);
    }
}

#if defined(ENABLE_TASKLET) && ENABLE_TASKLET
static void do_ppe_tasklet(unsigned long arg)
{
    *MBOX_IGU1_ISRC = *MBOX_IGU1_ISR;
    mailbox_oam_rx_handler();
    mailbox_aal_rx_handler();
    if ( (*MBOX_IGU1_ISR & ((1 << RX_DMA_CH_AAL) | (1 << RX_DMA_CH_OAM))) != 0 )
        tasklet_schedule(&g_dma_tasklet);
    else
        enable_irq(PPE_MAILBOX_IGU1_INT);
}
#endif

static irqreturn_t mailbox_irq_handler(int irq, void *dev_id)
{
    if ( !*MBOX_IGU1_ISR )
        return IRQ_HANDLED;

#if defined(ENABLE_TASKLET) && ENABLE_TASKLET
    disable_irq(PPE_MAILBOX_IGU1_INT);
    tasklet_schedule(&g_dma_tasklet);
#else
    *MBOX_IGU1_ISRC = *MBOX_IGU1_ISR;
    mailbox_oam_rx_handler();
    mailbox_aal_rx_handler();
#endif

    return IRQ_HANDLED;
}

static INLINE void mailbox_signal(unsigned int queue, int is_tx)
{
    int count = 1000;

    if ( is_tx ) {
        while ( MBOX_IGU3_ISR_ISR(queue + FIRST_QSB_QID + 16) && count)
		count--;
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(queue + FIRST_QSB_QID + 16);
    }
    else {
        while ( MBOX_IGU3_ISR_ISR(queue) && count)
		count--;
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(queue);
    }
    ASSERT(count != 0, "MBOX_IGU3_ISR = 0x%08x", ltq_r32(MBOX_IGU3_ISR));
}

static void set_qsb(struct atm_vcc *vcc, struct atm_qos *qos, unsigned int queue)
{
    struct clk *clk = clk_get(0, "fpi");
	unsigned int qsb_clk = clk_get_rate(clk);
    unsigned int qsb_qid = queue + FIRST_QSB_QID;
    union qsb_queue_parameter_table qsb_queue_parameter_table = {{0}};
    union qsb_queue_vbr_parameter_table qsb_queue_vbr_parameter_table = {{0}};
    unsigned int tmp;

#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) ) {
        static char *str_traffic_class[9] = {
            "ATM_NONE",
            "ATM_UBR",
            "ATM_CBR",
            "ATM_VBR",
            "ATM_ABR",
            "ATM_ANYCLASS",
            "ATM_VBR_RT",
            "ATM_UBR_PLUS",
            "ATM_MAX_PCR"
        };
        printk(KERN_INFO  "QoS Parameters:\n");
        printk(KERN_INFO  "\tAAL          : %d\n", qos->aal);
        printk(KERN_INFO  "\tTX Traffic Class: %s\n", str_traffic_class[qos->txtp.traffic_class]);
        printk(KERN_INFO  "\tTX Max PCR      : %d\n", qos->txtp.max_pcr);
        printk(KERN_INFO  "\tTX Min PCR      : %d\n", qos->txtp.min_pcr);
        printk(KERN_INFO  "\tTX PCR          : %d\n", qos->txtp.pcr);
        printk(KERN_INFO  "\tTX Max CDV      : %d\n", qos->txtp.max_cdv);
        printk(KERN_INFO  "\tTX Max SDU      : %d\n", qos->txtp.max_sdu);
        printk(KERN_INFO  "\tTX SCR          : %d\n", qos->txtp.scr);
        printk(KERN_INFO  "\tTX MBS          : %d\n", qos->txtp.mbs);
        printk(KERN_INFO  "\tTX CDV          : %d\n", qos->txtp.cdv);
        printk(KERN_INFO  "\tRX Traffic Class: %s\n", str_traffic_class[qos->rxtp.traffic_class]);
        printk(KERN_INFO  "\tRX Max PCR      : %d\n", qos->rxtp.max_pcr);
        printk(KERN_INFO  "\tRX Min PCR      : %d\n", qos->rxtp.min_pcr);
        printk(KERN_INFO  "\tRX PCR          : %d\n", qos->rxtp.pcr);
        printk(KERN_INFO  "\tRX Max CDV      : %d\n", qos->rxtp.max_cdv);
        printk(KERN_INFO  "\tRX Max SDU      : %d\n", qos->rxtp.max_sdu);
        printk(KERN_INFO  "\tRX SCR          : %d\n", qos->rxtp.scr);
        printk(KERN_INFO  "\tRX MBS          : %d\n", qos->rxtp.mbs);
        printk(KERN_INFO  "\tRX CDV          : %d\n", qos->rxtp.cdv);
    }
#endif  //  defined(DEBUG_QOS) && DEBUG_QOS

    /*
     *  Peak Cell Rate (PCR) Limiter
     */
    if ( qos->txtp.max_pcr == 0 )
        qsb_queue_parameter_table.bit.tp = 0;   /*  disable PCR limiter */
    else {
        /*  peak cell rate would be slightly lower than requested [maximum_rate / pcr = (qsb_clock / 8) * (time_step / 4) / pcr] */
        tmp = ((qsb_clk * qsb_tstep) >> 5) / qos->txtp.max_pcr + 1;
        /*  check if overflow takes place   */
        qsb_queue_parameter_table.bit.tp = tmp > QSB_TP_TS_MAX ? QSB_TP_TS_MAX : tmp;
    }

    //  A funny issue. Create two PVCs, one UBR and one UBR with max_pcr.
    //  Send packets to these two PVCs at same time, it trigger strange behavior.
    //  In A1, RAM from 0x80000000 to 0x0x8007FFFF was corrupted with fixed pattern 0x00000000 0x40000000.
    //  In A4, PPE firmware keep emiting unknown cell and do not respond to driver.
    //  To work around, create UBR always with max_pcr.
    //  If user want to create UBR without max_pcr, we give a default one larger than line-rate.
    if ( qos->txtp.traffic_class == ATM_UBR && qsb_queue_parameter_table.bit.tp == 0 ) {
        int port = g_atm_priv_data.conn[queue].port;
        unsigned int max_pcr = g_atm_priv_data.port[port].tx_max_cell_rate + 1000;

        tmp = ((qsb_clk * qsb_tstep) >> 5) / max_pcr + 1;
        if ( tmp > QSB_TP_TS_MAX )
            tmp = QSB_TP_TS_MAX;
        else if ( tmp < 1 )
            tmp = 1;
        qsb_queue_parameter_table.bit.tp = tmp;
    }

    /*
     *  Weighted Fair Queueing Factor (WFQF)
     */
    switch ( qos->txtp.traffic_class ) {
    case ATM_CBR:
    case ATM_VBR_RT:
        /*  real time queue gets weighted fair queueing bypass  */
        qsb_queue_parameter_table.bit.wfqf = 0;
        break;
    case ATM_VBR_NRT:
    case ATM_UBR_PLUS:
        /*  WFQF calculation here is based on virtual cell rates, to reduce granularity for high rates  */
        /*  WFQF is maximum cell rate / garenteed cell rate                                             */
        /*  wfqf = qsb_minimum_cell_rate * QSB_WFQ_NONUBR_MAX / requested_minimum_peak_cell_rate        */
        if ( qos->txtp.min_pcr == 0 )
            qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_NONUBR_MAX;
        else
        {
            tmp = QSB_GCR_MIN * QSB_WFQ_NONUBR_MAX / qos->txtp.min_pcr;
            if ( tmp == 0 )
                qsb_queue_parameter_table.bit.wfqf = 1;
            else if ( tmp > QSB_WFQ_NONUBR_MAX )
                qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_NONUBR_MAX;
            else
                qsb_queue_parameter_table.bit.wfqf = tmp;
        }
        break;
    default:
    case ATM_UBR:
        qsb_queue_parameter_table.bit.wfqf = QSB_WFQ_UBR_BYPASS;
    }

    /*
     *  Sustained Cell Rate (SCR) Leaky Bucket Shaper VBR.0/VBR.1
     */
    if ( qos->txtp.traffic_class == ATM_VBR_RT || qos->txtp.traffic_class == ATM_VBR_NRT ) {
        if ( qos->txtp.scr == 0 ) {
            /*  disable shaper  */
            qsb_queue_vbr_parameter_table.bit.taus = 0;
            qsb_queue_vbr_parameter_table.bit.ts = 0;
        }
        else {
            /*  Cell Loss Priority  (CLP)   */
            if ( (vcc->atm_options & ATM_ATMOPT_CLP) )
                /*  CLP1    */
                qsb_queue_parameter_table.bit.vbr = 1;
            else
                /*  CLP0    */
                qsb_queue_parameter_table.bit.vbr = 0;
            /*  Rate Shaper Parameter (TS) and Burst Tolerance Parameter for SCR (tauS) */
            tmp = ((qsb_clk * qsb_tstep) >> 5) / qos->txtp.scr + 1;
            qsb_queue_vbr_parameter_table.bit.ts = tmp > QSB_TP_TS_MAX ? QSB_TP_TS_MAX : tmp;
            tmp = (qos->txtp.mbs - 1) * (qsb_queue_vbr_parameter_table.bit.ts - qsb_queue_parameter_table.bit.tp) / 64;
            if ( tmp == 0 )
                qsb_queue_vbr_parameter_table.bit.taus = 1;
            else if ( tmp > QSB_TAUS_MAX )
                qsb_queue_vbr_parameter_table.bit.taus = QSB_TAUS_MAX;
            else
                qsb_queue_vbr_parameter_table.bit.taus = tmp;
        }
    }
    else {
        qsb_queue_vbr_parameter_table.bit.taus = 0;
        qsb_queue_vbr_parameter_table.bit.ts = 0;
    }

    /*  Queue Parameter Table (QPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_QPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(qsb_qid);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
        printk("QPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (unsigned int)QSB_RTM, *QSB_RTM, (unsigned int)QSB_RTD, *QSB_RTD, (unsigned int)QSB_RAMAC, *QSB_RAMAC);
#endif
    /*  Queue VBR Paramter Table (QVPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QVPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_vbr_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_VBR) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(qsb_qid);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
        printk("QVPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (unsigned int)QSB_RTM, *QSB_RTM, (unsigned int)QSB_RTD, *QSB_RTD, (unsigned int)QSB_RAMAC, *QSB_RAMAC);
#endif

#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) ) {
        printk("set_qsb\n");
        printk("  qsb_clk = %lu\n", (unsigned long)qsb_clk);
        printk("  qsb_queue_parameter_table.bit.tp       = %d\n", (int)qsb_queue_parameter_table.bit.tp);
        printk("  qsb_queue_parameter_table.bit.wfqf     = %d (0x%08X)\n", (int)qsb_queue_parameter_table.bit.wfqf, (int)qsb_queue_parameter_table.bit.wfqf);
        printk("  qsb_queue_parameter_table.bit.vbr      = %d\n", (int)qsb_queue_parameter_table.bit.vbr);
        printk("  qsb_queue_parameter_table.dword        = 0x%08X\n", (int)qsb_queue_parameter_table.dword);
        printk("  qsb_queue_vbr_parameter_table.bit.ts   = %d\n", (int)qsb_queue_vbr_parameter_table.bit.ts);
        printk("  qsb_queue_vbr_parameter_table.bit.taus = %d\n", (int)qsb_queue_vbr_parameter_table.bit.taus);
        printk("  qsb_queue_vbr_parameter_table.dword    = 0x%08X\n", (int)qsb_queue_vbr_parameter_table.dword);
    }
#endif
}

static void qsb_global_set(void)
{
    struct clk *clk = clk_get(0, "fpi");
	unsigned int qsb_clk = clk_get_rate(clk);
    int i;
    unsigned int tmp1, tmp2, tmp3;

    *QSB_ICDV = QSB_ICDV_TAU_SET(qsb_tau);
    *QSB_SBL  = QSB_SBL_SBL_SET(qsb_srvm);
    *QSB_CFG  = QSB_CFG_TSTEPC_SET(qsb_tstep >> 1);
#if defined(DEBUG_QOS) && DEBUG_QOS
    if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) ) {
        printk("qsb_clk = %u\n", qsb_clk);
        printk("QSB_ICDV (%08X) = %d (%d), QSB_SBL (%08X) = %d (%d), QSB_CFG (%08X) = %d (%d)\n", (unsigned int)QSB_ICDV, *QSB_ICDV, QSB_ICDV_TAU_SET(qsb_tau), (unsigned int)QSB_SBL, *QSB_SBL, QSB_SBL_SBL_SET(qsb_srvm), (unsigned int)QSB_CFG, *QSB_CFG, QSB_CFG_TSTEPC_SET(qsb_tstep >> 1));
    }
#endif

    /*
     *  set SCT and SPT per port
     */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ ) {
        if ( g_atm_priv_data.port[i].tx_max_cell_rate != 0 ) {
            tmp1 = ((qsb_clk * qsb_tstep) >> 1) / g_atm_priv_data.port[i].tx_max_cell_rate;
            tmp2 = tmp1 >> 6;                   /*  integer value of Tsb    */
            tmp3 = (tmp1 & ((1 << 6) - 1)) + 1; /*  fractional part of Tsb  */
            /*  carry over to integer part (?)  */
            if ( tmp3 == (1 << 6) )
            {
                tmp3 = 0;
                tmp2++;
            }
            if ( tmp2 == 0 )
                tmp2 = tmp3 = 1;
            /*  1. set mask                                 */
            /*  2. write value to data transfer register    */
            /*  3. start the tranfer                        */
            /*  SCT (FracRate)  */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SCT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(tmp3);
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SCT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
#if defined(DEBUG_QOS) && DEBUG_QOS
            if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
                printk("SCT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (unsigned int)QSB_RTM, *QSB_RTM, (unsigned int)QSB_RTD, *QSB_RTD, (unsigned int)QSB_RAMAC, *QSB_RAMAC);
#endif
            /*  SPT (SBV + PN + IntRage)    */
            *QSB_RTM   = QSB_RTM_DM_SET(QSB_SET_SPT_MASK);
            *QSB_RTD   = QSB_RTD_TTV_SET(QSB_SPT_SBV_VALID | QSB_SPT_PN_SET(i & 0x01) | QSB_SPT_INTRATE_SET(tmp2));
            *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_SPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(i & 0x01);
#if defined(DEBUG_QOS) && DEBUG_QOS
            if ( (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS) )
                printk("SPT: QSB_RTM (%08X) = 0x%08X, QSB_RTD (%08X) = 0x%08X, QSB_RAMAC (%08X) = 0x%08X\n", (unsigned int)QSB_RTM, *QSB_RTM, (unsigned int)QSB_RTD, *QSB_RTD, (unsigned int)QSB_RAMAC, *QSB_RAMAC);
#endif
        }
    }
}

static INLINE void set_htu_entry(unsigned int vpi, unsigned int vci, unsigned int queue, int aal5, int is_retx)
{
    struct htu_entry htu_entry = {  res1:       0x00,
                                    clp:        is_retx ? 0x01 : 0x00,
                                    pid:        g_atm_priv_data.conn[queue].port & 0x01,
                                    vpi:        vpi,
                                    vci:        vci,
                                    pti:        0x00,
                                    vld:        0x01};

    struct htu_mask htu_mask = {    set:        0x01,
#if !defined(ENABLE_ATM_RETX) || !ENABLE_ATM_RETX
                                    clp:        0x01,
                                    pid_mask:   0x02,
#else
                                    clp:        g_retx_htu ? 0x00 : 0x01,
                                    pid_mask:   RETX_MODE_CFG->retx_en ? 0x03 : 0x02,
#endif
                                    vpi_mask:   0x00,
#if !defined(ENABLE_ATM_RETX) || !ENABLE_ATM_RETX
                                    vci_mask:   0x0000,
#else
                                    vci_mask:   RETX_MODE_CFG->retx_en ? 0xFF00 : 0x0000,
#endif
                                    pti_mask:   0x03,   //  0xx, user data
                                    clear:      0x00};

    struct htu_result htu_result = {res1:       0x00,
                                    cellid:     queue,
                                    res2:       0x00,
                                    type:       aal5 ? 0x00 : 0x01,
                                    ven:        0x01,
                                    res3:       0x00,
                                    qid:        queue};

    *HTU_RESULT(queue + OAM_HTU_ENTRY_NUMBER) = htu_result;
    *HTU_MASK(queue + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
    *HTU_ENTRY(queue + OAM_HTU_ENTRY_NUMBER)  = htu_entry;
}

static INLINE void clear_htu_entry(unsigned int queue)
{
    HTU_ENTRY(queue + OAM_HTU_ENTRY_NUMBER)->vld = 0;
}

static void validate_oam_htu_entry(void)
{
    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 1;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 1;
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    HTU_ENTRY(OAM_ARQ_HTU_ENTRY)->vld = 1;
#endif
}

static void invalidate_oam_htu_entry(void)
{
    HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)->vld = 0;
    HTU_ENTRY(OAM_F5_HTU_ENTRY)->vld = 0;
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    HTU_ENTRY(OAM_ARQ_HTU_ENTRY)->vld = 0;
#endif
}

static INLINE int find_vpi(unsigned int vpi)
{
    int i;
    unsigned int bit;

    for ( i = 0, bit = 1; i < MAX_PVC_NUMBER; i++, bit <<= 1 ) {
        if ( (g_atm_priv_data.conn_table & bit) != 0
            && g_atm_priv_data.conn[i].vcc != NULL
            && vpi == g_atm_priv_data.conn[i].vcc->vpi )
            return i;
    }

    return -1;
}

static INLINE int find_vpivci(unsigned int vpi, unsigned int vci)
{
    int i;
    unsigned int bit;

    for ( i = 0, bit = 1; i < MAX_PVC_NUMBER; i++, bit <<= 1 ) {
        if ( (g_atm_priv_data.conn_table & bit) != 0
            && g_atm_priv_data.conn[i].vcc != NULL
            && vpi == g_atm_priv_data.conn[i].vcc->vpi
            && vci == g_atm_priv_data.conn[i].vcc->vci )
            return i;
    }

    return -1;
}

static INLINE int find_vcc(struct atm_vcc *vcc)
{
    int i;
    unsigned int bit;

    for ( i = 0, bit = 1; i < MAX_PVC_NUMBER; i++, bit <<= 1 ) {
        if ( (g_atm_priv_data.conn_table & bit) != 0
            && g_atm_priv_data.conn[i].vcc == vcc )
            return i;
    }

    return -1;
}

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX

static void retx_polling_func(unsigned long arg)
{
    int sys_flag;
    volatile struct dsl_param *p_dsl_param;
    int new_retx_htu;
    int retx_en;
    int i, max_htu;

    local_irq_save(sys_flag);
    if ( g_retx_playout_buffer == 0 && g_xdata_addr != NULL && (((volatile struct dsl_param *)g_xdata_addr)->RetxEnable || ((volatile struct dsl_param *)g_xdata_addr)->ServiceSpecificReTx) ) {
        local_irq_restore(sys_flag);
        g_retx_playout_buffer = __get_free_pages(GFP_KERNEL, RETX_PLAYOUT_BUFFER_ORDER);
        if ( g_retx_playout_buffer == 0 )
            panic("no memory for g_retx_playout_buffer\n");
        memset((void *)g_retx_playout_buffer, 0, RETX_PLAYOUT_BUFFER_SIZE);
        dma_cache_inv(g_retx_playout_buffer, RETX_PLAYOUT_BUFFER_SIZE);
    }
    else
        local_irq_restore(sys_flag);


    local_irq_save(sys_flag);
    if ( g_xdata_addr != NULL ) {
        p_dsl_param = (volatile struct dsl_param *)g_xdata_addr;
        g_retx_polling_cnt += RETX_POLLING_INTERVAL;

        if ( p_dsl_param->update_flag ) {
            do_gettimeofday(&g_retx_polling_start);

            g_dsl_param = *p_dsl_param;

            // we always enable retx (just for test purpose)
            //g_dsl_param.RetxEnable = 1;
            //RETX_TSYNC_CFG->fw_alpha = 0;

            if ( g_dsl_param.RetxEnable || g_dsl_param.ServiceSpecificReTx ) {
                //  ReTX enabled
                // MIB counter updated for each polling
                p_dsl_param->RxDtuCorruptedCNT          = *RxDTUCorruptedCNT;
                p_dsl_param->RxRetxDtuUnCorrectedCNT    = *RxRetxDTUUncorrectedCNT;
                p_dsl_param->RxLastEFB                  = *RxLastEFBCNT;
                p_dsl_param->RxDtuCorrectedCNT          = *RxDTUCorrectedCNT;

                // for RETX paramters, we check only once for every second
                if ( g_retx_polling_cnt < HZ )
                    goto _clear_update_flag;

                g_retx_polling_cnt -= HZ;

                if ( g_dsl_param.ServiceSpecificReTx && g_dsl_param.ReTxPVC == 0 )
                    new_retx_htu = 1;
                else
                    new_retx_htu = 0;

                // default fw_alpha equals to default hardware alpha
                RETX_TSYNC_CFG->fw_alpha    = 0;

                RETX_TD_CFG->td_max         = g_dsl_param.MaxDelayrt;
                RETX_TD_CFG->td_min         = g_dsl_param.MinDelayrt;

                *RETX_PLAYOUT_BUFFER_BASE   = ((((unsigned int)g_retx_playout_buffer | KSEG1) + 15) & 0xFFFFFFF0) >> 2;

                if ( g_dsl_param.ServiceSpecificReTx ) {
                    *RETX_SERVICE_HEADER_CFG= g_dsl_param.ReTxPVC << 4;
                    if ( g_dsl_param.ReTxPVC == 0 )
                        *RETX_MASK_HEADER_CFG = 1;
                    else
                        *RETX_MASK_HEADER_CFG = 0;
                }
                else {
                    *RETX_SERVICE_HEADER_CFG= 0;
                    *RETX_MASK_HEADER_CFG   = 0;
                }

                retx_en = 1;
            }
            else {
                //  ReTX disabled

                new_retx_htu = 0;

                RETX_TSYNC_CFG->fw_alpha    = 7;

                *RETX_SERVICE_HEADER_CFG    = 0;
                *RETX_MASK_HEADER_CFG       = 0;

                retx_en = 0;
            }


            if ( retx_en != RETX_MODE_CFG->retx_en ) {
                unsigned int pid_mask, vci_mask;

                if ( retx_en ) {
                    pid_mask = 0x03;
                    vci_mask = 0xFF00;
                }
                else {
                    pid_mask = 0x02;
                    vci_mask = 0x0000;
                }

                max_htu = *CFG_WRX_HTUTS;
                for ( i = OAM_HTU_ENTRY_NUMBER; i < max_htu; i++ )
                    if ( HTU_ENTRY(i)->vld ) {
                        HTU_MASK(i)->pid_mask = pid_mask;
                        HTU_MASK(i)->vci_mask = vci_mask;
                    }
            }

            if ( new_retx_htu != g_retx_htu ) {
                int conn, retx_conn;

                g_retx_htu = new_retx_htu;

                if ( g_retx_htu ) {
                    max_htu = *CFG_WRX_HTUTS;
                    for ( i = OAM_HTU_ENTRY_NUMBER; i < max_htu; i++ )
                        if ( HTU_ENTRY(i)->vld )
                            HTU_MASK(i)->clp = 0;

                    for ( conn = 0; conn < MAX_PVC_NUMBER; conn++ )
                        if ( g_atm_priv_data.conn[conn].vcc && g_atm_priv_data.conn[conn].vcc->qos.aal == ATM_AAL5 && !HTU_ENTRY(conn + OAM_HTU_ENTRY_NUMBER)->clp ) {
                            retx_conn = (conn + 8) % 16;    //  ReTX queue

                            if ( retx_conn < MAX_PVC_NUMBER && test_and_set_bit(retx_conn, &g_atm_priv_data.conn_table) == 0 ) {
                                g_atm_priv_data.conn[retx_conn].vcc = g_atm_priv_data.conn[conn].vcc;
                                set_htu_entry(g_atm_priv_data.conn[conn].vcc->vpi, g_atm_priv_data.conn[conn].vcc->vci, retx_conn, g_atm_priv_data.conn[conn].vcc->qos.aal == ATM_AAL5 ? 1 : 0, 1);
                            }
                            else {
                                err("Queue number %d for ReTX queue of PVC(%d.%d) is not available!", retx_conn, g_atm_priv_data.conn[conn].vcc->vpi, g_atm_priv_data.conn[conn].vcc->vci);
                            }
                        }
                }
                else
                {
                    for ( retx_conn = 0; retx_conn < MAX_PVC_NUMBER; retx_conn++ )
                        if ( g_atm_priv_data.conn[retx_conn].vcc && HTU_ENTRY(retx_conn + OAM_HTU_ENTRY_NUMBER)->clp ) {
                            clear_htu_entry(retx_conn);
                            g_atm_priv_data.conn[retx_conn].vcc = NULL;
                            g_atm_priv_data.conn[retx_conn].aal5_vcc_crc_err = 0;
                            g_atm_priv_data.conn[retx_conn].aal5_vcc_oversize_sdu = 0;
                            clear_bit(retx_conn, &g_atm_priv_data.conn_table);
                        }

                    max_htu = *CFG_WRX_HTUTS;
                    for ( i = OAM_HTU_ENTRY_NUMBER; i < max_htu; i++ )
                        if ( HTU_ENTRY(i)->vld )
                            HTU_MASK(i)->clp = 1;
                }
            }

            RETX_MODE_CFG->retx_en = retx_en;

_clear_update_flag:
            p_dsl_param->update_flag = 0;

            do_gettimeofday(&g_retx_polling_end);
        }

        g_retx_polling_timer.expires = jiffies + RETX_POLLING_INTERVAL;
        add_timer(&g_retx_polling_timer);
    }
    local_irq_restore(sys_flag);
}

static int init_atm_tc_retrans_param(void)
{
    int i = 0;
    struct DTU_stat_info reset_val;

    RETX_MODE_CFG->invld_range  = 128;
    RETX_MODE_CFG->buff_size    = RETX_PLAYOUT_FW_BUFF_SIZE > 4096/32 ? 4096/32 : RETX_PLAYOUT_FW_BUFF_SIZE ;
    RETX_MODE_CFG->retx_en      = 1;

    // default fw_alpha equals to default hardware alpha
    RETX_TSYNC_CFG->fw_alpha    = 7;
    RETX_TSYNC_CFG->sync_inp    = 0;

    RETX_TD_CFG->td_max         = 0;
    RETX_TD_CFG->td_min         = 0;

    // *RETX_PLAYOUT_BUFFER_BASE   = KSEG1ADDR(g_retx_playout_buffer); //  need " >> 2 " ?
    *RETX_PLAYOUT_BUFFER_BASE       = ((((unsigned int)g_retx_playout_buffer | KSEG1) + 15) & 0xFFFFFFF0) >> 2;

    *RETX_SERVICE_HEADER_CFG    = 0;
    *RETX_MASK_HEADER_CFG       = 0;

    // 20us
    RETX_MIB_TIMER_CFG->tick_cycle = 4800;
    RETX_MIB_TIMER_CFG->ticks_per_sec = 50000;

    *LAST_DTU_SID_IN            = 255;
    *RFBI_FIRST_CW              = 1;
    // init DTU_STAT_INFO

    memset(&reset_val, 0, sizeof(reset_val));
    reset_val.dtu_rd_ptr = reset_val.dtu_wr_ptr = 0xffff;

    for(i = 0 ; i < 256; i ++) {
        DTU_STAT_INFO[i] = reset_val;
    }
    return 0;
}

static void clear_atm_tc_retrans_param(void)
{
    if ( g_retx_playout_buffer ) {
        free_pages(g_retx_playout_buffer, RETX_PLAYOUT_BUFFER_ORDER);
        g_retx_playout_buffer = 0;
    }
}

#endif

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
static void dump_skb(struct sk_buff *skb, unsigned int len, char *title, int port, int ch, int is_tx)
{
    int i;

    if ( !(ifx_atm_dbg_enable & (is_tx ? DBG_ENABLE_MASK_DUMP_SKB_TX : DBG_ENABLE_MASK_DUMP_SKB_RX)) )
        return;

    if ( skb->len < len )
        len = skb->len;

    if ( len > RX_DMA_CH_AAL_BUF_SIZE ) {
        printk("too big data length: skb = %08x, skb->data = %08x, skb->len = %d\n", (unsigned int)skb, (unsigned int)skb->data, skb->len);
        return;
    }

    if ( ch >= 0 )
        printk("%s (port %d, ch %d)\n", title, port, ch);
    else
        printk("%s\n", title);
    printk("  skb->data = %08X, skb->tail = %08X, skb->len = %d\n", (unsigned int)skb->data, (unsigned int)skb->tail, (int)skb->len);
    for ( i = 1; i <= len; i++ ) {
        if ( i % 16 == 1 )
            printk("  %4d:", i - 1);
        printk(" %02X", (int)(*((char*)skb->data + i - 1) & 0xFF));
        if ( i % 16 == 0 )
            printk("\n");
    }
    if ( (i - 1) % 16 != 0 )
        printk("\n");
}
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
static void skb_swap(struct sk_buff *skb, unsigned int byteoff)
{
    unsigned int mac_offset = ~0;
    unsigned int ip_offset = ~0;
    unsigned char tmp[8];
    unsigned char *p = NULL;

    skb_pull(skb, byteoff + TX_INBAND_HEADER_LENGTH);

    if ( skb->data[0] == 0xAA && skb->data[1] == 0xAA && skb->data[2] == 0x03 ) {
        //  LLC
        if ( skb->data[3] == 0x00 && skb->data[4] == 0x80 && skb->data[5] == 0xC2 ) {
            //  EoA
            if ( skb->data[22] == 0x08 && skb->data[23] == 0x00 ) {
                //  IPv4
                mac_offset = 10;
                ip_offset = 24;
            }
            else if ( skb->data[31] == 0x21 ) {
                //  PPPoE IPv4
                mac_offset = 10;
                ip_offset = 32;
            }
        }
        else {
            //  IPoA
            if ( skb->data[6] == 0x08 && skb->data[7] == 0x00 ) {
                //  IPv4
                ip_offset = 8;
            }
        }
    }
    else if ( skb->data[0] == 0xFE && skb->data[1] == 0xFE && skb->data[2] == 0x03 ) {
        //  LLC PPPoA
        if ( skb->data[4] == 0x00 && skb->data[5] == 0x21 ) {
            //  IPv4
            ip_offset = 6;
        }
    }
    else {
        //  VC-mux
        if ( skb->data[0] == 0x00 && skb->data[1] == 0x21 ) {
            //  PPPoA IPv4
            ip_offset = 2;
        }
        else if ( skb->data[0] == 0x00 && skb->data[1] == 0x00 ) {
            //  EoA
            if ( skb->data[14] == 0x08 && skb->data[15] ==0x00 ) {
                //  IPv4
                mac_offset = 2;
                ip_offset = 16;
            }
            else if ( skb->data[23] == 0x21 ) {
                //  PPPoE IPv4
                mac_offset = 2;
                ip_offset = 26;
            }
        }
        else {
            //  IPoA
            ip_offset = 0;
        }
    }

    if ( mac_offset != ~0 && !(skb->data[mac_offset] & 0x01) ) {
        p = skb->data + mac_offset;
        //  swap MAC
        memcpy(tmp, p, 6);
        memcpy(p, p + 6, 6);
        memcpy(p + 6, tmp, 6);
        p += 12;
    }

    if ( ip_offset != ~0 ) {
        p = skb->data + ip_offset + 12;
        //  swap IP
        memcpy(tmp, p, 4);
        memcpy(p, p + 4, 4);
        memcpy(p + 4, tmp, 4);
        p += 8;
    }

    if ( p != NULL ) {
        dma_cache_wback((unsigned long)skb->data, (unsigned long)p - (unsigned long)skb->data);
    }

    skb_push(skb, byteoff + TX_INBAND_HEADER_LENGTH);
}
#endif

static INLINE void proc_file_create(void)
{
    struct proc_dir_entry *res;

    g_atm_dir = proc_mkdir("driver/ifx_atm", NULL);

    create_proc_read_entry("version",
                           0,
                           g_atm_dir,
                           proc_read_version,
                           NULL);

    res = create_proc_entry("mib",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
       res->read_proc  = proc_read_mib;
       res->write_proc = proc_write_mib;
    }

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    res = create_proc_entry("retx_mib",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
       res->read_proc  = proc_read_retx_mib;
       res->write_proc = proc_write_retx_mib;
    }
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    res = create_proc_entry("dbg",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
       res->read_proc  = proc_read_dbg;
       res->write_proc = proc_write_dbg;
    }

    res = create_proc_entry("mem",
                            0,
                            g_atm_dir);
    if ( res != NULL )
       res->write_proc = proc_write_mem;

 #if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE)
    res = create_proc_entry("pp32",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
       res->read_proc  = proc_read_pp32;
       res->write_proc = proc_write_pp32;
    }
 #endif
#endif

#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC
    create_proc_read_entry("htu",
                            0,
                            g_atm_dir,
                            proc_read_htu,
                            NULL);

    create_proc_read_entry("txq",
                           0,
                           g_atm_dir,
                           proc_read_txq,
                           NULL);

 #if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    create_proc_read_entry("retx_fw",
                           0,
                           g_atm_dir,
                           proc_read_retx_fw,
                           NULL);

    res = create_proc_entry("retx_stats",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
        res->read_proc  = proc_read_retx_stats;
        res->write_proc = proc_write_retx_stats;
    }

    res = create_proc_entry("retx_cfg",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
        res->read_proc  = proc_read_retx_cfg;
        res->write_proc = proc_write_retx_cfg;
    }

    create_proc_read_entry("retx_dsl_param",
                            0,
                            g_atm_dir,
                            proc_read_retx_dsl_param,
                            NULL);
 #endif
#endif
}

static INLINE void proc_file_delete(void)
{
#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC
 #if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    remove_proc_entry("retx_dsl_param", g_atm_dir);

    remove_proc_entry("retx_cfg", g_atm_dir);

    remove_proc_entry("retx_stats", g_atm_dir);

    remove_proc_entry("retx_fw", g_atm_dir);
 #endif

    remove_proc_entry("txq", g_atm_dir);

    remove_proc_entry("htu", g_atm_dir);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
 #if defined(CONFIG_AR9) || defined(CONFIG_VR9) || defined(CONFIG_DANUBE) || defined(CONFIG_AMAZON_SE)
    remove_proc_entry("pp32", g_atm_dir);
 #endif

    remove_proc_entry("mem", g_atm_dir);

    remove_proc_entry("dbg", g_atm_dir);
#endif

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    remove_proc_entry("retx_mib", g_atm_dir);
#endif

    remove_proc_entry("mib", g_atm_dir);

    remove_proc_entry("version", g_atm_dir);

    remove_proc_entry("driver/ifx_atm", NULL);
}

static int proc_read_version(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len = 0;

    len += ifx_atm_version(buf + len);

    if ( offset >= len ) {
        *start = buf;
        *eof = 1;
        return 0;
    }
    *start = buf + offset;
    if ( (len -= offset) > count )
        return count;
    *eof = 1;
    return len;
}

static int proc_read_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len,    "Firmware\n");
    len += sprintf(page + off + len,    "  wrx_drophtu_cell = %u\n", WAN_MIB_TABLE->wrx_drophtu_cell);
    len += sprintf(page + off + len,    "  wrx_dropdes_pdu  = %u\n", WAN_MIB_TABLE->wrx_dropdes_pdu);
    len += sprintf(page + off + len,    "  wrx_correct_pdu  = %u\n", WAN_MIB_TABLE->wrx_correct_pdu);
    len += sprintf(page + off + len,    "  wrx_err_pdu      = %u\n", WAN_MIB_TABLE->wrx_err_pdu);
    len += sprintf(page + off + len,    "  wrx_dropdes_cell = %u\n", WAN_MIB_TABLE->wrx_dropdes_cell);
    len += sprintf(page + off + len,    "  wrx_correct_cell = %u\n", WAN_MIB_TABLE->wrx_correct_cell);
    len += sprintf(page + off + len,    "  wrx_err_cell     = %u\n", WAN_MIB_TABLE->wrx_err_cell);
    len += sprintf(page + off + len,    "  wrx_total_byte   = %u\n", WAN_MIB_TABLE->wrx_total_byte);
    len += sprintf(page + off + len,    "  wtx_total_pdu    = %u\n", WAN_MIB_TABLE->wtx_total_pdu);
    len += sprintf(page + off + len,    "  wtx_total_cell   = %u\n", WAN_MIB_TABLE->wtx_total_cell);
    len += sprintf(page + off + len,    "  wtx_total_byte   = %u\n", WAN_MIB_TABLE->wtx_total_byte);
    len += sprintf(page + off + len,    "Driver\n");
    len += sprintf(page + off + len,    "  wrx_pdu      = %u\n", g_atm_priv_data.wrx_pdu);
    len += sprintf(page + off + len,    "  wrx_drop_pdu = %u\n", g_atm_priv_data.wrx_drop_pdu);
    len += sprintf(page + off + len,    "  wtx_pdu      = %u\n", g_atm_priv_data.wtx_pdu);
    len += sprintf(page + off + len,    "  wtx_err_pdu  = %u\n", g_atm_priv_data.wtx_err_pdu);
    len += sprintf(page + off + len,    "  wtx_drop_pdu = %u\n", g_atm_priv_data.wtx_drop_pdu);

    *eof = 1;

    return len;
}

static int proc_write_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[1024];
    char *p;
    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    if ( stricmp(p, "clear") == 0 || stricmp(p, "clear all") == 0
        || stricmp(p, "clean") == 0 || stricmp(p, "clean all") == 0 ) {
        memset(WAN_MIB_TABLE, 0, sizeof(*WAN_MIB_TABLE));
        g_atm_priv_data.wrx_pdu      = 0;
        g_atm_priv_data.wrx_drop_pdu = 0;
        g_atm_priv_data.wtx_pdu      = 0;
        g_atm_priv_data.wtx_err_pdu  = 0;
        g_atm_priv_data.wtx_drop_pdu = 0;
    }

    return count;
}

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX

static int proc_read_retx_mib(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    printk("Retx FW DTU MIB :\n");
    printk("  rx_total_dtu            = %u\n", *URETX_RX_TOTAL_DTU);
    printk("  rx_bad_dtu              = %u\n", *URETX_RX_BAD_DTU);
    printk("  rx_good_dtu             = %u\n", *URETX_RX_GOOD_DTU);
    printk("  rx_corrected_dtu        = %u\n", *URETX_RX_CORRECTED_DTU);
    printk("  rx_outofdate_dtu        = %u\n", *URETX_RX_OUTOFDATE_DTU);
    printk("  rx_duplicate_dtu        = %u\n", *URETX_RX_DUPLICATE_DTU);
    printk("  rx_timeout_dtu          = %u\n", *URETX_RX_TIMEOUT_DTU);
    printk("  RxDTURetransmittedCNT   = %u\n", *RxDTURetransmittedCNT);
    printk("\n");

    printk("Retx Standard DTU MIB:\n");
    printk("  RxLastEFB               = %u\n", *RxLastEFBCNT);
    printk("  RxDTUCorrectedCNT       = %u\n", *RxDTUCorrectedCNT);
    printk("  RxDTUCorruptedCNT       = %u\n", *RxDTUCorruptedCNT);
    printk("  RxRetxDTUUncorrectedCNT = %u\n", *RxRetxDTUUncorrectedCNT);
    printk("\n");

    printk("Retx FW Cell MIB :\n");
    printk("  bc0_total_cell          = %u\n", *WRX_BC0_CELL_NUM);
    printk("  bc0_drop_cell           = %u\n", *WRX_BC0_DROP_CELL_NUM);
    printk("  bc0_nonretx_cell        = %u\n", *WRX_BC0_NONRETX_CELL_NUM);
    printk("  bc0_retx_cell           = %u\n", *WRX_BC0_RETX_CELL_NUM);
    printk("  bc0_outofdate_cell      = %u\n", *WRX_BC0_OUTOFDATE_CELL_NUM);
    printk("  bc0_directup_cell       = %u\n", *WRX_BC0_DIRECTUP_NUM);
    printk("  bc0_to_pb_total_cell    = %u\n", *WRX_BC0_PBW_TOTAL_NUM);
    printk("  bc0_to_pb_succ_cell     = %u\n", *WRX_BC0_PBW_SUCC_NUM);
    printk("  bc0_to_pb_fail_cell     = %u\n", *WRX_BC0_PBW_FAIL_NUM);
    printk("  bc1_total_cell          = %u\n", *WRX_BC1_CELL_NUM);

    printk("\n");

    printk("ATM Rx AAL5/OAM MIB:\n");
    printk("  wrx_drophtu_cell  = %u\n", WAN_MIB_TABLE->wrx_drophtu_cell);
    printk("  wrx_dropdes_pdu   = %u\n", WAN_MIB_TABLE->wrx_dropdes_pdu);

    printk("  wrx_correct_pdu   = %-10u  ", WAN_MIB_TABLE->wrx_correct_pdu);
    if ( WAN_MIB_TABLE->wrx_correct_pdu == 0 )
        printk("\n");
    else {
        int i = 0;

        printk("[ ");
        for ( i = 0; i < 16; ++i ) {
            if ( WRX_PER_PVC_CORRECT_PDU_BASE[i] )
                printk("q%-2d = %-10u , ", i, WRX_PER_PVC_CORRECT_PDU_BASE[i]);
        }
        printk("]\n");
    }

    printk("  wrx_err_pdu       = %-10u  ", WAN_MIB_TABLE->wrx_err_pdu);
    if ( WAN_MIB_TABLE->wrx_err_pdu == 0 )
        printk("\n");
    else {
        int i = 0;

        printk("[ ");
        for ( i = 0; i < 16; ++i ) {
            if ( WRX_PER_PVC_ERROR_PDU_BASE[i] )
                printk("q%-2d = %-10u , ", i,  WRX_PER_PVC_ERROR_PDU_BASE[i] );
        }
        printk("]\n");
    }

    printk("  wrx_dropdes_cell  = %u\n", WAN_MIB_TABLE->wrx_dropdes_cell);
    printk("  wrx_correct_cell  = %u\n", WAN_MIB_TABLE->wrx_correct_cell);
    printk("  wrx_err_cell      = %u\n", WAN_MIB_TABLE->wrx_err_cell);
    printk("  wrx_total_byte    = %u\n", WAN_MIB_TABLE->wrx_total_byte);
    printk("\n");

    printk("ATM Tx MIB:\n");
    printk("  wtx_total_pdu     = %u\n", WAN_MIB_TABLE->wtx_total_pdu);
    printk("  wtx_total_cell    = %u\n", WAN_MIB_TABLE->wtx_total_cell);
    printk("  wtx_total_byte    = %u\n", WAN_MIB_TABLE->wtx_total_byte);
    printk("\n");

    printk("Debugging Info:\n");
    printk("  Firmware version                 = %d.%d.%d.%d.%d.%d\n",
            (int)FW_VER_ID->family, (int)FW_VER_ID->fwtype, (int)FW_VER_ID->interface,
            (int)FW_VER_ID->fwmode, (int)FW_VER_ID->major, (int)FW_VER_ID->minor);

    printk("  retx_alpha_switch_to_hunt_times  = %u\n", *URETX_ALPHA_SWITCH_TO_HUNT_TIMES);

    printk("\n");

    *eof = 1;

    return len;
}

static int proc_write_retx_mib(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;
    int len, rlen;
    int i;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    if ( stricmp(p, "clean") == 0 || stricmp(p, "clear") == 0 || stricmp(p, "clear_all") == 0) {
        *URETX_RX_TOTAL_DTU         = 0;
        *URETX_RX_BAD_DTU           = 0;
        *URETX_RX_GOOD_DTU          = 0;
        *URETX_RX_CORRECTED_DTU     = 0;
        *URETX_RX_OUTOFDATE_DTU     = 0;
        *URETX_RX_DUPLICATE_DTU     = 0;
        *URETX_RX_TIMEOUT_DTU       = 0;
        *RxDTURetransmittedCNT      = 0;

        *WRX_BC0_CELL_NUM           = 0;
        *WRX_BC0_DROP_CELL_NUM      = 0;
        *WRX_BC0_NONRETX_CELL_NUM   = 0;
        *WRX_BC0_RETX_CELL_NUM      = 0;
        *WRX_BC0_OUTOFDATE_CELL_NUM = 0;
        *WRX_BC0_DIRECTUP_NUM       = 0;
        *WRX_BC0_PBW_TOTAL_NUM      = 0;
        *WRX_BC0_PBW_SUCC_NUM       = 0;
        *WRX_BC0_PBW_FAIL_NUM       = 0;
        *WRX_BC1_CELL_NUM           = 0;

        for ( i = 0; i < 16; ++i ) {
            WRX_PER_PVC_CORRECT_PDU_BASE[i] = 0;
            WRX_PER_PVC_ERROR_PDU_BASE[i]   = 0;
        }

        WAN_MIB_TABLE->wrx_drophtu_cell = 0;
        WAN_MIB_TABLE->wrx_dropdes_pdu  = 0;
        WAN_MIB_TABLE->wrx_correct_pdu  = 0;
        WAN_MIB_TABLE->wrx_err_pdu      = 0;
        WAN_MIB_TABLE->wrx_dropdes_cell = 0;
        WAN_MIB_TABLE->wrx_correct_cell = 0;
        WAN_MIB_TABLE->wrx_err_cell     = 0;
        WAN_MIB_TABLE->wrx_total_byte   = 0;

        WAN_MIB_TABLE->wtx_total_pdu    = 0;
        WAN_MIB_TABLE->wtx_total_cell   = 0;
        WAN_MIB_TABLE->wtx_total_byte   = 0;

        *URETX_ALPHA_SWITCH_TO_HUNT_TIMES   = 0;

        if (stricmp(p, "clear_all") == 0) {
            *RxLastEFBCNT      = 0;
            *RxDTUCorrectedCNT = 0;
            *RxDTUCorruptedCNT = 0;
            *RxRetxDTUUncorrectedCNT = 0;
        }
    }

    return count;
}

#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC

static int proc_read_dbg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "error print - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_ERR)           ? "enabled" : "disabled");
    len += sprintf(page + off + len, "debug print - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DEBUG_PRINT)   ? "enabled" : "disabled");
    len += sprintf(page + off + len, "assert      - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_ASSERT)        ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump rx skb - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_RX)   ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump tx skb - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_SKB_TX)   ? "enabled" : "disabled");
    len += sprintf(page + off + len, "qos         - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_QOS)      ? "enabled" : "disabled");
    len += sprintf(page + off + len, "dump init   - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_DUMP_INIT)     ? "enabled" : "disabled");
    len += sprintf(page + off + len, "mac swap    - %s\n", (ifx_atm_dbg_enable & DBG_ENABLE_MASK_MAC_SWAP)      ? "enabled" : "disabled");

    *eof = 1;

    return len;
}

static int proc_write_dbg(struct file *file, const char *buf, unsigned long count, void *data)
{
    static const char *dbg_enable_mask_str[] = {
        " error print",
        " err",
        " debug print",
        " dbg",
        " assert",
        " assert",
        " dump rx skb",
        " rx",
        " dump tx skb",
        " tx",
        " dump qos",
        " qos",
        " dump init",
        " init",
        " mac swap",
        " swap",
        " all"
    };
    static const int dbg_enable_mask_str_len[] = {
        12, 4,
        12, 4,
        7,  7,
        12, 3,
        12, 3,
        9,  4,
        10, 5,
        9,  5,
        4
    };
    unsigned int dbg_enable_mask[] = {
        DBG_ENABLE_MASK_ERR,
        DBG_ENABLE_MASK_DEBUG_PRINT,
        DBG_ENABLE_MASK_ASSERT,
        DBG_ENABLE_MASK_DUMP_SKB_RX,
        DBG_ENABLE_MASK_DUMP_SKB_TX,
        DBG_ENABLE_MASK_DUMP_QOS,
        DBG_ENABLE_MASK_DUMP_INIT,
        DBG_ENABLE_MASK_MAC_SWAP,
        DBG_ENABLE_MASK_ALL
    };

    char *str;
	int str_buff_len = 1024;
    char *p;

    int len, rlen;

    int f_enable = 0;
    int i;
	
	str = vmalloc(str_buff_len);
	if(!str){
		return 0;
	}

    len = count < str_buff_len ? count : str_buff_len - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p ){
		vfree(str);
        return 0;
	}

    if ( strincmp(p, "enable", 6) == 0 ) {
        p += 6;
        f_enable = 1;
    }
    else if ( strincmp(p, "disable", 7) == 0 ) {
        p += 7;
        f_enable = -1;
    }
    else if ( strincmp(p, "help", 4) == 0 || *p == '?' ) {
        printk("echo <enable/disable> [err/dbg/assert/rx/tx/init/all] > /proc/eth/dbg\n");
    }

    if ( f_enable ) {
        if ( *p == 0 ) {
            if ( f_enable > 0 )
                ifx_atm_dbg_enable |= DBG_ENABLE_MASK_ALL & ~DBG_ENABLE_MASK_MAC_SWAP;
            else
                ifx_atm_dbg_enable &= ~DBG_ENABLE_MASK_ALL | DBG_ENABLE_MASK_MAC_SWAP;
        }
        else {
            do {
                for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
                    if ( strincmp(p, dbg_enable_mask_str[i], dbg_enable_mask_str_len[i]) == 0 ) {
                        if ( f_enable > 0 )
                            ifx_atm_dbg_enable |= dbg_enable_mask[i >> 1];
                        else
                            ifx_atm_dbg_enable &= ~dbg_enable_mask[i >> 1];
                        p += dbg_enable_mask_str_len[i];
                        break;
                    }
            } while ( i < NUM_ENTITY(dbg_enable_mask_str) );
        }
    }

	vfree(str);
    return count;
}

static inline unsigned long sb_addr_to_fpi_addr_convert(unsigned long sb_addr)
{
 #define PP32_SB_ADDR_END        0xFFFF

    if ( sb_addr < PP32_SB_ADDR_END )
        return (unsigned long)SB_BUFFER(sb_addr);
    else
        return sb_addr;
}

static int proc_write_mem(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *p1, *p2;
    int len;
    int colon;
    unsigned long *p;
    int i, n, l;
	int local_buf_size = 1024;
    char *local_buf = NULL;

	local_buf = vmalloc(local_buf_size);
	if ( !local_buf ){
		return 0;
	}

    len = local_buf_size < count ? local_buf_size - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    colon = 1;
    while ( get_token(&p1, &p2, &len, &colon) ) {
        if ( stricmp(p1, "w") == 0 || stricmp(p1, "write") == 0 || stricmp(p1, "r") == 0 || stricmp(p1, "read") == 0 )
            break;

        p1 = p2;
        colon = 1;
    }

    if ( *p1 == 'w' ) {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert((unsigned long)p);

        if ( (unsigned int)p >= KSEG0 )
            while ( 1 ) {
                ignore_space(&p2, &len);
                if ( !len || !((*p2 >= '0' && *p2 <= '9') || (*p2 >= 'a' && *p2 <= 'f') || (*p2 >= 'A' && *p2 <= 'F')) )
                    break;

                *p++ = (unsigned int)get_number(&p2, &len, 1);
            }
    }
    else if ( *p1 == 'r' ) {
        ignore_space(&p2, &len);
        p = (unsigned long *)get_number(&p2, &len, 1);
        p = (unsigned long *)sb_addr_to_fpi_addr_convert((unsigned long)p);

        if ( (unsigned int)p >= KSEG0 ) {
            ignore_space(&p2, &len);
            n = (int)get_number(&p2, &len, 0);
            if ( n ) {
                char str[32] = {0};
                char *pch = str;
                int k;
                unsigned int data;
                char c;

                n += (l = ((int)p >> 2) & 0x03);
                p = (unsigned long *)((unsigned int)p & ~0x0F);
                for ( i = 0; i < n; i++ ) {
                    if ( (i & 0x03) == 0 ) {
                        printk("%08X:", (unsigned int)p);
                        pch = str;
                    }
                    if ( i < l ) {
                        printk("         ");
                        sprintf(pch, "    ");
                    }
                    else {
                        data = (unsigned int)*p;
                        printk(" %08X", data);
                        for ( k = 0; k < 4; k++ ) {
                            c = ((char*)&data)[k];
                            pch[k] = c < ' ' ? '.' : c;
                        }
                    }
                    p++;
                    pch += 4;
                    if ( (i & 0x03) == 0x03 ) {
                        pch[0] = 0;
                        printk(" ; %s\n", str);
                    }
                }
                if ( (n & 0x03) != 0x00 ) {
                    for ( k = 4 - (n & 0x03); k > 0; k-- )
                        printk("         ");
                    pch[0] = 0;
                    printk(" ; %s\n", str);
                }
            }
        }
    }

	vfree(local_buf);
    return count;
}

 #if defined(CONFIG_AR9) || defined(CONFIG_VR9)

static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *stron = " on";
    static const char *stroff = "off";

    int len = 0;
    int cur_context;
    int f_stopped;
    char str[256];
    char strlength;
    int i, j;

    int pp32;

    for ( pp32 = 0; pp32 < NUM_OF_PP32; pp32++ ) {
        f_stopped = 0;

        len += sprintf(page + off + len, "===== pp32 core %d =====\n", pp32);

  #ifdef CONFIG_VR9
        if ( (*PP32_FREEZE & (1 << (pp32 << 4))) != 0 ) {
            sprintf(str, "freezed");
            f_stopped = 1;
        }
  #else
        if ( 0 ) {
        }
  #endif
        else if ( PP32_CPU_USER_STOPPED(pp32) || PP32_CPU_USER_BREAKIN_RCV(pp32) || PP32_CPU_USER_BREAKPOINT_MET(pp32) ) {
            strlength = 0;
            if ( PP32_CPU_USER_STOPPED(pp32) )
                strlength += sprintf(str + strlength, "stopped");
            if ( PP32_CPU_USER_BREAKPOINT_MET(pp32) )
                strlength += sprintf(str + strlength, strlength ? " | breakpoint" : "breakpoint");
            if ( PP32_CPU_USER_BREAKIN_RCV(pp32) )
                strlength += sprintf(str + strlength, strlength ? " | breakin" : "breakin");
            f_stopped = 1;
        }
  #if 0
        else if ( PP32_CPU_CUR_PC(pp32) == PP32_CPU_CUR_PC(pp32) ) {
            sprintf(str, "hang");
            f_stopped = 1;
        }
  #endif
        else
            sprintf(str, "running");
        cur_context = PP32_BRK_CUR_CONTEXT(pp32);
        len += sprintf(page + off + len, "Context: %d, PC: 0x%04x, %s\n", cur_context, PP32_CPU_CUR_PC(pp32), str);

        if ( PP32_CPU_USER_BREAKPOINT_MET(pp32) ) {
            strlength = 0;
            if ( PP32_BRK_PC_MET(pp32, 0) )
                strlength += sprintf(str + strlength, "pc0");
            if ( PP32_BRK_PC_MET(pp32, 1) )
                strlength += sprintf(str + strlength, strlength ? " | pc1" : "pc1");
            if ( PP32_BRK_DATA_ADDR_MET(pp32, 0) )
                strlength += sprintf(str + strlength, strlength ? " | daddr0" : "daddr0");
            if ( PP32_BRK_DATA_ADDR_MET(pp32, 1) )
                strlength += sprintf(str + strlength, strlength ? " | daddr1" : "daddr1");
            if ( PP32_BRK_DATA_VALUE_RD_MET(pp32, 0) ) {
                strlength += sprintf(str + strlength, strlength ? " | rdval0" : "rdval0");
                if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(pp32, 0) ) {
                    if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 0) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 0) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_RD_MET(pp32, 1) ) {
                strlength += sprintf(str + strlength, strlength ? " | rdval1" : "rdval1");
                if ( PP32_BRK_DATA_VALUE_RD_LO_EQ(pp32, 1) ) {
                    if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 1) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_RD_GT_EQ(pp32, 1) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_WR_MET(pp32, 0) ) {
                strlength += sprintf(str + strlength, strlength ? " | wtval0" : "wtval0");
                if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(pp32, 0) ) {
                    if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 0) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 0) )
                    strlength += sprintf(str + strlength, " >=");
            }
            if ( PP32_BRK_DATA_VALUE_WR_MET(pp32, 1) ) {
                strlength += sprintf(str + strlength, strlength ? " | wtval1" : "wtval1");
                if ( PP32_BRK_DATA_VALUE_WR_LO_EQ(pp32, 1) ) {
                    if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 1) )
                        strlength += sprintf(str + strlength, " ==");
                    else
                        strlength += sprintf(str + strlength, " <=");
                }
                else if ( PP32_BRK_DATA_VALUE_WR_GT_EQ(pp32, 1) )
                    strlength += sprintf(str + strlength, " >=");
            }
            len += sprintf(page + off + len, "break reason: %s\n", str);
        }

        if ( f_stopped )
        {
            len += sprintf(page + off + len, "General Purpose Register (Context %d):\n", cur_context);
            for ( i = 0; i < 4; i++ ) {
                for ( j = 0; j < 4; j++ )
                    len += sprintf(page + off + len, "   %2d: %08x", i + j * 4, *PP32_GP_CONTEXTi_REGn(pp32, cur_context, i + j * 4));
                len += sprintf(page + off + len, "\n");
            }
        }

        len += sprintf(page + off + len, "break out on: break in - %s, stop - %s\n",
                                            PP32_CTRL_OPT_BREAKOUT_ON_BREAKIN(pp32) ? stron : stroff,
                                            PP32_CTRL_OPT_BREAKOUT_ON_STOP(pp32) ? stron : stroff);
        len += sprintf(page + off + len, "     stop on: break in - %s, break point - %s\n",
                                            PP32_CTRL_OPT_STOP_ON_BREAKIN(pp32) ? stron : stroff,
                                            PP32_CTRL_OPT_STOP_ON_BREAKPOINT(pp32) ? stron : stroff);
        len += sprintf(page + off + len, "breakpoint:\n");
        len += sprintf(page + off + len, "     pc0: 0x%08x, %s\n", *PP32_BRK_PC(pp32, 0), PP32_BRK_GRPi_PCn(pp32, 0, 0) ? "group 0" : "off");
        len += sprintf(page + off + len, "     pc1: 0x%08x, %s\n", *PP32_BRK_PC(pp32, 1), PP32_BRK_GRPi_PCn(pp32, 1, 1) ? "group 1" : "off");
        len += sprintf(page + off + len, "  daddr0: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(pp32, 0), PP32_BRK_GRPi_DATA_ADDRn(pp32, 0, 0) ? "group 0" : "off");
        len += sprintf(page + off + len, "  daddr1: 0x%08x, %s\n", *PP32_BRK_DATA_ADDR(pp32, 1), PP32_BRK_GRPi_DATA_ADDRn(pp32, 1, 1) ? "group 1" : "off");
        len += sprintf(page + off + len, "  rdval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(pp32, 0));
        len += sprintf(page + off + len, "  rdval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_RD(pp32, 1));
        len += sprintf(page + off + len, "  wrval0: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(pp32, 0));
        len += sprintf(page + off + len, "  wrval1: 0x%08x\n", *PP32_BRK_DATA_VALUE_WR(pp32, 1));
    }

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *str = NULL;
    char *p;
    unsigned int addr;
	int str_buff_len = 1024;

    int len, rlen;

    int pp32 = 0;

	str = vmalloc(str_buff_len);
	if (!str) {
		return 0;	
	}

    len = count <  str_buff_len ? count : str_buff_len - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p ){
		vfree(str);
        return 0;
	}

    if ( strincmp(p, "pp32 ", 5) == 0 ) {
        p += 5;
        rlen -= 5;

        while ( rlen > 0 && *p >= '0' && *p <= '9' ) {
            pp32 += *p - '0';
            p++;
            rlen--;
        }
        while ( rlen > 0 && *p && *p <= ' ' ) {
            p++;
            rlen--;
        }

        if ( pp32 >= NUM_OF_PP32 ) {
            err("incorrect pp32 index - %d", pp32);
			vfree(str);
            return count;
        }
    }

    if ( stricmp(p, "start") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_RESTART;
    else if ( stricmp(p, "stop") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_STOP;
    else if ( stricmp(p, "step") == 0 )
        *PP32_CTRL_CMD(pp32) = PP32_CTRL_CMD_STEP;
  #ifdef CONFIG_VR9
    else if ( stricmp(p, "restart") == 0 )
        *PP32_FREEZE &= ~(1 << (pp32 << 4));
    else if ( stricmp(p, "freeze") == 0 )
        *PP32_FREEZE |= 1 << (pp32 << 4);
  #endif
    else if ( strincmp(p, "pc0 ", 4) == 0 ) {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 ) {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_OFF(0, 0);
            *PP32_BRK_PC_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(pp32, 0) = 0;
        }
        else {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(pp32, 0) = addr;
            *PP32_BRK_PC_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "pc1 ", 4) == 0 ) {
        p += 4;
        rlen -= 4;
        if ( stricmp(p, "off") == 0 ) {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_OFF(1, 1);
            *PP32_BRK_PC_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_PC(pp32, 1) = 0;
        }
        else {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_PC(pp32, 1) = addr;
            *PP32_BRK_PC_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_PCn_ON(1, 1);
        }
    }
    else if ( strincmp(p, "daddr0 ", 7) == 0 ) {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 ) {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_OFF(0, 0);
            *PP32_BRK_DATA_ADDR_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(pp32, 0) = 0;
        }
        else {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(pp32, 0) = addr;
            *PP32_BRK_DATA_ADDR_MASK(pp32, 0) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_ON(0, 0);
        }
    }
    else if ( strincmp(p, "daddr1 ", 7) == 0 ) {
        p += 7;
        rlen -= 7;
        if ( stricmp(p, "off") == 0 ) {
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_OFF(1, 1);
            *PP32_BRK_DATA_ADDR_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN;
            *PP32_BRK_DATA_ADDR(pp32, 1) = 0;
        }
        else {
            addr = get_number(&p, &rlen, 1);
            *PP32_BRK_DATA_ADDR(pp32, 1) = addr;
            *PP32_BRK_DATA_ADDR_MASK(pp32, 1) = PP32_BRK_CONTEXT_MASK_EN | PP32_BRK_CONTEXT_MASK(0) | PP32_BRK_CONTEXT_MASK(1) | PP32_BRK_CONTEXT_MASK(2) | PP32_BRK_CONTEXT_MASK(3);
            *PP32_BRK_TRIG(pp32) = PP32_BRK_GRPi_DATA_ADDRn_ON(1, 1);
        }
    }
    else {

        printk("echo \"<command>\" > /proc/driver/ifx_ptm/pp32\n");
        printk("  command:\n");
        printk("    start  - run pp32\n");
        printk("    stop   - stop pp32\n");
        printk("    step   - run pp32 with one step only\n");
        printk("    pc0    - pc0 <addr>/off, set break point PC0\n");
        printk("    pc1    - pc1 <addr>/off, set break point PC1\n");
        printk("    daddr0 - daddr0 <addr>/off, set break point data address 0\n");
        printk("    daddr0 - daddr1 <addr>/off, set break point data address 1\n");
        printk("    help   - print this screen\n");
    }

    if ( *PP32_BRK_TRIG(pp32) )
        *PP32_CTRL_OPT(pp32) = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_ON;
    else
        *PP32_CTRL_OPT(pp32) = PP32_CTRL_OPT_STOP_ON_BREAKPOINT_OFF;
	vfree(str);
    return count;
}

#elif defined(CONFIG_DANUBE)

static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *halt_stat[] = {
        "reset",
        "break in line",
        "stop",
        "step",
        "code",
        "data0",
        "data1"
    };
    static const char *brk_src_data[] = {
        "off",
        "read",
        "write",
        "read/write",
        "write_equal",
        "N/A",
        "N/A",
        "N/A"
    };
    static const char *brk_src_code[] = {
        "off",
        "on"
    };

    int len = 0;
    int i;
    int k;
    unsigned long bit;
    int tsk;

    tsk = *PP32_DBG_TASK_NO & 0x03;
    len += sprintf(page + off + len, "Task No %d, PC %04x\n", tsk, *PP32_DBG_CUR_PC & 0xFFFF);

    if ( !(*PP32_HALT_STAT & 0x01) )
        len += sprintf(page + off + len, "  Halt State: Running\n");
    else {
        len += sprintf(page + off + len, "  Halt State: Stopped");
        k = 0;
        for ( bit = 2, i = 0; bit <= (1 << 7); bit <<= 1, i++ )
            if ( (*PP32_HALT_STAT & bit) ) {
                if ( !k ) {
                    len += sprintf(page + off + len, ", ");
                    k++;
                }
                else
                    len += sprintf(page + off + len, " | ");
                len += sprintf(page + off + len, halt_stat[i]);
            }

        len += sprintf(page + off + len, "\n");

        len += sprintf(page + off + len, "  Regs (Task %d):\n", tsk);
        for ( i = 0; i < 8; i++ )
            len += sprintf(page + off + len, "    %2d. %08x    %2d. %08x\n", i, *PP32_DBG_REG_BASE(tsk, i), i + 8, *PP32_DBG_REG_BASE(tsk, i + 8));
    }

    len += sprintf(page + off + len, "  Break Src:  data1 - %s, data0 - %s, pc3 - %s, pc2 - %s, pc1 - %s, pc0 - %s\n",
                            brk_src_data[(*PP32_BRK_SRC >> 11) & 0x07],
                            brk_src_data[(*PP32_BRK_SRC >> 8) & 0x07],
                            brk_src_code[(*PP32_BRK_SRC >> 3) & 0x01],
                            brk_src_code[(*PP32_BRK_SRC >> 2) & 0x01],
                            brk_src_code[(*PP32_BRK_SRC >> 1) & 0x01],
                            brk_src_code[*PP32_BRK_SRC & 0x01]);

    for ( i = 0; i < 4; i++ )
        len += sprintf(page + off + len, "    pc%d:      %04x - %04x\n", i, *PP32_DBG_PC_MIN(i), *PP32_DBG_PC_MAX(i));

    for ( i = 0; i < 2; i++ )
        len += sprintf(page + off + len, "    data%d:    %04x - %04x (%08x)\n", i, *PP32_DBG_DATA_MIN(i), *PP32_DBG_DATA_MAX(i), *PP32_DBG_DATA_VAL(i));

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *str;
    char *p;

    int len, rlen;
	int str_buff_len = 2048;
	str = vmalloc(str_buff_len);
	if (!str){
		return 0;
	}
    len = count < str_buff_len ? count : str_buff_len - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
		vfree(str);
        return 0;

    if ( stricmp(p, "start") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_START_SET(1);
    else if ( stricmp(p, "stop") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STOP_SET(1);
    else if ( stricmp(p, "step") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STEP_SET(1);
    else if ( strincmp(p, "pc", 2) == 0 && p[2] >= '0' && p[2] <= '3' && p[3] <= ' ' ) {
        int n = p[2] - '0';
        int on_off_flag = -1;
        int addr_min, addr_max;

        p += 4;
        rlen -= 4;
        ignore_space(&p, &rlen);

        if ( strincmp(p, "off", 3) == 0 && p[3] <= ' ' ) {
            p += 3;
            rlen -= 3;
            on_off_flag = 0;
        }
        else if ( strincmp(p, "on", 2) == 0 && p[2] <= ' ' ) {
            p += 2;
            rlen -= 2;
            on_off_flag = 1;
        }
        ignore_space(&p, &rlen);

        if ( rlen ) {
            addr_min = get_number(&p, &rlen, 1);
            ignore_space(&p, &rlen);
            if ( rlen )
                addr_max = get_number(&p, &rlen, 1);
            else
                addr_max = addr_min;

            *PP32_DBG_PC_MIN(n) = addr_min;
            *PP32_DBG_PC_MAX(n) = addr_max;
        }

        if ( on_off_flag == 0 )
            *PP32_BRK_SRC &= ~(1 << n);
        else if ( on_off_flag > 0 )
            *PP32_BRK_SRC |= 1 << n;
    }
    else if ( strincmp(p, "data", 4) == 0 && p[4] >= '0' && p[4] <= '1' && p[5] <= ' ' ) {
        const static char *data_cmd_str[] = {"r", "w", "rw", "w=", "off", "min", "min addr", "max", "max addr", "val", "value"};
        const static int data_cmd_len[] = {1, 1, 2, 2, 3, 3, 8, 3, 8, 3, 5};
        const static int data_cmd_idx[] = {1, 2, 3, 4, 0, 5, 5, 6, 6, 7, 7};
        int n = p[4] - '0';
        int on_off_flag = -1, on_off_mask = 0;
        int addr_min = -1, addr_max = -1;
        int value = 0, f_got_value = 0;
        int stat = 0;
        int i;
        int tmp;

        p += 6;
        rlen -= 6;

        while ( 1 ) {
            ignore_space(&p, &rlen);
            if ( rlen <= 0 )
                break;
            for ( i = 0; i < NUM_ENTITY(data_cmd_str); i++ )
                if ( strincmp(p, data_cmd_str[i], data_cmd_len[i]) == 0 && p[data_cmd_len[i]] <= ' ' ) {
                    p += data_cmd_len[i];
                    rlen -= data_cmd_len[i];
                    stat = data_cmd_idx[i];
                    if ( stat <= 4 ) {
                        on_off_mask = 7;
                        on_off_flag = stat;
                    }
                    break;
                }
            if ( i == NUM_ENTITY(data_cmd_str) ) {
                if ( (*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F') ) {
                    tmp = get_number(&p, &rlen, 1);
                    if ( stat <= 5 ) {
                        addr_min = tmp;
                        stat = 6;
                    }
                    else if ( stat >= 7 ) {
                        value = tmp;
                        f_got_value = 1;
                    }
                    else {
                        addr_max = tmp;
                        stat = 7;
                    }
                }
                else
                    for ( ; rlen && *p > ' '; rlen--, p++ );
            }
        }

        if ( addr_min >= 0 )
            *PP32_DBG_DATA_MIN(n) = *PP32_DBG_DATA_MAX(n) = addr_min;
        if ( addr_max >= 0 )
            *PP32_DBG_DATA_MAX(n) = addr_max;
        if ( f_got_value )
            *PP32_DBG_DATA_VAL(n) = value;
        if ( on_off_mask && on_off_flag >= 0 ) {
            on_off_flag <<= n ? 11 : 8;
            on_off_mask <<= n ? 11 : 8;
            *PP32_BRK_SRC = (*PP32_BRK_SRC & ~on_off_mask) | on_off_flag;
        }
    }
    else {
        printk("echo \"<command>\" > /proc/eth/etop\n");
        printk("  command:\n");
        printk("    start - run pp32\n");
        printk("    stop  - stop pp32\n");
        printk("    step  - run pp32 with one step only\n");
        printk("    pc    - pc? [on/off] [min addr] [max addr], set PC break point\n");
        printk("    data  - data? [r/w/rw/w=/off] [min <addr>] [max <addr>] [val <value>], set data break point\n");
        printk("    help  - print this screen\n");
    }

	vfree(str);
    return count;
}

 #elif defined(CONFIG_AMAZON_SE)

static int proc_read_pp32(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    static const char *halt_stat[] = {
        "reset",
        "break in line",
        "stop",
        "step",
        "code",
        "data0",
        "data1"
    };
    static const char *brk_src_data[] = {
        "off",
        "read",
        "write",
        "read/write",
        "write_equal",
        "N/A",
        "N/A",
        "N/A"
    };
    static const char *brk_src_code[] = {
        "off",
        "on"
    };

    int len = 0;
    int i;
    int k;
    unsigned long bit;

    len += sprintf(page + off + len, "Task No %d, PC %04x\n", *PP32_DBG_TASK_NO & 0x03, *PP32_DBG_CUR_PC & 0xFFFF);

    if ( !(*PP32_HALT_STAT & 0x01) )
        len += sprintf(page + off + len, "  Halt State: Running\n");
    else
    {
        len += sprintf(page + off + len, "  Halt State: Stopped");
        k = 0;
        for ( bit = 2, i = 0; bit <= (1 << 7); bit <<= 1, i++ )
            if ( (*PP32_HALT_STAT & bit) )
            {
                if ( !k )
                {
                    len += sprintf(page + off + len, ", ");
                    k++;
                }
                else
                    len += sprintf(page + off + len, " | ");
                len += sprintf(page + off + len, halt_stat[i]);
            }

        len += sprintf(page + off + len, "\n");
    }

    len += sprintf(page + off + len, "  Break Src:  data1 - %s, data0 - %s, pc3 - %s, pc2 - %s, pc1 - %s, pc0 - %s\n",
                                                    brk_src_data[(*PP32_BRK_SRC >> 11) & 0x07], brk_src_data[(*PP32_BRK_SRC >> 8) & 0x07], brk_src_code[(*PP32_BRK_SRC >> 3) & 0x01], brk_src_code[(*PP32_BRK_SRC >> 2) & 0x01], brk_src_code[(*PP32_BRK_SRC >> 1) & 0x01], brk_src_code[*PP32_BRK_SRC & 0x01]);

//    for ( i = 0; i < 4; i++ )
//        len += sprintf(page + off + len, "    pc%d:      %04x - %04x\n", i, *PP32_DBG_PC_MIN(i), *PP32_DBG_PC_MAX(i));

//    for ( i = 0; i < 2; i++ )
//        len += sprintf(page + off + len, "    data%d:    %04x - %04x (%08x)\n", i, *PP32_DBG_DATA_MIN(i), *PP32_DBG_DATA_MAX(i), *PP32_DBG_DATA_VAL(i));

    *eof = 1;

    return len;
}

static int proc_write_pp32(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;

    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    if ( stricmp(str, "start") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_RESTART;
    else if ( stricmp(str, "stop") == 0 )
        *PP32_DBG_CTRL = DBG_CTRL_STOP;
//    else if ( stricmp(str, "step") == 0 )
//        *PP32_DBG_CTRL = DBG_CTRL_STEP_SET(1);
    else
    {
        printk("echo \"<command>\" > /proc/eth/etop\n");
        printk("  command:\n");
        printk("    start - run pp32\n");
        printk("    stop  - stop pp32\n");
//        printk("    step  - run pp32 with one step only\n");
        printk("    help  - print this screen\n");
    }

    return count;
}

 #endif

#endif

#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC

static INLINE int print_htu(char *buf, int i)
{
    int len = 0;

    if ( HTU_ENTRY(i)->vld ) {
        len += sprintf(buf + len, "%2d. valid\n", i);
        len += sprintf(buf + len, "    entry  0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(unsigned int*)HTU_ENTRY(i), HTU_ENTRY(i)->pid, HTU_ENTRY(i)->vpi, HTU_ENTRY(i)->vci, HTU_ENTRY(i)->pti);
        len += sprintf(buf + len, "    mask   0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(unsigned int*)HTU_MASK(i), HTU_MASK(i)->pid_mask, HTU_MASK(i)->vpi_mask, HTU_MASK(i)->vci_mask, HTU_MASK(i)->pti_mask);
        len += sprintf(buf + len, "    result 0x%08x - type: %s, qid: %d", *(unsigned int*)HTU_RESULT(i), HTU_RESULT(i)->type ? "cell" : "AAL5", HTU_RESULT(i)->qid);
        if ( HTU_RESULT(i)->type )
            len += sprintf(buf + len, ", cell id: %d, verification: %s", HTU_RESULT(i)->cellid, HTU_RESULT(i)->ven ? "on" : "off");
        len += sprintf(buf + len, "\n");
    }
    else
        len += sprintf(buf + len, "%2d. invalid\n", i);

    return len;
}

static int proc_read_htu(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    int llen;
    char *str;
    int htuts = *CFG_WRX_HTUTS;
    int i;
		
	str = vmalloc (1024);
	if (!str)
		return 0;

    pstr = *start = page;

    llen = sprintf(pstr, "HTU Table (Max %d):\n", htuts);
    pstr += llen;
    len += llen;

    for ( i = 0; i < htuts; i++ ) {
        llen = print_htu(str, i);
        if ( len <= off && len + llen > off ) {
            memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off ) {
            memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_HTU_OVERRUN_END;
    }

    *eof = 1;
	vfree(str);
    return len - off;

PROC_READ_HTU_OVERRUN_END:

    return len - llen - off;
}

static INLINE int print_tx_queue(char *buf, int i)
{
    int len = 0;

    if ( (*WTX_DMACH_ON & (1 << i)) ) {
        len += sprintf(buf + len, "%2d. valid\n", i);
        len += sprintf(buf + len, "    queue 0x%08x - sbid %u, qsb vcid %u, qsb %s\n", (unsigned int)WTX_QUEUE_CONFIG(i), (unsigned int)WTX_QUEUE_CONFIG(i)->sbid, (unsigned int)WTX_QUEUE_CONFIG(i)->qsb_vcid, WTX_QUEUE_CONFIG(i)->qsben ? "enable" : "disable");
        len += sprintf(buf + len, "    dma   0x%08x - base %08x, len %u, vlddes %u\n", (unsigned int)WTX_DMA_CHANNEL_CONFIG(i), WTX_DMA_CHANNEL_CONFIG(i)->desba, WTX_DMA_CHANNEL_CONFIG(i)->deslen, WTX_DMA_CHANNEL_CONFIG(i)->vlddes);
    }
    else
        len += sprintf(buf + len, "%2d. invalid\n", i);

    return len;
}

static int proc_read_txq(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    int len_max = off + count;
    char *pstr;
    int llen;
	int str_buff_len = 1024;
    char *str;

    int i;
	
	str = vmalloc(str_buff_len);
	if (!str){
		return 0;
	}
    pstr = *start = page;

    llen = sprintf(pstr, "TX Queue Config (Max %d):\n", *CFG_WTX_DCHNUM);
    pstr += llen;
    len += llen;

    for ( i = 0; i < 16; i++ ) {
        llen = print_tx_queue(str, i);
        if ( len <= off && len + llen > off ) {
            memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off ) {
            memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_HTU_OVERRUN_END;
    }


    *eof = 1;

	vfree(str);
    return len - off;

PROC_READ_HTU_OVERRUN_END:

    return len - llen - off;
}

 #if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX

static int proc_read_retx_fw(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    unsigned int next_dtu_sid_out, last_dtu_sid_in, next_cell_sid_out, isr_cell_id;
    unsigned int curr_time, sec_counter, curr_efb;
    struct Retx_adsl_ppe_intf adsl_ppe_intf;

    adsl_ppe_intf       = *RETX_ADSL_PPE_INTF;
    next_dtu_sid_out    = *NEXT_DTU_SID_OUT;
    last_dtu_sid_in     = *LAST_DTU_SID_IN;
    next_cell_sid_out   = *NEXT_CELL_SID_OUT;
    isr_cell_id         = *ISR_CELL_ID;

    curr_time   = *URetx_curr_time;
    sec_counter = *URetx_sec_counter;
    curr_efb    = *RxCURR_EFB;


    len += sprintf(page + off + len, "Adsl-PPE Interface:\n");
    len += sprintf(page + off + len, "  dtu_sid                = 0x%02x [%3u]\n", adsl_ppe_intf.dtu_sid, adsl_ppe_intf.dtu_sid);
    len += sprintf(page + off + len, "  dtu_timestamp          = 0x%02x\n", adsl_ppe_intf.dtu_timestamp);
    len += sprintf(page + off + len, "  local_time             = 0x%02x\n", adsl_ppe_intf.local_time);
    len += sprintf(page + off + len, "  is_last_cw             = %u\n", adsl_ppe_intf.is_last_cw);
    len += sprintf(page + off + len, "  reinit_flag            = %u\n", adsl_ppe_intf.reinit_flag);
    len += sprintf(page + off + len, "  is_bad_cw              = %u\n", adsl_ppe_intf.is_bad_cw);
    len += sprintf(page + off + len, "\n");


    len += sprintf(page + off + len, "Retx Firmware Context:\n");
    len += sprintf(page + off + len, "  next_dtu_sid_out       (0x%08x)  = 0x%02x [%3u]\n", (unsigned int )NEXT_DTU_SID_OUT,   next_dtu_sid_out, next_dtu_sid_out);
    len += sprintf(page + off + len, "  last_dtu_sid_in        (0x%08x)  = 0x%02x [%3u]\n", (unsigned int )LAST_DTU_SID_IN,    last_dtu_sid_in,  last_dtu_sid_in);
    len += sprintf(page + off + len, "  next_cell_sid_out      (0x%08x)  = %u\n", (unsigned int )NEXT_CELL_SID_OUT,  next_cell_sid_out);
    len += sprintf(page + off + len, "  isr_cell_id            (0x%08x)  = %u\n", (unsigned int )ISR_CELL_ID,        isr_cell_id);
    len += sprintf(page + off + len, "  pb_cell_search_idx     (0x%08x)  = %u\n", (unsigned int )PB_CELL_SEARCH_IDX, *PB_CELL_SEARCH_IDX);
    len += sprintf(page + off + len, "  pb_read_pend_flag      (0x%08x)  = %u\n", (unsigned int )PB_READ_PEND_FLAG,  *PB_READ_PEND_FLAG);
    len += sprintf(page + off + len, "  rfbi_first_cw          (0x%08x)  = %u\n", (unsigned int )RFBI_FIRST_CW,      *RFBI_FIRST_CW);
    len += sprintf(page + off + len, "  rfbi_bad_cw            (0x%08x)  = %u\n", (unsigned int )RFBI_BAD_CW,        *RFBI_BAD_CW);
    len += sprintf(page + off + len, "  rfbi_invalid_cw        (0x%08x)  = %u\n", (unsigned int )RFBI_INVALID_CW,    *RFBI_INVALID_CW);
    len += sprintf(page + off + len, "  rfbi_retx_cw           (0x%08x)  = %u\n", (unsigned int )RFBI_RETX_CW,       *RFBI_RETX_CW);
    len += sprintf(page + off + len, "  rfbi_chk_dtu_status    (0x%08x)  = %u\n", (unsigned int )RFBI_CHK_DTU_STATUS,*RFBI_CHK_DTU_STATUS);
    len += sprintf(page + off + len, "\n");


    len += sprintf(page + off + len, "SFSM Status:  bc0                      bc1  \n\n");
    len += sprintf(page + off + len, "  state     = %-22s , %s\n",
          (*__WRXCTXT_PortState(0) & 3) == 0 ? "Hunt" :
          (*__WRXCTXT_PortState(0) & 3) == 1 ? "Pre_sync" :
          (*__WRXCTXT_PortState(0) & 3) == 2 ? "Sync" :
                                               "Unknown(error)",
          (*__WRXCTXT_PortState(1) & 3) == 0 ? "Hunt" :
          (*__WRXCTXT_PortState(1) & 3) == 1 ? "Pre_sync" :
          (*__WRXCTXT_PortState(1) & 3) == 2 ? "Sync" :
                                               "Unknown(error)"  );
    len += sprintf(page + off + len, "  dbase     = 0x%04x  ( 0x%08x ) , 0x%04x  ( 0x%08x )\n",
            SFSM_DBA(0)->dbase, (unsigned int)PPM_INT_UNIT_ADDR(SFSM_DBA(0)->dbase + 0x2000),
            SFSM_DBA(1)->dbase, (unsigned int)PPM_INT_UNIT_ADDR(SFSM_DBA(1)->dbase + 0x2000));
    len += sprintf(page + off + len, "  cbase     = 0x%04x  ( 0x%08x ) , 0x%04x  ( 0x%08x )\n",
            SFSM_CBA(0)->cbase, (unsigned int)PPM_INT_UNIT_ADDR(SFSM_CBA(0)->cbase + 0x2000),
            SFSM_CBA(1)->cbase, (unsigned int)PPM_INT_UNIT_ADDR(SFSM_CBA(1)->cbase + 0x2000));
    len += sprintf(page + off + len, "  sen       = %-22d , %d\n", SFSM_CFG(0)->sen,        SFSM_CFG(1)->sen );
    len += sprintf(page + off + len, "  idlekeep  = %-22d , %d\n", SFSM_CFG(0)->idlekeep,   SFSM_CFG(1)->idlekeep );
    len += sprintf(page + off + len, "  pnum      = %-22d , %d\n", SFSM_CFG(0)->pnum,       SFSM_CFG(1)->pnum );
    len += sprintf(page + off + len, "  pptr      = %-22d , %d\n", SFSM_PGCNT(0)->pptr,     SFSM_PGCNT(1)->pptr);
    len += sprintf(page + off + len, "  upage     = %-22d , %d\n", SFSM_PGCNT(0)->upage,    SFSM_PGCNT(1)->upage);
    len += sprintf(page + off + len, "  l2_rdptr  = %-22d , %d\n", *__WRXCTXT_L2_RdPtr(0),  *__WRXCTXT_L2_RdPtr(1) );
    len += sprintf(page + off + len, "  l2_page   = %-22d , %d\n", *__WRXCTXT_L2Pages(0),   *__WRXCTXT_L2Pages(1) );
    len += sprintf(page + off + len, "\n");


    len += sprintf(page + off + len, "FFSM Status:  bc0                      bc1  \n\n");
    len += sprintf(page + off + len, "  dbase     = 0x%04x  ( 0x%08x ) , 0x%04x  ( 0x%08x )\n",
            FFSM_DBA(0)->dbase, (unsigned int)PPM_INT_UNIT_ADDR(FFSM_DBA(0)->dbase + 0x2000),
            FFSM_DBA(1)->dbase, (unsigned int)PPM_INT_UNIT_ADDR(FFSM_DBA(1)->dbase + 0x2000));
    len += sprintf(page + off + len, "  pnum      = %-22d , %d\n", FFSM_CFG(0)->pnum,       FFSM_CFG(1)->pnum);
    len += sprintf(page + off + len, "  vpage     = %-22d , %d\n", FFSM_PGCNT(0)->vpage,    FFSM_PGCNT(1)->vpage);
    len += sprintf(page + off + len, "  ival      = %-22d , %d\n", FFSM_PGCNT(0)->ival,     FFSM_PGCNT(1)->ival);
    len += sprintf(page + off + len, "  tc_wrptr  = %-22d , %d\n", *__WTXCTXT_TC_WRPTR(0),  *__WTXCTXT_TC_WRPTR(1));
    len += sprintf(page + off + len, "\n");


    len += sprintf(page + off + len, "Misc:  \n\n");
    len += sprintf(page + off + len, "  curr_time   = %08x\n", curr_time );
    len += sprintf(page + off + len, "  sec_counter = %d\n", sec_counter );
    len += sprintf(page + off + len, "  curr_efb    = %d\n", curr_efb );
    len += sprintf(page + off + len, "\n");

    *eof = 1;

    return len;
}

static inline int is_valid(unsigned int * dtu_vld_stat, int dtu_sid)
{
    int dw_idx = (dtu_sid / 32) & 7;
    int bit_pos = dtu_sid % 32;

    return dtu_vld_stat[dw_idx] & (0x80000000 >> bit_pos);
}

static int proc_read_retx_stats(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int i;
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[2048];
    int llen = 0;

    unsigned int next_dtu_sid_out, last_dtu_sid_in, next_cell_sid_out;
    unsigned int dtu_vld_stat[8];
    struct DTU_stat_info dtu_stat_info[256];
    struct Retx_adsl_ppe_intf adsl_ppe_intf;

    pstr = *start = page;

    __sync();

    // capture a snapshot of internal status
    next_dtu_sid_out    = *NEXT_DTU_SID_OUT;
    last_dtu_sid_in     = *LAST_DTU_SID_IN;
    next_cell_sid_out   = *NEXT_CELL_SID_OUT;
    adsl_ppe_intf       = *RETX_ADSL_PPE_INTF;

    memcpy(&dtu_vld_stat, (void *)DTU_VLD_STAT, sizeof(dtu_vld_stat));
    memcpy(&dtu_stat_info, (void *)DTU_STAT_INFO, sizeof(dtu_stat_info));


    llen += sprintf(str + llen, "Adsl-PPE Interface:\n");
    llen += sprintf(str + llen, "  dtu_sid                = 0x%02x [%3u]\n", adsl_ppe_intf.dtu_sid, adsl_ppe_intf.dtu_sid);
    llen += sprintf(str + llen, "  dtu_timestamp          = 0x%02x\n", adsl_ppe_intf.dtu_timestamp);
    llen += sprintf(str + llen, "  local_time             = 0x%02x\n", adsl_ppe_intf.local_time);
    llen += sprintf(str + llen, "  is_last_cw             = %u\n", adsl_ppe_intf.is_last_cw);
    llen += sprintf(str + llen, "  reinit_flag            = %u\n", adsl_ppe_intf.reinit_flag);
    llen += sprintf(str + llen, "  is_bad_cw              = %u\n", adsl_ppe_intf.is_bad_cw);
    llen += sprintf(str + llen, "\n");

    llen += sprintf(str + llen, "Retx Internal State:\n");
    llen += sprintf(str + llen, "  next_dtu_sid_out       (0x%08x)  = 0x%02x [%3u]\n", (unsigned int )NEXT_DTU_SID_OUT,   next_dtu_sid_out, next_dtu_sid_out);
    llen += sprintf(str + llen, "  last_dtu_sid_in        (0x%08x)  = 0x%02x [%3u]\n", (unsigned int )LAST_DTU_SID_IN,    last_dtu_sid_in, last_dtu_sid_in);
    llen += sprintf(str + llen, "  next_cell_sid_out      (0x%08x)  = %u\n", (unsigned int )NEXT_CELL_SID_OUT,  next_cell_sid_out);
    llen += sprintf(str + llen, "  dtu_valid_stat         (0x%08x)\n", (unsigned int )DTU_VLD_STAT);
    llen += sprintf(str + llen, "  dtu_stat_info          (0x%08x)\n", (unsigned int )DTU_STAT_INFO);
    llen += sprintf(str + llen, "  pb_buffer_usage        (0x%08x)\n", (unsigned int )PB_BUFFER_USAGE);

    if ( len <= off && len + llen > off ) {
        memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off ) {
        memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_RETX_STATS_OVERRUN_END;
    llen = 0;


    llen += sprintf(str + llen, "\n");
    llen += sprintf(str + llen, "DTU_VALID_STAT: [0x%08x]:\n", (unsigned int)DTU_VLD_STAT);
    llen += sprintf(str + llen, "%08X: %08X %08X %08X %08X    %08X %08X %08X %08X\n",
                    (unsigned int)DTU_VLD_STAT,
                    dtu_vld_stat[0], dtu_vld_stat[1], dtu_vld_stat[2], dtu_vld_stat[3],
                    dtu_vld_stat[4], dtu_vld_stat[5], dtu_vld_stat[6], dtu_vld_stat[7]);

    if ( len <= off && len + llen > off ) {
        memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off ) {
        memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_RETX_STATS_OVERRUN_END;
    llen = 0;


    llen += sprintf(str + llen, "\n");
    llen += sprintf(str + llen, "DTU_STAT_INFO: [0x%08x]:\n", (unsigned int)DTU_STAT_INFO);
    llen += sprintf(str + llen, "dtu_id        ts   complete   bad  cell_cnt  dtu_rd_ptr  dtu_wr_ptr\n");
    llen += sprintf(str + llen, "---------------------------------------------------------------------\n");
    for ( i = 0; i < 256; i++ ) {
        if ( !is_valid(dtu_vld_stat, i) )
            continue;

        llen += sprintf(str + llen, "0x%02x [%3u]   0x%02x      %d       %d     %3d        %5d      %5d\n",
                        i, i,
                        DTU_STAT_INFO[i].time_stamp,
                        DTU_STAT_INFO[i].complete,
                        DTU_STAT_INFO[i].bad,
                        DTU_STAT_INFO[i].cell_cnt,
                        DTU_STAT_INFO[i].dtu_rd_ptr,
                        DTU_STAT_INFO[i].dtu_wr_ptr );

        if ( len <= off && len + llen > off ) {
            memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_RETX_STATS_OVERRUN_END;
        llen = 0;
    }


    llen += sprintf(str + llen, "\n");
    llen += sprintf(str + llen, "Playout buffer status --- valid status [0x%08x]:\n", (unsigned int)PB_BUFFER_USAGE);
    for( i = 0; i <  RETX_MODE_CFG->buff_size; i += 8 ) {
        llen += sprintf(str + llen, "%08X: %08X %08X %08X %08X    %08X %08X %08X %08X\n",
                        (unsigned int)PB_BUFFER_USAGE + i * sizeof(unsigned int),
                        PB_BUFFER_USAGE[i],   PB_BUFFER_USAGE[i+1], PB_BUFFER_USAGE[i+2], PB_BUFFER_USAGE[i+3],
                        PB_BUFFER_USAGE[i+4], PB_BUFFER_USAGE[i+5], PB_BUFFER_USAGE[i+6], PB_BUFFER_USAGE[i+7]);
    }

    if ( len <= off && len + llen > off ) {
        memcpy(pstr, str + off - len, len + llen - off);
        pstr += len + llen - off;
    }
    else if ( len > off ) {
        memcpy(pstr, str, llen);
        pstr += llen;
    }
    len += llen;
    if ( len >= len_max )
        goto PROC_READ_RETX_STATS_OVERRUN_END;
    llen = 0;


    *eof = 1;

    return len - off;

PROC_READ_RETX_STATS_OVERRUN_END:
    return len - llen - off;
}

static int proc_write_retx_stats(struct file *file, const char *buf, unsigned long count, void *data)
{
    char str[2048];
    char *p;

    int len, rlen;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

    if ( stricmp(p, "help") == 0 ) {
        printk("echo clear_pb > /proc/driver/ifx_atm/retx_stats \n");
        printk("   :clear context in playout buffer\n\n");
        printk("echo read_pb <pb_index> <cell_num> > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : read playout buffer contents\n\n");
        printk("echo read_[r|t]x_cb > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : read cell buffer\n\n");
        printk("echo clear_[r|t]x_cb > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : clear cell buffer\n\n");
        printk("echo read_bad_dtu_intf_rec > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : read bad dtu intrface information record\n\n");
        printk("echo clear_bad_dtu_intf_rec > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : clear bad dtu interface information record\n\n");
        printk("echo read_wrx_context [i] > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : clear bad dtu interface information record\n\n");
        printk("echo read_intf_rec > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : read interface info record buffer\n\n");
        printk("echo reinit_intf_rec > /proc/driver/ifx_atm/retx_stats\n");
        printk("   : reinit intf record, must be called before showtime\n\n");
    }
    else if ( stricmp(p, "reinit_intf_rec") == 0 ) {
        int i = 0;
        struct Retx_adsl_ppe_intf_rec rec[16];

        *DBG_DTU_INTF_WRPTR             = 0;
        *DBG_INTF_FCW_DUP_CNT           = 0;
        *DBG_INTF_SID_CHANGE_IN_DTU_CNT = 0;
        *DBG_INTF_LCW_DUP_CNT           = 0;

        *DBG_RFBI_DONE_INT_CNT          = 0;
        *DBG_RFBI_INTV0                 = 0;
        *DBG_RFBI_INTV1                 = 0;
        *DBG_RFBI_BC0_INVALID_CNT       = 0;
        *DBG_RFBI_LAST_T                = 0;
        *DBG_DREG_BEG_END               = 0;

        memset((void *) DBG_INTF_INFO(0), 0, sizeof(rec));
        for( i = 0; i < 16; i++ )
            DBG_INTF_INFO(i)->res1_1 = 1;
        DBG_INTF_INFO(15)->dtu_sid = 255;
    }
    else if ( stricmp(p, "read_intf_rec") == 0 ) {
        int i, cnt;
        unsigned int dtu_intf_wrptr, fcw_dup_cnt, sid_change_in_dtu_cnt, lcw_dup_cnt ;
        unsigned int rfbi_done_int_cnt, rfbi_intv0, rfbi_intv1, rfbi_bc0_invalid_cnt, dreg_beg_end;
        struct Retx_adsl_ppe_intf_rec rec[16];

        memcpy((void *) rec, (void *) DBG_INTF_INFO(0), sizeof(rec));

        dtu_intf_wrptr          = *DBG_DTU_INTF_WRPTR;
        fcw_dup_cnt             = *DBG_INTF_FCW_DUP_CNT;
        sid_change_in_dtu_cnt   = *DBG_INTF_SID_CHANGE_IN_DTU_CNT;
        lcw_dup_cnt             = *DBG_INTF_LCW_DUP_CNT;

        rfbi_done_int_cnt       = *DBG_RFBI_DONE_INT_CNT;
        rfbi_intv0              = *DBG_RFBI_INTV0;
        rfbi_intv1              = *DBG_RFBI_INTV1;
        rfbi_bc0_invalid_cnt    = *DBG_RFBI_BC0_INVALID_CNT;
        dreg_beg_end            = *DBG_DREG_BEG_END;

        printk("PPE-Adsl Interface recrod [addr 0x23F0]:\n\n");

        printk("    rfbi_done_int_cnt   = %d [0x%x] \n", rfbi_done_int_cnt, rfbi_done_int_cnt);
        printk("    rfbi_intv           = 0x%08x  0x%08x [%d, %d, %d, %d, %d, %d, %d, %d]\n",
                    rfbi_intv0, rfbi_intv1,
                    rfbi_intv0 >> 24, (rfbi_intv0>>16) & 0xff, (rfbi_intv0>>8) & 0xff, rfbi_intv0 & 0xff,
                    rfbi_intv1 >> 24, (rfbi_intv1>>16) & 0xff, (rfbi_intv1>>8) & 0xff, rfbi_intv1 & 0xff
                    );
        printk("    rfbi_bc0_invld_cnt  = %d\n", rfbi_bc0_invalid_cnt);
        printk("    dreg_beg_end        = %d, %d\n\n", dreg_beg_end >> 16, dreg_beg_end & 0xffff);

        printk("    wrptr       = %d [0x%x] \n", dtu_intf_wrptr, dtu_intf_wrptr);
        printk("    fcw_dup_cnt = %d\n", fcw_dup_cnt);
        printk("    sid_chg_cnt = %d\n", sid_change_in_dtu_cnt);
        printk("    lcw_dup_cnt = %d\n\n", lcw_dup_cnt);


        printk("    idx  itf_dw0  itf_dw1  dtu_sid  timestamp  local_time   res1  last_cw  bad_flag  reinit\n");
        printk("    -------------------------------------------------------------------------------------\n");
        for ( i = (dtu_intf_wrptr + 1) % 16, cnt = 0; cnt < 16; cnt ++, i = (i + 1) % 16 ) {
            if(cnt < 15)
                printk("    ");
            else
                printk("   *");
            printk("%3d    %04x    %04x    %3d[%02x]   %3d[%02x]     %3d[%02x]    0x%02x       %d       %d       %d\n",
                i,
                (*(unsigned int *)&rec[i]) & 0xffff,
                (*(unsigned int *)&rec[i]) >> 16,
                rec[i].dtu_sid, rec[i].dtu_sid,
                rec[i].dtu_timestamp, rec[i].dtu_timestamp,
                rec[i].local_time, rec[i].local_time,
                rec[i].res1_1,
                rec[i].is_last_cw,
                rec[i].is_bad_cw,
                rec[i].reinit_flag );
        }
    }
    else if ( stricmp(p, "read_wrx_context") == 0 ) {
        int i = 0;
        int flag = 0;
        for( i = 0; i < 8; ++i ) {
            if ( !WRX_QUEUE_CONTEXT(i)->curr_des0 || !WRX_QUEUE_CONTEXT(i)->curr_des1 )
                continue;

            flag = 1;
            printk("WRX queue context [ %d ]: \n", i);
            printk("    curr_len = %4d, mfs = %d, ec = %d, clp1 = %d, aal5dp = %d\n",
                    WRX_QUEUE_CONTEXT(i)->curr_len, WRX_QUEUE_CONTEXT(i)->mfs,
                    WRX_QUEUE_CONTEXT(i)->ec, WRX_QUEUE_CONTEXT(i)->clp1,
                    WRX_QUEUE_CONTEXT(i)->aal5dp);
            printk("    initcrc  = %08x\n", WRX_QUEUE_CONTEXT(i)->intcrc);
            printk("    currdes  = %08x %08x\n",
                    WRX_QUEUE_CONTEXT(i)->curr_des0, WRX_QUEUE_CONTEXT(i)->curr_des1);
            printk("    last_dw  = %08x\n\n", WRX_QUEUE_CONTEXT(i)->last_dword);
            if( WRX_QUEUE_CONTEXT(i)->curr_len ) {
                int j = 0;
                unsigned char *p_char;
                struct rx_descriptor *desc = (struct rx_descriptor *)&(WRX_QUEUE_CONTEXT(i)->curr_des0);
                p_char = (unsigned char *)(((unsigned int)desc->dataptr << 2) | KSEG1);
                printk("    Data in SDRAM:\n        ");

                for ( j = 0 ; j < WRX_QUEUE_CONTEXT(i)->curr_len; ++j ) {
                    printk ("%02x", p_char[j]);
                    if ( j % 16 == 15 )
                        printk("\n        ");
                    else if ( j % 4 == 3 )
                        printk (" ");
                }
                printk("\n\n");
            }
        }
        if ( !flag ) {
            printk("No active wrx queue context\n");
        }
    }
    else if ( stricmp(p, "clear_pb") == 0 ) {
        if ( g_retx_playout_buffer )
            memset((void *)g_retx_playout_buffer, 0,  RETX_PLAYOUT_BUFFER_SIZE);
    }
    else if ( stricmp(p, "read_bad_dtu_intf_rec") == 0 ) {
        struct Retx_adsl_ppe_intf first_dtu_intf, last_dtu_intf;
        first_dtu_intf = *FIRST_BAD_REC_RETX_ADSL_PPE_INTF;
        last_dtu_intf = *BAD_REC_RETX_ADSL_PPE_INTF;

        printk("\nAdsl-PPE Interface for first and last DTU of recent noise:\n\n");
        printk("  dtu_sid                = 0x%02x [%3u], 0x%02x [%3u]\n",
                first_dtu_intf.dtu_sid, first_dtu_intf.dtu_sid,
                last_dtu_intf.dtu_sid, last_dtu_intf.dtu_sid);
        printk("  dtu_timestamp          = 0x%02x      , 0x%02x\n",
                first_dtu_intf.dtu_timestamp, last_dtu_intf.dtu_timestamp);
        printk("  local_time             = 0x%02x      , 0x%02x\n",
                first_dtu_intf.local_time, last_dtu_intf.local_time);
        printk("  is_last_cw             = %u          , %u\n",
                first_dtu_intf.is_last_cw, last_dtu_intf.is_last_cw);
        printk("  reinit_flag            = %u          , %u\n",
                first_dtu_intf.reinit_flag, last_dtu_intf.reinit_flag);
        printk("  is_bad_cw              = %u          , %u\n\n",
                first_dtu_intf.is_bad_cw, last_dtu_intf.is_bad_cw);
    }
    else if ( stricmp(p, "clear_bad_dtu_intf_rec") == 0 ) {
        memset((void *)BAD_REC_RETX_ADSL_PPE_INTF, 0, sizeof(struct Retx_adsl_ppe_intf));
        memset((void *)FIRST_BAD_REC_RETX_ADSL_PPE_INTF, 0, sizeof(struct Retx_adsl_ppe_intf));
    }
    else if ( stricmp(p, "clear_tx_cb") == 0 ) {
        unsigned int *dbase0;
        unsigned int pnum0;

        dbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( FFSM_DBA(0)->dbase + 0x2000);
        pnum0 = FFSM_CFG(0)->pnum;
        memset(dbase0, 0,  14 * sizeof(unsigned int ) * pnum0);
    }
    else if ( stricmp(p, "clear_rx_cb") == 0 ) {
        unsigned int *dbase0, *cbase0, *dbase1, *cbase1;
        unsigned int pnum0;

        dbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_DBA(0)->dbase + 0x2000);
        cbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_CBA(0)->cbase + 0x2000);

        dbase1 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_DBA(1)->dbase + 0x2000);
        cbase1 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_CBA(1)->cbase + 0x2000);

        pnum0 = SFSM_CFG(0)->pnum;

        memset(dbase0, 0,  14 * sizeof(unsigned int ) * pnum0);
        memset(cbase0, 0,  sizeof(unsigned int ) * pnum0);

        memset(dbase1, 0,  14 * sizeof(unsigned int ));
        memset(cbase1, 0,  sizeof(unsigned int ));
    }
    else if ( strnicmp(p, "read_tx_cb", 10) == 0 ) {
        unsigned int *dbase0;
        unsigned int pnum0, i;
        unsigned int * cell;

        dbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( FFSM_DBA(0)->dbase + 0x2000);
        pnum0 = FFSM_CFG(0)->pnum;

        printk("ATM TX BC 0 CELL data/ctrl buffer:\n\n");
        for(i = 0; i < pnum0 ; ++ i) {
            cell = dbase0 + i * 14;
            printk("cell %2d:                   %08x %08x\n", i, cell[0], cell[1]);
            printk("         %08x %08x %08x %08x\n",  cell[2], cell[3], cell[4], cell[5]);
            printk("         %08x %08x %08x %08x\n",  cell[6], cell[7], cell[8], cell[9]);
            printk("         %08x %08x %08x %08x\n",  cell[10], cell[11], cell[12], cell[13]);
        }
    }
    else if ( strnicmp(p, "read_rx_cb", 10) == 0 ) {
        unsigned int *dbase0, *cbase0, *dbase1, *cbase1;
        unsigned int pnum0, i;
        unsigned int * cell;

        dbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_DBA(0)->dbase + 0x2000);
        cbase0 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_CBA(0)->cbase + 0x2000);

        dbase1 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_DBA(1)->dbase + 0x2000);
        cbase1 = (unsigned int *)PPM_INT_UNIT_ADDR( SFSM_CBA(1)->cbase + 0x2000);

        pnum0 = SFSM_CFG(0)->pnum;

        printk("ATM RX BC 0 CELL data/ctrl buffer:\n\n");
        for(i = 0; i < pnum0 ; ++ i) {
            struct Retx_ctrl_field * p_ctrl;

            cell = dbase0 + i * 14;
            p_ctrl = (struct Retx_ctrl_field *) ( &cbase0[i]);
            printk("cell %2d:                   %08x %08x  -- [%08x]:", i, cell[0], cell[1], cbase0[i]);

            printk("l2_drop: %d, retx: %d", p_ctrl->l2_drop, p_ctrl->retx);
            if ( p_ctrl->retx ) {
                printk(", dtu_sid = %u, cell_sid = %u", p_ctrl->dtu_sid, p_ctrl->cell_sid);
            }

            printk("\n");

            printk("         %08x %08x %08x %08x\n",  cell[2], cell[3], cell[4], cell[5]);
            printk("         %08x %08x %08x %08x\n",  cell[6], cell[7], cell[8], cell[9]);
            printk("         %08x %08x %08x %08x\n",  cell[10], cell[11], cell[12], cell[13]);
        }

        printk("\n");
        printk("ATM RX BC 1 CELL data/ctrl buffer:\n\n");
        cell = dbase1;
        printk("cell %2d:                   %08x %08x  -- [%08x]: dtu_sid:%3d, cell_sid:%3d, next_ptr: %4d\n",
                0, cell[0], cell[1], cbase0[i], ( cell[1] >> 16) & 0xff, (cell[1] >> 24) & 0xff,    cell[1] & 0xffff );
        printk("         %08x %08x %08x %08x\n",  cell[2], cell[3], cell[4], cell[5]);
        printk("         %08x %08x %08x %08x\n",  cell[6], cell[7], cell[8], cell[9]);
        printk("         %08x %08x %08x %08x\n",  cell[10], cell[11], cell[12], cell[13]);
    }
    else if ( strnicmp(p, "read_pb ", 8) == 0 )
    {
        int start_cell_idx = 0;
        int cell_num = 0;
        unsigned int *cell;
        unsigned int pb_buff_size = RETX_MODE_CFG->buff_size * 32;

        p += 8;
        rlen -= 8;
        ignore_space(&p, &rlen);

        start_cell_idx = get_number(&p, &rlen, 0);
        ignore_space(&p, &rlen);
        cell_num = get_number(&p, &rlen, 0);

        if ( start_cell_idx >= pb_buff_size ) {
            printk(" Invalid cell index\n");
        }
        else {
            int i;
            if ( cell_num < 0 )
                cell_num = 1;

            if ( cell_num + start_cell_idx > pb_buff_size )
                cell_num = pb_buff_size - start_cell_idx;

            for ( i = 0; i < cell_num ; ++i ) {
                cell = (unsigned int *)((unsigned int *)g_retx_playout_buffer + (14 * (start_cell_idx + i)));
                printk("cell %4d:          %08x %08x [next_ptr = %4u, dtu_sid = %3u, cell_sid = %3u]\n",
                        start_cell_idx + i, cell[0], cell[1], cell[1] & 0xffff, (cell[1] >> 16) & 0xff, (cell[1] >> 24) & 0xff);
                printk("  %08x %08x %08x %08x\n",  cell[2], cell[3], cell[4], cell[5]);
                printk("  %08x %08x %08x %08x\n",  cell[6], cell[7], cell[8], cell[9]);
                printk("  %08x %08x %08x %08x\n",  cell[10], cell[11], cell[12], cell[13]);
            }
        }
    }

    return count;
}

static int proc_read_retx_cfg(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "ReTX FW Config:\n");
    len += sprintf(page + off + len, "  RETX_MODE_CFG            = 0x%08x, invld_range=%u, buff_size=%u, retx=%u\n", *(volatile unsigned int *)RETX_MODE_CFG, (unsigned int)RETX_MODE_CFG->invld_range, (unsigned int)RETX_MODE_CFG->buff_size * 32, (unsigned int)RETX_MODE_CFG->retx_en);
    len += sprintf(page + off + len, "  RETX_TSYNC_CFG           = 0x%08x, fw_alpha=%u, sync_inp=%u\n", *(volatile unsigned int *)RETX_TSYNC_CFG, (unsigned int)RETX_TSYNC_CFG->fw_alpha, (unsigned int)RETX_TSYNC_CFG->sync_inp);
    len += sprintf(page + off + len, "  RETX_TD_CFG              = 0x%08x, td_max=%u, td_min=%u\n", *(volatile unsigned int *)RETX_TD_CFG, (unsigned int)RETX_TD_CFG->td_max, (unsigned int)RETX_TD_CFG->td_min);
    len += sprintf(page + off + len, "  RETX_PLAYOUT_BUFFER_BASE = 0x%08x\n", *RETX_PLAYOUT_BUFFER_BASE);
    len += sprintf(page + off + len, "  RETX_SERVICE_HEADER_CFG  = 0x%08x\n", *RETX_SERVICE_HEADER_CFG);
    len += sprintf(page + off + len, "  RETX_MASK_HEADER_CFG     = 0x%08x\n", *RETX_MASK_HEADER_CFG);
    len += sprintf(page + off + len, "  RETX_MIB_TIMER_CFG       = 0x%08x, tick_cycle = %d, ticks_per_sec = %d\n",
                    *(unsigned int *)RETX_MIB_TIMER_CFG, RETX_MIB_TIMER_CFG->tick_cycle, RETX_MIB_TIMER_CFG->ticks_per_sec);

    *eof = 1;

    return len;
}

static int proc_write_retx_cfg(struct file *file, const char *buf, unsigned long count, void *data)
{
    char *p1, *p2;
    int len;
    int colon;
    char local_buf[1024];
    char *tokens[4] = {0};
    unsigned int token_num = 0;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;

    p1 = local_buf;
    colon = 0;
    while ( token_num < NUM_ENTITY(tokens) && get_token(&p1, &p2, &len, &colon) ) {
        tokens[token_num++] = p1;

        p1 = p2;
    }

    if ( token_num > 0 ) {
        if ( stricmp(tokens[0], "help") == 0 ) {
            printk("echo help > /proc/driver/ifx_atm/retx_cfg ==> \n\tprint this help message\n\n");

            printk("echo set retx <enable|disable|0|1|on|off> > /proc/driver/ifx_atm/retx_cfg\n");
            printk("\t:enable or disable retx feature\n\n");

            printk("echo set <td_max|td_min|fw_alpha|sync_inp|invld_range|buff_size> <number> > /proc/driver/ifx_atm/retx_cfg\n");
            printk("\t: set td_max, td_min, fw_alpha, sync_inp, invalid_range, buff_size\n\n");

            printk("echo set <service_header|service_mask> <hex_number> /proc/driver/ifx_atm/retx_cfg \n");
            printk("\t: set service_header, service_mask\n\n");
        }
        else if ( stricmp(tokens[0], "set") == 0 && token_num >= 3 ) {

            if ( stricmp(tokens[1], "retx") == 0 ) {
                if ( stricmp(tokens[2], "enable") == 0 ||
                     stricmp(tokens[2], "on") == 0 ||
                     stricmp(tokens[2], "1") == 0 )
                    RETX_MODE_CFG->retx_en = 1;
                else if ( stricmp(tokens[2], "disable") == 0 ||
                     stricmp(tokens[2], "off") == 0 ||
                     stricmp(tokens[2], "0") == 0 )
                    RETX_MODE_CFG->retx_en = 0;
                printk("RETX_MODE_CFG->retx_en - %d\n", RETX_MODE_CFG->retx_en);
            }
            else {
                unsigned int dec_val, hex_val;

                p1 = tokens[2];
                dec_val = (unsigned int)get_number(&p1, NULL, 0);
                p2 = tokens[2];
                hex_val = (unsigned int)get_number(&p2, NULL, 1);

                if ( *p2 == 0 ) {
                    if ( stricmp(tokens[1], "service_header") == 0 ) {
                        *RETX_SERVICE_HEADER_CFG = hex_val;
                        printk("RETX_SERVICE_HEADER_CFG - 0x%08x\n", *RETX_SERVICE_HEADER_CFG);
                    }
                    else if ( stricmp(tokens[1], "service_mask") == 0 ) {
                        *RETX_MASK_HEADER_CFG = hex_val;
                        printk("RETX_MASK_HEADER_CFG - 0x%08x\n", *RETX_MASK_HEADER_CFG);
                    }
                }
                if ( *p1 == 0 ) {
                    if ( stricmp(tokens[1], "td_max") == 0 ) {
                        (unsigned int)RETX_TD_CFG->td_max = (dec_val >= 0xff ? 0Xff : dec_val);
                        printk("RETX_TD_CFG->td_max - %d\n", RETX_TD_CFG->td_max);
                    }
                    else if ( stricmp(tokens[1], "td_min") == 0 ) {
                        (unsigned int)RETX_TD_CFG->td_min = (dec_val >= 0xff ? 0Xff : dec_val);
                        printk("RETX_TD_CFG->td_min - %d\n", RETX_TD_CFG->td_min);
                    }
                    else if ( stricmp(tokens[1], "fw_alpha") == 0 ) {
                        RETX_TSYNC_CFG->fw_alpha = dec_val >= 0x7FFE ? 0X7EEE : dec_val;
                        printk("RETX_TSYNC_CFG->fw_alpha - %d\n", RETX_TSYNC_CFG->fw_alpha);
                    }
                    else if ( stricmp(tokens[1], "sync_inp") == 0 ) {
                        RETX_TSYNC_CFG->sync_inp = dec_val >= 0x7FFE ? 0X7EEE : dec_val;
                        printk("RETX_TSYNC_CFG->sync_inp - %d\n", RETX_TSYNC_CFG->sync_inp);
                    }
                    else if ( stricmp(tokens[1], "invld_range") == 0 ) {
                        RETX_MODE_CFG->invld_range = dec_val >= 250 ? 250 : dec_val;
                        printk("RETX_MODE_CFG->invld_range - %d\n", RETX_MODE_CFG->invld_range);
                    }
                    else if ( stricmp(tokens[1], "buff_size") == 0 ) {
                        dec_val = (dec_val + 31) / 32;
                        RETX_MODE_CFG->buff_size = dec_val >= 4096 / 32 ? 4096 / 32 : dec_val;
                        printk("RETX_MODE_CFG->buff_size - %d\n", RETX_MODE_CFG->buff_size);
                    }
                }
            }

        }
    }

    return count;
}

static int proc_read_retx_dsl_param(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off + len, "DSL Param [timestamp %ld.%ld]:\n", g_retx_polling_start.tv_sec, g_retx_polling_start.tv_usec);

    if ( g_xdata_addr == NULL )
        len += sprintf(page + off + len, "  DSL parameters not available !\n");
    else {
        volatile struct dsl_param *p_dsl_param = (volatile struct dsl_param *)g_xdata_addr;

        len += sprintf(page + off + len, "  update_flag             = %u\n",     p_dsl_param->update_flag);
        len += sprintf(page + off + len, "  MinDelayrt              = %u\n",     p_dsl_param->MinDelayrt);
        len += sprintf(page + off + len, "  MaxDelayrt              = %u\n",     p_dsl_param->MaxDelayrt);
        len += sprintf(page + off + len, "  RetxEnable              = %u\n",     p_dsl_param->RetxEnable);
        len += sprintf(page + off + len, "  ServiceSpecificReTx     = %u\n",     p_dsl_param->ServiceSpecificReTx);
        len += sprintf(page + off + len, "  ReTxPVC                 = 0x%08x\n", p_dsl_param->ReTxPVC);
        len += sprintf(page + off + len, "  RxDtuCorruptedCNT       = %u\n",     p_dsl_param->RxDtuCorruptedCNT);
        len += sprintf(page + off + len, "  RxRetxDtuUnCorrectedCNT = %u\n",     p_dsl_param->RxRetxDtuUnCorrectedCNT);
        len += sprintf(page + off + len, "  RxLastEFB               = %u\n",     p_dsl_param->RxLastEFB);
        len += sprintf(page + off + len, "  RxDtuCorrectedCNT       = %u\n",     p_dsl_param->RxDtuCorrectedCNT);
    }
    if ( g_retx_polling_end.tv_sec != 0 || g_retx_polling_end.tv_usec != 0 ) {
        unsigned long polling_time_usec;

        polling_time_usec = (g_retx_polling_end.tv_sec - g_retx_polling_start.tv_sec) * 1000000 + (g_retx_polling_end.tv_usec - g_retx_polling_start.tv_usec);
        len += sprintf(page + off + len, "DSL Param Update Time: %lu.%03lums\n", polling_time_usec / 1000, polling_time_usec % 1000);
    }

    return len;
}

 #endif

#endif

static int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 ) {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC

static int strincmp(const char *p1, const char *p2, int n)
{
    int c1 = 0, c2;

    while ( n && *p1 && *p2 ) {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
        n--;
    }

    return n ? *p1 - *p2 : c1;
}

static int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;

    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z') || (**p1 >= '0' && **p1<= '9')) )
    {
        (*p1)++;
        (*len)--;
    }
    if ( !*len )
        return 0;

    if ( *colon )
    {
        *colon = 0;
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            if ( **p2 == ':' )
            {
                *colon = 1;
                break;
            }
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }
    else
    {
        *p2 = *p1;
        while ( *len && **p2 > ' ' && **p2 != ',' )
        {
            (*p2)++;
            (*len)--;
            tlen++;
        }
        **p2 = 0;
    }

    return tlen;
}

static unsigned int get_number(char **p, int *len, int is_hex)
{
    unsigned int ret = 0;
    unsigned int n = 0;

    if ( (*p)[0] == '0' && (*p)[1] == 'x' )
    {
        is_hex = 1;
        (*p) += 2;
        if ( len )
            (*len) -= 2;
    }

    if ( is_hex )
    {
        while ( (!len || *len) && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) )
        {
            if ( **p >= '0' && **p <= '9' )
                n = **p - '0';
            else if ( **p >= 'a' && **p <= 'f' )
               n = **p - 'a' + 10;
            else if ( **p >= 'A' && **p <= 'F' )
                n = **p - 'A' + 10;
            ret = (ret << 4) | n;
            (*p)++;
            if ( len )
                (*len)--;
        }
    }
    else
    {
        while ( (!len || *len) && **p >= '0' && **p <= '9' )
        {
            n = **p - '0';
            ret = ret * 10 + n;
            (*p)++;
            if ( len )
                (*len)--;
        }
    }

    return ret;
}

static void ignore_space(char **p, int *len)
{
    while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') )
    {
        (*p)++;
        (*len)--;
    }
}

#endif

static INLINE int ifx_atm_version(char *buf)
{
    int len = 0;
    unsigned int major, minor;

    ifx_atm_get_fw_ver(&major, &minor);

    len += sprintf(buf + len, "    ATM (A1) firmware version %d.%d.%d\n", IFX_ATM_VER_MAJOR, IFX_ATM_VER_MID,IFX_ATM_VER_MINOR);

    return len;
}

static INLINE void check_parameters(void)
{
    /*  Please refer to Amazon spec 15.4 for setting these values.  */
    if ( qsb_tau < 1 )
        qsb_tau = 1;
    if ( qsb_tstep < 1 )
        qsb_tstep = 1;
    else if ( qsb_tstep > 4 )
        qsb_tstep = 4;
    else if ( qsb_tstep == 3 )
        qsb_tstep = 2;

    /*  There is a delay between PPE write descriptor and descriptor is       */
    /*  really stored in memory. Host also has this delay when writing        */
    /*  descriptor. So PPE will use this value to determine if the write      */
    /*  operation makes effect.                                               */
    if ( write_descriptor_delay < 0 )
        write_descriptor_delay = 0;

    if ( aal5_fill_pattern < 0 )
        aal5_fill_pattern = 0;
    else
        aal5_fill_pattern &= 0xFF;

    /*  Because of the limitation of length field in descriptors, the packet  */
    /*  size could not be larger than 64K minus overhead size.                */
    if ( aal5r_max_packet_size < 0 )
        aal5r_max_packet_size = 0;
    else if ( aal5r_max_packet_size >= 65535 - MAX_RX_FRAME_EXTRA_BYTES )
        aal5r_max_packet_size = 65535 - MAX_RX_FRAME_EXTRA_BYTES;
    if ( aal5r_min_packet_size < 0 )
        aal5r_min_packet_size = 0;
    else if ( aal5r_min_packet_size > aal5r_max_packet_size )
        aal5r_min_packet_size = aal5r_max_packet_size;
    if ( aal5s_max_packet_size < 0 )
        aal5s_max_packet_size = 0;
    else if ( aal5s_max_packet_size >= 65535 - MAX_TX_FRAME_EXTRA_BYTES )
        aal5s_max_packet_size = 65535 - MAX_TX_FRAME_EXTRA_BYTES;
    if ( aal5s_min_packet_size < 0 )
        aal5s_min_packet_size = 0;
    else if ( aal5s_min_packet_size > aal5s_max_packet_size )
        aal5s_min_packet_size = aal5s_max_packet_size;

    if ( dma_rx_descriptor_length < 2 )
        dma_rx_descriptor_length = 2;
    if ( dma_tx_descriptor_length < 2 )
        dma_tx_descriptor_length = 2;
    if ( dma_rx_clp1_descriptor_threshold < 0 )
        dma_rx_clp1_descriptor_threshold = 0;
    else if ( dma_rx_clp1_descriptor_threshold > dma_rx_descriptor_length )
        dma_rx_clp1_descriptor_threshold = dma_rx_descriptor_length;

    if ( dma_tx_descriptor_length < 2 )
        dma_tx_descriptor_length = 2;
}

static INLINE int init_priv_data(void)
{
    void *p;
    int i;
    struct rx_descriptor rx_desc = {0};
    struct sk_buff *skb;
    volatile struct tx_descriptor *p_tx_desc;
    struct sk_buff **ppskb;

    //  clear atm private data structure
    memset(&g_atm_priv_data, 0, sizeof(g_atm_priv_data));

    //  allocate memory for RX (AAL) descriptors
    p = kzalloc(dma_rx_descriptor_length * sizeof(struct rx_descriptor) + DESC_ALIGNMENT, GFP_KERNEL);
    if ( p == NULL )
        return IFX_ERROR;
    dma_cache_wback_inv((unsigned long)p, dma_rx_descriptor_length * sizeof(struct rx_descriptor) + DESC_ALIGNMENT);
    g_atm_priv_data.aal_desc_base = p;
    p = (void *)((((unsigned int)p + DESC_ALIGNMENT - 1) & ~(DESC_ALIGNMENT - 1)) | KSEG1);
    g_atm_priv_data.aal_desc = (volatile struct rx_descriptor *)p;

    //  allocate memory for RX (OAM) descriptors
    p = kzalloc(RX_DMA_CH_OAM_DESC_LEN * sizeof(struct rx_descriptor) + DESC_ALIGNMENT, GFP_KERNEL);
    if ( p == NULL )
        return IFX_ERROR;
    dma_cache_wback_inv((unsigned long)p, RX_DMA_CH_OAM_DESC_LEN * sizeof(struct rx_descriptor) + DESC_ALIGNMENT);
    g_atm_priv_data.oam_desc_base = p;
    p = (void *)((((unsigned int)p + DESC_ALIGNMENT - 1) & ~(DESC_ALIGNMENT - 1)) | KSEG1);
    g_atm_priv_data.oam_desc = (volatile struct rx_descriptor *)p;

    //  allocate memory for RX (OAM) buffer
    p = kzalloc(RX_DMA_CH_OAM_DESC_LEN * RX_DMA_CH_OAM_BUF_SIZE + DATA_BUFFER_ALIGNMENT, GFP_KERNEL);
    if ( p == NULL )
        return IFX_ERROR;
    dma_cache_wback_inv((unsigned long)p, RX_DMA_CH_OAM_DESC_LEN * RX_DMA_CH_OAM_BUF_SIZE + DATA_BUFFER_ALIGNMENT);
    g_atm_priv_data.oam_buf_base = p;
    p = (void *)(((unsigned int)p + DATA_BUFFER_ALIGNMENT - 1) & ~(DATA_BUFFER_ALIGNMENT - 1));
    g_atm_priv_data.oam_buf = p;

    //  allocate memory for TX descriptors
    p = kzalloc(MAX_PVC_NUMBER * dma_tx_descriptor_length * sizeof(struct tx_descriptor) + DESC_ALIGNMENT, GFP_KERNEL);
    if ( p == NULL )
        return IFX_ERROR;
    dma_cache_wback_inv((unsigned long)p, MAX_PVC_NUMBER * dma_tx_descriptor_length * sizeof(struct tx_descriptor) + DESC_ALIGNMENT);
    g_atm_priv_data.tx_desc_base = p;

    //  allocate memory for TX skb pointers
    p = kzalloc(MAX_PVC_NUMBER * dma_tx_descriptor_length * sizeof(struct sk_buff *) + 4, GFP_KERNEL);
    if ( p == NULL )
        return IFX_ERROR;
    dma_cache_wback_inv((unsigned long)p, MAX_PVC_NUMBER * dma_tx_descriptor_length * sizeof(struct sk_buff *) + 4);
    g_atm_priv_data.tx_skb_base = p;

    //  setup RX (AAL) descriptors
    rx_desc.own     = 1;
    rx_desc.c       = 0;
    rx_desc.sop     = 1;
    rx_desc.eop     = 1;
    rx_desc.byteoff = 0;
    rx_desc.id      = 0;
    rx_desc.err     = 0;
    rx_desc.datalen = RX_DMA_CH_AAL_BUF_SIZE;
    for ( i = 0; i < dma_rx_descriptor_length; i++ ) {
        skb = alloc_skb_rx();
        if ( skb == NULL )
            return IFX_ERROR;
        rx_desc.dataptr = ((unsigned int)skb->data >> 2) & 0x0FFFFFFF;
        g_atm_priv_data.aal_desc[i] = rx_desc;
    }

    //  setup RX (OAM) descriptors
    p = (void *)((unsigned int)g_atm_priv_data.oam_buf | KSEG1);
    rx_desc.own     = 1;
    rx_desc.c       = 0;
    rx_desc.sop     = 1;
    rx_desc.eop     = 1;
    rx_desc.byteoff = 0;
    rx_desc.id      = 0;
    rx_desc.err     = 0;
    rx_desc.datalen = RX_DMA_CH_OAM_BUF_SIZE;
    for ( i = 0; i < RX_DMA_CH_OAM_DESC_LEN; i++ ) {
        rx_desc.dataptr = ((unsigned int)p >> 2) & 0x0FFFFFFF;
        g_atm_priv_data.oam_desc[i] = rx_desc;
        p = (void *)((unsigned int)p + RX_DMA_CH_OAM_BUF_SIZE);
    }

    //  setup TX descriptors and skb pointers
    p_tx_desc = (volatile struct tx_descriptor *)((((unsigned int)g_atm_priv_data.tx_desc_base + DESC_ALIGNMENT - 1) & ~(DESC_ALIGNMENT - 1)) | KSEG1);
    ppskb = (struct sk_buff **)(((unsigned int)g_atm_priv_data.tx_skb_base + 3) & ~3);
    for ( i = 0; i < MAX_PVC_NUMBER; i++ ) {
        g_atm_priv_data.conn[i].tx_desc = &p_tx_desc[i * dma_tx_descriptor_length];
        g_atm_priv_data.conn[i].tx_skb  = &ppskb[i * dma_tx_descriptor_length];
    }

    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        g_atm_priv_data.port[i].tx_max_cell_rate = DEFAULT_TX_LINK_RATE;

    return IFX_SUCCESS;
}

static INLINE void clear_priv_data(void)
{
    int i, j;
    struct sk_buff *skb;

    for ( i = 0; i < MAX_PVC_NUMBER; i++ ) {
        if ( g_atm_priv_data.conn[i].tx_skb != NULL ) {
            for ( j = 0; j < dma_tx_descriptor_length; j++ )
                if ( g_atm_priv_data.conn[i].tx_skb[j] != NULL )
                    dev_kfree_skb_any(g_atm_priv_data.conn[i].tx_skb[j]);
        }
    }

    if ( g_atm_priv_data.tx_skb_base != NULL )
        kfree(g_atm_priv_data.tx_skb_base);

    if ( g_atm_priv_data.tx_desc_base != NULL )
        kfree(g_atm_priv_data.tx_desc_base);

    if ( g_atm_priv_data.oam_buf_base != NULL )
        kfree(g_atm_priv_data.oam_buf_base);

    if ( g_atm_priv_data.oam_desc_base != NULL )
        kfree(g_atm_priv_data.oam_desc_base);

    if ( g_atm_priv_data.aal_desc_base != NULL ) {
        for ( i = 0; i < dma_rx_descriptor_length; i++ ) {
            if ( g_atm_priv_data.aal_desc[i].sop || g_atm_priv_data.aal_desc[i].eop ) { //  descriptor initialized
                skb = get_skb_rx_pointer(g_atm_priv_data.aal_desc[i].dataptr);
                dev_kfree_skb_any(skb);
            }
        }
        kfree(g_atm_priv_data.aal_desc_base);
    }
}

static INLINE void init_rx_tables(void)
{
    int i;
    struct wrx_queue_config wrx_queue_config = {0};
    struct wrx_dma_channel_config wrx_dma_channel_config = {0};
    struct htu_entry htu_entry = {0};
    struct htu_result htu_result = {0};
    struct htu_mask htu_mask = {    set:        0x01,
                                    clp:        0x01,
                                    pid_mask:   0x00,
                                    vpi_mask:   0x00,
                                    vci_mask:   0x00,
                                    pti_mask:   0x00,
                                    clear:      0x00};

    /*
     *  General Registers
     */
    *CFG_WRX_HTUTS  = MAX_PVC_NUMBER + OAM_HTU_ENTRY_NUMBER;
#ifndef CONFIG_AMAZON_SE
    *CFG_WRX_QNUM   = MAX_QUEUE_NUMBER;
#endif
    *CFG_WRX_DCHNUM = RX_DMA_CH_TOTAL;
    *WRX_DMACH_ON   = (1 << RX_DMA_CH_TOTAL) - 1;
    *WRX_HUNT_BITTH = DEFAULT_RX_HUNT_BITTH;

    /*
     *  WRX Queue Configuration Table
     */
    wrx_queue_config.uumask    = 0;
    wrx_queue_config.cpimask   = 0;
    wrx_queue_config.uuexp     = 0;
    wrx_queue_config.cpiexp    = 0;
    wrx_queue_config.mfs       = aal5r_max_packet_size;
    wrx_queue_config.oversize  = aal5r_max_packet_size;
    wrx_queue_config.undersize = aal5r_min_packet_size;
    wrx_queue_config.errdp     = aal5r_drop_error_packet;
    wrx_queue_config.dmach     = RX_DMA_CH_AAL;
    for ( i = 0; i < MAX_QUEUE_NUMBER; i++ )
        *WRX_QUEUE_CONFIG(i) = wrx_queue_config;
    WRX_QUEUE_CONFIG(OAM_RX_QUEUE)->dmach = RX_DMA_CH_OAM;

    /*
     *  WRX DMA Channel Configuration Table
     */
    wrx_dma_channel_config.chrl   = 0;
    wrx_dma_channel_config.clp1th = dma_rx_clp1_descriptor_threshold;
    wrx_dma_channel_config.mode   = 0;
    wrx_dma_channel_config.rlcfg  = 0;

    wrx_dma_channel_config.deslen = RX_DMA_CH_OAM_DESC_LEN;
    wrx_dma_channel_config.desba  = ((unsigned int)g_atm_priv_data.oam_desc >> 2) & 0x0FFFFFFF;
    *WRX_DMA_CHANNEL_CONFIG(RX_DMA_CH_OAM) = wrx_dma_channel_config;

    wrx_dma_channel_config.deslen = dma_rx_descriptor_length;
    wrx_dma_channel_config.desba  = ((unsigned int)g_atm_priv_data.aal_desc >> 2) & 0x0FFFFFFF;
    *WRX_DMA_CHANNEL_CONFIG(RX_DMA_CH_AAL) = wrx_dma_channel_config;

    /*
     *  HTU Tables
     */
    for ( i = 0; i < MAX_PVC_NUMBER; i++ )
    {
        htu_result.qid = (unsigned int)i;

        *HTU_ENTRY(i + OAM_HTU_ENTRY_NUMBER)  = htu_entry;
        *HTU_MASK(i + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
        *HTU_RESULT(i + OAM_HTU_ENTRY_NUMBER) = htu_result;
    }
    /*  OAM HTU Entry   */
    htu_entry.vci     = 0x03;
    htu_mask.pid_mask = 0x03;
    htu_mask.vpi_mask = 0xFF;
    htu_mask.vci_mask = 0x0000;
    htu_mask.pti_mask = 0x07;
    htu_result.cellid = OAM_RX_QUEUE;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = OAM_RX_QUEUE;
    *HTU_RESULT(OAM_F4_SEG_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_SEG_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_SEG_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x04;
    htu_result.cellid = OAM_RX_QUEUE;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = OAM_RX_QUEUE;
    *HTU_RESULT(OAM_F4_TOT_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F4_TOT_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F4_TOT_HTU_ENTRY)  = htu_entry;
    htu_entry.vci     = 0x00;
    htu_entry.pti     = 0x04;
    htu_mask.vci_mask = 0xFFFF;
    htu_mask.pti_mask = 0x01;
    htu_result.cellid = OAM_RX_QUEUE;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = OAM_RX_QUEUE;
    *HTU_RESULT(OAM_F5_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_F5_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_F5_HTU_ENTRY)  = htu_entry;
#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    htu_entry.pid     = 0x0;
    htu_entry.vpi     = 0x01;
    htu_entry.vci     = 0x0001;
    htu_entry.pti     = 0x00;
    htu_mask.pid_mask = 0x0;
    htu_mask.vpi_mask = 0x00;
    htu_mask.vci_mask = 0x0000;
    htu_mask.pti_mask = 0x3;
    htu_result.cellid = OAM_RX_QUEUE;
    htu_result.type   = 1;
    htu_result.ven    = 1;
    htu_result.qid    = OAM_RX_QUEUE;
    *HTU_RESULT(OAM_ARQ_HTU_ENTRY) = htu_result;
    *HTU_MASK(OAM_ARQ_HTU_ENTRY)   = htu_mask;
    *HTU_ENTRY(OAM_ARQ_HTU_ENTRY)  = htu_entry;
#endif
}

static INLINE void init_tx_tables(void)
{
    int i;
    struct wtx_queue_config wtx_queue_config = {0};
    struct wtx_dma_channel_config wtx_dma_channel_config = {0};
    struct wtx_port_config wtx_port_config = {  res1:   0,
                                                qid:    0,
                                                qsben:  1};

    /*
     *  General Registers
     */
    *CFG_WTX_DCHNUM     = MAX_TX_DMA_CHANNEL_NUMBER;
    *WTX_DMACH_ON       = ((1 << MAX_TX_DMA_CHANNEL_NUMBER) - 1) ^ ((1 << FIRST_QSB_QID) - 1);
    *CFG_WRDES_DELAY    = write_descriptor_delay;

    /*
     *  WTX Port Configuration Table
     */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        *WTX_PORT_CONFIG(i) = wtx_port_config;

    /*
     *  WTX Queue Configuration Table
     */
    wtx_queue_config.qsben = 1;
    wtx_queue_config.sbid  = 0;
    for ( i = 0; i < MAX_TX_DMA_CHANNEL_NUMBER; i++ ) {
        wtx_queue_config.qsb_vcid = i;
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;
    }

    /*
     *  WTX DMA Channel Configuration Table
     */
    wtx_dma_channel_config.mode   = 0;
    wtx_dma_channel_config.deslen = 0;
    wtx_dma_channel_config.desba  = 0;
    for ( i = 0; i < FIRST_QSB_QID; i++ )
        *WTX_DMA_CHANNEL_CONFIG(i) = wtx_dma_channel_config;
    /*  normal connection   */
    wtx_dma_channel_config.deslen = dma_tx_descriptor_length;
    for ( ; i < MAX_TX_DMA_CHANNEL_NUMBER ; i++ ) {
        wtx_dma_channel_config.desba = ((unsigned int)g_atm_priv_data.conn[i - FIRST_QSB_QID].tx_desc >> 2) & 0x0FFFFFFF;
        *WTX_DMA_CHANNEL_CONFIG(i) = wtx_dma_channel_config;
    }
}



/*
 * ####################################
 *           Global Function
 * ####################################
 */

static int atm_showtime_enter(struct port_cell_info *port_cell, void *xdata_addr)
{
    int i, j;

    ASSERT(port_cell != NULL, "port_cell is NULL");
    ASSERT(xdata_addr != NULL, "xdata_addr is NULL");

    for ( j = 0; j < ATM_PORT_NUMBER && j < port_cell->port_num; j++ )
        if ( port_cell->tx_link_rate[j] > 0 )
            break;
    for ( i = 0; i < ATM_PORT_NUMBER && i < port_cell->port_num; i++ )
        g_atm_priv_data.port[i].tx_max_cell_rate = port_cell->tx_link_rate[i] > 0 ? port_cell->tx_link_rate[i] : port_cell->tx_link_rate[j];

    qsb_global_set();

    for ( i = 0; i < MAX_PVC_NUMBER; i++ )
        if ( g_atm_priv_data.conn[i].vcc != NULL )
            set_qsb(g_atm_priv_data.conn[i].vcc, &g_atm_priv_data.conn[i].vcc->qos, i);

    //  TODO: ReTX set xdata_addr
    g_xdata_addr = xdata_addr;

    g_showtime = 1;

#if defined(CONFIG_VR9)
    IFX_REG_W32(0x0F, UTP_CFG);
#endif

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    if ( !timer_pending(&g_retx_polling_timer) ) {
        g_retx_polling_cnt = HZ;
        g_retx_polling_timer.expires = jiffies + RETX_POLLING_INTERVAL;
        add_timer(&g_retx_polling_timer);
    }
#endif

    //printk("enter showtime, cell rate: 0 - %d, 1 - %d, xdata addr: 0x%08x\n", g_atm_priv_data.port[0].tx_max_cell_rate, g_atm_priv_data.port[1].tx_max_cell_rate, (unsigned int)g_xdata_addr);

    return IFX_SUCCESS;
}

static int atm_showtime_exit(void)
{
    if ( !g_showtime )
        return IFX_ERROR;

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    RETX_MODE_CFG->retx_en = 0; //  disable ReTX
    del_timer(&g_retx_polling_timer);
#endif

#if defined(CONFIG_VR9)
    IFX_REG_W32(0x00, UTP_CFG);
#endif

    g_showtime = 0;

    //  TODO: ReTX clean state
    g_xdata_addr = NULL;

    printk("leave showtime\n");

    return IFX_SUCCESS;
}



/*
 * ####################################
 *           Init/Cleanup API
 * ####################################
 */

/*
 *  Description:
 *    Initialize global variables, PP32, comunication structures, register IRQ
 *    and register device.
 *  Input:
 *    none
 *  Output:
 *    0    --- successful
 *    else --- failure, usually it is negative value of error code
 */
static int __devinit ifx_atm_init(void)
{
    int ret;
    int port_num;
    struct port_cell_info port_cell = {0};
    int i, j;
    char ver_str[256];

    check_parameters();

    ret = init_priv_data();
    if ( ret != IFX_SUCCESS ) {
        err("INIT_PRIV_DATA_FAIL");
        goto INIT_PRIV_DATA_FAIL;
    }

    ifx_atm_init_chip();
    init_rx_tables();
    init_tx_tables();

    /*  create devices  */
    for ( port_num = 0; port_num < ATM_PORT_NUMBER; port_num++ ) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
	g_atm_priv_data.port[port_num].dev = atm_dev_register("ifxmips_atm", &g_ifx_atm_ops, -1, NULL);
#else
	g_atm_priv_data.port[port_num].dev = atm_dev_register("ifxmips_atm", NULL, &g_ifx_atm_ops, -1, NULL);
#endif

        if ( !g_atm_priv_data.port[port_num].dev ) {
            err("failed to register atm device %d!", port_num);
            ret = -EIO;
            goto ATM_DEV_REGISTER_FAIL;
        }
        else {
            g_atm_priv_data.port[port_num].dev->ci_range.vpi_bits = 8;
            g_atm_priv_data.port[port_num].dev->ci_range.vci_bits = 16;
            g_atm_priv_data.port[port_num].dev->link_rate = g_atm_priv_data.port[port_num].tx_max_cell_rate;
            g_atm_priv_data.port[port_num].dev->dev_data = (void*)port_num;
        }
    }

    /*  register interrupt handler  */
    ret = request_irq(PPE_MAILBOX_IGU1_INT, mailbox_irq_handler, IRQF_DISABLED, "atm_mailbox_isr", &g_atm_priv_data);
    if ( ret ) {
        if ( ret == -EBUSY ) {
            err("IRQ may be occupied by other driver, please reconfig to disable it.");
        }
        else {
            err("request_irq fail");
        }
        goto REQUEST_IRQ_PPE_MAILBOX_IGU1_INT_FAIL;
    }
    disable_irq(PPE_MAILBOX_IGU1_INT);

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    init_atm_tc_retrans_param();

    init_timer(&g_retx_polling_timer);
    g_retx_polling_timer.function = retx_polling_func;
#endif

    ret = ifx_pp32_start(0);
    if ( ret ) {
        err("ifx_pp32_start fail!");
        goto PP32_START_FAIL;
    }

    port_cell.port_num = ATM_PORT_NUMBER;
    if( !IS_ERR(&ifx_mei_atm_showtime_check) && &ifx_mei_atm_showtime_check)
        ifx_mei_atm_showtime_check(&g_showtime, &port_cell, &g_xdata_addr);
    if ( g_showtime ) {
        for ( i = 0; i < ATM_PORT_NUMBER; i++ )
            if ( port_cell.tx_link_rate[i] != 0 )
                break;
        for ( j = 0; j < ATM_PORT_NUMBER; j++ )
            g_atm_priv_data.port[j].tx_max_cell_rate = port_cell.tx_link_rate[j] != 0 ? port_cell.tx_link_rate[j] : port_cell.tx_link_rate[i];
    }

    qsb_global_set();
    validate_oam_htu_entry();

#if 0 /*defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK*/
    ifx_led_trigger_register("dsl_data", &g_data_led_trigger);
#endif

    /*  create proc file    */
    proc_file_create();

    if( !IS_ERR(&ifx_mei_atm_showtime_enter) && &ifx_mei_atm_showtime_enter )
        ifx_mei_atm_showtime_enter = atm_showtime_enter;

    if( !IS_ERR(&ifx_mei_atm_showtime_exit) && !ifx_mei_atm_showtime_exit )
        ifx_mei_atm_showtime_exit  = atm_showtime_exit;

    ifx_atm_version(ver_str);
    printk(KERN_INFO "%s", ver_str);

    printk("ifxmips_atm: ATM init succeed\n");

    return IFX_SUCCESS;

PP32_START_FAIL:
    free_irq(PPE_MAILBOX_IGU1_INT, &g_atm_priv_data);
REQUEST_IRQ_PPE_MAILBOX_IGU1_INT_FAIL:
ATM_DEV_REGISTER_FAIL:
    while ( port_num-- > 0 )
        atm_dev_deregister(g_atm_priv_data.port[port_num].dev);
INIT_PRIV_DATA_FAIL:
    clear_priv_data();
    printk("ifxmips_atm: ATM init failed\n");
    return ret;
}

/*
 *  Description:
 *    Release memory, free IRQ, and deregister device.
 *  Input:
 *    none
 *  Output:
 *   none
 */
static void __exit ifx_atm_exit(void)
{
    int port_num;

    if( !IS_ERR(&ifx_mei_atm_showtime_enter) && &ifx_mei_atm_showtime_enter )
        ifx_mei_atm_showtime_enter = NULL;
    if( !IS_ERR(&ifx_mei_atm_showtime_exit) && !ifx_mei_atm_showtime_exit )
        ifx_mei_atm_showtime_exit  = NULL;

    proc_file_delete();

#if 0 /*defined(ENABLE_LED_FRAMEWORK) && ENABLE_LED_FRAMEWORK*/
    ifx_led_trigger_deregister(g_data_led_trigger);
    g_data_led_trigger = NULL;
#endif

    invalidate_oam_htu_entry();

    ifx_pp32_stop(0);

#if defined(ENABLE_ATM_RETX) && ENABLE_ATM_RETX
    del_timer(&g_retx_polling_timer);
    clear_atm_tc_retrans_param();
#endif

    free_irq(PPE_MAILBOX_IGU1_INT, &g_atm_priv_data);

    for ( port_num = 0; port_num < ATM_PORT_NUMBER; port_num++ )
        atm_dev_deregister(g_atm_priv_data.port[port_num].dev);

    ifx_atm_uninit_chip();

    clear_priv_data();
}

module_init(ifx_atm_init);
module_exit(ifx_atm_exit);
MODULE_LICENSE("Dual BSD/GPL");
