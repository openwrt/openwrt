/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2006 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

// ### TO DO: general issues:
//            - power management
//            - interrupt handling (direct/indirect)
//            - pin/mux-handling (just overall concept due to project dependency)
//            - multiple instances capability
//            - slave functionality

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/ifxmips/ifx_ssc_defines.h>
#include <asm/ifxmips/ifx_ssc.h>

/* allow the user to set the major device number */
static int maj = 0;

/*
 * This is the per-channel data structure containing pointers, flags
 * and variables for the port. This driver supports a maximum of PORT_CNT.
 * isp is allocated in ifx_ssc_init() based on the chip version.
 */
static struct ifx_ssc_port *isp;

/* other forward declarations */
static unsigned int ifx_ssc_get_kernel_clk (struct ifx_ssc_port *info);
static void tx_int (struct ifx_ssc_port *);

extern unsigned int ifxmips_get_fpi_hz (void);
extern void ifxmips_mask_and_ack_irq (unsigned int irq_nr);

static inline unsigned int
ifx_ssc_get_kernel_clk (struct ifx_ssc_port *info)
{
	unsigned int rmc;

	rmc = (ifxmips_r32(IFXMIPS_SSC_CLC) & IFX_CLC_RUN_DIVIDER_MASK) >> IFX_CLC_RUN_DIVIDER_OFFSET;
	if (rmc == 0)
	{
		printk ("ifx_ssc_get_kernel_clk rmc==0 \n");
		return 0;
	}
	return ifxmips_get_fpi_hz () / rmc;
}

inline static void
rx_int (struct ifx_ssc_port *info)
{
	int fifo_fill_lev, bytes_in_buf, i;
	unsigned long tmp_val;
	unsigned long *tmp_ptr;
	unsigned int rx_valid_cnt;
	/* number of words waiting in the RX FIFO */
	fifo_fill_lev = (ifxmips_r32(IFXMIPS_SSC_FSTAT) & IFX_SSC_FSTAT_RECEIVED_WORDS_MASK) >> IFX_SSC_FSTAT_RECEIVED_WORDS_OFFSET;
	bytes_in_buf = info->rxbuf_end - info->rxbuf_ptr;
	// transfer with 32 bits per entry
	while ((bytes_in_buf >= 4) && (fifo_fill_lev > 0)) {
		tmp_ptr = (unsigned long *) info->rxbuf_ptr;
		*tmp_ptr = ifxmips_r32(IFXMIPS_SSC_RB);
		info->rxbuf_ptr += 4;
		info->stats.rxBytes += 4;
		fifo_fill_lev--;
		bytes_in_buf -= 4;
	}

	// now do the rest as mentioned in STATE.RXBV
	while ((bytes_in_buf > 0) && (fifo_fill_lev > 0)) {
		rx_valid_cnt = (ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_RX_BYTE_VALID_MASK) >> IFX_SSC_STATE_RX_BYTE_VALID_OFFSET;
		if (rx_valid_cnt == 0)
			break;

		if (rx_valid_cnt > bytes_in_buf)
			rx_valid_cnt = bytes_in_buf;

		tmp_val = ifxmips_r32(IFXMIPS_SSC_RB);

		for (i = 0; i < rx_valid_cnt; i++)
		{
			*info->rxbuf_ptr = (tmp_val >> (8 * (rx_valid_cnt - i - 1))) & 0xff;
			bytes_in_buf--;
			info->rxbuf_ptr++;
		}
		info->stats.rxBytes += rx_valid_cnt;
	}

	// check if transfer is complete
	if (info->rxbuf_ptr >= info->rxbuf_end)
	{
		disable_irq(IFXMIPS_SSC_RIR);
		wake_up_interruptible (&info->rwait);
	} else if ((info->opts.modeRxTx == IFX_SSC_MODE_RX) && (ifxmips_r32(IFXMIPS_SSC_RXCNT) == 0))
	{
		if (info->rxbuf_end - info->rxbuf_ptr < IFX_SSC_RXREQ_BLOCK_SIZE)
			ifxmips_w32((info->rxbuf_end - info->rxbuf_ptr) << IFX_SSC_RXREQ_RXCOUNT_OFFSET, IFXMIPS_SSC_RXREQ);
		else
			ifxmips_w32(IFX_SSC_RXREQ_BLOCK_SIZE << IFX_SSC_RXREQ_RXCOUNT_OFFSET,  IFXMIPS_SSC_RXREQ);
	}
}

inline static void
tx_int (struct ifx_ssc_port *info)
{

	int fifo_space, fill, i;
	fifo_space = ((ifxmips_r32(IFXMIPS_SSC_ID) & IFX_SSC_PERID_TXFS_MASK) >> IFX_SSC_PERID_TXFS_OFFSET)
		- ((ifxmips_r32(IFXMIPS_SSC_FSTAT) & IFX_SSC_FSTAT_TRANSMIT_WORDS_MASK) >> IFX_SSC_FSTAT_TRANSMIT_WORDS_OFFSET);

	if (fifo_space == 0)
		return;

	fill = info->txbuf_end - info->txbuf_ptr;

	if (fill > fifo_space * 4)
		fill = fifo_space * 4;

	for (i = 0; i < fill / 4; i++)
	{
		// at first 32 bit access
		ifxmips_w32(*(UINT32 *) info->txbuf_ptr, IFXMIPS_SSC_TB);
		info->txbuf_ptr += 4;
	}

	fifo_space -= fill / 4;
	info->stats.txBytes += fill & ~0x3;
	fill &= 0x3;
	if ((fifo_space > 0) & (fill > 1))
	{
		// trailing 16 bit access
		WRITE_PERIPHERAL_REGISTER_16 (*(UINT16 *) info->txbuf_ptr, info->mapbase + IFX_SSC_TB);
		info->txbuf_ptr += 2;
		info->stats.txBytes += 2;
		fifo_space--;
		fill -= 2;
	}

	if ((fifo_space > 0) & (fill > 0))
	{
		// trailing 8 bit access
		WRITE_PERIPHERAL_REGISTER_8 (*(UINT8 *) info->txbuf_ptr, info->mapbase + IFX_SSC_TB);
		info->txbuf_ptr++;
		info->stats.txBytes++;
	}

	// check if transmission complete
	if (info->txbuf_ptr >= info->txbuf_end)
	{
		disable_irq(IFXMIPS_SSC_TIR);
		kfree (info->txbuf);
		info->txbuf = NULL;
	}

}

