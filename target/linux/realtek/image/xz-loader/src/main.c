/*
 * xz-loader main program
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
#include "printf.h"
#include "xz.h"

extern void *_kernel_load_addr;
extern void *_my_load_addr;
extern int _my_load_size;
extern void *_kernel_data_addr;
extern int _kernel_data_size;
extern void *_heap_addr;
extern void *_heap_addr_max;

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

	printf("xz bootlader\n");
	printf("Found %s with %dMB\n", system, board_get_memory() >> 20);
}

void *decompress(void *out, void *in, int len)
{
	struct xz_dec *xz_dec;
	struct xz_buf xz_buf;
	enum xz_ret xz_ret;

	/*
	 * xz-embedded has been integrated into the kernel (since 5.1 and upwards) and is used
	 * especially for firmware loading. This function resembles fw_decompress_xz_single()
	 * and should need no further explanation because of its simplicity.
	 */

	printf("Extract kernel with %d bytes from 0x%08x to 0x%08x...\n", len, in, out);

	xz_crc32_init();
	xz_crc64_init();
	xz_dec = xz_dec_init(XZ_SINGLE, -1);
	if (!xz_dec) {
		printf("xz_dec_init() failed.\n");
		board_panic();
	}

	xz_buf.in_size = len;
	xz_buf.in = in;
	xz_buf.in_pos = 0;
	xz_buf.out_size = 32768000;
	xz_buf.out = out;
	xz_buf.out_pos = 0;

	xz_ret = xz_dec_run(xz_dec, &xz_buf);
	xz_dec_end(xz_dec);

	if (xz_ret != XZ_STREAM_END) {
		printf("extraction failed rc=%d.\n", xz_ret);
		board_panic();
	}

	printf("Extracted kernel size is %d bytes\n", xz_buf.out_pos);
	flush_cache(out, xz_buf.out_pos);

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
