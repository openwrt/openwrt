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
#define IFX_ATM_VER_MINOR               8



/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
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

/*
 *  Chip Specific Head File
 */
#include <lantiq_soc.h>
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

#define DUMP_SKB_LEN                          ~0



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
static INLINE int adsl_led_flash(void);

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
 *  Debug Functions
 */
#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
  static void dump_skb(struct sk_buff *, u32, char *, int, int, int);
#else
  #define dump_skb(skb, len, title, port, ch, is_tx)    do {} while (0)
#endif

/*
 *  Proc File Functions
 */
static INLINE void proc_file_create(void);
static INLINE void proc_file_delete(void);
static int proc_read_version(char *, char **, off_t, int, int *, void *);
static int proc_read_mib(char *, char **, off_t, int, int *, void *);
static int proc_write_mib(struct file *, const char *, unsigned long, void *);
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int proc_read_dbg(char *, char **, off_t, int, int *, void *);
  static int proc_write_dbg(struct file *, const char *, unsigned long, void *);
#endif
#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC
  static int proc_read_htu(char *, char **, off_t, int, int *, void *);
  static int proc_read_txq(char *, char **, off_t, int, int *, void *);
#endif

/*
 *  Proc Help Functions
 */
static int stricmp(const char *, const char *);
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
  static int strincmp(const char *, const char *, int);
#endif
static INLINE int ifx_atm_version(char *);
//static INLINE int print_reset_domain(char *, int);
//static INLINE int print_reset_handler(char *, int, ifx_rcu_handler_t *);

/*
 *  Init & clean-up functions
 */
#ifdef MODULE
  static INLINE void reset_ppe(void);
#endif
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
  extern int ifx_mei_atm_led_blink(void);
  extern int ifx_mei_atm_showtime_check(int *is_showtime, struct port_cell_info *port_cell, void **xdata_addr);
#else
  static inline int ifx_mei_atm_led_blink(void) { return IFX_SUCCESS; }
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
extern struct sk_buff* (*ifx_atm_alloc_tx)(struct atm_vcc *, unsigned int);
#if defined(CONFIG_IFXMIPS_DSL_CPE_MEI) || defined(CONFIG_IFXMIPS_DSL_CPE_MEI_MODULE)
  extern int (*ifx_mei_atm_showtime_enter)(struct port_cell_info *, void *);
  extern int (*ifx_mei_atm_showtime_exit)(void);
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
    .send_oam	= ppe_send_oam,
    .change_qos	= ppe_change_qos,
    .owner      = THIS_MODULE,
};

#if defined(ENABLE_TASKLET) && ENABLE_TASKLET
  DECLARE_TASKLET(g_dma_tasklet, do_ppe_tasklet, 0);
#endif

