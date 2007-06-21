/*

  Broadcom BCM43xx wireless driver

  PIO Transmission

  Copyright (c) 2005 Michael Buesch <mb@bu3sch.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include "bcm43xx.h"
#include "bcm43xx_pio.h"
#include "bcm43xx_main.h"
#include "bcm43xx_xmit.h"
#include "bcm43xx_power.h"

#include <linux/delay.h>


static void tx_start(struct bcm43xx_pioqueue *queue)
{
	bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
			  BCM43xx_PIO_TXCTL_INIT);
}

static void tx_octet(struct bcm43xx_pioqueue *queue,
		     u8 octet)
{
	if (queue->need_workarounds) {
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXDATA,
				  octet);
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
				  BCM43xx_PIO_TXCTL_WRITELO);
	} else {
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
				  BCM43xx_PIO_TXCTL_WRITELO);
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXDATA,
				  octet);
	}
}

static u16 tx_get_next_word(const u8 *txhdr,
			    const u8 *packet,
			    size_t txhdr_size,
			    unsigned int *pos)
{
	const u8 *source;
	unsigned int i = *pos;
	u16 ret;

	if (i < txhdr_size) {
		source = txhdr;
	} else {
		source = packet;
		i -= txhdr_size;
	}
	ret = le16_to_cpu( *((u16 *)(source + i)) );
	*pos += 2;

	return ret;
}

static void tx_data(struct bcm43xx_pioqueue *queue,
		    u8 *txhdr,
		    const u8 *packet,
		    unsigned int octets)
{
	u16 data;
	unsigned int i = 0;

	if (queue->need_workarounds) {
		data = tx_get_next_word(txhdr, packet,
					sizeof(struct bcm43xx_txhdr_fw4), &i);
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXDATA, data);
	}
	bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
			  BCM43xx_PIO_TXCTL_WRITELO |
			  BCM43xx_PIO_TXCTL_WRITEHI);
	while (i < octets - 1) {
		data = tx_get_next_word(txhdr, packet,
					sizeof(struct bcm43xx_txhdr_fw4), &i);
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXDATA, data);
	}
	if (octets % 2)
		tx_octet(queue, packet[octets - sizeof(struct bcm43xx_txhdr_fw4) - 1]);
}

static void tx_complete(struct bcm43xx_pioqueue *queue,
			struct sk_buff *skb)
{
	if (queue->need_workarounds) {
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXDATA,
				  skb->data[skb->len - 1]);
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
				  BCM43xx_PIO_TXCTL_WRITELO |
				  BCM43xx_PIO_TXCTL_COMPLETE);
	} else {
		bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
				  BCM43xx_PIO_TXCTL_COMPLETE);
	}
}

static u16 generate_cookie(struct bcm43xx_pioqueue *queue,
			   struct bcm43xx_pio_txpacket *packet)
{
	u16 cookie = 0x0000;
	int packetindex;

	/* We use the upper 4 bits for the PIO
	 * controller ID and the lower 12 bits
	 * for the packet index (in the cache).
	 */
	switch (queue->mmio_base) {
	case BCM43xx_MMIO_PIO1_BASE:
		break;
	case BCM43xx_MMIO_PIO2_BASE:
		cookie = 0x1000;
		break;
	case BCM43xx_MMIO_PIO3_BASE:
		cookie = 0x2000;
		break;
	case BCM43xx_MMIO_PIO4_BASE:
		cookie = 0x3000;
		break;
	default:
		assert(0);
	}
	packetindex = pio_txpacket_getindex(packet);
	assert(((u16)packetindex & 0xF000) == 0x0000);
	cookie |= (u16)packetindex;

	return cookie;
}

static
struct bcm43xx_pioqueue * parse_cookie(struct bcm43xx_wldev *dev,
				       u16 cookie,
				       struct bcm43xx_pio_txpacket **packet)
{
	struct bcm43xx_pio *pio = &dev->pio;
	struct bcm43xx_pioqueue *queue = NULL;
	int packetindex;

	switch (cookie & 0xF000) {
	case 0x0000:
		queue = pio->queue0;
		break;
	case 0x1000:
		queue = pio->queue1;
		break;
	case 0x2000:
		queue = pio->queue2;
		break;
	case 0x3000:
		queue = pio->queue3;
		break;
	default:
		assert(0);
	}
	packetindex = (cookie & 0x0FFF);
	assert(packetindex >= 0 && packetindex < BCM43xx_PIO_MAXTXPACKETS);
	*packet = &(queue->tx_packets_cache[packetindex]);

