/*
 * BCM47XX Sonics SiliconBackplane MIPS core routines
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <typedefs.h>
#include <osl.h>
#include <sbutils.h>
#include <bcmdevs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <hndmips.h>
#include <sbconfig.h>
#include <sbextif.h>
#include <sbchipc.h>
#include <sbmemc.h>

/*
 * Memory segments (32bit kernel mode addresses)
 */
#undef KUSEG
#undef KSEG0
#undef KSEG1
#undef KSEG2
#undef KSEG3
#define KUSEG		0x00000000
#define KSEG0		0x80000000
#define KSEG1		0xa0000000
#define KSEG2		0xc0000000
#define KSEG3		0xe0000000

/*
 * Map an address to a certain kernel segment
 */
#undef KSEG0ADDR
#undef KSEG1ADDR
#undef KSEG2ADDR
#undef KSEG3ADDR
#define KSEG0ADDR(a)		(((a) & 0x1fffffff) | KSEG0)
#define KSEG1ADDR(a)		(((a) & 0x1fffffff) | KSEG1)
#define KSEG2ADDR(a)		(((a) & 0x1fffffff) | KSEG2)
#define KSEG3ADDR(a)		(((a) & 0x1fffffff) | KSEG3)

/*
 * The following macros are especially useful for __asm__
 * inline assembler.
 */
#ifndef __STR
#define __STR(x) #x
#endif
#ifndef STR
#define STR(x) __STR(x)
#endif

/*  *********************************************************************
    *  CP0 Registers 
    ********************************************************************* */

#define C0_INX		0		/* CP0: TLB Index */
#define C0_RAND		1		/* CP0: TLB Random */
#define C0_TLBLO0	2		/* CP0: TLB EntryLo0 */
#define C0_TLBLO	C0_TLBLO0	/* CP0: TLB EntryLo0 */
#define C0_TLBLO1	3		/* CP0: TLB EntryLo1 */
#define C0_CTEXT	4		/* CP0: Context */
#define C0_PGMASK	5		/* CP0: TLB PageMask */
#define C0_WIRED	6		/* CP0: TLB Wired */
#define C0_BADVADDR	8		/* CP0: Bad Virtual Address */
#define C0_COUNT 	9		/* CP0: Count */
#define C0_TLBHI	10		/* CP0: TLB EntryHi */
#define C0_COMPARE	11		/* CP0: Compare */
#define C0_SR		12		/* CP0: Processor Status */
#define C0_STATUS	C0_SR		/* CP0: Processor Status */
#define C0_CAUSE	13		/* CP0: Exception Cause */
#define C0_EPC		14		/* CP0: Exception PC */
#define C0_PRID		15		/* CP0: Processor Revision Indentifier */
#define C0_CONFIG	16		/* CP0: Config */
#define C0_LLADDR	17		/* CP0: LLAddr */
#define C0_WATCHLO	18		/* CP0: WatchpointLo */
#define C0_WATCHHI	19		/* CP0: WatchpointHi */
#define C0_XCTEXT	20		/* CP0: XContext */
#define C0_DIAGNOSTIC	22		/* CP0: Diagnostic */
#define C0_BROADCOM	C0_DIAGNOSTIC	/* CP0: Broadcom Register */
#define C0_ECC		26		/* CP0: ECC */
#define C0_CACHEERR	27		/* CP0: CacheErr */
#define C0_TAGLO	28		/* CP0: TagLo */
#define C0_TAGHI	29		/* CP0: TagHi */
#define C0_ERREPC	30		/* CP0: ErrorEPC */

/*
 * Macros to access the system control coprocessor
 */

#define MFC0(source, sel)					\
({								\
	int __res;						\
	__asm__ __volatile__(					\
	".set\tnoreorder\n\t"					\
	".set\tnoat\n\t"					\
	".word\t"STR(0x40010000 | ((source)<<11) | (sel))"\n\t"	\
	"move\t%0,$1\n\t"					\
	".set\tat\n\t"						\
	".set\treorder"						\
	:"=r" (__res)						\
	:							\
	:"$1");							\
	__res;							\
})

#define MTC0(source, sel, value)				\
do {								\
	__asm__ __volatile__(					\
	".set\tnoreorder\n\t"					\
	".set\tnoat\n\t"					\
	"move\t$1,%z0\n\t"					\
	".word\t"STR(0x40810000 | ((source)<<11) | (sel))"\n\t"	\
	".set\tat\n\t"						\
	".set\treorder"						\
	:							\
	:"Jr" (value)						\
	:"$1");							\
} while (0)

/*
 * R4x00 interrupt enable / cause bits
 */
