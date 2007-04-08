/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/

/* Description: Serial port driver for the BCM963XX. */

#define CARDNAME    "bcm963xx_serial driver"
#define VERSION     "2.0"
#define VER_STR     CARDNAME " v" VERSION "\n"


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h> 
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

/* for definition of struct console */
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/serialP.h>
#include <asm/uaccess.h>

#include <bcmtypes.h>
#include <board.h>
#include <bcm_map_part.h>
#include <bcm_intr.h>
#include <6348_map_part.h>
#include <6348_intr.h>

static DEFINE_SPINLOCK(bcm963xx_serial_lock);

extern void _putc(char);
extern void _puts(const char *);

typedef struct bcm_serial {
    volatile Uart *         port;
    int                     type;
    int                     flags; 
    int                     irq;
    int                     baud_base;
    int                     blocked_open;
    unsigned short          close_delay;
    unsigned short          closing_wait;
    unsigned short          line;                /* port/line number */
    unsigned short          cflags;              /* line configuration flag */
    unsigned short          x_char;              /* xon/xoff character */
    unsigned short          read_status_mask;    /* mask for read condition */
    unsigned short          ignore_status_mask;  /* mask for ignore condition */
    unsigned long           event;               /* mask used in BH */
    int                     xmit_head;           /* Position of the head */
    int                     xmit_tail;           /* Position of the tail */
    int                     xmit_cnt;            /* Count of the chars in the buffer */
    int                     count;               /* indicates how many times it has been opened */
    int                     magic;

    struct async_icount     icount;              /* keep track of things ... */
    struct tty_struct       *tty;                /* tty associated */    
    struct termios          normal_termios;

    wait_queue_head_t       open_wait;
    wait_queue_head_t       close_wait;

    long                    session;             /* Session of opening process */
    long                    pgrp;                /* pgrp of opening process */

    unsigned char           is_initialized;
} Context;


/*---------------------------------------------------------------------*/
/* Define bits in the Interrupt Enable register                        */
/*---------------------------------------------------------------------*/
/* Enable receive interrupt              */
#define RXINT   (RXFIFONE|RXOVFERR)

/* Enable transmit interrupt             */
#define TXINT    (TXFIFOEMT|TXUNDERR|TXOVFERR) 

/* Enable receiver line status interrupt */
#define LSINT    (RXBRK|RXPARERR|RXFRAMERR)

#define BCM_NUM_UARTS                   1

#define BD_BCM63XX_TIMER_CLOCK_INPUT    (FPERIPH)


static struct bcm_serial multi[BCM_NUM_UARTS];
static struct bcm_serial *lines[BCM_NUM_UARTS];
static struct tty_driver *serial_driver;
static struct termios *serial_termios[BCM_NUM_UARTS];
static struct termios *serial_termios_locked[BCM_NUM_UARTS];


static void bcm_stop (struct tty_struct *tty);
static void bcm_start (struct tty_struct *tty);
static inline void receive_chars (struct bcm_serial * info);
static int startup (struct bcm_serial *info);
static void shutdown (struct bcm_serial * info);
static void change_speed( volatile Uart *pUart, tcflag_t cFlag );
static void bcm63xx_cons_flush_chars (struct tty_struct *tty);
static int bcm63xx_cons_write (struct tty_struct *tty, 
    const unsigned char *buf, int count);
static int bcm63xx_cons_write_room (struct tty_struct *tty);
static int bcm_chars_in_buffer (struct tty_struct *tty);
static void bcm_flush_buffer (struct tty_struct *tty);
static void bcm_throttle (struct tty_struct *tty);
static void bcm_unthrottle (struct tty_struct *tty);
static void bcm_send_xchar (struct tty_struct *tty, char ch);
static int get_serial_info(struct bcm_serial *info, struct serial_struct *retinfo);
static int set_serial_info (struct bcm_serial *info, struct serial_struct *new_info);
static int get_lsr_info (struct bcm_serial *info, unsigned int *value);
static void send_break (struct bcm_serial *info, int duration);
static int bcm_ioctl (struct tty_struct * tty, struct file * file,
    unsigned int cmd, unsigned long arg);