irqreturn_t
ifx_ssc_rx_int (int irq, void *dev_id)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *) dev_id;
	rx_int (info);

	return IRQ_HANDLED;
}

irqreturn_t
ifx_ssc_tx_int (int irq, void *dev_id)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *) dev_id;
	tx_int (info);

	return IRQ_HANDLED;
}

irqreturn_t
ifx_ssc_err_int (int irq, void *dev_id)
{
	struct ifx_ssc_port *info = (struct ifx_ssc_port *) dev_id;
	unsigned int state;
	unsigned int write_back = 0;
	unsigned long flags;

	local_irq_save (flags);
	state = ifxmips_r32(IFXMIPS_SSC_STATE);

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

	if ((state & IFX_SSC_STATE_MODE_ERR) != 0) {
		info->stats.modeErr++;
		write_back |= IFX_SSC_WHBSTATE_CLR_MODE_ERROR;
	}

	if (write_back)
		ifxmips_w32(write_back, IFXMIPS_SSC_WHBSTATE);

	local_irq_restore (flags);

	return IRQ_HANDLED;
}

static void
ifx_ssc_abort (struct ifx_ssc_port *info)
{
	unsigned long flags;
	bool enabled;

	local_irq_save (flags);

	disable_irq(IFXMIPS_SSC_RIR);
	disable_irq(IFXMIPS_SSC_TIR);
	disable_irq(IFXMIPS_SSC_EIR);

	local_irq_restore (flags);

	// disable SSC (also aborts a receive request!)
	// ### TO DO: Perhaps it's better to abort after the receiption of a 
	// complete word. The disable cuts the transmission immediatly and 
	// releases the chip selects. This could result in unpredictable 
	// behavior of connected external devices!
	enabled = (ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_IS_ENABLED) != 0;
	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ENABLE, IFXMIPS_SSC_WHBSTATE);

	// flush fifos
	ifxmips_w32(IFX_SSC_XFCON_FIFO_FLUSH, IFXMIPS_SSC_TXFCON);
	ifxmips_w32(IFX_SSC_XFCON_FIFO_FLUSH, IFXMIPS_SSC_RXFCON);

	// free txbuf
	if (info->txbuf != NULL)
	{
		kfree (info->txbuf);
		info->txbuf = NULL;
	}

	// wakeup read process
	if (info->rxbuf != NULL)
		wake_up_interruptible (&info->rwait);

	// clear pending int's 
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_RIR);
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_TIR);
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_EIR);

	// clear error flags
	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ALL_ERROR, IFXMIPS_SSC_WHBSTATE);

	if (enabled)
		ifxmips_w32(IFX_SSC_WHBSTATE_SET_ENABLE, IFXMIPS_SSC_WHBSTATE);

}

/*
 * This routine is called whenever a port is opened.  It enforces
 * exclusive opening of a port and enables interrupts, etc.
 */
int
ifx_ssc_open (struct inode *inode, struct file *filp)
{
	struct ifx_ssc_port *info;
	int line;
	int from_kernel = 0;

	if ((inode == (struct inode *) 0) || (inode == (struct inode *) 1)) {
		from_kernel = 1;
		line = (int) inode;
	} else {
		line = MINOR (filp->f_dentry->d_inode->i_rdev);
	}

	/* don't open more minor devices than we can support */
	if (line < 0 || line >= PORT_CNT)
		return -ENXIO;

	info = &isp[line];

	/* exclusive open */
	if (info->port_is_open != 0)
		return -EBUSY;
	info->port_is_open++;

	disable_irq(IFXMIPS_SSC_RIR);
	disable_irq(IFXMIPS_SSC_TIR);
	disable_irq(IFXMIPS_SSC_EIR);

	/* Flush and enable TX/RX FIFO */
	ifxmips_w32((IFX_SSC_DEF_TXFIFO_FL << IFX_SSC_XFCON_ITL_OFFSET) | IFX_SSC_XFCON_FIFO_FLUSH | IFX_SSC_XFCON_FIFO_ENABLE, IFXMIPS_SSC_TXFCON);
	ifxmips_w32((IFX_SSC_DEF_RXFIFO_FL << IFX_SSC_XFCON_ITL_OFFSET) | IFX_SSC_XFCON_FIFO_FLUSH | IFX_SSC_XFCON_FIFO_ENABLE, IFXMIPS_SSC_RXFCON);

	/* logically flush the software FIFOs */
	info->rxbuf_ptr = 0;
	info->txbuf_ptr = 0;

	/* clear all error bits */
	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ALL_ERROR, IFXMIPS_SSC_WHBSTATE);

	// clear pending interrupts
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_RIR);
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_TIR);
	ifxmips_mask_and_ack_irq(IFXMIPS_SSC_EIR);

	ifxmips_w32(IFX_SSC_WHBSTATE_SET_ENABLE, IFXMIPS_SSC_WHBSTATE);

	return 0;
}
EXPORT_SYMBOL(ifx_ssc_open);

int
ifx_ssc_close (struct inode *inode, struct file *filp)
{
	struct ifx_ssc_port *info;
	int idx;

	if ((inode == (struct inode *) 0) || (inode == (struct inode *) 1))
		idx = (int) inode;
	else
		idx = MINOR (filp->f_dentry->d_inode->i_rdev);

	if (idx < 0 || idx >= PORT_CNT)
		return -ENXIO;

	info = &isp[idx];
	if (!info)
		return -ENXIO;

	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ENABLE, IFXMIPS_SSC_WHBSTATE);

	ifx_ssc_abort(info);

	info->port_is_open--;

	return 0;
}
EXPORT_SYMBOL(ifx_ssc_close);

