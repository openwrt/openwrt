#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

CRYPTO_MENU:=Cryptographic API modules

CRYPTO_MODULES = \
	ALGAPI2=crypto_algapi \
	BLKCIPHER2=crypto_blkcipher

CRYPTO_TARGET = $(BOARD)/$(SUBTARGET)

crypto_confvar=CONFIG_CRYPTO_$(word 1,$(subst =,$(space),$(1)))
crypto_file=$(LINUX_DIR)/crypto/$(word 2,$(subst =,$(space),$(1))).ko
crypto_name=$(if $(findstring y,$($(call crypto_confvar,$(1)))),,$(word 2,$(subst =,$(space),$(1))))

define AddDepends/crypto
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS+= $(1)
endef


define KernelPackage/crypto-acompress
  TITLE:=Asynchronous Compression operations
  HIDDEN:=1
  KCONFIG:=CONFIG_CRYPTO_ACOMP2
  FILES:=$(LINUX_DIR)/crypto/crypto_acompress.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_acompress)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-acompress))


define KernelPackage/crypto-aead
  TITLE:=CryptoAPI AEAD support
  KCONFIG:= \
	CONFIG_CRYPTO_AEAD \
	CONFIG_CRYPTO_AEAD2
  FILES:=$(LINUX_DIR)/crypto/aead.ko
  AUTOLOAD:=$(call AutoLoad,09,aead,1)
  $(call AddDepends/crypto, +kmod-crypto-null)
endef

$(eval $(call KernelPackage,crypto-aead))


define KernelPackage/crypto-arc4
  TITLE:=ARC4 cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-user
  KCONFIG:= \
	  CONFIG_CRYPTO_ARC4 \
	  CONFIG_CRYPTO_USER_API_ENABLE_OBSOLETE=y
  FILES:= \
	  $(LINUX_DIR)/crypto/arc4.ko \
	  $(LINUX_DIR)/lib/crypto/libarc4.ko
  AUTOLOAD:=$(call AutoLoad,09,arc4)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-arc4))


define KernelPackage/crypto-authenc
  TITLE:=Combined mode wrapper for IPsec
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-null
  KCONFIG:=CONFIG_CRYPTO_AUTHENC
  FILES:= \
	$(LINUX_DIR)/crypto/authenc.ko \
	$(LINUX_DIR)/crypto/authencesn.ko
  AUTOLOAD:=$(call AutoLoad,09,authenc authencesn)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-authenc))


define KernelPackage/crypto-blake2b
  TITLE:=Support for BLAKE2b cryptographic hash function (RFC 7693)
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_BLAKE2B
  FILES:=$(LINUX_DIR)/crypto/blake2b_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,blake2b_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-blake2b))


define KernelPackage/crypto-cbc
  TITLE:=Cipher Block Chaining CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CBC
  FILES:=$(LINUX_DIR)/crypto/cbc.ko
  AUTOLOAD:=$(call AutoLoad,09,cbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cbc))


define KernelPackage/crypto-ccm
 TITLE:=Support for Counter with CBC MAC (CCM)
 DEPENDS:=+kmod-crypto-ctr +kmod-crypto-aead
 KCONFIG:=CONFIG_CRYPTO_CCM
 FILES:=$(LINUX_DIR)/crypto/ccm.ko
 AUTOLOAD:=$(call AutoLoad,09,ccm)
 $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ccm))


define KernelPackage/crypto-chacha20poly1305
  TITLE:=ChaCha20-Poly1305 AEAD support, RFC7539 (used by strongSwan IPsec VPN)
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-manager \
	+kmod-crypto-lib-chacha20 \
	+kmod-crypto-lib-poly1305
  KCONFIG:=CONFIG_CRYPTO_CHACHA20POLY1305
  FILES:=$(LINUX_DIR)/crypto/chacha20poly1305.ko \
	$(LINUX_DIR)/crypto/chacha.ko
  AUTOLOAD:=$(call AutoLoad,09,chacha chacha20poly1305)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-chacha20poly1305))


