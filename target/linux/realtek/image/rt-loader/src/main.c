/*
 * rt-loader main program
 * (c) 2025 Markus Stockhausen
 *
 * This code was inspired by the OpenWrt lzma loader. Thanks to
 *
 * Copyright (C) 2004 Manuel Novoa III (mjn3@codepoet.org)
 * Copyright (C) 2005 Mineharu Takahara <mtakahar@yahoo.com>
 * Copyright (C) 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 */

#include "board.h"
#include "globals.h"
#include "memory.h"

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS	1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS		0
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS		0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS		0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS	0
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS	0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS		0
#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"

extern void *_kernel_load_addr;
extern void *_kernel_data_addr;
extern int _kernel_data_size;
extern void *_my_load_addr;
extern int _my_load_size;

extern int unlzma(unsigned char *buf, long in_len,
	   long (*fill)(void*, unsigned long),
	   long (*flush)(void*, unsigned long),
	   unsigned char *output,
	   long *outlen,
	   long *posp,
	   void(*error)(char *x));

typedef void (*entry_func_t)(unsigned long reg_a0, unsigned long reg_a1,
			     unsigned long reg_a2, unsigned long reg_a3);

void *relocate(void *src, int len)
{
	void *addr;
	unsigned int offs;

	/*
	 * Relocate to highest possible memory address. This is usually the RAM size minus some
	 * space for the heap and the stack pointer. As we do not have any highmem features
	 * limit this to 256MB.
	 */

	offs = (board_get_memory() - STACK_SIZE - HEAP_SIZE - len - 1024) & 0xfff0000;
	addr = (void *)KSEG0 + offs;

	printf("Relocate %d bytes from 0x%08x to 0x%08x\n", len, src, addr);

	memcpy(addr, src, len);
	flush_cache(addr, len);

	return addr;
}

void welcome(void)
{
	char system[80];

	board_get_system(system, sizeof(system));

	printf("rt-loader\n");
	printf("Running on %s with %dMB\n", system, board_get_memory() >> 20);
}

void decompress_error(char *x)
{
	printf("%s\n", x);
}

void *decompress(void *out, void *in, int len)
{
	long outlen;

	printf("Extract kernel with %d bytes from 0x%08x to 0x%08x ...\n", len, in, out);

	if (unlzma(in, len, 0, 0, out, &outlen, 0, decompress_error))
		board_panic();

	printf("Extracted kernel size is %d bytes\n", outlen);
	flush_cache(out, outlen);

	return out;
}

void main(unsigned long reg_a0, unsigned long reg_a1,
	  unsigned long reg_a2, unsigned long reg_a3)
{
	entry_func_t fn;

	if (_kernel_load_addr == _my_load_addr) {
		/*
		 * During first run relocate the whole package to the end of memory. Use
		 * _my_load_size as relocation length. That includes the bss section, aka
		 * uninitialized globals. So it is possible to initialize globals during
		 * first run and have them at hand after relocation.
		 */

		welcome();
		fn = relocate(_my_load_addr, _my_load_size);
		fn(reg_a0, reg_a1, reg_a2, reg_a3);
	} else {
		/*
		 * During second run extract the attached kernel image to the memory address
		 * that the loader was loaded to in the first run.
		 */

		fn = decompress(_kernel_load_addr, _kernel_data_addr, _kernel_data_size);

		printf("Booting kernel from 0x%08x ...\n\n", fn);
		fn(reg_a0, reg_a1, reg_a2, reg_a3);
	}
}
