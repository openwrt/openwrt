/**************************************************
 *
 * drivers/ifx/serial/ifx_ssc.c
 *
 * Driver for IFX_SSC serial ports
 *
 * Copyright (C) 2004 Infineon Technologies AG
 * Author Michael Schoenenborn (IFX COM TI BT)
 *
 */
#define IFX_SSC_DRV_VERSION "0.2.1"
/*
 **************************************************
 *
 * This driver was originally based on the INCA-IP driver, but due to 
 * fundamental conceptual drawbacks there has been changed a lot.
 *
 * Based on INCA-IP driver Copyright (c) 2003 Gary Jennejohn <gj@denx.de>
 * Based on the VxWorks drivers Copyright (c) 2002, Infineon Technologies.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// ### TO DO: general issues:
//	      - power management
//	      - interrupt handling (direct/indirect)
//	      - pin/mux-handling (just overall concept due to project dependency)
//	      - multiple instances capability
//	      - slave functionality

/*
 * Include section 
 */
#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
//#include <linux/poll.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/ifx_ssc_defines.h>
#include <asm/amazon/ifx_ssc.h>

#ifdef SSC_FRAME_INT_ENABLE
#undef SSC_FRAME_INT_ENABLE
#endif

#define not_yet

#define SPI_VINETIC

/*
 * Deal with CONFIG_MODVERSIONS
 */
#if CONFIG_MODVERSIONS==1
# include <linux/modversions.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Schoenenborn");
MODULE_DESCRIPTION("IFX SSC driver");
MODULE_SUPPORTED_DEVICE("ifx_ssc");
MODULE_PARM(maj, "i");
MODULE_PARM_DESC(maj, "Major device number");

/* allow the user to set the major device number */
static int maj = 0;


/*
 * This is the per-channel data structure containing pointers, flags
 * and variables for the port. This driver supports a maximum of PORT_CNT.
 * isp is allocated in ifx_ssc_init() based on the chip version.
 */
static struct ifx_ssc_port *isp;

/* prototypes for fops */
static ssize_t ifx_ssc_read(struct file *, char *, size_t, loff_t *);
static ssize_t ifx_ssc_write(struct file *, const char *, size_t, loff_t *);
//static unsigned int ifx_ssc_poll(struct file *, struct poll_table_struct *);
int ifx_ssc_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
int ifx_ssc_open(struct inode *, struct file *);
int ifx_ssc_close(struct inode *, struct file *);

/* other forward declarations */
static unsigned int ifx_ssc_get_kernel_clk(struct ifx_ssc_port *info);
static void ifx_ssc_rx_int(int, void *, struct pt_regs *);
static void ifx_ssc_tx_int(int, void *, struct pt_regs *);
static void ifx_ssc_err_int(int, void *, struct pt_regs *);
#ifdef SSC_FRAME_INT_ENABLE
static void ifx_ssc_frm_int(int, void *, struct pt_regs *);
#endif
static void tx_int(struct ifx_ssc_port *);
static int ifx_ssc1_read_proc(char *, char **, off_t, int, int *, void *);
static void ifx_gpio_init(void);
/************************************************************************
 *  Function declaration
 ************************************************************************/
//interrupt.c
extern unsigned int amazon_get_fpi_hz(void);
extern void disable_amazon_irq(unsigned int irq_nr);
extern void enable_amazon_irq(unsigned int irq_nr);
extern void mask_and_ack_amazon_irq(unsigned int irq_nr);


/*****************************************************************/
typedef struct {
	int (*request)(unsigned int irq, 
		       void (*handler)(int, void *, struct pt_regs *), 
		       unsigned long irqflags,
		       const char * devname, 
		       void *dev_id);
	void (*free)(unsigned int irq, void *dev_id);
	void (*enable)(unsigned int irq);
	void (*disable)(unsigned int irq);
        void (*clear)(unsigned int irq);
} ifx_int_wrapper_t;

static ifx_int_wrapper_t ifx_int_wrapper = {
	request:	request_irq,	// IM action: enable int
	free:		free_irq,	// IM action: disable int
	enable:		enable_amazon_irq,
	disable:	disable_amazon_irq,
        clear:          mask_and_ack_amazon_irq,
	//end:		
};

/* Fops-struct */
static struct file_operations ifx_ssc_fops = {
        owner:		THIS_MODULE,
	read:		ifx_ssc_read,    /* read */
	write:		ifx_ssc_write,   /* write */
//        poll:		ifx_ssc_poll,    /* poll */
        ioctl:		ifx_ssc_ioctl,   /* ioctl */
        open:		ifx_ssc_open,    /* open */
        release:	ifx_ssc_close,   /* release */
};


static inline unsigned int ifx_ssc_get_kernel_clk(struct ifx_ssc_port *info)
{ // ATTENTION: This function assumes that the CLC register is set with the 
  // appropriate value for RMC.
	unsigned int rmc;

	rmc = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_CLC) & 
	       IFX_CLC_RUN_DIVIDER_MASK) >> IFX_CLC_RUN_DIVIDER_OFFSET;
	if (rmc == 0){
		printk("ifx_ssc_get_kernel_clk rmc==0 \n");
		return (0);
	}
	return (amazon_get_fpi_hz() / rmc);
}

#ifndef not_yet
#ifdef IFX_SSC_INT_USE_BH
/*
 * This routine is used by the interrupt handler to schedule
 * processing in the software interrupt portion of the driver
 * (also known as the "bottom half").  This can be called any
 * number of times for any channel without harm.
 */
static inline void
ifx_ssc_sched_event(struct ifx_ssc_port *info, int event)
{
    info->event |= 1 << event; /* remember what kind of event and who */
    queue_task(&info->tqueue, &tq_cyclades); /* it belongs to */
    mark_bh(CYCLADES_BH);                       /* then trigger event */
} /* ifx_ssc_sched_event */


/*
 * This routine is used to handle the "bottom half" processing for the
 * serial driver, known also the "software interrupt" processing.
 * This processing is done at the kernel interrupt level, after the
 * cy#/_interrupt() has returned, BUT WITH INTERRUPTS TURNED ON.  This
 * is where time-consuming activities which can not be done in the
 * interrupt driver proper are done; the interrupt driver schedules
 * them using ifx_ssc_sched_event(), and they get done here.
 *
 * This is done through one level of indirection--the task queue.
 * When a hardware interrupt service routine wants service by the
 * driver's bottom half, it enqueues the appropriate tq_struct (one
 * per port) to the tq_cyclades work queue and sets a request flag
 * via mark_bh for processing that queue.  When the time is right,
 * do_ifx_ssc_bh is called (because of the mark_bh) and it requests
 * that the work queue be processed.
 *
 * Although this may seem unwieldy, it gives the system a way to
 * pass an argument (in this case the pointer to the ifx_ssc_port
 * structure) to the bottom half of the driver.  Previous kernels
 * had to poll every port to see if that port needed servicing.
 */
static void
do_ifx_ssc_bh(void)
{
    run_task_queue(&tq_cyclades);
} /* do_ifx_ssc_bh */

static void
do_softint(void *private_)
{
  struct ifx_ssc_port *info = (struct ifx_ssc_port *) private_;

    if (test_and_clear_bit(Cy_EVENT_HANGUP, &info->event)) {
        wake_up_interruptible(&info->open_wait);
        info->flags &= ~(ASYNC_NORMAL_ACTIVE|
                             ASYNC_CALLOUT_ACTIVE);
    }
    if (test_and_clear_bit(Cy_EVENT_OPEN_WAKEUP, &info->event)) {
        wake_up_interruptible(&info->open_wait);
    }
    if (test_and_clear_bit(Cy_EVENT_DELTA_WAKEUP, &info->event)) {
	wake_up_interruptible(&info->delta_msr_wait);
    }
    if (test_and_clear_bit(Cy_EVENT_WRITE_WAKEUP, &info->event)) {
        wake_up_interruptible(&tty->write_wait);
    }
#ifdef Z_WAKE
    if (test_and_clear_bit(Cy_EVENT_SHUTDOWN_WAKEUP, &info->event)) {
        wake_up_interruptible(&info->shutdown_wait);
    }
#endif
} /* do_softint */
#endif /* IFX_SSC_INT_USE_BH */
#endif // not_yet

