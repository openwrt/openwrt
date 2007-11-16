/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     Driver for the IDT RC32434 on-chip ethernet controller.
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *  Copyright 2006 Felix Fietkau <nbd@openwrt.org>
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb
 *
 * Based on the driver developed by B. Maruthanayakam, H. Kou and others.
 *
 * Aug 2004 Sadik
 *
 * Added NAPI
 *
 **************************************************************************
 */

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/proc_fs.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <asm/bootinfo.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/pgtable.h>
#include <asm/segment.h>
#include <asm/io.h>
#include <asm/dma.h>

#include <asm/rc32434/rb.h>
#include "rc32434_eth.h"

#define DRIVER_VERSION "(mar2904)"

#define DRIVER_NAME "rc32434 Ethernet driver. " DRIVER_VERSION

#define STATION_ADDRESS_HIGH(dev) (((dev)->dev_addr[0] << 8) | \
			           ((dev)->dev_addr[1]))
#define STATION_ADDRESS_LOW(dev)  (((dev)->dev_addr[2] << 24) | \
				   ((dev)->dev_addr[3] << 16) | \
				   ((dev)->dev_addr[4] << 8)  | \
				   ((dev)->dev_addr[5]))

#define MII_CLOCK 1250000 				/* no more than 2.5MHz */
#define CONFIG_IDT_USE_NAPI 1


static inline void rc32434_abort_tx(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	rc32434_abort_dma(dev, lp->tx_dma_regs);
	
}

static inline void rc32434_abort_rx(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	rc32434_abort_dma(dev, lp->rx_dma_regs);
	
}

static inline void rc32434_start_tx(struct rc32434_local *lp,  volatile DMAD_t td)
{
	rc32434_start_dma(lp->tx_dma_regs, CPHYSADDR(td));
}

static inline void rc32434_start_rx(struct rc32434_local *lp, volatile DMAD_t rd)
{
	rc32434_start_dma(lp->rx_dma_regs, CPHYSADDR(rd));
}

static inline void rc32434_chain_tx(struct rc32434_local *lp, volatile DMAD_t td)
{
	rc32434_chain_dma(lp->tx_dma_regs, CPHYSADDR(td));
}

static inline void rc32434_chain_rx(struct rc32434_local *lp, volatile DMAD_t rd)
{
	rc32434_chain_dma(lp->rx_dma_regs, CPHYSADDR(rd));
}

#ifdef RC32434_PROC_DEBUG
static int rc32434_read_proc(char *buf, char **start, off_t fpos,
			     int length, int *eof, void *data)
{
	struct net_device *dev = (struct net_device *)data;
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	int len = 0;
	
	/* print out header */
	len += sprintf(buf + len, "\n\tKorina Ethernet Debug\n\n");
	len += sprintf (buf + len,
			"DMA halt count      = %10d, DMA run count = %10d\n",
			lp->dma_halt_cnt, lp->dma_run_cnt);
	
	if (fpos >= len) {
		*start = buf;
		*eof = 1;
		return 0;
	}
	*start = buf + fpos;
	
	if ((len -= fpos) > length) 
		return length;	
	*eof = 1;
	
	return len;
	
}
#endif


/*
 * Restart the RC32434 ethernet controller. 
 */
static int rc32434_restart(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	
	/*
	 * Disable interrupts
	 */
	disable_irq(lp->rx_irq);
	disable_irq(lp->tx_irq);
#ifdef	RC32434_REVISION
	disable_irq(lp->ovr_irq);
#endif	
	disable_irq(lp->und_irq);
	
	/* Mask F E bit in Tx DMA */
	__raw_writel(__raw_readl(&lp->tx_dma_regs->dmasm) | DMASM_f_m | DMASM_e_m, &lp->tx_dma_regs->dmasm);
	/* Mask D H E bit in Rx DMA */
	__raw_writel(__raw_readl(&lp->rx_dma_regs->dmasm) | DMASM_d_m | DMASM_h_m | DMASM_e_m, &lp->rx_dma_regs->dmasm);
	
	rc32434_init(dev);
	rc32434_multicast_list(dev);
	
	enable_irq(lp->und_irq);
#ifdef	RC32434_REVISION
	enable_irq(lp->ovr_irq);
#endif
	enable_irq(lp->tx_irq);
	enable_irq(lp->rx_irq);
	
	return 0;
}

