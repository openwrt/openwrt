/*
 * originally drivers/net/tulip/interrupt.c
 *   Copyright 2000,2001  The Linux Kernel Team
 *   Written/copyright 1994-2001 by Donald Becker.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include "net.h"

int tulip_refill_rx(struct net_device *dev)
{
	struct tulip_private *tp = netdev_priv(dev);
	int entry;
	int refilled = 0;

	/* Refill the Rx ring buffers. */
	for (; tp->cur_rx - tp->dirty_rx > 0; tp->dirty_rx++) {
		entry = tp->dirty_rx % RX_RING_SIZE;
		if (tp->rx_buffers[entry].skb == NULL) {
			struct sk_buff *skb;
			dma_addr_t mapping;

			skb = tp->rx_buffers[entry].skb = dev_alloc_skb(PKT_BUF_SZ);
			if (skb == NULL)
				break;

			mapping = dma_map_single(&dev->dev, skb->data,
				PKT_BUF_SZ, DMA_FROM_DEVICE);
			tp->rx_buffers[entry].mapping = mapping;

			skb->dev = dev;			/* Mark as being used by this device. */
			tp->rx_ring[entry].buffer1 = cpu_to_le32(mapping);
			refilled++;
		}
		tp->rx_ring[entry].status = cpu_to_le32(DescOwned);
	}
	return refilled;
}

void oom_timer(unsigned long data)
{
        struct net_device *dev = (struct net_device *)data;
	struct tulip_private *tp = netdev_priv(dev);
	napi_schedule(&tp->napi);
}