inline static void
rx_int(struct ifx_ssc_port *info)
{
	int		fifo_fill_lev, bytes_in_buf, i;
	unsigned long	tmp_val;
	unsigned long	*tmp_ptr;
	unsigned int	rx_valid_cnt;
	/* number of words waiting in the RX FIFO */
	fifo_fill_lev = (READ_PERIPHERAL_REGISTER(info->mapbase + 
                                                  IFX_SSC_FSTAT) & 
                         IFX_SSC_FSTAT_RECEIVED_WORDS_MASK) >>
                         IFX_SSC_FSTAT_RECEIVED_WORDS_OFFSET;
        // Note: There are always 32 bits in a fifo-entry except for the last 
        // word of a contigous transfer block and except for not in rx-only 
        // mode and CON.ENBV set. But for this case it should be a convention 
        // in software which helps:
        // In tx or rx/tx mode all transfers from the buffer to the FIFO are 
        // 32-bit wide, except for the last three bytes, which could be a 
        // combination of 16- and 8-bit access.
        // => The whole block is received as 32-bit words as a contigous stream, 
        // even if there was a gap in tx which has the fifo run out of data! 
        // Just the last fifo entry *may* be partially filled (0, 1, 2 or 3 bytes)!

	/* free space in the RX buffer */
	bytes_in_buf = info->rxbuf_end - info->rxbuf_ptr;
        // transfer with 32 bits per entry
	while ((bytes_in_buf >= 4) && (fifo_fill_lev > 0)) {
		tmp_ptr = (unsigned long *)info->rxbuf_ptr;
                *tmp_ptr = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RB);
                info->rxbuf_ptr += 4;
                info->stats.rxBytes += 4;
                fifo_fill_lev --;
                bytes_in_buf -= 4;
	} // while ((bytes_in_buf >= 4) && (fifo_fill_lev > 0))
        // now do the rest as mentioned in STATE.RXBV
        while ((bytes_in_buf > 0) && (fifo_fill_lev > 0)) {
                rx_valid_cnt = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) & 
                                IFX_SSC_STATE_RX_BYTE_VALID_MASK) >> 
                        IFX_SSC_STATE_RX_BYTE_VALID_OFFSET;
		if (rx_valid_cnt == 0) break;
                if (rx_valid_cnt > bytes_in_buf) {
                        // ### TO DO: warning message: not block aligned data, other data 
                        //                             in this entry will be lost
                        rx_valid_cnt = bytes_in_buf;
                }
                tmp_val = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RB);
		
                for (i=0; i<rx_valid_cnt; i++) {		
			*info->rxbuf_ptr = (tmp_val >> ( 8 * (rx_valid_cnt - i-1))) & 0xff;
/*			
                        *info->rxbuf_ptr = tmp_val & 0xff;
                        tmp_val >>= 8;
*/			
			bytes_in_buf--;

			
			info->rxbuf_ptr++;
                }
                info->stats.rxBytes += rx_valid_cnt;
        }  // while ((bytes_in_buf > 0) && (fifo_fill_lev > 0))

        // check if transfer is complete
        if (info->rxbuf_ptr >= info->rxbuf_end) {
                ifx_int_wrapper.disable(info->rxirq);
                /* wakeup any processes waiting in read() */
                wake_up_interruptible(&info->rwait);
                /* and in poll() */
                //wake_up_interruptible(&info->pwait);
        } else if ((info->opts.modeRxTx == IFX_SSC_MODE_RX) && 
		   (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RXCNT) == 0)) {
		// if buffer not filled completely and rx request done initiate new transfer
/*
		if  (info->rxbuf_end - info->rxbuf_ptr < 65536)
*/
		if  (info->rxbuf_end - info->rxbuf_ptr < IFX_SSC_RXREQ_BLOCK_SIZE)
                        WRITE_PERIPHERAL_REGISTER((info->rxbuf_end - info->rxbuf_ptr) << 
						  IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
                else 
			WRITE_PERIPHERAL_REGISTER(IFX_SSC_RXREQ_BLOCK_SIZE << IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
	}
} // rx_int

inline static void
tx_int(struct ifx_ssc_port *info)
{
	
	int fifo_space, fill, i;
	fifo_space = ((READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_ID) & 
                       IFX_SSC_PERID_TXFS_MASK) >> IFX_SSC_PERID_TXFS_OFFSET) - 
                ((READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_FSTAT) & 
                  IFX_SSC_FSTAT_TRANSMIT_WORDS_MASK) >> 
                 IFX_SSC_FSTAT_TRANSMIT_WORDS_OFFSET);

	if (fifo_space == 0)
		return;

	fill = info->txbuf_end - info->txbuf_ptr;

	if (fill > fifo_space * 4)
		fill = fifo_space * 4;

	for (i = 0; i < fill / 4; i++) {
                // at first 32 bit access
		WRITE_PERIPHERAL_REGISTER(*(UINT32 *)info->txbuf_ptr, info->mapbase + IFX_SSC_TB);
                info->txbuf_ptr += 4;
	}

        fifo_space -= fill / 4;
	info->stats.txBytes += fill & ~0x3;
        fill &= 0x3;
        if ((fifo_space > 0) & (fill > 1)) {
                // trailing 16 bit access
                WRITE_PERIPHERAL_REGISTER_16(*(UINT16 *)info->txbuf_ptr, info->mapbase + IFX_SSC_TB);
                info->txbuf_ptr += 2;
                info->stats.txBytes += 2;
                fifo_space --;
/* added by bingtao */
		fill -=2;
	}
        if ((fifo_space > 0) & (fill > 0)) {
                // trailing 8 bit access
                WRITE_PERIPHERAL_REGISTER_8(*(UINT8 *)info->txbuf_ptr, info->mapbase + IFX_SSC_TB);
                info->txbuf_ptr ++;
                info->stats.txBytes ++;
/*
                fifo_space --;
*/
	}

        // check if transmission complete
        if (info->txbuf_ptr >= info->txbuf_end) {
                ifx_int_wrapper.disable(info->txirq);
                kfree(info->txbuf);
                info->txbuf = NULL;
                /* wake up any process waiting in poll() */
                //wake_up_interruptible(&info->pwait);
        }	

} // tx_int

static void
ifx_ssc_rx_int(int irq, void *dev_id, struct pt_regs *regs)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *)dev_id;
	//WRITE_PERIPHERAL_REGISTER(IFX_SSC_R_BIT, info->mapbase + IFX_SSC_IRN_CR);
        rx_int(info);
}

static void
ifx_ssc_tx_int(int irq, void *dev_id, struct pt_regs *regs)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *)dev_id;
	//WRITE_PERIPHERAL_REGISTER(IFX_SSC_T_BIT, info->mapbase + IFX_SSC_IRN_CR);
        tx_int(info);
}

static void
ifx_ssc_err_int(int irq, void *dev_id, struct pt_regs *regs)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *)dev_id;
	unsigned int state;
	unsigned int write_back = 0;
	unsigned long flags;


	local_irq_save(flags);
	state = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE); 

	if ((state & IFX_SSC_STATE_RX_UFL) != 0) {
		info->stats.rxUnErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_RX_UFL_ERROR;
	}
	if ((state & IFX_SSC_STATE_RX_OFL) != 0) {
		info->stats.rxOvErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_RX_OFL_ERROR;
	}
	if ((state & IFX_SSC_STATE_TX_OFL) != 0) {
		info->stats.txOvErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_TX_OFL_ERROR;
	}
	if ((state & IFX_SSC_STATE_TX_UFL) != 0) {
		info->stats.txUnErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_TX_UFL_ERROR;
	}
//	if ((state & IFX_SSC_STATE_ABORT_ERR) != 0) {
//		info->stats.abortErr++;
//		write_back |= IFX_SSC_WHBSTATE_CLR_ABORT_ERROR;
//	}
	if ((state & IFX_SSC_STATE_MODE_ERR) != 0) {
		info->stats.modeErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_MODE_ERROR;
	}

	if (write_back)
		WRITE_PERIPHERAL_REGISTER(write_back, 
					  info->mapbase + IFX_SSC_WHBSTATE);

	local_irq_restore(flags);
}

#ifdef SSC_FRAME_INT_ENABLE
static void
ifx_ssc_frm_int(int irq, void *dev_id, struct pt_regs *regs)
{
	// ### TO DO: wake up framing wait-queue in conjunction with batch execution
}
#endif

static void
ifx_ssc_abort(struct ifx_ssc_port *info)
{
	unsigned long flags;
	bool enabled;

        local_irq_save(flags);

	// disable all int's
	ifx_int_wrapper.disable(info->rxirq);
	ifx_int_wrapper.disable(info->txirq);
	ifx_int_wrapper.disable(info->errirq);
/*
	ifx_int_wrapper.disable(info->frmirq);
*/
        local_irq_restore(flags);

	// disable SSC (also aborts a receive request!)
	// ### TO DO: Perhaps it's better to abort after the receiption of a 
	// complete word. The disable cuts the transmission immediatly and 
	// releases the chip selects. This could result in unpredictable 
	// behavior of connected external devices!
	enabled = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) 
		   & IFX_SSC_STATE_IS_ENABLED) != 0;
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ENABLE, 
				  info->mapbase + IFX_SSC_WHBSTATE);


	// flush fifos
        WRITE_PERIPHERAL_REGISTER(IFX_SSC_XFCON_FIFO_FLUSH, 
                                  info->mapbase + IFX_SSC_TXFCON);
        WRITE_PERIPHERAL_REGISTER(IFX_SSC_XFCON_FIFO_FLUSH, 
                                  info->mapbase + IFX_SSC_RXFCON);

	// free txbuf
	if (info->txbuf != NULL) {
		kfree(info->txbuf);
		info->txbuf = NULL;
	}

	// wakeup read process
	if (info->rxbuf != NULL)
		wake_up_interruptible(&info->rwait);

	// clear pending int's 
	ifx_int_wrapper.clear(info->rxirq);
	ifx_int_wrapper.clear(info->txirq);
	ifx_int_wrapper.clear(info->errirq);
/*
	ifx_int_wrapper.clear(info->frmirq);
*/

	// clear error flags
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ALL_ERROR,
				  info->mapbase + IFX_SSC_WHBSTATE);

	//printk("IFX SSC%d: Transmission aborted\n", info->port_nr);
	// enable SSC
	if (enabled)
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_SET_ENABLE, 
					  info->mapbase + IFX_SSC_WHBSTATE);

} // ifx_ssc_abort


/*
 * This routine is called whenever a port is opened.  It enforces
 * exclusive opening of a port and enables interrupts, etc.
 */
