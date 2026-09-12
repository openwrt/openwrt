// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * LZMA loader for RouterBOOT based MikroTik devices.
 *
 * Memory map, see loader.lds:
 *   0x01100000  loader code, data and bss; stack just below
 *   0x44000000  decompressed kernel
 *   0x48000000  DTB
 */

#include "LzmaDecode.h"

#define KERNEL_DST	0x44000000UL
#define DTB_ADDR	0x48000000UL

/* TLMM: GPIO_CFG(n) = 0x01000000 + n*0x1000, bit 9 enables output;
 * GPIO_IN_OUT(n) = +4, bit 1 is the output value. GPIO 20 is system-green. */
#define TLMM_BASE	0x01000000UL
#define LED_GPIO	20
#define LED_CFG		(TLMM_BASE + LED_GPIO * 0x1000)
#define LED_IO		(LED_CFG + 4)

/* Probability table for lc+lp <= 4; "lzma e" uses lc=3, lp=0. */
#define MAX_LC_LP	4
#define MAX_PROBS	(LZMA_BASE_SIZE + (LZMA_LIT_SIZE << MAX_LC_LP))

extern unsigned char _payload_start[];
extern unsigned char _payload_end[];
extern unsigned char _dtb_start[];
extern unsigned char _dtb_end[];

void jump_kernel(unsigned long dtb, unsigned long kernel) __attribute__((noreturn));

static CProb lzma_probs[MAX_PROBS];

static inline void wr32(unsigned long a, unsigned int v)
{
	*(volatile unsigned int *)a = v;
}

static inline unsigned int rd32(unsigned long a)
{
	return *(volatile unsigned int *)a;
}


/* UARTDM, blsp1_uart3. RouterBOOT has already initialised it, so there is
 * no clock or baud rate setup here.
 * The protocol follows __msm_console_write() in msm_serial.c: NCF_TX takes
 * the character count, then four packed bytes per write to TF, handshaked
 * through SR.TX_READY.
 */
#define UART_BASE		0x078b1000UL
#define UART_SR			0x08
#define UART_SR_TX_EMPTY	(1 << 3)
#define UART_SR_TX_READY	(1 << 2)
#define UART_CR			0x10
#define UART_CR_RESET_TX_RDY	0x300
#define UART_ISR		0x14
#define UART_ISR_TX_READY	(1 << 7)
#define UART_NCF_TX		0x40
#define UART_TF			0x70
#define UART_TIMEOUT		0x20000		/* never wait forever */

/* Watchdog (qcom,kpss-wdt), offsets from drivers/watchdog/qcom-wdt.c */
#define WDT_BASE		0x0b017000UL
#define WDT_RST			0x04
#define WDT_EN			0x08

static unsigned int wdt_saved;

static void uart_write(const char *s, unsigned int len)
{
	unsigned int i, j, t;

	if (!len)
		return;

	for (t = UART_TIMEOUT; t; t--) {
		if (rd32(UART_BASE + UART_SR) & UART_SR_TX_EMPTY)
			break;
		if (rd32(UART_BASE + UART_ISR) & UART_ISR_TX_READY)
			break;
	}
	wr32(UART_BASE + UART_CR, UART_CR_RESET_TX_RDY);

	wr32(UART_BASE + UART_NCF_TX, len);
	(void)rd32(UART_BASE + UART_NCF_TX);

	for (i = 0; i < len;) {
		unsigned int word = 0;

		for (j = 0; j < 4 && i + j < len; j++)
			word |= (unsigned int)(unsigned char)s[i + j] << (8 * j);

		for (t = UART_TIMEOUT; t; t--)
			if (rd32(UART_BASE + UART_SR) & UART_SR_TX_READY)
				break;

		wr32(UART_BASE + UART_TF, word);
		i += j;
	}
}

static void puts(const char *s)
{
	unsigned int n = 0;

	while (s[n])
		n++;
	uart_write(s, n);
}

static void puthex(unsigned long v)
{
	char buf[8];
	int i;

	for (i = 7; i >= 0; i--) {
		unsigned int nib = v & 0xf;

		buf[i] = nib < 10 ? '0' + nib : 'a' + nib - 10;
		v >>= 4;
	}
	uart_write(buf, 8);
}

/*
 * Watchdog: save the original state and disable it while decompressing.
 * Before jumping to the kernel it is retriggered and restored, so that a
 * hanging kernel boot still recovers the way RouterBOOT intended.
 */
static void wdt_off(void)
{
	wdt_saved = rd32(WDT_BASE + WDT_EN);
	wr32(WDT_BASE + WDT_EN, 0);
}