static void bcm_set_termios (struct tty_struct *tty, struct termios *old_termios);
static void bcm63xx_cons_close (struct tty_struct *tty, struct file *filp);
static void bcm_hangup (struct tty_struct *tty);
static int block_til_ready (struct tty_struct *tty, struct file *filp, struct bcm_serial *info);
static int bcm63xx_cons_open (struct tty_struct * tty, struct file * filp);
static int __init bcm63xx_serialinit(void);


/*
 * ------------------------------------------------------------
 * rs_stop () and rs_start ()
 *
 * These routines are called before setting or resetting 
 * tty->stopped. They enable or disable transmitter interrupts, 
 * as necessary.
 * ------------------------------------------------------------
 */
static void bcm_stop (struct tty_struct *tty)
{
}  

static void bcm_start (struct tty_struct *tty)
{
    _puts(CARDNAME " Start\n");
}  

/*
 * ------------------------------------------------------------
 * receive_char ()
 *
 * This routine deals with inputs from any lines.
 * ------------------------------------------------------------
 */
static inline void receive_chars (struct bcm_serial * info)
{
    struct tty_struct *tty = 0;
    struct async_icount * icount;
    int ignore = 0;
    unsigned short status, tmp;
    UCHAR ch = 0;
    while ((status = info->port->intStatus) & RXINT)
    {
		char flag_char = TTY_NORMAL;

        if (status & RXFIFONE)
            ch = info->port->Data;  // Read the character
        tty = info->tty;                  /* now tty points to the proper dev */
        icount = &info->icount;
        if (! tty)
            break;
        if (!tty_buffer_request_room(tty, 1))
            break;
        icount->rx++;
        if (status & RXBRK)
        {
            flag_char = TTY_BREAK;
            icount->brk++;
        }
        // keep track of the statistics
        if (status & (RXFRAMERR | RXPARERR | RXOVFERR))
        {
            if (status & RXPARERR)                /* parity error */
                icount->parity++;
            else
                if (status & RXFRAMERR)           /* frame error */
                    icount->frame++;
            if (status & RXOVFERR)
            {
                // Overflow. Reset the RX FIFO
                info->port->fifoctl |= RSTRXFIFOS;
                icount->overrun++;
            }
            // check to see if we should ignore the character
            // and mask off conditions that should be ignored
            if (status & info->ignore_status_mask)
            {
                if (++ignore > 100 )
                    break;
                goto ignore_char;
            }
            // Mask off the error conditions we want to ignore
            tmp = status & info->read_status_mask;
            if (tmp & RXPARERR)
            {
                flag_char = TTY_PARITY;
            }
            else
                if (tmp & RXFRAMERR)
                {
                    flag_char = TTY_FRAME;
                }
            if (tmp & RXOVFERR)
            {
	        tty_insert_flip_char(tty, ch, flag_char);
		ch = 0;
		flag_char = TTY_OVERRUN;
		if (!tty_buffer_request_room(tty, 1))
		  break;
            }
        }
	tty_insert_flip_char(tty, ch, flag_char);
    }
ignore_char:;
	tty_flip_buffer_push(tty);
	tty_schedule_flip(tty);

}


/*
 * ------------------------------------------------------------
 * bcm_interrupt ()
 *
 * this is the main interrupt routine for the chip.
 * It deals with the multiple ports.
 * ------------------------------------------------------------
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
static irqreturn_t bcm_interrupt (int irq, void * dev)
#else
static void bcm_interrupt (int irq, void * dev, struct pt_regs * regs)
#endif
{
    struct bcm_serial * info = lines[0];
    UINT16  intStat;

    /* get pending interrupt flags from UART  */

    /* Mask with only the serial interrupts that are enabled */
    intStat = info->port->intStatus & info->port->intMask;
    while (intStat)
    {
        if (intStat & RXINT)
            receive_chars (info);          
        else
            if (intStat & TXINT)
                info->port->intStatus = TXINT;
            else /* don't know what it was, so let's mask it */
                info->port->intMask &= ~intStat;

        intStat = info->port->intStatus & info->port->intMask;
    }

    // Clear the interrupt
    BcmHalInterruptEnable (INTERRUPT_ID_UART);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    return IRQ_HANDLED;
#endif    
}

/*
 * -------------------------------------------------------------------
 * startup ()
 *
 * various initialization tasks
 * ------------------------------------------------------------------- 
 */
static int startup (struct bcm_serial *info)
{
    // Port is already started...
    return 0;
}

