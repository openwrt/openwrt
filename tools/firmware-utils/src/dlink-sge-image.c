// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/*
 * Copyright (C) 2021 Sebastian Schaper <openwrt@sebastianschaper.net>
 *
 * This tool encrypts factory images for certain D-Link Devices
 * manufactured by SGE / T&W, e.g. COVR-C1200, COVR-P2500, DIR-882, ...
 *
 * Build instructions:
 *   gcc -lcrypto dlink-sge-image.c -o dlink-sge-image
 *
 * Usage:
 *   ./dlink-sge-image infile outfile [-d: decrypt]
 *
 */

#include "dlink-sge-image.h"

#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE		4096

#define HEAD_MAGIC		"SHRS"
#define HEAD_MAGIC_LEN	4
#define SHA512_DIGEST_LENGTH	64
#define RSA_KEY_LENGTH_BYTES	512
#define AES_BLOCK_SIZE	16
#define HEADER_LEN		1756

unsigned char aes_iv[AES_BLOCK_SIZE];

unsigned char readbuf[BUFSIZE];
unsigned char encbuf[BUFSIZE];

unsigned int read_bytes = 0;
unsigned long read_total = 0;
unsigned int i;

unsigned char vendor_key[AES_BLOCK_SIZE];
AES_KEY enc_key;

FILE * input_file;
FILE * output_file;

void image_encrypt(void)
{
	// seek to position 1756 (begin of AES-encrypted data),
	// write image headers later
	char buf[HEADER_LEN];
	memset(buf, 0, HEADER_LEN);
	fwrite(&buf, 1, HEADER_LEN, output_file);

	RSA *rsa = RSA_new();
	BIO *rsa_private_bio = BIO_new_mem_buf(key2_pem, -1);
	PEM_read_bio_RSAPrivateKey(rsa_private_bio, &rsa, NULL, NULL);

	SHA512_CTX digest;
	SHA512_Init(&digest);

	SHA512_CTX digest_post;
	SHA512_Init(&digest_post);

	memcpy(&aes_iv, &salt, AES_BLOCK_SIZE);
	AES_set_encrypt_key(&vendor_key[0], 128, &enc_key);

	uint32_t payload_length_before, pad_len;

	while((read_bytes = fread(&readbuf, 1, BUFSIZE, input_file)) == BUFSIZE)
	{
		SHA512_Update(&digest, &readbuf[0], read_bytes);
		read_total += read_bytes;

		AES_cbc_encrypt(&readbuf[0], encbuf, BUFSIZE, &enc_key, \
			aes_iv, AES_ENCRYPT);
		fwrite(&encbuf, 1, BUFSIZE, output_file);

		SHA512_Update(&digest_post, &encbuf[0], BUFSIZE);
	}

	// handle last block of data (read_bytes < BUFSIZE)
	SHA512_Update(&digest, &readbuf[0], read_bytes);
	read_total += read_bytes;

	pad_len = AES_BLOCK_SIZE - (read_total % AES_BLOCK_SIZE);
	if(pad_len == 0)
	{
		pad_len = 16;
	}
	memset(&readbuf[read_bytes], 0, pad_len);

	AES_cbc_encrypt(&readbuf[0], encbuf, read_bytes + pad_len, \
		&enc_key, aes_iv, AES_ENCRYPT);
	fwrite(&encbuf, 1, read_bytes + pad_len, output_file);

	SHA512_Update(&digest_post, &encbuf[0], read_bytes + pad_len);

	fclose(input_file);
	payload_length_before = read_total;
	printf("\npayload_length_before: %li\n", read_total);

	// copy digest state, since we need another one with vendor key appended,
	// without having to re-hash the whole file (SHA512_Final is destructive)
	SHA512_CTX digest_vendor;
	memcpy(&digest_vendor, &digest, sizeof(SHA512_CTX));

	unsigned char md_before[SHA512_DIGEST_LENGTH];
	SHA512_Final(&md_before[0], &digest);

	printf("\ndigest_before: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_before[i]);
	}

	unsigned char md_vendor[SHA512_DIGEST_LENGTH];
	SHA512_Update(&digest_vendor, &vendor_key[0], AES_BLOCK_SIZE);
	SHA512_Final(&md_vendor[0], &digest_vendor);

	printf("\ndigest_vendor: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_vendor[i]);
	}

	unsigned char md_post[SHA512_DIGEST_LENGTH];
	SHA512_Final(&md_post[0], &digest_post);

	printf("\ndigest_post: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_post[i]);
	}

	char footer[] = {0x00, 0x00, 0x00, 0x00, 0x30};
	fwrite(&footer, 1, 5, output_file);

	// go back to file header and write all the digests and signatures
	fseek(output_file, 0, SEEK_SET);

	fwrite(HEAD_MAGIC, 1, HEAD_MAGIC_LEN, output_file);

	// write payload length before
	uint32_t sizebuf;
	sizebuf = htonl(payload_length_before);
	fwrite((char *) &sizebuf, 1, 4, output_file);

	// write payload length post
	payload_length_before += pad_len;
	sizebuf = htonl(payload_length_before);
	fwrite((char *) &sizebuf, 1, 4, output_file);

	// write salt and digests
	fwrite(salt, 1, AES_BLOCK_SIZE, output_file);
	fwrite(&md_vendor[0], 1, SHA512_DIGEST_LENGTH, output_file);
	fwrite(&md_before[0], 1, SHA512_DIGEST_LENGTH, output_file);
	fwrite(&md_post[0],   1, SHA512_DIGEST_LENGTH, output_file);

	unsigned char sigret[RSA_KEY_LENGTH_BYTES];
	unsigned int siglen = RSA_KEY_LENGTH_BYTES;

	// zero-fill rsa_pub field, unused in header
	memset(sigret, 0, RSA_KEY_LENGTH_BYTES);
	fwrite(&sigret[0], 1, RSA_KEY_LENGTH_BYTES, output_file);

	// sign md_before
	RSA_sign(NID_sha512, &md_before[0], SHA512_DIGEST_LENGTH, \
		&sigret[0], &siglen, rsa);
	printf("\nsigned before:\n");
	for (i = 0; i < RSA_KEY_LENGTH_BYTES; i++)  {
	  printf("%02x", sigret[i]);
	}
	fwrite(&sigret[0], 1, RSA_KEY_LENGTH_BYTES, output_file);

	// sign md_post
	RSA_sign(NID_sha512, &md_post[0], SHA512_DIGEST_LENGTH, \
		&sigret[0], &siglen, rsa);
	printf("\nsigned post:\n");
	for (i = 0; i < RSA_KEY_LENGTH_BYTES; i++)  {
	  printf("%02x", sigret[i]);
	}
	fwrite(&sigret[0], 1, RSA_KEY_LENGTH_BYTES, output_file);

	fclose(output_file);
}