static ssize_t
ifx_ssc_read_helper_poll (struct ifx_ssc_port *info, char *buf, size_t len, int from_kernel)
{
	ssize_t ret_val;
	unsigned long flags;

	if (info->opts.modeRxTx == IFX_SSC_MODE_TX)
		return -EFAULT;
	local_irq_save (flags);
	info->rxbuf_ptr = info->rxbuf;
	info->rxbuf_end = info->rxbuf + len;
	local_irq_restore (flags);
	/* Vinetic driver always works in IFX_SSC_MODE_RXTX */
	/* TXRX in poll mode */
	while (info->rxbuf_ptr < info->rxbuf_end)
	{
		if (info->txbuf_ptr < info->txbuf_end)
			tx_int (info);

		rx_int (info);
	};

	ret_val = info->rxbuf_ptr - info->rxbuf;

	return ret_val;
}

static ssize_t
ifx_ssc_read_helper (struct ifx_ssc_port *info, char *buf, size_t len, int from_kernel)
{
	ssize_t ret_val;
	unsigned long flags;
	DECLARE_WAITQUEUE (wait, current);

	if (info->opts.modeRxTx == IFX_SSC_MODE_TX)
		return -EFAULT;

	local_irq_save (flags);
	info->rxbuf_ptr = info->rxbuf;
	info->rxbuf_end = info->rxbuf + len;

	if (info->opts.modeRxTx == IFX_SSC_MODE_RXTX)
	{
		if ((info->txbuf == NULL) || (info->txbuf != info->txbuf_ptr) || (info->txbuf_end != len + info->txbuf))
		{
			local_irq_restore (flags);
			printk ("IFX SSC - %s: write must be called before calling " "read in combined RX/TX!\n", __func__);
			return -EFAULT;
		}

		local_irq_restore(flags);
		tx_int (info);

		if (info->txbuf_ptr < info->txbuf_end)
			enable_irq(IFXMIPS_SSC_TIR);

		enable_irq(IFXMIPS_SSC_RIR);
	} else {
		local_irq_restore(flags);
		if (ifxmips_r32(IFXMIPS_SSC_RXCNT) & IFX_SSC_RXCNT_TODO_MASK)
			return -EBUSY;
		enable_irq(IFXMIPS_SSC_RIR);
		if (len < IFX_SSC_RXREQ_BLOCK_SIZE)
			ifxmips_w32(len << IFX_SSC_RXREQ_RXCOUNT_OFFSET, IFXMIPS_SSC_RXREQ);
		else
			ifxmips_w32(IFX_SSC_RXREQ_BLOCK_SIZE << IFX_SSC_RXREQ_RXCOUNT_OFFSET, IFXMIPS_SSC_RXREQ);
	}

	__add_wait_queue (&info->rwait, &wait);
	set_current_state (TASK_INTERRUPTIBLE);

	do {
		local_irq_save (flags);
		if (info->rxbuf_ptr >= info->rxbuf_end)
			break;

		local_irq_restore (flags);

		if (signal_pending (current))
		{
			ret_val = -ERESTARTSYS;
			goto out;
		}
		schedule();
	} while (1);

	ret_val = info->rxbuf_ptr - info->rxbuf;
	local_irq_restore (flags);

out:
	current->state = TASK_RUNNING;
	__remove_wait_queue (&info->rwait, &wait);

	return (ret_val);
}

static ssize_t
ifx_ssc_write_helper (struct ifx_ssc_port *info, const char *buf,
		      size_t len, int from_kernel)
{
	if (info->opts.modeRxTx == IFX_SSC_MODE_RX)
		return -EFAULT;

	info->txbuf_ptr = info->txbuf;
	info->txbuf_end = len + info->txbuf;
	if (info->opts.modeRxTx == IFX_SSC_MODE_TX)
	{
		tx_int (info);
		if (info->txbuf_ptr < info->txbuf_end)
		{
			enable_irq(IFXMIPS_SSC_TIR);
		}
	}

	return len;
}

ssize_t
ifx_ssc_kread (int port, char *kbuf, size_t len)
{
	struct ifx_ssc_port *info;
	ssize_t ret_val;

	if (port < 0 || port >= PORT_CNT)
		return -ENXIO;

	if (len == 0)
		return 0;

	info = &isp[port];

	if (info->rxbuf != NULL)
	{
		printk ("SSC device busy\n");
		return -EBUSY;
	}

	info->rxbuf = kbuf;
	if (info->rxbuf == NULL)
	{
		printk ("SSC device error\n");
		return -EINVAL;
	}

	ret_val = ifx_ssc_read_helper_poll (info, kbuf, len, 1);
	info->rxbuf = NULL;

	disable_irq(IFXMIPS_SSC_RIR);

	return ret_val;
}
EXPORT_SYMBOL(ifx_ssc_kread);

ssize_t
ifx_ssc_kwrite (int port, const char *kbuf, size_t len)
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

	info->txbuf = (char *) kbuf;

	ret_val = ifx_ssc_write_helper (info, info->txbuf, len, 1);

	if (ret_val < 0)
		info->txbuf = NULL;

	return ret_val;
}
EXPORT_SYMBOL(ifx_ssc_kwrite);

static ssize_t
ifx_ssc_read (struct file *filp, char *ubuf, size_t len, loff_t * off)
{
	ssize_t ret_val;
	int idx;
	struct ifx_ssc_port *info;

	idx = MINOR (filp->f_dentry->d_inode->i_rdev);
	info = &isp[idx];

	if (info->rxbuf != NULL)
		return -EBUSY;

	info->rxbuf = kmalloc (len + 3, GFP_KERNEL);
	if (info->rxbuf == NULL)
		return -ENOMEM;

	ret_val = ifx_ssc_read_helper (info, info->rxbuf, len, 0);
	if (copy_to_user ((void *) ubuf, info->rxbuf, ret_val) != 0)
		ret_val = -EFAULT;

	disable_irq(IFXMIPS_SSC_RIR);

	kfree (info->rxbuf);
	info->rxbuf = NULL;

	return (ret_val);
}