/* 
 * -------------------------------------------------------------------
 * shutdown ()
 *
 * This routine will shutdown a serial port; interrupts are disabled, and
 * DTR is dropped if the hangup on close termio flag is on.
 * ------------------------------------------------------------------- 
 */
static void shutdown (struct bcm_serial * info)
{
    unsigned long flags;
    if (!info->is_initialized)
        return;

    spin_lock_irqsave(&bcm963xx_serial_lock, flags);

    info->port->control &= ~(BRGEN|TXEN|RXEN);
    if (info->tty)
        set_bit (TTY_IO_ERROR, &info->tty->flags);
    info->is_initialized = 0;

    spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
}
/* 
 * -------------------------------------------------------------------
 * change_speed ()
 *
 * Set the baud rate, character size, parity and stop bits.
 * ------------------------------------------------------------------- 
 */
static void change_speed( volatile Uart *pUart, tcflag_t cFlag )
{
    unsigned long ulFlags, ulBaud, ulClockFreqHz, ulTmp;
	
    spin_lock_irqsave(&bcm963xx_serial_lock, ulFlags);
    switch( cFlag & (CBAUD | CBAUDEX) )
    {
    case B115200:
        ulBaud = 115200;
        break;
    case B57600:
        ulBaud = 57600;
        break;
    case B38400:
        ulBaud = 38400;
        break;
    case B19200:
        ulBaud = 19200;
        break;
    case B9600:
        ulBaud = 9600;
        break;
    case B4800:
        ulBaud = 4800;
        break;
    case B2400:
        ulBaud = 2400;
        break;
    case B1800:
        ulBaud = 1800;
        break;
    case B1200:
        ulBaud = 1200;
        break;
    case B600:
        ulBaud = 600;
        break;
    case B300:
        ulBaud = 300;
        break;
    case B200:
        ulBaud = 200;
        break;
    case B150:
        ulBaud = 150;
        break;
    case B134:
        ulBaud = 134;
        break;
    case B110:
        ulBaud = 110;
        break;
    case B75:
        ulBaud = 75;
        break;
    case B50:
        ulBaud = 50;
        break;
    default:
        ulBaud = 115200;
        break;
    }

    /* Calculate buad rate.  */
    ulClockFreqHz = BD_BCM63XX_TIMER_CLOCK_INPUT;
    ulTmp = (ulClockFreqHz / ulBaud) / 16;
    if( ulTmp & 0x01 )
        ulTmp /= 2; /* Rounding up, so sub is already accounted for */
    else
        ulTmp = (ulTmp / 2) - 1; /* Rounding down so we must sub 1 */
    pUart->baudword = ulTmp;

    /* Set character size, stop bits and parity.  */
    switch( cFlag & CSIZE )
    {
    case CS5:
        ulTmp = BITS5SYM; /* select transmit 5 bit data size */
        break;
    case CS6:
        ulTmp = BITS6SYM; /* select transmit 6 bit data size */
        break;
    case CS7:
        ulTmp = BITS7SYM; /* select transmit 7 bit data size */
        break;
    default:
        ulTmp = BITS8SYM; /* select transmit 8 bit data size */
        break;
    }
    if( cFlag & CSTOPB )
        ulTmp |= TWOSTOP;         /* select 2 stop bits */
    else
        ulTmp |= ONESTOP;         /* select one stop bit */

    /* Write these values into the config reg.  */
    pUart->config = ulTmp;
    pUart->control &= ~(RXPARITYEN | TXPARITYEN | RXPARITYEVEN | TXPARITYEVEN);
    switch( cFlag & (PARENB | PARODD) )
    {
    case PARENB|PARODD:
        pUart->control |= RXPARITYEN | TXPARITYEN;
        break;
    case PARENB:
        pUart->control |= RXPARITYEN | TXPARITYEN | RXPARITYEVEN | TXPARITYEVEN;
        break;
    default:
        pUart->control |= 0;
        break;
    }

    /* Reset and flush uart */
    pUart->fifoctl = RSTTXFIFOS | RSTRXFIFOS;
    spin_unlock_irqrestore(&bcm963xx_serial_lock, ulFlags);
}


/* 
 * -------------------------------------------------------------------
 * bcm_flush_char ()
 *
 * Nothing to flush.  Polled I/O is used.
 * ------------------------------------------------------------------- 
 */
static void bcm63xx_cons_flush_chars (struct tty_struct *tty)
{
}


