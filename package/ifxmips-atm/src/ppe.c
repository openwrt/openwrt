#include <asm/mach-ifxmips/cgu.h>
#include "common.h"

#include "ifx_ppe_fw.h"
static void set_qsb(struct atm_vcc *vcc, struct atm_qos *qos, unsigned int connection)
{

    u32 qsb_clk = cgu_get_fpi_bus_clock(2);         /*  FPI configuration 2 (slow FPI bus) */
	union qsb_queue_parameter_table qsb_queue_parameter_table = {{0}};
    union qsb_queue_vbr_parameter_table qsb_queue_vbr_parameter_table = {{0}};
    u32 tmp;

    /*
     *  Peak Cell Rate (PCR) Limiter
     */
    if ( qos->txtp.max_pcr == 0 )
        qsb_queue_parameter_table.bit.tp = 0;   /*  disable PCR limiter */
    else
    {
        /*  peak cell rate would be slightly lower than requested [maximum_rate / pcr = (qsb_clock / 8) * (time_step / 4) / pcr] */
        tmp = ((qsb_clk * ppe_dev.qsb.tstepc) >> 5) / qos->txtp.max_pcr + 1;
        /*  check if overflow takes place   */
        qsb_queue_parameter_table.bit.tp = tmp > QSB_TP_TS_MAX ? QSB_TP_TS_MAX : tmp;
    }
    /*
     *  Weighted Fair Queueing Factor (WFQF)
     */
    switch ( qos->txtp.traffic_class )
    {
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
    if ( qos->txtp.traffic_class == ATM_VBR_RT || qos->txtp.traffic_class == ATM_VBR_NRT )
    {
        if ( qos->txtp.scr == 0 )
        {
            /*  disable shaper  */
            qsb_queue_vbr_parameter_table.bit.taus = 0;
            qsb_queue_vbr_parameter_table.bit.ts = 0;
        }
        else
        {
            /*  Cell Loss Priority  (CLP)   */
            if ( (vcc->atm_options & ATM_ATMOPT_CLP) )
                /*  CLP1    */
                qsb_queue_parameter_table.bit.vbr = 1;
            else
                /*  CLP0    */
                qsb_queue_parameter_table.bit.vbr = 0;
            /*  Rate Shaper Parameter (TS) and Burst Tolerance Parameter for SCR (tauS) */
            tmp = ((qsb_clk * ppe_dev.qsb.tstepc) >> 5) / qos->txtp.scr + 1;
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
    else
    {
        qsb_queue_vbr_parameter_table.bit.taus = 0;
        qsb_queue_vbr_parameter_table.bit.ts = 0;
    }

    /*  Queue Parameter Table (QPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_QPT) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(connection);
    /*  Queue VBR Paramter Table (QVPT) */
    *QSB_RTM   = QSB_RTM_DM_SET(QSB_QVPT_SET_MASK);
    *QSB_RTD   = QSB_RTD_TTV_SET(qsb_queue_vbr_parameter_table.dword);
    *QSB_RAMAC = QSB_RAMAC_RW_SET(QSB_RAMAC_RW_WRITE) | QSB_RAMAC_TSEL_SET(QSB_RAMAC_TSEL_VBR) | QSB_RAMAC_LH_SET(QSB_RAMAC_LH_LOW) | QSB_RAMAC_TESEL_SET(connection);

}


static inline void u64_add_u32(ppe_u64_t opt1, u32 opt2,ppe_u64_t *ret)
{
    ret->l = opt1.l + opt2;
    if ( ret->l < opt1.l || ret->l < opt2 )
        ret->h++;
}

int find_vcc(struct atm_vcc *vcc)
{
	int i;
	struct connection *connection = ppe_dev.connection;
	int max_connections = ppe_dev.port[(int)vcc->dev->dev_data].max_connections;
	u32 occupation_table = ppe_dev.port[(int)vcc->dev->dev_data].connection_table;
	int base = ppe_dev.port[(int)vcc->dev->dev_data].connection_base;
	for ( i = 0; i < max_connections; i++, base++ )
		if ( (occupation_table & (1 << i))
			&& connection[base].vcc == vcc )
			return base;
	return -1;
}

int find_vpi(unsigned int vpi)
{
    int i, j;
    struct connection *connection = ppe_dev.connection;
    struct port *port;
    int base;

    port = ppe_dev.port;
    for ( i = 0; i < ATM_PORT_NUMBER; i++, port++ )
    {
        base = port->connection_base;
        for ( j = 0; j < port->max_connections; j++, base++ )
            if ( (port->connection_table & (1 << j))
                && connection[base].vcc != NULL
                && vpi == connection[base].vcc->vpi )
                return base;
    }
    return -1;
}

int find_vpivci(unsigned int vpi, unsigned int vci)
{
    int i, j;
    struct connection *connection = ppe_dev.connection;
    struct port *port;
    int base;

    port = ppe_dev.port;
    for ( i = 0; i < ATM_PORT_NUMBER; i++, port++ )
    {
        base = port->connection_base;
        for ( j = 0; j < port->max_connections; j++, base++ )
            if ( (port->connection_table & (1 << j))
                && connection[base].vcc != NULL
                && vpi == connection[base].vcc->vpi
                && vci == connection[base].vcc->vci )
                return base;
    }
    return -1;
}


static inline void clear_htu_entry(unsigned int connection)
{
    HTU_ENTRY(connection - QSB_QUEUE_NUMBER_BASE + OAM_HTU_ENTRY_NUMBER)->vld = 0;
}

static inline void set_htu_entry(unsigned int vpi, unsigned int vci, unsigned int connection, int aal5)
{
    struct htu_entry htu_entry = {  res1:       0x00,
                                    pid:        ppe_dev.connection[connection].port & 0x01,
                                    vpi:        vpi,
                                    vci:        vci,
                                    pti:        0x00,
                                    vld:        0x01};

    struct htu_mask htu_mask = {    set:        0x03,
                                    pid_mask:   0x02,
                                    vpi_mask:   0x00,
                                    vci_mask:   0x0000,
                                    pti_mask:   0x03,   //  0xx, user data
                                    clear:      0x00};

    struct htu_result htu_result = {res1:       0x00,
                                    cellid:     connection,
                                    res2:       0x00,
                                    type:       aal5 ? 0x00 : 0x01,
                                    ven:        0x01,
                                    res3:       0x00,
                                    qid:        connection};

    *HTU_RESULT(connection - QSB_QUEUE_NUMBER_BASE + OAM_HTU_ENTRY_NUMBER) = htu_result;
    *HTU_MASK(connection - QSB_QUEUE_NUMBER_BASE + OAM_HTU_ENTRY_NUMBER)   = htu_mask;
    *HTU_ENTRY(connection - QSB_QUEUE_NUMBER_BASE + OAM_HTU_ENTRY_NUMBER)  = htu_entry;
}

int alloc_tx_connection(int connection)
{
    unsigned long sys_flag;
    int desc_base;

    if ( ppe_dev.dma.tx_desc_alloc_pos[connection] == ppe_dev.dma.tx_desc_release_pos[connection] && ppe_dev.dma.tx_desc_alloc_flag[connection] )
        return -1;

    /*  amend descriptor pointer and allocation number  */
    local_irq_save(sys_flag);
    desc_base = ppe_dev.dma.tx_descriptor_number * (connection - QSB_QUEUE_NUMBER_BASE) + ppe_dev.dma.tx_desc_alloc_pos[connection];
    if ( ++ppe_dev.dma.tx_desc_alloc_pos[connection] == ppe_dev.dma.tx_descriptor_number )
        ppe_dev.dma.tx_desc_alloc_pos[connection] = 0;
    ppe_dev.dma.tx_desc_alloc_flag[connection] = 1;
    local_irq_restore(sys_flag);

    return desc_base;
}


int ppe_open(struct atm_vcc *vcc)
{
    int ret;
    struct port *port = &ppe_dev.port[(int)vcc->dev->dev_data];
    int conn;
    int f_enable_irq = 0;
    int i;
printk("%s:%s[%d] removed 2 args from signature\n", __FILE__, __func__, __LINE__);

printk("ppe_open");

    if ( vcc->qos.aal != ATM_AAL5 && vcc->qos.aal != ATM_AAL0 )
        return -EPROTONOSUPPORT;

    down(&ppe_dev.sem);

    /*  check bandwidth */
    if ( (vcc->qos.txtp.traffic_class == ATM_CBR && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_VBR_RT && vcc->qos.txtp.max_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_VBR_NRT && vcc->qos.txtp.pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate))
      || (vcc->qos.txtp.traffic_class == ATM_UBR_PLUS && vcc->qos.txtp.min_pcr > (port->tx_max_cell_rate - port->tx_current_cell_rate)) )
    {
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

    printk("alloc vpi = %d, vci = %d\n", vcc->vpi, vcc->vci);

    /*  check existing vpi,vci  */
    conn = find_vpivci(vcc->vpi, vcc->vci);
    if ( conn >= 0 )
    {
        ret = -EADDRINUSE;
        goto PPE_OPEN_EXIT;
    }

    /*  check whether it need to enable irq */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( ppe_dev.port[i].max_connections != 0 && ppe_dev.port[i].connection_table != 0 )
            break;
    if ( i == ATM_PORT_NUMBER )
        f_enable_irq = 1;

    /*  allocate connection */
    for ( i = 0, conn = port->connection_base; i < port->max_connections; i++, conn++ )
        if ( !(port->connection_table & (1 << i)) )
        {
            port->connection_table |= 1 << i;
            ppe_dev.connection[conn].vcc = vcc;
            break;
        }
    if ( i == port->max_connections )
    {
        ret = -EINVAL;
        goto PPE_OPEN_EXIT;
    }

#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    /*  assign DMA channel and setup weight value for RX QoS    */
    switch ( vcc->qos.rxtp.traffic_class )
    {
    case ATM_CBR:
        ppe_dev.connection[conn].rx_dma_channel = RX_DMA_CH_CBR;
        break;
    case ATM_VBR_RT:
        ppe_dev.connection[conn].rx_dma_channel = RX_DMA_CH_VBR_RT;
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_RT] += vcc->qos.rxtp.max_pcr;
        ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] += vcc->qos.rxtp.max_pcr;
        break;
    case ATM_VBR_NRT:
        ppe_dev.connection[conn].rx_dma_channel = RX_DMA_CH_VBR_NRT;
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_NRT] += vcc->qos.rxtp.pcr;
        ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] += vcc->qos.rxtp.pcr;
        break;
    case ATM_ABR:
        ppe_dev.connection[conn].rx_dma_channel = RX_DMA_CH_AVR;
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_AVR] += vcc->qos.rxtp.min_pcr;
        ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] += vcc->qos.rxtp.min_pcr;
        break;
    case ATM_UBR_PLUS:
    default:
        ppe_dev.connection[conn].rx_dma_channel = RX_DMA_CH_UBR;
        break;
    }

    /*  update RX queue configuration table */
    WRX_QUEUE_CONFIG(conn)->dmach = ppe_dev.connection[conn].rx_dma_channel;

    printk("ppe_open: QID %d, DMA %d\n", conn, WRX_QUEUE_CONFIG(conn)->dmach);

    printk("conn = %d, dmach = %d", conn, WRX_QUEUE_CONFIG(conn)->dmach);
#endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS

    /*  reserve bandwidth   */
    switch ( vcc->qos.txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_current_cell_rate += vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
        port->tx_current_cell_rate += vcc->qos.txtp.pcr;
        break;
    case ATM_UBR_PLUS:
        port->tx_current_cell_rate += vcc->qos.txtp.min_pcr;
        break;
    }

    /*  set qsb */
    set_qsb(vcc, &vcc->qos, conn);

    /*  update atm_vcc structure    */
    vcc->itf = (int)vcc->dev->dev_data;

    set_bit(ATM_VF_READY, &vcc->flags);

    /*  enable irq  */
    printk("ppe_open: enable_irq\n");
    if ( f_enable_irq )
        enable_irq(IFXMIPS_PPE_MBOX_INT);

    /*  enable mailbox  */
    *MBOX_IGU1_ISRC =  (1 << conn) | (1 << (conn + 16));
    *MBOX_IGU1_IER  |= (1 << conn) | (1 << (conn + 16));
    *MBOX_IGU3_ISRC =  (1 << conn) | (1 << (conn + 16));
    *MBOX_IGU3_IER  |= (1 << conn) | (1 << (conn + 16));

    /*  set htu entry   */
    set_htu_entry(vcc->vpi, vcc->vci, conn, vcc->qos.aal == ATM_AAL5 ? 1 : 0);

    ret = 0;

    printk("ppe_open(%d.%d): conn = %d, ppe_dev.dma = %08X\n", vcc->vpi, vcc->vci, conn, (u32)&ppe_dev.dma.rx_descriptor_number);


