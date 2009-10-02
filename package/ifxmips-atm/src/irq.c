#include <linux/atmdev.h>
#include <linux/irq.h>

#include "common.h"

void mailbox_signal(unsigned int channel, int is_tx)
{
    if(is_tx)
    {
        while(MBOX_IGU3_ISR_ISR(channel + 16));
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(channel + 16);
    } else  {
        while(MBOX_IGU3_ISR_ISR(channel));
        *MBOX_IGU3_ISRS = MBOX_IGU3_ISRS_SET(channel);
    }
}

static int mailbox_rx_irq_handler(unsigned int channel, unsigned int *len)
{
    int conn;
    int skb_base;
    register struct rx_descriptor reg_desc;
    struct rx_descriptor *desc;
    struct sk_buff *skb;
    struct atm_vcc *vcc;
    struct rx_inband_trailer *trailer;

    /*  get sk_buff pointer and descriptor  */
    skb_base = ppe_dev.dma.rx_descriptor_number * channel + ppe_dev.dma.rx_desc_read_pos[channel];
    desc = &ppe_dev.dma.rx_descriptor_base[skb_base];
    reg_desc = *desc;
    if ( reg_desc.own || !reg_desc.c )
        return -EAGAIN;

    if ( ++ppe_dev.dma.rx_desc_read_pos[channel] == ppe_dev.dma.rx_descriptor_number )
        ppe_dev.dma.rx_desc_read_pos[channel] = 0;

    skb = *(struct sk_buff **)((((u32)reg_desc.dataptr << 2) | KSEG0) - 4);
    if ( (u32)skb <= 0x80000000 )
    {
        int key = 0;
        printk("skb problem: skb = %08X, system is panic!\n", (u32)skb);
        for ( ; !key; );
    }

    conn = reg_desc.id;

    if ( conn == ppe_dev.oam_rx_queue )
    {
        /*  OAM */
        struct uni_cell_header *header = (struct uni_cell_header *)skb->data;

        if ( header->pti == ATM_PTI_SEGF5 || header->pti == ATM_PTI_E2EF5 )
            conn = find_vpivci(header->vpi, header->vci);
        else if ( header->vci == 0x03 || header->vci == 0x04 )
            conn = find_vpi(header->vpi);
        else
            conn = -1;

        if ( conn >= 0 && ppe_dev.connection[conn].vcc != NULL )
        {
            vcc = ppe_dev.connection[conn].vcc;
            ppe_dev.connection[conn].access_time = xtime;
            if ( vcc->push_oam != NULL )
                vcc->push_oam(vcc, skb->data);
        }

        /*  don't need resize   */
    }
    else
    {
        if ( len )
            *len = 0;

        if ( ppe_dev.connection[conn].vcc != NULL )
        {
            vcc = ppe_dev.connection[conn].vcc;

            if ( !reg_desc.err )
                if ( vcc->qos.aal == ATM_AAL5 )
                {
                    /*  AAL5 packet */
                    resize_skb_rx(skb, reg_desc.datalen + reg_desc.byteoff, 0);
                    skb_reserve(skb, reg_desc.byteoff);
                    skb_put(skb, reg_desc.datalen);

                    if ( (u32)ATM_SKB(skb) <= 0x80000000 )
                    {
                        int key = 0;
                        printk("ATM_SKB(skb) problem: ATM_SKB(skb) = %08X, system is panic!\n", (u32)ATM_SKB(skb));
                        for ( ; !key; );
                    }
                    ATM_SKB(skb)->vcc = vcc;
                    ppe_dev.connection[conn].access_time = xtime;
                    if ( atm_charge(vcc, skb->truesize) )
                    {
                        struct sk_buff *new_skb;

                        new_skb = alloc_skb_rx();
                        if ( new_skb )
                        {

                            UPDATE_VCC_STAT(conn, rx_pdu, 1);

                            ppe_dev.mib.wrx_pdu++;
                            if ( vcc->stats )
                                atomic_inc(&vcc->stats->rx);
                            vcc->push(vcc, skb);
                            {
                                struct k_atm_aal_stats stats = *vcc->stats;
                                int flag = 0;

                                vcc->push(vcc, skb);
                                if ( vcc->stats->rx.counter != stats.rx.counter )
                                {
                                    printk("vcc->stats->rx (diff) = %d", vcc->stats->rx.counter - stats.rx.counter);
                                    flag++;
                                }
                                if ( vcc->stats->rx_err.counter != stats.rx_err.counter )
                                {
                                    printk("vcc->stats->rx_err (diff) = %d", vcc->stats->rx_err.counter - stats.rx_err.counter);
                                    flag++;
                                }
                                if ( vcc->stats->rx_drop.counter != stats.rx_drop.counter )
                                {
                                    printk("vcc->stats->rx_drop (diff) = %d", vcc->stats->rx_drop.counter - stats.rx_drop.counter);
                                    flag++;
                                }
                                if ( vcc->stats->tx.counter != stats.tx.counter )
                                {
                                    printk("vcc->stats->tx (diff) = %d", vcc->stats->tx.counter - stats.tx.counter);
                                    flag++;
                                }
                                if ( vcc->stats->tx_err.counter != stats.tx_err.counter )
                                {
                                    printk("vcc->stats->tx_err (diff) = %d", vcc->stats->tx_err.counter - stats.tx_err.counter);
                                    flag++;
                                }
                                if ( !flag )
                                    printk("vcc->stats not changed");
                            }
                            reg_desc.dataptr = (u32)new_skb->data >> 2;

                            if ( len )
                                *len = reg_desc.datalen;
                        }
                        else
                        {
                            /*  no sk buffer    */
                            UPDATE_VCC_STAT(conn, rx_sw_drop_pdu, 1);

                            ppe_dev.mib.wrx_drop_pdu++;
                            if ( vcc->stats )
                                atomic_inc(&vcc->stats->rx_drop);

                            resize_skb_rx(skb, ppe_dev.aal5.rx_buffer_size, 0);
                        }
                    }
                    else
                    {
                        /*  no enough space */
                        UPDATE_VCC_STAT(conn, rx_sw_drop_pdu, 1);

                        ppe_dev.mib.wrx_drop_pdu++;
                        if ( vcc->stats )
                            atomic_inc(&vcc->stats->rx_drop);

                        resize_skb_rx(skb, ppe_dev.aal5.rx_buffer_size, 0);
                    }
                }
                else
                {
                    /*  AAL0 cell   */
                    resize_skb_rx(skb, CELL_SIZE, 1);
                    skb_put(skb, CELL_SIZE);

                    ATM_SKB(skb)->vcc = vcc;
                    ppe_dev.connection[conn].access_time = xtime;
                    if ( atm_charge(vcc, skb->truesize) )
                    {
                        struct sk_buff *new_skb;

                        new_skb = alloc_skb_rx();
                        if ( new_skb )
                        {
                            if ( vcc->stats )
                                atomic_inc(&vcc->stats->rx);
                            vcc->push(vcc, skb);
                            reg_desc.dataptr = (u32)new_skb->data >> 2;

                            if ( len )
                                *len = CELL_SIZE;
                        }
                        else
                        {
                            if ( vcc->stats )
                                atomic_inc(&vcc->stats->rx_drop);
                            resize_skb_rx(skb, ppe_dev.aal5.rx_buffer_size, 0);
                        }
                    }
                    else
                    {
                        if ( vcc->stats )
                            atomic_inc(&vcc->stats->rx_drop);
                        resize_skb_rx(skb, ppe_dev.aal5.rx_buffer_size, 0);
                    }
                }
            else
            {
printk("reg_desc.err\n");

                /*  drop packet/cell    */
                if ( vcc->qos.aal == ATM_AAL5 )
                {
                    UPDATE_VCC_STAT(conn, rx_err_pdu, 1);

                    trailer = (struct rx_inband_trailer *)((u32)skb->data + ((reg_desc.byteoff + reg_desc.datalen + DMA_ALIGNMENT - 1) & ~ (DMA_ALIGNMENT - 1)));
                    if ( trailer->stw_crc )
                        ppe_dev.connection[conn].aal5_vcc_crc_err++;
                    if ( trailer->stw_ovz )
                        ppe_dev.connection[conn].aal5_vcc_oversize_sdu++;
                }
                if ( vcc->stats )
                    atomic_inc(&vcc->stats->rx_err);
                /*  don't need resize   */
            }
        }
        else
        {
printk("ppe_dev.connection[%d].vcc == NULL\n", conn);

            ppe_dev.mib.wrx_drop_pdu++;

            /*  don't need resize   */
        }
    }

    reg_desc.byteoff = 0;
    reg_desc.datalen = ppe_dev.aal5.rx_buffer_size;
    reg_desc.own = 1;
    reg_desc.c   = 0;

    /*  write discriptor to memory  */
    *desc = reg_desc;

printk("leave mailbox_rx_irq_handler");

    return 0;
}