/* 
 * -------------------------------------------------------------------
 * bcm63xx_cons_write ()
 *
 * Main output routine using polled I/O.
 * ------------------------------------------------------------------- 
 */
static int bcm63xx_cons_write (struct tty_struct *tty, 
    const unsigned char *buf, int count)
{
    int c;

    for (c = 0; c < count; c++)
        _putc(buf[c]);
    return count;
}

/* 
 * -------------------------------------------------------------------
 * bcm63xx_cons_write_room ()
 *
 * Compute the amount of space available for writing.
 * ------------------------------------------------------------------- 
 */
static int bcm63xx_cons_write_room (struct tty_struct *tty)
{
    /* Pick a number.  Any number.  Polled I/O is used. */
    return 1024;
}

/* 
 * -------------------------------------------------------------------
 * bcm_chars_in_buffer ()
 *
 * compute the amount of char left to be transmitted
 * ------------------------------------------------------------------- 
 */
static int bcm_chars_in_buffer (struct tty_struct *tty)
{
    return 0;
}

/* 
 * -------------------------------------------------------------------
 * bcm_flush_buffer ()
 *
 * Empty the output buffer
 * ------------------------------------------------------------------- 
 */
static void bcm_flush_buffer (struct tty_struct *tty)
{
	tty_wakeup(tty);
}

/*
 * ------------------------------------------------------------
 * bcm_throttle () and bcm_unthrottle ()
 * 
 * This routine is called by the upper-layer tty layer to signal that
 * incoming characters should be throttled (or not).
 * ------------------------------------------------------------
 */
static void bcm_throttle (struct tty_struct *tty)
{
    struct bcm_serial *info = (struct bcm_serial *)tty->driver_data;  
    if (I_IXOFF(tty))
        info->x_char = STOP_CHAR(tty);
}

static void bcm_unthrottle (struct tty_struct *tty)
{
    struct bcm_serial *info = (struct bcm_serial *)tty->driver_data;  
    if (I_IXOFF(tty))
    {
        if (info->x_char)
            info->x_char = 0;
        else
            info->x_char = START_CHAR(tty);
    }
}

static void bcm_send_xchar (struct tty_struct *tty, char ch)
{
    struct bcm_serial *info = (struct bcm_serial *)tty->driver_data;
    info->x_char = ch;
    if (ch)
        bcm_start (info->tty);
}

/*
 * ------------------------------------------------------------
 * rs_ioctl () and friends
 * ------------------------------------------------------------
 */
static int get_serial_info(struct bcm_serial *info, struct serial_struct *retinfo)
{
    struct serial_struct tmp;

    if (!retinfo)
        return -EFAULT;

    memset (&tmp, 0, sizeof(tmp));
    tmp.type            = info->type;
    tmp.line            = info->line;
    tmp.port            = (int) info->port;
    tmp.irq             = info->irq;
    tmp.flags           = 0;
    tmp.baud_base       = info->baud_base;
    tmp.close_delay     = info->close_delay;
    tmp.closing_wait    = info->closing_wait;

    return copy_to_user (retinfo, &tmp, sizeof(*retinfo));
}

static int set_serial_info (struct bcm_serial *info, struct serial_struct *new_info)
{
    struct serial_struct new_serial;
    struct bcm_serial old_info;
    int retval = 0;

    if (!new_info)
        return -EFAULT;

    copy_from_user (&new_serial, new_info, sizeof(new_serial));
    old_info = *info;

    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;


    if (info->count > 1)
        return -EBUSY;

    /* OK, past this point, all the error checking has been done.
     * At this point, we start making changes.....
     */
    info->baud_base = new_serial.baud_base;
    info->type = new_serial.type;
    info->close_delay = new_serial.close_delay;
    info->closing_wait = new_serial.closing_wait;
    retval = startup (info);
    return retval;
}

/*
 * get_lsr_info - get line status register info
 *
 * Purpose: Let user call ioctl() to get info when the UART physically
 *          is emptied.  On bus types like RS485, the transmitter must
 *          release the bus after transmitting. This must be done when
 *          the transmit shift register is empty, not be done when the
 *          transmit holding register is empty.  This functionality
 *          allows an RS485 driver to be written in user space. 
 */
static int get_lsr_info (struct bcm_serial *info, unsigned int *value)
{
    return( 0 );
}

/*
 * This routine sends a break character out the serial port.
 */
