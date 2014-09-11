/*
 * Algorithm testing framework and tests.
 *
 * Copyright (c) 2002 James Morris <jmorris@intercode.com.au>
 * Copyright (c) 2002 Jean-Francois Dive <jef@linuxbe.org>
 * Copyright (c) 2007 Nokia Siemens Networks
 * Copyright (c) 2008 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#include <crypto/hash.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <crypto/rng.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/sched.h>

#include "internal.h"
#include "ifxmips_testmgr.h"
#include "ifxmips_tcrypt.h"
#include "ifxmips_deu.h"

/* changes for LQ ablkcipher speedtest */
#include <linux/timex.h>
#include <linux/interrupt.h>
#include <asm/mipsregs.h>

/*
 * Need slab memory for testing (size in number of pages).
 */
#define XBUFSIZE	8

/*
 * Indexes into the xbuf to simulate cross-page access.
 */
#define IDX1		32
#define IDX2		32400
#define IDX3		1
#define IDX4		8193
#define IDX5		22222
#define IDX6		17101
#define IDX7		27333
#define IDX8		3000

/*
* Used by test_cipher()
*/
#define ENCRYPT 1
#define DECRYPT 0

/*
 * Need slab memory for testing (size in number of pages).
 */
#define TVMEMSIZE	4

/*
* Used by test_cipher_speed()
*/
#define ENCRYPT 1
#define DECRYPT 0

/*
 * Used by test_cipher_speed()
 */

#ifndef INIT_COMPLETION
#define INIT_COMPLETION(a) reinit_completion(&a)
#endif


static unsigned int sec;

static char *alg = NULL;
static u32 type;
static u32 mask;
static int mode;
static char *tvmem[TVMEMSIZE];

static char *check[] = {
	"des", "md5", "des3_ede", "rot13", "sha1", "sha224", "sha256",
	"blowfish", "twofish", "serpent", "sha384", "sha512", "md4", "aes",
	"cast6", "arc4", "michael_mic", "deflate", "crc32c", "tea", "xtea",
	"khazad", "wp512", "wp384", "wp256", "tnepres", "xeta",  "fcrypt",
	"camellia", "seed", "salsa20", "rmd128", "rmd160", "rmd256", "rmd320",
	"lzo", "cts", "zlib", NULL
};
struct tcrypt_result {
	struct completion completion;
	int err;
};

struct aead_test_suite {
	struct {
		struct aead_testvec *vecs;
		unsigned int count;
	} enc, dec;
};

struct cipher_test_suite {
	struct {
		struct cipher_testvec *vecs;
		unsigned int count;
	} enc, dec;
};

struct comp_test_suite {
	struct {
		struct comp_testvec *vecs;
		unsigned int count;
	} comp, decomp;
};

struct pcomp_test_suite {
	struct {
		struct pcomp_testvec *vecs;
		unsigned int count;
	} comp, decomp;
};

struct hash_test_suite {
	struct hash_testvec *vecs;
	unsigned int count;
};

struct cprng_test_suite {
	struct cprng_testvec *vecs;
	unsigned int count;
};

struct alg_test_desc {
	const char *alg;
	int (*test)(const struct alg_test_desc *desc, const char *driver,
		    u32 type, u32 mask);
	int fips_allowed;	/* set if alg is allowed in fips mode */

	union {
		struct aead_test_suite aead;
		struct cipher_test_suite cipher;
		struct comp_test_suite comp;
		struct pcomp_test_suite pcomp;
		struct hash_test_suite hash;
		struct cprng_test_suite cprng;
	} suite;
};

static unsigned int IDX[8] = { IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8 };

static void hexdump(unsigned char *buf, unsigned int len)
{
	print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
			16, 1,
			buf, len, false);
}

static void tcrypt_complete(struct crypto_async_request *req, int err)
{
	struct tcrypt_result *res = req->data;
        
        //printk("Signal done test\n");

	if (err == -EINPROGRESS) {
                printk("********************* Completion didnt go too well **************************** \n");
		return;
        }

	res->err = err;
	complete_all(&res->completion);
}

static int testmgr_alloc_buf(char *buf[XBUFSIZE])
{
	int i;

	for (i = 0; i < XBUFSIZE; i++) {
		buf[i] = (void *)__get_free_page(GFP_KERNEL);
		if (!buf[i])
			goto err_free_buf;
	}

	return 0;

err_free_buf:
	while (i-- > 0)
		free_page((unsigned long)buf[i]);

	return -ENOMEM;
}

static void testmgr_free_buf(char *buf[XBUFSIZE])
{
	int i;

	for (i = 0; i < XBUFSIZE; i++)
		free_page((unsigned long)buf[i]);
}

static int test_hash(struct crypto_ahash *tfm, struct hash_testvec *template,
		     unsigned int tcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_ahash_tfm(tfm));
	unsigned int i, j, k, temp;
	struct scatterlist sg[8];
	char result[64];
	struct ahash_request *req;
	struct tcrypt_result tresult;
	void *hash_buff;
	char *xbuf[XBUFSIZE];
	int ret = -ENOMEM;

	if (testmgr_alloc_buf(xbuf))
		goto out_nobuf;

	init_completion(&tresult.completion);

	req = ahash_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		printk(KERN_ERR "alg: hash: Failed to allocate request for "
		       "%s\n", algo);
		goto out_noreq;
	}
	ahash_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				   tcrypt_complete, &tresult);

	j = 0;
	for (i = 0; i < tcount; i++) {
		if (template[i].np)
			continue;

		j++;
		memset(result, 0, 64);

		hash_buff = xbuf[0];

		memcpy(hash_buff, template[i].plaintext, template[i].psize);
		sg_init_one(&sg[0], hash_buff, template[i].psize);

		if (template[i].ksize) {
			crypto_ahash_clear_flags(tfm, ~0);
			ret = crypto_ahash_setkey(tfm, template[i].key,
						  template[i].ksize);
			if (ret) {
				printk(KERN_ERR "alg: hash: setkey failed on "
				       "test %d for %s: ret=%d\n", j, algo,
				       -ret);
				goto out;
			}
		}

		ahash_request_set_crypt(req, sg, result, template[i].psize);
		ret = crypto_ahash_digest(req);
		switch (ret) {
		case 0:
			break;
		case -EINPROGRESS:
		case -EBUSY:
			ret = wait_for_completion_interruptible(
				&tresult.completion);
			if (!ret && !(ret = tresult.err)) {
				INIT_COMPLETION(tresult.completion);
				break;
			}
			/* fall through */
		default:
			printk(KERN_ERR "alg: hash: digest failed on test %d "
			       "for %s: ret=%d\n", j, algo, -ret);
			goto out;
		}

		if (memcmp(result, template[i].digest,
			   crypto_ahash_digestsize(tfm))) {
			printk(KERN_ERR "alg: hash: Test %d failed for %s\n",
			       j, algo);
			hexdump(result, crypto_ahash_digestsize(tfm));
			ret = -EINVAL;
			goto out;
		}
		else {
			printk(KERN_ERR "alg: hash: Test %d passed for %s\n",
			       j, algo);
			hexdump(result, crypto_ahash_digestsize(tfm));
		}
	}

	j = 0;
	for (i = 0; i < tcount; i++) {
		if (template[i].np) {
			j++;
			memset(result, 0, 64);

			temp = 0;
			sg_init_table(sg, template[i].np);
			ret = -EINVAL;
			for (k = 0; k < template[i].np; k++) {
				if (WARN_ON(offset_in_page(IDX[k]) +
					    template[i].tap[k] > PAGE_SIZE))
					goto out;
				sg_set_buf(&sg[k],
					   memcpy(xbuf[IDX[k] >> PAGE_SHIFT] +
						  offset_in_page(IDX[k]),
						  template[i].plaintext + temp,
						  template[i].tap[k]),
					   template[i].tap[k]);
				temp += template[i].tap[k];
			}

			if (template[i].ksize) {
				crypto_ahash_clear_flags(tfm, ~0);
				ret = crypto_ahash_setkey(tfm, template[i].key,
							  template[i].ksize);

				if (ret) {
					printk(KERN_ERR "alg: hash: setkey "
					       "failed on chunking test %d "
					       "for %s: ret=%d\n", j, algo,
					       -ret);
					goto out;
				}
			}

			ahash_request_set_crypt(req, sg, result,
						template[i].psize);
			ret = crypto_ahash_digest(req);
			switch (ret) {
			case 0:
				break;
			case -EINPROGRESS:
			case -EBUSY:
				ret = wait_for_completion_interruptible(
					&tresult.completion);
				if (!ret && !(ret = tresult.err)) {
					INIT_COMPLETION(tresult.completion);
					break;
				}
				/* fall through */
			default:
				printk(KERN_ERR "alg: hash: digest failed "
				       "on chunking test %d for %s: "
				       "ret=%d\n", j, algo, -ret);
				goto out;
			}

			if (memcmp(result, template[i].digest,
				   crypto_ahash_digestsize(tfm))) {
				printk(KERN_ERR "alg: hash: Chunking test %d "
				       "failed for %s\n", j, algo);
				hexdump(result, crypto_ahash_digestsize(tfm));
				ret = -EINVAL;
				goto out;
			}
			else {
				printk(KERN_ERR "alg: hash: Chunking test %d "
				       "passed for %s\n", j, algo);
				hexdump(result, crypto_ahash_digestsize(tfm));
			}
		}
	}

	ret = 0;

out:
	ahash_request_free(req);
out_noreq:
	testmgr_free_buf(xbuf);
out_nobuf:
	return ret;
}