int
ifx_ssc_open(struct inode *inode, struct file * filp)
{
	struct ifx_ssc_port  *info;
	int line;
	int from_kernel = 0;

	if ((inode == (struct inode *)0) || (inode == (struct inode *)1)) {
		from_kernel = 1;
		line = (int)inode;
	}
	else {
		line = MINOR(filp->f_dentry->d_inode->i_rdev);
		filp->f_op = &ifx_ssc_fops;
	}

	/* don't open more minor devices than we can support */
	if (line < 0 || line >= PORT_CNT)
		return -ENXIO;

	info = &isp[line];

	/* exclusive open */
	if (info->port_is_open != 0)
		return -EBUSY;
	info->port_is_open++;

	ifx_int_wrapper.disable(info->rxirq);
	ifx_int_wrapper.disable(info->txirq);
	ifx_int_wrapper.disable(info->errirq);
/*
	ifx_int_wrapper.disable(info->frmirq);
*/

	/* Flush and enable TX/RX FIFO */
	WRITE_PERIPHERAL_REGISTER((IFX_SSC_DEF_TXFIFO_FL << 
                                   IFX_SSC_XFCON_ITL_OFFSET) | 
				  IFX_SSC_XFCON_FIFO_FLUSH   |
                                  IFX_SSC_XFCON_FIFO_ENABLE, 
                                  info->mapbase + IFX_SSC_TXFCON);
        WRITE_PERIPHERAL_REGISTER((IFX_SSC_DEF_RXFIFO_FL << 
                                   IFX_SSC_XFCON_ITL_OFFSET) |
				  IFX_SSC_XFCON_FIFO_FLUSH   | 
                                  IFX_SSC_XFCON_FIFO_ENABLE, 
                                  info->mapbase + IFX_SSC_RXFCON);


	/* logically flush the software FIFOs */
	info->rxbuf_ptr = 0;
	info->txbuf_ptr = 0;

	/* clear all error bits */
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ALL_ERROR,
				  info->mapbase + IFX_SSC_WHBSTATE);

	// clear pending interrupts
	ifx_int_wrapper.clear(info->rxirq);
	ifx_int_wrapper.clear(info->txirq);
	ifx_int_wrapper.clear(info->errirq);
/*
	ifx_int_wrapper.clear(info->frmirq);
*/

	// enable SSC
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_SET_ENABLE, 
				  info->mapbase + IFX_SSC_WHBSTATE);

	MOD_INC_USE_COUNT;

	return 0;
} /* ifx_ssc_open */
EXPORT_SYMBOL(ifx_ssc_open);

/*
 * This routine is called when a particular device is closed.
 */
int
ifx_ssc_close(struct inode *inode, struct file *filp)
{
	struct ifx_ssc_port *info;
        int idx;

	if ((inode == (struct inode *)0) || (inode == (struct inode *)1))
		idx = (int)inode;
	else
		idx = MINOR(filp->f_dentry->d_inode->i_rdev);

	if (idx < 0 || idx >= PORT_CNT)
		return -ENXIO;

	info = &isp[idx];
        if (!info)
                return -ENXIO;

	// disable SSC
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ENABLE, 
				  info->mapbase + IFX_SSC_WHBSTATE);

	// call abort function to disable int's, flush fifos...
	ifx_ssc_abort(info);

	info->port_is_open --;
	MOD_DEC_USE_COUNT;

	return 0;
} /* ifx_ssc_close */
EXPORT_SYMBOL(ifx_ssc_close);

/* added by bingtao */
/* helper routine to handle reads from the kernel or user-space */
/* info->rxbuf : never kfree and contains valid data */
/* should be points to NULL after copying data !!! */
static ssize_t
ifx_ssc_read_helper_poll(struct ifx_ssc_port *info, char *buf, size_t len,
                    int from_kernel)
{
        ssize_t         ret_val;
	unsigned long   flags;

        if (info->opts.modeRxTx == IFX_SSC_MODE_TX) 
                return -EFAULT;
        local_irq_save(flags);
        info->rxbuf_ptr = info->rxbuf;
        info->rxbuf_end = info->rxbuf + len;
        local_irq_restore(flags);
/* Vinetic driver always works in IFX_SSC_MODE_RXTX */ 
/* TXRX in poll mode */
	while (info->rxbuf_ptr < info->rxbuf_end){
/* This is the key point, if you don't check this condition 
   kfree (NULL) will happen 
   because tx only need write into FIFO, it's much fast than rx
   So when rx still waiting , tx already finish and release buf	
*/	
		if (info->txbuf_ptr < info->txbuf_end) {
			tx_int(info);
		}
		
		rx_int(info);
        };

        ret_val = info->rxbuf_ptr - info->rxbuf; 
	return (ret_val);
} // ifx_ssc_read_helper_poll

