/*
 * LZMA compressed kernel decompressor for ADM5120 boards
 *
 * Copyright (C) 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
 * Copyright (C) 2007 OpenWrt.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 * Please note, this was code based on the bunzip2 decompressor code
 * by Manuel Novoa III  (mjn3@codepoet.org), although the only thing left
 * is an idea and part of original vendor code
 *
 *
 * 12-Mar-2005  Mineharu Takahara <mtakahar@yahoo.com>
 *   pass actual output size to decoder (stream mode
 *   compressed input is not a requirement anymore)
 *
 * 24-Apr-2005 Oleg I. Vdovikin
 *   reordered functions using lds script, removed forward decl
 *
 * 24-Mar-2007 Gabor Juhos
 *   pass original values of the a0,a1,a2,a3 registers to the kernel
 *
 * 19-May-2007 Gabor Juhos
 *   endiannes related cleanups
 *   add support for decompressing an embedded kernel 
 *
 */

#include <stddef.h>

#include "LzmaDecode.h"

#define ADM5120_FLASH_START	0x1fc00000	/* Flash start */
#define ADM5120_FLASH_END	0x1fe00000	/* Flash end */

#define KSEG0			0x80000000
#define KSEG1			0xa0000000

#define KSEG1ADDR(a)		((((unsigned)(a)) & 0x1fffffffU) | KSEG1)

#define Index_Invalidate_I	0x00
#define Index_Writeback_Inv_D   0x01

#define cache_unroll(base,op)	\
	__asm__ __volatile__(		\
		".set noreorder;\n"		\
		".set mips3;\n"			\
		"cache %1, (%0);\n"		\
		".set mips0;\n"			\
		".set reorder\n"		\
		:				\
		: "r" (base),			\
		  "i" (op));

static __inline__ void blast_icache(unsigned long size, unsigned long lsize)
{
	unsigned long start = KSEG0;
	unsigned long end = (start + size);

	while(start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += lsize;
	}
}

static __inline__ void blast_dcache(unsigned long size, unsigned long lsize)
{
	unsigned long start = KSEG0;
	unsigned long end = (start + size);

	while(start < end) {
		cache_unroll(start,Index_Writeback_Inv_D);
		start += lsize;
	}
}

#define TRX_MAGIC       0x30524448      /* "HDR0" */
#define TRX_ALIGN	0x1000

struct trx_header {
	unsigned int magic;		/* "HDR0" */
	unsigned int len;		/* Length of file including header */
	unsigned int crc32;		/* 32-bit CRC from flag_version to end of file */
	unsigned int flag_version;	/* 0:15 flags, 16:31 version */
	unsigned int offsets[3];	/* Offsets of partitions from start of header */
};

/* beyound the image end, size not known in advance */
extern unsigned char workspace[];
#if LZMA_WRAPPER
extern unsigned char _lzma_data_start[];
extern unsigned char _lzma_data_end[];
#endif

extern void board_init(void);
extern void board_putc(int ch);

unsigned char *data;
unsigned long datalen;

typedef void (*kernel_entry)(unsigned long reg_a0, unsigned long reg_a1,
	unsigned long reg_a2, unsigned long reg_a3);

static int read_byte(void *object, unsigned char **buffer, UInt32 *bufferSize)
{
	*bufferSize = 1;
	*buffer = data++;

	return LZMA_RESULT_OK;
}

static __inline__ unsigned char get_byte(void)
{
	unsigned char *buffer;
	UInt32 fake;

	read_byte(0, &buffer, &fake);
	return *buffer;
}

static __inline__ unsigned int read_le32(void *buf)
{
	unsigned char *p;

	p = buf;
	return ((unsigned int)p[0] + ((unsigned int)p[1] << 8) +
		((unsigned int)p[2] << 16) +((unsigned int)p[3] << 24));
}

static void print_char(char ch)
{
	if (ch == '\n')
		board_putc('\r');
    	board_putc(ch);
}

static void print_str(char * str)
{
	while ( *str != 0 )
    		print_char(*str++);
}

static void print_hex(int val)
{
	int i;
	int tmp;

	print_str("0x");
	for ( i=0 ; i<8 ; i++ ) {
    		tmp = (val >> ((7-i) * 4 )) & 0xf;
    		tmp = tmp < 10 ? (tmp + '0') : (tmp + 'A' - 10);
    		board_putc(tmp);
	}
}

