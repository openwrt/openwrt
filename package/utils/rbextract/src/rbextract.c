/*
 *  RouterBoot helper routines
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2018 Chris Schimp <silverchris@gmail.com>
 *  
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>

#include "rle.h"
#include "routerboot.h"
#include "minilzo.h"

#define RB_ART_SIZE		0x10000
#define RB_BLOCK_SIZE		0x1000
#define RB_MAGIC_ERD		0x00455244	/* extended radio data */

uint8_t    *rb_hardconfig;
long    rb_hardconfig_len;

static uint32_t get_u32(void *buf)
{
	uint8_t *p = buf;

	return ((uint32_t) p[3] + ((uint32_t) p[2] << 8) + ((uint32_t) p[1] << 16) +
	       ((uint32_t) p[0] << 24));
}

static uint16_t get_u16(void *buf)
{
	uint8_t *p = buf;

	return (uint16_t) p[1] + ((uint16_t) p[0] << 8);
}

int
routerboot_find_tag(uint8_t *buf, unsigned int buflen, uint16_t tag_id,
		    uint8_t **tag_data, uint16_t *tag_len)
{
	uint32_t magic;
	bool align = false;
	int ret;

	if (buflen < 4)
		return 1;

	magic = get_u32(buf);
	switch (magic) {
	case RB_MAGIC_ERD:
		align = true;
		/* fall trough */
	case RB_MAGIC_HARD:
		/* skip magic value */
		buf += 4;
		buflen -= 4;
		break;

	case RB_MAGIC_SOFT:
		if (buflen < 8)
			return 1;

		/* skip magic and CRC value */
		buf += 8;
		buflen -= 8;

		break;

	default:
		return 1;
	}

	ret = 1;
	while (buflen > 2) {
		uint16_t id;
		uint16_t len;

		len = get_u16(buf);
		buf += 2;
		buflen -= 2;

		if (buflen < 2)
			break;

		id = get_u16(buf);
		buf += 2;
		buflen -= 2;

		if (id == RB_ID_TERMINATOR)
			break;

		if (buflen < len)
			break;

		if (id == tag_id) {
			if (tag_len)
				*tag_len = len;
			if (tag_data)
				*tag_data = buf;
			ret = 0;
			break;
		}

		if (align)
			len = (len + 3) / 4;

		buf += len;
		buflen -= len;
	}

	return ret;
}

static inline int
rb_find_hard_cfg_tag(uint16_t tag_id, uint8_t **tag_data, uint16_t *tag_len)
{
	if (!rb_hardconfig ||
	    !rb_hardconfig_len)
		return 1;

	return routerboot_find_tag(rb_hardconfig,
				   rb_hardconfig_len,
				   tag_id, tag_data, tag_len);
}

const char *
rb_get_board_name(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_BOARD_NAME, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

uint32_t
rb_get_hw_options(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_HW_OPTIONS, &tag, &tag_len);
	if (err)
		return 0;

	return get_u32(tag);
}

static uint8_t * 
__rb_get_wlan_data(uint16_t id)
{
	uint16_t tag_len;
	uint8_t *tag;
	uint8_t *buf;
	int err;
	uint32_t magic;
	size_t src_done;
	size_t dst_done;

	err = rb_find_hard_cfg_tag(RB_ID_WLAN_DATA, &tag, &tag_len);
	if (err) {
		printf("no calibration data found\n");
		goto err;
	}

	buf = malloc(RB_ART_SIZE);
	if (buf == NULL) {
		printf("no memory for calibration data\n");
		goto err;
	}

	magic = get_u32(tag);
	if (magic == RB_MAGIC_ERD) {
		uint8_t *erd_data;
		uint16_t erd_len;

		printf("Decompressing with LZO\n");
		if (id == 0)
			goto err_free;

		err = routerboot_find_tag(tag, tag_len, id,
					  &erd_data, &erd_len);
		if (err) {
			printf("no ERD data found for id %u\n", id);
			goto err_free;
		}

		dst_done = RB_ART_SIZE;
		err = lzo1x_decompress_safe(erd_data, erd_len, buf, &dst_done, NULL);
		if (err) {
			printf("unable to decompress calibration data %d\n",
			       err);
			goto err_free;
		}
	} else {
		printf("Decompressing with RLE\n");
		if (id != 0)
			goto err_free;

		err = rle_decode((unsigned char *) tag, tag_len, buf, RB_ART_SIZE,
				 &src_done, &dst_done);
		if (err) {
			printf("unable to decode calibration data\n");
			goto err_free;
		}
	}

	return buf;

err_free:
	free(buf);
err:
	return NULL;
}

void *
rb_get_wlan_data(void)
{
	return __rb_get_wlan_data(0);
}


int
main(int argc, char **argv)
{
	FILE    *infile;
	FILE	*outfile;
	uint8_t *buf;
	uint32_t magic;
	uint32_t i;
	
	if(argc != 3){
		printf("Extracts ath9k calibration data from routerboot partition\n");
		printf("Usage:\n");
		printf("routerboot /dev/mtd1 /lib/firmware/soc_wmac.eeprom\n");
		exit(1);
	}
	
	infile = fopen(argv[1], "r");
	
	if(infile == NULL)
    return 1;
	
	fseek(infile, 0L, SEEK_END);
	rb_hardconfig_len = ftell(infile);
	
	fseek(infile, 0L, SEEK_SET);
	
	rb_hardconfig = (uint8_t*)calloc(rb_hardconfig_len, sizeof(uint8_t));	
	if(rb_hardconfig == NULL)
		return 1;

	fread(rb_hardconfig, sizeof(uint8_t), rb_hardconfig_len, infile);
	fclose(infile);
	
	magic = get_u32(rb_hardconfig);
	if(magic != RB_MAGIC_HARD){
		printf("Routerboot Hard Config not found");
		exit(1);
	}
	
	printf("%s\n", rb_get_board_name());
	buf = __rb_get_wlan_data(0);
	
	outfile = fopen(argv[2], "wb");
	//hack off the leading 4k of 0xFF
	for(i = 4096; i<=RB_ART_SIZE; i++){
		fwrite(&buf[i], sizeof(uint8_t), sizeof(uint8_t), outfile);
	}
	fclose(outfile);
	exit(0);
}