static int test_aead(struct crypto_aead *tfm, int enc,
		     struct aead_testvec *template, unsigned int tcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_aead_tfm(tfm));
	unsigned int i, j, k, n, temp;
	int ret = -ENOMEM;
	char *q;
	char *key;
	struct aead_request *req;
	struct scatterlist sg[8];
	struct scatterlist asg[8];
	const char *e;
	struct tcrypt_result result;
	unsigned int authsize;
	void *input;
	void *assoc;
	char iv[MAX_IVLEN];
	char *xbuf[XBUFSIZE];
	char *axbuf[XBUFSIZE];

	if (testmgr_alloc_buf(xbuf))
		goto out_noxbuf;
	if (testmgr_alloc_buf(axbuf))
		goto out_noaxbuf;

	if (enc == ENCRYPT)
		e = "encryption";
	else
		e = "decryption";

	init_completion(&result.completion);

	req = aead_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		printk(KERN_ERR "alg: aead: Failed to allocate request for "
		       "%s\n", algo);
		goto out;
	}

	aead_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				  tcrypt_complete, &result);

	for (i = 0, j = 0; i < tcount; i++) {
		if (!template[i].np) {
			j++;

			/* some tepmplates have no input data but they will
			 * touch input
			 */
			input = xbuf[0];
			assoc = axbuf[0];

			ret = -EINVAL;
			if (WARN_ON(template[i].ilen > PAGE_SIZE ||
				    template[i].alen > PAGE_SIZE))
				goto out;

			memcpy(input, template[i].input, template[i].ilen);
			memcpy(assoc, template[i].assoc, template[i].alen);
			if (template[i].iv)
				memcpy(iv, template[i].iv, MAX_IVLEN);
			else
				memset(iv, 0, MAX_IVLEN);

			crypto_aead_clear_flags(tfm, ~0);
			if (template[i].wk)
				crypto_aead_set_flags(
					tfm, CRYPTO_TFM_REQ_WEAK_KEY);

			key = template[i].key;

			ret = crypto_aead_setkey(tfm, key,
						 template[i].klen);
			if (!ret == template[i].fail) {
				printk(KERN_ERR "alg: aead: setkey failed on "
				       "test %d for %s: flags=%x\n", j, algo,
				       crypto_aead_get_flags(tfm));
				goto out;
			} else if (ret)
				continue;

			authsize = abs(template[i].rlen - template[i].ilen);
			ret = crypto_aead_setauthsize(tfm, authsize);
			if (ret) {
				printk(KERN_ERR "alg: aead: Failed to set "
				       "authsize to %u on test %d for %s\n",
				       authsize, j, algo);
				goto out;
			}

			sg_init_one(&sg[0], input,
				    template[i].ilen + (enc ? authsize : 0));

			sg_init_one(&asg[0], assoc, template[i].alen);

			aead_request_set_crypt(req, sg, sg,
					       template[i].ilen, iv);

			aead_request_set_assoc(req, asg, template[i].alen);

			ret = enc ?
				crypto_aead_encrypt(req) :
				crypto_aead_decrypt(req);

			switch (ret) {
			case 0:
				if (template[i].novrfy) {
					/* verification was supposed to fail */
					printk(KERN_ERR "alg: aead: %s failed "
					       "on test %d for %s: ret was 0, "
					       "expected -EBADMSG\n",
					       e, j, algo);
					/* so really, we got a bad message */
					ret = -EBADMSG;
					goto out;
				}
				break;
			case -EINPROGRESS:
			case -EBUSY:
				ret = wait_for_completion_interruptible(
					&result.completion);
				if (!ret && !(ret = result.err)) {
					INIT_COMPLETION(result.completion);
					break;
				}
			case -EBADMSG:
				if (template[i].novrfy)
					/* verification failure was expected */
					continue;
				/* fall through */
			default:
				printk(KERN_ERR "alg: aead: %s failed on test "
				       "%d for %s: ret=%d\n", e, j, algo, -ret);
				goto out;
			}

			q = input;
			if (memcmp(q, template[i].result, template[i].rlen)) {
				printk(KERN_ERR "alg: aead: Test %d failed on "
				       "%s for %s\n", j, e, algo);
				hexdump(q, template[i].rlen);
				ret = -EINVAL;
				goto out;
			}
			else {
				printk(KERN_ERR "alg: aead: Test %d passed on "
				       "%s for %s\n", j, e, algo);
				hexdump(q, template[i].rlen);
			}
		}
	}

	for (i = 0, j = 0; i < tcount; i++) {
		if (template[i].np) {
			j++;

			if (template[i].iv)
				memcpy(iv, template[i].iv, MAX_IVLEN);
			else
				memset(iv, 0, MAX_IVLEN);

			crypto_aead_clear_flags(tfm, ~0);
			if (template[i].wk)
				crypto_aead_set_flags(
					tfm, CRYPTO_TFM_REQ_WEAK_KEY);
			key = template[i].key;

			ret = crypto_aead_setkey(tfm, key, template[i].klen);
			if (!ret == template[i].fail) {
				printk(KERN_ERR "alg: aead: setkey failed on "
				       "chunk test %d for %s: flags=%x\n", j,
				       algo, crypto_aead_get_flags(tfm));
				goto out;
			} else if (ret)
				continue;

			authsize = abs(template[i].rlen - template[i].ilen);

			ret = -EINVAL;
			sg_init_table(sg, template[i].np);
			for (k = 0, temp = 0; k < template[i].np; k++) {
				if (WARN_ON(offset_in_page(IDX[k]) +
					    template[i].tap[k] > PAGE_SIZE))
					goto out;

				q = xbuf[IDX[k] >> PAGE_SHIFT] +
				    offset_in_page(IDX[k]);

				memcpy(q, template[i].input + temp,
				       template[i].tap[k]);

				n = template[i].tap[k];
				if (k == template[i].np - 1 && enc)
					n += authsize;
				if (offset_in_page(q) + n < PAGE_SIZE)
					q[n] = 0;

				sg_set_buf(&sg[k], q, template[i].tap[k]);
				temp += template[i].tap[k];
			}

			ret = crypto_aead_setauthsize(tfm, authsize);
			if (ret) {
				printk(KERN_ERR "alg: aead: Failed to set "
				       "authsize to %u on chunk test %d for "
				       "%s\n", authsize, j, algo);
				goto out;
			}

			if (enc) {
				if (WARN_ON(sg[k - 1].offset +
					    sg[k - 1].length + authsize >
					    PAGE_SIZE)) {
					ret = -EINVAL;
					goto out;
				}

				sg[k - 1].length += authsize;
			}

			sg_init_table(asg, template[i].anp);
			ret = -EINVAL;
			for (k = 0, temp = 0; k < template[i].anp; k++) {
				if (WARN_ON(offset_in_page(IDX[k]) +
					    template[i].atap[k] > PAGE_SIZE))
					goto out;
				sg_set_buf(&asg[k],
					   memcpy(axbuf[IDX[k] >> PAGE_SHIFT] +
						  offset_in_page(IDX[k]),
						  template[i].assoc + temp,
						  template[i].atap[k]),
					   template[i].atap[k]);
				temp += template[i].atap[k];
			}

			aead_request_set_crypt(req, sg, sg,
					       template[i].ilen,
					       iv);

			aead_request_set_assoc(req, asg, template[i].alen);

			ret = enc ?
				crypto_aead_encrypt(req) :
				crypto_aead_decrypt(req);

			switch (ret) {
			case 0:
				if (template[i].novrfy) {
					/* verification was supposed to fail */
					printk(KERN_ERR "alg: aead: %s failed "
					       "on chunk test %d for %s: ret "
					       "was 0, expected -EBADMSG\n",
					       e, j, algo);
					/* so really, we got a bad message */
					ret = -EBADMSG;
					goto out;
				}
				break;
			case -EINPROGRESS:
			case -EBUSY:
				ret = wait_for_completion_interruptible(
					&result.completion);
				if (!ret && !(ret = result.err)) {
					INIT_COMPLETION(result.completion);
					break;
				}
			case -EBADMSG:
				if (template[i].novrfy)
					/* verification failure was expected */
					continue;
				/* fall through */
			default:
				printk(KERN_ERR "alg: aead: %s failed on "
				       "chunk test %d for %s: ret=%d\n", e, j,
				       algo, -ret);
				goto out;
			}

			ret = -EINVAL;
			for (k = 0, temp = 0; k < template[i].np; k++) {
				q = xbuf[IDX[k] >> PAGE_SHIFT] +
				    offset_in_page(IDX[k]);

				n = template[i].tap[k];
				if (k == template[i].np - 1)
					n += enc ? authsize : -authsize;

				if (memcmp(q, template[i].result + temp, n)) {
					printk(KERN_ERR "alg: aead: Chunk "
					       "test %d failed on %s at page "
					       "%u for %s\n", j, e, k, algo);
					hexdump(q, n);
					goto out;
				}
				else {
					printk(KERN_ERR "alg: aead: Chunk "
					       "test %d passed on %s at page "
					       "%u for %s\n", j, e, k, algo);
					hexdump(q, n);
				}

				q += n;
				if (k == template[i].np - 1 && !enc) {
					if (memcmp(q, template[i].input +
						      temp + n, authsize))
						n = authsize;
					else
						n = 0;
				} else {
					for (n = 0; offset_in_page(q + n) &&
						    q[n]; n++)
						;
				}
				if (n) {
					printk(KERN_ERR "alg: aead: Result "
					       "buffer corruption in chunk "
					       "test %d on %s at page %u for "
					       "%s: %u bytes:\n", j, e, k,
					       algo, n);
					hexdump(q, n);
					goto out;
				}
				temp += template[i].tap[k];
			}
		}
	}

	ret = 0;

out:
	aead_request_free(req);
	testmgr_free_buf(axbuf);
out_noaxbuf:
	testmgr_free_buf(xbuf);
out_noxbuf:
	return ret;
}

static int test_cipher(struct crypto_cipher *tfm, int enc,
		       struct cipher_testvec *template, unsigned int tcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_cipher_tfm(tfm));
	unsigned int i, j, k;
	char *q;
	const char *e;
	void *data;
	char *xbuf[XBUFSIZE];
	int ret = -ENOMEM;

	if (testmgr_alloc_buf(xbuf))
		goto out_nobuf;

	if (enc == ENCRYPT)
	        e = "encryption";
	else
		e = "decryption";

	j = 0;
	for (i = 0; i < tcount; i++) {
		if (template[i].np)
			continue;

		j++;

		ret = -EINVAL;
		if (WARN_ON(template[i].ilen > PAGE_SIZE))
			goto out;

		data = xbuf[0];
		memcpy(data, template[i].input, template[i].ilen);

		crypto_cipher_clear_flags(tfm, ~0);
		if (template[i].wk)
			crypto_cipher_set_flags(tfm, CRYPTO_TFM_REQ_WEAK_KEY);

		ret = crypto_cipher_setkey(tfm, template[i].key,
					   template[i].klen);
		if (!ret == template[i].fail) {
			printk(KERN_ERR "alg: cipher: setkey failed "
			       "on test %d for %s: flags=%x\n", j,
			       algo, crypto_cipher_get_flags(tfm));
			goto out;
		} else if (ret)
			continue;

		for (k = 0; k < template[i].ilen;
		     k += crypto_cipher_blocksize(tfm)) {
			if (enc)
				crypto_cipher_encrypt_one(tfm, data + k,
							  data + k);
			else
				crypto_cipher_decrypt_one(tfm, data + k,
							  data + k);
		}

		q = data;
		if (memcmp(q, template[i].result, template[i].rlen)) {
			printk(KERN_ERR "alg: cipher: Test %d failed "
			       "on %s for %s\n", j, e, algo);
			hexdump(q, template[i].rlen);
			ret = -EINVAL;
			goto out;
		}
		else {
			printk(KERN_ERR "alg: cipher: Test %d passed "
			       "on %s for %s\n", j, e, algo);
			hexdump(q, template[i].rlen);
		}
	}

	ret = 0;

out:
	testmgr_free_buf(xbuf);
out_nobuf:
	return ret;
}

static int test_skcipher(struct crypto_ablkcipher *tfm, int enc,
			 struct cipher_testvec *template, unsigned int tcount)
{
	const char *algo =
		crypto_tfm_alg_driver_name(crypto_ablkcipher_tfm(tfm));
	unsigned int i, j, k, n, temp;
	char *q;
	struct ablkcipher_request *req;
	struct scatterlist sg[8];
	const char *e;
	struct tcrypt_result result;
	void *data;
	char iv[MAX_IVLEN];
	char *xbuf[XBUFSIZE];
	int ret = -ENOMEM;

	if (testmgr_alloc_buf(xbuf))
		goto out_nobuf;

	if (enc == ENCRYPT)
	        e = "encryption";
	else
		e = "decryption";

	init_completion(&result.completion);