	return queue;
}

union txhdr_union {
	struct bcm43xx_txhdr_fw4 txhdr_fw4;
};

static void pio_tx_write_fragment(struct bcm43xx_pioqueue *queue,
				  struct sk_buff *skb,
				  struct bcm43xx_pio_txpacket *packet,
				  size_t txhdr_size)
{
	union txhdr_union txhdr_data;
	u8 *txhdr = NULL;
	unsigned int octets;

	txhdr = (u8 *)(&txhdr_data.txhdr_fw4);

	assert(skb_shinfo(skb)->nr_frags == 0);
	bcm43xx_generate_txhdr(queue->dev,
			       txhdr, skb->data, skb->len,
			       &packet->txstat.control,
			       generate_cookie(queue, packet));

	tx_start(queue);
	octets = skb->len + txhdr_size;
	if (queue->need_workarounds)
		octets--;
	tx_data(queue, txhdr, (u8 *)skb->data, octets);
	tx_complete(queue, skb);
}

static void free_txpacket(struct bcm43xx_pio_txpacket *packet,
			  int irq_context)
{
	struct bcm43xx_pioqueue *queue = packet->queue;

	if (packet->skb) {
		if (irq_context)
			dev_kfree_skb_irq(packet->skb);
		else
			dev_kfree_skb(packet->skb);
	}
	list_move(&packet->list, &queue->txfree);
	queue->nr_txfree++;
}

static int pio_tx_packet(struct bcm43xx_pio_txpacket *packet)
{
	struct bcm43xx_pioqueue *queue = packet->queue;
	struct sk_buff *skb = packet->skb;
	u16 octets;

	octets = (u16)skb->len + sizeof(struct bcm43xx_txhdr_fw4);
	if (queue->tx_devq_size < octets) {
		printkl(KERN_WARNING PFX "PIO queue too small. "
					 "Dropping packet.\n");
		/* Drop it silently (return success) */
		free_txpacket(packet, 1);
		return 0;
	}
	assert(queue->tx_devq_packets <= BCM43xx_PIO_MAXTXDEVQPACKETS);
	assert(queue->tx_devq_used <= queue->tx_devq_size);
	/* Check if there is sufficient free space on the device
	 * TX queue. If not, return and let the TX tasklet
	 * retry later.
	 */
	if (queue->tx_devq_packets == BCM43xx_PIO_MAXTXDEVQPACKETS)
		return -EBUSY;
	if (queue->tx_devq_used + octets > queue->tx_devq_size)
		return -EBUSY;
	/* Now poke the device. */
	pio_tx_write_fragment(queue, skb, packet, sizeof(struct bcm43xx_txhdr_fw4));

	/* Account for the packet size.
	 * (We must not overflow the device TX queue)
	 */
	queue->tx_devq_packets++;
	queue->tx_devq_used += octets;

	/* Transmission started, everything ok, move the
	 * packet to the txrunning list.
	 */
	list_move_tail(&packet->list, &queue->txrunning);

	return 0;
}

static void tx_tasklet(unsigned long d)
{
	struct bcm43xx_pioqueue *queue = (struct bcm43xx_pioqueue *)d;
	struct bcm43xx_wldev *dev = queue->dev;
	unsigned long flags;
	struct bcm43xx_pio_txpacket *packet, *tmp_packet;
	int err;
	u16 txctl;

	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if (queue->tx_frozen)
		goto out_unlock;
	txctl = bcm43xx_pio_read(queue, BCM43xx_PIO_TXCTL);
	if (txctl & BCM43xx_PIO_TXCTL_SUSPEND)
		goto out_unlock;

	list_for_each_entry_safe(packet, tmp_packet, &queue->txqueue, list) {
		/* Try to transmit the packet. This can fail, if
		 * the device queue is full. In case of failure, the
		 * packet is left in the txqueue.
		 * If transmission succeed, the packet is moved to txrunning.
		 * If it is impossible to transmit the packet, it
		 * is dropped.
		 */
		err = pio_tx_packet(packet);
		if (err)
			break;
	}
out_unlock:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
}

static void setup_txqueues(struct bcm43xx_pioqueue *queue)
{
	struct bcm43xx_pio_txpacket *packet;
	int i;

	queue->nr_txfree = BCM43xx_PIO_MAXTXPACKETS;
	for (i = 0; i < BCM43xx_PIO_MAXTXPACKETS; i++) {
		packet = &(queue->tx_packets_cache[i]);

		packet->queue = queue;
		INIT_LIST_HEAD(&packet->list);

		list_add(&packet->list, &queue->txfree);
	}
}