#undef IE_SW0
#undef IE_SW1
#undef IE_IRQ0
#undef IE_IRQ1
#undef IE_IRQ2
#undef IE_IRQ3
#undef IE_IRQ4
#undef IE_IRQ5
#define IE_SW0		(1<< 8)
#define IE_SW1		(1<< 9)
#define IE_IRQ0		(1<<10)
#define IE_IRQ1		(1<<11)
#define IE_IRQ2		(1<<12)
#define IE_IRQ3		(1<<13)
#define IE_IRQ4		(1<<14)
#define IE_IRQ5		(1<<15)

/*
 * Bitfields in the R4xx0 cp0 status register
 */
#define ST0_IE			0x00000001
#define ST0_EXL			0x00000002
#define ST0_ERL			0x00000004
#define ST0_KSU			0x00000018
#  define KSU_USER		0x00000010
#  define KSU_SUPERVISOR	0x00000008
#  define KSU_KERNEL		0x00000000
#define ST0_UX			0x00000020
#define ST0_SX			0x00000040
#define ST0_KX 			0x00000080
#define ST0_DE			0x00010000
#define ST0_CE			0x00020000

/*
 * Status register bits available in all MIPS CPUs.
 */
#define ST0_IM			0x0000ff00
#define ST0_CH			0x00040000
#define ST0_SR			0x00100000
#define ST0_TS			0x00200000
#define ST0_BEV			0x00400000
#define ST0_RE			0x02000000
#define ST0_FR			0x04000000
#define ST0_CU			0xf0000000
#define ST0_CU0			0x10000000
#define ST0_CU1			0x20000000
#define ST0_CU2			0x40000000
#define ST0_CU3			0x80000000
#define ST0_XX			0x80000000	/* MIPS IV naming */

/*
 * Cache Operations
 */

#ifndef Fill_I
#define Fill_I			0x14
#endif

#define cache_unroll(base,op)			\
	__asm__ __volatile__("			\
		.set noreorder;			\
		.set mips3;			\
		cache %1, (%0);			\
		.set mips0;			\
		.set reorder"			\
		:				\
		: "r" (base),			\
		  "i" (op));

/* 
 * These are the UART port assignments, expressed as offsets from the base
 * register.  These assignments should hold for any serial port based on
 * a 8250, 16450, or 16550(A).
 */

#define UART_MCR	4	/* Out: Modem Control Register */
#define UART_MSR	6	/* In:  Modem Status Register */
#define UART_MCR_LOOP	0x10	/* Enable loopback test mode */

/* 
 * Returns TRUE if an external UART exists at the given base
 * register.
 */
static bool
serial_exists(uint8 *regs)
{
	uint8 save_mcr, status1;

	save_mcr = R_REG(&regs[UART_MCR]);
	W_REG(&regs[UART_MCR], UART_MCR_LOOP | 0x0a);
	status1 = R_REG(&regs[UART_MSR]) & 0xf0;
	W_REG(&regs[UART_MCR], save_mcr);

	return (status1 == 0x90);
}

/* 
 * Initializes UART access. The callback function will be called once
 * per found UART.
*/
void
sb_serial_init(void *sbh, void (*add)(void *regs, uint irq, uint baud_base, uint reg_shift))
{
	void *regs;
	ulong base;
	uint irq;
	int i, n;

	if ((regs = sb_setcore(sbh, SB_EXTIF, 0))) {
		extifregs_t *eir = (extifregs_t *) regs;
		sbconfig_t *sb;

		/* Determine external UART register base */
		sb = (sbconfig_t *)((ulong) eir + SBCONFIGOFF);
		base = EXTIF_CFGIF_BASE(sb_base(R_REG(&sb->sbadmatch1)));

		/* Determine IRQ */
		irq = sb_irq(sbh);

		/* Disable GPIO interrupt initially */
		W_REG(&eir->gpiointpolarity, 0);
		W_REG(&eir->gpiointmask, 0);

		/* Search for external UARTs */
		n = 2;
		for (i = 0; i < 2; i++) {
			regs = (void *) REG_MAP(base + (i * 8), 8);
			if (serial_exists(regs)) {
				/* Set GPIO 1 to be the external UART IRQ */
				W_REG(&eir->gpiointmask, 2);
				if (add)
					add(regs, irq, 13500000, 0);
			}
		}

		/* Add internal UART if enabled */
		if (R_REG(&eir->corecontrol) & CC_UE)
			if (add)
				add((void *) &eir->uartdata, irq, sb_clock(sbh), 2);
	} else if ((regs = sb_setcore(sbh, SB_CC, 0))) {
		chipcregs_t *cc = (chipcregs_t *) regs;
		uint32 rev, cap, pll, baud_base, div;

		/* Determine core revision and capabilities */
		rev = sb_corerev(sbh);
		cap = R_REG(&cc->capabilities);
		pll = cap & CAP_PLL_MASK;

		/* Determine IRQ */
		irq = sb_irq(sbh);

		if (pll == PLL_TYPE1) {
			/* PLL clock */
			baud_base = sb_clock_rate(pll,
						  R_REG(&cc->clockcontrol_n),
						  R_REG(&cc->clockcontrol_m2));
			div = 1;
		} else if (rev >= 3) {
			/* Internal backplane clock */
			baud_base = sb_clock_rate(pll,
						  R_REG(&cc->clockcontrol_n),
						  R_REG(&cc->clockcontrol_sb));
			div = 2;	/* Minimum divisor */
			W_REG(&cc->clkdiv, ((R_REG(&cc->clkdiv) & ~CLKD_UART) | div));
		} else {
			/* Fixed internal backplane clock */
			baud_base = 88000000;
			div = 48;
		}

		/* Clock source depends on strapping if UartClkOverride is unset */
		if ((rev > 0) && ((R_REG(&cc->corecontrol) & CC_UARTCLKO) == 0)) {
			if ((cap & CAP_UCLKSEL) == CAP_UINTCLK) {
				/* Internal divided backplane clock */
				baud_base /= div;
			} else {
				/* Assume external clock of 1.8432 MHz */
				baud_base = 1843200;
			}
		}

		/* Add internal UARTs */
		n = cap & CAP_UARTS_MASK;
		for (i = 0; i < n; i++) {
			/* Register offset changed after revision 0 */
			if (rev)
				regs = (void *)((ulong) &cc->uart0data + (i * 256));
			else
				regs = (void *)((ulong) &cc->uart0data + (i * 8));

			if (add)
				add(regs, irq, baud_base, 0);
		}
	}
}

