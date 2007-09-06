/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     Definitions for IDT RC32434 on-chip ethernet controller.
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
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
 * Initial Release
 *
 * Aug 2004
 *
 * Added NAPI
 *
 **************************************************************************
 */


#include  <asm/rc32434/rc32434.h>
#include  <asm/rc32434/dma_v.h>
#include  <asm/rc32434/eth_v.h>

#define CONFIG_IDT_USE_NAPI 1
#define RC32434_DEBUG	2
//#define RC32434_PROC_DEBUG
#undef	RC32434_DEBUG

#ifdef RC32434_DEBUG

/* use 0 for production, 1 for verification, >2 for debug */
static int rc32434_debug = RC32434_DEBUG;
#define ASSERT(expr) \
	if(!(expr)) {	\
		printk( "Assertion failed! %s,%s,%s,line=%d\n",	\
		#expr,__FILE__,__FUNCTION__,__LINE__);		}
#define DBG(lvl, format, arg...) if (rc32434_debug > lvl) printk(KERN_INFO "%s: " format, dev->name , ## arg)
#else
#define ASSERT(expr) do {} while (0)
#define DBG(lvl, format, arg...) do {} while (0)
#endif

#define INFO(format, arg...) printk(KERN_INFO "%s: " format, dev->name , ## arg)
#define ERR(format, arg...) printk(KERN_ERR "%s: " format, dev->name , ## arg)
#define WARN(format, arg...) printk(KERN_WARNING "%s: " format, dev->name , ## arg)		

/* the following must be powers of two */
#ifdef CONFIG_IDT_USE_NAPI
#define RC32434_NUM_RDS    64    		/* number of receive descriptors */
#define RC32434_NUM_TDS    64    		/* number of transmit descriptors */
#else
#define RC32434_NUM_RDS    128    		/* number of receive descriptors */
#define RC32434_NUM_TDS    128    		/* number of transmit descriptors */
#endif

#define RC32434_RBSIZE     1536  		/* size of one resource buffer = Ether MTU */
#define RC32434_RDS_MASK   (RC32434_NUM_RDS-1)
#define RC32434_TDS_MASK   (RC32434_NUM_TDS-1)
#define RD_RING_SIZE (RC32434_NUM_RDS * sizeof(struct DMAD_s))
#define TD_RING_SIZE (RC32434_NUM_TDS * sizeof(struct DMAD_s))

#define RC32434_TX_TIMEOUT HZ * 100

#define rc32434_eth0_regs ((ETH_t)(ETH0_VirtualAddress))
#define rc32434_eth1_regs ((ETH_t)(ETH1_VirtualAddress))

enum status	{ filled,	empty};
#define IS_DMA_FINISHED(X)   (((X) & (DMAD_f_m)) != 0)
#define IS_DMA_DONE(X)   (((X) & (DMAD_d_m)) != 0)


/* Information that need to be kept for each board. */
struct rc32434_local {
	ETH_t  eth_regs;
	DMA_Chan_t  rx_dma_regs;
	DMA_Chan_t  tx_dma_regs;
	volatile DMAD_t   td_ring;			/* transmit descriptor ring */ 
	volatile DMAD_t   rd_ring;			/* receive descriptor ring  */
	
	struct sk_buff* tx_skb[RC32434_NUM_TDS]; 	/* skbuffs for pkt to trans */
	struct sk_buff* rx_skb[RC32434_NUM_RDS]; 	/* skbuffs for pkt to trans */
	
#ifndef CONFIG_IDT_USE_NAPI
	struct tasklet_struct * rx_tasklet;
#endif
	struct tasklet_struct * tx_tasklet;
	
	int	rx_next_done;
	int	rx_chain_head;
	int	rx_chain_tail;
	enum status	rx_chain_status;
	
	int	tx_next_done;
	int	tx_chain_head;
	int	tx_chain_tail;
	enum status	tx_chain_status;
	int tx_count;			
	int	tx_full;
	
	struct timer_list    mii_phy_timer;
	unsigned long duplex_mode;
	
	int   	rx_irq;
	int    tx_irq;
	int    ovr_irq;
	int    und_irq;
	
	struct net_device_stats stats;
	spinlock_t lock; 
	
	/* debug /proc entry */
	struct proc_dir_entry *ps;
	int dma_halt_cnt;  int dma_run_cnt;
};

extern unsigned int idt_cpu_freq;

/* Index to functions, as function prototypes. */
static int rc32434_open(struct net_device *dev);
static int rc32434_send_packet(struct sk_buff *skb, struct net_device *dev);
static void rc32434_mii_handler(unsigned long data);
static irqreturn_t  rc32434_und_interrupt(int irq, void *dev_id);
static irqreturn_t rc32434_rx_dma_interrupt(int irq, void *dev_id);
static irqreturn_t rc32434_tx_dma_interrupt(int irq, void *dev_id);
#ifdef	RC32434_REVISION	
static irqreturn_t rc32434_ovr_interrupt(int irq, void *dev_id);
#endif
static int  rc32434_close(struct net_device *dev);
static struct net_device_stats *rc32434_get_stats(struct net_device *dev);
static void rc32434_multicast_list(struct net_device *dev);
static int  rc32434_init(struct net_device *dev);
static void rc32434_tx_timeout(struct net_device *dev);

static void rc32434_tx_tasklet(unsigned long tx_data_dev);
#ifdef CONFIG_IDT_USE_NAPI
static int rc32434_poll(struct net_device *rx_data_dev, int *budget);
#else
static void rc32434_rx_tasklet(unsigned long rx_data_dev);
#endif
static void rc32434_cleanup_module(void);


static inline void rc32434_abort_dma(struct net_device *dev, DMA_Chan_t ch)
{
	if (__raw_readl(&ch->dmac) & DMAC_run_m) {
		__raw_writel(0x10, &ch->dmac); 
		
		while (!(__raw_readl(&ch->dmas) & DMAS_h_m))
			dev->trans_start = jiffies;		
		
		__raw_writel(0, &ch->dmas);  
	}
	
	__raw_writel(0, &ch->dmadptr); 
	__raw_writel(0, &ch->dmandptr); 
}