static int rc32434_probe(struct platform_device *pdev)
{
	struct korina_device *bif = (struct korina_device *) pdev->dev.platform_data;
	struct rc32434_local *lp = NULL;
	struct net_device *dev = NULL;
	struct resource *r;
	int i, retval,err;
	
	dev = alloc_etherdev(sizeof(struct rc32434_local));
	if(!dev) {
		ERR("Korina_eth: alloc_etherdev failed\n");
		return -1;
	}

	platform_set_drvdata(pdev, dev);
	SET_MODULE_OWNER(dev);
	bif->dev = dev;
	
	memcpy(dev->dev_addr, bif->mac, 6);

	/* Initialize the device structure. */
	if (dev->priv == NULL) {
		lp = (struct rc32434_local *)kmalloc(sizeof(*lp), GFP_KERNEL);
		memset(lp, 0, sizeof(struct rc32434_local));
	} 
	else {
		lp = (struct rc32434_local *)dev->priv;
	}
	
	lp->rx_irq = platform_get_irq_byname(pdev, "korina_rx");
	lp->tx_irq = platform_get_irq_byname(pdev, "korina_tx");
	lp->ovr_irq = platform_get_irq_byname(pdev, "korina_ovr");
	lp->und_irq = platform_get_irq_byname(pdev, "korina_und");

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "korina_regs");
	dev->base_addr = r->start;
	lp->eth_regs = ioremap_nocache(r->start, r->end - r->start);
	if (!lp->eth_regs) {
		ERR("Can't remap eth registers\n");
		retval = -ENXIO;
		goto probe_err_out;
	}

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "korina_dma_rx");
	lp->rx_dma_regs = ioremap_nocache(r->start, r->end - r->start);
	if (!lp->rx_dma_regs) {
		ERR("Can't remap Rx DMA registers\n");
		retval = -ENXIO;
		goto probe_err_out;
	}

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "korina_dma_tx");
	lp->tx_dma_regs = ioremap_nocache(r->start, r->end - r->start);
	if (!lp->tx_dma_regs) {
		ERR("Can't remap Tx DMA registers\n");
		retval = -ENXIO;
		goto probe_err_out;
	}
	
#ifdef RC32434_PROC_DEBUG
	lp->ps = create_proc_read_entry (bif->name, 0, proc_net,
					 rc32434_read_proc, dev);
#endif
	
	lp->td_ring =	(DMAD_t)kmalloc(TD_RING_SIZE + RD_RING_SIZE, GFP_KERNEL);
	if (!lp->td_ring) {
		ERR("Can't allocate descriptors\n");
		retval = -ENOMEM;
		goto probe_err_out;
	}
	
	dma_cache_inv((unsigned long)(lp->td_ring), TD_RING_SIZE + RD_RING_SIZE);
	
	/* now convert TD_RING pointer to KSEG1 */
	lp->td_ring = (DMAD_t )KSEG1ADDR(lp->td_ring);
	lp->rd_ring = &lp->td_ring[RC32434_NUM_TDS];
	
	
	spin_lock_init(&lp->lock);
	
	/* just use the rx dma irq */
	dev->irq = lp->rx_irq;
	
	dev->priv = lp;
	
	dev->open = rc32434_open;
	dev->stop = rc32434_close;
	dev->hard_start_xmit = rc32434_send_packet;
	dev->get_stats	= rc32434_get_stats;
	dev->set_multicast_list = &rc32434_multicast_list;
	dev->tx_timeout = rc32434_tx_timeout;
	dev->watchdog_timeo = RC32434_TX_TIMEOUT;

#ifdef CONFIG_IDT_USE_NAPI
	dev->poll = rc32434_poll;
	dev->weight = 64;
	printk("Using NAPI with weight %d\n",dev->weight);
#else
	lp->rx_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	tasklet_init(lp->rx_tasklet, rc32434_rx_tasklet, (unsigned long)dev);
#endif
	lp->tx_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	tasklet_init(lp->tx_tasklet, rc32434_tx_tasklet, (unsigned long)dev);
	
	if ((err = register_netdev(dev))) {
		printk(KERN_ERR "rc32434 ethernet. Cannot register net device %d\n", err);
		free_netdev(dev);
		retval = -EINVAL;
		goto probe_err_out;
	}
	
	INFO("Rx IRQ %d, Tx IRQ %d, ", lp->rx_irq, lp->tx_irq);
	for (i = 0; i < 6; i++) {
		printk("%2.2x", dev->dev_addr[i]);
		if (i<5)
			printk(":");
	}
	printk("\n");
	
	return 0;
	
 probe_err_out:
	rc32434_cleanup_module();
	ERR(" failed.  Returns %d\n", retval);
	return retval;
	
}

static int rc32434_remove(struct platform_device *pdev)
{
	struct korina_device *bif = (struct korina_device *) pdev->dev.platform_data;
	
	if (bif->dev != NULL) {
		struct rc32434_local *lp = (struct rc32434_local *)bif->dev->priv;
		if (lp != NULL) {
			if (lp->eth_regs)
				iounmap((void*)lp->eth_regs);
			if (lp->rx_dma_regs)
				iounmap((void*)lp->rx_dma_regs);
			if (lp->tx_dma_regs)
				iounmap((void*)lp->tx_dma_regs);
			if (lp->td_ring)
				kfree((void*)KSEG0ADDR(lp->td_ring));
			
#ifdef RC32434_PROC_DEBUG
			if (lp->ps) {
				remove_proc_entry(bif->name, proc_net);
			}
#endif
			kfree(lp);
		}
		
		platform_set_drvdata(pdev, NULL);
		unregister_netdev(bif->dev);
		free_netdev(bif->dev);
		kfree(bif->dev);
	}
	return 0;
}


