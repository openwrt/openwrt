/*
 *  RouterBoot helper routines
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2018 Chris Schimp <silverchris@gmail.com>
 *  Copyright (C) 2019 Robert Marko <robimarko@gmail.com>
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
#include <string.h>
#include <endian.h>
#include <arpa/inet.h>

#include <lzo/lzo1x.h>

#include "rle.h"
#include "routerboot.h"

inline uint32_t
get_u32(const void *buf)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	return *(uint32_t *)buf;
#elif __BYTE_ORDER == __BIG_ENDIAN
	const uint8_t *p = buf;
	return ((uint32_t) p[3] + ((uint32_t) p[2] << 8) +
	       ((uint32_t) p[1] << 16) + ((uint32_t) p[0] << 24));
#else
#error "Unknown byte order!"
#endif
}

int
routerboot_find_tag(uint8_t *buf, unsigned int buflen, uint16_t tag_id,
		    uint8_t **tag_data, uint16_t *tag_len)
{
	uint16_t id;
	uint16_t len;
	uint32_t magic;
	bool align = false;
	int ret;

	if (buflen < 4)
		return 1;

	magic = get_u32(buf);

	switch (magic) {
	case RB_MAGIC_LZOR:
		buf += 4;
		buflen -= 4;
		break;
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
	while (buflen > 4){
		uint32_t id_and_len = get_u32(buf);
		buf += 4;
		buflen -= 4;
		id = id_and_len & 0xFFFF;
		len = id_and_len >> 16;

		if (align)
			len += (4 - len % 4) % 4;

		if (id == RB_ID_TERMINATOR) {
			break;
		}

		if (buflen < len)
			break;

		if (id == tag_id) {
			*tag_len = len;
			*tag_data = buf;
			ret = 0;
			break;
		}

		buf += len;
		buflen -= len;
	}

	return ret;
}

inline int
rb_find_hard_cfg_tag(uint16_t tag_id, uint8_t **tag_data, uint16_t *tag_len)
{
	if (!rb_hardconfig ||
	    !rb_hardconfig_len)
		return 1;

	return routerboot_find_tag(rb_hardconfig,
				   rb_hardconfig_len,
				   tag_id, tag_data, tag_len);
}

const uint8_t *
rb_get_board_product_code(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_BOARD_PRODUCT_CODE, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

uint32_t
rb_get_board_mac(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_MAC_ADDRESS_PACK, &tag, &tag_len);
	if (err)
		return 0;

	return htonl(get_u32(tag));
}

const uint8_t *
rb_get_board_serial(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_SERIAL_NUMBER, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

const uint8_t *
rb_get_board_identifier(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_BOARD_IDENTIFIER, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

const uint8_t *
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

const uint8_t *
rb_get_factory_booter_version(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_BIOS_VERSION, &tag, &tag_len);
	if (err)
		return NULL;

	return tag;
}

uint32_t
rb_get_flash_info(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	int err;

	err = rb_find_hard_cfg_tag(RB_ID_FLASH_INFO, &tag, &tag_len);
	if (err)
		return 0;

	return htonl(get_u32(tag));
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

	return htonl(get_u32(tag));
}

uint8_t * 
__rb_get_wlan_data(void)
{
	uint16_t tag_len;
	uint8_t *tag;
	uint16_t erd_tag_len;
	uint8_t *erd_tag;
	uint8_t *buf_lzo_in;
	uint8_t *buf_lzo_out;
	uint8_t *buf_rle_out;
	int err;
	uint32_t magic;
	uint32_t erd_magic;
	uint32_t erd_offset;
	size_t lzo_out_len;

	err = rb_find_hard_cfg_tag(RB_ID_WLAN_DATA, &tag, &tag_len);
	if (err) {
		printf("no calibration data found\n");
		goto err;
	}

	buf_lzo_in = malloc(RB_ART_SIZE);
	if (buf_lzo_in == NULL) {
		printf("no memory for calibration data\n");
		goto err;
	}

	buf_rle_out = malloc(RB_ART_SIZE);
	if (buf_rle_out == NULL) {
		printf("no memory for calibration data\n");
		goto err_free_lzo_out;
	}

	buf_lzo_out = malloc(RB_ART_SIZE);
	if (buf_lzo_out == NULL) {
		printf("no memory for calibration data\n");
		goto err_free_lzo_in;
	}

	magic = get_u32(tag);
	if (magic == RB_MAGIC_LZOR) {
		tag += 4;
		tag_len -= 4;
		if (tag_len + sizeof(lzo_prefix) > RB_ART_SIZE) {
			printf("Calibration data too large\n");
			goto err_free_lzo_in;
		}
		printf("Copying fixed LZO prefix (size: %d)\n", sizeof(lzo_prefix));
		memcpy(buf_lzo_in, lzo_prefix, sizeof(lzo_prefix));

		printf("Copying input data (size: %d)\n", tag_len);
		memcpy(buf_lzo_in + sizeof(lzo_prefix), tag, tag_len);

		printf("Decompressing with LZO\n");
		lzo_out_len = RB_ART_SIZE;
		err = lzo1x_decompress_safe(buf_lzo_in, tag_len + sizeof(lzo_prefix),
					    buf_lzo_out, &lzo_out_len, NULL);
		/* For some reason, I get this "input not consumed" error
		 * even though the output is correct, so ignore it. */
		if (err && err != LZO_E_INPUT_NOT_CONSUMED) {
			printf("unable to decompress calibration data: %d\n",
			       err);
			goto err_free_lzo_out;
		}

		printf("Looking for ERD data in decompressed output\n");
		erd_magic = 0;
		for (erd_offset = 0; erd_offset < lzo_out_len; erd_offset++) {
			erd_magic = get_u32(buf_lzo_out + erd_offset);
			if (erd_magic == RB_MAGIC_ERD)
				break;
		}
		if (erd_magic != RB_MAGIC_ERD) {
			printf("no ERD data found\n");
			goto err_free_lzo_out;
		}
		printf("Found ERD magic at offset %d\n", erd_offset);

		err = routerboot_find_tag(buf_lzo_out + erd_offset,
					  lzo_out_len - erd_offset,
					  0x1, &erd_tag, &erd_tag_len);
		if (err) {
			printf("No ERD chunk found\n");
			goto err_free_lzo_out;
		}

		printf("Decompress ERD data with RLE\n");
		err = rle_decode(erd_tag, erd_tag_len, buf_rle_out, RB_ART_SIZE,
				 NULL, NULL);
		if (err) {
			printf("unable to decode ERD data\n");
			goto err_free_rle_out;
		}
	}
	/* Older ath79-based boards directly show the RB_MAGIC_ERD bytes followed by
	the LZO-compressed calibration data with no RLE */
	else if (magic == RB_MAGIC_ERD) {
		if (tag_len > RB_ART_SIZE) {
			printf("Calibration data too large\n");
			goto err_free_lzo_in;
		}

		err = routerboot_find_tag(tag, tag_len,
					  0x1, &buf_lzo_in, &erd_tag_len);
		if (err) {
			printf("No ERD chunk found\n");
			goto err_free_lzo_out;
		}

		printf("Decompressing with LZO\n");
		lzo_out_len = RB_ART_SIZE;
		err = lzo1x_decompress_safe(buf_lzo_in, tag_len,
					    buf_lzo_out, &lzo_out_len, NULL);
		/* For some reason, I get this "input not consumed" error
		 * even though the output is correct, so ignore it. */
		if (err && err != LZO_E_INPUT_NOT_CONSUMED) {
			printf("unable to decompress calibration data: %d\n",
			       err);
			goto err_free_lzo_out;
		}

		buf_rle_out = buf_lzo_out;
	}
	/* Even older ath79-base boards directly have RLE-encoded calibration data,
	without any LZO compresion nor showing RB_MAGIC_ERD bytes */
	else {
		printf("Decode calibration data with RLE\n");
		err = rle_decode(tag, tag_len, buf_rle_out, RB_ART_SIZE,
				 NULL, NULL);
		if (err) {
			printf("unable to decode ERD data\n");
			goto err_free_rle_out;
		}
	}

	return buf_rle_out;

