/*
 * (INCA) ASC UART support
 */

#include <config.h>

#if defined(CONFIG_PURPLE) || defined(CONFIG_INCA_IP)

#ifdef CONFIG_PURPLE
#define	serial_init	asc_serial_init
#define	serial_putc	asc_serial_putc
#define	serial_puts	asc_serial_puts
#define	serial_getc	asc_serial_getc
#define	serial_tstc	asc_serial_tstc
#define	serial_setbrg	asc_serial_setbrg
#endif

#include <common.h>
#include <asm/inca-ip.h>
#include "asc_serial.h"

#ifdef CONFIG_PURPLE

#undef ASC_FIFO_PRESENT
#define TOUT_LOOP	100000

/* Set base address for second FPI interrupt control register bank */
#define SFPI_INTCON_BASEADDR	0xBF0F0000

/* Register offset from base address */
#define FBS_ISR		0x00000000	/* Interrupt status register */
#define FBS_IMR		0x00000008	/* Interrupt mask register */
#define FBS_IDIS	0x00000010	/* Interrupt disable register */

/* Interrupt status register bits */
#define FBS_ISR_AT	0x00000040	/* ASC transmit interrupt */
#define FBS_ISR_AR 	0x00000020	/* ASC receive interrupt */
#define FBS_ISR_AE	0x00000010	/* ASC error interrupt */
#define FBS_ISR_AB	0x00000008	/* ASC transmit buffer interrupt */
#define FBS_ISR_AS      0x00000004 	/* ASC start of autobaud detection interrupt */
#define FBS_ISR_AF	0x00000002	/* ASC end of autobaud detection interrupt */

#else

#define ASC_FIFO_PRESENT

#endif


#define SET_BIT(reg, mask)                  reg |= (mask)
#define CLEAR_BIT(reg, mask)                reg &= (~mask)
#define CLEAR_BITS(reg, mask)               CLEAR_BIT(reg, mask)
#define SET_BITS(reg, mask)                 SET_BIT(reg, mask)
#define SET_BITFIELD(reg, mask, off, val)   {reg &= (~mask); reg |= (val << off);}

extern uint incaip_get_fpiclk(void);

static int serial_setopt (void);

/* pointer to ASC register base address */
static volatile incaAsc_t *pAsc = (incaAsc_t *)INCA_IP_ASC;

/******************************************************************************
*
* serial_init - initialize a INCAASC channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/

int serial_init (void)
{
#ifdef CONFIG_INCA_IP
    /* we have to set PMU.EN13 bit to enable an ASC device*/
    INCAASC_PMU_ENABLE(13);
#endif

    /* and we have to set CLC register*/
    CLEAR_BIT(pAsc->asc_clc, ASCCLC_DISS);
    SET_BITFIELD(pAsc->asc_clc, ASCCLC_RMCMASK, ASCCLC_RMCOFFSET, 0x0001);

    /* initialy we are in async mode */
    pAsc->asc_con = ASCCON_M_8ASYNC;

    /* select input port */
    pAsc->asc_pisel = (CONSOLE_TTY & 0x1);

#ifdef ASC_FIFO_PRESENT
    /* TXFIFO's filling level */
    SET_BITFIELD(pAsc->asc_txfcon, ASCTXFCON_TXFITLMASK,
		    ASCTXFCON_TXFITLOFF, INCAASC_TXFIFO_FL);
    /* enable TXFIFO */
    SET_BIT(pAsc->asc_txfcon, ASCTXFCON_TXFEN);

    /* RXFIFO's filling level */
    SET_BITFIELD(pAsc->asc_txfcon, ASCRXFCON_RXFITLMASK,
		    ASCRXFCON_RXFITLOFF, INCAASC_RXFIFO_FL);
    /* enable RXFIFO */
    SET_BIT(pAsc->asc_rxfcon, ASCRXFCON_RXFEN);
#endif

    /* enable error signals */
    SET_BIT(pAsc->asc_con, ASCCON_FEN);
    SET_BIT(pAsc->asc_con, ASCCON_OEN);

#ifdef CONFIG_INCA_IP
    /* acknowledge ASC interrupts */
    ASC_INTERRUPTS_CLEAR(INCAASC_IRQ_LINE_ALL);

    /* disable ASC interrupts */
    ASC_INTERRUPTS_DISABLE(INCAASC_IRQ_LINE_ALL);
#endif

#ifdef ASC_FIFO_PRESENT
    /* set FIFOs into the transparent mode */
    SET_BIT(pAsc->asc_txfcon, ASCTXFCON_TXTMEN);
    SET_BIT(pAsc->asc_rxfcon, ASCRXFCON_RXTMEN);
#endif

    /* set baud rate */
    serial_setbrg();

    /* set the options */
    serial_setopt();

    return 0;
}