/* helper routine to handle reads from the kernel or user-space */
/* info->rx_buf : never kfree and contains valid data */
/* should be points to NULL after copying data !!! */
static ssize_t
ifx_ssc_read_helper(struct ifx_ssc_port *info, char *buf, size_t len,
                    int from_kernel)
{
        ssize_t         ret_val;
	unsigned long   flags;
        DECLARE_WAITQUEUE(wait, current);

        if (info->opts.modeRxTx == IFX_SSC_MODE_TX) 
                return -EFAULT;
        local_irq_save(flags);
        info->rxbuf_ptr = info->rxbuf;
        info->rxbuf_end = info->rxbuf + len;
        if (info->opts.modeRxTx == IFX_SSC_MODE_RXTX) {
                if ((info->txbuf == NULL) ||
                    (info->txbuf != info->txbuf_ptr) ||
                    (info->txbuf_end != len + info->txbuf)) {
                        local_irq_restore(flags);
                        printk("IFX SSC - %s: write must be called before calling "
                               "read in combined RX/TX!\n", __FUNCTION__);
                        return -EFAULT;
                }
		local_irq_restore(flags);
		/* should enable tx, right?*/
		tx_int(info);
		if (info->txbuf_ptr < info->txbuf_end){
			ifx_int_wrapper.enable(info->txirq);
		}
                
		ifx_int_wrapper.enable(info->rxirq);
        } else { // rx mode
		local_irq_restore(flags);
                if (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RXCNT) &
                    IFX_SSC_RXCNT_TODO_MASK)
                        return -EBUSY;
		ifx_int_wrapper.enable(info->rxirq);
		// rx request limited to ' bytes
/*
                if (len < 65536)
*/
                if (len < IFX_SSC_RXREQ_BLOCK_SIZE)
                        WRITE_PERIPHERAL_REGISTER(len << IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
                else 
			WRITE_PERIPHERAL_REGISTER(IFX_SSC_RXREQ_BLOCK_SIZE << IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
        }

        __add_wait_queue(&info->rwait, &wait);
        set_current_state(TASK_INTERRUPTIBLE);
        // wakeup done in rx_int
 
        do {
		local_irq_save(flags);
                if (info->rxbuf_ptr >= info->rxbuf_end)
                        break;
		local_irq_restore(flags);

//                if (filp->f_flags & O_NONBLOCK)
//                {
//                        N = -EAGAIN;
//                        goto out;
//                }
                if (signal_pending(current)) {
                        ret_val = -ERESTARTSYS;
                        goto out;
                }
                schedule();
        } while (1);

        ret_val = info->rxbuf_ptr - info->rxbuf; // should be equal to len
	local_irq_restore(flags);

  out:
        current->state = TASK_RUNNING;
        __remove_wait_queue(&info->rwait, &wait);
	return (ret_val);
} // ifx_ssc_read_helper


#if 0
/* helper routine to handle reads from the kernel or user-space */
/* appropriate in interrupt context */
static ssize_t
ifx_ssc_read_helper(struct ifx_ssc_port *info, char *buf, size_t len,
                    int from_kernel)
{
        ssize_t         ret_val;
	unsigned long   flags;
       	DECLARE_WAITQUEUE(wait, current);
	
        if (info->opts.modeRxTx == IFX_SSC_MODE_TX) 
                return -EFAULT;
        local_irq_save(flags);
        info->rxbuf_ptr = info->rxbuf;
        info->rxbuf_end = info->rxbuf + len;
        if (info->opts.modeRxTx == IFX_SSC_MODE_RXTX) {
                if ((info->txbuf == NULL) ||
                    (info->txbuf != info->txbuf_ptr) ||
                    (info->txbuf_end != len + info->txbuf)) {
                        local_irq_restore(flags);
                        printk("IFX SSC - %s: write must be called before calling "
                               "read in combined RX/TX!\n", __FUNCTION__);
                        return -EFAULT;
                }
		local_irq_restore(flags);
		/* should enable tx, right?*/
		tx_int(info);
		if (!in_irq()){
			if (info->txbuf_ptr < info->txbuf_end){
				ifx_int_wrapper.enable(info->txirq);
			}
			ifx_int_wrapper.enable(info->rxirq);
		}
        } else { // rx mode
		local_irq_restore(flags);
                if (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RXCNT) &
                    IFX_SSC_RXCNT_TODO_MASK)
                        return -EBUSY;
		if (!in_irq()){
			ifx_int_wrapper.enable(info->rxirq);
		}

                if (len < IFX_SSC_RXREQ_BLOCK_SIZE)
                        WRITE_PERIPHERAL_REGISTER(len << IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
                else 
			WRITE_PERIPHERAL_REGISTER(IFX_SSC_RXREQ_BLOCK_SIZE << IFX_SSC_RXREQ_RXCOUNT_OFFSET, 
                                                  info->mapbase + IFX_SSC_RXREQ);
        }
	if (in_irq()){
		do {
			rx_int(info);
			if (info->opts.modeRxTx == IFX_SSC_MODE_RXTX) {
				tx_int(info);
			}
			
                	if (info->rxbuf_ptr >= info->rxbuf_end)
                        	break;
        	} while (1);
		ret_val = info->rxbuf_ptr - info->rxbuf;
	}else{
        	__add_wait_queue(&info->rwait, &wait);
        	set_current_state(TASK_INTERRUPTIBLE);
        	// wakeup done in rx_int
 	
        	do {
			local_irq_save(flags);
                	if (info->rxbuf_ptr >= info->rxbuf_end)
                        	break;
			local_irq_restore(flags);

                	if (signal_pending(current)) {
                        	ret_val = -ERESTARTSYS;
                        	goto out;
                	}
                	schedule();
        	} while (1);

        	ret_val = info->rxbuf_ptr - info->rxbuf; // should be equal to len
		local_irq_restore(flags);

  out:
        	current->state = TASK_RUNNING;
        	__remove_wait_queue(&info->rwait, &wait);
	}
	return (ret_val);
} // ifx_ssc_read_helper
#endif

/* helper routine to handle writes to the kernel or user-space */
/* info->txbuf has two cases:
 *	1) return value < 0 (-EFAULT), not touched at all
 *	2) kfree and points to NULL in interrupt routine (but maybe later )
 */
static ssize_t
ifx_ssc_write_helper(struct ifx_ssc_port *info, const char *buf,
		size_t len, int from_kernel)
{
        // check if in tx or tx/rx mode
        if (info->opts.modeRxTx == IFX_SSC_MODE_RX)
                return -EFAULT;

        info->txbuf_ptr = info->txbuf;
        info->txbuf_end = len + info->txbuf;
	/* start the transmission (not in rx/tx, see read helper) */
        if (info->opts.modeRxTx == IFX_SSC_MODE_TX) {
		tx_int(info);
		if (info->txbuf_ptr < info->txbuf_end){
			ifx_int_wrapper.enable(info->txirq);
		}
        }
	//local_irq_restore(flags);
        return len;
}

/*
 * kernel interfaces for read and write.
 * The caller must set port to: n for SSC<m> with n=m-1 (e.g. n=0 for SSC1)
 */
ssize_t
ifx_ssc_kread(int port, char *kbuf, size_t len)
{
	struct ifx_ssc_port *info;
        ssize_t ret_val;

	if (port < 0 || port >= PORT_CNT)
		return -ENXIO;

        if (len == 0)
                return 0;

	info = &isp[port];

        // check if reception in progress
        if (info->rxbuf != NULL){
		printk("SSC device busy\n");
                return -EBUSY;
	}

	info->rxbuf = kbuf;
        if (info->rxbuf == NULL){
		printk("SSC device error\n");
                return -EINVAL;
	}

/* changed by bingtao */
	/* change by TaiCheng */
	//if (!in_irq()){
	if (0){
		ret_val = ifx_ssc_read_helper(info, kbuf, len, 1);
	}else{
		ret_val = ifx_ssc_read_helper_poll(info, kbuf, len, 1);
	};		
	info->rxbuf = NULL;

        // ### TO DO: perhaps warn if ret_val != len
	ifx_int_wrapper.disable(info->rxirq);

        return (ret_val);
} // ifx_ssc_kread
EXPORT_SYMBOL(ifx_ssc_kread);

ssize_t
ifx_ssc_kwrite(int port, const char *kbuf, size_t len)
{
	struct ifx_ssc_port *info;
	ssize_t ret_val;

	if (port < 0 || port >= PORT_CNT)
		return -ENXIO;

        if (len == 0)
                return 0;

	info = &isp[port];

        // check if transmission in progress
        if (info->txbuf != NULL)
                return -EBUSY;
        info->txbuf = (char *)kbuf;

	ret_val = ifx_ssc_write_helper(info, info->txbuf, len, 1);
	if (ret_val < 0){
		info->txbuf = NULL;
	}
	return ret_val;
}
EXPORT_SYMBOL(ifx_ssc_kwrite);


/* 
 * user interfaces to read and write
 */
static ssize_t
ifx_ssc_read(struct file *filp, char *ubuf, size_t len, loff_t *off)
{
        ssize_t ret_val;
        int idx;
	struct ifx_ssc_port *info;

/*
        if (len == 0)
                return (0);
*/
        idx = MINOR(filp->f_dentry->d_inode->i_rdev);
	info = &isp[idx];

        // check if reception in progress
        if (info->rxbuf != NULL)
                return -EBUSY;

	info->rxbuf = kmalloc(len+ 3, GFP_KERNEL);
        if (info->rxbuf == NULL)
                return -ENOMEM;

	ret_val = ifx_ssc_read_helper(info, info->rxbuf, len, 0);
	// ### TO DO: perhaps warn if ret_val != len
        if (copy_to_user((void*)ubuf, info->rxbuf, ret_val) != 0)
                ret_val = -EFAULT;

	ifx_int_wrapper.disable(info->rxirq);

        kfree(info->rxbuf);
	info->rxbuf = NULL;
        return (ret_val);
} // ifx_ssc_read

/*
 * As many bytes as we have free space for are copied from the user
 * into txbuf and the actual byte count is returned. The transmission is
 * always kicked off by calling the appropriate TX routine.
 */
static ssize_t
ifx_ssc_write(struct file *filp, const char *ubuf, size_t len, loff_t *off)
{
        int idx;
	struct ifx_ssc_port *info;
	int ret_val;

        if (len == 0)
                return (0);

        idx = MINOR(filp->f_dentry->d_inode->i_rdev);
	info = &isp[idx];

        // check if transmission in progress
        if (info->txbuf != NULL)
                return -EBUSY;

	info->txbuf = kmalloc(len+ 3, GFP_KERNEL);
        if (info->txbuf == NULL)
                return -ENOMEM;

	ret_val = copy_from_user(info->txbuf, ubuf, len);
	if (ret_val == 0)
		ret_val = ifx_ssc_write_helper(info, info->txbuf, len, 0);
	else
		ret_val = -EFAULT;
	if (ret_val < 0) {
                kfree(info->txbuf); // otherwise will be done in ISR
		info->txbuf = NULL;
	}
	return (ret_val);
} /* ifx_ssc_write */


/*
 * ------------------------------------------------------------
 * ifx_ssc_ioctl() and friends
 * ------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 FUNC-NAME  : ifx_ssc_frm_status_get
 LONG-NAME  : framing status get
 PURPOSE    : Get the actual status of the framing.

 PARAMETER  : *info	pointer to the port-specific structure ifx_ssc_port.

 RESULT     : pointer to a structure ifx_ssc_frm_status which holds busy and 
	      count values.

 REMARKS    : Returns a register value independent of framing is enabled or 
	      not! Changes structure inside of info, so the return value isn't 
	      needed at all, but could be used for simple access.
-----------------------------------------------------------------------------*/
static struct ifx_ssc_frm_status *
ifx_ssc_frm_status_get(struct ifx_ssc_port *info)
{
	unsigned long tmp;
	
	tmp = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_SFSTAT);
	info->frm_status.DataBusy = (tmp & IFX_SSC_SFSTAT_IN_DATA) > 0;
	info->frm_status.PauseBusy = (tmp & IFX_SSC_SFSTAT_IN_PAUSE) > 0;
	info->frm_status.DataCount = (tmp & IFX_SSC_SFSTAT_DATA_COUNT_MASK) 
		>> IFX_SSC_SFSTAT_DATA_COUNT_OFFSET;
	info->frm_status.PauseCount = (tmp & IFX_SSC_SFSTAT_PAUSE_COUNT_MASK) 
		>> IFX_SSC_SFSTAT_PAUSE_COUNT_OFFSET;
	tmp = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_SFCON);
	info->frm_status.EnIntAfterData = 
		(tmp & IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE) > 0;
	info->frm_status.EnIntAfterPause = 
		(tmp & IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE) > 0;
	return (&info->frm_status);
} // ifx_ssc_frm_status_get


/*-----------------------------------------------------------------------------
 FUNC-NAME  : ifx_ssc_frm_control_get
 LONG-NAME  : framing control get
 PURPOSE    : Get the actual control values of the framing.

 PARAMETER  : *info	pointer to the port-specific structure ifx_ssc_port.

 RESULT     : pointer to a structure ifx_ssc_frm_opts which holds control bits  
	      and count reload values.

 REMARKS    : Changes structure inside of info, so the return value isn't 
	      needed at all, but could be used for simple access.
-----------------------------------------------------------------------------*/
static struct ifx_ssc_frm_opts *
ifx_ssc_frm_control_get(struct ifx_ssc_port *info)
{
	unsigned long tmp;
	
	tmp = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_SFCON);
	info->frm_opts.FrameEnable = (tmp & IFX_SSC_SFCON_SF_ENABLE) > 0;
	info->frm_opts.DataLength = (tmp & IFX_SSC_SFCON_DATA_LENGTH_MASK) 
		>> IFX_SSC_SFCON_DATA_LENGTH_OFFSET;
	info->frm_opts.PauseLength = (tmp & IFX_SSC_SFCON_PAUSE_LENGTH_MASK) 
		>> IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET;
	info->frm_opts.IdleData = (tmp & IFX_SSC_SFCON_PAUSE_DATA_MASK) 
		>> IFX_SSC_SFCON_PAUSE_DATA_OFFSET;
	info->frm_opts.IdleClock = (tmp & IFX_SSC_SFCON_PAUSE_CLOCK_MASK) 
		>> IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET;
	info->frm_opts.StopAfterPause = 
		(tmp & IFX_SSC_SFCON_STOP_AFTER_PAUSE) > 0;
	return (&info->frm_opts);
} // ifx_ssc_frm_control_get