static ssize_t
ifx_ssc_write (struct file *filp, const char *ubuf, size_t len, loff_t * off)
{
	int idx;
	struct ifx_ssc_port *info;
	int ret_val;

	if (len == 0)
		return (0);

	idx = MINOR (filp->f_dentry->d_inode->i_rdev);
	info = &isp[idx];

	if (info->txbuf != NULL)
		return -EBUSY;

	info->txbuf = kmalloc (len + 3, GFP_KERNEL);
	if (info->txbuf == NULL)
		return -ENOMEM;

	ret_val = copy_from_user (info->txbuf, ubuf, len);
	if (ret_val == 0)
		ret_val = ifx_ssc_write_helper (info, info->txbuf, len, 0);
	else
		ret_val = -EFAULT;

	if (ret_val < 0)
	{
		kfree (info->txbuf);
		info->txbuf = NULL;
	}

	return (ret_val);
}

static struct ifx_ssc_frm_status *
ifx_ssc_frm_status_get (struct ifx_ssc_port *info)
{
	unsigned long tmp;

	tmp = ifxmips_r32(IFXMIPS_SSC_SFSTAT);
	info->frm_status.DataBusy = (tmp & IFX_SSC_SFSTAT_IN_DATA) > 0;
	info->frm_status.PauseBusy = (tmp & IFX_SSC_SFSTAT_IN_PAUSE) > 0;
	info->frm_status.DataCount = (tmp & IFX_SSC_SFSTAT_DATA_COUNT_MASK) >> IFX_SSC_SFSTAT_DATA_COUNT_OFFSET;
	info->frm_status.PauseCount = (tmp & IFX_SSC_SFSTAT_PAUSE_COUNT_MASK) >> IFX_SSC_SFSTAT_PAUSE_COUNT_OFFSET;
	tmp = ifxmips_r32(IFXMIPS_SSC_SFCON);
	info->frm_status.EnIntAfterData = (tmp & IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE) > 0;
	info->frm_status.EnIntAfterPause = (tmp & IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE) > 0;

	return &info->frm_status;
}


static struct ifx_ssc_frm_opts *
ifx_ssc_frm_control_get (struct ifx_ssc_port *info)
{
	unsigned long tmp;

	tmp = ifxmips_r32(IFXMIPS_SSC_SFCON);
	info->frm_opts.FrameEnable = (tmp & IFX_SSC_SFCON_SF_ENABLE) > 0;
	info->frm_opts.DataLength = (tmp & IFX_SSC_SFCON_DATA_LENGTH_MASK) >> IFX_SSC_SFCON_DATA_LENGTH_OFFSET;
	info->frm_opts.PauseLength = (tmp & IFX_SSC_SFCON_PAUSE_LENGTH_MASK) >> IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET;
	info->frm_opts.IdleData = (tmp & IFX_SSC_SFCON_PAUSE_DATA_MASK) >> IFX_SSC_SFCON_PAUSE_DATA_OFFSET;
	info->frm_opts.IdleClock = (tmp & IFX_SSC_SFCON_PAUSE_CLOCK_MASK) >> IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET;
	info->frm_opts.StopAfterPause = (tmp & IFX_SSC_SFCON_STOP_AFTER_PAUSE) > 0;

	return &info->frm_opts;
}

static int
ifx_ssc_frm_control_set (struct ifx_ssc_port *info)
{
	unsigned long tmp;

	if ((info->frm_opts.DataLength > IFX_SSC_SFCON_DATA_LENGTH_MAX)
	    || (info->frm_opts.DataLength < 1)
	    || (info->frm_opts.PauseLength > IFX_SSC_SFCON_PAUSE_LENGTH_MAX)
	    || (info->frm_opts.PauseLength < 1)
	    || (info->frm_opts.IdleData & ~(IFX_SSC_SFCON_PAUSE_DATA_MASK >> IFX_SSC_SFCON_PAUSE_DATA_OFFSET))
	    || (info->frm_opts.IdleClock & ~(IFX_SSC_SFCON_PAUSE_CLOCK_MASK >> IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET)))
		return -EINVAL;

	// read interrupt bits (they're not changed here)
	tmp = ifxmips_r32(IFXMIPS_SSC_SFCON) &
		(IFX_SSC_SFCON_FIR_ENABLE_BEFORE_PAUSE | IFX_SSC_SFCON_FIR_ENABLE_AFTER_PAUSE);

	// set all values with respect to it's bit position (for data and pause 
	// length set N-1)
	tmp = (info->frm_opts.DataLength - 1) << IFX_SSC_SFCON_DATA_LENGTH_OFFSET;
	tmp |= (info->frm_opts.PauseLength - 1) << IFX_SSC_SFCON_PAUSE_LENGTH_OFFSET;
	tmp |= info->frm_opts.IdleData << IFX_SSC_SFCON_PAUSE_DATA_OFFSET;
	tmp |= info->frm_opts.IdleClock << IFX_SSC_SFCON_PAUSE_CLOCK_OFFSET;
	tmp |= info->frm_opts.FrameEnable * IFX_SSC_SFCON_SF_ENABLE;
	tmp |= info->frm_opts.StopAfterPause * IFX_SSC_SFCON_STOP_AFTER_PAUSE;

	ifxmips_w32(tmp, IFXMIPS_SSC_SFCON);

	return 0;
}

static int
ifx_ssc_rxtx_mode_set (struct ifx_ssc_port *info, unsigned int val)
{
	unsigned long tmp;

	if (!(info) || (val & ~(IFX_SSC_MODE_MASK)))
		return -EINVAL;

	if ((ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_BUSY)
	    || (ifxmips_r32(IFXMIPS_SSC_RXCNT) & IFX_SSC_RXCNT_TODO_MASK))
		return -EBUSY;

	tmp = (ifxmips_r32(IFXMIPS_SSC_CON) & ~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF)) | (val);
	ifxmips_w32(tmp, IFXMIPS_SSC_SFCON);
	info->opts.modeRxTx = val;

	return 0;
}