static
struct bcm43xx_pioqueue * bcm43xx_setup_pioqueue(struct bcm43xx_wldev *dev,
						 u16 pio_mmio_base)
{
	struct bcm43xx_pioqueue *queue;
	u32 value;
	u16 qsize;

	queue = kzalloc(sizeof(*queue), GFP_KERNEL);
	if (!queue)
		goto out;

	queue->dev = dev;
	queue->mmio_base = pio_mmio_base;
	queue->need_workarounds = (dev->dev->id.revision < 3);

	INIT_LIST_HEAD(&queue->txfree);
	INIT_LIST_HEAD(&queue->txqueue);
	INIT_LIST_HEAD(&queue->txrunning);
	tasklet_init(&queue->txtask, tx_tasklet,
		     (unsigned long)queue);

	value = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	value &= ~BCM43xx_SBF_XFER_REG_BYTESWAP;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, value);

	qsize = bcm43xx_read16(dev, queue->mmio_base + BCM43xx_PIO_TXQBUFSIZE);
	if (qsize == 0) {
		printk(KERN_ERR PFX "ERROR: This card does not support PIO "
				    "operation mode. Please use DMA mode "
				    "(module parameter pio=0).\n");
		goto err_freequeue;
	}
	if (qsize <= BCM43xx_PIO_TXQADJUST) {
		printk(KERN_ERR PFX "PIO tx device-queue too small (%u)\n",
		       qsize);
		goto err_freequeue;
	}
	qsize -= BCM43xx_PIO_TXQADJUST;
	queue->tx_devq_size = qsize;

	setup_txqueues(queue);

out:
	return queue;

err_freequeue:
	kfree(queue);
	queue = NULL;
	goto out;
}

static void cancel_transfers(struct bcm43xx_pioqueue *queue)
{
	struct bcm43xx_pio_txpacket *packet, *tmp_packet;

	tasklet_disable(&queue->txtask);

	list_for_each_entry_safe(packet, tmp_packet, &queue->txrunning, list)
		free_txpacket(packet, 0);
	list_for_each_entry_safe(packet, tmp_packet, &queue->txqueue, list)
		free_txpacket(packet, 0);
}

static void bcm43xx_destroy_pioqueue(struct bcm43xx_pioqueue *queue)
{
	if (!queue)
		return;

	cancel_transfers(queue);
	kfree(queue);
}

void bcm43xx_pio_free(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_pio *pio;

	if (!bcm43xx_using_pio(dev))
		return;
	pio = &dev->pio;

	bcm43xx_destroy_pioqueue(pio->queue3);
	pio->queue3 = NULL;
	bcm43xx_destroy_pioqueue(pio->queue2);
	pio->queue2 = NULL;
	bcm43xx_destroy_pioqueue(pio->queue1);
	pio->queue1 = NULL;
	bcm43xx_destroy_pioqueue(pio->queue0);
	pio->queue0 = NULL;
}

int bcm43xx_pio_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_pio *pio = &dev->pio;
	struct bcm43xx_pioqueue *queue;
	int err = -ENOMEM;

	queue = bcm43xx_setup_pioqueue(dev, BCM43xx_MMIO_PIO1_BASE);
	if (!queue)
		goto out;
	pio->queue0 = queue;

	queue = bcm43xx_setup_pioqueue(dev, BCM43xx_MMIO_PIO2_BASE);
	if (!queue)
		goto err_destroy0;
	pio->queue1 = queue;

	queue = bcm43xx_setup_pioqueue(dev, BCM43xx_MMIO_PIO3_BASE);
	if (!queue)
		goto err_destroy1;
	pio->queue2 = queue;

	queue = bcm43xx_setup_pioqueue(dev, BCM43xx_MMIO_PIO4_BASE);
	if (!queue)
		goto err_destroy2;
	pio->queue3 = queue;

	if (dev->dev->id.revision < 3)
		dev->irq_savedstate |= BCM43xx_IRQ_PIO_WORKAROUND;

	dprintk(KERN_INFO PFX "PIO initialized\n");
	err = 0;
out:
	return err;

err_destroy2:
	bcm43xx_destroy_pioqueue(pio->queue2);
	pio->queue2 = NULL;
err_destroy1:
	bcm43xx_destroy_pioqueue(pio->queue1);
	pio->queue1 = NULL;
err_destroy0:
	bcm43xx_destroy_pioqueue(pio->queue0);
	pio->queue0 = NULL;
	goto out;
}