static int rc32434_open(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	
	/* Initialize */
	if (rc32434_init(dev)) {
		ERR("Error: cannot open the Ethernet device\n");
		return -EAGAIN;
	}
	
	/* Install the interrupt handler that handles the Done Finished Ovr and Und Events */	
	if (request_irq(lp->rx_irq, &rc32434_rx_dma_interrupt,
		  SA_SHIRQ | SA_INTERRUPT,
			"Korina ethernet Rx", dev)) {
		ERR(": unable to get Rx DMA IRQ %d\n",
		    lp->rx_irq);
		return -EAGAIN;
	}
	if (request_irq(lp->tx_irq, &rc32434_tx_dma_interrupt,
		  SA_SHIRQ | SA_INTERRUPT,
			"Korina ethernet Tx", dev)) {
		ERR(": unable to get Tx DMA IRQ %d\n",
		    lp->tx_irq);
		free_irq(lp->rx_irq, dev);
		return -EAGAIN;
	}
	
#ifdef	RC32434_REVISION
	/* Install handler for overrun error. */
	if (request_irq(lp->ovr_irq, &rc32434_ovr_interrupt,
			SA_SHIRQ | SA_INTERRUPT,
			"Ethernet Overflow", dev)) {
		ERR(": unable to get OVR IRQ %d\n",
		    lp->ovr_irq);
		free_irq(lp->rx_irq, dev);
		free_irq(lp->tx_irq, dev);
		return -EAGAIN;
	}
#endif
	
	/* Install handler for underflow error. */
	if (request_irq(lp->und_irq, &rc32434_und_interrupt,
			SA_SHIRQ | SA_INTERRUPT,
			"Ethernet Underflow", dev)) {
		ERR(": unable to get UND IRQ %d\n",
		    lp->und_irq);
		free_irq(lp->rx_irq, dev);
		free_irq(lp->tx_irq, dev);
#ifdef	RC32434_REVISION		
		free_irq(lp->ovr_irq, dev);		
#endif
		return -EAGAIN;
	}
	
	
	return 0;
}




static int rc32434_close(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	u32 tmp;
	
	/* Disable interrupts */
	disable_irq(lp->rx_irq);
	disable_irq(lp->tx_irq);
#ifdef	RC32434_REVISION
	disable_irq(lp->ovr_irq);
#endif
	disable_irq(lp->und_irq);
	
	tmp = __raw_readl(&lp->tx_dma_regs->dmasm);
	tmp = tmp | DMASM_f_m | DMASM_e_m;
	__raw_writel(tmp, &lp->tx_dma_regs->dmasm);
	
	tmp = __raw_readl(&lp->rx_dma_regs->dmasm);
	tmp = tmp | DMASM_d_m | DMASM_h_m | DMASM_e_m;
	__raw_writel(tmp, &lp->rx_dma_regs->dmasm);
	
	free_irq(lp->rx_irq, dev);
	free_irq(lp->tx_irq, dev);
#ifdef	RC32434_REVISION	
	free_irq(lp->ovr_irq, dev);
#endif
	free_irq(lp->und_irq, dev);
	return 0;
}


/* transmit packet */
static int rc32434_send_packet(struct sk_buff *skb, struct net_device *dev)
{
	struct rc32434_local		*lp = (struct rc32434_local *)dev->priv;
	unsigned long 			flags;
	u32					length;
	DMAD_t				td;
	
	
	spin_lock_irqsave(&lp->lock, flags);
	
	td = &lp->td_ring[lp->tx_chain_tail];
	
	/* stop queue when full, drop pkts if queue already full */
	if(lp->tx_count >= (RC32434_NUM_TDS - 2)) {
		lp->tx_full = 1;
		
		if(lp->tx_count == (RC32434_NUM_TDS - 2)) {
			netif_stop_queue(dev);
		}
		else {
			lp->stats.tx_dropped++;
			dev_kfree_skb_any(skb);
			spin_unlock_irqrestore(&lp->lock, flags);
			return 1;
		}	   
	}	 
	
	lp->tx_count ++;
	
	lp->tx_skb[lp->tx_chain_tail] = skb;
	
	length = skb->len;
	dma_cache_wback((u32)skb->data, skb->len);
	
	/* Setup the transmit descriptor. */
	dma_cache_inv((u32) td, sizeof(*td));
	td->ca = CPHYSADDR(skb->data);
	
	if(__raw_readl(&(lp->tx_dma_regs->dmandptr)) == 0) {
		if( lp->tx_chain_status == empty ) {
			td->control = DMA_COUNT(length) |DMAD_cof_m |DMAD_iof_m;                                /*  Update tail      */
			lp->tx_chain_tail = (lp->tx_chain_tail + 1) & RC32434_TDS_MASK;                          /*   Move tail       */
			__raw_writel(CPHYSADDR(&lp->td_ring[lp->tx_chain_head]), &(lp->tx_dma_regs->dmandptr)); /* Write to NDPTR    */
			lp->tx_chain_head = lp->tx_chain_tail;                                                  /* Move head to tail */
		}
		else {
			td->control = DMA_COUNT(length) |DMAD_cof_m|DMAD_iof_m;                                 /* Update tail */
			lp->td_ring[(lp->tx_chain_tail-1)& RC32434_TDS_MASK].control &=  ~(DMAD_cof_m);          /* Link to prev */
			lp->td_ring[(lp->tx_chain_tail-1)& RC32434_TDS_MASK].link =  CPHYSADDR(td);              /* Link to prev */
			lp->tx_chain_tail = (lp->tx_chain_tail + 1) & RC32434_TDS_MASK;                          /* Move tail */
			__raw_writel(CPHYSADDR(&lp->td_ring[lp->tx_chain_head]), &(lp->tx_dma_regs->dmandptr)); /* Write to NDPTR */
			lp->tx_chain_head = lp->tx_chain_tail;                                                  /* Move head to tail */
			lp->tx_chain_status = empty;
		}
	}
	else {
		if( lp->tx_chain_status == empty ) {
			td->control = DMA_COUNT(length) |DMAD_cof_m |DMAD_iof_m;                                /* Update tail */
			lp->tx_chain_tail = (lp->tx_chain_tail + 1) & RC32434_TDS_MASK;                          /* Move tail */
			lp->tx_chain_status = filled;
		}
		else {
			td->control = DMA_COUNT(length) |DMAD_cof_m |DMAD_iof_m;                                /* Update tail */
			lp->td_ring[(lp->tx_chain_tail-1)& RC32434_TDS_MASK].control &=  ~(DMAD_cof_m);          /* Link to prev */
			lp->td_ring[(lp->tx_chain_tail-1)& RC32434_TDS_MASK].link =  CPHYSADDR(td);              /* Link to prev */
			lp->tx_chain_tail = (lp->tx_chain_tail + 1) & RC32434_TDS_MASK;                          /* Move tail */
		}
	}
	dma_cache_wback((u32) td, sizeof(*td));
	
	dev->trans_start = jiffies;				
	
	spin_unlock_irqrestore(&lp->lock, flags);
	
	return 0;
}


