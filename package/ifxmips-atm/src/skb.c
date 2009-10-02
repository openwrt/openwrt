#include <linux/skbuff.h>

#include "common.h"

void resize_skb_rx(struct sk_buff *skb, unsigned int size, int is_cell)
{
    if((u32)skb < 0x80000000)
    {
        int key = 0;
        printk("resize_skb_rx problem: skb = %08X, size = %d, is_cell = %d\n", (u32)skb, size, is_cell);
        while(!key){}
    }
    skb->data = (unsigned char*)(((u32)skb->head + 16 + (DMA_ALIGNMENT - 1)) & ~(DMA_ALIGNMENT - 1));
    skb->tail = skb->data;

    /*  Set up other state  */
    skb->len = 0;
    skb->cloned = 0;
#if defined(CONFIG_IMQ) || defined (CONFIG_IMQ_MODULE)
    skb->imq_flags = 0;
    skb->nf_info = NULL;
#endif
    skb->data_len = 0;
}

struct sk_buff* alloc_skb_rx(void)
{
    struct sk_buff *skb;

    /*  allocate memroy including trailer and padding   */
    skb = dev_alloc_skb(ppe_dev.aal5.rx_buffer_size + DMA_ALIGNMENT);
    if (skb)
    {
        /*  must be burst length alignment  */
        if ( ((u32)skb->data & (DMA_ALIGNMENT - 1)) != 0 )
            skb_reserve(skb, ~((u32)skb->data + (DMA_ALIGNMENT - 1)) & (DMA_ALIGNMENT - 1));
        /*  put skb in reserved area "skb->data - 4"    */
        *((u32*)skb->data - 1) = (u32)skb;
        /*  invalidate cache    */
        dma_cache_inv((unsigned long)skb->head, (u32)skb->end - (u32)skb->head);
    }
    return skb;
}

void atm_free_tx_skb_vcc(struct sk_buff *skb)
{
    struct atm_vcc* vcc;

    if ( (u32)skb <= 0x80000000 )
    {
        volatile int key = 0;
        printk("atm_free_tx_skb_vcc: skb = %08X\n", (u32)skb);
        for ( ; !key; );
    }

    vcc = ATM_SKB(skb)->vcc;
    if ( vcc != NULL && vcc->pop != NULL )
    {
        if ( atomic_read(&skb->users) == 0 )
        {
            volatile int key = 0;
            printk("atm_free_tx_skb_vcc(vcc->pop): skb->users == 0, skb = %08X\n", (u32)skb);
            for ( ; !key; );
        }
        vcc->pop(vcc, skb);
    }
    else
    {
        if ( atomic_read(&skb->users) == 0 )
        {
            volatile int key = 0;
            printk("atm_free_tx_skb_vcc(dev_kfree_skb_any): skb->users == 0, skb = %08X\n", (u32)skb);
            for ( ; !key; );
        }
        dev_kfree_skb_any(skb);
    }
}

struct sk_buff* alloc_skb_tx(unsigned int size)
{
    struct sk_buff *skb;

    /*  allocate memory including header and padding    */
    size += TX_INBAND_HEADER_LENGTH + MAX_TX_PACKET_ALIGN_BYTES + MAX_TX_PACKET_PADDING_BYTES;
    size &= ~(DMA_ALIGNMENT - 1);
    skb = dev_alloc_skb(size + DMA_ALIGNMENT);
    /*  must be burst length alignment  */
    if ( skb )
        skb_reserve(skb, (~((u32)skb->data + (DMA_ALIGNMENT - 1)) & (DMA_ALIGNMENT - 1)) + TX_INBAND_HEADER_LENGTH);
    return skb;
}

struct sk_buff* atm_alloc_tx(struct atm_vcc *vcc, unsigned int size)
{
    int conn;
    struct sk_buff *skb;

    /*  oversize packet */
    if ( ((size + TX_INBAND_HEADER_LENGTH + MAX_TX_PACKET_ALIGN_BYTES + MAX_TX_PACKET_PADDING_BYTES) & ~(DMA_ALIGNMENT - 1))  > ppe_dev.aal5.tx_max_packet_size )
    {
        printk("atm_alloc_tx: oversize packet\n");
        return NULL;
    }
    /*  send buffer overflow    */
    if ( atomic_read(&vcc->sk.sk_wmem_alloc) && !atm_may_send(vcc, size) )
    {
        printk("atm_alloc_tx: send buffer overflow\n");
        return NULL;
    }
    conn = find_vcc(vcc);
    if ( conn < 0 )
    {
        printk("atm_alloc_tx: unknown VCC\n");
        return NULL;
    }

    skb = dev_alloc_skb(size);
    if ( skb == NULL )
    {
        printk("atm_alloc_tx: sk buffer is used up\n");
        return NULL;
    }
#define ATM_PDU_OVHD 0
    atomic_add(skb->truesize + ATM_PDU_OVHD, &vcc->sk.sk_wmem_alloc);

    return skb;
}