int bcm43xx_pio_tx(struct bcm43xx_wldev *dev,
		   struct sk_buff *skb,
		   struct ieee80211_tx_control *ctl)
{
	struct bcm43xx_pioqueue *queue = dev->pio.queue1;
	struct bcm43xx_pio_txpacket *packet;

	assert(!queue->tx_suspended);
	assert(!list_empty(&queue->txfree));

	packet = list_entry(queue->txfree.next, struct bcm43xx_pio_txpacket, list);
	packet->skb = skb;

	memset(&packet->txstat, 0, sizeof(packet->txstat));
	memcpy(&packet->txstat.control, ctl, sizeof(*ctl));

	list_move_tail(&packet->list, &queue->txqueue);
	queue->nr_txfree--;
	queue->nr_tx_packets++;
	assert(queue->nr_txfree < BCM43xx_PIO_MAXTXPACKETS);

	tasklet_schedule(&queue->txtask);

	return 0;
}

void bcm43xx_pio_handle_txstatus(struct bcm43xx_wldev *dev,
				 const struct bcm43xx_txstatus *status)
{
	struct bcm43xx_pioqueue *queue;
	struct bcm43xx_pio_txpacket *packet;

	queue = parse_cookie(dev, status->cookie, &packet);
	assert(queue);

	queue->tx_devq_packets--;
	queue->tx_devq_used -= (packet->skb->len + sizeof(struct bcm43xx_txhdr_fw4));

	if (status->acked)
		packet->txstat.flags |= IEEE80211_TX_STATUS_ACK;
	packet->txstat.retry_count = status->frame_count - 1;
	ieee80211_tx_status_irqsafe(dev->wl->hw, packet->skb,
				    &(packet->txstat));
	packet->skb = NULL;

	free_txpacket(packet, 1);
	/* If there are packets on the txqueue, poke the tasklet
	 * to transmit them.
	 */
	if (!list_empty(&queue->txqueue))
		tasklet_schedule(&queue->txtask);
}

void bcm43xx_pio_get_tx_stats(struct bcm43xx_wldev *dev,
			      struct ieee80211_tx_queue_stats *stats)
{
	struct bcm43xx_pio *pio = &dev->pio;
	struct bcm43xx_pioqueue *queue;
	struct ieee80211_tx_queue_stats_data *data;

	queue = pio->queue1;
	data = &(stats->data[0]);
	data->len = BCM43xx_PIO_MAXTXPACKETS - queue->nr_txfree;
	data->limit = BCM43xx_PIO_MAXTXPACKETS;
	data->count = queue->nr_tx_packets;
}

static void pio_rx_error(struct bcm43xx_pioqueue *queue,
			 int clear_buffers,
			 const char *error)
{
	int i;

	printkl("PIO RX error: %s\n", error);
	bcm43xx_pio_write(queue, BCM43xx_PIO_RXCTL,
			  BCM43xx_PIO_RXCTL_READY);
	if (clear_buffers) {
		assert(queue->mmio_base == BCM43xx_MMIO_PIO1_BASE);
		for (i = 0; i < 15; i++) {
			/* Dummy read. */
			bcm43xx_pio_read(queue, BCM43xx_PIO_RXDATA);
		}
	}
}