PPE_OPEN_EXIT:
    up(&ppe_dev.sem);

    printk("open ATM itf = %d, vpi = %d, vci = %d, ret = %d", (int)vcc->dev->dev_data, (int)vcc->vpi, vcc->vci, ret);
    return ret;
}

void ppe_close(struct atm_vcc *vcc)
{
    int conn;
    struct port *port;
    struct connection *connection;
    int i;

    if ( vcc == NULL )
        return;

    down(&ppe_dev.sem);

    /*  get connection id   */
    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        printk("can't find vcc\n");
        goto PPE_CLOSE_EXIT;
    }
    if(!((Atm_Priv *)vcc)->on)
		goto PPE_CLOSE_EXIT;
	connection = &ppe_dev.connection[conn];
    port = &ppe_dev.port[connection->port];

    /*  clear htu   */
    clear_htu_entry(conn);

    /*  release connection  */
    port->connection_table &= ~(1 << (conn - port->connection_base));
    connection->vcc = NULL;
    connection->access_time.tv_sec = 0;
    connection->access_time.tv_nsec = 0;
    connection->aal5_vcc_crc_err = 0;
    connection->aal5_vcc_oversize_sdu = 0;

    /*  disable irq */
    for ( i = 0; i < ATM_PORT_NUMBER; i++ )
        if ( ppe_dev.port[i].max_connections != 0 && ppe_dev.port[i].connection_table != 0 )
            break;
    if ( i == ATM_PORT_NUMBER )
        disable_irq(IFXMIPS_PPE_MBOX_INT);

    *MBOX_IGU1_ISRC =  (1 << conn) | (1 << (conn + 16));

