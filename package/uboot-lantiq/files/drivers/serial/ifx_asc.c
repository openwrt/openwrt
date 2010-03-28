/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * (C) Copyright 2009
 * Infineon Technologies AG, http://www.infineon.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/addrspace.h>

#include "ifx_asc.h"

#define SET_BIT(reg, mask)			asc_writel(reg, asc_readl(reg) | (mask))
#define CLEAR_BIT(reg, mask)			asc_writel(reg, asc_readl(reg) & (~mask))
#define SET_BITFIELD(reg, mask, off, val)	asc_writel(reg, (asc_readl(reg) & (~mask)) | (val << off) )

#undef DEBUG_ASC_RAW
#ifdef DEBUG_ASC_RAW
#define DEBUG_ASC_RAW_RX_BUF			0xA0800000
#define DEBUG_ASC_RAW_TX_BUF			0xA0900000
#endif

DECLARE_GLOBAL_DATA_PTR;

static IfxAsc_t *pAsc = (IfxAsc_t *)CKSEG1ADDR(CONFIG_SYS_IFX_ASC_BASE);

/*
 *             FDV            fASC
 * BaudRate = ----- * --------------------
 *             512    16 * (ReloadValue+1)
 */

/*
 *                  FDV          fASC
 * ReloadValue = ( ----- * --------------- ) - 1
 *                  512     16 * BaudRate
 */
static void serial_divs(u32 baudrate, u32 fasc, u32 *pfdv, u32 *preload)
{
   u32 clock = fasc / 16;

   u32 fdv; /* best fdv */
   u32 reload = 0; /* best reload */
   u32 diff; /* smallest diff */
   u32 idiff; /* current diff */
   u32 ireload; /* current reload */
   u32 i; /* current fdv */
   u32 result; /* current resulting baudrate */

   if (clock > 0x7FFFFF)
      clock /= 512;
   else
      baudrate *= 512;

   fdv = 512; /* start with 1:1 fraction */
   diff = baudrate; /* highest possible */

   /* i is the test fdv value -- start with the largest possible */
   for (i = 512; i > 0; i--)
   {
      ireload = (clock * i) / baudrate;
      if (ireload < 1)
         break; /* already invalid */
      result = (clock * i) / ireload;

      idiff = (result > baudrate) ? (result - baudrate) : (baudrate - result);
      if (idiff == 0)
      {
         fdv = i;
         reload = ireload;
         break; /* can't do better */
      }
      else if (idiff < diff)
      {
         fdv = i; /* best so far */
         reload = ireload;
         diff = idiff; /* update lowest diff*/
      }
   }

   *pfdv = (fdv == 512) ? 0 : fdv;
   *preload = reload - 1;
}


void serial_setbrg (void)
{
	u32 ReloadValue, fdv;

	serial_divs(gd->baudrate, get_bus_freq(0), &fdv, &ReloadValue);

	/* Disable Baud Rate Generator; BG should only be written when R=0 */
	CLEAR_BIT(asc_con, ASCCON_R);

	/* Enable Fractional Divider */
	SET_BIT(asc_con, ASCCON_FDE);	/* FDE = 1 */

	/* Set fractional divider value */
	asc_writel(asc_fdv, fdv & ASCFDV_VALUE_MASK);

	/* Set reload value in BG */
	asc_writel(asc_bg, ReloadValue);

	/* Enable Baud Rate Generator */
	SET_BIT(asc_con, ASCCON_R);	/* R = 1 */
}


int serial_init (void)
{

	/* and we have to set CLC register*/
	CLEAR_BIT(asc_clc, ASCCLC_DISS);
	SET_BITFIELD(asc_clc, ASCCLC_RMCMASK, ASCCLC_RMCOFFSET, 0x0001);

	/* initialy we are in async mode */
	asc_writel(asc_con, ASCCON_M_8ASYNC);

	/* select input port */
	asc_writel(asc_pisel, CONSOLE_TTY & 0x1);

	/* TXFIFO's filling level */
	SET_BITFIELD(asc_txfcon, ASCTXFCON_TXFITLMASK,
			ASCTXFCON_TXFITLOFF, ASC_TXFIFO_FL);
	/* enable TXFIFO */
	SET_BIT(asc_txfcon, ASCTXFCON_TXFEN);

	/* RXFIFO's filling level */
	SET_BITFIELD(asc_txfcon, ASCRXFCON_RXFITLMASK,
			ASCRXFCON_RXFITLOFF, ASC_RXFIFO_FL);
	/* enable RXFIFO */
	SET_BIT(asc_rxfcon, ASCRXFCON_RXFEN);

	/* set baud rate */
	serial_setbrg();

	/* enable error signals &  Receiver enable  */
	SET_BIT(asc_whbstate, ASCWHBSTATE_SETREN|ASCCON_FEN|ASCCON_TOEN|ASCCON_ROEN);

	return 0;
}


void serial_putc (const char c)
{
	u32 txFl = 0;
#ifdef DEBUG_ASC_RAW
	static u8 * debug = (u8 *) DEBUG_ASC_RAW_TX_BUF;
	*debug++=c;
#endif
	if (c == '\n')
		serial_putc ('\r');
	/* check do we have a free space in the TX FIFO */
	/* get current filling level */
	do {
		txFl = ( asc_readl(asc_fstat) & ASCFSTAT_TXFFLMASK ) >> ASCFSTAT_TXFFLOFF;
	}
	while ( txFl == ASC_TXFIFO_FULL );

	asc_writel(asc_tbuf, c); /* write char to Transmit Buffer Register */

	/* check for errors */
	if ( asc_readl(asc_state) & ASCSTATE_TOE ) {
		SET_BIT(asc_whbstate, ASCWHBSTATE_CLRTOE);
		return;
	}
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

int serial_getc (void)
{
	char c;
	while ((asc_readl(asc_fstat) & ASCFSTAT_RXFFLMASK) == 0 );
	c = (char)(asc_readl(asc_rbuf) & 0xff);

#ifdef 	DEBUG_ASC_RAW
	static u8* debug=(u8*)(DEBUG_ASC_RAW_RX_BUF);
	*debug++=c;
#endif
	return c;
}


int serial_tstc (void)
{
	int res = 1;

	if ( (asc_readl(asc_fstat) & ASCFSTAT_RXFFLMASK) == 0 ) {
		res = 0;
	}
	return res;
}