static int
ifx_ssc_sethwopts (struct ifx_ssc_port *info)
{
	unsigned long flags, bits;
	struct ifx_ssc_hwopts *opts = &info->opts;

	if ((opts->dataWidth < IFX_SSC_MIN_DATA_WIDTH)
	    || (opts->dataWidth > IFX_SSC_MAX_DATA_WIDTH))
		return -EINVAL;

	bits = (opts->dataWidth - 1) << IFX_SSC_CON_DATA_WIDTH_OFFSET;
	bits |= IFX_SSC_CON_ENABLE_BYTE_VALID;

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

	switch (opts->modeRxTx)
	{
	case IFX_SSC_MODE_TX:
		bits |= IFX_SSC_CON_RX_OFF;
		break;
	case IFX_SSC_MODE_RX:
		bits |= IFX_SSC_CON_TX_OFF;
		break;
	}

	local_irq_save (flags);

	ifxmips_w32(bits, IFXMIPS_SSC_CON);
	ifxmips_w32((info->opts.gpoCs << IFX_SSC_GPOCON_ISCSB0_POS) |
				   (info->opts.gpoInv << IFX_SSC_GPOCON_INVOUT0_POS), IFXMIPS_SSC_GPOCON);

	ifxmips_w32(info->opts.gpoCs << IFX_SSC_WHBGPOSTAT_SETOUT0_POS, IFXMIPS_SSC_WHBGPOSTAT);

	//master mode
	if (opts->masterSelect)
		ifxmips_w32(IFX_SSC_WHBSTATE_SET_MASTER_SELECT, IFXMIPS_SSC_WHBSTATE);
	else
		ifxmips_w32(IFX_SSC_WHBSTATE_CLR_MASTER_SELECT, IFXMIPS_SSC_WHBSTATE);

	// init serial framing
	ifxmips_w32(0, IFXMIPS_SSC_SFCON);
	/* set up the port pins */
	//check for general requirements to switch (external) pad/pin characteristics
	/* TODO: P0.9 SPI_CS4, P0.10 SPI_CS5, P 0.11 SPI_CS6, because of ASC0 */
	/* p0.15 SPI_CS1(EEPROM), P0.13 SPI_CS3, */
	/* Set p0.15 to alternative 01, others to 00 (In/OUT) */
	*(IFXMIPS_GPIO_P0_DIR) = (*IFXMIPS_GPIO_P0_DIR) | (0xA000);
	*(IFXMIPS_GPIO_P0_ALTSEL0) = (((*IFXMIPS_GPIO_P0_ALTSEL0) | (0x8000)) & (~(0x2000)));
	*(IFXMIPS_GPIO_P0_ALTSEL1) = (((*IFXMIPS_GPIO_P0_ALTSEL1) & (~0x8000)) & (~(0x2000)));
	*(IFXMIPS_GPIO_P0_OD) = (*IFXMIPS_GPIO_P0_OD) | 0xA000;

	/* p1.6 SPI_CS2(SFLASH), p1.0 SPI_DIN, p1.1 SPI_DOUT, p1.2 SPI_CLK */
	*(IFXMIPS_GPIO_P1_DIR) = ((*IFXMIPS_GPIO_P1_DIR) | (0x46)) & (~1);
	*(IFXMIPS_GPIO_P1_ALTSEL0) = ((*IFXMIPS_GPIO_P1_ALTSEL0) | (0x47));
	*(IFXMIPS_GPIO_P1_ALTSEL1) = (*IFXMIPS_GPIO_P1_ALTSEL1) & (~0x47);
	*(IFXMIPS_GPIO_P1_OD) = (*IFXMIPS_GPIO_P1_OD) | 0x0046;

	/*CS3 */
	/*TODO: CS4 CS5 CS6 */
	*IFXMIPS_GPIO_P0_OUT = ((*IFXMIPS_GPIO_P0_OUT) | 0x2000);

	local_irq_restore (flags);

	return 0;
}

static int
ifx_ssc_set_baud (struct ifx_ssc_port *info, unsigned int baud)
{
	unsigned int ifx_ssc_clock;
	unsigned int br;
	unsigned long flags;
	bool enabled;
	int retval = 0;

	ifx_ssc_clock = ifx_ssc_get_kernel_clk(info);
	if (ifx_ssc_clock == 0)
	{
		retval = -EINVAL;
		goto out;
	}

	local_irq_save (flags);

	enabled = (ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_IS_ENABLED);
	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ENABLE, IFXMIPS_SSC_WHBSTATE);

	br = (((ifx_ssc_clock >> 1) + baud / 2) / baud) - 1;
	wmb();

	if (br > 0xffff || ((br == 0) &&
			((ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_IS_MASTER) == 0))) {
		local_irq_restore (flags);
		printk ("%s: invalid baudrate %u\n", __func__, baud);
		return -EINVAL;
	}

	ifxmips_w32(br, IFXMIPS_SSC_BR);

	if (enabled)
		ifxmips_w32(IFX_SSC_WHBSTATE_SET_ENABLE, IFXMIPS_SSC_WHBSTATE);

	local_irq_restore(flags);

out:
	return retval;
}

