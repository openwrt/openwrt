#ifndef _CRYPTO_SIFLOWER_AES_H
#define _CRYPTO_SIFLOWER_AES_H

#include <crypto/aead.h>
#include <crypto/skcipher.h>

extern struct skcipher_alg sf_ce_aes_ctr __read_mostly;
extern struct aead_alg sf_ce_aes_gcm __read_mostly;
extern struct aead_alg sf_ce_aes_ccm __read_mostly;

#endif