define KernelPackage/crypto-cmac
  TITLE:=Support for Cipher-based Message Authentication Code (CMAC)
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CMAC
  FILES:=$(LINUX_DIR)/crypto/cmac.ko
  AUTOLOAD:=$(call AutoLoad,09,cmac)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cmac))


define KernelPackage/crypto-crc32
  TITLE:=CRC32 CRC module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CRC32
  HIDDEN:=1
  FILES:=$(LINUX_DIR)/crypto/crc32-cryptoapi.ko
  AUTOLOAD:=$(call AutoLoad,04,crc32-cryptoapi,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-crc32))


define KernelPackage/crypto-crc32c
  TITLE:=CRC32c CRC module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CRC32C
  FILES:=$(LINUX_DIR)/crypto/crc32c-cryptoapi.ko
  AUTOLOAD:=$(call AutoLoad,04,crc32c-cryptoapi,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-crc32c))


define KernelPackage/crypto-ctr
  TITLE:=Counter Mode CryptoAPI module
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-seqiv
  KCONFIG:=CONFIG_CRYPTO_CTR
  FILES:=$(LINUX_DIR)/crypto/ctr.ko
  AUTOLOAD:=$(call AutoLoad,09,ctr)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ctr))


define KernelPackage/crypto-cts
  TITLE:=Cipher Text Stealing CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CTS
  FILES:=$(LINUX_DIR)/crypto/cts.ko
  AUTOLOAD:=$(call AutoLoad,09,cts)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cts))


define KernelPackage/crypto-deflate
  TITLE:=Deflate compression CryptoAPI module
  DEPENDS:=+kmod-lib-zlib-inflate +kmod-lib-zlib-deflate +kmod-crypto-acompress
  KCONFIG:=CONFIG_CRYPTO_DEFLATE
  FILES:=$(LINUX_DIR)/crypto/deflate.ko
  AUTOLOAD:=$(call AutoLoad,09,deflate)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-deflate))


define KernelPackage/crypto-des
  TITLE:=DES/3DES cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_DES
  FILES:= \
	$(LINUX_DIR)/crypto/des_generic.ko \
	$(LINUX_DIR)/lib/crypto/libdes.ko
  AUTOLOAD:=$(call AutoLoad,09,des_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-des))


define KernelPackage/crypto-ecb
  TITLE:=Electronic CodeBook CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_ECB
  FILES:=$(LINUX_DIR)/crypto/ecb.ko
  AUTOLOAD:=$(call AutoLoad,09,ecb)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ecb))


define KernelPackage/crypto-ecdh
  TITLE:=ECDH algorithm
  DEPENDS:=+kmod-crypto-kpp +kmod-crypto-rng
  KCONFIG:= CONFIG_CRYPTO_ECDH
  FILES:= \
	$(LINUX_DIR)/crypto/ecdh_generic.ko \
	$(LINUX_DIR)/crypto/ecc.ko
  AUTOLOAD:=$(call AutoLoad,10,ecdh_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ecdh))


define KernelPackage/crypto-echainiv
  TITLE:=Encrypted Chain IV Generator
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-geniv
  KCONFIG:=CONFIG_CRYPTO_ECHAINIV
  FILES:=$(LINUX_DIR)/crypto/echainiv.ko
  AUTOLOAD:=$(call AutoLoad,09,echainiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-echainiv))

define KernelPackage/crypto-engine
  TITLE:=Crypto engine
  HIDDEN:=1
  KCONFIG:=CONFIG_CRYPTO_ENGINE
  FILES:=$(LINUX_DIR)/crypto/crypto_engine.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_engine)
  $(call AddDepends/crypto, +kmod-crypto-rsa +kmod-crypto-kpp)
endef

$(eval $(call KernelPackage,crypto-engine))