/* Returns the SB interrupt flag of the current core. */
uint32
sb_flag(void *sbh)
{
	void *regs;
	sbconfig_t *sb;

	regs = sb_coreregs(sbh);
	sb = (sbconfig_t *)((ulong) regs + SBCONFIGOFF);

	return (R_REG(&sb->sbtpsflag) & SBTPS_NUM0_MASK);
}

static const uint32 sbips_int_mask[] = {
	0,
	SBIPS_INT1_MASK,
	SBIPS_INT2_MASK,
	SBIPS_INT3_MASK,
	SBIPS_INT4_MASK
};

static const uint32 sbips_int_shift[] = {
	0,
	0,
	SBIPS_INT2_SHIFT,
	SBIPS_INT3_SHIFT,
	SBIPS_INT4_SHIFT
};

/* 
 * Returns the MIPS IRQ assignment of the current core. If unassigned,
 * 0 is returned.
 */
uint
sb_irq(void *sbh)
{
	uint idx;
	void *regs;
	sbconfig_t *sb;
	uint32 flag, sbipsflag;
	uint irq = 0;

	flag = sb_flag(sbh);

	idx = sb_coreidx(sbh);

	if ((regs = sb_setcore(sbh, SB_MIPS, 0)) ||
	    (regs = sb_setcore(sbh, SB_MIPS33, 0))) {
		sb = (sbconfig_t *)((ulong) regs + SBCONFIGOFF);

		/* sbipsflag specifies which core is routed to interrupts 1 to 4 */
		sbipsflag = R_REG(&sb->sbipsflag);
		for (irq = 1; irq <= 4; irq++) {
			if (((sbipsflag & sbips_int_mask[irq]) >> sbips_int_shift[irq]) == flag)
				break;
		}
		if (irq == 5)
			irq = 0;
	}

	sb_setcoreidx(sbh, idx);

	return irq;
}

/* Clears the specified MIPS IRQ. */
static void
sb_clearirq(void *sbh, uint irq)
{
	void *regs;
	sbconfig_t *sb;

	if (!(regs = sb_setcore(sbh, SB_MIPS, 0)) &&
	    !(regs = sb_setcore(sbh, SB_MIPS33, 0)))
		ASSERT(regs);
	sb = (sbconfig_t *)((ulong) regs + SBCONFIGOFF);

	if (irq == 0)
		W_REG(&sb->sbintvec, 0);
	else
		OR_REG(&sb->sbipsflag, sbips_int_mask[irq]);
}

/* 
 * Assigns the specified MIPS IRQ to the specified core. Shared MIPS
 * IRQ 0 may be assigned more than once.
 */
static void
sb_setirq(void *sbh, uint irq, uint coreid, uint coreunit)
{
	void *regs;
	sbconfig_t *sb;
	uint32 flag;

	regs = sb_setcore(sbh, coreid, coreunit);
	ASSERT(regs);
	flag = sb_flag(sbh);

	if (!(regs = sb_setcore(sbh, SB_MIPS, 0)) &&
	    !(regs = sb_setcore(sbh, SB_MIPS33, 0)))
		ASSERT(regs);
	sb = (sbconfig_t *)((ulong) regs + SBCONFIGOFF);

	if (irq == 0)
		OR_REG(&sb->sbintvec, 1 << flag);
	else {
		flag <<= sbips_int_shift[irq];
		ASSERT(!(flag & ~sbips_int_mask[irq]));
		flag |= R_REG(&sb->sbipsflag) & ~sbips_int_mask[irq];
		W_REG(&sb->sbipsflag, flag);
	}
}	

