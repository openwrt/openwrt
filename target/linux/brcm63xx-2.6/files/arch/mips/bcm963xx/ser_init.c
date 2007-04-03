/*
<:copyright-gpl 
 Copyright 2004 Broadcom Corp. All Rights Reserved. 
 
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
/*
 *  Broadcom bcm63xx serial port initialization, also prepare for printk
 *  by registering with console_init
 *   
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/console.h>
#include <linux/sched.h>

#include <asm/addrspace.h>
#include <asm/irq.h>
#include <asm/reboot.h>
#include <asm/gdb-stub.h>
#include <asm/mc146818rtc.h> 

#include <bcm_map_part.h>
#include <board.h>

#define  SER63XX_DEFAULT_BAUD      115200
#define BD_BCM63XX_TIMER_CLOCK_INPUT    (FPERIPH)
#define stUart ((volatile Uart * const) UART_BASE)

// Transmit interrupts
#define TXINT       (TXFIFOEMT | TXUNDERR | TXOVFERR)
// Receive interrupts
#define RXINT       (RXFIFONE | RXOVFERR)

/* --------------------------------------------------------------------------
    Name: serial_init
 Purpose: Initalize the UART
-------------------------------------------------------------------------- */
void __init serial_init(void)
{
    UINT32 tmpVal = SER63XX_DEFAULT_BAUD;
    ULONG clockFreqHz;    

#if defined(CONFIG_BCM96345)
    // Make sure clock is ticking
    PERF->blkEnables |= UART_CLK_EN;
#endif
		
    /* Dissable channel's receiver and transmitter.                */
    stUart->control &= ~(BRGEN|TXEN|RXEN);
		
    /*--------------------------------------------------------------------*/
    /* Write the table value to the clock select register.                */
    /* DPullen - this is the equation to use:                             */
    /*       value = clockFreqHz / baud / 32-1;                           */
    /*   (snmod) Actually you should also take into account any necessary */
    /*           rounding.  Divide by 16, look at lsb, if 0, divide by 2  */
    /*           and subtract 1.  If 1, just divide by 2                  */
    /*--------------------------------------------------------------------*/
    clockFreqHz = BD_BCM63XX_TIMER_CLOCK_INPUT;
    tmpVal = (clockFreqHz / tmpVal) / 16;
    if( tmpVal & 0x01 )
        tmpVal /= 2;  //Rounding up, so sub is already accounted for
    else
        tmpVal = (tmpVal / 2) - 1; // Rounding down so we must sub 1
    stUart->baudword = tmpVal;
        
    /* Finally, re-enable the transmitter and receiver.            */
    stUart->control |= (BRGEN|TXEN|RXEN);

    stUart->config   = (BITS8SYM | ONESTOP);
    // Set the FIFO interrupt depth ... stUart->fifocfg  = 0xAA;
    stUart->fifoctl  =  RSTTXFIFOS | RSTRXFIFOS;
    stUart->intMask  = 0;       
    stUart->intMask = RXINT | TXINT;
}


/* prom_putc()
 * Output a character to the UART
 */
void prom_putc(char c)
{
	/* Wait for Tx uffer to empty */
	while (! (READ16(stUart->intStatus) & TXFIFOEMT));
	/* Send character */
	stUart->Data = c;
}

/* prom_puts()
 * Write a string to the UART
 */
void prom_puts(const char *s)
{
	while (*s) {
		if (*s == '\n') {
			prom_putc('\r');
		}
		prom_putc(*s++);
	}
}


/* prom_getc_nowait()
 * Returns a character from the UART
 * Returns -1 if no characters available or corrupted
 */
int prom_getc_nowait(void)
{
    uint16  uStatus;
    int    cData = -1;

     uStatus = READ16(stUart->intStatus);

     if (uStatus & RXFIFONE) { /* Do we have a character? */
           cData =  READ16(stUart->Data) & 0xff; /* Read character */
           if (uStatus & (RXFRAMERR | RXPARERR)) {  /* If we got an error, throw it away */
               cData = -1;
           }
  }

   return cData;
}

/* prom_getc()
 * Returns a charcter from the serial port
 * Will block until it receives a valid character
*/
char prom_getc(void)
{
    int    cData = -1;

    /* Loop until we get a valid character */
    while(cData == -1) {
	cData = prom_getc_nowait();
    }
   return (char) cData;
}

/* prom_testc()
 * Returns 0 if no characters available
 */
int prom_testc(void)
{
    uint16  uStatus;

     uStatus = READ16(stUart->intStatus);

     return (uStatus & RXFIFONE);
}

#if defined (CONFIG_REMOTE_DEBUG)
/* Prevent other code from writing to the serial port */
void _putc(char c) { }
void _puts(const char *ptr) { }
#else
/* Low level outputs call prom routines */
void _putc(char c) {
	prom_putc(c);
}
void _puts(const char *ptr) {
	prom_puts(ptr);
}
#endif
