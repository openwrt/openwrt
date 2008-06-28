/*****************************************************************************
 * DANUBE BootROM
 * Copyright (c) 2005, Infineon Technologies AG, All rights reserved
 * IFAP DC COM SD
 *****************************************************************************/

#include <config.h>
//#include <lib.h>
#include <asm/danube.h>
#include <asm/addrspace.h>
#include <asm/ifx_asc.h>


#define ASC_FIFO_PRESENT
#define SET_BIT(reg, mask)                  reg |= (mask)
#define CLEAR_BIT(reg, mask)                reg &= (~mask)
#define CLEAR_BITS(reg, mask)               CLEAR_BIT(reg, mask)
#define SET_BITS(reg, mask)                 SET_BIT(reg, mask)
#define SET_BITFIELD(reg, mask, off, val)   {reg &= (~mask); reg |= (val << off);}


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed   long s32;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef volatile unsigned short vuint;



void serial_setbrg (void);

/*TODO: undefine this !!!*/
#undef DEBUG_ASC_RAW
#ifdef DEBUG_ASC_RAW
#define DEBUG_ASC_RAW_RX_BUF		0xA0800000
#define DEBUG_ASC_RAW_TX_BUF		0xA0900000
#endif

static volatile DanubeAsc_t *pAsc = (DanubeAsc_t *)DANUBE_ASC1;

typedef struct{
  u16 fdv; /* 0~511 fractional divider value*/
  u16 reload; /* 13 bit reload value*/
} ifx_asc_baud_reg_t;

#ifdef ON_VENUS
/*9600 @1.25M rel 00.08*/
//#define FDV 503
//#define RELOAD 7
/*9600 @0.625M rel final00.01 & rtl_freeze*/
#define FDV 503
#define RELOAD 3
/* first index is DDR_SEL, second index is FPI_SEL */
#endif
static ifx_asc_baud_reg_t g_danube_asc_baud[4][2] = 
{
#ifdef ON_VENUS
     {{503,3},{503,3}},   /* 1152000 @ 166.67M and half*/
      {{503,3},{503,3}},   /* 1152000 @ 133.3M  and half*/
      {{503,3},{503,3}},   /* 1152000 @ 111.11M and half*/
      {{503.3},{503,3}}    /* 1152000 @ 83.33M  and half*/
#else
/*  TAPEOUT table */
     {{436,76},{419,36}},   /* 1152000 @ 166.67M and half*/
      {{453,63},{453,31}},   /* 1152000 @ 133.3M  and half*/
      {{501,58},{510,29}},   /* 1152000 @ 111.11M and half*/
      {{419.36},{453,19}}    /* 1152000 @ 83.33M  and half*/
#endif
};
/******************************************************************************
*
* asc_init - initialize a Danube ASC channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/

int serial_init (void)
{

	/* and we have to set CLC register*/
	CLEAR_BIT(pAsc->asc_clc, ASCCLC_DISS);
	SET_BITFIELD(pAsc->asc_clc, ASCCLC_RMCMASK, ASCCLC_RMCOFFSET, 0x0001);

	/* initialy we are in async mode */
	pAsc->asc_con = ASCCON_M_8ASYNC;

	/* select input port */
	pAsc->asc_pisel = (CONSOLE_TTY & 0x1);

	/* TXFIFO's filling level */
	SET_BITFIELD(pAsc->asc_txfcon, ASCTXFCON_TXFITLMASK,
			ASCTXFCON_TXFITLOFF, DANUBEASC_TXFIFO_FL);
	/* enable TXFIFO */
	SET_BIT(pAsc->asc_txfcon, ASCTXFCON_TXFEN);

	/* RXFIFO's filling level */
	SET_BITFIELD(pAsc->asc_txfcon, ASCRXFCON_RXFITLMASK,
			ASCRXFCON_RXFITLOFF, DANUBEASC_RXFIFO_FL);
	/* enable RXFIFO */
	SET_BIT(pAsc->asc_rxfcon, ASCRXFCON_RXFEN);

	/* set baud rate */
	serial_setbrg();

	/* enable error signals &  Receiver enable  */
	SET_BIT(pAsc->asc_whbstate, ASCWHBSTATE_SETREN|ASCCON_FEN|ASCCON_TOEN|ASCCON_ROEN);

	return 0;
}