	req = ablkcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		printk(KERN_ERR "alg: skcipher: Failed to allocate request "
		       "for %s\n", algo);
		goto out;
	}
 
        //printk("tcount: %u\n", tcount);

	ablkcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
					tcrypt_complete, &result);

	j = 0;
	for (i = 0; i < tcount; i++) {
		if (template[i].iv)
			memcpy(iv, template[i].iv, MAX_IVLEN);
		else
			memset(iv, 0, MAX_IVLEN);

		if (!(template[i].np)) {
                        //printk("np: %d, i: %d, j: %d\n", template[i].np, i, j);
			j++;

			ret = -EINVAL;
			if (WARN_ON(template[i].ilen > PAGE_SIZE))
				goto out;

			data = xbuf[0];
			memcpy(data, template[i].input, template[i].ilen);

			crypto_ablkcipher_clear_flags(tfm, ~0);
			if (template[i].wk)
				crypto_ablkcipher_set_flags(
					tfm, CRYPTO_TFM_REQ_WEAK_KEY);

			ret = crypto_ablkcipher_setkey(tfm, template[i].key,
						       template[i].klen);
			if (!ret == template[i].fail) {
				printk(KERN_ERR "alg: skcipher: setkey failed "
				       "on test %d for %s: flags=%x\n", j,
				       algo, crypto_ablkcipher_get_flags(tfm));
                                printk("ERROR\n");
				goto out;
			} else if (ret)
				continue;

			sg_init_one(&sg[0], data, template[i].ilen);

			ablkcipher_request_set_crypt(req, sg, sg,
						     template[i].ilen, iv);
			ret = enc ?
				crypto_ablkcipher_encrypt(req) :
				crypto_ablkcipher_decrypt(req);

			switch (ret) {
			case 0:
				break;
			case -EINPROGRESS:
			case -EBUSY:
				ret = wait_for_completion_interruptible(
					&result.completion);
				if (!ret && !((ret = result.err))) {
					INIT_COMPLETION(result.completion);
					break;
				}
				/* fall through */
			default:
				printk(KERN_ERR "alg: skcipher: %s failed on "
				       "test %d for %s: ret=%d\n", e, j, algo,
				       -ret);
                                printk("ERROR\n");
				goto out;
			}
			q = data;
			if (memcmp(q, template[i].result, template[i].rlen)) {
				printk(KERN_ERR "alg: skcipher: Test %d "
				       "failed on %s for %s\n", j, e, algo);
				hexdump(q, template[i].rlen);
                                printk("ERROR\n");
				ret = -EINVAL;
				goto out;
			}
			else {
				printk(KERN_ERR "alg: skcipher: Test %d "
				       "*PASSED* on %s for %s\n", j, e, algo);
				hexdump(q, template[i].rlen);
                                printk("DONE\n");
			}
		}
	}
        printk("Testing %s chunking across pages.\n", algo);
	j = 0;
	for (i = 0; i < tcount; i++) {             
		if (template[i].iv)
			memcpy(iv, template[i].iv, MAX_IVLEN);
		else
			memset(iv, 0, MAX_IVLEN);

		if (template[i].np) {
			j++;

			crypto_ablkcipher_clear_flags(tfm, ~0);
			if (template[i].wk)
				crypto_ablkcipher_set_flags(
					tfm, CRYPTO_TFM_REQ_WEAK_KEY);

			ret = crypto_ablkcipher_setkey(tfm, template[i].key,
						       template[i].klen);
			if (!ret == template[i].fail) {
				printk(KERN_ERR "alg: skcipher: setkey failed "
				       "on chunk test %d for %s: flags=%x\n",
				       j, algo,
				       crypto_ablkcipher_get_flags(tfm));
                                printk("ERROR\n");
				goto out;
			} else if (ret)
				continue;

			temp = 0;
			ret = -EINVAL;
			sg_init_table(sg, template[i].np);
			for (k = 0; k < template[i].np; k++) {
				if (WARN_ON(offset_in_page(IDX[k]) +
					    template[i].tap[k] > PAGE_SIZE))
					goto out;

				q = xbuf[IDX[k] >> PAGE_SHIFT] +
				    offset_in_page(IDX[k]);

				memcpy(q, template[i].input + temp,
				       template[i].tap[k]);

				if (offset_in_page(q) + template[i].tap[k] <
				    PAGE_SIZE)
					q[template[i].tap[k]] = 0;

				sg_set_buf(&sg[k], q, template[i].tap[k]);

				temp += template[i].tap[k];
			}

			ablkcipher_request_set_crypt(req, sg, sg,
					template[i].ilen, iv);

			ret = enc ?
				crypto_ablkcipher_encrypt(req) :
				crypto_ablkcipher_decrypt(req);

			switch (ret) {
			case 0:
				break;
			case -EINPROGRESS:
			case -EBUSY:
				ret = wait_for_completion_interruptible(
					&result.completion);
				if (!ret && !((ret = result.err))) {
					INIT_COMPLETION(result.completion);
					break;
				}
				/* fall through */
			default:
				printk(KERN_ERR "alg: skcipher: %s failed on "
				       "chunk test %d for %s: ret=%d\n", e, j,
				       algo, -ret);
                                printk("ERROR\n");
				goto out;
			}

			temp = 0;
			ret = -EINVAL;
			for (k = 0; k < template[i].np; k++) {
				q = xbuf[IDX[k] >> PAGE_SHIFT] +
				    offset_in_page(IDX[k]);

				if (memcmp(q, template[i].result + temp,
					   template[i].tap[k])) {
					printk(KERN_ERR "alg: skcipher: Chunk "
					       "test %d failed on %s at page "
					       "%u for %s\n", j, e, k, algo);
					hexdump(q, template[i].tap[k]);
                                        printk("ERROR\n");
					goto out;
				}
				else {
					printk(KERN_ERR "alg: skcipher: Chunk "
					       "test %d *PASSED* on %s at page "
					       "%u for %s\n", j, e, k, algo);
					hexdump(q, template[i].tap[k]);
                                        printk("DONE\n");
				}

				q += template[i].tap[k];
				for (n = 0; offset_in_page(q + n) && q[n]; n++)
					;
#if 1
				if (n) {
					printk(KERN_ERR "alg: skcipher: "
					       "Result buffer corruption in "
					       "chunk test %d on %s at page "
					       "%u for %s: %u bytes:\n", j, e,
					       k, algo, n);
					hexdump(q, n);
                                        printk("ERROR\n");
					goto out;
				}
                                else {
					printk(KERN_ERR "alg: skcipher: "
					       "Result buffer clean in "
					       "chunk test %d on %s at page "
					       "%u for %s: %u bytes:\n", j, e,
					       k, algo, n);
					hexdump(q, n);
                                        printk("Chunk Buffer clean\n");
                                }
#endif
				temp += template[i].tap[k];
			}
		}
	}

	ret = 0;
out:
	ablkcipher_request_free(req);
	testmgr_free_buf(xbuf);
out_nobuf:
	return ret;
}

static int test_comp(struct crypto_comp *tfm, struct comp_testvec *ctemplate,
		     struct comp_testvec *dtemplate, int ctcount, int dtcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_comp_tfm(tfm));
	unsigned int i;
	char result[COMP_BUF_SIZE];
	int ret;

	for (i = 0; i < ctcount; i++) {
		int ilen;
		unsigned int dlen = COMP_BUF_SIZE;

		memset(result, 0, sizeof (result));

		ilen = ctemplate[i].inlen;
		ret = crypto_comp_compress(tfm, ctemplate[i].input,
		                           ilen, result, &dlen);
		if (ret) {
			printk(KERN_ERR "alg: comp: compression failed "
			       "on test %d for %s: ret=%d\n", i + 1, algo,
			       -ret);
			goto out;
		}

		if (dlen != ctemplate[i].outlen) {
			printk(KERN_ERR "alg: comp: Compression test %d "
			       "failed for %s: output len = %d\n", i + 1, algo,
			       dlen);
			ret = -EINVAL;
			goto out;
		}

		if (memcmp(result, ctemplate[i].output, dlen)) {
			printk(KERN_ERR "alg: comp: Compression test %d "
			       "failed for %s\n", i + 1, algo);
			hexdump(result, dlen);
			ret = -EINVAL;
			goto out;
		}
		else {
			printk(KERN_ERR "alg: comp: Compression test %d "
			       "passed for %s\n", i + 1, algo);
			hexdump(result, dlen);
		}
	}

	for (i = 0; i < dtcount; i++) {
		int ilen;
		unsigned int dlen = COMP_BUF_SIZE;

		memset(result, 0, sizeof (result));

		ilen = dtemplate[i].inlen;
		ret = crypto_comp_decompress(tfm, dtemplate[i].input,
		                             ilen, result, &dlen);
		if (ret) {
			printk(KERN_ERR "alg: comp: decompression failed "
			       "on test %d for %s: ret=%d\n", i + 1, algo,
			       -ret);
			goto out;
		}

		if (dlen != dtemplate[i].outlen) {
			printk(KERN_ERR "alg: comp: Decompression test %d "
			       "failed for %s: output len = %d\n", i + 1, algo,
			       dlen);
			ret = -EINVAL;
			goto out;
		}

		if (memcmp(result, dtemplate[i].output, dlen)) {
			printk(KERN_ERR "alg: comp: Decompression test %d "
			       "failed for %s\n", i + 1, algo);
			hexdump(result, dlen);
			ret = -EINVAL;
			goto out;
		}
		else {
			printk(KERN_ERR "alg: comp: Decompression test %d "
			       "passed for %s\n", i + 1, algo);
			hexdump(result, dlen);
		}
	}

	ret = 0;

out:
	return ret;
}