static int g_showtime = 0;
static void *g_xdata_addr = NULL;

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

    /*  check bandwidth */
    if ( (vcc->qos.txtp.traffic_class == ATM_CBR && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_VBR_RT && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_VBR_NRT && vcc->qos.txtp.scr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS && vcc->qos.txtp.min_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate)) )
    {
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

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
    WTX_QUEUE_CONFIG(conn)->sbid = (int)vcc->dev->dev_data;

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

    /*  release connection  */
    clear_bit(conn, &g_atm_priv_data.conn_table);
    connection->vcc = NULL;
    connection->aal5_vcc_crc_err = 0;
    connection->aal5_vcc_oversize_sdu = 0;

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
    g_atm_priv_data.conn[conn].tx_desc[desc_base] = reg_desc;
    dma_cache_wback((unsigned long)skb->data, skb->len);

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

static INLINE int adsl_led_flash(void)
{
    return ifx_mei_atm_led_blink();
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
    if ( is_tx ) {
        while ( MBOX_IGU3_ISR_ISR(queue + FIRST_QSB_QID + 16) );
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(queue + FIRST_QSB_QID + 16);
    }
    else {
        while ( MBOX_IGU3_ISR_ISR(queue) );
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(queue);
    }
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

#if defined(DEBUG_DUMP_SKB) && DEBUG_DUMP_SKB
static void dump_skb(struct sk_buff *skb, u32 len, char *title, int port, int ch, int is_tx)
{
    int i;

    if ( !(ifx_atm_dbg_enable & (is_tx ? DBG_ENABLE_MASK_DUMP_SKB_TX : DBG_ENABLE_MASK_DUMP_SKB_RX)) )
        return;

    if ( skb->len < len )
        len = skb->len;

    if ( len > RX_DMA_CH_AAL_BUF_SIZE ) {
        printk("too big data length: skb = %08x, skb->data = %08x, skb->len = %d\n", (u32)skb, (u32)skb->data, skb->len);
        return;
    }

    if ( ch >= 0 )
        printk("%s (port %d, ch %d)\n", title, port, ch);
    else
        printk("%s\n", title);
    printk("  skb->data = %08X, skb->tail = %08X, skb->len = %d\n", (u32)skb->data, (u32)skb->tail, (int)skb->len);
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

static INLINE void proc_file_create(void)
{
#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    struct proc_dir_entry *res;
#endif

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

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    res = create_proc_entry("dbg",
                            0,
                            g_atm_dir);
    if ( res != NULL ) {
        res->read_proc  = proc_read_dbg;
        res->write_proc = proc_write_dbg;
    }
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
#endif
}

static INLINE void proc_file_delete(void)
{
#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC
    remove_proc_entry("txq", g_atm_dir);

    remove_proc_entry("htu", g_atm_dir);
#endif

#if defined(ENABLE_DBG_PROC) && ENABLE_DBG_PROC
    remove_proc_entry("dbg", g_atm_dir);
#endif

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
        4
    };
    u32 dbg_enable_mask[] = {
        DBG_ENABLE_MASK_ERR,
        DBG_ENABLE_MASK_DEBUG_PRINT,
        DBG_ENABLE_MASK_ASSERT,
        DBG_ENABLE_MASK_DUMP_SKB_RX,
        DBG_ENABLE_MASK_DUMP_SKB_TX,
        DBG_ENABLE_MASK_DUMP_QOS,
        DBG_ENABLE_MASK_DUMP_INIT,
        DBG_ENABLE_MASK_ALL
    };

    char str[2048];
    char *p;

    int len, rlen;

    int f_enable = 0;
    int i;

    len = count < sizeof(str) ? count : sizeof(str) - 1;
    rlen = len - copy_from_user(str, buf, len);
    while ( rlen && str[rlen - 1] <= ' ' )
        rlen--;
    str[rlen] = 0;
    for ( p = str; *p && *p <= ' '; p++, rlen-- );
    if ( !*p )
        return 0;

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
                ifx_atm_dbg_enable |= DBG_ENABLE_MASK_ALL;
            else
                ifx_atm_dbg_enable &= ~DBG_ENABLE_MASK_ALL;
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

    return count;
}

#endif

#if defined(ENABLE_FW_PROC) && ENABLE_FW_PROC

static INLINE int print_htu(char *buf, int i)
{
    int len = 0;

    if ( HTU_ENTRY(i)->vld ) {
        len += sprintf(buf + len, "%2d. valid\n", i);
        len += sprintf(buf + len, "    entry  0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(u32*)HTU_ENTRY(i), HTU_ENTRY(i)->pid, HTU_ENTRY(i)->vpi, HTU_ENTRY(i)->vci, HTU_ENTRY(i)->pti);
        len += sprintf(buf + len, "    mask   0x%08x - pid %01x vpi %02x vci %04x pti %01x\n", *(u32*)HTU_MASK(i), HTU_MASK(i)->pid_mask, HTU_MASK(i)->vpi_mask, HTU_MASK(i)->vci_mask, HTU_MASK(i)->pti_mask);
        len += sprintf(buf + len, "    result 0x%08x - type: %s, qid: %d", *(u32*)HTU_RESULT(i), HTU_RESULT(i)->type ? "cell" : "AAL5", HTU_RESULT(i)->qid);
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
    char str[1024];
    int llen;

    int htuts = *CFG_WRX_HTUTS;
    int i;

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

    return len - off;

PROC_READ_HTU_OVERRUN_END:

    return len - llen - off;
}

static INLINE int print_tx_queue(char *buf, int i)
{
    int len = 0;

    if ( (*WTX_DMACH_ON & (1 << i)) ) {
        len += sprintf(buf + len, "%2d. valid\n", i);
        len += sprintf(buf + len, "    queue 0x%08x - sbid %u, qsb %s\n", *(u32*)WTX_QUEUE_CONFIG(i), (unsigned int)WTX_QUEUE_CONFIG(i)->sbid, WTX_QUEUE_CONFIG(i)->qsben ? "enable" : "disable");
        len += sprintf(buf + len, "    dma   0x%08x - base %08x, len %u, vlddes %u\n", *(u32*)WTX_DMA_CHANNEL_CONFIG(i), WTX_DMA_CHANNEL_CONFIG(i)->desba, WTX_DMA_CHANNEL_CONFIG(i)->deslen, WTX_DMA_CHANNEL_CONFIG(i)->vlddes);
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
    char str[1024];
    int llen;

    int i;

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

    return len - off;

PROC_READ_HTU_OVERRUN_END:

    return len - llen - off;
}

#endif

static int stricmp(const char *p1, const char *p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
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

    while ( n && *p1 && *p2 )
    {
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
#endif

static INLINE int ifx_atm_version(char *buf)
{
    int len = 0;
    unsigned int major, minor;

    ifx_atm_get_fw_ver(&major, &minor);

    len += sprintf(buf + len, "Infineon Technologies ATM driver version %d.%d.%d\n", IFX_ATM_VER_MAJOR, IFX_ATM_VER_MID, IFX_ATM_VER_MINOR);
    len += sprintf(buf + len, "Infineon Technologies ATM (A1) firmware version %d.%d\n", major, minor);

    return len;
}

#ifdef MODULE
static INLINE void reset_ppe(void)
{
    //  TODO:
}
#endif

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
    *CFG_WRX_QNUM   = MAX_QUEUE_NUMBER;
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
    wtx_queue_config.type  = 0x0;
    wtx_queue_config.qsben = 1;
    wtx_queue_config.sbid  = 0;
    for ( i = 0; i < MAX_TX_DMA_CHANNEL_NUMBER; i++ )
        *WTX_QUEUE_CONFIG(i) = wtx_queue_config;

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

    pr_debug("enter showtime, cell rate: 0 - %d, 1 - %d, xdata addr: 0x%08x\n", g_atm_priv_data.port[0].tx_max_cell_rate, g_atm_priv_data.port[1].tx_max_cell_rate, (unsigned int)g_xdata_addr);

    return IFX_SUCCESS;
}

static int atm_showtime_exit(void)
{
#if defined(CONFIG_VR9)
    IFX_REG_W32(0x00, UTP_CFG);
#endif

    g_showtime = 0;

    //  TODO: ReTX clean state
    g_xdata_addr = NULL;

    pr_debug("leave showtime\n");

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

#ifdef MODULE
    reset_ppe();
#endif

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

    ret = ifx_pp32_start(0);
    if ( ret ) {
        err("ifx_pp32_start fail!");
        goto PP32_START_FAIL;
    }

    port_cell.port_num = ATM_PORT_NUMBER;
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

    /*  create proc file    */
    proc_file_create();

    ifx_mei_atm_showtime_enter = atm_showtime_enter;
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

    ifx_mei_atm_showtime_enter = NULL;
    ifx_mei_atm_showtime_exit  = NULL;

    proc_file_delete();

    invalidate_oam_htu_entry();

    ifx_pp32_stop(0);

    free_irq(PPE_MAILBOX_IGU1_INT, &g_atm_priv_data);

    for ( port_num = 0; port_num < ATM_PORT_NUMBER; port_num++ )
        atm_dev_deregister(g_atm_priv_data.port[port_num].dev);

    ifx_atm_uninit_chip();

    clear_priv_data();
}

module_init(ifx_atm_init);
module_exit(ifx_atm_exit);
MODULE_LICENSE("Dual BSD/GPL");
