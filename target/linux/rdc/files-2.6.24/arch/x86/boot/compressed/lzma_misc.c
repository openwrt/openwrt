/*
 * lzma_misc.c
 * 
 * malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 * puts by Nick Holloway 1993, better puts by Martin Mares 1995
 * High loaded stuff by Hans Lermen & Werner Almesberger, Feb. 1996
 * 
 * Decompress LZMA compressed vmlinuz 
 * Version 0.9 Copyright (c) Ming-Ching Tiew mctiew@yahoo.com
 * Program adapted from misc.c for 2.6 kernel
 * Forward ported to latest 2.6 version of misc.c by
 * Felix Fietkau <nbd@openwrt.org>
 */

#undef CONFIG_PARAVIRT
#include <linux/linkage.h>
#include <linux/vmalloc.h>
#include <linux/screen_info.h>
#include <linux/console.h>
#include <linux/string.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/boot.h>

/* WARNING!!
 * This code is compiled with -fPIC and it is relocated dynamically
 * at run time, but no relocation processing is performed.
 * This means that it is not safe to place pointers in static structures.
 */

/*
 * Getting to provable safe in place decompression is hard.
 * Worst case behaviours need to be analized.
 * Background information:
 *
 * The file layout is:
 *    magic[2]
 *    method[1]
 *    flags[1]
 *    timestamp[4]
 *    extraflags[1]
 *    os[1]
 *    compressed data blocks[N]
 *    crc[4] orig_len[4]
 *
 * resulting in 18 bytes of non compressed data overhead.
 *
 * Files divided into blocks
 * 1 bit (last block flag)
 * 2 bits (block type)
 *
 * 1 block occurs every 32K -1 bytes or when there 50% compression has been achieved.
 * The smallest block type encoding is always used.
 *
 * stored:
 *    32 bits length in bytes.
 *
 * fixed:
 *    magic fixed tree.
 *    symbols.
 *
 * dynamic:
 *    dynamic tree encoding.
 *    symbols.
 *
 *
 * The buffer for decompression in place is the length of the
 * uncompressed data, plus a small amount extra to keep the algorithm safe.
 * The compressed data is placed at the end of the buffer.  The output
 * pointer is placed at the start of the buffer and the input pointer
 * is placed where the compressed data starts.  Problems will occur
 * when the output pointer overruns the input pointer.
 *
 * The output pointer can only overrun the input pointer if the input
 * pointer is moving faster than the output pointer.  A condition only
 * triggered by data whose compressed form is larger than the uncompressed
 * form.
 *
 * The worst case at the block level is a growth of the compressed data
 * of 5 bytes per 32767 bytes.
 *
 * The worst case internal to a compressed block is very hard to figure.
 * The worst case can at least be boundined by having one bit that represents
 * 32764 bytes and then all of the rest of the bytes representing the very
 * very last byte.
 *
 * All of which is enough to compute an amount of extra data that is required
 * to be safe.  To avoid problems at the block level allocating 5 extra bytes
 * per 32767 bytes of data is sufficient.  To avoind problems internal to a block
 * adding an extra 32767 bytes (the worst case uncompressed block size) is
 * sufficient, to ensure that in the worst case the decompressed data for
 * block will stop the byte before the compressed data for a block begins.
 * To avoid problems with the compressed data's meta information an extra 18
 * bytes are needed.  Leading to the formula:
 *
 * extra_bytes = (uncompressed_size >> 12) + 32768 + 18 + decompressor_size.
 *
 * Adding 8 bytes per 32K is a bit excessive but much easier to calculate.
 * Adding 32768 instead of 32767 just makes for round numbers.
 * Adding the decompressor_size is necessary as it musht live after all
 * of the data as well.  Last I measured the decompressor is about 14K.
 * 10K of actuall data and 4K of bss.
 *
 */

/*
 * gzip declarations
 */

#define OF(args)  args
#define STATIC static

#undef memcpy

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#define WSIZE 0x80000000	/* Window size must be at least 32k,
				 * and a power of two
				 * We don't actually have a window just
				 * a huge output buffer so I report
				 * a 2G windows size, as that should
				 * always be larger than our output buffer.
				 */

static uch *inbuf;	/* input buffer */
static uch *window;	/* Sliding window buffer, (and final output buffer) */

static unsigned insize;  /* valid bytes in inbuf */
static unsigned inptr;   /* index of next byte to be processed in inbuf */
static unsigned long workspace;

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())
		
/* Diagnostic functions */
#ifdef DEBUG
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

static int  fill_inbuf(void);
  
/*
 * This is set up by the setup-routine at boot-time
 */
static unsigned char *real_mode; /* Pointer to real-mode data */
extern unsigned char input_data[];
extern int input_len;

static void error(char *x);
static void *memcpy(void *dest, const void *src, unsigned n);

#ifdef CONFIG_X86_NUMAQ
void *xquad_portio;
#endif