static inline void mailbox_tx_irq_handler(unsigned int conn)
{
	if ( ppe_dev.dma.tx_desc_alloc_flag[conn] )
    {
        int desc_base;
        int *release_pos;
        struct sk_buff *skb;

        release_pos = &ppe_dev.dma.tx_desc_release_pos[conn];
        desc_base = ppe_dev.dma.tx_descriptor_number * (conn - QSB_QUEUE_NUMBER_BASE) + *release_pos;
        while ( !ppe_dev.dma.tx_descriptor_base[desc_base].own )
        {
            skb = ppe_dev.dma.tx_skb_pointers[desc_base];

            ppe_dev.dma.tx_descriptor_base[desc_base].own = 1;  //  pretend PP32 hold owner bit, so that won't be released more than once, so allocation process don't check this bit

            if ( ++*release_pos == ppe_dev.dma.tx_descriptor_number )
                *release_pos = 0;

            if ( *release_pos == ppe_dev.dma.tx_desc_alloc_pos[conn] )
            {
                ppe_dev.dma.tx_desc_alloc_flag[conn] = 0;

		atm_free_tx_skb_vcc(skb);
                break;
            }

            if ( *release_pos == 0 )
                desc_base = ppe_dev.dma.tx_descriptor_number * (conn - QSB_QUEUE_NUMBER_BASE);
            else
                desc_base++;

	    atm_free_tx_skb_vcc(skb);
        }
    }
}