static int test_pcomp(struct crypto_pcomp *tfm,
		      struct pcomp_testvec *ctemplate,
		      struct pcomp_testvec *dtemplate, int ctcount,
		      int dtcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_pcomp_tfm(tfm));
	unsigned int i;
	char result[COMP_BUF_SIZE];
	int res;

	for (i = 0; i < ctcount; i++) {
		struct comp_request req;
		unsigned int produced = 0;

		res = crypto_compress_setup(tfm, ctemplate[i].params,
					    ctemplate[i].paramsize);
		if (res) {
			pr_err("alg: pcomp: compression setup failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}

		res = crypto_compress_init(tfm);
		if (res) {
			pr_err("alg: pcomp: compression init failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}

		memset(result, 0, sizeof(result));

		req.next_in = ctemplate[i].input;
		req.avail_in = ctemplate[i].inlen / 2;
		req.next_out = result;
		req.avail_out = ctemplate[i].outlen / 2;

		res = crypto_compress_update(tfm, &req);
		if (res < 0 && (res != -EAGAIN || req.avail_in)) {
			pr_err("alg: pcomp: compression update failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		if (res > 0)
			produced += res;

		/* Add remaining input data */
		req.avail_in += (ctemplate[i].inlen + 1) / 2;

		res = crypto_compress_update(tfm, &req);
		if (res < 0 && (res != -EAGAIN || req.avail_in)) {
			pr_err("alg: pcomp: compression update failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		if (res > 0)
			produced += res;

		/* Provide remaining output space */
		req.avail_out += COMP_BUF_SIZE - ctemplate[i].outlen / 2;

		res = crypto_compress_final(tfm, &req);
		if (res < 0) {
			pr_err("alg: pcomp: compression final failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		produced += res;

		if (COMP_BUF_SIZE - req.avail_out != ctemplate[i].outlen) {
			pr_err("alg: comp: Compression test %d failed for %s: "
			       "output len = %d (expected %d)\n", i + 1, algo,
			       COMP_BUF_SIZE - req.avail_out,
			       ctemplate[i].outlen);
			return -EINVAL;
		}

		if (produced != ctemplate[i].outlen) {
			pr_err("alg: comp: Compression test %d failed for %s: "
			       "returned len = %u (expected %d)\n", i + 1,
			       algo, produced, ctemplate[i].outlen);
			return -EINVAL;
		}

		if (memcmp(result, ctemplate[i].output, ctemplate[i].outlen)) {
			pr_err("alg: pcomp: Compression test %d failed for "
			       "%s\n", i + 1, algo);
			hexdump(result, ctemplate[i].outlen);
			return -EINVAL;
		}
	}

	for (i = 0; i < dtcount; i++) {
		struct comp_request req;
		unsigned int produced = 0;

		res = crypto_decompress_setup(tfm, dtemplate[i].params,
					      dtemplate[i].paramsize);
		if (res) {
			pr_err("alg: pcomp: decompression setup failed on "
			       "test %d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}

		res = crypto_decompress_init(tfm);
		if (res) {
			pr_err("alg: pcomp: decompression init failed on test "
			       "%d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}

		memset(result, 0, sizeof(result));

		req.next_in = dtemplate[i].input;
		req.avail_in = dtemplate[i].inlen / 2;
		req.next_out = result;
		req.avail_out = dtemplate[i].outlen / 2;

		res = crypto_decompress_update(tfm, &req);
		if (res < 0 && (res != -EAGAIN || req.avail_in)) {
			pr_err("alg: pcomp: decompression update failed on "
			       "test %d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		if (res > 0)
			produced += res;

		/* Add remaining input data */
		req.avail_in += (dtemplate[i].inlen + 1) / 2;

		res = crypto_decompress_update(tfm, &req);
		if (res < 0 && (res != -EAGAIN || req.avail_in)) {
			pr_err("alg: pcomp: decompression update failed on "
			       "test %d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		if (res > 0)
			produced += res;

		/* Provide remaining output space */
		req.avail_out += COMP_BUF_SIZE - dtemplate[i].outlen / 2;

		res = crypto_decompress_final(tfm, &req);
		if (res < 0 && (res != -EAGAIN || req.avail_in)) {
			pr_err("alg: pcomp: decompression final failed on "
			       "test %d for %s: error=%d\n", i + 1, algo, res);
			return res;
		}
		if (res > 0)
			produced += res;

		if (COMP_BUF_SIZE - req.avail_out != dtemplate[i].outlen) {
			pr_err("alg: comp: Decompression test %d failed for "
			       "%s: output len = %d (expected %d)\n", i + 1,
			       algo, COMP_BUF_SIZE - req.avail_out,
			       dtemplate[i].outlen);
			return -EINVAL;
		}

		if (produced != dtemplate[i].outlen) {
			pr_err("alg: comp: Decompression test %d failed for "
			       "%s: returned len = %u (expected %d)\n", i + 1,
			       algo, produced, dtemplate[i].outlen);
			return -EINVAL;
		}

		if (memcmp(result, dtemplate[i].output, dtemplate[i].outlen)) {
			pr_err("alg: pcomp: Decompression test %d failed for "
			       "%s\n", i + 1, algo);
			hexdump(result, dtemplate[i].outlen);
			return -EINVAL;
		}
	}

	return 0;
}

static int test_ablkcipher_jiffies(struct ablkcipher_request *req, int enc,
				   int sec, struct tcrypt_result *result,
				   int blen)
{
        unsigned long start, end;
        int bcount;
        int ret;

        for (start = jiffies, end = start + sec * HZ, bcount = 0;
             time_before(jiffies, end); bcount++) {

                if (enc)
                        ret = crypto_ablkcipher_encrypt(req);
                else
                        ret = crypto_ablkcipher_decrypt(req);

                switch (ret) {
                    case 0:
			break;
		    case -EINPROGRESS: 
		    case -EBUSY:
	                ret = wait_for_completion_interruptible(
                              &result->completion);
                         if (!ret && !((ret = result->err))) {
                              INIT_COMPLETION(result->completion);
                              break;
                         }
		    default:
                        printk("ERROR\n");
			return ret;
                }       
        }

        printk("%d operations in %d seconds (%ld bytes)\n",
               bcount, sec, (long)bcount * blen);
   
	return 0;
}

static int test_ablkcipher_cycles(struct ablkcipher_request *req, int enc,
				  int sec, struct tcrypt_result *result,
				  int blen)
{
        unsigned long cycles = 0;
        int ret = 0;
        int i;
        unsigned long start, end = 0;
        //local_bh_disable();
        //local_irq_disable();
        /* Warm-up run. */
        for (i = 0; i < 4; i++) {
                if (enc)
                        ret = crypto_ablkcipher_encrypt(req);
                else
                        ret = crypto_ablkcipher_decrypt(req);

                switch (ret) {
                    case 0:
			break;
		    case -EINPROGRESS: 
		    case -EBUSY:
#if 0
	                ret = wait_for_completion_interruptible(
                              &result->completion);
                        if (!ret && !((ret = result->err))) {
                              INIT_COMPLETION(result->completion);
                              break;
                        } 
#else

	                 wait_for_completion(&result->completion);
                         INIT_COMPLETION(result->completion);
                         break;
#endif
		    default:
                        printk("ERROR\n");
			return ret;
                }     
 
		if (signal_pending(current)) {
		    printk("Signal caught\n");
		    break;
		} 

        }

	//printk("Debug ln: (%d), fn: %s\n", __LINE__, __func__);
        /* The real thing. */
        for (i = 0; i < 8; i++) {
                end = 0;
                start = 0;
                start = read_c0_count();
                if (enc)
                        ret = crypto_ablkcipher_encrypt(req);
                else
                        ret = crypto_ablkcipher_decrypt(req);

                switch (ret) {
                    case 0:
			break;
		    case -EINPROGRESS: 
		    case -EBUSY:
#if 0
	                ret = wait_for_completion_interruptible(
                              &result->completion);
                	      end = get_cycles();
                         if (!ret && !((ret = result->err))) {
                              INIT_COMPLETION(result->completion);
                              break;
                         }
#else
	                 wait_for_completion(&result->completion);
                         end = read_c0_count();
                         INIT_COMPLETION(result->completion);
                         break;
#endif
		    default:
                        printk("ERROR\n");
			return ret;
                }       

		if (signal_pending(current)) {
		    printk("Signal caught\n");
		    break;
		} 
        
                cycles += end - start;
        }

       // local_irq_enable();
       // local_bh_enable();

        printk("1 operation in %lu cycles (%d bytes)\n",
               (cycles + 4) / 8, blen);

        return 0;

}

static u32 b_size[] = {16, 64, 256, 1024, 8192, 0};

static int test_skcipher_speed(struct crypto_ablkcipher *tfm, int enc,
			       struct cipher_speed_template *template, 
                               unsigned int tcount, unsigned int sec, 
                               u8* keysize)	
{
	const char *algo =
		crypto_tfm_alg_driver_name(crypto_ablkcipher_tfm(tfm));

	unsigned int i = 0, j, iv_len;
	struct ablkcipher_request *req;
	//struct scatterlist sg[8];
	const char *e;
	struct tcrypt_result result;
	char iv[MAX_IVLEN];
	static char *xbuf[XBUFSIZE];
	int ret = -ENOMEM;
        u32 *block_size;
        static char *tvmem_buf[4];
        const char *key;
	
	if (testmgr_alloc_buf(xbuf))
		goto out_nobuf;

	if (enc == ENCRYPT)
	        e = "encryption";
	else
		e = "decryption";

	init_completion(&result.completion);

        printk("Start ablkcipher speed test\n");

	req = ablkcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		printk(KERN_ERR "alg: skcipher: Failed to allocate request "
		       "for %s\n", algo);
		goto out;
	}
 
//	ablkcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
	ablkcipher_request_set_callback(req, 0,
					tcrypt_complete, &result);

        do {

            block_size = b_size;      

            do {
                struct scatterlist sg[4];
                if ((*keysize + *block_size) > 4 * PAGE_SIZE) {
                    printk("template (%u) too big for "
                          "tvmem_buf (%lu)\n", *keysize + *block_size,
                           4 * PAGE_SIZE);
                           goto out;
                }
		crypto_ablkcipher_clear_flags(tfm, ~0);

                printk("test %u (%d bit key, %d byte blocks): ", i,
                        *keysize * 8, *block_size);                

                memset(tvmem_buf[0], 0xff, PAGE_SIZE);
                key = tvmem_buf[0];

                for (j = 0; j < tcount; j++) {
                    if (template[j].klen == *keysize) {
                        key = template[j].key;
                        break;
                    }    
                }
                ret = crypto_ablkcipher_setkey(tfm, key, *keysize);
                if (ret) {
                    printk("Error setting of keys\n");
                    goto out;
                }

                sg_init_table(sg, 4); 

                for (j = 0; j < 4; j++) {
		    tvmem_buf[j] = xbuf[j];
		    memset(tvmem_buf[j], 0xff, PAGE_SIZE);
                    sg_set_buf(sg + j, tvmem_buf[j], PAGE_SIZE);
                }

		iv_len = crypto_ablkcipher_ivsize(tfm);
                if (iv_len) {
                    memset(&iv, 0xff, iv_len);
                }

		ablkcipher_request_set_crypt(req, sg, sg,
                                             *block_size, iv);
              
                //printk("Debug ln: %d, %s\n", __LINE__, __func__);
		if (sec) 
		    ret = test_ablkcipher_jiffies(req, enc, sec,
						  &result, *block_size);
                else
                    ret = test_ablkcipher_cycles(req, enc, sec,
						 &result, *block_size);
                
                
                if (ret) {
		    printk(KERN_ERR "alg: skcipher: %s failed on "
			   "test %d for %s: ret=%d\n", e, j, algo,
			   -ret);
                    goto out;
		}

                block_size++;
                i++;
            } while (*block_size);
	     keysize++;
        } while (*keysize);  

	ret = 0;
out:
        printk("End ablkcipher speed test\n");
	ablkcipher_request_free(req);
	testmgr_free_buf(xbuf);
#if 0
       if (!completion_done(&result->completion)) {
           printk("There are threads waiting for completion, completing all\n");
           complete_all(&result->completion); 
       }
#endif 

	//testmgr_free_buf(tvbuf);
out_nobuf:
	return ret;

}

static int test_cprng(struct crypto_rng *tfm, struct cprng_testvec *template,
		      unsigned int tcount)
{
	const char *algo = crypto_tfm_alg_driver_name(crypto_rng_tfm(tfm));
	int err = 0, i, j, seedsize;
	u8 *seed;
	char result[32];

	seedsize = crypto_rng_seedsize(tfm);

	seed = kmalloc(seedsize, GFP_KERNEL);
	if (!seed) {
		printk(KERN_ERR "alg: cprng: Failed to allocate seed space "
		       "for %s\n", algo);
		return -ENOMEM;
	}

	for (i = 0; i < tcount; i++) {
		memset(result, 0, 32);

		memcpy(seed, template[i].v, template[i].vlen);
		memcpy(seed + template[i].vlen, template[i].key,
		       template[i].klen);
		memcpy(seed + template[i].vlen + template[i].klen,
		       template[i].dt, template[i].dtlen);

		err = crypto_rng_reset(tfm, seed, seedsize);
		if (err) {
			printk(KERN_ERR "alg: cprng: Failed to reset rng "
			       "for %s\n", algo);
			goto out;
		}

		for (j = 0; j < template[i].loops; j++) {
			err = crypto_rng_get_bytes(tfm, result,
						   template[i].rlen);
			if (err != template[i].rlen) {
				printk(KERN_ERR "alg: cprng: Failed to obtain "
				       "the correct amount of random data for "
				       "%s (requested %d, got %d)\n", algo,
				       template[i].rlen, err);
				goto out;
			}
		}

		err = memcmp(result, template[i].result,
			     template[i].rlen);
		if (err) {
			printk(KERN_ERR "alg: cprng: Test %d failed for %s\n",
			       i, algo);
			hexdump(result, template[i].rlen);
			err = -EINVAL;
			goto out;
		}
	}

out:
	kfree(seed);
	return err;
}

static int alg_test_aead(const struct alg_test_desc *desc, const char *driver,
			 u32 type, u32 mask)
{
	struct crypto_aead *tfm;
	int err = 0;

	tfm = crypto_alloc_aead(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: aead: Failed to load transform for %s: "
		       "%ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	if (desc->suite.aead.enc.vecs) {
		err = test_aead(tfm, ENCRYPT, desc->suite.aead.enc.vecs,
				desc->suite.aead.enc.count);
		if (err)
			goto out;
	}

	if (!err && desc->suite.aead.dec.vecs)
		err = test_aead(tfm, DECRYPT, desc->suite.aead.dec.vecs,
				desc->suite.aead.dec.count);

out:
	crypto_free_aead(tfm);
	return err;
}

static int alg_test_cipher(const struct alg_test_desc *desc,
			   const char *driver, u32 type, u32 mask)
{
	struct crypto_cipher *tfm;
	int err = 0;

	tfm = crypto_alloc_cipher(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: cipher: Failed to load transform for "
		       "%s: %ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	if (desc->suite.cipher.enc.vecs) {
		err = test_cipher(tfm, ENCRYPT, desc->suite.cipher.enc.vecs,
				  desc->suite.cipher.enc.count);
		if (err)
			goto out;
	}

	if (desc->suite.cipher.dec.vecs)
		err = test_cipher(tfm, DECRYPT, desc->suite.cipher.dec.vecs,
				  desc->suite.cipher.dec.count);

out:
	crypto_free_cipher(tfm);
	return err;
}

static int alg_test_skcipher(const struct alg_test_desc *desc,
			     const char *driver, u32 type, u32 mask)
{
	struct crypto_ablkcipher *tfm;
	int err = 0;

	tfm = crypto_alloc_ablkcipher(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: skcipher: Failed to load transform for "
		       "%s: %ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	if (desc->suite.cipher.enc.vecs) {
		err = test_skcipher(tfm, ENCRYPT, desc->suite.cipher.enc.vecs,
				    desc->suite.cipher.enc.count);
		if (err)
			goto out;
	}

	if (desc->suite.cipher.dec.vecs)
		err = test_skcipher(tfm, DECRYPT, desc->suite.cipher.dec.vecs,
				    desc->suite.cipher.dec.count);

out:
	crypto_free_ablkcipher(tfm);
	return err;
}

static int alg_test_comp(const struct alg_test_desc *desc, const char *driver,
			 u32 type, u32 mask)
{
	struct crypto_comp *tfm;
	int err;

	tfm = crypto_alloc_comp(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: comp: Failed to load transform for %s: "
		       "%ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	err = test_comp(tfm, desc->suite.comp.comp.vecs,
			desc->suite.comp.decomp.vecs,
			desc->suite.comp.comp.count,
			desc->suite.comp.decomp.count);

	crypto_free_comp(tfm);
	return err;
}

static int alg_test_pcomp(const struct alg_test_desc *desc, const char *driver,
			  u32 type, u32 mask)
{
	struct crypto_pcomp *tfm;
	int err;

	tfm = crypto_alloc_pcomp(driver, type, mask);
	if (IS_ERR(tfm)) {
		pr_err("alg: pcomp: Failed to load transform for %s: %ld\n",
		       driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	err = test_pcomp(tfm, desc->suite.pcomp.comp.vecs,
			 desc->suite.pcomp.decomp.vecs,
			 desc->suite.pcomp.comp.count,
			 desc->suite.pcomp.decomp.count);

	crypto_free_pcomp(tfm);
	return err;
}

static int alg_test_hash(const struct alg_test_desc *desc, const char *driver,
			 u32 type, u32 mask)
{
	struct crypto_ahash *tfm;
	int err;

	tfm = crypto_alloc_ahash(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: hash: Failed to load transform for %s: "
		       "%ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}

	err = test_hash(tfm, desc->suite.hash.vecs, desc->suite.hash.count);

	crypto_free_ahash(tfm);
	return err;
}

static int alg_test_crc32c(const struct alg_test_desc *desc,
			   const char *driver, u32 type, u32 mask)
{
	struct crypto_shash *tfm;
	u32 val;
	int err;

	err = alg_test_hash(desc, driver, type, mask);
	if (err)
		goto out;

	tfm = crypto_alloc_shash(driver, type, mask);
	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: crc32c: Failed to load transform for %s: "
		       "%ld\n", driver, PTR_ERR(tfm));
		err = PTR_ERR(tfm);
		goto out;
	}

	do {
		struct {
			struct shash_desc shash;
			char ctx[crypto_shash_descsize(tfm)];
		} sdesc;

		sdesc.shash.tfm = tfm;
		sdesc.shash.flags = 0;

		*(u32 *)sdesc.ctx = le32_to_cpu(420553207);
		err = crypto_shash_final(&sdesc.shash, (u8 *)&val);
		if (err) {
			printk(KERN_ERR "alg: crc32c: Operation failed for "
			       "%s: %d\n", driver, err);
			break;
		}

		if (val != ~420553207) {
			printk(KERN_ERR "alg: crc32c: Test failed for %s: "
			       "%d\n", driver, val);
			err = -EINVAL;
		}
	} while (0);

	crypto_free_shash(tfm);

out:
	return err;
}

static int alg_test_cprng(const struct alg_test_desc *desc, const char *driver,
			  u32 type, u32 mask)
{
	struct crypto_rng *rng;
	int err = 0;

	rng = crypto_alloc_rng(driver, type, mask);
	if (IS_ERR(rng)) {
		printk(KERN_ERR "alg: cprng: Failed to load transform for %s: "
		       "%ld\n", driver, PTR_ERR(rng));
		return PTR_ERR(rng);
	}

	err = test_cprng(rng, desc->suite.cprng.vecs, desc->suite.cprng.count);

	crypto_free_rng(rng);

	return err;
}

/* Please keep this list sorted by algorithm name. */
static const struct alg_test_desc alg_test_descs[] = {
	{
		.alg = "ansi_cprng",
		.test = alg_test_cprng,
		.fips_allowed = 1,
		.suite = {
			.cprng = {
				.vecs = ansi_cprng_aes_tv_template,
				.count = ANSI_CPRNG_AES_TEST_VECTORS
			}
		}
	}, {
		.alg = "cbc(aes)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_cbc_enc_tv_template,
					.count = AES_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_cbc_dec_tv_template,
					.count = AES_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(anubis)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = anubis_cbc_enc_tv_template,
					.count = ANUBIS_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = anubis_cbc_dec_tv_template,
					.count = ANUBIS_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(blowfish)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = bf_cbc_enc_tv_template,
					.count = BF_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = bf_cbc_dec_tv_template,
					.count = BF_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(camellia)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = camellia_cbc_enc_tv_template,
					.count = CAMELLIA_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = camellia_cbc_dec_tv_template,
					.count = CAMELLIA_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(des)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = des_cbc_enc_tv_template,
					.count = DES_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = des_cbc_dec_tv_template,
					.count = DES_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(des3_ede)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = des3_ede_cbc_enc_tv_template,
					.count = DES3_EDE_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = des3_ede_cbc_dec_tv_template,
					.count = DES3_EDE_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cbc(twofish)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = tf_cbc_enc_tv_template,
					.count = TF_CBC_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = tf_cbc_dec_tv_template,
					.count = TF_CBC_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ccm(aes)",
		.test = alg_test_aead,
		.fips_allowed = 1,
		.suite = {
			.aead = {
				.enc = {
					.vecs = aes_ccm_enc_tv_template,
					.count = AES_CCM_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_ccm_dec_tv_template,
					.count = AES_CCM_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "crc32c",
		.test = alg_test_crc32c,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = crc32c_tv_template,
				.count = CRC32C_TEST_VECTORS
			}
		}
	}, {
		.alg = "ctr(aes)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_ctr_enc_tv_template,
					.count = AES_CTR_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_ctr_dec_tv_template,
					.count = AES_CTR_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "cts(cbc(aes))",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = cts_mode_enc_tv_template,
					.count = CTS_MODE_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = cts_mode_dec_tv_template,
					.count = CTS_MODE_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "deflate",
		.test = alg_test_comp,
		.suite = {
			.comp = {
				.comp = {
					.vecs = deflate_comp_tv_template,
					.count = DEFLATE_COMP_TEST_VECTORS
				},
				.decomp = {
					.vecs = deflate_decomp_tv_template,
					.count = DEFLATE_DECOMP_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(aes)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_enc_tv_template,
					.count = AES_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_dec_tv_template,
					.count = AES_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(anubis)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = anubis_enc_tv_template,
					.count = ANUBIS_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = anubis_dec_tv_template,
					.count = ANUBIS_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(arc4)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = arc4_enc_tv_template,
					.count = ARC4_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = arc4_dec_tv_template,
					.count = ARC4_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(blowfish)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = bf_enc_tv_template,
					.count = BF_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = bf_dec_tv_template,
					.count = BF_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(camellia)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = camellia_enc_tv_template,
					.count = CAMELLIA_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = camellia_dec_tv_template,
					.count = CAMELLIA_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(cast5)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = cast5_enc_tv_template,
					.count = CAST5_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = cast5_dec_tv_template,
					.count = CAST5_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(cast6)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = cast6_enc_tv_template,
					.count = CAST6_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = cast6_dec_tv_template,
					.count = CAST6_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(des)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = des_enc_tv_template,
					.count = DES_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = des_dec_tv_template,
					.count = DES_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(des3_ede)",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = des3_ede_enc_tv_template,
					.count = DES3_EDE_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = des3_ede_dec_tv_template,
					.count = DES3_EDE_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(khazad)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = khazad_enc_tv_template,
					.count = KHAZAD_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = khazad_dec_tv_template,
					.count = KHAZAD_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(seed)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = seed_enc_tv_template,
					.count = SEED_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = seed_dec_tv_template,
					.count = SEED_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(serpent)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = serpent_enc_tv_template,
					.count = SERPENT_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = serpent_dec_tv_template,
					.count = SERPENT_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(tea)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = tea_enc_tv_template,
					.count = TEA_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = tea_dec_tv_template,
					.count = TEA_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(tnepres)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = tnepres_enc_tv_template,
					.count = TNEPRES_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = tnepres_dec_tv_template,
					.count = TNEPRES_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(twofish)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = tf_enc_tv_template,
					.count = TF_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = tf_dec_tv_template,
					.count = TF_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(xeta)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = xeta_enc_tv_template,
					.count = XETA_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = xeta_dec_tv_template,
					.count = XETA_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "ecb(xtea)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = xtea_enc_tv_template,
					.count = XTEA_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = xtea_dec_tv_template,
					.count = XTEA_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "gcm(aes)",
		.test = alg_test_aead,
		.fips_allowed = 1,
		.suite = {
			.aead = {
				.enc = {
					.vecs = aes_gcm_enc_tv_template,
					.count = AES_GCM_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_gcm_dec_tv_template,
					.count = AES_GCM_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "hmac(md5)",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = hmac_md5_tv_template,
				.count = HMAC_MD5_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(rmd128)",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = hmac_rmd128_tv_template,
				.count = HMAC_RMD128_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(rmd160)",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = hmac_rmd160_tv_template,
				.count = HMAC_RMD160_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(sha1)",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = hmac_sha1_tv_template,
				.count = HMAC_SHA1_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(sha224)",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = hmac_sha224_tv_template,
				.count = HMAC_SHA224_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(sha256)",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = hmac_sha256_tv_template,
				.count = HMAC_SHA256_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(sha384)",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = hmac_sha384_tv_template,
				.count = HMAC_SHA384_TEST_VECTORS
			}
		}
	}, {
		.alg = "hmac(sha512)",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = hmac_sha512_tv_template,
				.count = HMAC_SHA512_TEST_VECTORS
			}
		}
#if !defined(CONFIG_CRYPTO_DEV_AES) && !defined(CONFIG_CRYPTO_ASYNC_AES)
	}, {
		.alg = "lrw(aes)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_lrw_enc_tv_template,
					.count = AES_LRW_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_lrw_dec_tv_template,
					.count = AES_LRW_DEC_TEST_VECTORS
				}
			}
		}
#endif
	}, {
		.alg = "lzo",
		.test = alg_test_comp,
		.suite = {
			.comp = {
				.comp = {
					.vecs = lzo_comp_tv_template,
					.count = LZO_COMP_TEST_VECTORS
				},
				.decomp = {
					.vecs = lzo_decomp_tv_template,
					.count = LZO_DECOMP_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "md4",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = md4_tv_template,
				.count = MD4_TEST_VECTORS
			}
		}
	}, {
		.alg = "md5",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = md5_tv_template,
				.count = MD5_TEST_VECTORS
			}
		}
	}, {
		.alg = "michael_mic",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = michael_mic_tv_template,
				.count = MICHAEL_MIC_TEST_VECTORS
			}
		}
	}, {
		.alg = "pcbc(fcrypt)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = fcrypt_pcbc_enc_tv_template,
					.count = FCRYPT_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = fcrypt_pcbc_dec_tv_template,
					.count = FCRYPT_DEC_TEST_VECTORS
				}
			}
		}

	}, {
		.alg = "rfc3686(ctr(aes))",
		.test = alg_test_skcipher,
		.fips_allowed = 1,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_ctr_rfc3686_enc_tv_template,
					.count = AES_CTR_3686_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_ctr_rfc3686_dec_tv_template,
					.count = AES_CTR_3686_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "rfc4309(ccm(aes))",
		.test = alg_test_aead,
		.fips_allowed = 1,
		.suite = {
			.aead = {
				.enc = {
					.vecs = aes_ccm_rfc4309_enc_tv_template,
					.count = AES_CCM_4309_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_ccm_rfc4309_dec_tv_template,
					.count = AES_CCM_4309_DEC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "rmd128",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = rmd128_tv_template,
				.count = RMD128_TEST_VECTORS
			}
		}
	}, {
		.alg = "rmd160",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = rmd160_tv_template,
				.count = RMD160_TEST_VECTORS
			}
		}
	}, {
		.alg = "rmd256",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = rmd256_tv_template,
				.count = RMD256_TEST_VECTORS
			}
		}
	}, {
		.alg = "rmd320",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = rmd320_tv_template,
				.count = RMD320_TEST_VECTORS
			}
		}
	}, {
		.alg = "salsa20",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = salsa20_stream_enc_tv_template,
					.count = SALSA20_STREAM_ENC_TEST_VECTORS
				}
			}
		}
	}, {
		.alg = "sha1",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = sha1_tv_template,
				.count = SHA1_TEST_VECTORS
			}
		}
	}, {
		.alg = "sha224",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = sha224_tv_template,
				.count = SHA224_TEST_VECTORS
			}
		}
	}, {
		.alg = "sha256",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = sha256_tv_template,
				.count = SHA256_TEST_VECTORS
			}
		}
	}, {
		.alg = "sha384",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = sha384_tv_template,
				.count = SHA384_TEST_VECTORS
			}
		}
	}, {
		.alg = "sha512",
		.test = alg_test_hash,
		.fips_allowed = 1,
		.suite = {
			.hash = {
				.vecs = sha512_tv_template,
				.count = SHA512_TEST_VECTORS
			}
		}
	}, {
		.alg = "tgr128",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = tgr128_tv_template,
				.count = TGR128_TEST_VECTORS
			}
		}
	}, {
		.alg = "tgr160",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = tgr160_tv_template,
				.count = TGR160_TEST_VECTORS
			}
		}
	}, {
		.alg = "tgr192",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = tgr192_tv_template,
				.count = TGR192_TEST_VECTORS
			}
		}
	}, {
		.alg = "vmac(aes)",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = aes_vmac128_tv_template,
				.count = VMAC_AES_TEST_VECTORS
			}
		}
	}, {
		.alg = "wp256",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = wp256_tv_template,
				.count = WP256_TEST_VECTORS
			}
		}
	}, {
		.alg = "wp384",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = wp384_tv_template,
				.count = WP384_TEST_VECTORS
			}
		}
	}, {
		.alg = "wp512",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = wp512_tv_template,
				.count = WP512_TEST_VECTORS
			}
		}
	}, {
		.alg = "xcbc(aes)",
		.test = alg_test_hash,
		.suite = {
			.hash = {
				.vecs = aes_xcbc128_tv_template,
				.count = XCBC_AES_TEST_VECTORS
			}
		}
#if 0
	}, {
		.alg = "xts(aes)",
		.test = alg_test_skcipher,
		.suite = {
			.cipher = {
				.enc = {
					.vecs = aes_xts_enc_tv_template,
					.count = AES_XTS_ENC_TEST_VECTORS
				},
				.dec = {
					.vecs = aes_xts_dec_tv_template,
					.count = AES_XTS_DEC_TEST_VECTORS
				}
			}
		}
#endif
	}, {
		.alg = "zlib",
		.test = alg_test_pcomp,
		.suite = {
			.pcomp = {
				.comp = {
					.vecs = zlib_comp_tv_template,
					.count = ZLIB_COMP_TEST_VECTORS
				},
				.decomp = {
					.vecs = zlib_decomp_tv_template,
					.count = ZLIB_DECOMP_TEST_VECTORS
				}
			}
		}
	}
};