int tulip_poll(struct napi_struct *napi, int budget)
{
	struct tulip_private *tp = container_of(napi, struct tulip_private, napi);
	struct net_device *dev = tp->dev;
	int entry = tp->cur_rx % RX_RING_SIZE;
	int work_done = 0;

	if (tulip_debug > 4)
		printk(KERN_DEBUG " In tulip_rx(), entry %d %08x\n",
		       entry, tp->rx_ring[entry].status);

       do {
		if (ioread32(tp->base_addr + CSR5) == 0xffffffff) {
			printk(KERN_DEBUG " In tulip_poll(), hardware disappeared\n");
			break;
		}
               /* Acknowledge current RX interrupt sources. */
               iowrite32((RxIntr | RxNoBuf), tp->base_addr + CSR5);


               /* If we own the next entry, it is a new packet. Send it up. */
               while ( ! (tp->rx_ring[entry].status & cpu_to_le32(DescOwned))) {
                       s32 status = le32_to_cpu(tp->rx_ring[entry].status);
		       short pkt_len;

                       if (tp->dirty_rx + RX_RING_SIZE == tp->cur_rx)
                               break;

                       if (tulip_debug > 5)
                               printk(KERN_DEBUG "%s: In tulip_rx(), entry %d %08x\n",
                                      dev->name, entry, status);

		       if (++work_done >= budget)
                               goto not_done;

		       /*
			* Omit the four octet CRC from the length.
			* (May not be considered valid until we have
			* checked status for RxLengthOver2047 bits)
			*/
		       pkt_len = ((status >> 16) & 0x7ff) - 4;

#if 0
			csr6 = ioread32(tp->base_addr + CSR6);
				if (csr6 & 0x1)
					pkt_len += 2;

#endif
		       /*
			* Maximum pkt_len is 1518 (1514 + vlan header)
			* Anything higher than this is always invalid
			* regardless of RxLengthOver2047 bits
			*/

		       if ((status & (RxLengthOver2047 |
				      RxDescCRCError |
				      RxDescCollisionSeen |
				      RxDescRunt |
				      RxDescDescErr |
				      RxWholePkt)) != RxWholePkt ||
			   pkt_len > 1518) {
			       if ((status & (RxLengthOver2047 |
					      RxWholePkt)) != RxWholePkt) {
                                /* Ingore earlier buffers. */
                                       if ((status & 0xffff) != 0x7fff) {
                                               if (tulip_debug > 1)
                                                       dev_warn(&dev->dev,
								"Oversized Ethernet frame spanned multiple buffers, status %08x!\n",
								status);
                                               tp->stats.rx_length_errors++;
                                       }
			       } else {
                                /* There was a fatal error. */
                                       if (tulip_debug > 2)
                                               printk(KERN_DEBUG "%s: Receive error, Rx status %08x\n",
                                                      dev->name, status);
                                       tp->stats.rx_errors++; /* end of a packet.*/
				       if (pkt_len > 1518 ||
					   (status & RxDescRunt))
					       tp->stats.rx_length_errors++;

                                       if (status & 0x0004) tp->stats.rx_frame_errors++;
                                       if (status & 0x0002) tp->stats.rx_crc_errors++;
                                       if (status & 0x0001) tp->stats.rx_fifo_errors++;
                               }
                       } else {
                               struct sk_buff *skb = tp->rx_buffers[entry].skb;
                               char *temp = skb_put(skb, pkt_len);

#if 0
				if (csr6 & 1)
					skb_pull(skb, 2);
#endif
#ifndef final_version
                                       if (tp->rx_buffers[entry].mapping !=
						le32_to_cpu(tp->rx_ring[entry].buffer1)) {
                                               dev_err(&dev->dev,
						       "Internal fault: The skbuff addresses do not match in tulip_rx: %08x vs. %08llx %p / %p\n",
						       le32_to_cpu(tp->rx_ring[entry].buffer1),
						       (unsigned long long)tp->rx_buffers[entry].mapping,
						       skb->head, temp);
                                       }
#endif

                                       tp->rx_buffers[entry].skb = NULL;
                                       tp->rx_buffers[entry].mapping = 0;
                               skb->protocol = eth_type_trans(skb, dev);

                               netif_receive_skb(skb);

                               tp->stats.rx_packets++;
                               tp->stats.rx_bytes += pkt_len;
                       }
                       entry = (++tp->cur_rx) % RX_RING_SIZE;
                       if (tp->cur_rx - tp->dirty_rx > RX_RING_SIZE/4)
                               tulip_refill_rx(dev);

                }

               /* New ack strategy... irq does not ack Rx any longer
                  hopefully this helps */

               /* Really bad things can happen here... If new packet arrives
                * and an irq arrives (tx or just due to occasionally unset
                * mask), it will be acked by irq handler, but new thread
                * is not scheduled. It is major hole in design.
                * No idea how to fix this if "playing with fire" will fail
                * tomorrow (night 011029). If it will not fail, we won
                * finally: amount of IO did not increase at all. */
       } while ((ioread32(tp->base_addr + CSR5) & RxIntr));

         tulip_refill_rx(dev);

         /* If RX ring is not full we are out of memory. */
         if (tp->rx_buffers[tp->dirty_rx % RX_RING_SIZE].skb == NULL)
		 goto oom;

         /* Remove us from polling list and enable RX intr. */
         napi_complete(napi);
         iowrite32(VALID_INTR, tp->base_addr+CSR7);

         /* The last op happens after poll completion. Which means the following:
          * 1. it can race with disabling irqs in irq handler
          * 2. it can race with dise/enabling irqs in other poll threads
          * 3. if an irq raised after beginning loop, it will be immediately
          *    triggered here.
          *
          * Summarizing: the logic results in some redundant irqs both
          * due to races in masking and due to too late acking of already
          * processed irqs. But it must not result in losing events.
          */

         return work_done;

 not_done:
         if (tp->cur_rx - tp->dirty_rx > RX_RING_SIZE/2 ||
             tp->rx_buffers[tp->dirty_rx % RX_RING_SIZE].skb == NULL)
                 tulip_refill_rx(dev);

         if (tp->rx_buffers[tp->dirty_rx % RX_RING_SIZE].skb == NULL)
		 goto oom;

         return work_done;

 oom:    /* Executed with RX ints disabled */

         /* Start timer, stop polling, but do not enable rx interrupts. */
         mod_timer(&tp->oom_timer, jiffies+1);

         /* Think: timer_pending() was an explicit signature of bug.
          * Timer can be pending now but fired and completed
          * before we did napi_complete(). See? We would lose it. */

         /* remove ourselves from the polling list */
         napi_complete(napi);

         return work_done;
}

/* The interrupt handler does all of the Rx thread work and cleans up
   after the Tx thread. */