define KernelPackage/crypto-essiv
  TITLE:=ESSIV support for block encryption
  DEPENDS:=+kmod-crypto-authenc
  KCONFIG:=CONFIG_CRYPTO_ESSIV
  FILES:= $(LINUX_DIR)/crypto/essiv.ko
  AUTOLOAD:=$(call AutoLoad,10,essiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-essiv))


define KernelPackage/crypto-fcrypt
  TITLE:=FCRYPT cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_FCRYPT
  FILES:=$(LINUX_DIR)/crypto/fcrypt.ko
  AUTOLOAD:=$(call AutoLoad,09,fcrypt)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-fcrypt))


define KernelPackage/crypto-gcm
  TITLE:=GCM/GMAC CryptoAPI module
  DEPENDS:=+kmod-crypto-ctr +kmod-crypto-ghash +kmod-crypto-null
  KCONFIG:=CONFIG_CRYPTO_GCM
  FILES:=$(LINUX_DIR)/crypto/gcm.ko
  AUTOLOAD:=$(call AutoLoad,09,gcm)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-gcm))


define KernelPackage/crypto-xcbc
  TITLE:=XCBC CryptoAPI module
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_XCBC
  FILES:=$(LINUX_DIR)/crypto/xcbc.ko
  AUTOLOAD:=$(call AutoLoad,09,xcbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xcbc))


define KernelPackage/crypto-gf128
  TITLE:=GF(2^128) multiplication functions CryptoAPI module
  KCONFIG:= \
	CONFIG_CRYPTO_LIB_GF128MUL
  FILES:=$(LINUX_DIR)/lib/crypto/gf128mul.ko
  AUTOLOAD:=$(call AutoLoad,09,gf128mul)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-gf128))


define KernelPackage/crypto-ghash
  TITLE:=GHASH digest CryptoAPI module
  DEPENDS:=+kmod-crypto-gf128 +kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_GHASH \
	CONFIG_CRYPTO_GHASH_ARM_CE
  FILES:=$(LINUX_DIR)/crypto/ghash-generic.ko
  AUTOLOAD:=$(call AutoLoad,09,ghash-generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-ghash/arm-ce
  FILES+= $(LINUX_DIR)/arch/arm/crypto/ghash-arm-ce.ko
  AUTOLOAD+=ghash-arm-ce
endef

KernelPackage/crypto-ghash/imx/cortexa7=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/imx/cortexa9=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/ipq40xx=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/mvebu/cortexa9=$(KernelPackage/crypto-ghash/arm-ce)

$(eval $(call KernelPackage,crypto-ghash))


define KernelPackage/crypto-hash
  TITLE:=CryptoAPI hash support
  KCONFIG:=CONFIG_CRYPTO_HASH
  FILES:=$(LINUX_DIR)/crypto/crypto_hash.ko
  AUTOLOAD:=$(call AutoLoad,02,crypto_hash,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hash))


define KernelPackage/crypto-hmac
  TITLE:=HMAC digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_HMAC
  FILES:=$(LINUX_DIR)/crypto/hmac.ko
  AUTOLOAD:=$(call AutoLoad,09,hmac)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hmac))


define KernelPackage/crypto-hw-atmel
  TITLE:=Microchip / Atmel ECC/SHA/RNG hw accelerator
  DEPENDS:=+kmod-i2c-core +kmod-crypto-ecdh +kmod-crypto-sha1 \
	+kmod-crypto-sha256 +kmod-lib-crc16 +kmod-random-core
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_ATMEL_I2C \
	CONFIG_CRYPTO_DEV_ATMEL_ECC \
	CONFIG_CRYPTO_DEV_ATMEL_SHA204A
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/atmel-i2c.ko \
	$(LINUX_DIR)/drivers/crypto/atmel-ecc.ko \
	$(LINUX_DIR)/drivers/crypto/atmel-sha204a.ko
  AUTOLOAD:=$(call AutoLoad,09,atmel-i2c atmel-ecc atmel-sha204a)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-atmel))