void serial_setbrg (void)
{
    ulong      uiReloadValue, fdv;
    ulong      f_ASC;

#ifdef CONFIG_INCA_IP
    f_ASC = incaip_get_fpiclk();
#else
    f_ASC = ASC_CLOCK_RATE;
#endif

#ifndef INCAASC_USE_FDV
    fdv = 2;
    uiReloadValue = (f_ASC / (fdv * 16 * CONFIG_BAUDRATE)) - 1;
#else
    fdv = INCAASC_FDV_HIGH_BAUDRATE;
    uiReloadValue = (f_ASC / (8192 * CONFIG_BAUDRATE / fdv)) - 1;
#endif /* INCAASC_USE_FDV */

    if ( (uiReloadValue < 0) || (uiReloadValue > 8191) )
    {
#ifndef INCAASC_USE_FDV
	fdv = 3;
	uiReloadValue = (f_ASC / (fdv * 16 * CONFIG_BAUDRATE)) - 1;
#else
	fdv = INCAASC_FDV_LOW_BAUDRATE;
	uiReloadValue = (f_ASC / (8192 * CONFIG_BAUDRATE / fdv)) - 1;
#endif /* INCAASC_USE_FDV */

	if ( (uiReloadValue < 0) || (uiReloadValue > 8191) )
	{
	    return;    /* can't impossibly generate that baud rate */
	}
    }

    /* Disable Baud Rate Generator; BG should only be written when R=0 */
    CLEAR_BIT(pAsc->asc_con, ASCCON_R);

#ifndef INCAASC_USE_FDV
    /*
     * Disable Fractional Divider (FDE)
     * Divide clock by reload-value + constant (BRS)
     */
    /* FDE = 0 */
    CLEAR_BIT(pAsc->asc_con, ASCCON_FDE);

    if ( fdv == 2 )
	CLEAR_BIT(pAsc->asc_con, ASCCON_BRS);   /* BRS = 0 */
    else
	SET_BIT(pAsc->asc_con, ASCCON_BRS); /* BRS = 1 */

#else /* INCAASC_USE_FDV */

    /* Enable Fractional Divider */
    SET_BIT(pAsc->asc_con, ASCCON_FDE); /* FDE = 1 */

    /* Set fractional divider value */
    pAsc->asc_fdv = fdv & ASCFDV_VALUE_MASK;

#endif /* INCAASC_USE_FDV */

    /* Set reload value in BG */
    pAsc->asc_bg = uiReloadValue;

    /* Enable Baud Rate Generator */
    SET_BIT(pAsc->asc_con, ASCCON_R);           /* R = 1 */
}

/*******************************************************************************
*
* serial_setopt - set the serial options
*
* Set the channel operating mode to that specified. Following options
* are supported: CREAD, CSIZE, PARENB, and PARODD.
*
* Note, this routine disables the transmitter.  The calling routine
* may have to re-enable it.
*
* RETURNS:
* Returns 0 to indicate success, otherwise -1 is returned
*/

static int serial_setopt (void)
{
    ulong  con;

    switch ( ASC_OPTIONS & ASCOPT_CSIZE )
    {
    /* 7-bit-data */
    case ASCOPT_CS7:
	con = ASCCON_M_7ASYNCPAR;   /* 7-bit-data and parity bit */
	break;

    /* 8-bit-data */
    case ASCOPT_CS8:
	if ( ASC_OPTIONS & ASCOPT_PARENB )
	    con = ASCCON_M_8ASYNCPAR;   /* 8-bit-data and parity bit */
	else
	    con = ASCCON_M_8ASYNC;      /* 8-bit-data no parity */
	break;

    /*
     *  only 7 and 8-bit frames are supported
     *  if we don't use IOCTL extensions
     */
    default:
	return -1;
    }

    if ( ASC_OPTIONS & ASCOPT_STOPB )
	SET_BIT(con, ASCCON_STP);       /* 2 stop bits */
    else
	CLEAR_BIT(con, ASCCON_STP);     /* 1 stop bit */

    if ( ASC_OPTIONS & ASCOPT_PARENB )
	SET_BIT(con, ASCCON_PEN);           /* enable parity checking */
    else
	CLEAR_BIT(con, ASCCON_PEN);         /* disable parity checking */

    if ( ASC_OPTIONS & ASCOPT_PARODD )
	SET_BIT(con, ASCCON_ODD);       /* odd parity */
    else
	CLEAR_BIT(con, ASCCON_ODD);     /* even parity */

    if ( ASC_OPTIONS & ASCOPT_CREAD )
	SET_BIT(pAsc->asc_whbcon, ASCWHBCON_SETREN); /* Receiver enable */

    pAsc->asc_con |= con;

    return 0;
}

void serial_putc (const char c)
{
#ifdef ASC_FIFO_PRESENT
    uint txFl = 0;
#else
    uint timeout = 0;
#endif

    if (c == '\n') serial_putc ('\r');

#ifdef ASC_FIFO_PRESENT
    /* check do we have a free space in the TX FIFO */
    /* get current filling level */
    do
    {
	txFl = ( pAsc->asc_fstat & ASCFSTAT_TXFFLMASK ) >> ASCFSTAT_TXFFLOFF;
    }
    while ( txFl == INCAASC_TXFIFO_FULL );
#else

    while(!(*(volatile unsigned long*)(SFPI_INTCON_BASEADDR + FBS_ISR) &
			   FBS_ISR_AB))
    {
	    if (timeout++ > TOUT_LOOP)
	    {
		    break;
	    }
    }
#endif

    pAsc->asc_tbuf = c; /* write char to Transmit Buffer Register */

#ifndef ASC_FIFO_PRESENT
    *(volatile unsigned long*)(SFPI_INTCON_BASEADDR + FBS_ISR) = FBS_ISR_AB |
								 FBS_ISR_AT;
#endif

    /* check for errors */
    if ( pAsc->asc_con & ASCCON_OE )
    {
	SET_BIT(pAsc->asc_whbcon, ASCWHBCON_CLROE);
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

int serial_getc (void)
{
    ulong symbol_mask;
    char c;

    while (!serial_tstc());

    symbol_mask =
	((ASC_OPTIONS & ASCOPT_CSIZE) == ASCOPT_CS7) ? (0x7f) : (0xff);

    c = (char)(pAsc->asc_rbuf & symbol_mask);

#ifndef ASC_FIFO_PRESENT
    *(volatile unsigned long*)(SFPI_INTCON_BASEADDR + FBS_ISR) = FBS_ISR_AR;
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

    return res;
}
#endif /* CONFIG_PURPLE || CONFIG_INCA_IP */
