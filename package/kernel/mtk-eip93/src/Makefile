# SPDX-License-Identifier: GPL-2.0-only
obj-m := crypto-hw-eip93.o

crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93) += eip93-main.o \
						eip93-common.o

crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93_SKCIPHER) += eip93-cipher.o
crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93_AEAD) += eip93-aead.o
crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93_HASH) += eip93-hash.o
crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93_PRNG) += eip93-prng.o
crypto-hw-eip93-$(CONFIG_CRYPTO_DEV_EIP93_IPSEC) += eip93-ipsec.o
