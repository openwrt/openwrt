/*
 * LZMA compressed kernel decompressor for bcm947xx boards
 *
 * Copyright (C) 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
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
 */

#include "LzmaDecode.h"

#define BCM4710_FLASH		0x1fc00000	/* Flash */

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
		:						\
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

struct trx_header {
	unsigned int magic;		/* "HDR0" */
	unsigned int len;		/* Length of file including header */
	unsigned int crc32;		/* 32-bit CRC from flag_version to end of file */
	unsigned int flag_version;	/* 0:15 flags, 16:31 version */
	unsigned int offsets[3];	/* Offsets of partitions from start of header */
};

/* beyound the image end, size not known in advance */
extern unsigned char workspace[];

unsigned int offset;
unsigned char *data;

typedef void (*kernel_entry)(unsigned long reg_a0, unsigned long reg_a1,
	unsigned long reg_a2, unsigned long reg_a3);

/* flash access should be aligned, so wrapper is used */
/* read byte from the flash, all accesses are 32-bit aligned */
static int read_byte(void *object, unsigned char **buffer, UInt32 *bufferSize)
{
	static unsigned int val;

	if (((unsigned int)offset % 4) == 0) {
		val = *(unsigned int *)data;
		data += 4;
	}
	
	*bufferSize = 1;
	*buffer = ((unsigned char *)&val) + (offset++ & 3);
	
	return LZMA_RESULT_OK;
}

static __inline__ unsigned char get_byte(void)
{
	unsigned char *buffer;
	UInt32 fake;
	
	return read_byte(0, &buffer, &fake), *buffer;
}

/* should be the first function */
void entry(unsigned long reg_a0, unsigned long reg_a1,
	unsigned long reg_a2, unsigned long reg_a3,
	unsigned long icache_size, unsigned long icache_lsize, 
	unsigned long dcache_size, unsigned long dcache_lsize)
{
	unsigned int i;  /* temp value */
	unsigned int lc; /* literal context bits */
	unsigned int lp; /* literal pos state bits */
	unsigned int pb; /* pos state bits */
	unsigned int osize; /* uncompressed size */

	ILzmaInCallback callback;
	callback.Read = read_byte;

    uart_write_str("decompress kernel ... ");
    
	/* look for trx header, 32-bit data access */
	for (data = ((unsigned char *) KSEG1ADDR(BCM4710_FLASH));
		((struct trx_header *)data)->magic != TRX_MAGIC; data += 65536);

	/* compressed kernel is in the partition 0 or 1 */
	if (((struct trx_header *)data)->offsets[1] > 65536) 
		data += ((struct trx_header *)data)->offsets[0];
	else
		data += ((struct trx_header *)data)->offsets[1];

	offset = 0;

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

	/* decompress kernel */
	if (LzmaDecode(workspace, ~0, lc, lp, pb, &callback,
		(unsigned char*)LOADADDR, osize, &i) == LZMA_RESULT_OK)
	{
		blast_dcache(dcache_size, dcache_lsize);
		blast_icache(icache_size, icache_lsize);

		/* Jump to load address */
        uart_write_str("ok\r\n");
		((kernel_entry) LOADADDR)(reg_a0, reg_a1, reg_a2, reg_a3);
	}
    uart_write_str("failed\r\n");
    while (1 );
}

/*  *********************************************************************
    *  
    *  ADM5120 UART driver			File: dev_adm_uart.c
    *  
    *  This is a console device driver for an ADM5120 UART
    *  
    *********************************************************************
    *
    *  Copyright 2006
    *  Compex Systems. All rights reserved.
    *
    ********************************************************************* */

#define READCSR(r)      *(volatile UInt32 *)(0xB2600000+(r))
#define WRITECSR(r,v)   *(volatile UInt32 *)(0xB2600000+(r)) = v

#define UART_DR_REG         0x00
#define UART_FR_REG         0x18
#define UART_TX_FIFO_FULL   0x20

int uart_write(int val)
{
	WRITECSR(UART_DR_REG, val);
    while ( (READCSR(UART_FR_REG) & UART_TX_FIFO_FULL) );
    return 0;
}

int uart_write_str(char * str)
{
    while ( *str != 0 ) {
        uart_write ( *str++ );
    }
    return 0;
}

int uart_write_hex(int val)
{
    int i;
    int tmp;
    
    uart_write_str("0x");
    for ( i=0 ; i<8 ; i++ ) {
        tmp = (val >> ((7-i) * 4 )) & 0xf;
        tmp = tmp < 10 ? (tmp + '0') : (tmp + 'A' - 10);
        uart_write(tmp);
    }
    uart_write_str("\r\n");
    return 0;
}