/* Ethernet MII-PHY Handler */
static void rc32434_mii_handler(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;		
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	unsigned long 	flags;
	unsigned long duplex_status;
	int port_addr = (lp->rx_irq == 0x2c? 1:0) << 8;
	
	spin_lock_irqsave(&lp->lock, flags);
	
	/* Two ports are using the same MII, the difference is the PHY address */
	__raw_writel(0, &rc32434_eth0_regs->miimcfg);  
	__raw_writel(0, &rc32434_eth0_regs->miimcmd);  
	__raw_writel(port_addr |0x05, &rc32434_eth0_regs->miimaddr);  
	__raw_writel(MIIMCMD_scn_m, &rc32434_eth0_regs->miimcmd);  
	while(__raw_readl(&rc32434_eth0_regs->miimind) & MIIMIND_nv_m);
	
	ERR("irq:%x		port_addr:%x	RDD:%x\n", 
	    lp->rx_irq, port_addr, __raw_readl(&rc32434_eth0_regs->miimrdd));
	duplex_status = (__raw_readl(&rc32434_eth0_regs->miimrdd) & 0x140)? ETHMAC2_fd_m: 0;
	if(duplex_status != lp->duplex_mode) {
		ERR("The MII-PHY is Auto-negotiated to %s-Duplex mode for Eth-%x\n", duplex_status? "Full":"Half", lp->rx_irq == 0x2c? 1:0);		
		lp->duplex_mode = duplex_status;
		rc32434_restart(dev);		
	}
	
	lp->mii_phy_timer.expires = jiffies + 10 * HZ;	
	add_timer(&lp->mii_phy_timer);
	
	spin_unlock_irqrestore(&lp->lock, flags);
	
}

#ifdef	RC32434_REVISION	
/* Ethernet Rx Overflow interrupt */
static irqreturn_t
rc32434_ovr_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct rc32434_local *lp;
	unsigned int ovr;
	irqreturn_t retval = IRQ_NONE;
	
	ASSERT(dev != NULL);
	
	lp = (struct rc32434_local *)dev->priv;
	spin_lock(&lp->lock);
	ovr = __raw_readl(&lp->eth_regs->ethintfc);
	
	if(ovr & ETHINTFC_ovr_m) {
		netif_stop_queue(dev);
		
		/* clear OVR bit */
		__raw_writel((ovr & ~ETHINTFC_ovr_m), &lp->eth_regs->ethintfc);
		
		/* Restart interface */
		rc32434_restart(dev);
		retval = IRQ_HANDLED;
	}
	spin_unlock(&lp->lock);
	
	return retval;
}

#endif


/* Ethernet Tx Underflow interrupt */
static irqreturn_t
rc32434_und_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct rc32434_local *lp;
	unsigned int und;
	irqreturn_t retval = IRQ_NONE;
	
	ASSERT(dev != NULL);
	
	lp = (struct rc32434_local *)dev->priv;
	
	spin_lock(&lp->lock);
	
	und = __raw_readl(&lp->eth_regs->ethintfc);
	
	if(und & ETHINTFC_und_m) {
		netif_stop_queue(dev);
		
		__raw_writel((und & ~ETHINTFC_und_m), &lp->eth_regs->ethintfc);
		
		/* Restart interface */
		rc32434_restart(dev);
		retval = IRQ_HANDLED;
	}
	
	spin_unlock(&lp->lock);
	
	return retval;
}


