#ifndef _CRYPTO_SIFLOWER_HASH_H
#define _CRYPTO_SIFLOWER_HASH_H

#include <crypto/hash.h>

extern struct ahash_alg sf_ce_sha1 __read_mostly;
extern struct ahash_alg sf_ce_sha256 __read_mostly;
extern struct ahash_alg sf_ce_sha224 __read_mostly;
extern struct ahash_alg sf_ce_sha512 __read_mostly;
extern struct ahash_alg sf_ce_sha384 __read_mostly;
extern struct ahash_alg sf_ce_md5 __read_mostly;

#endif
