// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/*
 * Copyright (C) 2020 Sebastian Schaper <openwrt@sebastianschaper.net>
 *
 * This tool encrypts factory images for D-Link DIR-842 or DIR-859.
 *
 * Build instructions:
 *   gcc -lcrypto alpha_encimg.c -o alpha_encimg
 *
 * Sample call for DIR-842 Rev. C3:
 *   ./alpha_encimg image.bin encrypted.bin wrgac65_dlink.2015_dir842EU \
 *     xQYoRZeD726UAbRb846kO7TeNw8eZa6u zufEbNF3kUafxFiE
 *
 * The AES key and IV are found in the GPL sources officially released by
 * D-Link (e.g. DIR842C3_GPL312EUb01DLink/tools/encimg/fw_sign_data.c) or
 * can be extracted from the official images using 'strings /htdocs/cgibin'.
 *   memcpy(aes_key, &______progs_board_fw_sign_data[32], 32);
 *   memcpy(aes_iv,  &______progs_board_fw_sign_data[96], 16);
 *
 * Copyright 2020 OpenWrt.org
 */

#include <openssl/aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096

void alpha_xor(char *input_bytes, unsigned char input_size,
		char *xorkey, unsigned char xorkey_size, unsigned char *out)
{
	for (unsigned char i = 0; i < input_size; i++) {
		out[i] = (i + 1) ^ input_bytes[i];
		out[i] = xorkey[i % xorkey_size] ^ out[i];
	}
}

int main(int argc, char **argv)
{
	char *seama_signature = argv[3];
	char *key = argv[4];
	char *iv = argv[5];

	unsigned char readbuf[BUFSIZE];
	unsigned char encbuf[BUFSIZE];
	unsigned char aes_key[32];
	unsigned char aes_iv[16];

	unsigned int read_bytes = 0;
	unsigned long read_total = 0;
	unsigned int blocks = 0;

	if (argc != 6) {
		fprintf(stderr, "Usage:\n"
			"\talpha_encimg infile outfile aes_key iv seama_signature\n\n");
		exit(1);
	}

	FILE * input_file = fopen(argv[1], "rb");

	if (input_file == NULL) {
		fprintf(stderr, "File %s could not be opened.", argv[1]);
		exit(1);
	}

	if (strlen(key) != 32) {
		fprintf(stderr, "AES key must be 32 bytes.\n");
		exit(1);
	}

	if (strlen(iv) != 16) {
		fprintf(stderr, "AES IV must be 16 bytes.\n");
		exit(1);
	}

	alpha_xor(key, 32, seama_signature, strlen(seama_signature), aes_key);
	alpha_xor(iv,  16, seama_signature, strlen(seama_signature), aes_iv);

	AES_KEY enc_key;

	AES_set_encrypt_key(aes_key, 256, &enc_key);

	FILE *output_file = fopen(argv[2], "wb");

	while ((read_bytes = fread(&readbuf, 1, BUFSIZE, input_file)) > 0) {
		read_total += read_bytes;
		if (read_bytes == BUFSIZE) {
			AES_cbc_encrypt(readbuf, encbuf, BUFSIZE, &enc_key, aes_iv,
				AES_ENCRYPT);
			fwrite(&encbuf, 1, BUFSIZE, output_file);
		} else {
			blocks = read_bytes / 16;
			for (unsigned char i = 0; i < blocks; i++) {
				AES_cbc_encrypt(&readbuf[i * 16], encbuf, 16, &enc_key,
					aes_iv, AES_ENCRYPT);
				fwrite(&encbuf, 1, 16, output_file);
			}
			break;
		}
	}

	fclose(input_file);

	if (read_total < 16) {
		fprintf(stderr, "Input file is too short for encryption.\n");
		exit(1);
	}

	if (read_bytes % 16) {
		// pad to 16 bytes
		unsigned char padding = 16 - (read_bytes % 16);

		memset(&readbuf[read_bytes], 0, padding);
		AES_cbc_encrypt(&readbuf[blocks * 16], encbuf, 16, &enc_key, aes_iv,
			AES_ENCRYPT);
		fwrite(&encbuf, 1, 16, output_file);
	} else {
		// force padding 16 bytes if input is already aligned
		memset(&readbuf, 0, 16);
		AES_cbc_encrypt(readbuf, encbuf, 16, &enc_key, aes_iv, AES_ENCRYPT);
		fwrite(&encbuf, 1, 16, output_file);
	}

	// append 32 bit image size + 12 bytes padding
	readbuf[0] = (read_total & 0xFF000000) >> 24;
	readbuf[1] = (read_total & 0x00FF0000) >> 16;
	readbuf[2] = (read_total & 0x0000FF00) >>  8;
	readbuf[3] =  read_total & 0x000000FF;
	for (unsigned char i = 4; i < 16; i++)
		readbuf[i] = 0x00;

	AES_cbc_encrypt(readbuf, encbuf, 16, &enc_key, aes_iv, AES_ENCRYPT);
	fwrite(&encbuf, 1, 16, output_file);

	fclose(output_file);

	return 0;
}