/* Ethernet Rx DMA interrupt */
static irqreturn_t
rc32434_rx_dma_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct rc32434_local* lp;
	volatile u32 dmas,dmasm;
	irqreturn_t retval;
	
	ASSERT(dev != NULL);
	
	lp = (struct rc32434_local *)dev->priv;
	
	spin_lock(&lp->lock);
	dmas = __raw_readl(&lp->rx_dma_regs->dmas);
	if(dmas & (DMAS_d_m|DMAS_h_m|DMAS_e_m)) {
		/* Mask D H E bit in Rx DMA */
		dmasm = __raw_readl(&lp->rx_dma_regs->dmasm);
		__raw_writel(dmasm | (DMASM_d_m | DMASM_h_m | DMASM_e_m), &lp->rx_dma_regs->dmasm);
#ifdef CONFIG_IDT_USE_NAPI
		if(netif_rx_schedule_prep(dev))
                        __netif_rx_schedule(dev);
#else
		tasklet_hi_schedule(lp->rx_tasklet);
#endif
		
		if (dmas & DMAS_e_m)
			ERR(": DMA error\n");
		
		retval = IRQ_HANDLED;
	}
	else
		retval = IRQ_NONE;
	
	spin_unlock(&lp->lock);
	return retval;
}

#ifdef CONFIG_IDT_USE_NAPI
static int rc32434_poll(struct net_device *rx_data_dev, int *budget)
#else
static void rc32434_rx_tasklet(unsigned long rx_data_dev)
#endif
{
	struct net_device *dev = (struct net_device *)rx_data_dev;	
	struct rc32434_local* lp = netdev_priv(dev);
	volatile DMAD_t  rd = &lp->rd_ring[lp->rx_next_done];
	struct sk_buff *skb, *skb_new;
	u8* pkt_buf;
	u32 devcs, count, pkt_len, pktuncrc_len;
	volatile u32 dmas;
#ifdef CONFIG_IDT_USE_NAPI
	u32 received = 0;
	int rx_work_limit = min(*budget,dev->quota);
#else
	unsigned long 	flags;
	spin_lock_irqsave(&lp->lock, flags);
#endif

	dma_cache_inv((u32)rd, sizeof(*rd));
	while ( (count = RC32434_RBSIZE - (u32)DMA_COUNT(rd->control)) != 0) {
#ifdef CONFIG_IDT_USE_NAPI
		if(--rx_work_limit <0)
                {
                        break;
                }
#endif
		/* init the var. used for the later operations within the while loop */
		skb_new = NULL;
		devcs = rd->devcs;
		pkt_len = RCVPKT_LENGTH(devcs);
		skb = lp->rx_skb[lp->rx_next_done];
      
		if (count < 64) {
			lp->stats.rx_errors++;
			lp->stats.rx_dropped++;			
		}
		else if ((devcs & ( ETHRX_ld_m)) !=	ETHRX_ld_m) {
			/* check that this is a whole packet */
			/* WARNING: DMA_FD bit incorrectly set in Rc32434 (errata ref #077) */
			lp->stats.rx_errors++;
			lp->stats.rx_dropped++;
		}
		else if ( (devcs & ETHRX_rok_m)  ) {
			
			{
				/* must be the (first and) last descriptor then */
				pkt_buf = (u8*)lp->rx_skb[lp->rx_next_done]->data;
				
				pktuncrc_len = pkt_len - 4;
				/* invalidate the cache */
				dma_cache_inv((unsigned long)pkt_buf, pktuncrc_len);
				
				/* Malloc up new buffer. */					  
				skb_new = dev_alloc_skb(RC32434_RBSIZE + 2);					             	
				
				if (skb_new != NULL){
					/* Make room */
					skb_put(skb, pktuncrc_len);		    
					
					skb->protocol = eth_type_trans(skb, dev);
					
					/* pass the packet to upper layers */
#ifdef CONFIG_IDT_USE_NAPI
					netif_receive_skb(skb);
#else
					netif_rx(skb);
#endif
					
					dev->last_rx = jiffies;
					lp->stats.rx_packets++;
					lp->stats.rx_bytes += pktuncrc_len;
					
					if (IS_RCV_MP(devcs))
						lp->stats.multicast++;
					
					/* 16 bit align */						  
					skb_reserve(skb_new, 2);	
					
					skb_new->dev = dev;
					lp->rx_skb[lp->rx_next_done] = skb_new;
				}
				else {
					ERR("no memory, dropping rx packet.\n");
					lp->stats.rx_errors++;		
					lp->stats.rx_dropped++;					
				}
			}
			
		}			
		else {
			/* This should only happen if we enable accepting broken packets */
			lp->stats.rx_errors++;
			lp->stats.rx_dropped++;
			
			/* add statistics counters */
			if (IS_RCV_CRC_ERR(devcs)) {
				DBG(2, "RX CRC error\n");
				lp->stats.rx_crc_errors++;
			} 
			else if (IS_RCV_LOR_ERR(devcs)) {
				DBG(2, "RX LOR error\n");
				lp->stats.rx_length_errors++;
			}				
			else if (IS_RCV_LE_ERR(devcs)) {
				DBG(2, "RX LE error\n");
				lp->stats.rx_length_errors++;
			}
			else if (IS_RCV_OVR_ERR(devcs)) {
				lp->stats.rx_over_errors++;
			}
			else if (IS_RCV_CV_ERR(devcs)) {
				/* code violation */
				DBG(2, "RX CV error\n");
				lp->stats.rx_frame_errors++;
			}
			else if (IS_RCV_CES_ERR(devcs)) {
				DBG(2, "RX Preamble error\n");
			}
		}
		
		rd->devcs = 0;
		
		/* restore descriptor's curr_addr */
		if(skb_new)
			rd->ca = CPHYSADDR(skb_new->data); 
		else
			rd->ca = CPHYSADDR(skb->data);
		
		rd->control = DMA_COUNT(RC32434_RBSIZE) |DMAD_cod_m |DMAD_iod_m;
		lp->rd_ring[(lp->rx_next_done-1)& RC32434_RDS_MASK].control &=  ~(DMAD_cod_m); 	
		
		lp->rx_next_done = (lp->rx_next_done + 1) & RC32434_RDS_MASK;
		dma_cache_wback((u32)rd, sizeof(*rd));
		rd = &lp->rd_ring[lp->rx_next_done];
		__raw_writel( ~DMAS_d_m, &lp->rx_dma_regs->dmas);
	}	
#ifdef CONFIG_IDT_USE_NAPI
        dev->quota -= received;
        *budget =- received;
        if(rx_work_limit < 0)
                goto not_done;
#endif
	
	dmas = __raw_readl(&lp->rx_dma_regs->dmas);
	
	if(dmas & DMAS_h_m) {
		__raw_writel( ~(DMAS_h_m | DMAS_e_m), &lp->rx_dma_regs->dmas);
#ifdef RC32434_PROC_DEBUG
		lp->dma_halt_cnt++;
#endif
		rd->devcs = 0;
		skb = lp->rx_skb[lp->rx_next_done];
		rd->ca = CPHYSADDR(skb->data);
		dma_cache_wback((u32)rd, sizeof(*rd));
		rc32434_chain_rx(lp,rd);
	}
	
#ifdef CONFIG_IDT_USE_NAPI
	netif_rx_complete(dev);
#endif
	/* Enable D H E bit in Rx DMA */
	__raw_writel(__raw_readl(&lp->rx_dma_regs->dmasm) & ~(DMASM_d_m | DMASM_h_m |DMASM_e_m), &lp->rx_dma_regs->dmasm); 
#ifdef CONFIG_IDT_USE_NAPI
	return 0;
 not_done:
	return 1;
#else
	spin_unlock_irqrestore(&lp->lock, flags);
	return;
#endif

	
}	