define KernelPackage/crypto-hw-ccp
  TITLE:=AMD Cryptographic Coprocessor
  DEPENDS:= \
	@TARGET_x86 \
	+kmod-crypto-authenc \
	+kmod-crypto-hash \
	+kmod-crypto-manager \
	+kmod-crypto-rsa \
	+kmod-crypto-sha1 \
	+kmod-crypto-sha256 \
	+kmod-random-core
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_CCP=y \
	CONFIG_CRYPTO_DEV_CCP_CRYPTO \
	CONFIG_CRYPTO_DEV_CCP_DD \
	CONFIG_CRYPTO_DEV_SP_CCP=y
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/ccp/ccp.ko \
	$(LINUX_DIR)/drivers/crypto/ccp/ccp-crypto.ko
  AUTOLOAD:=$(call AutoLoad,09,ccp ccp-crypto)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-ccp))


define KernelPackage/crypto-hw-geode
  TITLE:=AMD Geode hardware crypto module
  DEPENDS:=@TARGET_x86_geode +kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_GEODE
  FILES:=$(LINUX_DIR)/drivers/crypto/geode-aes.ko
  AUTOLOAD:=$(call AutoLoad,09,geode-aes)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-geode))


define KernelPackage/crypto-hw-hifn-795x
  TITLE:=HIFN 795x crypto accelerator
  DEPENDS:=@PCI_SUPPORT +kmod-random-core +kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_HIFN_795X \
	CONFIG_CRYPTO_DEV_HIFN_795X_RNG=y
  FILES:=$(LINUX_DIR)/drivers/crypto/hifn_795x.ko
  AUTOLOAD:=$(call AutoLoad,09,hifn_795x)
  $(call AddDepends/crypto,+kmod-crypto-des)
endef

$(eval $(call KernelPackage,crypto-hw-hifn-795x))

define KernelPackage/crypto-hw-ixp4xx
  TITLE:=Intel IXP4xx crypto accelerator
  DEPENDS:=@TARGET_ixp4xx +kmod-random-core +kmod-crypto-manager +kmod-crypto-authenc +kmod-crypto-des
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_IXP4XX
  FILES:=$(LINUX_DIR)/drivers/crypto/intel/ixp4xx/ixp4xx_crypto.ko
  AUTOLOAD:=$(call AutoProbe,ixp4xx_crypto)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-ixp4xx))


define KernelPackage/crypto-hw-padlock
  TITLE:=VIA PadLock ACE with AES/SHA hw crypto module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_PADLOCK \
	CONFIG_CRYPTO_DEV_PADLOCK_AES \
	CONFIG_CRYPTO_DEV_PADLOCK_SHA
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/padlock-aes.ko \
	$(LINUX_DIR)/drivers/crypto/padlock-sha.ko
  AUTOLOAD:=$(call AutoLoad,09,padlock-aes padlock-sha)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-padlock))


define KernelPackage/crypto-hw-safexcel
  TITLE:= MVEBU SafeXcel Crypto Engine module
  DEPENDS:=@(TARGET_mvebu_cortexa53||TARGET_mvebu_cortexa72||TARGET_mediatek_filogic||TARGET_mediatek_mt7623) \
	+eip197-mini-firmware +kmod-crypto-authenc +kmod-crypto-des +kmod-crypto-md5 +kmod-crypto-hmac \
	+kmod-crypto-sha1 +kmod-crypto-sha256 +kmod-crypto-sha512
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_SAFEXCEL
  FILES:=$(LINUX_DIR)/drivers/crypto/inside-secure/crypto_safexcel.ko
  AUTOLOAD:=$(call AutoLoad,90,crypto_safexcel)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-hw-safexcel/description
MVEBU's EIP97 and EIP197 Cryptographic Engine driver designed by
Inside Secure. This is found on Marvell Armada 37xx/7k/8k SoCs.