static void wdt_restore(void)
{
	wr32(WDT_BASE + WDT_RST, 1);		/* restart the counter */
	wr32(WDT_BASE + WDT_EN, wdt_saved);
}

static void led(int on)
{
	wr32(LED_CFG, rd32(LED_CFG) | (1u << 9));	/* Output Enable */
	wr32(LED_IO, on ? (1u << 1) : 0u);
}

static void delay(unsigned int loops)
{
	volatile unsigned int i;

	for (i = 0; i < loops; i++)
		;
}

/*
 * Identity mapping, set up purely to get cacheable memory for decompression.
 *
 * With the MMU off, ARM64 forces every data access to Device-nGnRnE, so LZMA
 * runs entirely uncached: the probability array is hit constantly, and every
 * match copy reads back from the output window. Both go straight to DRAM.
 *
 * The layout is dictated by where things physically sit:
 *
 *   0x01000000 - 0x010fffff   TLMM registers (GPIO n at + n * 0x1000)  Device
 *   0x01100000 - 0x01ffffff   this loader: code, .bss, payload, DTB    cached
 *   0x42000000 - 0x49ffffff   decompressed kernel and DTB              cached
 *   everything else                                                    Device
 *
 * Note that the loader is loaded inside the window the device tree declares
 * for TLMM, which is larger than the actual register file. The split at
 * 0x01100000 keeps the registers Device while making our own image cacheable.
 */
#define L1_ENTRIES		4
#define L2_ENTRIES		512
#define L3_ENTRIES		512

#define DESC_BLOCK		0x1UL
#define DESC_TABLE		0x3UL
#define DESC_PAGE		0x3UL
#define DESC_AF			(1UL << 10)
#define DESC_SH_INNER		(3UL << 8)
#define DESC_ATTR_DEVICE	(0UL << 2)
#define DESC_ATTR_NORMAL	(1UL << 2)

#define DEV_BLOCK(pa)		((pa) | DESC_AF | DESC_ATTR_DEVICE | DESC_BLOCK)
#define MEM_BLOCK(pa)		((pa) | DESC_AF | DESC_SH_INNER | \
				 DESC_ATTR_NORMAL | DESC_BLOCK)
#define DEV_PAGE(pa)		((pa) | DESC_AF | DESC_ATTR_DEVICE | DESC_PAGE)
#define MEM_PAGE(pa)		((pa) | DESC_AF | DESC_SH_INNER | \
				 DESC_ATTR_NORMAL | DESC_PAGE)

#define LOADER_CACHED_START	0x01100000UL
#define LOADER_CACHED_END	0x02000000UL
#define KERNEL_CACHED_START	0x42000000UL
#define KERNEL_CACHED_END	0x4a000000UL

static unsigned long l1_table[L1_ENTRIES]  __attribute__((aligned(4096)));
static unsigned long l2_low[L2_ENTRIES]    __attribute__((aligned(4096)));
static unsigned long l2_dram[L2_ENTRIES]   __attribute__((aligned(4096)));
static unsigned long l3_tlmm[L3_ENTRIES]   __attribute__((aligned(4096)));

extern int mmu_enable(unsigned long ttbr0);
extern unsigned long read_cntpct(void);
extern unsigned long read_cntfrq(void);

static void mmu_build_tables(void)
{
	unsigned long i, pa;

	/* Level 1: four 1 GB entries covering the low 4 GB. */
	l1_table[0] = (unsigned long)l2_low | DESC_TABLE;
	l1_table[1] = (unsigned long)l2_dram | DESC_TABLE;
	l1_table[2] = DEV_BLOCK(0x80000000UL);
	l1_table[3] = DEV_BLOCK(0xc0000000UL);

	/* 0x00000000 - 0x3fffffff in 2 MB blocks, Device by default. */
	for (i = 0; i < L2_ENTRIES; i++) {
		pa = i << 21;

		if (pa == 0x01000000UL)
			l2_low[i] = (unsigned long)l3_tlmm | DESC_TABLE;
		else if (pa >= LOADER_CACHED_START && pa < LOADER_CACHED_END)
			l2_low[i] = MEM_BLOCK(pa);
		else
			l2_low[i] = DEV_BLOCK(pa);
	}

	/*
	 * The 2 MB block at 0x01000000 straddles the TLMM registers and the
	 * start of our own image, so it needs 4 KB granularity.
	 */
	for (i = 0; i < L3_ENTRIES; i++) {
		pa = 0x01000000UL + (i << 12);

		if (pa >= LOADER_CACHED_START)
			l3_tlmm[i] = MEM_PAGE(pa);
		else
			l3_tlmm[i] = DEV_PAGE(pa);
	}

	/* 0x40000000 - 0x7fffffff in 2 MB blocks; only the target area cached. */
	for (i = 0; i < L2_ENTRIES; i++) {
		pa = 0x40000000UL + (i << 21);

		if (pa >= KERNEL_CACHED_START && pa < KERNEL_CACHED_END)
			l2_dram[i] = MEM_BLOCK(pa);
		else
			l2_dram[i] = DEV_BLOCK(pa);
	}
}