#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    /*  remove weight value from RX DMA channel */
    switch ( vcc->qos.rxtp.traffic_class )
    {
    case ATM_VBR_RT:
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_RT] -= vcc->qos.rxtp.max_pcr;
        if ( ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] > ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_RT] )
            ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_RT];
        break;
    case ATM_VBR_NRT:
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_NRT] -= vcc->qos.rxtp.pcr;
        if ( ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] > ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_NRT] )
            ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_NRT];
        break;
    case ATM_ABR:
        ppe_dev.dma.rx_default_weight[RX_DMA_CH_AVR] -= vcc->qos.rxtp.min_pcr;
        if ( ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] > ppe_dev.dma.rx_default_weight[RX_DMA_CH_AVR] )
            ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_AVR];
        break;
    case ATM_CBR:
    case ATM_UBR_PLUS:
    default:
        break;
    }
#endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS

    /*  release bandwidth   */
    switch ( vcc->qos.txtp.traffic_class )
    {
    case ATM_CBR:
    case ATM_VBR_RT:
        port->tx_current_cell_rate -= vcc->qos.txtp.max_pcr;
        break;
    case ATM_VBR_NRT:
        port->tx_current_cell_rate -= vcc->qos.txtp.pcr;
        break;
    case ATM_UBR_PLUS:
        port->tx_current_cell_rate -= vcc->qos.txtp.min_pcr;
        break;
    }

    /*  idle for a while to let parallel operation finish   */
    for ( i = 0; i < IDLE_CYCLE_NUMBER; i++ );
	((Atm_Priv *)vcc)->on = 0;

PPE_CLOSE_EXIT:
    up(&ppe_dev.sem);
}

int ppe_ioctl(struct atm_dev *dev, unsigned int cmd, void *arg)
{
	return -ENOTTY;
}