static int
ifx_ssc_hwinit (struct ifx_ssc_port *info)
{
	unsigned long flags;
	bool enabled;

	enabled = (ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_IS_ENABLED);
	ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ENABLE, IFXMIPS_SSC_WHBSTATE);

	if (ifx_ssc_sethwopts (info) < 0)
	{
		printk ("%s: setting the hardware options failed\n", __func__);
		return -EINVAL;
	}

	if (ifx_ssc_set_baud (info, info->baud) < 0)
	{
		printk ("%s: setting the baud rate failed\n", __func__);
		return -EINVAL;
	}

	local_irq_save (flags);

	/* TX FIFO */
	ifxmips_w32((IFX_SSC_DEF_TXFIFO_FL << IFX_SSC_XFCON_ITL_OFFSET) | IFX_SSC_XFCON_FIFO_ENABLE, IFXMIPS_SSC_TXFCON);
	/* RX FIFO */
	ifxmips_w32((IFX_SSC_DEF_RXFIFO_FL << IFX_SSC_XFCON_ITL_OFFSET) | IFX_SSC_XFCON_FIFO_ENABLE, IFXMIPS_SSC_RXFCON);

	local_irq_restore (flags);

	if (enabled)
		ifxmips_w32(IFX_SSC_WHBSTATE_SET_ENABLE, IFXMIPS_SSC_WHBSTATE);

	return 0;
}

int
ifx_ssc_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long data)
{
	struct ifx_ssc_port *info;
	int line, ret_val = 0;
	unsigned long flags;
	unsigned long tmp;
	int from_kernel = 0;

	if ((inode == (struct inode *) 0) || (inode == (struct inode *) 1))
	{
		from_kernel = 1;
		line = (int) inode;
	} else {
		line = MINOR (filp->f_dentry->d_inode->i_rdev);
	}

	if (line < 0 || line >= PORT_CNT)
		return -ENXIO;

	info = &isp[line];

	switch (cmd)
	{
	case IFX_SSC_STATS_READ:
		/* data must be a pointer to a struct ifx_ssc_statistics */
		if (from_kernel)
			memcpy ((void *) data, (void *) &info->stats,
				sizeof (struct ifx_ssc_statistics));
		else if (copy_to_user ((void *) data,
				       (void *) &info->stats,
				       sizeof (struct ifx_ssc_statistics)))
			ret_val = -EFAULT;
		break;
	case IFX_SSC_STATS_RESET:
		/* just resets the statistics counters */
		memset ((void *) &info->stats, 0,
			sizeof (struct ifx_ssc_statistics));
		break;
	case IFX_SSC_BAUD_SET:
		/* if the buffers are not empty then the port is */
		/* busy and we shouldn't change things on-the-fly! */
		if (!info->txbuf || !info->rxbuf ||
		    (ifxmips_r32(IFXMIPS_SSC_STATE) & IFX_SSC_STATE_BUSY)) {
			ret_val = -EBUSY;
			break;
		}
		/* misuse flags */
		if (from_kernel)
			flags = *((unsigned long *) data);
		else if (copy_from_user ((void *) &flags,
					 (void *) data, sizeof (flags))) {
			ret_val = -EFAULT;
			break;
		}
		if (flags == 0) {
			ret_val = -EINVAL;
			break;
		}
		if (ifx_ssc_set_baud (info, flags) < 0) {
			ret_val = -EINVAL;
			break;
		}
		info->baud = flags;
		break;
	case IFX_SSC_BAUD_GET:
		if (from_kernel)
			*((unsigned int *) data) = info->baud;
		else if (copy_to_user ((void *) data,
				       (void *) &info->baud,
				       sizeof (unsigned long)))
			ret_val = -EFAULT;
		break;
	case IFX_SSC_RXTX_MODE_SET:
		if (from_kernel)
			tmp = *((unsigned long *) data);
		else if (copy_from_user ((void *) &tmp,
					 (void *) data, sizeof (tmp))) {
			ret_val = -EFAULT;
			break;
		}
		ret_val = ifx_ssc_rxtx_mode_set (info, tmp);
		break;
	case IFX_SSC_RXTX_MODE_GET:
		tmp = ifxmips_r32(IFXMIPS_SSC_CON) &
			(~(IFX_SSC_CON_RX_OFF | IFX_SSC_CON_TX_OFF));
		if (from_kernel)
			*((unsigned int *) data) = tmp;
		else if (copy_to_user ((void *) data,
				       (void *) &tmp, sizeof (tmp)))
			ret_val = -EFAULT;
		break;

	case IFX_SSC_ABORT:
		ifx_ssc_abort (info);
		break;

	case IFX_SSC_GPO_OUT_SET:
		if (from_kernel)
			tmp = *((unsigned long *) data);
		else if (copy_from_user ((void *) &tmp,
					 (void *) data, sizeof (tmp))) {
			ret_val = -EFAULT;
			break;
		}
		if (tmp > IFX_SSC_MAX_GPO_OUT)
			ret_val = -EINVAL;
		else
			ifxmips_w32(1 << (tmp + IFX_SSC_WHBGPOSTAT_SETOUT0_POS),
				 IFXMIPS_SSC_WHBGPOSTAT);
		break;
	case IFX_SSC_GPO_OUT_CLR:
		if (from_kernel)
			tmp = *((unsigned long *) data);
		else if (copy_from_user ((void *) &tmp, (void *) data, sizeof (tmp))) {
			ret_val = -EFAULT;
			break;
		}
		if (tmp > IFX_SSC_MAX_GPO_OUT)
			ret_val = -EINVAL;
		else {
			ifxmips_w32(1 << (tmp + IFX_SSC_WHBGPOSTAT_CLROUT0_POS),
				 IFXMIPS_SSC_WHBGPOSTAT);
		}
		break;
	case IFX_SSC_GPO_OUT_GET:
		tmp = ifxmips_r32(IFXMIPS_SSC_GPOSTAT);
		if (from_kernel)
			*((unsigned int *) data) = tmp;
		else if (copy_to_user ((void *) data,
				       (void *) &tmp, sizeof (tmp)))
			ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_STATUS_GET:
		ifx_ssc_frm_status_get (info);
		if (from_kernel)
			memcpy ((void *) data, (void *) &info->frm_status,
				sizeof (struct ifx_ssc_frm_status));
		else if (copy_to_user ((void *) data,
				       (void *) &info->frm_status,
				       sizeof (struct ifx_ssc_frm_status)))
			ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_CONTROL_GET:
		ifx_ssc_frm_control_get (info);
		if (from_kernel)
			memcpy ((void *) data, (void *) &info->frm_opts,
				sizeof (struct ifx_ssc_frm_opts));
		else if (copy_to_user ((void *) data,
				       (void *) &info->frm_opts,
				       sizeof (struct ifx_ssc_frm_opts)))
			ret_val = -EFAULT;
		break;
	case IFX_SSC_FRM_CONTROL_SET:
		if (from_kernel)
			memcpy ((void *) &info->frm_opts, (void *) data,
				sizeof (struct ifx_ssc_frm_opts));
		else if (copy_to_user ((void *) &info->frm_opts,
				       (void *) data,
				       sizeof (struct ifx_ssc_frm_opts))) {
			ret_val = -EFAULT;
			break;
		}
		ret_val = ifx_ssc_frm_control_set (info);
		break;
	case IFX_SSC_HWOPTS_SET:
		/* data must be a pointer to a struct ifx_ssc_hwopts */
		/* if the buffers are not empty then the port is */
		/* busy and we shouldn't change things on-the-fly! */
		if (!info->txbuf || !info->rxbuf ||
		    (ifxmips_r32(IFXMIPS_SSC_STATE)
		     & IFX_SSC_STATE_BUSY)) {
			ret_val = -EBUSY;
			break;
		}
		if (from_kernel)
			memcpy ((void *) &info->opts, (void *) data,
				sizeof (struct ifx_ssc_hwopts));
		else if (copy_from_user ((void *) &info->opts,
					 (void *) data, sizeof(struct ifx_ssc_hwopts))) {
			ret_val = -EFAULT;
			break;
		}
		if (ifx_ssc_hwinit (info) < 0) {
			ret_val = -EIO;
		}
		break;
	case IFX_SSC_HWOPTS_GET:
		/* data must be a pointer to a struct ifx_ssc_hwopts */
		if (from_kernel)
			memcpy ((void *) data, (void *) &info->opts,
				sizeof (struct ifx_ssc_hwopts));
		else if (copy_to_user ((void *) data,
				       (void *) &info->opts,
				       sizeof (struct ifx_ssc_hwopts)))
			ret_val = -EFAULT;
		break;
	default:
		ret_val = -ENOIOCTLCMD;
	}

	return ret_val;
}
EXPORT_SYMBOL(ifx_ssc_ioctl);

