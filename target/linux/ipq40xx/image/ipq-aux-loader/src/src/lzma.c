/*
 * Auxiliary kernel loader for Qualcom IPQ-4XXX/806X based boards
 *
 * Copyright (C) 2019 Sergey Sergeev <adron@mstnt.com>
 *
 * Some parts of this code was based on the OpenWrt specific lzma-loader
 * for the Atheros AR7XXX/AR9XXX based boards:
 *	Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <printf.h>
#include <types.h>
#include <LzmaDecode.h>

/* beyond the image end, size not known in advance */
extern unsigned char workspace[];

static CLzmaDecoderState lzma_state;
static unsigned char *lzma_data;
static unsigned long lzma_datasize;
static unsigned long lzma_outsize;
static unsigned long kernel_la;

static void lzma_init_data(void *_kernel_la, void *_lzma_data, u32 _lzma_datasize)
{
	kernel_la = (unsigned long)_kernel_la;
	lzma_data = _lzma_data;
	lzma_datasize = _lzma_datasize;
}

static __inline__ unsigned char lzma_get_byte(void)
{
	unsigned char c;

	lzma_datasize--;
	c = *lzma_data++;

	return c;
}

static int lzma_init_props(u32 *lzma_outsize_ret)
{
	unsigned char props[LZMA_PROPERTIES_SIZE];
	int res;
	int i;

	/* read lzma properties */
	for (i = 0; i < LZMA_PROPERTIES_SIZE; i++)
		props[i] = lzma_get_byte();

	/* read the lower half of uncompressed size in the header */
	lzma_outsize = ((SizeT) lzma_get_byte()) +
		       ((SizeT) lzma_get_byte() << 8) +
		       ((SizeT) lzma_get_byte() << 16) +
		       ((SizeT) lzma_get_byte() << 24);

	if(lzma_outsize_ret)
		*lzma_outsize_ret = lzma_outsize;
	/* skip rest of the header (upper half of uncompressed size) */
	for (i = 0; i < 4; i++)
		lzma_get_byte();

	res = LzmaDecodeProperties(&lzma_state.Properties, props,
					LZMA_PROPERTIES_SIZE);
	return res;
}

static int lzma_decompress(unsigned char *outStream)
{
	SizeT ip, op;
	int ret;

	lzma_state.Probs = (CProb *) workspace;

	ret = LzmaDecode(&lzma_state, lzma_data, lzma_datasize, &ip, outStream,
			 lzma_outsize, &op);

	if (ret != LZMA_RESULT_OK) {
		int i;

		printf("LzmaDecode error %d at %08x, osize:%d ip:%d op:%d\n",
		    ret, lzma_data + ip, lzma_outsize, ip, op);

		for (i = 0; i < 16; i++)
			printf("%02x ", lzma_data[ip + i]);

		printf("\n");
	}
	return ret;
}

int lzma_gogogo(void *_kernel_la, void *_lzma_data, u32 _lzma_datasize, u32 *lzma_outsize_ret){
	int res;
	lzma_init_data(_kernel_la, _lzma_data, _lzma_datasize);
	res = lzma_init_props(lzma_outsize_ret);
	if (res != LZMA_RESULT_OK) {
		printf("Incorrect LZMA stream properties!\n");
		return -1;
	}
	res = lzma_decompress((unsigned char *) kernel_la);
	if (res != LZMA_RESULT_OK) {
		printf("failed, ");
		switch (res) {
		case LZMA_RESULT_DATA_ERROR:
			printf("data error!\n");
			break;
		default:
			printf("unknown error %d!\n", res);
		}
		return -2;
	} else {
		printf("Done\n");
	}
	return 0;
}
