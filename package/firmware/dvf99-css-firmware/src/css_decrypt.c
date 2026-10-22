// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#define BLOCK 128
#define CHUNK 65536

static int read_full(int fd, unsigned char *buf, size_t n)
{
	size_t off = 0;

	while (off < n) {
		ssize_t r = read(fd, buf + off, n - off);

		if (r < 0)
			return -1;
		if (r == 0)
			break;
		off += (size_t)r;
	}

	return (int)off;
}

int main(int argc, char **argv)
{
	unsigned char block0[BLOCK], block1[BLOCK], salthdr[16];
	unsigned char pass[BLOCK], key[16], iv[16];
	unsigned char in[CHUNK], out[CHUNK + 16];
	EVP_PKEY_CTX *rc;
	EVP_CIPHER_CTX *c;
	EVP_PKEY *pk;
	size_t passlen = sizeof(pass);
	FILE *pf;
	ssize_t r;
	int outl;

	if (argc != 2) {
		fprintf(stderr, "usage: %s pubkey.pem < update.bin > payload.tgz\n",
			argv[0]);
		return 2;
	}

	pf = fopen(argv[1], "r");
	if (!pf) {
		perror("pubkey");
		return 1;
	}
	pk = PEM_read_PUBKEY(pf, NULL, NULL, NULL);
	fclose(pf);
	if (!pk) {
		fprintf(stderr, "bad public key\n");
		return 1;
	}

	if (read_full(0, block0, BLOCK) != BLOCK ||
	    read_full(0, block1, BLOCK) != BLOCK) {
		fprintf(stderr, "short header\n");
		return 1;
	}

	rc = EVP_PKEY_CTX_new(pk, NULL);
	if (!rc || EVP_PKEY_verify_recover_init(rc) <= 0 ||
	    EVP_PKEY_CTX_set_rsa_padding(rc, RSA_PKCS1_PADDING) <= 0 ||
	    EVP_PKEY_verify_recover(rc, pass, &passlen, block1, BLOCK) <= 0) {
		fprintf(stderr, "key recover failed\n");
		return 1;
	}
	EVP_PKEY_CTX_free(rc);
	EVP_PKEY_free(pk);

	{
		unsigned char *nl = memchr(pass, '\n', passlen);

		if (nl)
			passlen = (size_t)(nl - pass);
	}

	if (read_full(0, salthdr, 16) != 16 ||
	    memcmp(salthdr, "Salted__", 8) != 0) {
		fprintf(stderr, "no Salted__ header\n");
		return 1;
	}

	if (EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), salthdr + 8, pass,
			   (int)passlen, 1, key, iv) != 16) {
		fprintf(stderr, "kdf failed\n");
		return 1;
	}

	c = EVP_CIPHER_CTX_new();
	if (!c || EVP_DecryptInit_ex(c, EVP_aes_128_cbc(), NULL, key, iv) <= 0) {
		fprintf(stderr, "cipher init failed\n");
		return 1;
	}

	while ((r = read(0, in, sizeof(in))) > 0) {
		if (EVP_DecryptUpdate(c, out, &outl, in, (int)r) <= 0) {
			fprintf(stderr, "decrypt failed\n");
			return 1;
		}
		if (outl && fwrite(out, 1, (size_t)outl, stdout) != (size_t)outl)
			return 1;
	}
	if (r < 0) {
		perror("read");
		return 1;
	}

	if (EVP_DecryptFinal_ex(c, out, &outl) <= 0) {
		fprintf(stderr, "decrypt final failed\n");
		return 1;
	}
	if (outl && fwrite(out, 1, (size_t)outl, stdout) != (size_t)outl)
		return 1;
	EVP_CIPHER_CTX_free(c);

	return 0;
}