/*-----------------------------------------------------------------------------
 FUNC-NAME  : ifx_ssc_frm_control_set
 LONG-NAME  : framing control set
 PURPOSE    : Set the actual control values of the framing.

 PARAMETER  : *info	pointer to the port-specific structure ifx_ssc_port.

 RESULT     : pointer to a structure ifx_ssc_frm_opts which holds control bits  
	      and count reload values.

 REMARKS    : 
-----------------------------------------------------------------------------*/
static int
ifx_ssc_frm_control_set(struct ifx_ssc_port *info)
{
	unsigned long tmp;

	// check parameters
	if ((info->frm_opts.DataLength > IFX_SSC_SFCON_DATA_LENGTH_MAX) || 
	    (info->frm_opts.DataLength < 1) ||
	    (info->frm_opts.PauseLength > IFX_SSC_SFCON_PAUSE_LENGTH_MAX) || 
	    (info->frm_opts.PauseLength < 1) ||
	    ((info->frm_opts.IdleData & ~(IFX_SSC_SFCON_PAUSE_DATA_MASK >> 
					  IFX_SSC_SFCON_PAUSE_DATA_OFFSET)) != 0 ) || 
	    ((info->frm_opts.IdleClock & ~(IFX_SSC_SFCON_PAUSE_CLOCK_MASK >> 
					   IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET)) != 0 ))
		return -EINVAL;

	// read interrupt bits (they're not changed here)
	tmp = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_SFCON) &
		(IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE | 
		 IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE);

	// set all values with respect to it's bit position (for data and pause 
	// length set N-1)
	tmp = (info->frm_opts.DataLength - 1) << IFX_SSC_SFCON_DATA_LENGTH_OFFSET;
	tmp |= (info->frm_opts.PauseLength - 1) << IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET;
	tmp |= info->frm_opts.IdleData << IFX_SSC_SFCON_PAUSE_DATA_OFFSET;
	tmp |= info->frm_opts.IdleClock << IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET;
	tmp |= info->frm_opts.FrameEnable * IFX_SSC_SFCON_SF_ENABLE;
	tmp |= info->frm_opts.StopAfterPause * IFX_SSC_SFCON_STOP_AFTER_PAUSE;

	WRITE_PERIPHERAL_REGISTER(tmp, info->mapbase + IFX_SSC_SFCON);

	return 0;
} // ifx_ssc_frm_control_set


/*-----------------------------------------------------------------------------
 FUNC-NAME  : ifx_ssc_rxtx_mode_set
 LONG-NAME  : rxtx mode set
 PURPOSE    : Set the transmission mode.

 PARAMETER  : *info	pointer to the port-specific structure ifx_ssc_port.

 RESULT     : Returns error code

 REMARKS    : Assumes that SSC not used (SSC disabled, device not opened yet 
	      or just closed) 
-----------------------------------------------------------------------------*/
static int
ifx_ssc_rxtx_mode_set(struct ifx_ssc_port *info, unsigned int val)
{
	unsigned long tmp;

	// check parameters
	if (!(info) || (val & ~(IFX_SSC_MODE_MASK)))
		return -EINVAL;
	/*check BUSY and RXCNT*/	
	if (  READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) & IFX_SSC_STATE_BUSY
	    ||READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_RXCNT) & IFX_SSC_RXCNT_TODO_MASK)
		return -EBUSY;
	// modify 
	tmp = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_CON) &
		~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF)) | (val);
	WRITE_PERIPHERAL_REGISTER(tmp, info->mapbase + IFX_SSC_CON);
	info->opts.modeRxTx = val;
/*	
	printk(KERN_DEBUG "IFX SSC%d: Setting mode to %s%s\n", 
	       info->port_nr,
	       ((val & IFX_SSC_CON_RX_OFF) == 0) ? "rx ":"", 
	       ((val & IFX_SSC_CON_TX_OFF) == 0) ? "tx":"");
*/
	return 0;
} // ifx_ssc_rxtx_mode_set

void ifx_gpio_init(void) 
{
	u32	temp;
/* set gpio pin p0.10(SPI_DIN) p0.11(SPI_DOUT) p0.12(SPI_CLK) p0.13(SPI_CS2) direction */
	temp = *(AMAZON_GPIO_P0_DIR) ;
      	temp &= 0xFFFFFBFF;
      	temp |= 0x3800;
	*(AMAZON_GPIO_P0_DIR) = temp;
/* set port 0 alternate select register 0 */
	temp = *(AMAZON_GPIO_P0_ALTSEL0) ;
	temp &= 0xFFFFC3FF;
	temp |= 0x00001c00;
	*(AMAZON_GPIO_P0_ALTSEL0) = temp;

/* set port 0 alternate select register 1 */
 	temp = *(AMAZON_GPIO_P0_ALTSEL1) ;
	temp &= 0xFFFFC3FF;
	temp |= 0x00002000;
	*(AMAZON_GPIO_P0_ALTSEL1) = temp;

/* set port 0 open drain mode register */
	temp = *(AMAZON_GPIO_P0_OD);
	temp |= 0x00003800;	/* set output pin normal mode */
	*(AMAZON_GPIO_P0_OD)= temp;
}

/*
 * This routine intializes the SSC appropriately depending
 * on slave/master and full-/half-duplex mode.
 * It assumes that the SSC is disabled and the fifo's and buffers 
 * are flushes later on.
 */
static int
ifx_ssc_sethwopts(struct ifx_ssc_port *info)
{
	unsigned long flags, bits;
	struct ifx_ssc_hwopts *opts = &info->opts;

	/* sanity checks */
	if ((opts->dataWidth < IFX_SSC_MIN_DATA_WIDTH) ||
	    (opts->dataWidth > IFX_SSC_MAX_DATA_WIDTH)) {
		printk("%s: sanity check failed\n", __FUNCTION__);
	    	return -EINVAL;
	}
        bits = (opts->dataWidth - 1) << IFX_SSC_CON_DATA_WIDTH_OFFSET;
	bits |= IFX_SSC_CON_ENABLE_BYTE_VALID;
//	if (opts->abortErrDetect)
//		bits |= IFX_SSC_CON_ABORT_ERR_CHECK;
	if (opts->rxOvErrDetect)
		bits |= IFX_SSC_CON_RX_OFL_CHECK;
	if (opts->rxUndErrDetect)
		bits |= IFX_SSC_CON_RX_UFL_CHECK;
	if (opts->txOvErrDetect)
		bits |= IFX_SSC_CON_TX_OFL_CHECK;
	if (opts->txUndErrDetect)
		bits |= IFX_SSC_CON_TX_UFL_CHECK;
	if (opts->loopBack)
		bits |= IFX_SSC_CON_LOOPBACK_MODE;
	if (opts->echoMode)
		bits |= IFX_SSC_CON_ECHO_MODE_ON;
	if (opts->headingControl)
		bits |= IFX_SSC_CON_MSB_FIRST;
	if (opts->clockPhase)
		bits |= IFX_SSC_CON_LATCH_THEN_SHIFT;
	if (opts->clockPolarity)
		bits |= IFX_SSC_CON_CLOCK_FALL;
	switch (opts->modeRxTx) {
	case IFX_SSC_MODE_TX:
		bits |= IFX_SSC_CON_RX_OFF;
		break;
	case IFX_SSC_MODE_RX:
		bits |= IFX_SSC_CON_TX_OFF;
		break;
	} // switch (opts->modeRxT)
	local_irq_save(flags);
        WRITE_PERIPHERAL_REGISTER(bits, info->mapbase + IFX_SSC_CON);
        WRITE_PERIPHERAL_REGISTER((info->opts.gpoCs << IFX_SSC_GPOCON_ISCSB0_POS) | 
				  (info->opts.gpoInv << IFX_SSC_GPOCON_INVOUT0_POS), 
				  info->mapbase + IFX_SSC_GPOCON);
	//master mode
	if (opts->masterSelect){
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_SET_MASTER_SELECT,info->mapbase + IFX_SSC_WHBSTATE);
	}else{
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_MASTER_SELECT,info->mapbase + IFX_SSC_WHBSTATE);
	}
	// init serial framing
        WRITE_PERIPHERAL_REGISTER(0, info->mapbase + IFX_SSC_SFCON);
	/* set up the port pins */
        //check for general requirements to switch (external) pad/pin characteristics
	ifx_gpio_init();
	local_irq_restore(flags);

	return 0;
} // ifx_ssc_sethwopts

static int
ifx_ssc_set_baud(struct ifx_ssc_port *info, unsigned int baud)
{
	unsigned int ifx_ssc_clock;
	unsigned int br;
	unsigned long flags;
	bool enabled;

	ifx_ssc_clock = ifx_ssc_get_kernel_clk(info);
	if (ifx_ssc_clock ==0)
		return -EINVAL;

	local_irq_save(flags);
	/* have to disable the SSC to set the baudrate */
	enabled = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) 
		   & IFX_SSC_STATE_IS_ENABLED) != 0;
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ENABLE, 
				  info->mapbase + IFX_SSC_WHBSTATE);

	// compute divider
	br = ((ifx_ssc_clock >> 1)/baud) - 1;
	asm("SYNC"); 
	if (br > 0xffff || 
	    ((br == 0) && 
	     ((READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) & 
	       IFX_SSC_STATE_IS_MASTER) == 0))){
		local_irq_restore(flags);
		printk("%s: illegal baudrate %u\n", __FUNCTION__, baud);
		return -EINVAL;
	}
	WRITE_PERIPHERAL_REGISTER(br, info->mapbase + IFX_SSC_BR);
	if (enabled)
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_SET_ENABLE, 
					  info->mapbase + IFX_SSC_WHBSTATE);

	local_irq_restore(flags);
	return 0;
} // ifx_ssc_set_baud