/* Ethernet Tx DMA interrupt */
static irqreturn_t
rc32434_tx_dma_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *)dev_id;
	struct rc32434_local *lp;
	volatile u32 dmas,dmasm;
	irqreturn_t retval;
	
	ASSERT(dev != NULL);
	
	lp = (struct rc32434_local *)dev->priv;
	
	spin_lock(&lp->lock);
	
	dmas = __raw_readl(&lp->tx_dma_regs->dmas);
	
	if (dmas & (DMAS_f_m | DMAS_e_m)) {
		dmasm = __raw_readl(&lp->tx_dma_regs->dmasm);
		/* Mask F E bit in Tx DMA */
		__raw_writel(dmasm | (DMASM_f_m | DMASM_e_m), &lp->tx_dma_regs->dmasm);
		
		tasklet_hi_schedule(lp->tx_tasklet);
		
		if(lp->tx_chain_status == filled && (__raw_readl(&(lp->tx_dma_regs->dmandptr)) == 0)) {
			__raw_writel(CPHYSADDR(&lp->td_ring[lp->tx_chain_head]), &(lp->tx_dma_regs->dmandptr));			
			lp->tx_chain_status = empty;
			lp->tx_chain_head = lp->tx_chain_tail;
			dev->trans_start = jiffies;
		}
		
		if (dmas & DMAS_e_m)
			ERR(": DMA error\n");
		
		retval = IRQ_HANDLED;
	}
	else
		retval = IRQ_NONE;
	
	spin_unlock(&lp->lock);
	
	return retval;
}