Particular version of these IP (EIP197B and EIP197D) require firmware.
The mini firmware package provides limited functionality, for most operations
a full-featured firmware is required. Unfortunately the "full" firmware is not
freely available and needs signed Non-Disclosure Agreement (NDA) with Marvell.
For those who have signed NDA the firmware can be obtained at
https://extranet.marvell.com.
endef

$(eval $(call KernelPackage,crypto-hw-safexcel))


define KernelPackage/crypto-hw-talitos
  TITLE:=Freescale integrated security engine (SEC) driver
  DEPENDS:=@(TARGET_mpc85xx||TARGET_layerscape) +kmod-crypto-manager \
	+kmod-crypto-hash +kmod-random-core +kmod-crypto-authenc +kmod-crypto-des
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_TALITOS \
	CONFIG_CRYPTO_DEV_TALITOS1=y \
	CONFIG_CRYPTO_DEV_TALITOS2=y
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/talitos.ko
  AUTOLOAD:=$(call AutoLoad,09,talitos)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-talitos))

define KernelPackage/crypto-hw-eip93
  TITLE:=MTK EIP93 crypto module
  DEPENDS:=@(TARGET_ramips_mt7621||TARGET_airoha) \
	+kmod-crypto-authenc \
	+kmod-crypto-des \
	+kmod-crypto-md5 \
	+kmod-crypto-sha1 \
	+kmod-crypto-sha256
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_EIP93 \
	CONFIG_CRYPTO_DEV_EIP93_GENERIC_SW_MAX_LEN=256 \
	CONFIG_CRYPTO_DEV_EIP93_AES_128_SW_MAX_LEN=512
  FILES:=$(LINUX_DIR)/drivers/crypto/inside-secure/eip93/crypto-hw-eip93.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto-hw-eip93)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-hw-eip93/description
Kernel module to enable EIP-93 Crypto engine as found
in Mediatek MT7621 and Airoha SoCs.
It enables DES/3DES/AES ECB/CBC/CTR and
IPSEC offload with authenc(hmac(sha1/sha256), aes/cbc/rfc3686)
endef

$(eval $(call KernelPackage,crypto-hw-eip93))

define KernelPackage/crypto-kpp
  TITLE:=Key-agreement Protocol Primitives
  KCONFIG:=CONFIG_CRYPTO_KPP
  FILES:=$(LINUX_DIR)/crypto/kpp.ko
  AUTOLOAD:=$(call AutoLoad,09,kpp)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-kpp))

define KernelPackage/crypto-lib-chacha20
  TITLE:=ChaCha library interface
  KCONFIG:=CONFIG_CRYPTO_LIB_CHACHA
  HIDDEN:=1
  FILES:=$(LINUX_DIR)/lib/crypto/libchacha.ko
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-lib-chacha20))


define KernelPackage/crypto-lib-chacha20poly1305
  TITLE:=ChaCha20-Poly1305 AEAD support (8-byte nonce library version)
  KCONFIG:=CONFIG_CRYPTO_LIB_CHACHA20POLY1305
  HIDDEN:=1
  FILES:=$(LINUX_DIR)/lib/crypto/libchacha20poly1305.ko
  $(call AddDepends/crypto, +kmod-crypto-lib-chacha20 +kmod-crypto-lib-poly1305)
endef

$(eval $(call KernelPackage,crypto-lib-chacha20poly1305))


define KernelPackage/crypto-lib-curve25519
  TITLE:=Curve25519 scalar multiplication library
  KCONFIG:=CONFIG_CRYPTO_LIB_CURVE25519
  HIDDEN:=1
  FILES:= \
	$(LINUX_DIR)/lib/crypto/libcurve25519.ko
  $(call AddDepends/crypto,+kmod-crypto-kpp)
endef

$(eval $(call KernelPackage,crypto-lib-curve25519))