/*
 * Fatal error indication. The progress blinks that used to mark each step are
 * gone: they cost roughly 30 million uncached loop iterations, which is real
 * time on this loader, and serial output covers the same ground. Only the
 * failure case still blinks, because that is the one case where there may be
 * nobody watching a console.
 */
static void blink_forever(int code)
{
	int i;

	puts("[chateau-lzma] error, blink code ");
	puthex((unsigned long)code);
	puts("\r\n");

	for (;;) {
		for (i = 0; i < code; i++) {
			led(1);
			delay(400000);
			led(0);
			delay(400000);
		}
		delay(1200000);
	}
}

/*
 * OpenWrt's LzmaDecode.c disables LzmaDecodeProperties with "#if 0".
 * Reimplemented here so that changed compression parameters cannot silently
 * lead to wrong behaviour.
 */
static int lzma_props(CLzmaProperties *p, unsigned char prop0)
{
	if (prop0 >= (9 * 5 * 5))
		return LZMA_RESULT_DATA_ERROR;

	for (p->pb = 0; prop0 >= (9 * 5); p->pb++, prop0 -= (9 * 5))
		;
	for (p->lp = 0; prop0 >= 9; p->lp++, prop0 -= 9)
		;
	p->lc = prop0;

	return LZMA_RESULT_OK;
}

void main(void)
{
	CLzmaDecoderState vs;
	SizeT in_done, out_done;
	unsigned char *in = _payload_start;
	unsigned long in_size = (unsigned long)(_payload_end - _payload_start);
	unsigned long out_size = 0;
	int i;

	unsigned long t_start, t_end, freq, ms;
	int cached;

	wdt_off();

	puts("\r\n[chateau-lzma] loader @0x01100000, WDT_EN was 0x");
	puthex(wdt_saved);
	puts("\r\n");

	if (in_size <= 13)
		blink_forever(5);

	if (lzma_props(&vs.Properties, in[0]) != LZMA_RESULT_OK)
		blink_forever(6);

	if (vs.Properties.lc + vs.Properties.lp > MAX_LC_LP)
		blink_forever(7);

	/* Uncompressed size from the .lzma container (8 bytes, little endian) */
	for (i = 7; i >= 0; i--)
		out_size = (out_size << 8) | in[5 + i];

	if (!out_size || out_size > 0x02000000UL)
		blink_forever(8);

	if (KERNEL_DST + out_size > (unsigned long)_payload_start &&
	    KERNEL_DST < (unsigned long)_payload_end)
		blink_forever(9);

	/*
	 * Turn caching on before decompressing. Everything below still works if
	 * this fails, it is just slow, so a failure is reported and not fatal.
	 */
	mmu_build_tables();
	cached = mmu_enable((unsigned long)l1_table);

	puts("[chateau-lzma] unpacking 0x");
	puthex(in_size);
	puts(" -> 0x");
	puthex(out_size);
	puts(" at 0x");
	puthex(KERNEL_DST);
	puts(cached ? " (cached)\r\n" : " (UNCACHED)\r\n");

	vs.Probs = lzma_probs;

	t_start = read_cntpct();

	if (LzmaDecode(&vs,
		       in + 13, (SizeT)(in_size - 13), &in_done,
		       (unsigned char *)KERNEL_DST, (SizeT)out_size, &out_done)
	    != LZMA_RESULT_OK)
		blink_forever(10);

	t_end = read_cntpct();

	if (out_done != out_size)
		blink_forever(11);

	freq = read_cntfrq();
	ms = freq ? ((t_end - t_start) / (freq / 1000)) : 0;

	puts("[chateau-lzma] unpacked in 0x");
	puthex(ms);
	puts(" ms\r\n");

	/* Copy the DTB from our own segment to its target address. */
	{
		unsigned long n = (unsigned long)(_dtb_end - _dtb_start);
		unsigned char *d = (unsigned char *)DTB_ADDR;
		unsigned long k;

		for (k = 0; k < n; k++)
			d[k] = _dtb_start[k];
	}

	puts("[chateau-lzma] DTB magic=0x");
	puthex(__builtin_bswap32(*(volatile unsigned int *)DTB_ADDR));
	puts("\r\n[chateau-lzma] watchdog rearmed, handoff -> kernel\r\n");

	wdt_restore();
	jump_kernel(DTB_ADDR, KERNEL_DST);
}