static int
ifx_ssc_hwinit(struct ifx_ssc_port *info)
{
	unsigned long flags;
	bool enabled;

	/* have to disable the SSC */
	enabled = (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) 
		   & IFX_SSC_STATE_IS_ENABLED) != 0;
	WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ENABLE, 
				  info->mapbase + IFX_SSC_WHBSTATE);

	if (ifx_ssc_sethwopts(info) < 0)
	{
		printk("%s: setting the hardware options failed\n",
                       __FUNCTION__);
		return -EINVAL;
	}

	if (ifx_ssc_set_baud(info, info->baud) < 0) {
		printk("%s: setting the baud rate failed\n", __FUNCTION__);
		return -EINVAL;
	}
        local_irq_save(flags);
        /* TX FIFO */
        WRITE_PERIPHERAL_REGISTER((IFX_SSC_DEF_TXFIFO_FL << 
                                   IFX_SSC_XFCON_ITL_OFFSET) | 
                                  IFX_SSC_XFCON_FIFO_ENABLE, 
                                  info->mapbase + IFX_SSC_TXFCON);
        /* RX FIFO */
        WRITE_PERIPHERAL_REGISTER((IFX_SSC_DEF_RXFIFO_FL << 
                                   IFX_SSC_XFCON_ITL_OFFSET) | 
                                  IFX_SSC_XFCON_FIFO_ENABLE, 
                                  info->mapbase + IFX_SSC_RXFCON);
        local_irq_restore(flags);
	if (enabled)
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_SET_ENABLE, 
					  info->mapbase + IFX_SSC_WHBSTATE);
	return 0;
} // ifx_ssc_hwinit

/*-----------------------------------------------------------------------------
 FUNC-NAME  : ifx_ssc_batch_exec
 LONG-NAME  : 
 PURPOSE    : 

 PARAMETER  : *info	pointer to the port-specific structure ifx_ssc_port.

 RESULT     : Returns error code

 REMARKS    : 
-----------------------------------------------------------------------------*/
static int
ifx_ssc_batch_exec(struct ifx_ssc_port *info, struct ifx_ssc_batch_list *batch_anchor)
{
	// ### TO DO: implement user space batch execution
	// first, copy the whole linked list from user to kernel space
	// save some hardware options
	// execute list
	// restore hardware options if selected
	return -EFAULT;
} // ifx_ssc_batch_exec


/*
 * This routine allows the driver to implement device-
 * specific ioctl's.  If the ioctl number passed in cmd is
 * not recognized by the driver, it should return ENOIOCTLCMD.
 */
int
ifx_ssc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
              unsigned long data)
{
	struct ifx_ssc_port *info;
	int line, ret_val = 0;
	unsigned long flags;
	unsigned long tmp;
	int from_kernel = 0;

	if ((inode == (struct inode *)0) || (inode == (struct inode *)1))
	{
		from_kernel = 1;
		line = (int)inode;
	}
	else
		line = MINOR(filp->f_dentry->d_inode->i_rdev);

	/* don't use more minor devices than we can support */
	if (line < 0 || line >= PORT_CNT)
		return -ENXIO;

	info = &isp[line];

	switch (cmd) {
	case IFX_SSC_STATS_READ:
		/* data must be a pointer to a struct ifx_ssc_statistics */
		if (from_kernel)
			memcpy((void *)data, (void *)&info->stats,
			       sizeof(struct ifx_ssc_statistics));
		else
			if (copy_to_user((void *)data,
					 (void *)&info->stats,
					 sizeof(struct ifx_ssc_statistics)))
				ret_val = -EFAULT;
		break;
	case IFX_SSC_STATS_RESET:
		/* just resets the statistics counters */
		memset((void *)&info->stats, 0, sizeof(struct ifx_ssc_statistics));
		break;
        case IFX_SSC_BAUD_SET:
                /* if the buffers are not empty then the port is */
                /* busy and we shouldn't change things on-the-fly! */
                if (!info->txbuf || !info->rxbuf || 
		    (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) 
		     & IFX_SSC_STATE_BUSY)) {
                        ret_val = -EBUSY;
                        break;
                }
                /* misuse flags */
                if (from_kernel)
                        flags = *((unsigned long *)data);
                else
                        if (copy_from_user((void *)&flags,
                                           (void *)data, sizeof(flags)))
                        {
                                ret_val = -EFAULT;
                                break;
                        }
                if (flags == 0)
                {
                        ret_val = -EINVAL;
                        break;
                }
                if (ifx_ssc_set_baud(info, flags) < 0)
                {
                        ret_val = -EINVAL;
                        break;
                }
                info->baud = flags;
                break;
        case IFX_SSC_BAUD_GET:
                if (from_kernel)
                        *((unsigned int *)data) = info->baud;
                else
                        if (copy_to_user((void *)data,
                                         (void *)&info->baud,
                                         sizeof(unsigned long)))
                                ret_val = -EFAULT;
                break;
	case IFX_SSC_RXTX_MODE_SET:
                if (from_kernel)
                        tmp = *((unsigned long *)data);
                else
                        if (copy_from_user((void *)&tmp,
                                           (void *)data, sizeof(tmp))) {
                                ret_val = -EFAULT;
                                break;
                        }
		ret_val = ifx_ssc_rxtx_mode_set(info, tmp);
		break;
	case IFX_SSC_RXTX_MODE_GET:
		tmp = READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_CON) &
			(~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF));
                if (from_kernel)
                        *((unsigned int *)data) = tmp;
                else
                        if (copy_to_user((void *)data,
                                         (void *)&tmp,
                                         sizeof(tmp)))
                                ret_val = -EFAULT;
		break;

	case IFX_SSC_ABORT:
		ifx_ssc_abort(info);
		break;

	case IFX_SSC_GPO_OUT_SET:
                if (from_kernel)
                        tmp = *((unsigned long *)data);
                else
                        if (copy_from_user((void *)&tmp,
                                           (void *)data, sizeof(tmp))) {
                                ret_val = -EFAULT;
                                break;
                        }
		if (tmp > IFX_SSC_MAX_GPO_OUT)
			ret_val = -EINVAL;
		else
			WRITE_PERIPHERAL_REGISTER
				(1<<(tmp + IFX_SSC_WHBGPOSTAT_SETOUT0_POS), 
				 info->mapbase + IFX_SSC_WHBGPOSTAT);
		break;
	case IFX_SSC_GPO_OUT_CLR:
                if (from_kernel)
                        tmp = *((unsigned long *)data);
                else
                        if (copy_from_user((void *)&tmp,
                                           (void *)data, sizeof(tmp))) {
                                ret_val = -EFAULT;
                                break;
                        }
		if (tmp > IFX_SSC_MAX_GPO_OUT)
			ret_val = -EINVAL;
		else {
			WRITE_PERIPHERAL_REGISTER
				(1<<(tmp + IFX_SSC_WHBGPOSTAT_CLROUT0_POS), 
				 info->mapbase + IFX_SSC_WHBGPOSTAT);
		}
		break;
	case IFX_SSC_GPO_OUT_GET:
		tmp = READ_PERIPHERAL_REGISTER
			(info->mapbase + IFX_SSC_GPOSTAT);
                if (from_kernel)
                        *((unsigned int *)data) = tmp;
                else
                        if (copy_to_user((void *)data,
                                         (void *)&tmp,
                                         sizeof(tmp)))
                                ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_STATUS_GET:
		ifx_ssc_frm_status_get(info);
		if (from_kernel)
			memcpy((void *)data, (void *)&info->frm_status,
			       sizeof(struct ifx_ssc_frm_status));
		else
			if (copy_to_user((void *)data,
					 (void *)&info->frm_status,
					 sizeof(struct ifx_ssc_frm_status)))
				ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_CONTROL_GET:
		ifx_ssc_frm_control_get(info);
		if (from_kernel)
			memcpy((void *)data, (void *)&info->frm_opts,
			       sizeof(struct ifx_ssc_frm_opts));
		else
			if (copy_to_user((void *)data,
					 (void *)&info->frm_opts,
					 sizeof(struct ifx_ssc_frm_opts)))
				ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_CONTROL_SET:
		if (from_kernel)
			memcpy((void *)&info->frm_opts, (void *)data, 
			       sizeof(struct ifx_ssc_frm_opts));
		else
			if (copy_to_user((void *)&info->frm_opts,
					 (void *)data,
					 sizeof(struct ifx_ssc_frm_opts))){
				ret_val = -EFAULT;
				break;
			}
		ret_val = ifx_ssc_frm_control_set(info);
		break;
        case IFX_SSC_HWOPTS_SET:
		/* data must be a pointer to a struct ifx_ssc_hwopts */
                /* if the buffers are not empty then the port is */
                /* busy and we shouldn't change things on-the-fly! */
                if (!info->txbuf || !info->rxbuf || 
		    (READ_PERIPHERAL_REGISTER(info->mapbase + IFX_SSC_STATE) 
		     & IFX_SSC_STATE_BUSY)) {
                        ret_val = -EBUSY;
                        break;
                }
                if (from_kernel)
                        memcpy((void *)&info->opts, (void *)data,
                               sizeof(struct ifx_ssc_hwopts));
                else
                        if (copy_from_user((void *)&info->opts,
                                           (void *)data,
                                           sizeof(struct ifx_ssc_hwopts)))
                        {
                                ret_val = -EFAULT;
                                break;
                        }
                if (ifx_ssc_hwinit(info) < 0)
                {
                        ret_val = -EIO;
                }
                break;
        case IFX_SSC_HWOPTS_GET:
		/* data must be a pointer to a struct ifx_ssc_hwopts */
                if (from_kernel)
                        memcpy((void *)data, (void *)&info->opts,
                               sizeof(struct ifx_ssc_hwopts));
                else
                        if (copy_to_user((void *)data,
                                         (void *)&info->opts,
                                         sizeof(struct ifx_ssc_hwopts)))
                                ret_val = -EFAULT;
                break;
        default:
                ret_val = -ENOIOCTLCMD;
	}

	return ret_val;
} /* ifx_ssc_ioctl */
EXPORT_SYMBOL(ifx_ssc_ioctl);