static struct file_operations ifx_ssc_fops = {
      .owner = THIS_MODULE,
      .read = ifx_ssc_read,
      .write = ifx_ssc_write,
      .ioctl = ifx_ssc_ioctl,
      .open = ifx_ssc_open,
      .release = ifx_ssc_close,
};

int __init
ifx_ssc_init (void)
{
	struct ifx_ssc_port *info;
	int i, nbytes;
	unsigned long flags;
	int ret_val;

	ret_val = -ENOMEM;
	nbytes = PORT_CNT * sizeof(struct ifx_ssc_port);
	isp = (struct ifx_ssc_port*)kmalloc(nbytes, GFP_KERNEL);

	if (isp == NULL)
	{
		printk("%s: no memory for isp\n", __func__);
		return (ret_val);
	}
	memset(isp, 0, nbytes);

	ret_val = -ENXIO;
	if ((i = register_chrdev (maj, "ssc", &ifx_ssc_fops)) < 0)
	{
		printk ("Unable to register major %d for the Infineon SSC\n", maj);
		if (maj == 0)
		{
			goto errout;
		} else {
			maj = 0;
			if ((i = register_chrdev (maj, "ssc", &ifx_ssc_fops)) < 0)
			{
				printk ("Unable to register major %d for the Infineon SSC\n", maj);
				goto errout;
			}
		}
	}

	if (maj == 0)
		maj = i;

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
			info->mapbase = IFXMIPS_SSC_BASE_ADDR;
		}

		ifxmips_w32(IFX_SSC_DEF_RMC << IFX_CLC_RUN_DIVIDER_OFFSET, IFXMIPS_SSC_CLC);

		init_waitqueue_head (&info->rwait);

		local_irq_save (flags);

		// init serial framing register
		ifxmips_w32(IFX_SSC_DEF_SFCON, IFXMIPS_SSC_SFCON);

		ret_val = request_irq(IFXMIPS_SSC_TIR, ifx_ssc_tx_int, IRQF_DISABLED, "ifx_ssc_tx", info);
		if (ret_val)
		{
			printk("%s: unable to get irq %d\n", __func__, IFXMIPS_SSC_TIR);
			local_irq_restore(flags);
			goto errout;
		}

		ret_val = request_irq(IFXMIPS_SSC_RIR, ifx_ssc_rx_int, IRQF_DISABLED, "ifx_ssc_rx", info);
		if (ret_val)
		{
			printk ("%s: unable to get irq %d\n", __func__, IFXMIPS_SSC_RIR);
			local_irq_restore (flags);
			goto irqerr;
		}

		ret_val = request_irq(IFXMIPS_SSC_EIR, ifx_ssc_err_int, IRQF_DISABLED, "ifx_ssc_err", info);
		if (ret_val)
		{
			printk ("%s: unable to get irq %d\n", __func__, IFXMIPS_SSC_EIR);
			local_irq_restore (flags);
			goto irqerr;
		}
		ifxmips_w32(IFX_SSC_DEF_IRNEN, IFXMIPS_SSC_IRN);

		//enable_irq(IFXMIPS_SSC_TIR);
		//enable_irq(IFXMIPS_SSC_RIR);
		//enable_irq(IFXMIPS_SSC_EIR);

		local_irq_restore (flags);
	}

	for (i = 0; i < PORT_CNT; i++) {
		info = &isp[i];
		if (ifx_ssc_hwinit (info) < 0)
		{
			printk ("%s: hardware init failed for port %d\n", __func__, i);
			goto irqerr;
		}
	}


	return 0;

irqerr:
	free_irq(IFXMIPS_SSC_TIR, &isp[0]);
	free_irq(IFXMIPS_SSC_RIR, &isp[0]);
	free_irq(IFXMIPS_SSC_EIR, &isp[0]);