#if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
static inline int check_desc_valid(unsigned int channel)
{
    int skb_base;
    struct rx_descriptor *desc;

    skb_base = ppe_dev.dma.rx_descriptor_number * channel + ppe_dev.dma.rx_desc_read_pos[channel];
    desc = &ppe_dev.dma.rx_descriptor_base[skb_base];
    return !desc->own && desc->c ? 1 : 0;
}
#endif

irqreturn_t mailbox_irq_handler(int irq, void *dev_id)
{
    int channel_mask;   /*  DMA channel accordant IRQ bit mask  */
    int channel;
    unsigned int rx_irq_number[MAX_RX_DMA_CHANNEL_NUMBER] = {0};
    unsigned int total_rx_irq_number = 0;

printk("mailbox_irq_handler");

    if ( !*MBOX_IGU1_ISR )
        return IRQ_RETVAL(1);

    channel_mask = 1;
    channel = 0;
    while ( channel < ppe_dev.dma.rx_total_channel_used )
    {
        if ( (*MBOX_IGU1_ISR & channel_mask) )
        {
            /*  RX  */
            /*  clear IRQ   */
            *MBOX_IGU1_ISRC = channel_mask;
printk("  RX: *MBOX_IGU1_ISR = 0x%08X\n", *MBOX_IGU1_ISR);
            /*  wait for mailbox cleared    */
            while ( (*MBOX_IGU3_ISR & channel_mask) );

            /*  shadow the number of valid descriptor   */
            rx_irq_number[channel] = WRX_DMA_CHANNEL_CONFIG(channel)->vlddes;

            total_rx_irq_number += rx_irq_number[channel];

printk("total_rx_irq_number = %d", total_rx_irq_number);
printk("vlddes = %d, rx_irq_number[%d] = %d, total_rx_irq_number = %d\n", WRX_DMA_CHANNEL_CONFIG(channel)->vlddes, channel, rx_irq_number[channel], total_rx_irq_number);
        }

        channel_mask <<= 1;
        channel++;
    }

    channel_mask = 1 << (16 + QSB_QUEUE_NUMBER_BASE);
    channel = QSB_QUEUE_NUMBER_BASE;
    while ( channel - QSB_QUEUE_NUMBER_BASE < ppe_dev.dma.tx_total_channel_used )
    {
        if ( (*MBOX_IGU1_ISR & channel_mask) )
        {
//            if ( channel != 1 )
//            {
printk("TX irq error\n");
//                while ( 1 )
//                {
//                }
//            }
            /*  TX  */
            /*  clear IRQ   */
            *MBOX_IGU1_ISRC = channel_mask;
printk("  TX: *MBOX_IGU1_ISR = 0x%08X\n", *MBOX_IGU1_ISR);
            mailbox_tx_irq_handler(channel);
        }

        channel_mask <<= 1;
        channel++;
    }

  #if defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
    channel = 0;
    while ( total_rx_irq_number )
    {
        switch ( channel )
        {
        case RX_DMA_CH_CBR:
        case RX_DMA_CH_OAM:
            /*  handle it as soon as possible   */
            while ( rx_irq_number[channel] != 0 && mailbox_rx_irq_handler(channel, NULL) == 0 )
            {
                rx_irq_number[channel]--;
                total_rx_irq_number--;
printk("RX_DMA_CH_CBR, total_rx_irq_number = %d", total_rx_irq_number);
printk("RX_DMA_CH_CBR, total_rx_irq_number = %d, rx_irq_number[%d] = %d\n", total_rx_irq_number, channel, rx_irq_number[channel]);
                /*  signal firmware that descriptor is updated */
                mailbox_signal(channel, 0);
            }
//            if ( rx_irq_number[channel] != 0 )
printk("RX_DMA_CH_CBR, rx_irq_number[channel] = %d", rx_irq_number[channel]);
            break;
        case RX_DMA_CH_VBR_RT:
            /*  WFQ */
            if ( rx_irq_number[RX_DMA_CH_VBR_RT] != 0
                && (rx_irq_number[RX_DMA_CH_VBR_NRT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_NRT) || ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] < ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT])
                && (rx_irq_number[RX_DMA_CH_AVR] == 0 || !check_desc_valid(RX_DMA_CH_AVR) || ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] < ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT])
            )
            {
                unsigned int len;

                if ( mailbox_rx_irq_handler(RX_DMA_CH_VBR_RT, &len) == 0 )
                {
                    rx_irq_number[RX_DMA_CH_VBR_RT]--;
                    total_rx_irq_number--;
printk("RX_DMA_CH_VBR_RT, total_rx_irq_number = %d", total_rx_irq_number);
printk("RX_DMA_CH_VBR_RT, total_rx_irq_number = %d, rx_irq_number[%d] = %d\n", total_rx_irq_number, channel, rx_irq_number[channel]);
                    /*  signal firmware that descriptor is updated */
                    mailbox_signal(channel, 0);

                    len = (len + CELL_SIZE - 1) / CELL_SIZE;
                    if ( ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] <= len )
                        ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_RT] + ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] - len;
                }
            }