static void send_break (struct bcm_serial *info, int duration)
{
    unsigned long flags;

    if (!info->port)
        return;

    current->state = TASK_INTERRUPTIBLE;

    /*save_flags (flags);
    cli();*/
    spin_lock_irqsave(&bcm963xx_serial_lock, flags);

    info->port->control |= XMITBREAK;
    schedule_timeout(duration);
    info->port->control &= ~XMITBREAK;

    spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
    //restore_flags (flags);
}

static int bcm_ioctl (struct tty_struct * tty, struct file * file,
    unsigned int cmd, unsigned long arg)
{
    int error;
    struct bcm_serial * info = (struct bcm_serial *)tty->driver_data;
    int retval;

    if ((cmd != TIOCGSERIAL) && (cmd != TIOCSSERIAL) &&
        (cmd != TIOCSERCONFIG) && (cmd != TIOCSERGWILD)  &&
        (cmd != TIOCSERSWILD) && (cmd != TIOCSERGSTRUCT))
    {
        if (tty->flags & (1 << TTY_IO_ERROR))
            return -EIO;
    }
    switch (cmd) 
    {

    case TCSBRK:    /* SVID version: non-zero arg --> no break */
        retval = tty_check_change (tty);
        if (retval)
            return retval;
        tty_wait_until_sent (tty, 0);
        if (!arg)
            send_break (info, HZ/4); /* 1/4 second */
        return 0;

    case TCSBRKP:   /* support for POSIX tcsendbreak() */
        retval = tty_check_change (tty);
        if (retval)
            return retval;
        tty_wait_until_sent (tty, 0);
        send_break (info, arg ? arg*(HZ/10) : HZ/4);
        return 0;

    case TIOCGSOFTCAR:
        error = access_ok (VERIFY_WRITE, (void *)arg, sizeof(long));
        if (!error)
            return -EFAULT;
	else
	{
	    put_user (C_CLOCAL(tty) ? 1 : 0, (unsigned long *)arg);
	    return 0;
	}

    case TIOCSSOFTCAR:
        error = get_user (arg, (unsigned long *)arg);
        if (error)
            return error;
        tty->termios->c_cflag = ((tty->termios->c_cflag & ~CLOCAL) | (arg ? CLOCAL : 0));
        return 0;

    case TIOCGSERIAL:
        error = access_ok (VERIFY_WRITE, (void *)arg, sizeof(struct serial_struct));
        if (!error)
            return -EFAULT;
	else
	    return get_serial_info (info, (struct serial_struct *)arg);

    case TIOCSSERIAL:
        return set_serial_info (info, (struct serial_struct *) arg);

    case TIOCSERGETLSR: /* Get line status register */
        error = access_ok (VERIFY_WRITE, (void *)arg, sizeof(unsigned int));
        if (!error)
            return -EFAULT;
        else
            return get_lsr_info (info, (unsigned int *)arg);

    case TIOCSERGSTRUCT:
        error = access_ok (VERIFY_WRITE, (void *)arg, sizeof(struct bcm_serial));
        if (!error)
            return -EFAULT;
	else
	{
	    copy_to_user((struct bcm_serial *)arg, info, sizeof(struct bcm_serial));
	    return 0;
	}

    default:
        return -ENOIOCTLCMD;
    }
    return 0;
}

static void bcm_set_termios (struct tty_struct *tty, struct termios *old_termios)
{
    struct bcm_serial *info = (struct bcm_serial *)tty->driver_data;

    if( tty->termios->c_cflag != old_termios->c_cflag )
        change_speed (info->port, tty->termios->c_cflag);
}

/*
 * ------------------------------------------------------------
 * bcm63xx_cons_close()
 * 
 * This routine is called when the serial port gets closed.  First, we
 * wait for the last remaining data to be sent.  Then, we turn off
 * the transmit enable and receive enable flags.
 * ------------------------------------------------------------
 */