/* 
 * Initializes clocks and interrupts. SB and NVRAM access must be
 * initialized prior to calling.
 */
void
sb_mips_init(void *sbh)
{
	ulong hz, ns, tmp;
	extifregs_t *eir;
	chipcregs_t *cc;
	char *value;
	uint irq;

	/* Figure out current SB clock speed */
	if ((hz = sb_clock(sbh)) == 0)
		hz = 100000000;
	ns = 1000000000 / hz;

	/* Setup external interface timing */
	if ((eir = sb_setcore(sbh, SB_EXTIF, 0))) {
		/* Initialize extif so we can get to the LEDs and external UART */
		W_REG(&eir->prog_config, CF_EN);

		/* Set timing for the flash */
		tmp = CEIL(10, ns) << FW_W3_SHIFT;	/* W3 = 10nS */
		tmp = tmp | (CEIL(40, ns) << FW_W1_SHIFT); /* W1 = 40nS */
		tmp = tmp | CEIL(120, ns);		/* W0 = 120nS */
		W_REG(&eir->prog_waitcount, tmp);	/* 0x01020a0c for a 100Mhz clock */

		/* Set programmable interface timing for external uart */
		tmp = CEIL(10, ns) << FW_W3_SHIFT;	/* W3 = 10nS */
		tmp = tmp | (CEIL(20, ns) << FW_W2_SHIFT); /* W2 = 20nS */
		tmp = tmp | (CEIL(100, ns) << FW_W1_SHIFT); /* W1 = 100nS */
		tmp = tmp | CEIL(120, ns);		/* W0 = 120nS */
		W_REG(&eir->prog_waitcount, tmp);	/* 0x01020a0c for a 100Mhz clock */
	} else if ((cc = sb_setcore(sbh, SB_CC, 0))) {
		/* Set timing for the flash */
		tmp = CEIL(10, ns) << FW_W3_SHIFT;	/* W3 = 10nS */
		tmp |= CEIL(10, ns) << FW_W1_SHIFT;	/* W1 = 10nS */
		tmp |= CEIL(120, ns);			/* W0 = 120nS */
		W_REG(&cc->flash_waitcount, tmp);

		W_REG(&cc->pcmcia_memwait, tmp);
	}

	/* Chip specific initialization */
	switch (sb_chip(sbh)) {
	case BCM4710_DEVICE_ID:
		/* Clear interrupt map */
		for (irq = 0; irq <= 4; irq++)
			sb_clearirq(sbh, irq);
		sb_setirq(sbh, 0, SB_CODEC, 0);
		sb_setirq(sbh, 0, SB_EXTIF, 0);
		sb_setirq(sbh, 2, SB_ENET, 1);
		sb_setirq(sbh, 3, SB_ILINE20, 0);
		sb_setirq(sbh, 4, SB_PCI, 0);
		ASSERT(eir);
		value = nvram_get("et0phyaddr");
		if (value && !strcmp(value, "31")) {
			/* Enable internal UART */
			W_REG(&eir->corecontrol, CC_UE);
			/* Give USB its own interrupt */
			sb_setirq(sbh, 1, SB_USB, 0);
		} else {
			/* Disable internal UART */
			W_REG(&eir->corecontrol, 0);
			/* Give Ethernet its own interrupt */
			sb_setirq(sbh, 1, SB_ENET, 0);
			sb_setirq(sbh, 0, SB_USB, 0);
		}
		break;
	case BCM4310_DEVICE_ID:
		MTC0(C0_BROADCOM, 0, MFC0(C0_BROADCOM, 0) & ~(1 << 22));
		break;
	}
}

uint32
sb_mips_clock(void *sbh)
{
	extifregs_t *eir;
	chipcregs_t *cc;
	uint32 n, m;
	uint idx;
	uint32 pll_type, rate = 0;

	/* get index of the current core */
	idx = sb_coreidx(sbh);
	pll_type = PLL_TYPE1;

	/* switch to extif or chipc core */
	if ((eir = (extifregs_t *) sb_setcore(sbh, SB_EXTIF, 0))) {
		n = R_REG(&eir->clockcontrol_n);
		m = R_REG(&eir->clockcontrol_sb);
	} else if ((cc = (chipcregs_t *) sb_setcore(sbh, SB_CC, 0))) {
		pll_type = R_REG(&cc->capabilities) & CAP_PLL_MASK;
		n = R_REG(&cc->clockcontrol_n);
		if ((pll_type == PLL_TYPE2) || (pll_type == PLL_TYPE4))
			m = R_REG(&cc->clockcontrol_mips);
		else if (pll_type == PLL_TYPE3) {
			rate = 200000000;
			goto out;
		} else
			m = R_REG(&cc->clockcontrol_sb);
	} else
		goto out;

	/* calculate rate */
	rate = sb_clock_rate(pll_type, n, m);

out:
	/* switch back to previous core */
	sb_setcoreidx(sbh, idx);

	return rate;
}