void bcm43xx_pio_rx(struct bcm43xx_pioqueue *queue)
{
	u16 preamble[21] = { 0 };
	struct bcm43xx_rxhdr_fw4 *rxhdr;
	u16 tmp, len, macstat;
	int i, preamble_readwords;
	struct sk_buff *skb;

	tmp = bcm43xx_pio_read(queue, BCM43xx_PIO_RXCTL);
	if (!(tmp & BCM43xx_PIO_RXCTL_DATAAVAILABLE))
		return;
	bcm43xx_pio_write(queue, BCM43xx_PIO_RXCTL,
			  BCM43xx_PIO_RXCTL_DATAAVAILABLE);

	for (i = 0; i < 10; i++) {
		tmp = bcm43xx_pio_read(queue, BCM43xx_PIO_RXCTL);
		if (tmp & BCM43xx_PIO_RXCTL_READY)
			goto data_ready;
		udelay(10);
	}
	dprintkl(KERN_ERR PFX "PIO RX timed out\n");
	return;
data_ready:

	len = bcm43xx_pio_read(queue, BCM43xx_PIO_RXDATA);
	if (unlikely(len > 0x700)) {
		pio_rx_error(queue, 0, "len > 0x700");
		return;
	}
	if (unlikely(len == 0 && queue->mmio_base != BCM43xx_MMIO_PIO4_BASE)) {
		pio_rx_error(queue, 0, "len == 0");
		return;
	}
	preamble[0] = cpu_to_le16(len);
	if (queue->mmio_base == BCM43xx_MMIO_PIO4_BASE)
		preamble_readwords = 14 / sizeof(u16);
	else
		preamble_readwords = 18 / sizeof(u16);
	for (i = 0; i < preamble_readwords; i++) {
		tmp = bcm43xx_pio_read(queue, BCM43xx_PIO_RXDATA);
		preamble[i + 1] = cpu_to_le16(tmp);
	}
	rxhdr = (struct bcm43xx_rxhdr_fw4 *)preamble;
	macstat = le16_to_cpu(rxhdr->mac_status);
	if (macstat & BCM43xx_RX_MAC_FCSERR) {
		pio_rx_error(queue,
			     (queue->mmio_base == BCM43xx_MMIO_PIO1_BASE),
			     "Frame FCS error");
		return;
	}
	if (queue->mmio_base == BCM43xx_MMIO_PIO4_BASE) {
		/* We received an xmit status. */
		struct bcm43xx_hwtxstatus *hw;

		hw = (struct bcm43xx_hwtxstatus *)(preamble + 1);
		bcm43xx_handle_hwtxstatus(queue->dev, hw);

		return;
	}

	skb = dev_alloc_skb(len);
	if (unlikely(!skb)) {
		pio_rx_error(queue, 1, "OOM");
		return;
	}
	skb_put(skb, len);
	for (i = 0; i < len - 1; i += 2) {
		tmp = bcm43xx_pio_read(queue, BCM43xx_PIO_RXDATA);
		*((u16 *)(skb->data + i)) = cpu_to_le16(tmp);
	}
	if (len % 2) {
		tmp = bcm43xx_pio_read(queue, BCM43xx_PIO_RXDATA);
		skb->data[len - 1] = (tmp & 0x00FF);
/* The specs say the following is required, but
 * it is wrong and corrupts the PLCP. If we don't do
 * this, the PLCP seems to be correct. So ifdef it out for now.
 */
#if 0
		if (rxflags2 & BCM43xx_RXHDR_FLAGS2_TYPE2FRAME)
			skb->data[2] = (tmp & 0xFF00) >> 8;
		else
			skb->data[0] = (tmp & 0xFF00) >> 8;
#endif
	}
	bcm43xx_rx(queue->dev, skb, rxhdr);
}

void bcm43xx_pio_tx_suspend(struct bcm43xx_pioqueue *queue)
{
	bcm43xx_power_saving_ctl_bits(queue->dev, -1, 1);
	bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
			  bcm43xx_pio_read(queue, BCM43xx_PIO_TXCTL)
			  | BCM43xx_PIO_TXCTL_SUSPEND);
}

void bcm43xx_pio_tx_resume(struct bcm43xx_pioqueue *queue)
{
	bcm43xx_pio_write(queue, BCM43xx_PIO_TXCTL,
			  bcm43xx_pio_read(queue, BCM43xx_PIO_TXCTL)
			  & ~BCM43xx_PIO_TXCTL_SUSPEND);
	bcm43xx_power_saving_ctl_bits(queue->dev, -1, -1);
	tasklet_schedule(&queue->txtask);
}

void bcm43xx_pio_freeze_txqueues(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_pio *pio;

	assert(bcm43xx_using_pio(dev));
	pio = &dev->pio;
	pio->queue0->tx_frozen = 1;
	pio->queue1->tx_frozen = 1;
	pio->queue2->tx_frozen = 1;
	pio->queue3->tx_frozen = 1;
}

void bcm43xx_pio_thaw_txqueues(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_pio *pio;

	assert(bcm43xx_using_pio(dev));
	pio = &dev->pio;
	pio->queue0->tx_frozen = 0;
	pio->queue1->tx_frozen = 0;
	pio->queue2->tx_frozen = 0;
	pio->queue3->tx_frozen = 0;
	if (!list_empty(&pio->queue0->txqueue))
		tasklet_schedule(&pio->queue0->txtask);
	if (!list_empty(&pio->queue1->txqueue))
		tasklet_schedule(&pio->queue1->txtask);
	if (!list_empty(&pio->queue2->txqueue))
		tasklet_schedule(&pio->queue2->txtask);
	if (!list_empty(&pio->queue3->txqueue))
		tasklet_schedule(&pio->queue3->txtask);
}