static void rc32434_tx_tasklet(unsigned long tx_data_dev)
{
	struct net_device *dev = (struct net_device *)tx_data_dev;	
	struct rc32434_local* lp = (struct rc32434_local *)dev->priv;
	volatile DMAD_t td = &lp->td_ring[lp->tx_next_done];
	u32 devcs;
	unsigned long 	flags;
	volatile u32 dmas;
	
	spin_lock_irqsave(&lp->lock, flags);
	
	/* process all desc that are done */
	while(IS_DMA_FINISHED(td->control)) {
		if(lp->tx_full == 1) {
			netif_wake_queue(dev);
			lp->tx_full = 0;
		}
		
		devcs = lp->td_ring[lp->tx_next_done].devcs;    
		if ((devcs & (ETHTX_fd_m | ETHTX_ld_m)) != (ETHTX_fd_m | ETHTX_ld_m)) {
			lp->stats.tx_errors++;
			lp->stats.tx_dropped++;				
			
			/* should never happen */
			DBG(1, __FUNCTION__ ": split tx ignored\n");
		}
		else if (IS_TX_TOK(devcs)) {
			lp->stats.tx_packets++;
			lp->stats.tx_bytes+=lp->tx_skb[lp->tx_next_done]->len;
		}
		else {
			lp->stats.tx_errors++;
			lp->stats.tx_dropped++;				
			
			/* underflow */
			if (IS_TX_UND_ERR(devcs)) 
				lp->stats.tx_fifo_errors++;
			
			/* oversized frame */
			if (IS_TX_OF_ERR(devcs))
				lp->stats.tx_aborted_errors++;
			
			/* excessive deferrals */
			if (IS_TX_ED_ERR(devcs))
				lp->stats.tx_carrier_errors++;
			
			/* collisions: medium busy */
			if (IS_TX_EC_ERR(devcs))
				lp->stats.collisions++;
			
			/* late collision */
			if (IS_TX_LC_ERR(devcs))
				lp->stats.tx_window_errors++;
			
		}
		
		/* We must always free the original skb */
		if (lp->tx_skb[lp->tx_next_done] != NULL) {
			dev_kfree_skb_any(lp->tx_skb[lp->tx_next_done]);
			lp->tx_skb[lp->tx_next_done] = NULL;
		}
		
		lp->td_ring[lp->tx_next_done].control = DMAD_iof_m;
		lp->td_ring[lp->tx_next_done].devcs = ETHTX_fd_m | ETHTX_ld_m;	
		lp->td_ring[lp->tx_next_done].link = 0;
		lp->td_ring[lp->tx_next_done].ca = 0;
		lp->tx_count --;
		
		/* go on to next transmission */
		lp->tx_next_done = (lp->tx_next_done + 1) & RC32434_TDS_MASK;
		td = &lp->td_ring[lp->tx_next_done];
		
	}
	
	dmas = __raw_readl(&lp->tx_dma_regs->dmas);
	__raw_writel( ~dmas, &lp->tx_dma_regs->dmas);
	
	/* Enable F E bit in Tx DMA */
	__raw_writel(__raw_readl(&lp->tx_dma_regs->dmasm) & ~(DMASM_f_m | DMASM_e_m), &lp->tx_dma_regs->dmasm); 
	spin_unlock_irqrestore(&lp->lock, flags);
	
}


static struct net_device_stats * rc32434_get_stats(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	return &lp->stats;
}


/*
 * Set or clear the multicast filter for this adaptor.
 */
static void rc32434_multicast_list(struct net_device *dev)
{   
	/* listen to broadcasts always and to treat 	*/
	/*       IFF bits independantly	*/
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	unsigned long flags;
	u32 recognise = ETHARC_ab_m; 		/* always accept broadcasts */
	
	if (dev->flags & IFF_PROMISC)         		/* set promiscuous mode */
		recognise |= ETHARC_pro_m;
	
	if ((dev->flags & IFF_ALLMULTI) || (dev->mc_count > 15))
		recognise |= ETHARC_am_m;    	  	/* all multicast & bcast */
	else if (dev->mc_count > 0) {
		DBG(2, __FUNCTION__ ": mc_count %d\n", dev->mc_count);
		recognise |= ETHARC_am_m;    	  	/* for the time being */
	}
	
	spin_lock_irqsave(&lp->lock, flags);
	__raw_writel(recognise, &lp->eth_regs->etharc);
	spin_unlock_irqrestore(&lp->lock, flags);
}


static void rc32434_tx_timeout(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	unsigned long flags;
	
	spin_lock_irqsave(&lp->lock, flags);
	rc32434_restart(dev);
	spin_unlock_irqrestore(&lp->lock, flags);
	
}


/*
 * Initialize the RC32434 ethernet controller.
 */