static void
icache_probe(int *size, int *lsize)
{
	uint32 config1;
	uint sets, ways;

	config1 = MFC0(C0_CONFIG, 1);

	/* Instruction Cache Size = Associativity * Line Size * Sets Per Way */
	if ((*lsize = ((config1 >> 19) & 7)))
		*lsize = 2 << *lsize;
	sets = 64 << ((config1 >> 22) & 7);
	ways = 1 + ((config1 >> 16) & 7);
	*size = *lsize * sets * ways;
}

#define ALLINTS (IE_IRQ0 | IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4)

static void
handler(void)
{
	/* Step 11 */
	__asm__ (
		".set\tmips32\n\t"
		"ssnop\n\t"
		"ssnop\n\t"
	/* Disable interrupts */
	/*	MTC0(C0_STATUS, 0, MFC0(C0_STATUS, 0) & ~(ALLINTS | STO_IE)); */
		"mfc0 $15, $12\n\t"
		"and $15, $15, -31746\n\t"
		"mtc0 $15, $12\n\t"
		"eret\n\t"
		"nop\n\t"
		"nop\n\t"
		".set\tmips0"
	);
}

/* The following MUST come right after handler() */
static void
afterhandler(void)
{
}

/*
 * Set the MIPS, backplane and PCI clocks as closely as possible.
 */