static void bcm63xx_cons_close (struct tty_struct *tty, struct file *filp)
{
    struct bcm_serial * info = (struct bcm_serial *)tty->driver_data;
    unsigned long flags;

    if (!info)
        return;

    /*save_flags (flags); 
    cli();*/
    spin_lock_irqsave(&bcm963xx_serial_lock, flags);

    if (tty_hung_up_p (filp))
    {
        spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
        //restore_flags (flags);
        return;
    }

    if ((tty->count == 1) && (info->count != 1))
    {

        /* Uh, oh.  tty->count is 1, which means that the tty
         * structure will be freed.  Info->count should always
         * be one in these conditions.  If it's greater than
         * one, we've got real problems, since it means the
         * serial port won't be shutdown.
         */
        printk("bcm63xx_cons_close: bad serial port count; tty->count is 1, "
            "info->count is %d\n", info->count);
        info->count = 1;
    }

    if (--info->count < 0)
    {
        printk("ds_close: bad serial port count for ttys%d: %d\n",
        info->line, info->count);
        info->count = 0;
    }

    if (info->count)
    {
        //restore_flags (flags);
	spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
        return;
    }

    /* Now we wait for the transmit buffer to clear; and we notify 
     * the line discipline to only process XON/XOFF characters.
     */
    tty->closing = 1;

    /* At this point we stop accepting input.  To do this, we
     * disable the receive line status interrupts.
     */
    shutdown (info);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    if (tty->driver->flush_buffer)
        tty->driver->flush_buffer (tty);
#else    
    if (tty->driver.flush_buffer)
        tty->driver.flush_buffer (tty);
#endif
    if (tty->ldisc.flush_buffer)
        tty->ldisc.flush_buffer (tty);

    tty->closing = 0;
    info->event = 0;
    info->tty = 0;
    if (tty->ldisc.num != tty_ldisc_get(N_TTY)->num)
    {
        if (tty->ldisc.close)
            (tty->ldisc.close)(tty);
        tty->ldisc = *tty_ldisc_get(N_TTY);
        tty->termios->c_line = N_TTY;
        if (tty->ldisc.open)
            (tty->ldisc.open)(tty);
    }
    if (info->blocked_open)
    {
        if (info->close_delay)
        {
            current->state = TASK_INTERRUPTIBLE;
            schedule_timeout(info->close_delay);
        }
        wake_up_interruptible (&info->open_wait);
    }
    wake_up_interruptible (&info->close_wait);

    //restore_flags (flags);
    spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
}

/*
 * bcm_hangup () --- called by tty_hangup() when a hangup is signaled.
 */
static void bcm_hangup (struct tty_struct *tty)
{

    struct bcm_serial *info = (struct bcm_serial *)tty->driver_data;

    shutdown (info);
    info->event = 0;
    info->count = 0;
    info->tty = 0;
    wake_up_interruptible (&info->open_wait);
}

/*
 * ------------------------------------------------------------
 * rs_open() and friends
 * ------------------------------------------------------------
 */
static int block_til_ready (struct tty_struct *tty, struct file *filp,
    struct bcm_serial *info)
{
    return 0;
}       

/*
 * This routine is called whenever a serial port is opened.  It
 * enables interrupts for a serial port. It also performs the 
 * serial-specific initialization for the tty structure.
 */
static int bcm63xx_cons_open (struct tty_struct * tty, struct file * filp)
{
    struct bcm_serial *info;
    int retval, line;

    // Make sure we're only opening on of the ports we support
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    line = MINOR(tty->driver->cdev.dev) - tty->driver->minor_start;
#else
    line = MINOR(tty->device) - tty->driver.minor_start;
#endif    

    if ((line < 0) || (line >= BCM_NUM_UARTS))
        return -ENODEV;

    info = lines[line];

    tty->low_latency=1;
    info->port->intMask  = 0;     /* Clear any pending interrupts */
    info->port->intMask  = RXINT; /* Enable RX */

    info->count++;
    tty->driver_data = info;
    info->tty = tty;
    BcmHalInterruptEnable (INTERRUPT_ID_UART);

    // Start up serial port
    retval = startup (info);
    if (retval)
        return retval;

    retval = block_til_ready (tty, filp, info);
    if (retval)
        return retval;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    info->pgrp = process_group(current);
    info->session = current->signal->session;
#else
    info->session = current->session;    
    info->pgrp = current->pgrp;
#endif    

    return 0;
}


static struct tty_operations rs_ops = {
	.open = bcm63xx_cons_open,
	.close = bcm63xx_cons_close,
	.write = bcm63xx_cons_write,
	.flush_chars = bcm63xx_cons_flush_chars,
	.write_room = bcm63xx_cons_write_room,
	.chars_in_buffer = bcm_chars_in_buffer,
	.flush_buffer = bcm_flush_buffer,
	.ioctl = bcm_ioctl,
	.throttle = bcm_throttle,
	.unthrottle = bcm_unthrottle,
	.send_xchar = bcm_send_xchar,
	.set_termios = bcm_set_termios,
	.stop = bcm_stop,
	.start = bcm_start,
	.hangup = bcm_hangup,
};