static int rc32434_init(struct net_device *dev)
{
	struct rc32434_local *lp = (struct rc32434_local *)dev->priv;
	int i, j;
	
	/* Disable DMA */       
	rc32434_abort_tx(dev);
	rc32434_abort_rx(dev); 
	
	/* reset ethernet logic */ 
	__raw_writel(0, &lp->eth_regs->ethintfc);
	while((__raw_readl(&lp->eth_regs->ethintfc) & ETHINTFC_rip_m))
		dev->trans_start = jiffies;	
	
	/* Enable Ethernet Interface */ 
	__raw_writel(ETHINTFC_en_m, &lp->eth_regs->ethintfc); 
	
#ifndef CONFIG_IDT_USE_NAPI
	tasklet_disable(lp->rx_tasklet);
#endif
	tasklet_disable(lp->tx_tasklet);
	
	/* Initialize the transmit Descriptors */
	for (i = 0; i < RC32434_NUM_TDS; i++) {
		lp->td_ring[i].control = DMAD_iof_m;
		lp->td_ring[i].devcs = ETHTX_fd_m | ETHTX_ld_m;
		lp->td_ring[i].ca = 0;
		lp->td_ring[i].link = 0;
		if (lp->tx_skb[i] != NULL) {
			dev_kfree_skb_any(lp->tx_skb[i]);
			lp->tx_skb[i] = NULL;
		}
	}
	lp->tx_next_done = lp->tx_chain_head = lp->tx_chain_tail = 	lp->tx_full = lp->tx_count = 0;
	lp->	tx_chain_status = empty;
	
	/*
	 * Initialize the receive descriptors so that they
	 * become a circular linked list, ie. let the last
	 * descriptor point to the first again.
	 */
	for (i=0; i<RC32434_NUM_RDS; i++) {
		struct sk_buff *skb = lp->rx_skb[i];
		
		if (lp->rx_skb[i] == NULL) {
			skb = dev_alloc_skb(RC32434_RBSIZE + 2);
			if (skb == NULL) {
				ERR("No memory in the system\n");
				for (j = 0; j < RC32434_NUM_RDS; j ++)
					if (lp->rx_skb[j] != NULL) 
						dev_kfree_skb_any(lp->rx_skb[j]);
				
				return 1;
			}
			else {
				skb->dev = dev;
				skb_reserve(skb, 2);
				lp->rx_skb[i] = skb;
				lp->rd_ring[i].ca = CPHYSADDR(skb->data); 
				
			}
		}
		lp->rd_ring[i].control =	DMAD_iod_m | DMA_COUNT(RC32434_RBSIZE);
		lp->rd_ring[i].devcs = 0;
		lp->rd_ring[i].ca = CPHYSADDR(skb->data);
		lp->rd_ring[i].link = CPHYSADDR(&lp->rd_ring[i+1]);
		
	}
	/* loop back */
	lp->rd_ring[RC32434_NUM_RDS-1].link = CPHYSADDR(&lp->rd_ring[0]);
	lp->rx_next_done   = 0;
	
	lp->rd_ring[RC32434_NUM_RDS-1].control |= DMAD_cod_m;
	lp->rx_chain_head = 0;
	lp->rx_chain_tail = 0;
	lp->rx_chain_status = empty;
	
	__raw_writel(0, &lp->rx_dma_regs->dmas);
	/* Start Rx DMA */
	rc32434_start_rx(lp, &lp->rd_ring[0]);
	
	/* Enable F E bit in Tx DMA */
	__raw_writel(__raw_readl(&lp->tx_dma_regs->dmasm) & ~(DMASM_f_m | DMASM_e_m), &lp->tx_dma_regs->dmasm); 
	/* Enable D H E bit in Rx DMA */
	__raw_writel(__raw_readl(&lp->rx_dma_regs->dmasm) & ~(DMASM_d_m | DMASM_h_m | DMASM_e_m), &lp->rx_dma_regs->dmasm); 
	
	/* Accept only packets destined for this Ethernet device address */
	__raw_writel(ETHARC_ab_m, &lp->eth_regs->etharc); 
	
	/* Set all Ether station address registers to their initial values */ 
	__raw_writel(STATION_ADDRESS_LOW(dev), &lp->eth_regs->ethsal0); 
	__raw_writel(STATION_ADDRESS_HIGH(dev), &lp->eth_regs->ethsah0);
	
	__raw_writel(STATION_ADDRESS_LOW(dev), &lp->eth_regs->ethsal1); 
	__raw_writel(STATION_ADDRESS_HIGH(dev), &lp->eth_regs->ethsah1);
	
	__raw_writel(STATION_ADDRESS_LOW(dev), &lp->eth_regs->ethsal2); 
	__raw_writel(STATION_ADDRESS_HIGH(dev), &lp->eth_regs->ethsah2);
	
	__raw_writel(STATION_ADDRESS_LOW(dev), &lp->eth_regs->ethsal3); 
	__raw_writel(STATION_ADDRESS_HIGH(dev), &lp->eth_regs->ethsah3); 
	
	
	/* Frame Length Checking, Pad Enable, CRC Enable, Full Duplex set */ 
	__raw_writel(ETHMAC2_pe_m | ETHMAC2_cen_m | ETHMAC2_fd_m, &lp->eth_regs->ethmac2);  
	//ETHMAC2_flc_m		ETHMAC2_fd_m	lp->duplex_mode
	
	/* Back to back inter-packet-gap */ 
	__raw_writel(0x15, &lp->eth_regs->ethipgt); 
	/* Non - Back to back inter-packet-gap */ 
	__raw_writel(0x12, &lp->eth_regs->ethipgr); 
	
	/* Management Clock Prescaler Divisor */
	/* Clock independent setting */
	__raw_writel(((idt_cpu_freq)/MII_CLOCK+1) & ~1,
		       &lp->eth_regs->ethmcp);
	
	/* don't transmit until fifo contains 48b */
	__raw_writel(48, &lp->eth_regs->ethfifott);
	
	__raw_writel(ETHMAC1_re_m, &lp->eth_regs->ethmac1);
	
#ifndef CONFIG_IDT_USE_NAPI
	tasklet_enable(lp->rx_tasklet);
#endif
	tasklet_enable(lp->tx_tasklet);
	
	netif_start_queue(dev);
	
	return 0; 
}

static struct platform_driver korina_driver = {
	.driver.name = "korina",
	.probe = rc32434_probe,
	.remove = rc32434_remove,
};

static int __init rc32434_init_module(void)
{
	return platform_driver_register(&korina_driver);
}

static void rc32434_cleanup_module(void)
{
	return platform_driver_unregister(&korina_driver);
}

module_init(rc32434_init_module);
module_exit(rc32434_cleanup_module);