bool
sb_mips_setclock(void *sbh, uint32 mipsclock, uint32 sbclock, uint32 pciclock)
{
	extifregs_t *eir = NULL;
	chipcregs_t *cc = NULL;
	mipsregs_t *mipsr = NULL;
	volatile uint32 *clockcontrol_n, *clockcontrol_sb, *clockcontrol_pci;
	uint32 orig_n, orig_sb, orig_pci, orig_m2, orig_mips, orig_ratio_parm, new_ratio;
	uint32 pll_type, sync_mode;
	uint idx, i;
	typedef struct {
		uint32 mipsclock;
		uint16 n;
		uint32 sb;
		uint32 pci33;
		uint32 pci25;
	} n3m_table_t;
	static n3m_table_t type1_table[] = {
		{  96000000, 0x0303, 0x04020011, 0x11030011, 0x11050011 }, /*  96.000 32.000 24.000 */
		{ 100000000, 0x0009, 0x04020011, 0x11030011, 0x11050011 }, /* 100.000 33.333 25.000 */
		{ 104000000, 0x0802, 0x04020011, 0x11050009, 0x11090009 }, /* 104.000 31.200 24.960 */
		{ 108000000, 0x0403, 0x04020011, 0x11050009, 0x02000802 }, /* 108.000 32.400 24.923 */
		{ 112000000, 0x0205, 0x04020011, 0x11030021, 0x02000403 }, /* 112.000 32.000 24.889 */
		{ 115200000, 0x0303, 0x04020009, 0x11030011, 0x11050011 }, /* 115.200 32.000 24.000 */
		{ 120000000, 0x0011, 0x04020011, 0x11050011, 0x11090011 }, /* 120.000 30.000 24.000 */
		{ 124800000, 0x0802, 0x04020009, 0x11050009, 0x11090009 }, /* 124.800 31.200 24.960 */
		{ 128000000, 0x0305, 0x04020011, 0x11050011, 0x02000305 }, /* 128.000 32.000 24.000 */
		{ 132000000, 0x0603, 0x04020011, 0x11050011, 0x02000305 }, /* 132.000 33.000 24.750 */
		{ 136000000, 0x0c02, 0x04020011, 0x11090009, 0x02000603 }, /* 136.000 32.640 24.727 */
		{ 140000000, 0x0021, 0x04020011, 0x11050021, 0x02000c02 }, /* 140.000 30.000 24.706 */
		{ 144000000, 0x0405, 0x04020011, 0x01020202, 0x11090021 }, /* 144.000 30.857 24.686 */
		{ 150857142, 0x0605, 0x04020021, 0x02000305, 0x02000605 }, /* 150.857 33.000 24.000 */
		{ 152000000, 0x0e02, 0x04020011, 0x11050021, 0x02000e02 }, /* 152.000 32.571 24.000 */
		{ 156000000, 0x0802, 0x04020005, 0x11050009, 0x11090009 }, /* 156.000 31.200 24.960 */
		{ 160000000, 0x0309, 0x04020011, 0x11090011, 0x02000309 }, /* 160.000 32.000 24.000 */
		{ 163200000, 0x0c02, 0x04020009, 0x11090009, 0x02000603 }, /* 163.200 32.640 24.727 */
		{ 168000000, 0x0205, 0x04020005, 0x11030021, 0x02000403 }, /* 168.000 32.000 24.889 */
		{ 176000000, 0x0602, 0x04020003, 0x11050005, 0x02000602 }, /* 176.000 33.000 24.000 */
	};
	typedef struct {
		uint32 mipsclock;
		uint32 sbclock;
		uint16 n;
		uint32 sb;
		uint32 pci33;
		uint32 m2;
		uint32 m3;
		uint32 ratio;
		uint32 ratio_parm;
	} n4m_table_t;

	static n4m_table_t type2_table[] = {
		{ 180000000,  80000000, 0x0403, 0x01010000, 0x01020300, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 180000000,  90000000, 0x0403, 0x01000100, 0x01020300, 0x01000100, 0x05000100, 0x21, 0x0aaa0555 },
		{ 200000000, 100000000, 0x0303, 0x01000000, 0x01000600, 0x01000000, 0x05000000, 0x21, 0x0aaa0555 },
		{ 211200000, 105600000, 0x0902, 0x01000200, 0x01030400, 0x01000200, 0x05000200, 0x21, 0x0aaa0555 },
		{ 220800000, 110400000, 0x1500, 0x01000200, 0x01030400, 0x01000200, 0x05000200, 0x21, 0x0aaa0555 },
		{ 230400000, 115200000, 0x0604, 0x01000200, 0x01020600, 0x01000200, 0x05000200, 0x21, 0x0aaa0555 },
		{ 234000000, 104000000, 0x0b01, 0x01010000, 0x01010700, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 240000000, 120000000,	0x0803,	0x01000200, 0x01020600,	0x01000200, 0x05000200, 0x21, 0x0aaa0555 },
		{ 252000000, 126000000,	0x0504,	0x01000100, 0x01020500,	0x01000100, 0x05000100, 0x21, 0x0aaa0555 },
		{ 264000000, 132000000, 0x0903, 0x01000200, 0x01020700, 0x01000200, 0x05000200, 0x21, 0x0aaa0555 },
		{ 270000000, 120000000, 0x0703, 0x01010000, 0x01030400, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 276000000, 122666666, 0x1500, 0x01010000, 0x01030400, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 280000000, 140000000, 0x0503, 0x01000000, 0x01010600, 0x01000000, 0x05000000, 0x21, 0x0aaa0555 },
		{ 288000000, 128000000, 0x0604, 0x01010000, 0x01030400, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 288000000, 144000000, 0x0404, 0x01000000, 0x01010600, 0x01000000, 0x05000000, 0x21, 0x0aaa0555 },
		{ 300000000, 133333333, 0x0803, 0x01010000, 0x01020600, 0x01020600, 0x05000100, 0x94, 0x012a0115 },
		{ 300000000, 150000000, 0x0803, 0x01000100, 0x01020600, 0x01000100, 0x05000100, 0x21, 0x0aaa0555 }
	};

	static n4m_table_t type4_table[] = {
		{ 192000000,  96000000, 0x0702,	0x04020011, 0x11030011, 0x04020011, 0x04020003, 0x21, 0x0aaa0555 },
		{ 200000000, 100000000, 0x0009,	0x04020011, 0x11030011, 0x04020011, 0x04020003, 0x21, 0x0aaa0555 },
		{ 216000000, 108000000, 0x0111, 0x11020005, 0x01030303, 0x11020005, 0x04000005, 0x21, 0x0aaa0555 },
		{ 228000000, 101333333, 0x0e02, 0x11030003, 0x11210005, 0x11030305, 0x04000005, 0x94, 0x012a00a9 },
		{ 228000000, 114000000, 0x0e02, 0x11020005, 0x11210005, 0x11020005, 0x04000005, 0x21, 0x0aaa0555 },
		{ 240000000, 120000000,	0x0109,	0x11030002, 0x01050203,	0x11030002, 0x04000003, 0x21, 0x0aaa0555 },
		{ 252000000, 126000000,	0x0203,	0x04000005, 0x11050005,	0x04000005, 0x04000002, 0x21, 0x0aaa0555 },
		{ 264000000, 132000000, 0x0602, 0x04000005, 0x11050005, 0x04000005, 0x04000002, 0x21, 0x0aaa0555 },
		{ 272000000, 116571428, 0x0c02, 0x04000021, 0x02000909, 0x02000221, 0x04000003, 0x73, 0x254a14a9 },
		{ 280000000, 120000000, 0x0209, 0x04000021, 0x01030303, 0x02000221, 0x04000003, 0x73, 0x254a14a9 },
		{ 288000000, 123428571, 0x0111, 0x04000021, 0x01030303, 0x02000221, 0x04000003, 0x73, 0x254a14a9 },
		{ 300000000, 120000000, 0x0009, 0x04000009, 0x01030203, 0x02000902, 0x04000002, 0x52, 0x02520129 }
	};
	uint icache_size, ic_lsize;
	ulong start, end, dst;
	bool ret = FALSE;

	/* get index of the current core */
	idx = sb_coreidx(sbh);

	/* switch to extif or chipc core */
	if ((eir = (extifregs_t *) sb_setcore(sbh, SB_EXTIF, 0))) {
		pll_type = PLL_TYPE1;
		clockcontrol_n = &eir->clockcontrol_n;
		clockcontrol_sb = &eir->clockcontrol_sb;
		clockcontrol_pci = &eir->clockcontrol_pci;
	} else if ((cc = (chipcregs_t *) sb_setcore(sbh, SB_CC, 0))) {
		pll_type = R_REG(&cc->capabilities) & CAP_PLL_MASK;
		clockcontrol_n = &cc->clockcontrol_n;
		clockcontrol_sb = &cc->clockcontrol_sb;
		clockcontrol_pci = &cc->clockcontrol_pci;
	} else
		goto done;

	/* Store the current clock register values */
	orig_n = R_REG(clockcontrol_n);
	orig_sb = R_REG(clockcontrol_sb);
	orig_pci = R_REG(clockcontrol_pci);

	if (pll_type == PLL_TYPE1) {
		/* Keep the current PCI clock if not specified */
		if (pciclock == 0) {
			pciclock = sb_clock_rate(pll_type, R_REG(clockcontrol_n), R_REG(clockcontrol_pci));
			pciclock = (pciclock <= 25000000) ? 25000000 : 33000000;
		}

		/* Search for the closest MIPS clock less than or equal to a preferred value */
		for (i = 0; i < ARRAYSIZE(type1_table); i++) {
			ASSERT(type1_table[i].mipsclock ==
			       sb_clock_rate(pll_type, type1_table[i].n, type1_table[i].sb));
			if (type1_table[i].mipsclock > mipsclock)
				break;
		}
		if (i == 0) {
			ret = FALSE;
			goto done;
		} else {
			ret = TRUE;
			i--;
		}
		ASSERT(type1_table[i].mipsclock <= mipsclock);

		/* No PLL change */
		if ((orig_n == type1_table[i].n) &&
		    (orig_sb == type1_table[i].sb) &&
		    (orig_pci == type1_table[i].pci33))
			goto done;

		/* Set the PLL controls */
		W_REG(clockcontrol_n, type1_table[i].n);
		W_REG(clockcontrol_sb, type1_table[i].sb);
		if (pciclock == 25000000)
			W_REG(clockcontrol_pci, type1_table[i].pci25);
		else
			W_REG(clockcontrol_pci, type1_table[i].pci33);

		/* Reset */
		sb_watchdog(sbh, 1);
		while (1);
	} else if ((pll_type == PLL_TYPE2) || (pll_type == PLL_TYPE4)) {
		n4m_table_t *table = (pll_type == PLL_TYPE2) ? type2_table : type4_table;
		uint tabsz = (pll_type == PLL_TYPE2) ? ARRAYSIZE(type2_table) : ARRAYSIZE(type4_table);

		ASSERT(cc);

		/* Store the current clock register values */
		orig_m2 = R_REG(&cc->clockcontrol_m2);
		orig_mips = R_REG(&cc->clockcontrol_mips);
		orig_ratio_parm = 0;

		/* Look up current ratio */
		for (i = 0; i < tabsz; i++) {
			if ((orig_n == table[i].n) &&
			    (orig_sb == table[i].sb) &&
			    (orig_pci == table[i].pci33) &&
			    (orig_m2 == table[i].m2) &&
			    (orig_mips == table[i].m3)) {
				orig_ratio_parm = table[i].ratio_parm;
				break;
			}
		}

		/* Search for the closest MIPS clock greater or equal to a preferred value */
		for (i = 0; i < tabsz; i++) {
			ASSERT(table[i].mipsclock ==
			       sb_clock_rate(pll_type, table[i].n, table[i].m3));
			if ((mipsclock <= table[i].mipsclock) &&
			    ((sbclock == 0) || (sbclock <= table[i].sbclock)))
				break;
		}
		if (i == tabsz) {
			ret = FALSE;
			goto done;
		} else {
			ret = TRUE;
		}

		/* No PLL change */
		if ((orig_n == table[i].n) &&
		    (orig_sb == table[i].sb) &&
		    (orig_pci == table[i].pci33) &&
		    (orig_m2 == table[i].m2) &&
		    (orig_mips == table[i].m3))
			goto done;

		/* Set the PLL controls */
		W_REG(clockcontrol_n, table[i].n);
		W_REG(clockcontrol_sb, table[i].sb);
		W_REG(clockcontrol_pci, table[i].pci33);
		W_REG(&cc->clockcontrol_m2, table[i].m2);
		W_REG(&cc->clockcontrol_mips, table[i].m3);

		/* No ratio change */
		if (orig_ratio_parm == table[i].ratio_parm)
			goto end_fill;

		new_ratio = table[i].ratio_parm;

		icache_probe(&icache_size, &ic_lsize);

		/* Preload the code into the cache */
		start = ((ulong) &&start_fill) & ~(ic_lsize - 1);
		end = ((ulong) &&end_fill + (ic_lsize - 1)) & ~(ic_lsize - 1);
		while (start < end) {
			cache_unroll(start, Fill_I);
			start += ic_lsize;
		}

		/* Copy the handler */
		start = (ulong) &handler;
		end = (ulong) &afterhandler;
		dst = KSEG1ADDR(0x180);
		for (i = 0; i < (end - start); i += 4)
			*((ulong *)(dst + i)) = *((ulong *)(start + i));
		
		/* Preload handler into the cache one line at a time */
		for (i = 0; i < (end - start); i += 4)
			cache_unroll(dst + i, Fill_I);

		/* Clear BEV bit */
		MTC0(C0_STATUS, 0, MFC0(C0_STATUS, 0) & ~ST0_BEV);

		/* Enable interrupts */
		MTC0(C0_STATUS, 0, MFC0(C0_STATUS, 0) | (ALLINTS | ST0_IE));

		/* Enable MIPS timer interrupt */
		if (!(mipsr = sb_setcore(sbh, SB_MIPS, 0)) &&
		    !(mipsr = sb_setcore(sbh, SB_MIPS33, 0)))
			ASSERT(mipsr);
		W_REG(&mipsr->intmask, 1);

	start_fill:
		/* step 1, set clock ratios */
		MTC0(C0_BROADCOM, 3, new_ratio);
		MTC0(C0_BROADCOM, 1, 8);

		/* step 2: program timer intr */
		W_REG(&mipsr->timer, 100);
		(void) R_REG(&mipsr->timer);

		/* step 3, switch to async */
		sync_mode = MFC0(C0_BROADCOM, 4);
		MTC0(C0_BROADCOM, 4, 1 << 22);

		/* step 4, set cfg active */
		MTC0(C0_BROADCOM, 2, 0x9);


		/* steps 5 & 6 */ 
		__asm__ __volatile__ (
			".set\tmips3\n\t"
			"wait\n\t"
			".set\tmips0"
		);

		/* step 7, clear cfg_active */
		MTC0(C0_BROADCOM, 2, 0);
		
		/* Additional Step: set back to orig sync mode */
		MTC0(C0_BROADCOM, 4, sync_mode);

		/* step 8, fake soft reset */
		MTC0(C0_BROADCOM, 5, MFC0(C0_BROADCOM, 5) | 4);

	end_fill:
		/* step 9 set watchdog timer */
		sb_watchdog(sbh, 20);
		(void) R_REG(&cc->chipid);

		/* step 11 */
		__asm__ __volatile__ (
			".set\tmips3\n\t"
			"sync\n\t"
			"wait\n\t"
			".set\tmips0"
		);
		while (1);
	}

done:
	/* switch back to previous core */
	sb_setcoreidx(sbh, idx);

	return ret;
}