static int alg_find_test(const char *alg)
{
	int start = 0;
	int end = ARRAY_SIZE(alg_test_descs);

	while (start < end) {
		int i = (start + end) / 2;
		int diff = strcmp(alg_test_descs[i].alg, alg);

		if (diff > 0) {
			end = i;
			continue;
		}

		if (diff < 0) {
			start = i + 1;
			continue;
		}

		return i;
	}

	return -1;
}

static int ifx_alg_test(const char *driver, const char *alg, u32 type, u32 mask)
{
	int i;
	int j;
	int rc;

	if ((type & CRYPTO_ALG_TYPE_MASK) == CRYPTO_ALG_TYPE_CIPHER) {
		char nalg[CRYPTO_MAX_ALG_NAME];

		if (snprintf(nalg, sizeof(nalg), "ecb(%s)", alg) >=
		    sizeof(nalg))
			return -ENAMETOOLONG;

		i = alg_find_test(nalg);
		if (i < 0)
			goto notest;

		if (fips_enabled && !alg_test_descs[i].fips_allowed)
			goto non_fips_alg;

		rc = alg_test_cipher(alg_test_descs + i, driver, type, mask);
		goto test_done;
	}

	i = alg_find_test(alg);
	j = alg_find_test(driver);
	if (i < 0 && j < 0)
		goto notest;

	if (fips_enabled && ((i >= 0 && !alg_test_descs[i].fips_allowed) ||
			     (j >= 0 && !alg_test_descs[j].fips_allowed)))
		goto non_fips_alg;

	rc = 0;
	if (i >= 0)
		rc |= alg_test_descs[i].test(alg_test_descs + i, driver,
					     type, mask);
	if (j >= 0)
		rc |= alg_test_descs[j].test(alg_test_descs + j, driver,
					     type, mask);

test_done:
	if (fips_enabled && rc)
		panic("%s: %s alg self test failed in fips mode!\n", driver, alg);

	if (fips_enabled && !rc)
		printk(KERN_INFO "alg: self-tests for %s (%s) passed\n",
		       driver, alg);

	return rc;

notest:
	printk(KERN_INFO "alg: No test for %s (%s)\n", alg, driver);
	return 0;
non_fips_alg:
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(ifx_alg_test);

/* Modified speed test for async block cipher mode*/

static int ifx_alg_speed_test(const char *driver, const char *alg, 
		   unsigned int sec,
		   struct cipher_speed_template *template,
		   unsigned int tcount, u8 *keysize)
{
	int i;
	int j;
	int err;
	int type = 0, mask = 0;
	struct crypto_ablkcipher *tfm;

	i = alg_find_test(alg);
	j = alg_find_test(driver);

	if (i < 0 && j < 0)
		goto notest;

	if (fips_enabled && ((i >= 0 && !alg_test_descs[i].fips_allowed) ||
			     (j >= 0 && !alg_test_descs[j].fips_allowed)))
		goto non_fips_alg;

	tfm = crypto_alloc_ablkcipher(driver, type, mask);

	if (IS_ERR(tfm)) {
		printk(KERN_ERR "alg: skcipher: Failed to load transform for "
		       "%s: %ld\n", driver, PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
        err = test_skcipher_speed(tfm, ENCRYPT, template,
               	            	  tcount, sec, keysize);
        if (err)
           goto test_done;

       	err = test_skcipher_speed(tfm, DECRYPT, template,
               	                   tcount, sec, keysize);
	if (!err)
	   goto test_done;

notest:
	return 0;
non_fips_alg:
	return -EINVAL;

test_done:
	if (fips_enabled && err)
		panic("%s: %s alg self test failed in fips mode!\n", driver, alg);

	if (fips_enabled && !err)
		printk(KERN_INFO "alg: self-tests for %s (%s) passed\n",
		       driver, alg);

        crypto_free_ablkcipher(tfm);
	return err;
}
EXPORT_SYMBOL_GPL(ifx_alg_speed_test);


static int test_cipher_jiffies(struct blkcipher_desc *desc, int enc,
			       struct scatterlist *sg, int blen, int sec)
{
	unsigned long start, end;
	int bcount;
	int ret;

	for (start = jiffies, end = start + sec * HZ, bcount = 0;
	     time_before(jiffies, end); bcount++) {
		if (enc)
			ret = crypto_blkcipher_encrypt(desc, sg, sg, blen);
		else
			ret = crypto_blkcipher_decrypt(desc, sg, sg, blen);

		if (ret)
			return ret;
	}

	printk("%d operations in %d seconds (%ld bytes)\n",
	       bcount, sec, (long)bcount * blen);
	return 0;
}

static int test_cipher_cycles(struct blkcipher_desc *desc, int enc,
			      struct scatterlist *sg, int blen)
{
	unsigned long cycles = 0;
        unsigned long start, end;
	int ret = 0;
	int i;

	local_bh_disable();
	local_irq_disable();

	/* Warm-up run. */
	for (i = 0; i < 4; i++) {
		if (enc)
			ret = crypto_blkcipher_encrypt(desc, sg, sg, blen);
		else
			ret = crypto_blkcipher_decrypt(desc, sg, sg, blen);

		if (ret)
			goto out;
	}

	/* The real thing. */
	for (i = 0; i < 8; i++) {
                /* Original code to get cycles, does not work with MIPS
		 * cycles_t start, end;
                 * start = get_cycles();
                 */
                
                start = read_c0_count(); // LQ modified tcrypt      

		if (enc)
			ret = crypto_blkcipher_encrypt(desc, sg, sg, blen);
		else
			ret = crypto_blkcipher_decrypt(desc, sg, sg, blen);
		
                /* Original code to get cycles, does not work with MIPS
                 * end = get_cycles();
                 */

                end = read_c0_count(); //LQ modified tcrypt

		if (ret)
			goto out;

		cycles += end - start;
	}

out:
	local_irq_enable();
	local_bh_enable();

	if (ret == 0)
		printk("1 operation in %lu cycles (%d bytes)\n",
		       (cycles + 4) / 8, blen);

	return ret;
}

static u32 block_sizes[] = { 16, 64, 256, 1024, 8192, 0 };

static void test_cipher_speed(const char *algo, int enc, unsigned int sec,
			      struct cipher_speed_template *template,
			      unsigned int tcount, u8 *keysize)
{
	unsigned int ret, i, j, iv_len;
	const char *key, iv[128];
	struct crypto_blkcipher *tfm;
	struct blkcipher_desc desc;
	const char *e;
	u32 *b_size;

	if (enc == ENCRYPT)
	        e = "encryption";
	else
		e = "decryption";

	printk("\n ******* testing speed of %s %s ******* \n", algo, e);

	tfm = crypto_alloc_blkcipher(algo, 0, CRYPTO_ALG_ASYNC);

	if (IS_ERR(tfm)) {
		printk("failed to load transform for %s: %ld\n", algo,
		       PTR_ERR(tfm));
		return;
	}
	desc.tfm = tfm;
	desc.flags = 0;

	i = 0;
	do {

		b_size = block_sizes;
		do {
			struct scatterlist sg[TVMEMSIZE];

			if ((*keysize + *b_size) > TVMEMSIZE * PAGE_SIZE) {
				printk("template (%u) too big for "
				       "tvmem (%lu)\n", *keysize + *b_size,
				       TVMEMSIZE * PAGE_SIZE);
				goto out;
			}

			printk("test %u (%d bit key, %d byte blocks): ", i,
					*keysize * 8, *b_size);

			memset(tvmem[0], 0xff, PAGE_SIZE);

			/* set key, plain text and IV */
			key = tvmem[0];
			for (j = 0; j < tcount; j++) {
				if (template[j].klen == *keysize) {
					key = template[j].key;
					break;
				}
			}

			ret = crypto_blkcipher_setkey(tfm, key, *keysize);
			if (ret) {
				printk("setkey() failed flags=%x\n",
						crypto_blkcipher_get_flags(tfm));
				goto out;
			}

			sg_init_table(sg, TVMEMSIZE);
			sg_set_buf(sg, tvmem[0] + *keysize,
				   PAGE_SIZE - *keysize);
			for (j = 1; j < TVMEMSIZE; j++) {
				sg_set_buf(sg + j, tvmem[j], PAGE_SIZE);
				memset (tvmem[j], 0xff, PAGE_SIZE);
			}

			iv_len = crypto_blkcipher_ivsize(tfm);
			if (iv_len) {
				memset(&iv, 0xff, iv_len);
				crypto_blkcipher_set_iv(tfm, iv, iv_len);
			}

			if (sec)
				ret = test_cipher_jiffies(&desc, enc, sg,
							  *b_size, sec);
			else
				ret = test_cipher_cycles(&desc, enc, sg,
							 *b_size);

			if (ret) {
				printk("%s() failed flags=%x\n", e, desc.flags);
				break;
			}
			b_size++;
			i++;
		} while (*b_size);
		keysize++;
	} while (*keysize);

out:
	crypto_free_blkcipher(tfm);
}

static int test_hash_jiffies_digest(struct hash_desc *desc,
				    struct scatterlist *sg, int blen,
				    char *out, int sec)
{
	unsigned long start, end;
	int bcount;
	int ret;

	for (start = jiffies, end = start + sec * HZ, bcount = 0;
	     time_before(jiffies, end); bcount++) {
		ret = crypto_hash_digest(desc, sg, blen, out);
		if (ret)
			return ret;
	}

	printk("%6u opers/sec, %9lu bytes/sec\n",
	       bcount / sec, ((long)bcount * blen) / sec);

	return 0;
}

static int test_hash_jiffies(struct hash_desc *desc, struct scatterlist *sg,
			     int blen, int plen, char *out, int sec)
{
	unsigned long start, end;
	int bcount, pcount;
	int ret;

	if (plen == blen)
		return test_hash_jiffies_digest(desc, sg, blen, out, sec);

	for (start = jiffies, end = start + sec * HZ, bcount = 0;
	     time_before(jiffies, end); bcount++) {
		ret = crypto_hash_init(desc);
		if (ret)
			return ret;
		for (pcount = 0; pcount < blen; pcount += plen) {
			ret = crypto_hash_update(desc, sg, plen);
			if (ret)
				return ret;
		}
		/* we assume there is enough space in 'out' for the result */
		ret = crypto_hash_final(desc, out);
		if (ret)
			return ret;
	}

	printk("%6u opers/sec, %9lu bytes/sec\n",
	       bcount / sec, ((long)bcount * blen) / sec);

	return 0;
}

static int test_hash_cycles_digest(struct hash_desc *desc,
				   struct scatterlist *sg, int blen, char *out)
{
	unsigned long cycles = 0;
        unsigned long start, end;
	int i;
	int ret;

	local_bh_disable();
	local_irq_disable();

	/* Warm-up run. */
	for (i = 0; i < 4; i++) {
		ret = crypto_hash_digest(desc, sg, blen, out);
		if (ret)
			goto out;
	}

	/* The real thing. */
	for (i = 0; i < 8; i++) {
                
                /* Original code to get cycles, does not work with MIPS
		 * cycles_t start, end;
                 * start = get_cycles();
                 */

                start = read_c0_count(); // LQ modified tcrypt

		ret = crypto_hash_digest(desc, sg, blen, out);
		if (ret)
			goto out;

                /* Original code to get cycles, does not work with MIPS
		 * end = get_cycles();
                 */

                 end = read_c0_count(); // LQ modified tcrypt

		cycles += end - start;
	}

out:
	local_irq_enable();
	local_bh_enable();

	if (ret)
		return ret;

	printk("%6lu cycles/operation, %4lu cycles/byte\n",
	       cycles / 8, cycles / (8 * blen));

	return 0;
}

static int test_hash_cycles(struct hash_desc *desc, struct scatterlist *sg,
			    int blen, int plen, char *out)
{
	unsigned long cycles = 0;
        unsigned long start, end;
	int i, pcount;
	int ret;

	if (plen == blen)
		return test_hash_cycles_digest(desc, sg, blen, out);

	local_bh_disable();
	local_irq_disable();

	/* Warm-up run. */
	for (i = 0; i < 4; i++) {
		ret = crypto_hash_init(desc);
		if (ret)
			goto out;
		for (pcount = 0; pcount < blen; pcount += plen) {
			ret = crypto_hash_update(desc, sg, plen);
			if (ret)
				goto out;
		}
		ret = crypto_hash_final(desc, out);
		if (ret)
			goto out;
	}

	/* The real thing. */
	for (i = 0; i < 8; i++) {
              
                /* Original code for getting cycles, not working for MIPS
                 * cycle_t start, end;
		 * end = get_cycles();
                 */
                
		start = read_c0_count(); // LQ modified tcrypt

		ret = crypto_hash_init(desc);
		if (ret)
			goto out;
		for (pcount = 0; pcount < blen; pcount += plen) {
			ret = crypto_hash_update(desc, sg, plen);
			if (ret)
				goto out;
		}
		ret = crypto_hash_final(desc, out);
		if (ret)
			goto out;

                /* Original code for getting cycles, not working for MIPS
		 * end = get_cycles();
                 */ 

                end = read_c0_count(); // LQ modified tcrypt

		cycles += end - start;
	}

out:
	local_irq_enable();
	local_bh_enable();

	if (ret)
		return ret;

	printk("%6lu cycles/operation, %4lu cycles/byte\n",
	       cycles / 8, cycles / (8 * blen));

	return 0;
}

static void test_hash_speed(const char *algo, unsigned int sec,
			    struct hash_speed *speed)
{
	struct scatterlist sg[TVMEMSIZE];
	struct crypto_hash *tfm;
	struct hash_desc desc;
	static char output[1024];
	int i;
	int ret;

	printk(KERN_INFO "\ntesting speed of %s\n", algo);

	tfm = crypto_alloc_hash(algo, 0, CRYPTO_ALG_ASYNC);

	if (IS_ERR(tfm)) {
		printk(KERN_ERR "failed to load transform for %s: %ld\n", algo,
		       PTR_ERR(tfm));
		return;
	}

	desc.tfm = tfm;
	desc.flags = 0;

	if (crypto_hash_digestsize(tfm) > sizeof(output)) {
		printk(KERN_ERR "digestsize(%u) > outputbuffer(%zu)\n",
		       crypto_hash_digestsize(tfm), sizeof(output));
		goto out;
	}

	sg_init_table(sg, TVMEMSIZE);
	for (i = 0; i < TVMEMSIZE; i++) {
		sg_set_buf(sg + i, tvmem[i], PAGE_SIZE);
		memset(tvmem[i], 0xff, PAGE_SIZE);
	}

	for (i = 0; speed[i].blen != 0; i++) {
		if (speed[i].blen > TVMEMSIZE * PAGE_SIZE) {
			printk(KERN_ERR
			       "template (%u) too big for tvmem (%lu)\n",
			       speed[i].blen, TVMEMSIZE * PAGE_SIZE);
			goto out;
		}

		printk(KERN_INFO "test%3u "
		       "(%5u byte blocks,%5u bytes per update,%4u updates): ",
		       i, speed[i].blen, speed[i].plen, speed[i].blen / speed[i].plen);

		if (sec)
			ret = test_hash_jiffies(&desc, sg, speed[i].blen,
						speed[i].plen, output, sec);
		else
			ret = test_hash_cycles(&desc, sg, speed[i].blen,
					       speed[i].plen, output);

		if (ret) {
			printk(KERN_ERR "hashing failed ret=%d\n", ret);
			break;
		}
	}

out:
	crypto_free_hash(tfm);
}


static void test_available(void)
{
	char **name = check;

	while (*name) {
		printk("alg %s ", *name);
		printk(crypto_has_alg(*name, 0, 0) ?
		       "found\n" : "not found\n");
		name++;
	}
}

static inline int tcrypt_test(const char *alg)
{
	int ret;

	printk("Running test %s\n", alg);
	ret = ifx_alg_test(alg, alg, 0, 0);
	/* non-fips algs return -EINVAL in fips mode */
	if (fips_enabled && ret == -EINVAL)
		ret = 0;
	return ret;
}

static inline int tcrypt_speedtest(const char *alg,
				   struct cipher_speed_template *template,
				   unsigned int tcount, u8 *keysize)
{
	int ret;

        printk("[****** Running speedtest %s *******]\n", alg);	
	ret = ifx_alg_speed_test(alg, alg, sec, template, tcount, keysize);
        if (fips_enabled && ret == -EINVAL)
                ret = 0;
        return ret;
}


static int do_test(int m)
{
	int i;
	int ret = 0;

	switch (m) {
	case 0:
		for (i = 1; i < 200; i++)
			ret += do_test(i);
		break;

	case 1:
		ret += tcrypt_test("md5");
		break;

	case 2:
		ret += tcrypt_test("sha1");
		break;

	case 3:
		ret += tcrypt_test("ecb(des)");
		ret += tcrypt_test("cbc(des)");
		break;

	case 4:
		ret += tcrypt_test("ecb(des3_ede)");
		ret += tcrypt_test("cbc(des3_ede)");
		break;

	case 5:
		ret += tcrypt_test("md4");
		break;

	case 6:
		ret += tcrypt_test("sha256");
		break;

	case 7:
		ret += tcrypt_test("ecb(blowfish)");
		ret += tcrypt_test("cbc(blowfish)");
		break;

	case 8:
		ret += tcrypt_test("ecb(twofish)");
		ret += tcrypt_test("cbc(twofish)");
		break;

	case 9:
		ret += tcrypt_test("ecb(serpent)");
		break;

	case 10:
		ret += tcrypt_test("ecb(aes)");
		ret += tcrypt_test("cbc(aes)");
	//	ret += tcrypt_test("lrw(aes)");
	//	ret += tcrypt_test("xts(aes)");
		ret += tcrypt_test("ctr(aes)");
		ret += tcrypt_test("rfc3686(ctr(aes))");
		break;

	case 11:
		ret += tcrypt_test("sha384");
		break;

	case 12:
		ret += tcrypt_test("sha512");
		break;

	case 13:
		ret += tcrypt_test("deflate");
		break;

	case 14:
		ret += tcrypt_test("ecb(cast5)");
		break;

	case 15:
		ret += tcrypt_test("ecb(cast6)");
		break;

	case 16:
		ret += tcrypt_test("ecb(arc4)");
		break;

	case 17:
		ret += tcrypt_test("michael_mic");
		break;

	case 18:
		ret += tcrypt_test("crc32c");
		break;

	case 19:
		ret += tcrypt_test("ecb(tea)");
		break;

	case 20:
		ret += tcrypt_test("ecb(xtea)");
		break;

	case 21:
		ret += tcrypt_test("ecb(khazad)");
		break;

	case 22:
		ret += tcrypt_test("wp512");
		break;

	case 23:
		ret += tcrypt_test("wp384");
		break;

	case 24:
		ret += tcrypt_test("wp256");
		break;

	case 25:
		ret += tcrypt_test("ecb(tnepres)");
		break;

	case 26:
		ret += tcrypt_test("ecb(anubis)");
		ret += tcrypt_test("cbc(anubis)");
		break;

	case 27:
		ret += tcrypt_test("tgr192");
		break;

	case 28:

		ret += tcrypt_test("tgr160");
		break;

	case 29:
		ret += tcrypt_test("tgr128");
		break;

	case 30:
		ret += tcrypt_test("ecb(xeta)");
		break;

	case 31:
		ret += tcrypt_test("pcbc(fcrypt)");
		break;

	case 32:
		ret += tcrypt_test("ecb(camellia)");
		ret += tcrypt_test("cbc(camellia)");
		break;
	case 33:
		ret += tcrypt_test("sha224");
		break;

	case 34:
		ret += tcrypt_test("salsa20");
		break;

	case 35:
		ret += tcrypt_test("gcm(aes)");
		break;

	case 36:
		ret += tcrypt_test("lzo");
		break;

	case 37:
		ret += tcrypt_test("ccm(aes)");
		break;

	case 38:
		ret += tcrypt_test("cts(cbc(aes))");
		break;

        case 39:
		ret += tcrypt_test("rmd128");
		break;

        case 40:
		ret += tcrypt_test("rmd160");
		break;

	case 41:
		ret += tcrypt_test("rmd256");
		break;

	case 42:
		ret += tcrypt_test("rmd320");
		break;

	case 43:
		ret += tcrypt_test("ecb(seed)");
		break;

	case 44:
		ret += tcrypt_test("zlib");
		break;

	case 45:
		ret += tcrypt_test("rfc4309(ccm(aes))");
		break;

	case 100:
		ret += tcrypt_test("hmac(md5)");
		break;

	case 101:
		ret += tcrypt_test("hmac(sha1)");
		break;

	case 102:
		ret += tcrypt_test("hmac(sha256)");
		break;

	case 103:
		ret += tcrypt_test("hmac(sha384)");
		break;

	case 104:
		ret += tcrypt_test("hmac(sha512)");
		break;

	case 105:
		ret += tcrypt_test("hmac(sha224)");
		break;

	case 106:
		ret += tcrypt_test("xcbc(aes)");
		break;

	case 107:
		ret += tcrypt_test("hmac(rmd128)");
		break;

	case 108:
		ret += tcrypt_test("hmac(rmd160)");
		break;

	case 109:
		ret += tcrypt_test("vmac(aes)");
		break;

	case 150:
		ret += tcrypt_test("ansi_cprng");
		break;

	case 200:
		test_cipher_speed("ecb(aes)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("ecb(aes)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(aes)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(aes)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
#if !defined(CONFIG_CRYPTO_DEV_AES) && !defined(CONFIG_CRYPTO_ASYNC_AES)
		test_cipher_speed("lrw(aes)", ENCRYPT, sec, NULL, 0,
				speed_template_32_40_48);
		test_cipher_speed("lrw(aes)", DECRYPT, sec, NULL, 0,
				speed_template_32_40_48);
		test_cipher_speed("xts(aes)", ENCRYPT, sec, NULL, 0,
				speed_template_32_48_64);
		test_cipher_speed("xts(aes)", DECRYPT, sec, NULL, 0,
				speed_template_32_48_64);
#endif
		break;

	case 201:
		test_cipher_speed("ecb(des3_ede)", ENCRYPT, sec,
				des3_speed_template, DES3_SPEED_VECTORS,
				speed_template_24);
		test_cipher_speed("ecb(des3_ede)", DECRYPT, sec,
				des3_speed_template, DES3_SPEED_VECTORS,
				speed_template_24);
		test_cipher_speed("cbc(des3_ede)", ENCRYPT, sec,
				des3_speed_template, DES3_SPEED_VECTORS,
				speed_template_24);
		test_cipher_speed("cbc(des3_ede)", DECRYPT, sec,
				des3_speed_template, DES3_SPEED_VECTORS,
				speed_template_24);
		break;

	case 202:
		test_cipher_speed("ecb(twofish)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("ecb(twofish)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(twofish)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(twofish)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		break;

	case 203:
		test_cipher_speed("ecb(blowfish)", ENCRYPT, sec, NULL, 0,
				  speed_template_8_32);
		test_cipher_speed("ecb(blowfish)", DECRYPT, sec, NULL, 0,
				  speed_template_8_32);
		test_cipher_speed("cbc(blowfish)", ENCRYPT, sec, NULL, 0,
				  speed_template_8_32);
		test_cipher_speed("cbc(blowfish)", DECRYPT, sec, NULL, 0,
				  speed_template_8_32);
		break;

	case 204:
		test_cipher_speed("ecb(des)", ENCRYPT, sec, NULL, 0,
				  speed_template_8);
		test_cipher_speed("ecb(des)", DECRYPT, sec, NULL, 0,
				  speed_template_8);
		test_cipher_speed("cbc(des)", ENCRYPT, sec, NULL, 0,
				  speed_template_8);
		test_cipher_speed("cbc(des)", DECRYPT, sec, NULL, 0,
				  speed_template_8);
		break;

	case 205:
		test_cipher_speed("ecb(camellia)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("ecb(camellia)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(camellia)", ENCRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		test_cipher_speed("cbc(camellia)", DECRYPT, sec, NULL, 0,
				speed_template_16_24_32);
		break;

	case 206:
		test_cipher_speed("salsa20", ENCRYPT, sec, NULL, 0,
				  speed_template_16_32);
		break;

	case 300:
		/* fall through */

	case 301:
		test_hash_speed("md4", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 302:
		test_hash_speed("md5", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 303:
		test_hash_speed("sha1", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 304:
		test_hash_speed("sha256", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 305:
		test_hash_speed("sha384", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 306:
		test_hash_speed("sha512", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 307:
		test_hash_speed("wp256", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 308:
		test_hash_speed("wp384", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 309:
		test_hash_speed("wp512", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 310:
		test_hash_speed("tgr128", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 311:
		test_hash_speed("tgr160", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 312:
		test_hash_speed("tgr192", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 313:
		test_hash_speed("sha224", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 314:
		test_hash_speed("rmd128", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 315:
		test_hash_speed("rmd160", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 316:
		test_hash_speed("rmd256", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 317:
		test_hash_speed("rmd320", sec, generic_hash_speed_template);
		if (mode > 300 && mode < 400) break;

	case 399:
		break;

        /* Modified speed test for async block cipher mode */
        case 400:
	  	tcrypt_speedtest("ecb(aes)", NULL, 0,
				 speed_template_16_24_32);
		tcrypt_speedtest("cbc(aes)", NULL, 0,
				speed_template_16_24_32);
		break;

	case 401:
		tcrypt_speedtest("ecb(des3_ede)", des3_speed_template, 
				DES3_SPEED_VECTORS,speed_template_24);
		tcrypt_speedtest("cbc(des3_ede)", des3_speed_template, 
				DES3_SPEED_VECTORS,speed_template_24);
		break;

	case 404:
		tcrypt_speedtest("ecb(des)", NULL, 0, 
				speed_template_8);
		tcrypt_speedtest("cbc(des)", NULL, 0, 
				speed_template_8);
		break;

	case 1000:
		test_available();
		break;
	}

	return ret;
}
#if !defined(CONFIG_CRYPTO_DEV_DEU)
static int do_alg_test(const char *alg, u32 type, u32 mask)
{
	return crypto_has_alg(alg, type, mask ?: CRYPTO_ALG_TYPE_MASK) ?
	       0 : -ENOENT;
}
#endif

static int __init tcrypt_mod_init(void)
{
	int err = -ENOMEM;
	int i;

	printk("Starting Lantiq DEU Crypto TESTS . . . . . . .\n");

	for (i = 0; i < TVMEMSIZE; i++) {
		tvmem[i] = (void *)__get_free_page(GFP_KERNEL);
		if (!tvmem[i])
			goto err_free_tv;
	}

#if defined(CONFIG_CRYPTO_DEV_DEU)
#if defined(CONFIG_CRYPTO_DEV_MD5)
        mode = 1; // test md5 only
        err = do_test(mode);
        if (err)
            goto md5_err;

md5_err:	
	if (err) {
                printk(KERN_ERR "md5: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined(CONFIG_CRYPTO_DEV_SHA1)
        mode = 2; // test sha1 only
        err = do_test(mode);
        if (err)
            goto sha1_err;

sha1_err:
	if (err) {
                printk(KERN_ERR "sha1: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined (CONFIG_CRYPTO_DEV_DES) || defined (CONFIG_CRYPTO_ASYNC_DES)
        mode = 3; // test des only
        err = do_test(mode);
	if (err) 
            goto des_err;
       
        mode = 4; // test des3 only
        err = do_test(mode);
        if (err)
            goto des_err;

des_err:
	if (err) {
                printk(KERN_ERR "des3: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined (CONFIG_CRYPTO_ASYNC_AES) ||  defined (CONFIG_CRYPTO_DEV_AES)                 
	mode = 10; // test aes only
        err = do_test(mode);
        if (err)
             goto aes_err;

aes_err:
	if (err) {
                printk(KERN_ERR "aes: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined(CONFIG_CRYPTO_DEV_ARC4)
	mode = 16;
	err = do_test(mode);

	if (err) {
                printk(KERN_ERR "arc4: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined (CONFIG_CRYPTO_DEV_MD5_HMAC)	
	mode = 100;
	err = do_test(mode);

	if (err) {
                printk(KERN_ERR "tcrypt: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif
#if defined (CONFIG_CRYPTO_DEV_SHA1_HMAC)	
        mode = 101;
	err = do_test(mode);

	if (err) {
                printk(KERN_ERR "tcrypt: one or more tests failed!\n");
                goto err_free_tv;
        }
#endif

/* Start Speed tests test modes */
#if defined(CONFIG_CRYPTO_DEV_SPEED_TEST)
#if   defined(CONFIG_CRYPTO_DEV_AES) 
     mode = 200;
     err = do_test(mode);
     if (err) 
         goto speed_err;
#endif
#if   defined (CONFIG_CRYPTO_DEV_DES)
      mode = 201;
      err = do_test(mode);  
      if (err)
         goto speed_err;

      mode = 204;
      err = do_test(mode);  
      if (err)
         goto speed_err;
#endif
#if defined (CONFIG_CRYPTO_DEV_MD5)
      mode = 302;
      err = do_test(mode);
      if (err)
          goto speed_err;  
#endif 
#if defined (CONFIG_CRYPTO_DEV_SHA1)
      mode = 303;
      err = do_test(mode); 
      if (err)
          goto speed_err;
#endif
      printk("Speed tests finished successfully\n"); 
      goto fips_check;

speed_err:
	printk(KERN_ERR "tcrypt: one or more tests failed!\n");
        goto err_free_tv;
#endif /* CONFIG_CRYPTO_DEV_SPEED_TEST */

#else
	if (alg)
		err = do_alg_test(alg, type, mask);
	else
		err = do_test(mode);

	if (err) {
		printk(KERN_ERR "tcrypt: one or more tests failed!\n");
		goto err_free_tv;
	}
#endif /* CONFIG_CRYPTO_DEV_DEU */

fips_check:
	/* We intentionaly return -EAGAIN to prevent keeping the module,
	 * unless we're running in fips mode. It does all its work from
	 * init() and doesn't offer any runtime functionality, but in
	 * the fips case, checking for a successful load is helpful.
	 * => we don't need it in the memory, do we?
	 *                                        -- mludvig
	 */
	if (!fips_enabled)
		err = -EAGAIN;

err_free_tv:
	for (i = 0; i < TVMEMSIZE && tvmem[i]; i++ ){
		printk("Freeing page: %d\n", i);
		free_page((unsigned long)tvmem[i]);
	}

	printk("Finished DEU testing . . . . . .\n");
	return err;
}

/*
 * If an init function is provided, an exit function must also be provided
 * to allow module unload.
 */
static void __exit tcrypt_mod_fini(void) {}


module_init(tcrypt_mod_init);
module_exit(tcrypt_mod_fini);

module_param(alg, charp, 0);
module_param(type, uint, 0);
module_param(mask, uint, 0);
module_param(mode, int, 0);
module_param(sec, uint, 0);
MODULE_PARM_DESC(sec, "Length in seconds of speed tests "
		      "(defaults to zero which uses CPU cycles instead)");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quick & dirty crypto testing module");
MODULE_AUTHOR("James Morris <jmorris@intercode.com.au>");