/* --------------------------------------------------------------------------
    Name: bcm63xx_serialinit
 Purpose: Initialize our BCM63xx serial driver
-------------------------------------------------------------------------- */
static int __init bcm63xx_serialinit(void)
{
    int i, flags;
    struct bcm_serial * info;

    // Print the driver version information
    printk(VER_STR);
    serial_driver = alloc_tty_driver(BCM_NUM_UARTS);
    if (!serial_driver)
	return -ENOMEM;

	serial_driver->owner 	    = THIS_MODULE;
//	serial_driver->devfs_name 	    = "tts/";
//    serial_driver.magic             = TTY_DRIVER_MAGIC;
    serial_driver->name              = "ttyS";
    serial_driver->major             = TTY_MAJOR;
    serial_driver->minor_start       = 64;
//    serial_driver.num               = BCM_NUM_UARTS;
    serial_driver->type              = TTY_DRIVER_TYPE_SERIAL;
    serial_driver->subtype           = SERIAL_TYPE_NORMAL;
    serial_driver->init_termios      = tty_std_termios;
    serial_driver->init_termios.c_cflag = B115200 | CS8 | CREAD | HUPCL | CLOCAL;
    serial_driver->flags             = TTY_DRIVER_REAL_RAW;

    serial_driver->termios           = serial_termios;
    serial_driver->termios_locked    = serial_termios_locked;
    
    tty_set_operations(serial_driver, &rs_ops);

    if (tty_register_driver (serial_driver))
        panic("Couldn't register serial driver\n");

    //save_flags(flags); cli();
    spin_lock_irqsave(&bcm963xx_serial_lock, flags);
    
    for (i = 0; i < BCM_NUM_UARTS; i++)
    {
        info = &multi[i]; 
        lines[i] = info;
        info->magic = SERIAL_MAGIC;
        info->port                  = (Uart *) ((char *)UART_BASE + (i * 0x20));
        info->tty                   = 0;
        info->irq                   = (2 - i) + 8;
        info->line                  = i;
        info->close_delay           = 50;
        info->closing_wait          = 3000;
        info->x_char                = 0;
        info->event                 = 0;
        info->count                 = 0;
        info->blocked_open          = 0;	
        info->normal_termios        = serial_driver->init_termios;
        init_waitqueue_head(&info->open_wait); 
        init_waitqueue_head(&info->close_wait); 

        /* If we are pointing to address zero then punt - not correctly
         * set up in setup.c to handle this. 
         */
        if (! info->port)
            return 0;
        BcmHalMapInterrupt(bcm_interrupt, 0, INTERRUPT_ID_UART);
    }

    /* order matters here... the trick is that flags
     * is updated... in request_irq - to immediatedly obliterate
     * it is unwise. 
     */
    spin_unlock_irqrestore(&bcm963xx_serial_lock, flags);
    return 0;
}

module_init(bcm63xx_serialinit);

/* --------------------------------------------------------------------------
    Name: bcm_console_print
 Purpose: bcm_console_print is registered for printk.
          The console_lock must be held when we get here.
-------------------------------------------------------------------------- */
static void bcm_console_print (struct console * cons, const char * str,
    unsigned int count)
{
    unsigned int i;
    //_puts(str);
    for(i=0; i<count; i++, str++)
    {
        _putc(*str);
        if (*str == 10)
        {
            _putc(13);
        }
    }
}

static struct tty_driver * bcm_console_device(struct console * c, int *index)
{
    *index = c->index;
    return serial_driver;
}

static int __init bcm_console_setup(struct console * co, char * options)
{
    return 0;
}

static struct console bcm_sercons = {
    .name	= "ttyS",
    .write	= bcm_console_print,
    .device	= bcm_console_device,
    .setup	= bcm_console_setup,
    .flags	= CON_PRINTBUFFER,
    .index	= -1,
};

static int __init bcm63xx_console_init(void)
{
    register_console(&bcm_sercons);
    return 0;
}

console_initcall(bcm63xx_console_init);