///* the poll routine */
//static unsigned int
//ifx_ssc_poll(struct file *filp, struct poll_table_struct *pts)
//{
//        int unit = MINOR(filp->f_dentry->d_inode->i_rdev);
//	struct ifx_ssc_port *info;
//        unsigned int mask = 0; 
//	int spc;
//
//	info = &isp[unit];
//
//        /* add event to the wait queues */
//        /* DO NOT FORGET TO DO A WAKEUP ON THESE !!!! */
//        poll_wait(filp, &info->pwait, pts);
//
//	/* are there bytes in the RX SW-FIFO? */
//        if (info->rxrp != info->rxwp)
//                mask |= POLLIN | POLLRDNORM;
//
//	/* free space in the TX SW-FIFO */
//	spc = info->txrp - info->txwp - 1;
//	if (spc < 0)
//		spc += TX_BUFSIZE;
//#ifdef IFX_SSC_USEDMA
//	/* writing always works, except in the DMA case when all descriptors */
//	/* are used up */
//	if (unit == 1 && info->dma_freecnt == 0)
//		spc = 0;
//#endif
//	if (spc > 0)
//		mask |= POLLOUT | POLLWRNORM;
//
//        return (mask);
//}

static int
ifx_ssc1_read_proc(char *page, char **start, off_t offset, int count, int *eof, void *data)
{
	int off = 0;
	unsigned long flags;

	/* don't want any interrupts here */
	save_flags(flags);
	cli();


	/* print statistics */
	off += sprintf(page+off, "Statistics for Infineon Synchronous Serial Controller SSC1\n");
	off += sprintf(page+off, "RX overflow errors %d\n", isp[0].stats.rxOvErr);
	off += sprintf(page+off, "RX underflow errors %d\n", isp[0].stats.rxUnErr);
	off += sprintf(page+off, "TX overflow errors %d\n", isp[0].stats.txOvErr);
	off += sprintf(page+off, "TX underflow errors %d\n", isp[0].stats.txUnErr);
	off += sprintf(page+off, "Abort errors %d\n", isp[0].stats.abortErr);
	off += sprintf(page+off, "Mode errors %d\n", isp[0].stats.modeErr);
	off += sprintf(page+off, "RX Bytes %d\n", isp[0].stats.rxBytes);
	off += sprintf(page+off, "TX Bytes %d\n", isp[0].stats.txBytes);

	restore_flags (flags); /* XXXXX */
	*eof = 1;
	return (off);
}


/*
 * This routine prints out the appropriate serial driver version number
 */
static inline void
show_version(void)
{
#if 0
    printk("Infineon Technologies Synchronous Serial Controller (SSC) driver\n"
           "  version %s - built %s %s\n", IFX_SSC_DRV_VERSION, __DATE__, __TIME__);
#endif	 
} /* show_version */


/*
 * Due to the fact that a port can be dynamically switched between slave
 * and master mode using an IOCTL the hardware is not initialized here,
 * but in ifx_ssc_hwinit() as a result of an IOCTL.
 */
int __init
ifx_ssc_init(void)
{
	struct ifx_ssc_port  *info;
	int i, nbytes;
	unsigned long flags;
	int ret_val;

	// ### TO DO: dynamic port count evaluation due to pin multiplexing

	ret_val = -ENOMEM;
	nbytes = PORT_CNT * sizeof(struct ifx_ssc_port);
	isp = (struct ifx_ssc_port *)kmalloc(nbytes, GFP_KERNEL);
	if (isp == NULL)
	{
		printk("%s: no memory for isp\n", __FUNCTION__);
		return (ret_val);
	}
	memset(isp, 0, nbytes);

	show_version();

	/* register the device */
	ret_val = -ENXIO;
/*
	i = maj;
*/
	if ((i = register_chrdev(maj, "ssc", &ifx_ssc_fops)) < 0)
	{
		printk("Unable to register major %d for the Infineon SSC\n", maj);
		if (maj == 0){
			goto errout;
		}
		else{
			maj = 0;
			if ((i = register_chrdev(maj, "ssc", &ifx_ssc_fops)) < 0)
			{
				printk("Unable to register major %d for the Infineon SSC\n", maj);
				goto errout;
			}
		}
	}
	if (maj == 0) maj = i;
	//printk("registered major %d for Infineon SSC\n", maj);

	/* set default values in ifx_ssc_port */
	for (i = 0; i < PORT_CNT; i++) {
		info = &isp[i];
                info->port_nr = i;
		/* default values for the HwOpts */
		info->opts.AbortErrDetect = IFX_SSC_DEF_ABRT_ERR_DETECT;
		info->opts.rxOvErrDetect = IFX_SSC_DEF_RO_ERR_DETECT;
		info->opts.rxUndErrDetect = IFX_SSC_DEF_RU_ERR_DETECT;
		info->opts.txOvErrDetect = IFX_SSC_DEF_TO_ERR_DETECT;
		info->opts.txUndErrDetect = IFX_SSC_DEF_TU_ERR_DETECT;
		info->opts.loopBack = IFX_SSC_DEF_LOOP_BACK;
		info->opts.echoMode = IFX_SSC_DEF_ECHO_MODE;
		info->opts.idleValue = IFX_SSC_DEF_IDLE_DATA;
		info->opts.clockPolarity = IFX_SSC_DEF_CLOCK_POLARITY;
		info->opts.clockPhase = IFX_SSC_DEF_CLOCK_PHASE;
		info->opts.headingControl = IFX_SSC_DEF_HEADING_CONTROL;
		info->opts.dataWidth = IFX_SSC_DEF_DATA_WIDTH;
		info->opts.modeRxTx = IFX_SSC_DEF_MODE_RXTX;
		info->opts.gpoCs = IFX_SSC_DEF_GPO_CS;
		info->opts.gpoInv = IFX_SSC_DEF_GPO_INV;
		info->opts.masterSelect = IFX_SSC_DEF_MASTERSLAVE;
		info->baud = IFX_SSC_DEF_BAUDRATE;
		info->rxbuf = NULL;
		info->txbuf = NULL;
		/* values specific to SSC1 */
		if (i == 0) {
			info->mapbase = AMAZON_SSC_BASE_ADD_0;
			// ### TO DO: power management

			// setting interrupt vectors
			info->txirq = IFX_SSC_TIR;
			info->rxirq = IFX_SSC_RIR;
			info->errirq = IFX_SSC_EIR;
/*
			info->frmirq = IFX_SSC_FIR;
*/
		}
		/* activate SSC */
		/* CLC.DISS = 0 */
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_DEF_RMC << IFX_CLC_RUN_DIVIDER_OFFSET, info->mapbase + IFX_SSC_CLC);

// ### TO DO: multiple instances

		init_waitqueue_head(&info->rwait);
		//init_waitqueue_head(&info->pwait);

		local_irq_save(flags);

		// init serial framing register
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_DEF_SFCON, info->mapbase + IFX_SSC_SFCON);

		/* try to get the interrupts */
		// ### TO DO: interrupt handling with multiple instances
		ret_val = ifx_int_wrapper.request(info->txirq, ifx_ssc_tx_int, 
						  0, "ifx_ssc_tx", info);
		if (ret_val){
			printk("%s: unable to get irq %d\n", __FUNCTION__,
					info->txirq);
			local_irq_restore(flags);
			goto errout;
		}
		ret_val = ifx_int_wrapper.request(info->rxirq, ifx_ssc_rx_int, 
						  0, "ifx_ssc_rx", info);
		if (ret_val){
			printk("%s: unable to get irq %d\n", __FUNCTION__,
					info->rxirq);
			local_irq_restore(flags);
			goto irqerr;
		}
		ret_val = ifx_int_wrapper.request(info->errirq, ifx_ssc_err_int, 
						  0, "ifx_ssc_err", info);
		if (ret_val){
			printk("%s: unable to get irq %d\n", __FUNCTION__,
					info->errirq);
			local_irq_restore(flags);
			goto irqerr;
		}