static unsigned char *find_kernel(void)
{
	struct trx_header *hdr;
	unsigned char *ret;

    	print_str("Looking for TRX header... ");
	/* look for trx header, 32-bit data access */
	hdr = NULL;
	for (ret = ((unsigned char *) KSEG1ADDR(ADM5120_FLASH_START));
		ret < ((unsigned char *)KSEG1ADDR(ADM5120_FLASH_END));
		ret += TRX_ALIGN) {
		
		if (read_le32(ret) == TRX_MAGIC) {
			hdr = (struct trx_header *)ret;
			break;
		}
	}

	if (hdr == NULL) {
		print_str("not found!\n");
		return NULL;	
	}

	print_str("found at ");
	print_hex((unsigned int)ret);
	print_str(", kernel in partition ");
	
	/* compressed kernel is in the partition 0 or 1 */
	if ((read_le32(&hdr->offsets[1]) == 0) ||
		(read_le32(&hdr->offsets[1]) > 65536)) {
		ret += read_le32(&hdr->offsets[0]);
		print_str("0\n");
	} else {
		ret += read_le32(&hdr->offsets[1]);
		print_str("1\n");
	}
		
	return ret;
}

static void halt(void)
{
	print_str("\nSystem halted!\n");
	for(;;);
}

/* should be the first function */
void decompress_entry(unsigned long reg_a0, unsigned long reg_a1,
	unsigned long reg_a2, unsigned long reg_a3,
	unsigned long icache_size, unsigned long icache_lsize,
	unsigned long dcache_size, unsigned long dcache_lsize)
{
	unsigned int i;  /* temp value */
	unsigned int lc; /* literal context bits */
	unsigned int lp; /* literal pos state bits */
	unsigned int pb; /* pos state bits */
	unsigned int osize; /* uncompressed size */
	int res;
#if !(LZMA_WRAPPER)
	ILzmaInCallback callback;
#endif

	board_init();

	print_str("\n\nLZMA loader for ADM5120, Copyright (C) 2007 OpenWrt.org\n\n");

#if LZMA_WRAPPER
	data = _lzma_data_start;
	datalen = _lzma_data_end - _lzma_data_start;
#else
	data = find_kernel();
	if (data == NULL) {
		/* no compressed kernel found, halting */
		halt();
	}

	datalen = ((unsigned char *) KSEG1ADDR(ADM5120_FLASH_END))-data;
#endif

	/* lzma args */
	i = get_byte();
	lc = i % 9, i = i / 9;
	lp = i % 5, pb = i / 5;

	/* skip rest of the LZMA coder property */
	for (i = 0; i < 4; i++)
		get_byte();

	/* read the lower half of uncompressed size in the header */
	osize = ((unsigned int)get_byte()) +
		((unsigned int)get_byte() << 8) +
		((unsigned int)get_byte() << 16) +
		((unsigned int)get_byte() << 24);

	/* skip rest of the header (upper half of uncompressed size) */
	for (i = 0; i < 4; i++)
		get_byte();

	print_str("decompressing kernel... ");

	/* decompress kernel */
#if LZMA_WRAPPER
	res = LzmaDecode(workspace, ~0, lc, lp, pb, data, datalen,
		(unsigned char*)LOADADDR, osize, &i);
#else
	callback.Read = read_byte;
	res = LzmaDecode(workspace, ~0, lc, lp, pb, &callback,
		(unsigned char*)LOADADDR, osize, &i);
#endif
	if (res != LZMA_RESULT_OK) {
		print_str("failed!\n");
		print_str("LzmaDecode: ");
		switch (res) {
		case LZMA_RESULT_DATA_ERROR:
			print_str("data error\n");
			break;
		case LZMA_RESULT_NOT_ENOUGH_MEM:
			print_str("not enough memory\n");
			break;
		default:
			print_str("unknown error, err=0x");
			print_hex(res);
			print_str("\n");
		}
		halt();
	}

        print_str("done!\n");

	blast_dcache(dcache_size, dcache_lsize);
	blast_icache(icache_size, icache_lsize);

	print_str("launching kernel...\n\n");

	/* Jump to load address */
	((kernel_entry) LOADADDR)(reg_a0, reg_a1, reg_a2, reg_a3);
}