/* returns the ncdl value to be programmed into sdram_ncdl for calibration */
uint32
sb_memc_get_ncdl(void *sbh)
{
	sbmemcregs_t *memc;
	uint32 ret = 0;
	uint32 config, rd, wr, misc, dqsg, cd, sm, sd;
	uint idx, rev;

	idx = sb_coreidx(sbh);

	memc = (sbmemcregs_t *)sb_setcore(sbh, SB_MEMC, 0);
	if (memc == 0)
		goto out;

	rev = sb_corerev(sbh);

	config = R_REG(&memc->config);
	wr = R_REG(&memc->wrncdlcor);
	rd = R_REG(&memc->rdncdlcor);
	misc = R_REG(&memc->miscdlyctl);
	dqsg = R_REG(&memc->dqsgatencdl);

	rd &= MEMC_RDNCDLCOR_RD_MASK;
	wr &= MEMC_WRNCDLCOR_WR_MASK; 
	dqsg &= MEMC_DQSGATENCDL_G_MASK;

	if (config & MEMC_CONFIG_DDR) {
		ret = (wr << 16) | (rd << 8) | dqsg;
	} else {
		if (rev > 0)
			cd = rd;
		else
			cd = (rd == MEMC_CD_THRESHOLD) ? rd : (wr + MEMC_CD_THRESHOLD);
		sm = (misc & MEMC_MISC_SM_MASK) >> MEMC_MISC_SM_SHIFT;
		sd = (misc & MEMC_MISC_SD_MASK) >> MEMC_MISC_SD_SHIFT;
		ret = (sm << 16) | (sd << 8) | cd;
	}

out:
	/* switch back to previous core */
	sb_setcoreidx(sbh, idx);

	return ret;
}