//            if ( rx_irq_number[channel] != 0 )
//            {
printk("RX_DMA_CH_VBR_RT, rx_irq_number[channel] = %d, total_rx_irq_number = %d", rx_irq_number[channel], total_rx_irq_number);
//                rx_irq_number[channel] = 0;
//                total_rx_irq_number = 0;
//            }
            break;
        case RX_DMA_CH_VBR_NRT:
            /*  WFQ */
            if ( rx_irq_number[RX_DMA_CH_VBR_NRT] != 0
                && (rx_irq_number[RX_DMA_CH_VBR_RT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_RT) || ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] < ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT])
                && (rx_irq_number[RX_DMA_CH_AVR] == 0 || !check_desc_valid(RX_DMA_CH_AVR) || ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] < ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT])
            )
            {
                unsigned int len;

                if ( mailbox_rx_irq_handler(RX_DMA_CH_VBR_NRT, &len) == 0 )
                {
                    rx_irq_number[RX_DMA_CH_VBR_NRT]--;
                    total_rx_irq_number--;
printk("RX_DMA_CH_VBR_NRT, total_rx_irq_number = %d", total_rx_irq_number);
printk("RX_DMA_CH_VBR_NRT, total_rx_irq_number = %d, rx_irq_number[%d] = %d\n", total_rx_irq_number, channel, rx_irq_number[channel]);
                    /*  signal firmware that descriptor is updated */
                    mailbox_signal(channel, 0);

                    len = (len + CELL_SIZE - 1) / CELL_SIZE;
                    if ( ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] <= len )
                        ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_VBR_NRT] + ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] - len;
                }
            }