define KernelPackage/crypto-lib-poly1305
  TITLE:=Poly1305 library interface
  KCONFIG:=CONFIG_CRYPTO_LIB_POLY1305
  HIDDEN:=1
  FILES:=$(LINUX_DIR)/lib/crypto/libpoly1305.ko
  $(call AddDepends/crypto,+kmod-crypto-hash)
endef

$(eval $(call KernelPackage,crypto-lib-poly1305))


define KernelPackage/crypto-manager
  TITLE:=CryptoAPI algorithm manager
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_MANAGER \
	CONFIG_CRYPTO_MANAGER2
  FILES:=$(LINUX_DIR)/crypto/cryptomgr.ko
  AUTOLOAD:=$(call AutoLoad,09,cryptomgr,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-manager))


define KernelPackage/crypto-md4
  TITLE:=MD4 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_MD4
  FILES:=$(LINUX_DIR)/crypto/md4.ko
  AUTOLOAD:=$(call AutoLoad,09,md4)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-md4))


define KernelPackage/crypto-md5
  TITLE:=MD5 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_MD5
  FILES:=$(LINUX_DIR)/crypto/md5.ko \
	$(LINUX_DIR)/lib/crypto/libmd5.ko
  AUTOLOAD:=$(call AutoLoad,09,md5 libmd5)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-md5))


define KernelPackage/crypto-michael-mic
  TITLE:=Michael MIC keyed digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_MICHAEL_MIC
  FILES:=$(LINUX_DIR)/crypto/michael_mic.ko
  AUTOLOAD:=$(call AutoLoad,09,michael_mic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-michael-mic))


define KernelPackage/crypto-null
  TITLE:=Null CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_NULL
  FILES:=$(LINUX_DIR)/crypto/crypto_null.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_null)
  $(call AddDepends/crypto, +kmod-crypto-hash)
endef

$(eval $(call KernelPackage,crypto-null))


define KernelPackage/crypto-pcbc
  TITLE:=Propagating Cipher Block Chaining CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_PCBC
  FILES:=$(LINUX_DIR)/crypto/pcbc.ko
  AUTOLOAD:=$(call AutoLoad,09,pcbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-pcbc))


define KernelPackage/crypto-rsa
  TITLE:=RSA algorithm
  DEPENDS:=+kmod-crypto-manager +kmod-asn1-decoder
  KCONFIG:= CONFIG_CRYPTO_RSA
  HIDDEN:=1
  FILES:= \
	$(LINUX_DIR)/lib/crypto/mpi/mpi.ko \
	$(LINUX_DIR)/crypto/akcipher.ko \
	$(LINUX_DIR)/crypto/rsa_generic.ko
  AUTOLOAD:=$(call AutoLoad,10,rsa_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rsa))


define KernelPackage/crypto-rmd160
  TITLE:=RIPEMD160 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_RMD160
  FILES:=$(LINUX_DIR)/crypto/rmd160.ko
  AUTOLOAD:=$(call AutoLoad,09,rmd160)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rmd160))


define KernelPackage/crypto-rng
  TITLE:=CryptoAPI random number generation
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-hmac +kmod-crypto-sha512 +kmod-crypto-sha3
  KCONFIG:= \
	CONFIG_CRYPTO_DRBG \
	CONFIG_CRYPTO_DRBG_HMAC=y \
	CONFIG_CRYPTO_DRBG_HASH=n \
	CONFIG_CRYPTO_DRBG_MENU \
	CONFIG_CRYPTO_JITTERENTROPY \
	CONFIG_CRYPTO_RNG2
  FILES:= \
	$(LINUX_DIR)/crypto/drbg.ko \
	$(LINUX_DIR)/crypto/jitterentropy_rng.ko \
	$(LINUX_DIR)/crypto/rng.ko
  AUTOLOAD:=$(call AutoLoad,09,drbg jitterentropy_rng rng)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rng))