err_free_rle_out:
	free(buf_rle_out);
err_free_lzo_out:
	free(buf_lzo_out);
err_free_lzo_in:
	free(buf_lzo_in);
err:
	return NULL;
}

int
main(int argc, char **argv)
{
	FILE    *infile;
	FILE	*outfile;
	uint8_t *buf;
	uint32_t magic;
	uint32_t i;
	
	if(argc < 2){
		printf("Not enough arguments\n");
		printf("Use -h for help\n");
		exit(1);
	}

	if(strcmp(argv[1], "-h") == 0){
		printf("This program can extract various data from MikroTik devices hard_config partition\n");
		printf("Usage: rbextract <options> <hard_config_location> <output_file> (Optional)\n");
		printf("Options:\n");
		printf("-a Prints all possible info\n");
		printf("-n Prints board name\n");
		printf("-p Prints board product code\n");
		printf("-i Prints board identifier\n");
		printf("-s Prints board serial number\n");
		printf("-m Prints board MAC\n");
		printf("-o Prints board HW options\n");
		printf("-r Prints board RouterBoot factory version\n");
		printf("-f Prints board flash identifier\n");
		printf("-e Extract board radio calibration\n");
		printf("hard_config_location: Path to hard_config partiton\n");
		printf("output_file: Path to where caldata will be output\n");
	} else {
		infile = fopen(argv[2], "r");

		if(infile == NULL){
			printf("Cant open given path\n");
    		return 1;
    	}

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
			printf("Routerboot Hard Config not found\n");
			exit(1);
		}

		if(strcmp(argv[1], "-a") == 0){
			printf("Board name: %s\n", rb_get_board_name());
			printf("Board product code: %s\n", rb_get_board_product_code());
			printf("Board identifier: %s\n", rb_get_board_identifier());
			printf("Board serial: %s\n", rb_get_board_serial());
			printf("Board MAC: %X\n", rb_get_board_mac());
			printf("HW Options %x\n", rb_get_hw_options());
			printf("Factory RouterBoot version: %s\n", rb_get_factory_booter_version());
			printf("Flash identifier: %x\n", rb_get_flash_info());
		} else if(strcmp(argv[1], "-n") == 0){
			printf("%s\n", rb_get_board_name());
		} else if(strcmp(argv[1], "-p") == 0){
			printf("%s\n", rb_get_board_product_code());
		} else if(strcmp(argv[1], "-i") == 0){
			printf("%s\n", rb_get_board_identifier());
		} else if(strcmp(argv[1], "-s") == 0){
			printf("%s\n", rb_get_board_serial());
		} else if(strcmp(argv[1], "-m") == 0){
			printf("%x\n", rb_get_board_mac());
		} else if(strcmp(argv[1], "-o") == 0){
			printf("%x\n", rb_get_hw_options());
		} else if(strcmp(argv[1], "-r") == 0){
			printf("%s\n", rb_get_factory_booter_version());
		} else if(strcmp(argv[1], "-f") == 0){
			printf("%x\n", rb_get_flash_info());
		} else if(strcmp(argv[1], "-e") == 0){
			buf = __rb_get_wlan_data();
			if (buf == NULL) {
				printf("Could not extract calibration data\n");
				return 1;
			}

			if(argv[3] == NULL){
				printf("Missing output file argument\n");
				return 1;
			}
	
			outfile = fopen(argv[3], "wb");
			if(outfile == NULL){
				printf("Cant open given path\n");
    			return 1;
    		}

			/* Write 65536 bytes of caldata */
			for(i = 0; i<RB_ART_SIZE; i++){
				fwrite(&buf[i], sizeof(uint8_t), sizeof(uint8_t), outfile);
			}
			fclose(outfile);
		}
	}
	exit(0);
}