void serial_setbrg (void)
{
	u32 uiReloadValue, fdv;

#if defined(ON_IKOS)
	/*1200 @77K */
	fdv=472;
	uiReloadValue=5;
#else
	/*venus & tapeout */
  u32 ddr_sel,fpi_sel;
  ddr_sel = (* DANUBE_CGU_SYS) & 0x3;
  fpi_sel = ((* DANUBE_CGU_SYS) & 0x40)?1:0;
	fdv= g_danube_asc_baud[ddr_sel][fpi_sel].fdv;
	uiReloadValue=g_danube_asc_baud[ddr_sel][fpi_sel].reload;
#endif	//ON_IKOS
	/* Disable Baud Rate Generator; BG should only be written when R=0 */
	CLEAR_BIT(pAsc->asc_con, ASCCON_R);

	/* Enable Fractional Divider */
	SET_BIT(pAsc->asc_con, ASCCON_FDE); /* FDE = 1 */

	/* Set fractional divider value */
	pAsc->asc_fdv = fdv & ASCFDV_VALUE_MASK;

	/* Set reload value in BG */
	pAsc->asc_bg = uiReloadValue;

	/* Enable Baud Rate Generator */
	SET_BIT(pAsc->asc_con, ASCCON_R);           /* R = 1 */
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
	do
	{
		txFl = ( pAsc->asc_fstat & ASCFSTAT_TXFFLMASK ) >> ASCFSTAT_TXFFLOFF;
	}
	while ( txFl == DANUBEASC_TXFIFO_FULL );

	pAsc->asc_tbuf = c; /* write char to Transmit Buffer Register */

	/* check for errors */
	if ( pAsc->asc_state & ASCSTATE_TOE )
	{
		SET_BIT(pAsc->asc_whbstate, ASCWHBSTATE_CLRTOE);
		return;
	}
}

void serial_puts (const char *s)
{
	while (*s)
	{
		serial_putc (*s++);
	}
}

int asc_inb(int timeout)
{
	u32 symbol_mask;
	char c;
	while ((pAsc->asc_fstat & ASCFSTAT_RXFFLMASK) == 0 ) {
	}
	symbol_mask = ((ASC_OPTIONS & ASCOPT_CSIZE) == ASCOPT_CS7) ? (0x7f) : (0xff);
	c = (char)(pAsc->asc_rbuf & symbol_mask);
	return (c);
}

int serial_getc (void)
{
	char c;
	while ((pAsc->asc_fstat & ASCFSTAT_RXFFLMASK) == 0 );
	c = (char)(pAsc->asc_rbuf & 0xff);

#ifdef 	DEBUG_ASC_RAW
	static u8* debug=(u8*)(DEBUG_ASC_RAW_RX_BUF);
	*debug++=c;
#endif
	return c;
}



int serial_tstc (void)
{
         int res = 1;

#ifdef ASC_FIFO_PRESENT
    if ( (pAsc->asc_fstat & ASCFSTAT_RXFFLMASK) == 0 )
    {
        res = 0;
    }
#else
    if (!(*(volatile unsigned long*)(SFPI_INTCON_BASEADDR + FBS_ISR) &
			    					FBS_ISR_AR))
    
    {
        res = 0;
    }
#endif
#if 0
    else if ( pAsc->asc_con & ASCCON_FE )
    {
        SET_BIT(pAsc->asc_whbcon, ASCWHBCON_CLRFE);
        res = 0;
    }
    else if ( pAsc->asc_con & ASCCON_PE )
    {
        SET_BIT(pAsc->asc_whbcon, ASCWHBCON_CLRPE);
        res = 0;
    }
    else if ( pAsc->asc_con & ASCCON_OE )
    {
        SET_BIT(pAsc->asc_whbcon, ASCWHBCON_CLROE);
        res = 0;
    }
#endif
  return res;
}


int serial_start(void)
{
   return 1;
}

int serial_stop(void)
{
   return 1;
}