//            if ( rx_irq_number[channel] != 0 )
printk("RX_DMA_CH_VBR_NRT, rx_irq_number[channel] = %d", rx_irq_number[channel]);
            break;
        case RX_DMA_CH_AVR:
            /*  WFQ */
            if ( rx_irq_number[RX_DMA_CH_AVR] != 0
                && (rx_irq_number[RX_DMA_CH_VBR_RT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_RT) || ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_RT] < ppe_dev.dma.rx_weight[RX_DMA_CH_AVR])
                && (rx_irq_number[RX_DMA_CH_VBR_NRT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_NRT) || ppe_dev.dma.rx_weight[RX_DMA_CH_VBR_NRT] < ppe_dev.dma.rx_weight[RX_DMA_CH_AVR])
            )
            {
                unsigned int len;

                if ( mailbox_rx_irq_handler(RX_DMA_CH_AVR, &len) == 0 )
                {
                    rx_irq_number[RX_DMA_CH_AVR]--;
                    total_rx_irq_number--;
printk("RX_DMA_CH_AVR, total_rx_irq_number = %d", total_rx_irq_number);
printk("RX_DMA_CH_AVR, total_rx_irq_number = %d, rx_irq_number[%d] = %d\n", total_rx_irq_number, channel, rx_irq_number[channel]);
                    /*  signal firmware that descriptor is updated */
                    mailbox_signal(channel, 0);

                    len = (len + CELL_SIZE - 1) / CELL_SIZE;
                    if ( ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] <= len )
                        ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] = ppe_dev.dma.rx_default_weight[RX_DMA_CH_AVR] + ppe_dev.dma.rx_weight[RX_DMA_CH_AVR] - len;
                }
            }
//            if ( rx_irq_number[channel] != 0 )
printk("RX_DMA_CH_AVR, rx_irq_number[channel] = %d", rx_irq_number[channel]);
            break;
        case RX_DMA_CH_UBR:
        default:
            /*  Handle it when all others are handled or others are not available to handle.    */
            if ( rx_irq_number[channel] != 0
                && (rx_irq_number[RX_DMA_CH_VBR_RT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_RT))
                && (rx_irq_number[RX_DMA_CH_VBR_NRT] == 0 || !check_desc_valid(RX_DMA_CH_VBR_NRT))
                && (rx_irq_number[RX_DMA_CH_AVR] == 0 || !check_desc_valid(RX_DMA_CH_AVR)) )
                if ( mailbox_rx_irq_handler(channel, NULL) == 0 )
                {
                    rx_irq_number[channel]--;
                    total_rx_irq_number--;
printk("RX_DMA_CH_UBR, total_rx_irq_number = %d, rx_irq_number[%d] = %d", total_rx_irq_number, channel, rx_irq_number[channel]);
printk("RX_DMA_CH_UBR, total_rx_irq_number = %d, rx_irq_number[%d] = %d\n", total_rx_irq_number, channel, rx_irq_number[channel]);
                    /*  signal firmware that descriptor is updated */
                    mailbox_signal(channel, 0);
                }
printk("RX_DMA_CH_UBR, rx_irq_number[channel] = %d", rx_irq_number[channel]);
        }

        if ( ++channel == ppe_dev.dma.rx_total_channel_used )
            channel = 0;
    }
  #else
    channel = 0;
    while ( total_rx_irq_number )
    {
        while ( rx_irq_number[channel] != 0 && mailbox_rx_irq_handler(channel, NULL) == 0 )
        {
            rx_irq_number[channel]--;
            total_rx_irq_number--;
            /*  signal firmware that descriptor is updated */
            mailbox_signal(channel, 0);
        }

        if ( ++channel == ppe_dev.dma.rx_total_channel_used )
            channel = 0;
    }
  #endif  //  defined(ENABLE_RX_QOS) && ENABLE_RX_QOS
	return IRQ_RETVAL(1);
}