/*
		ret_val = ifx_int_wrapper.request(info->frmirq, ifx_ssc_frm_int, 
						  0, "ifx_ssc_frm", info);
		if (ret_val){
			printk("%s: unable to get irq %d\n", __FUNCTION__,
					info->frmirq);
			local_irq_restore(flags);
			goto irqerr;
		}

*/
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_DEF_IRNEN, info->mapbase + IFX_SSC_IRN_EN);

		local_irq_restore(flags);
	} // for (i = 0; i < PORT_CNT; i++)

	/* init the SSCs with default values */
	for (i = 0; i < PORT_CNT; i++)
	{
		info = &isp[i];
		if (ifx_ssc_hwinit(info) < 0)
		{
			printk("%s: hardware init failed for port %d\n",
				__FUNCTION__, i);
			goto irqerr;
		}
	}

	/* register /proc read handler */
	// ### TO DO: multiple instances
	/* for SSC1, which is always present */
	create_proc_read_entry("driver/ssc1", 0, NULL, ifx_ssc1_read_proc, NULL);
	return 0;

irqerr:
	// ### TO DO: multiple instances
	ifx_int_wrapper.free(isp[0].txirq,&isp[0]);
	ifx_int_wrapper.free(isp[0].rxirq,&isp[0]);
	ifx_int_wrapper.free(isp[0].errirq,&isp[0]);
/*
	ifx_int_wrapper.free(isp[0].frmirq, &isp[0]);
*/
errout:
	/* free up any allocated memory in the error case */
	kfree(isp);
	return (ret_val);
} /* ifx_ssc_init */


void
ifx_ssc_cleanup_module(void)
{
	int i;

	/* free up any allocated memory */
	for (i = 0; i < PORT_CNT; i++)
	{
		/* disable the SSC */
		WRITE_PERIPHERAL_REGISTER(IFX_SSC_WHBSTATE_CLR_ENABLE,isp[i].mapbase + IFX_SSC_WHBSTATE);
		/* free the interrupts */
		ifx_int_wrapper.free(isp[i].txirq, &isp[i]);
		ifx_int_wrapper.free(isp[i].rxirq, &isp[i]);
		ifx_int_wrapper.free(isp[i].errirq, &isp[i]);
/*
		ifx_int_wrapper.free(isp[i].frmirq, &isp[i]);

		if (isp[i].rxbuf != NULL)
			kfree(isp[i].rxbuf);
		if (isp[i].txbuf != NULL)
			kfree(isp[i].txbuf);
*/
	}
	kfree(isp);
	/* unregister the device */
	if (unregister_chrdev(maj, "ssc"))
	{
		printk("Unable to unregister major %d for the SSC\n", maj);
	}
	/* delete /proc read handler */
	remove_proc_entry("driver/ssc1", NULL);
	remove_proc_entry("driver/ssc2", NULL);
} /* ifx_ssc_cleanup_module */

module_exit(ifx_ssc_cleanup_module);

/* Module entry-points */
module_init(ifx_ssc_init);

#ifndef MODULE
static int __init
ifx_ssc_set_maj(char *str)
{
	maj = simple_strtol(str, NULL, 0);
	return 1;
}
__setup("ssc_maj=", ifx_ssc_set_maj);
#endif /* !MODULE */

#define AMAZON_SSC_EMSG(fmt,arg...) printk("%s: "fmt,__FUNCTION__, ##arg)
/* Brief:	chip select enable
 */
inline int amazon_ssc_cs_low(u32 pin)
{
	int ret=0;
	if ((ret=ifx_ssc_ioctl((struct inode *)0, NULL,IFX_SSC_GPO_OUT_CLR, (unsigned long)&pin))){
		AMAZON_SSC_EMSG("clear CS %d fails\n",pin);
	}
	wmb();
	return ret;
}
EXPORT_SYMBOL(amazon_ssc_cs_low);
/* Brief:	chip select disable
 */
inline int amazon_ssc_cs_high(u32 pin)
{
	int ret=0;
	if ((ret=ifx_ssc_ioctl((struct inode *)0, NULL,IFX_SSC_GPO_OUT_SET, (unsigned long)&pin))){
		AMAZON_SSC_EMSG("set CS %d fails\n", pin);
	}
	wmb();
	return ret;
}
EXPORT_SYMBOL(amazon_ssc_cs_high);
/* Brief:	one SSC session
 * Parameter:	
 *	tx_buf
 *	tx_len
 *	rx_buf
 *	rx_len
 *	session_mode: IFX_SSC_MODE_RXTX or IFX_SSC_MODE_TX
 * Return:	>=0 number of bytes received (if rx_buf != 0) or transmitted
 *		<0 error code
 * Description:
 *	0. copy data to internal buffer 
 *	1. Write command
 *	2a. If SSC_SESSION_MODE_TXONLY, read tx_len data
 *	2b. If not Read back (tx_len + rx_len) data
 *	3. copy internal buffer to rx buf if necessary
 */
static int ssc_session(char * tx_buf, u32 tx_len, char * rx_buf, u32 rx_len)
{
	int ret=0;

	char * ssc_tx_buf=NULL;
	char * ssc_rx_buf=NULL;

//	volatile char ssc_tx_buf[128]={0};
//	volatile char ssc_rx_buf[128]={0};

	int eff_size=0;
	u8 mode=0;
	
	if (tx_buf == NULL && tx_len ==0 && rx_buf == NULL && rx_len == 0){
		AMAZON_SSC_EMSG("invalid parameters\n");
		ret=-EINVAL;
		goto ssc_session_exit;
	}else if (tx_buf == NULL || tx_len == 0){
		if (rx_buf != NULL && rx_len != 0){
			mode = IFX_SSC_MODE_RX;
		}else{
			AMAZON_SSC_EMSG("invalid parameters\n");
			ret=-EINVAL;
			goto ssc_session_exit;
		}
	}else if (rx_buf == NULL || rx_len ==0){
		if (tx_buf != NULL && tx_len != 0){
			mode = IFX_SSC_MODE_TX;
		}else{
			AMAZON_SSC_EMSG("invalid parameters\n");
			ret=-EINVAL;
			goto ssc_session_exit;
		}
	}else{
		mode = IFX_SSC_MODE_RXTX;
	}
	
	if (mode == IFX_SSC_MODE_RXTX){
		eff_size = tx_len + rx_len;
	}else if (mode == IFX_SSC_MODE_RX){
		eff_size = rx_len;
	}else{
		eff_size = tx_len;
	}

	//4 bytes alignment,  required by driver
	/* change by TaiCheng */
	//if (in_irq()){
	if (1){
		ssc_tx_buf = (char*) kmalloc(sizeof(char) *  ((eff_size + 3) & (~3)), GFP_ATOMIC);
		ssc_rx_buf = (char*) kmalloc(sizeof(char) *  ((eff_size + 3) & (~3)), GFP_ATOMIC);
	}else{
		ssc_tx_buf = (char*) kmalloc(sizeof(char) *  ((eff_size + 3) & (~3)), GFP_KERNEL);
		ssc_rx_buf = (char*) kmalloc(sizeof(char) *  ((eff_size + 3) & (~3)), GFP_KERNEL);
	}
	if (ssc_tx_buf == NULL || ssc_rx_buf == NULL){
		AMAZON_SSC_EMSG("no memory for size of %d\n", eff_size);
		ret = -ENOMEM;
		goto ssc_session_exit;
	}
	memset((void*)ssc_tx_buf, 0, eff_size);
	memset((void*)ssc_rx_buf, 0, eff_size);

	if (tx_len>0){
		memcpy(ssc_tx_buf, tx_buf, tx_len);
	}
	
	ret=ifx_ssc_kwrite(0, ssc_tx_buf, eff_size);

	if (ret > 0) {
		ssc_tx_buf = NULL; //should be freed by ifx_ssc_kwrite
	}
		
	if (  ret != eff_size ){
		AMAZON_SSC_EMSG("ifx_ssc_write return %d\n",ret); 
		goto ssc_session_exit;
	}
	ret=ifx_ssc_kread(0, ssc_rx_buf,eff_size);
	if (  ret != eff_size ){
		AMAZON_SSC_EMSG("ifx_ssc_read return %d\n",ret); 
		goto ssc_session_exit;
	}
	
	memcpy(rx_buf, ssc_rx_buf+tx_len, rx_len);
	
	if (mode == IFX_SSC_MODE_TX) {
		ret = tx_len;
	}else{
		ret = rx_len;
	}
ssc_session_exit:

	if (ssc_tx_buf != NULL) kfree(ssc_tx_buf);
	if (ssc_rx_buf != NULL) kfree(ssc_rx_buf);

	if (ret<0) {
		printk("ssc session fails\n");
	}
	return ret;
}
/* Brief:	TX-RX session
 * Parameter:	
 *	tx_buf
 *	tx_len
 *	rx_buf
 *	rx_len
 * Return:	>=0 number of bytes received
 *		<0 error code
 * Description: 
 *	1. TX session
 *	2. RX session
 */
int amazon_ssc_txrx(char * tx_buf, u32 tx_len, char * rx_buf, u32 rx_len)
{
	return ssc_session(tx_buf,tx_len,rx_buf,rx_len);
}
EXPORT_SYMBOL(amazon_ssc_txrx);
/* Brief:	TX only session
 * Parameter:
 *	tx_buf
 *	tx_len
 * Return:	>=0 number of bytes transmitted
 *		<0 error code
 */
int amazon_ssc_tx(char * tx_buf, u32 tx_len)
{
	return ssc_session(tx_buf,tx_len,NULL,0);
}
EXPORT_SYMBOL(amazon_ssc_tx);
/* Brief:	RX only session
 * Parameter:
 *	rx_buf
 *	rx_len
 * Return:	>=0 number of bytes received
 *		<0 error code
 */
int amazon_ssc_rx(char * rx_buf, u32 rx_len)
{
	return ssc_session(NULL,0,rx_buf,rx_len);
}
EXPORT_SYMBOL(amazon_ssc_rx);