static void* memcpy(void* dest, const void* src, unsigned n)
{
	int i;
	char *d = (char *)dest, *s = (char *)src;

	for (i=0;i<n;i++) d[i] = s[i];
	return dest;
}

/* ===========================================================================
 * Fill the input buffer. This is called only when the buffer is empty
 * and at least one byte is really needed.
 */
static int fill_inbuf(void)
{
	error("ran out of input data");
	return 0;
}


// When using LZMA in callback, the compressed length is not needed.
// Otherwise you need a special version of lzma compression program
// which will pad the compressed length in the header.
#define _LZMA_IN_CB
#include "LzmaDecode.h"
#include "LzmaDecode.c"

static int read_byte(void *object, unsigned char **buffer, UInt32 *bufferSize);

static int early_serial_base = 0x3f8;  /* ttyS0 */

#define XMTRDY          0x20

#define DLAB            0x80

#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */
#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define FCR             2       /*  FIFO control              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */
#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */

static int early_serial_putc(unsigned char ch)
{
        unsigned timeout = 0xffff;
        while ((inb(early_serial_base + LSR) & XMTRDY) == 0 && --timeout)
                cpu_relax();
        outb(ch, early_serial_base + TXR);
        return timeout ? 0 : -1;
}

static void early_serial_write(const char *s, unsigned n)
{
        while (*s && n-- > 0) {
                if (*s == '\n')
                        early_serial_putc('\r');
                early_serial_putc(*s);
                s++;
        }
}

#define DEFAULT_BAUD 38400

static __init void early_serial_init(void)
{
        unsigned char c;
        unsigned divisor;
        unsigned baud = DEFAULT_BAUD;
        char *e;

        outb(0x3, early_serial_base + LCR);     /* 8n1 */
        outb(0, early_serial_base + IER);       /* no interrupt */
        outb(0, early_serial_base + FCR);       /* no fifo */
        outb(0x3, early_serial_base + MCR);     /* DTR + RTS */

	baud = DEFAULT_BAUD;

        divisor = 115200 / baud;
        c = inb(early_serial_base + LCR);
        outb(c | DLAB, early_serial_base + LCR);
        outb(divisor & 0xff, early_serial_base + DLL);
        outb((divisor >> 8) & 0xff, early_serial_base + DLH);
        outb(c & ~DLAB, early_serial_base + LCR);
}

/*
 * Do the lzma decompression
 * When using LZMA in callback, the end of input stream is automatically determined
 */
static int lzma_unzip(void)
{

	unsigned int i;  /* temp value */
	unsigned int lc; /* literal context bits */
	unsigned int lp; /* literal pos state bits */
	unsigned int pb; /* pos state bits */
	unsigned int uncompressedSize = 0;
	unsigned char* p;
	
	ILzmaInCallback callback;
	callback.Read = read_byte;

	/* lzma args */
	i = get_byte();
	lc = i % 9, i = i / 9;
	lp = i % 5, pb = i / 5;
	
	/* skip dictionary size */
	for (i = 0; i < 4; i++) 
		get_byte();
	// get uncompressedSize 	
	p= (char*)&uncompressedSize;	
	for (i = 0; i < 4; i++) 
	    *p++ = get_byte();
	    
	//get compressedSize 
	for (i = 0; i < 4; i++) 
		get_byte();
	
	// point it beyond uncompresedSize
	//workspace = window + uncompressedSize;
	
	/* decompress kernel */
	if (LzmaDecode((unsigned char*)workspace, ~0, lc, lp, pb, &callback,
		(unsigned char*)window, uncompressedSize, &i) == LZMA_RESULT_OK)
		return 0;
	else
		return 1;
}


#ifdef  _LZMA_IN_CB
static int read_byte(void *object, unsigned char **buffer, UInt32 *bufferSize)
{
	static unsigned int i = 0;
	static unsigned char val;
	*bufferSize = 1;
	val = get_byte();
	*buffer = &val;
	return LZMA_RESULT_OK;
}	
#endif

static void error(char *x)
{
	while(1);	/* Halt */
}

asmlinkage void decompress_kernel(void *rmode, unsigned long end,
			uch *input_data, unsigned long input_len, uch *output)
{
	real_mode = rmode;

	window = output;
	inbuf  = input_data;	/* Input buffer */
	insize = input_len;
	inptr  = 0;

	if ((u32)output & (CONFIG_PHYSICAL_ALIGN -1))
		error("Destination address not CONFIG_PHYSICAL_ALIGN aligned");
	if ((workspace = end) > ((-__PAGE_OFFSET-(512 <<20)-1) & 0x7fffffff))
		error("Destination address too large");
#ifndef CONFIG_RELOCATABLE
	if ((u32)output != LOAD_PHYSICAL_ADDR)
		error("Wrong destination address");
#endif
	early_serial_init();
	early_serial_write("Uncompressing Linux\n", 512);
	lzma_unzip();
	early_serial_write("Done, booting\n", 512);
	return;
}