define KernelPackage/crypto-geniv
  TITLE:=CryptoAPI Shared IV generator
  HIDDEN:=1
  DEPENDS:=+kmod-crypto-rng +kmod-crypto-aead
  KCONFIG:=CONFIG_CRYPTO_GENIV
  FILES:=$(LINUX_DIR)/crypto/geniv.ko
  AUTOLOAD:=$(call AutoLoad,09,geniv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-geniv))


define KernelPackage/crypto-seqiv
  TITLE:=CryptoAPI Sequence Number IV Generator
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-rng +kmod-crypto-geniv
  KCONFIG:=CONFIG_CRYPTO_SEQIV
  FILES:=$(LINUX_DIR)/crypto/seqiv.ko
  AUTOLOAD:=$(call AutoLoad,09,seqiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-seqiv))


define KernelPackage/crypto-sha1
  TITLE:=SHA1 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA1
  FILES:= \
	$(LINUX_DIR)/crypto/sha1.ko \
	$(LINUX_DIR)/lib/crypto/libsha1.ko
  AUTOLOAD:=$(call AutoLoad,09,sha1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha1))


define KernelPackage/crypto-sha3
  TITLE:=SHA3 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= CONFIG_CRYPTO_SHA3
  FILES:=$(LINUX_DIR)/crypto/sha3_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha3_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha3))


define KernelPackage/crypto-sha256
  TITLE:=SHA224 SHA256 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA256
  FILES:= \
	$(LINUX_DIR)/crypto/sha256.ko \
	$(LINUX_DIR)/lib/crypto/libsha256.ko
  AUTOLOAD:=$(call AutoLoad,09,sha256)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha256))


define KernelPackage/crypto-sha512
  TITLE:=SHA512 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA512
  FILES:= \
	$(LINUX_DIR)/crypto/sha512.ko \
	$(LINUX_DIR)/lib/crypto/libsha512.ko
  AUTOLOAD:=$(call AutoLoad,09,sha512)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha512))


define KernelPackage/crypto-test
  TITLE:=Test CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_BENCHMARK
  FILES:=$(LINUX_DIR)/crypto/tcrypt.ko
  $(call AddDepends/crypto,+kmod-crypto-manager)
endef

$(eval $(call KernelPackage,crypto-test))


define KernelPackage/crypto-user
  TITLE:=CryptoAPI userspace interface
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager +kmod-crypto-rng
  KCONFIG:= \
	CONFIG_CRYPTO_USER \
	CONFIG_CRYPTO_USER_API \
	CONFIG_CRYPTO_USER_API_AEAD \
	CONFIG_CRYPTO_USER_API_HASH \
	CONFIG_CRYPTO_USER_API_RNG \
	CONFIG_CRYPTO_USER_API_SKCIPHER
  FILES:= \
	$(LINUX_DIR)/crypto/af_alg.ko \
	$(LINUX_DIR)/crypto/algif_aead.ko \
	$(LINUX_DIR)/crypto/algif_hash.ko \
	$(LINUX_DIR)/crypto/algif_rng.ko \
	$(LINUX_DIR)/crypto/algif_skcipher.ko \
	$(LINUX_DIR)/crypto/crypto_user.ko
  AUTOLOAD:=$(call AutoLoad,09,af_alg algif_aead algif_hash algif_rng algif_skcipher crypto_user)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-user))


define KernelPackage/crypto-xts
  TITLE:=XTS cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-gf128 +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_XTS
  FILES:=$(LINUX_DIR)/crypto/xts.ko
  AUTOLOAD:=$(call AutoLoad,09,xts)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xts))


define KernelPackage/crypto-xxhash
  TITLE:=xxHash non-cryptographic hash algorithm
  DEPENDS:=+kmod-crypto-hash +kmod-lib-xxhash
  KCONFIG:=CONFIG_CRYPTO_XXHASH
  FILES:=$(LINUX_DIR)/crypto/xxhash_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,xxhash_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xxhash))