int ppe_send(struct atm_vcc *vcc, struct sk_buff *skb)
{
    int ret;
    int conn;
    int desc_base;
    register struct tx_descriptor reg_desc;
    struct tx_descriptor *desc;


printk("ppe_send");
printk("ppe_send\n");
printk("skb->users = %d\n", skb->users.counter);

    if ( vcc == NULL || skb == NULL )
        return -EINVAL;

//    down(&ppe_dev.sem);

    ATM_SKB(skb)->vcc = vcc;
    conn = find_vcc(vcc);
//    if ( conn != 1 )
printk("ppe_send: conn = %d\n", conn);
    if ( conn < 0 )
    {
        ret = -EINVAL;
        goto FIND_VCC_FAIL;
    }

printk("find_vcc");

    if ( vcc->qos.aal == ATM_AAL5 )
    {
        int byteoff;
        int datalen;
        struct tx_inband_header *header;

    /*  allocate descriptor */
	    desc_base = alloc_tx_connection(conn);
	    if ( desc_base < 0 )
	    {
	        ret = -EIO;
	        //goto ALLOC_TX_CONNECTION_FAIL;
	    }
	    desc = &ppe_dev.dma.tx_descriptor_base[desc_base];

    /*  load descriptor from memory */
	    reg_desc = *desc;

        datalen = skb->len;
        byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
        if ( skb_headroom(skb) < byteoff + TX_INBAND_HEADER_LENGTH )
        {
            struct sk_buff *new_skb;

printk("skb_headroom(skb) < byteoff + TX_INBAND_HEADER_LENGTH");
printk("skb_headroom(skb 0x%08X, skb->data 0x%08X) (%d) < byteoff (%d) + TX_INBAND_HEADER_LENGTH (%d)\n", (u32)skb, (u32)skb->data, skb_headroom(skb), byteoff, TX_INBAND_HEADER_LENGTH);

            new_skb = alloc_skb_tx(datalen);
            if ( new_skb == NULL )
            {
                printk("alloc_skb_tx: fail\n");
                ret = -ENOMEM;
                goto ALLOC_SKB_TX_FAIL;
            }
            ATM_SKB(new_skb)->vcc = NULL;
            skb_put(new_skb, datalen);
            memcpy(new_skb->data, skb->data, datalen);
            atm_free_tx_skb_vcc(skb);
            skb = new_skb;
            byteoff = (u32)skb->data & (DMA_ALIGNMENT - 1);
        }
        else
        {
printk("skb_headroom(skb) >= byteoff + TX_INBAND_HEADER_LENGTH");
        }
printk("before skb_push, skb->data = 0x%08X", (u32)skb->data);
        skb_push(skb, byteoff + TX_INBAND_HEADER_LENGTH);
printk("after skb_push, skb->data = 0x%08X", (u32)skb->data);

        header = (struct tx_inband_header *)(u32)skb->data;
printk("header = 0x%08X", (u32)header);

        /*  setup inband trailer    */
        header->uu   = 0;
        header->cpi  = 0;
        header->pad  = ppe_dev.aal5.padding_byte;
        header->res1 = 0;

        /*  setup cell header   */
        header->clp  = (vcc->atm_options & ATM_ATMOPT_CLP) ? 1 : 0;
        header->pti  = ATM_PTI_US0;
        header->vci  = vcc->vci;
        header->vpi  = vcc->vpi;
        header->gfc  = 0;

        /*  setup descriptor    */
        reg_desc.dataptr = (u32)skb->data >> 2;
        reg_desc.datalen = datalen;
        reg_desc.byteoff = byteoff;
        reg_desc.iscell  = 0;

printk("setup header, datalen = %d, byteoff = %d", reg_desc.datalen, reg_desc.byteoff);

        UPDATE_VCC_STAT(conn, tx_pdu, 1);

        if ( vcc->stats )
            atomic_inc(&vcc->stats->tx);
    }
    else
    {
    /*  allocate descriptor */
	    desc_base = alloc_tx_connection(conn);
	    if ( desc_base < 0 )
    {
	        ret = -EIO;
	        goto ALLOC_TX_CONNECTION_FAIL;
    }
	    desc = &ppe_dev.dma.tx_descriptor_base[desc_base];

    /*  load descriptor from memory */
	    reg_desc = *desc;

        /*  if data pointer is not aligned, allocate new sk_buff    */
        if ( ((u32)skb->data & (DMA_ALIGNMENT - 1)) )
        {
            struct sk_buff *new_skb;

            printk("skb->data not aligned\n");

            new_skb = alloc_skb_tx(skb->len);
            if ( new_skb == NULL )
            {
                ret = -ENOMEM;
                goto ALLOC_SKB_TX_FAIL;
            }
            ATM_SKB(new_skb)->vcc = NULL;
            skb_put(new_skb, skb->len);
            memcpy(new_skb->data, skb->data, skb->len);
            atm_free_tx_skb_vcc(skb);
            skb = new_skb;
        }

        reg_desc.dataptr = (u32)skb->data >> 2;
        reg_desc.datalen = skb->len;
        reg_desc.byteoff = 0;
        reg_desc.iscell  = 1;

        if ( vcc->stats )
            atomic_inc(&vcc->stats->tx);
    }

    reg_desc.own = 1;
    reg_desc.c = 1;

printk("update descriptor send pointer, desc = 0x%08X", (u32)desc);

    ppe_dev.dma.tx_skb_pointers[desc_base] = skb;
    *desc = reg_desc;
    dma_cache_wback((unsigned long)skb->data, skb->len);

    mailbox_signal(conn, 1);

printk("ppe_send: success");
//    up(&ppe_dev.sem);

    return 0;

FIND_VCC_FAIL:
    printk("FIND_VCC_FAIL\n");

//    up(&ppe_dev.sem);
    ppe_dev.mib.wtx_err_pdu++;
    atm_free_tx_skb_vcc(skb);

    return ret;

ALLOC_SKB_TX_FAIL:
    printk("ALLOC_SKB_TX_FAIL\n");

//    up(&ppe_dev.sem);
    if ( vcc->qos.aal == ATM_AAL5 )
    {
        UPDATE_VCC_STAT(conn, tx_err_pdu, 1);
        ppe_dev.mib.wtx_err_pdu++;
    }
    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx_err);
    atm_free_tx_skb_vcc(skb);

    return ret;