irqreturn_t tulip_interrupt(int irq, void *dev_instance)
{
	struct net_device *dev = (struct net_device *)dev_instance;
	struct tulip_private *tp = netdev_priv(dev);
	void __iomem *ioaddr = tp->base_addr;
	int csr5;
	int missed;
	int rx = 0;
	int tx = 0;
	int oi = 0;
	int maxrx = RX_RING_SIZE;
	int maxtx = TX_RING_SIZE;
	int maxoi = TX_RING_SIZE;
	int rxd = 0;
	unsigned int work_count = 25;
	unsigned int handled = 0;

	/* Let's see whether the interrupt really is for us */
	csr5 = ioread32(ioaddr + CSR5);

	if ((csr5 & (NormalIntr|AbnormalIntr)) == 0)
		return IRQ_RETVAL(handled);

	tp->nir++;

	do {

		if (!rxd && (csr5 & (RxIntr | RxNoBuf))) {
			rxd++;
			/* Mask RX intrs and add the device to poll list. */
			iowrite32(VALID_INTR&~RxPollInt, ioaddr + CSR7);
			napi_schedule(&tp->napi);

			if (!(csr5&~(AbnormalIntr|NormalIntr|RxPollInt|TPLnkPass)))
                               break;
		}

               /* Acknowledge the interrupt sources we handle here ASAP
                  the poll function does Rx and RxNoBuf acking */

		iowrite32(csr5 & 0x0001ff3f, ioaddr + CSR5);

		if (tulip_debug > 4)
			printk(KERN_DEBUG "%s: interrupt  csr5=%#8.8x new csr5=%#8.8x\n",
			       dev->name, csr5, ioread32(ioaddr + CSR5));


		if (csr5 & (TxNoBuf | TxDied | TxIntr | TimerInt)) {
			unsigned int dirty_tx;

			spin_lock(&tp->lock);

			for (dirty_tx = tp->dirty_tx; tp->cur_tx - dirty_tx > 0;
				 dirty_tx++) {
				int entry = dirty_tx % TX_RING_SIZE;
				int status = le32_to_cpu(tp->tx_ring[entry].status);

				if (status < 0)
					break;			/* It still has not been Txed */

				if (status & 0x8000) {
					/* There was an major error, log it. */
#ifndef final_version
					if (tulip_debug > 1)
						printk(KERN_DEBUG "%s: Transmit error, Tx status %08x\n",
						       dev->name, status);
#endif
					tp->stats.tx_errors++;
					if (status & 0x4104) tp->stats.tx_aborted_errors++;
					if (status & 0x0C00) tp->stats.tx_carrier_errors++;
					if (status & 0x0200) tp->stats.tx_window_errors++;
					if (status & 0x0002) tp->stats.tx_fifo_errors++;
					if (status & 0x0080) tp->stats.tx_heartbeat_errors++;
				} else {
					tp->stats.tx_bytes +=
						tp->tx_buffers[entry].skb->len;
					tp->stats.collisions += (status >> 3) & 15;
					tp->stats.tx_packets++;
				}

				dma_unmap_single(&tp->pdev->dev, tp->tx_buffers[entry].mapping,
					tp->tx_buffers[entry].skb->len, DMA_TO_DEVICE);
				/* Free the original skb. */
				dev_kfree_skb_irq(tp->tx_buffers[entry].skb);
				tp->tx_buffers[entry].skb = NULL;
				tp->tx_buffers[entry].mapping = 0;
				tx++;
			}

#ifndef final_version
			if (tp->cur_tx - dirty_tx > TX_RING_SIZE) {
				dev_err(&dev->dev,
					"Out-of-sync dirty pointer, %d vs. %d\n",
					dirty_tx, tp->cur_tx);
				dirty_tx += TX_RING_SIZE;
			}
#endif

			if (tp->cur_tx - dirty_tx < TX_RING_SIZE - 2)
				netif_wake_queue(dev);

			tp->dirty_tx = dirty_tx;
			if (csr5 & TxDied) {
				if (tulip_debug > 2)
					dev_warn(&dev->dev,
						 "The transmitter stopped.  CSR5 is %x, CSR6 %x, new CSR6 %x\n",
						 csr5, ioread32(ioaddr + CSR6),
						 tp->csr6);
				tulip_restart_rxtx(tp);
			}
			spin_unlock(&tp->lock);
		}

		/* Log errors. */
		if (csr5 & AbnormalIntr) {	/* Abnormal error summary bit. */
			if (csr5 == 0xffffffff)
				break;
			if (csr5 & TxJabber) tp->stats.tx_errors++;
			if (csr5 & TxFIFOUnderflow) {
				if ((tp->csr6 & 0xC000) != 0xC000)
					tp->csr6 += 0x4000;	/* Bump up the Tx threshold */
				else
					tp->csr6 |= 0x00200000;  /* Store-n-forward. */
				/* Restart the transmit process. */
				tulip_restart_rxtx(tp);
				iowrite32(0, ioaddr + CSR1);
			}
			if (csr5 & (RxDied | RxNoBuf)) {
				iowrite32(tp->mc_filter[0], ioaddr + CSR27);
				iowrite32(tp->mc_filter[1], ioaddr + CSR28);
			}
			if (csr5 & RxDied) {		/* Missed a Rx frame. */
                                tp->stats.rx_missed_errors += ioread32(ioaddr + CSR8) & 0xffff;
				tp->stats.rx_errors++;
				tulip_start_rxtx(tp);
			}
			/*
			 * NB: t21142_lnk_change() does a del_timer_sync(), so be careful if this
			 * call is ever done under the spinlock
			 */
			if (csr5 & (TPLnkPass | TPLnkFail | 0x08000000)) {
				if (tp->link_change)
					(tp->link_change)(dev, csr5);
			}
			if (csr5 & SystemError) {
				int error = (csr5 >> 23) & 7;
				/* oops, we hit a PCI error.  The code produced corresponds
				 * to the reason:
				 *  0 - parity error
				 *  1 - master abort
				 *  2 - target abort
				 * Note that on parity error, we should do a software reset
				 * of the chip to get it back into a sane state (according
				 * to the 21142/3 docs that is).
				 *   -- rmk
				 */
				dev_err(&dev->dev,
					"(%lu) System Error occurred (%d)\n",
					tp->nir, error);
			}
			/* Clear all error sources, included undocumented ones! */
			iowrite32(0x0800f7ba, ioaddr + CSR5);
			oi++;
		}
		if (csr5 & TimerInt) {

			if (tulip_debug > 2)
				dev_err(&dev->dev,
					"Re-enabling interrupts, %08x\n",
					csr5);
			iowrite32(VALID_INTR, ioaddr + CSR7);
			oi++;
		}
		if (tx > maxtx || rx > maxrx || oi > maxoi) {
			if (tulip_debug > 1)
				dev_warn(&dev->dev, "Too much work during an interrupt, csr5=0x%08x. (%lu) (%d,%d,%d)\n",
					 csr5, tp->nir, tx, rx, oi);

                       /* Acknowledge all interrupt sources. */
                        iowrite32(0x8001ffff, ioaddr + CSR5);
                        /* Mask all interrupting sources, set timer to
				re-enable. */
                        iowrite32(((~csr5) & 0x0001ebef) | AbnormalIntr | TimerInt, ioaddr + CSR7);
                        iowrite32(0x0012, ioaddr + CSR11);
			break;
		}

		work_count--;
		if (work_count == 0)
			break;

		csr5 = ioread32(ioaddr + CSR5);

		if (rxd)
			csr5 &= ~RxPollInt;
	} while ((csr5 & (TxNoBuf |
			  TxDied |
			  TxIntr |
			  TimerInt |
			  /* Abnormal intr. */
			  RxDied |
			  TxFIFOUnderflow |
			  TxJabber |
			  TPLnkFail |
			  SystemError )) != 0);

	if ((missed = ioread32(ioaddr + CSR8) & 0x1ffff)) {
		tp->stats.rx_dropped += missed & 0x10000 ? 0x10000 : missed;
	}

	if (tulip_debug > 4)
		printk(KERN_DEBUG "%s: exiting interrupt, csr5=%#04x\n",
		       dev->name, ioread32(ioaddr + CSR5));

	return IRQ_HANDLED;
}