void image_decrypt(void)
{
	printf("\ndecrypt mode\n");

	RSA *rsa = RSA_new();
	BIO *rsa_public_bio = BIO_new_mem_buf(public_pem, -1);
	RSA_print(rsa_public_bio, rsa, 0);
	PEM_read_bio_RSAPublicKey(rsa_public_bio, &rsa, NULL, NULL);

	char magic[4];
	fread(&magic, 1, HEAD_MAGIC_LEN, input_file);
	if(strncmp(magic, HEAD_MAGIC, HEAD_MAGIC_LEN) != 0)
	{
		fprintf(stderr, "Input File header magic does not match '%s'.\n"
			"Maybe this file is not encrypted?\n", HEAD_MAGIC);
		exit(1);
	}

	uint32_t payload_length_before, payload_length_post;
	fread((char *) &payload_length_before, 1, 4, input_file);
	fread((char *) &payload_length_post  , 1, 4, input_file);
	payload_length_before = ntohl(payload_length_before);
	payload_length_post   = ntohl(payload_length_post);

	char salt[AES_BLOCK_SIZE];
	fread(salt, 1, AES_BLOCK_SIZE, input_file);

	char md_vendor[SHA512_DIGEST_LENGTH];
	fread(md_vendor, 1, SHA512_DIGEST_LENGTH, input_file);

	char md_before[SHA512_DIGEST_LENGTH];
	fread(md_before, 1, SHA512_DIGEST_LENGTH, input_file);

	char md_post[SHA512_DIGEST_LENGTH];
	fread(md_post, 1, SHA512_DIGEST_LENGTH, input_file);

	// skip rsa_pub
	fread(readbuf, 1, RSA_KEY_LENGTH_BYTES, input_file);

	unsigned char rsa_sign_before[RSA_KEY_LENGTH_BYTES];
	fread(rsa_sign_before, 1, RSA_KEY_LENGTH_BYTES, input_file);

	unsigned char rsa_sign_post[RSA_KEY_LENGTH_BYTES];
	fread(rsa_sign_post, 1, RSA_KEY_LENGTH_BYTES, input_file);

	// file should be at position HEADER_LEN now, start AES decryption
	SHA512_CTX digest_post;
	SHA512_Init(&digest_post);
	SHA512_CTX digest_before;
	SHA512_Init(&digest_before);
	SHA512_CTX digest_vendor;

	memcpy(&aes_iv, &salt, AES_BLOCK_SIZE);
	AES_set_decrypt_key(&vendor_key[0], 128, &enc_key);
	uint32_t pad_len = payload_length_post - payload_length_before;

	while(read_total < payload_length_post)
	{
		if(read_total + BUFSIZE <= payload_length_post)
		{
			read_bytes = fread(&readbuf, 1, BUFSIZE, input_file);
		}
		else
		{
			read_bytes = fread(&readbuf, 1, \
				payload_length_post - read_total, input_file);
		}

		read_total += read_bytes;

		SHA512_Update(&digest_post, &readbuf[0], read_bytes);

		AES_cbc_encrypt(&readbuf[0], &encbuf[0], read_bytes, &enc_key, \
			aes_iv, AES_DECRYPT);

		// only update digest_before until payload_length_before,
		// do not hash decrypted padding
		if(read_total > payload_length_before)
		{
			// only calc hash for data before padding
			SHA512_Update(&digest_before, &encbuf[0], read_bytes - pad_len);
			fwrite(&encbuf[0], 1, read_bytes - pad_len, output_file);

			// copy state of digest, since SHA512_Final is desctructive
			memcpy(&digest_vendor, &digest_before, sizeof(SHA512_CTX));

			// append vendor_key
			SHA512_Update(&digest_vendor, &vendor_key[0], AES_BLOCK_SIZE);
		}
		else
		{
			// calc hash for all of read_bytes
			SHA512_Update(&digest_before, &encbuf[0], read_bytes);
			fwrite(&encbuf[0], 1, read_bytes, output_file);
		}
	}

	fclose(output_file);

	unsigned char md_post_actual[SHA512_DIGEST_LENGTH];
	SHA512_Final(&md_post_actual[0], &digest_post);

	printf("\ndigest_post: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_post_actual[i]);
	}

	if(strncmp(md_post, (char *) md_post_actual, SHA512_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "SHA512 post does not match file contents.\n");
		exit(1);
	}

	unsigned char md_before_actual[SHA512_DIGEST_LENGTH];
	SHA512_Final(&md_before_actual[0], &digest_before);

	printf("\ndigest_before: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_before_actual[i]);
	}

	if(strncmp(md_before, (char *) md_before_actual, SHA512_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "SHA512 before does not match decrypted payload.\n");
		exit(1);
	}

	unsigned char md_vendor_actual[SHA512_DIGEST_LENGTH];
	SHA512_Final(&md_vendor_actual[0], &digest_vendor);

	printf("\ndigest_vendor: ");
	for (i = 0; i < SHA512_DIGEST_LENGTH; i++)  {
	  printf("%02x", md_vendor_actual[i]);
	}

	if(strncmp(md_vendor, (char *) md_vendor_actual, SHA512_DIGEST_LENGTH) != 0)
	{
		fprintf(stderr, "SHA512 vendor does not match decrypted payload"
			" padded with vendor key.\n");
		exit(1);
	}

	if(RSA_verify(NID_sha512, &md_before_actual[0], SHA512_DIGEST_LENGTH, \
		&rsa_sign_before[0], RSA_KEY_LENGTH_BYTES, rsa))
	{
		printf("\nsignature before verification success");
	}
	else
	{
		fprintf(stderr, "Signature before verification failed.\nThe decrypted"
			" image file may however be flashable via bootloader recovery.\n");
	}

	if(RSA_verify(NID_sha512, &md_post_actual[0], SHA512_DIGEST_LENGTH, \
		&rsa_sign_post[0], RSA_KEY_LENGTH_BYTES, rsa))
	{
		printf("\nsignature post verification success");
	}
	else
	{
		fprintf(stderr, "Signature post verification failed.\nThe decrypted"
			" image file may however be flashable via bootloader recovery.\n");
	}

	printf("\n");
}

int main(int argc, char **argv)
{
	if(argc < 2 || argc > 4)
	{
		fprintf(stderr, "Usage:\n"
			"\tdlink-sge-image infile outfile [-d: decrypt]\n\n");
		exit(1);
	}

	input_file = fopen(argv[1], "rb");
	if(input_file == NULL)
	{
		fprintf(stderr, "Input File %s could not be opened.\n", argv[1]);
		exit(1);
	}

	output_file = fopen(argv[2], "wb");
	if(input_file == NULL)
	{
		fprintf(stderr, "Output File %s could not be opened.\n", argv[2]);
		exit(1);
	}

	memcpy(&aes_iv, &iv, AES_BLOCK_SIZE);
	AES_set_decrypt_key(&key1[0], 128, &enc_key);
	AES_cbc_encrypt(&key2[0], &vendor_key[0], AES_BLOCK_SIZE, &enc_key, \
		aes_iv, AES_DECRYPT);

	printf("\nvendor_key: ");
	for (i = 0; i < AES_BLOCK_SIZE; i++)  {
	  printf("%02x", vendor_key[i]);
	}

	if(argc == 4 && strncmp(argv[3], "-d", 2) == 0)
	{
		image_decrypt();
	}
	else
	{
		image_encrypt();
	}
}