ALLOC_TX_CONNECTION_FAIL:
    printk("ALLOC_TX_CONNECTION_FAIL\n");

//    up(&ppe_dev.sem);
    if ( vcc->qos.aal == ATM_AAL5 )
    {
        UPDATE_VCC_STAT(conn, tx_sw_drop_pdu, 1);
        ppe_dev.mib.wtx_drop_pdu++;
    }
    if ( vcc->stats )
        atomic_inc(&vcc->stats->tx_err);
    atm_free_tx_skb_vcc(skb);

    return ret;
}

int ppe_send_oam(struct atm_vcc *vcc, void *cell, int flags)
{
    int conn;
    struct uni_cell_header *uni_cell_header = (struct uni_cell_header *)cell;
    int desc_base;
    struct sk_buff *skb;
    register struct tx_descriptor reg_desc;
    struct tx_descriptor *desc;

printk("ppe_send_oam");

    if ( ((uni_cell_header->pti == ATM_PTI_SEGF5 || uni_cell_header->pti == ATM_PTI_E2EF5)
        && find_vpivci(uni_cell_header->vpi, uni_cell_header->vci) < 0)
        || ((uni_cell_header->vci == 0x03 || uni_cell_header->vci == 0x04)
        && find_vpi(uni_cell_header->vpi) < 0) )
        return -EINVAL;

#if OAM_TX_QUEUE_NUMBER_PER_PORT != 0
    /*  get queue ID of OAM TX queue, and the TX DMA channel ID is the same as queue ID */
    conn = ppe_dev.port[(int)vcc->dev->dev_data].oam_tx_queue;
#else
    /*  find queue ID   */
    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        printk("OAM not find queue\n");
//        up(&ppe_dev.sem);
        return -EINVAL;
    }
#endif  //  OAM_TX_QUEUE_NUMBER_PER_PORT != 0

    /*  allocate descriptor */
    desc_base = alloc_tx_connection(conn);
    if ( desc_base < 0 )
    {
        printk("OAM not alloc tx connection\n");
//        up(&ppe_dev.sem);
        return -EIO;
    }

    desc = &ppe_dev.dma.tx_descriptor_base[desc_base];

    /*  load descriptor from memory */
    reg_desc = *(struct tx_descriptor *)desc;

    /*  allocate sk_buff    */
    skb = alloc_skb_tx(CELL_SIZE);
    if ( skb == NULL )
    {
//        up(&ppe_dev.sem);
        return -ENOMEM;
    }
#if OAM_TX_QUEUE_NUMBER_PER_PORT != 0
    ATM_SKB(skb)->vcc = NULL;
#else
    ATM_SKB(skb)->vcc = vcc;