errout:
	kfree (isp);
	return (ret_val);
}

void
ifx_ssc_cleanup_module (void)
{
	int i;

	for (i = 0; i < PORT_CNT; i++) {
		ifxmips_w32(IFX_SSC_WHBSTATE_CLR_ENABLE, IFXMIPS_SSC_WHBSTATE);
		free_irq(IFXMIPS_SSC_TIR, &isp[i]);
		free_irq(IFXMIPS_SSC_RIR, &isp[i]);
		free_irq(IFXMIPS_SSC_EIR, &isp[i]);
	}
	kfree (isp);
}

module_init(ifx_ssc_init);
module_exit(ifx_ssc_cleanup_module);


inline int
ifx_ssc_cs_low (u32 pin)
{
	int ret = 0;
	if ((ret = ifx_ssc_ioctl ((struct inode *) 0, NULL, IFX_SSC_GPO_OUT_CLR, (unsigned long) &pin)))
		printk ("clear CS %d fails\n", pin);
	wmb ();

	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_low);

inline int
ifx_ssc_cs_high (u32 pin)
{
	int ret = 0;
	if ((ret = ifx_ssc_ioctl((struct inode *) 0, NULL, IFX_SSC_GPO_OUT_SET, (unsigned long) &pin)))
		printk ("set CS %d fails\n", pin);
	wmb ();

	return ret;
}
EXPORT_SYMBOL(ifx_ssc_cs_high);

static int
ssc_session (char *tx_buf, u32 tx_len, char *rx_buf, u32 rx_len)
{
	int ret = 0;

	char *ssc_tx_buf = NULL;
	char *ssc_rx_buf = NULL;
	int eff_size = 0;
	u8 mode = 0;

	if (tx_buf == NULL && tx_len == 0 && rx_buf == NULL && rx_len == 0) {
		printk ("invalid parameters\n");
		ret = -EINVAL;
		goto ssc_session_exit;
	}
	else if (tx_buf == NULL || tx_len == 0) {
		if (rx_buf != NULL && rx_len != 0) {
			mode = IFX_SSC_MODE_RX;
		}
		else {
			printk ("invalid parameters\n");
			ret = -EINVAL;
			goto ssc_session_exit;
		}
	}
	else if (rx_buf == NULL || rx_len == 0) {
		if (tx_buf != NULL && tx_len != 0) {
			mode = IFX_SSC_MODE_TX;
		}
		else {
			printk ("invalid parameters\n");
			ret = -EINVAL;
			goto ssc_session_exit;
		}
	}
	else {
		mode = IFX_SSC_MODE_RXTX;
	}

	if (mode == IFX_SSC_MODE_RXTX) {
		eff_size = tx_len + rx_len;
	}
	else if (mode == IFX_SSC_MODE_RX) {
		eff_size = rx_len;
	}
	else {
		eff_size = tx_len;
	}

	//4 bytes alignment,  required by driver
	/* change by TaiCheng */
	//if (in_irq()){
	if (1) {
		ssc_tx_buf =
			(char *) kmalloc (sizeof (char) *
					  ((eff_size + 3) & (~3)),
					  GFP_ATOMIC);
		ssc_rx_buf =
			(char *) kmalloc (sizeof (char) *
					  ((eff_size + 3) & (~3)),
					  GFP_ATOMIC);
	}
	else {
		ssc_tx_buf =
			(char *) kmalloc (sizeof (char) *
					  ((eff_size + 3) & (~3)),
					  GFP_KERNEL);
		ssc_rx_buf =
			(char *) kmalloc (sizeof (char) *
					  ((eff_size + 3) & (~3)),
					  GFP_KERNEL);
	}
	if (ssc_tx_buf == NULL || ssc_rx_buf == NULL) {
		printk ("no memory for size of %d\n", eff_size);
		ret = -ENOMEM;
		goto ssc_session_exit;
	}
	memset ((void *) ssc_tx_buf, 0, eff_size);
	memset ((void *) ssc_rx_buf, 0, eff_size);

	if (tx_len > 0) {
		memcpy (ssc_tx_buf, tx_buf, tx_len);
	}

	ret = ifx_ssc_kwrite (0, ssc_tx_buf, eff_size);

	if (ret > 0) {
		ssc_tx_buf = NULL;	//should be freed by ifx_ssc_kwrite
	}

	if (ret != eff_size) {
		printk ("ifx_ssc_write return %d\n", ret);
		goto ssc_session_exit;
	}
	ret = ifx_ssc_kread (0, ssc_rx_buf, eff_size);
	if (ret != eff_size) {
		printk ("ifx_ssc_read return %d\n", ret);
		goto ssc_session_exit;
	}

	memcpy (rx_buf, ssc_rx_buf + tx_len, rx_len);

	if (mode == IFX_SSC_MODE_TX) {
		ret = tx_len;
	}
	else {
		ret = rx_len;
	}
      ssc_session_exit:

	if (ssc_tx_buf != NULL)
		kfree (ssc_tx_buf);
	if (ssc_rx_buf != NULL)
		kfree (ssc_rx_buf);

	if (ret < 0) {
		printk ("ssc session fails\n");
	}
	return ret;
}

int
ifx_ssc_txrx (char *tx_buf, u32 tx_len, char *rx_buf, u32 rx_len)
{
	return ssc_session(tx_buf, tx_len, rx_buf, rx_len);
}
EXPORT_SYMBOL(ifx_ssc_txrx);

int
ifx_ssc_tx (char *tx_buf, u32 tx_len)
{
	return ssc_session(tx_buf, tx_len, NULL, 0);
}
EXPORT_SYMBOL(ifx_ssc_tx);

int
ifx_ssc_rx (char *rx_buf, u32 rx_len)
{
	return ssc_session(NULL, 0, rx_buf, rx_len);
}
EXPORT_SYMBOL(ifx_ssc_rx);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("ifxmips ssc driver");