#endif  //  OAM_TX_QUEUE_NUMBER_PER_PORT != 0

    /*  copy data   */
    skb_put(skb, CELL_SIZE);
    memcpy(skb->data, cell, CELL_SIZE);

    /*  setup descriptor    */
    reg_desc.dataptr = (u32)skb->data >> 2;
    reg_desc.datalen = CELL_SIZE;
    reg_desc.byteoff = 0;
    reg_desc.iscell  = 1;
    reg_desc.own = 1;
    reg_desc.c = 1;

    /*  update descriptor send pointer  */
    ppe_dev.dma.tx_skb_pointers[desc_base] = skb;

    /*  write discriptor to memory and write back cache */
    *(struct tx_descriptor *)desc = reg_desc;
    dma_cache_wback((unsigned long)skb->data, skb->len);

    /*  signal PPE  */
    mailbox_signal(conn, 1);

    return 0;
}

int ppe_change_qos(struct atm_vcc *vcc, struct atm_qos *qos, int flags)
{
    int conn;
	printk("%s:%s[%d]\n", __FILE__, __func__, __LINE__);

    if(vcc == NULL || qos == NULL )
        return -EINVAL;
    conn = find_vcc(vcc);
    if ( conn < 0 )
        return -EINVAL;
    set_qsb(vcc, qos, conn);

	return 0;
}

static inline void init_chip(void)
{
    /*  enable PPE module in PMU    */
    *(unsigned long *)0xBF10201C &= ~((1 << 15) | (1 << 13) | (1 << 9));

    *EMA_CMDCFG  = (EMA_CMD_BUF_LEN << 16) | (EMA_CMD_BASE_ADDR >> 2);
    *EMA_DATACFG = (EMA_DATA_BUF_LEN << 16) | (EMA_DATA_BASE_ADDR >> 2);
    *EMA_IER     = 0x000000FF;
	*EMA_CFG     = EMA_READ_BURST | (EMA_WRITE_BURST << 2);

    /*  enable mailbox  */
    *MBOX_IGU1_ISRC = 0xFFFFFFFF;
    *MBOX_IGU1_IER  = 0x00000000;
    *MBOX_IGU3_ISRC = 0xFFFFFFFF;
    *MBOX_IGU3_IER  = 0x00000000;
}

int pp32_download_code(u32 *code_src, unsigned int code_dword_len, u32 *data_src, unsigned int data_dword_len)
{
    u32 reg_old_value;
    volatile u32 *dest;

    if ( code_src == 0 || ((unsigned long)code_src & 0x03) != 0
        || data_src == 0 || ((unsigned long)data_src & 0x03) )
        return -EINVAL;

    /*  save the old value of CDM_CFG and set PPE code memory to FPI bus access mode    */
    reg_old_value = *CDM_CFG;
    if ( code_dword_len <= 4096 )
        *CDM_CFG = CDM_CFG_RAM1_SET(0x00) | CDM_CFG_RAM0_SET(0x00);
    else
        *CDM_CFG = CDM_CFG_RAM1_SET(0x01) | CDM_CFG_RAM0_SET(0x00);

    /*  copy code   */
    dest = CDM_CODE_MEMORY_RAM0_ADDR(0);
    while ( code_dword_len-- > 0 )
        *dest++ = *code_src++;

    /*  copy data   */
    dest = PP32_DATA_MEMORY_RAM1_ADDR(0);
    while ( data_dword_len-- > 0 )
        *dest++ = *data_src++;

    return 0;
}

int pp32_start(void)
{
    int ret;
    register int i;
	init_chip();
    /*  download firmware   */
    ret = pp32_download_code(firmware_binary_code, sizeof(firmware_binary_code) / sizeof(*firmware_binary_code), firmware_binary_data, sizeof(firmware_binary_data) / sizeof(*firmware_binary_data));
    if ( ret )
        return ret;

    /*  run PP32    */
    *PP32_DBG_CTRL = DBG_CTRL_START_SET(1);

    /*  idle for a while to let PP32 init itself    */
    for ( i = 0; i < IDLE_CYCLE_NUMBER; i++ );

    return 0;
}

void pp32_stop(void)
{
    /*  halt PP32   */
    *PP32_DBG_CTRL = DBG_CTRL_STOP_SET(1);
}
