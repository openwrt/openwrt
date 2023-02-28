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

define AddDepends/crypto
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS+= $(1)
endef


define KernelPackage/crypto-acompress
  TITLE:=Asynchronous Compression operations
  HIDDEN:=1
  KCONFIG:=CONFIG_CRYPTO_ACOMP2
  FILES:=crypto/crypto_acompress.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_acompress)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-acompress))


define KernelPackage/crypto-aead
  TITLE:=CryptoAPI AEAD support
  KCONFIG:= \
	CONFIG_CRYPTO_AEAD \
	CONFIG_CRYPTO_AEAD2 \
	CONFIG_CRYPTO_GENIV@lt6.6
  FILES:= \
	  crypto/aead.ko \
	  crypto/geniv.ko@lt6.6
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
	  crypto/arc4.ko \
	  lib/crypto/libarc4.ko
  AUTOLOAD:=$(call AutoLoad,09,arc4)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-arc4))


define KernelPackage/crypto-authenc
  TITLE:=Combined mode wrapper for IPsec
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-null
  KCONFIG:=CONFIG_CRYPTO_AUTHENC
  FILES:= \
	crypto/authenc.ko \
	crypto/authencesn.ko
  AUTOLOAD:=$(call AutoLoad,09,authenc authencesn)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-authenc))


define KernelPackage/crypto-blake2b
  TITLE:=Support for BLAKE2b cryptographic hash function (RFC 7693)
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_BLAKE2B
  FILES:=crypto/blake2b_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,blake2b_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-blake2b))


define KernelPackage/crypto-cbc
  TITLE:=Cipher Block Chaining CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CBC
  FILES:=crypto/cbc.ko
  AUTOLOAD:=$(call AutoLoad,09,cbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cbc))


define KernelPackage/crypto-ccm
 TITLE:=Support for Counter with CBC MAC (CCM)
 DEPENDS:=+kmod-crypto-ctr +kmod-crypto-aead
 KCONFIG:=CONFIG_CRYPTO_CCM
 FILES:=crypto/ccm.ko
 AUTOLOAD:=$(call AutoLoad,09,ccm)
 $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ccm))


define KernelPackage/crypto-chacha20poly1305
  TITLE:=ChaCha20-Poly1305 AEAD support, RFC7539 (used by strongSwan IPsec VPN)
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CHACHA20POLY1305
  FILES:=crypto/chacha20poly1305.ko
  AUTOLOAD:=$(call AutoLoad,09,chacha20poly1305)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-chacha20poly1305))


define KernelPackage/crypto-cmac
  TITLE:=Support for Cipher-based Message Authentication Code (CMAC)
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CMAC
  FILES:=crypto/cmac.ko
  AUTOLOAD:=$(call AutoLoad,09,cmac)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cmac))


define KernelPackage/crypto-crc32
  TITLE:=CRC32 CRC module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CRC32
  HIDDEN:=1
  FILES:=crypto/crc32_generic.ko
  AUTOLOAD:=$(call AutoLoad,04,crc32_generic,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-crc32))


define KernelPackage/crypto-crc32c
  TITLE:=CRC32c CRC module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CRC32C
  FILES:=crypto/crc32c_generic.ko
  AUTOLOAD:=$(call AutoLoad,04,crc32c_generic,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-crc32c))


define KernelPackage/crypto-ctr
  TITLE:=Counter Mode CryptoAPI module
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-seqiv
  KCONFIG:=CONFIG_CRYPTO_CTR
  FILES:=crypto/ctr.ko
  AUTOLOAD:=$(call AutoLoad,09,ctr)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ctr))


define KernelPackage/crypto-cts
  TITLE:=Cipher Text Stealing CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CTS
  FILES:=crypto/cts.ko
  AUTOLOAD:=$(call AutoLoad,09,cts)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cts))


define KernelPackage/crypto-deflate
  TITLE:=Deflate compression CryptoAPI module
  DEPENDS:=+kmod-lib-zlib-inflate +kmod-lib-zlib-deflate +kmod-crypto-acompress
  KCONFIG:=CONFIG_CRYPTO_DEFLATE
  FILES:=crypto/deflate.ko
  AUTOLOAD:=$(call AutoLoad,09,deflate)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-deflate))


define KernelPackage/crypto-des
  TITLE:=DES/3DES cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_DES
  FILES:= \
	crypto/des_generic.ko \
	lib/crypto/libdes.ko
  AUTOLOAD:=$(call AutoLoad,09,des_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-des))


define KernelPackage/crypto-ecb
  TITLE:=Electronic CodeBook CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_ECB
  FILES:=crypto/ecb.ko
  AUTOLOAD:=$(call AutoLoad,09,ecb)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ecb))


define KernelPackage/crypto-ecdh
  TITLE:=ECDH algorithm
  DEPENDS:=+kmod-crypto-kpp +kmod-crypto-rng
  KCONFIG:= CONFIG_CRYPTO_ECDH
  FILES:= \
	crypto/ecdh_generic.ko \
	crypto/ecc.ko
  AUTOLOAD:=$(call AutoLoad,10,ecdh_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ecdh))


define KernelPackage/crypto-echainiv
  TITLE:=Encrypted Chain IV Generator
  DEPENDS:=+kmod-crypto-aead +LINUX_6_6:kmod-crypto-geniv
  KCONFIG:=CONFIG_CRYPTO_ECHAINIV
  FILES:=crypto/echainiv.ko
  AUTOLOAD:=$(call AutoLoad,09,echainiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-echainiv))


define KernelPackage/crypto-essiv
  TITLE:=ESSIV support for block encryption
  DEPENDS:=+kmod-crypto-authenc
  KCONFIG:=CONFIG_CRYPTO_ESSIV
  FILES:= crypto/essiv.ko
  AUTOLOAD:=$(call AutoLoad,10,essiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-essiv))


define KernelPackage/crypto-fcrypt
  TITLE:=FCRYPT cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_FCRYPT
  FILES:=crypto/fcrypt.ko
  AUTOLOAD:=$(call AutoLoad,09,fcrypt)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-fcrypt))


define KernelPackage/crypto-gcm
  TITLE:=GCM/GMAC CryptoAPI module
  DEPENDS:=+kmod-crypto-ctr +kmod-crypto-ghash +kmod-crypto-null
  KCONFIG:=CONFIG_CRYPTO_GCM
  FILES:=crypto/gcm.ko
  AUTOLOAD:=$(call AutoLoad,09,gcm)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-gcm))


define KernelPackage/crypto-xcbc
  TITLE:=XCBC CryptoAPI module
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_XCBC
  FILES:=crypto/xcbc.ko
  AUTOLOAD:=$(call AutoLoad,09,xcbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xcbc))


define KernelPackage/crypto-gf128
  TITLE:=GF(2^128) multiplication functions CryptoAPI module
  KCONFIG:= \
	CONFIG_CRYPTO_GF128MUL \
	CONFIG_CRYPTO_LIB_GF128MUL
  FILES:= \
	crypto/gf128mul.ko@lt6.2 \
	lib/crypto/gf128mul.ko@ge6.2
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
  FILES:=crypto/ghash-generic.ko
  AUTOLOAD:=$(call AutoLoad,09,ghash-generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-ghash/arm-ce
  FILES+=arch/arm/crypto/ghash-arm-ce.ko
  AUTOLOAD+=$(call AutoLoad,09,ghash-arm-ce)
endef

KernelPackage/crypto-ghash/imx/cortexa7=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/imx/cortexa9=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/ipq40xx=$(KernelPackage/crypto-ghash/arm-ce)
KernelPackage/crypto-ghash/mvebu/cortexa9=$(KernelPackage/crypto-ghash/arm-ce)

$(eval $(call KernelPackage,crypto-ghash))


define KernelPackage/crypto-hash
  TITLE:=CryptoAPI hash support
  KCONFIG:= \
	CONFIG_CRYPTO_HASH \
	CONFIG_CRYPTO_HASH2 \
	CONFIG_CRYPTO_ALGAPI \
	CONFIG_CRYPTO_ALGAPI2
  FILES:= \
	crypto/crypto_algapi.ko \
	crypto/crypto_hash.ko
  AUTOLOAD:=$(call AutoLoad,02,crypto_hash,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hash))


define KernelPackage/crypto-hmac
  TITLE:=HMAC digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_HMAC
  FILES:=crypto/hmac.ko
  AUTOLOAD:=$(call AutoLoad,09,hmac)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hmac))


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
	drivers/crypto/ccp/ccp.ko \
	drivers/crypto/ccp/ccp-crypto.ko
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
  FILES:=drivers/crypto/geode-aes.ko
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
  FILES:=drivers/crypto/hifn_795x.ko
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
  FILES:=drivers/crypto/intel/ixp4xx/ixp4xx_crypto.ko
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
	drivers/crypto/padlock-aes.ko \
	drivers/crypto/padlock-sha.ko
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
  FILES:=drivers/crypto/inside-secure/crypto_safexcel.ko
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
	drivers/crypto/talitos.ko
  AUTOLOAD:=$(call AutoLoad,09,talitos)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-talitos))

define KernelPackage/crypto-hw-eip93
  TITLE:=MTK EIP93 crypto module
  DEPENDS:=@TARGET_ramips_mt7621 \
	+kmod-crypto-authenc \
	+kmod-crypto-des \
	+kmod-crypto-md5 \
	+kmod-crypto-sha1 \
	+kmod-crypto-sha256
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_EIP93 \
	CONFIG_CRYPTO_DEV_EIP93_AES=y \
	CONFIG_CRYPTO_DEV_EIP93_DES=y \
	CONFIG_CRYPTO_DEV_EIP93_AEAD=y \
	CONFIG_CRYPTO_DEV_EIP93_GENERIC_SW_MAX_LEN=256 \
	CONFIG_CRYPTO_DEV_EIP93_AES_128_SW_MAX_LEN=512
  FILES:=drivers/crypto/mtk-eip93/crypto-hw-eip93.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto-hw-eip93)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-hw-eip93/description
Kernel module to enable EIP-93 Crypto engine as found
in the Mediatek MT7621 SoC.
It enables DES/3DES/AES ECB/CBC/CTR and
IPSEC offload with authenc(hmac(sha1/sha256), aes/cbc/rfc3686)
endef

$(eval $(call KernelPackage,crypto-hw-eip93))

define KernelPackage/crypto-kpp
  TITLE:=Key-agreement Protocol Primitives
  KCONFIG:=CONFIG_CRYPTO_KPP
  FILES:=crypto/kpp.ko
  AUTOLOAD:=$(call AutoLoad,09,kpp)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-kpp))

define KernelPackage/crypto-lib-chacha20
  TITLE:=ChaCha library interface
  KCONFIG:=CONFIG_CRYPTO_LIB_CHACHA
  HIDDEN:=1
  FILES:=lib/crypto/libchacha.ko
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-lib-chacha20/x86_64
  KCONFIG+=CONFIG_CRYPTO_CHACHA20_X86_64
  FILES+=arch/x86/crypto/chacha-x86_64.ko
endef

# Note that a non-neon fallback implementation is available on arm32 when
# NEON is not supported, hence all arm targets can utilize lib-chacha20/arm
define KernelPackage/crypto-lib-chacha20/arm
  KCONFIG+=CONFIG_CRYPTO_CHACHA20_NEON
  FILES:=arch/arm/crypto/chacha-neon.ko
endef

KernelPackage/crypto-lib-chacha20/armeb=$(KernelPackage/crypto-lib-chacha20/arm)

define KernelPackage/crypto-lib-chacha20/aarch64
  KCONFIG+=CONFIG_CRYPTO_CHACHA20_NEON
  FILES+=arch/arm64/crypto/chacha-neon.ko
endef

define KernelPackage/crypto-lib-chacha20/mips32r2
  KCONFIG+=CONFIG_CRYPTO_CHACHA_MIPS
  FILES:=arch/mips/crypto/chacha-mips.ko
endef

ifeq ($(CONFIG_CPU_MIPS32_R2),y)
  KernelPackage/crypto-lib-chacha20/$(ARCH)=\
	  $(KernelPackage/crypto-lib-chacha20/mips32r2)
endif

ifdef KernelPackage/crypto-lib-chacha20/$(ARCH)
  KernelPackage/crypto-lib-chacha20/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-lib-chacha20/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-lib-chacha20))


define KernelPackage/crypto-lib-chacha20poly1305
  TITLE:=ChaCha20-Poly1305 AEAD support (8-byte nonce library version)
  KCONFIG:=CONFIG_CRYPTO_LIB_CHACHA20POLY1305
  HIDDEN:=1
  FILES:=lib/crypto/libchacha20poly1305.ko
  $(call AddDepends/crypto, +kmod-crypto-lib-chacha20 +kmod-crypto-lib-poly1305)
endef

$(eval $(call KernelPackage,crypto-lib-chacha20poly1305))


define KernelPackage/crypto-lib-curve25519
  TITLE:=Curve25519 scalar multiplication library
  KCONFIG:=CONFIG_CRYPTO_LIB_CURVE25519
  HIDDEN:=1
  FILES:= \
	lib/crypto/libcurve25519.ko \
	lib/crypto/libcurve25519-generic.ko
  $(call AddDepends/crypto,+PACKAGE_kmod-crypto-kpp:kmod-crypto-kpp)
endef

define KernelPackage/crypto-lib-curve25519/config
  imply PACKAGE_kmod-crypto-kpp
endef

define KernelPackage/crypto-lib-curve25519/x86_64
  KCONFIG+=CONFIG_CRYPTO_CURVE25519_X86
  FILES+=arch/x86/crypto/curve25519-x86_64.ko
endef

define KernelPackage/crypto-lib-curve25519/arm-neon
  KCONFIG+=CONFIG_CRYPTO_CURVE25519_NEON
  FILES+=arch/arm/crypto/curve25519-neon.ko
endef

ifeq ($(ARCH)-$(CONFIG_KERNEL_MODE_NEON),arm-y)
  KernelPackage/crypto-lib-curve25519/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-lib-curve25519/arm-neon)
endif

ifdef KernelPackage/crypto-lib-curve25519/$(ARCH)
  KernelPackage/crypto-lib-curve25519/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-lib-curve25519/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-lib-curve25519))


define KernelPackage/crypto-lib-poly1305
  TITLE:=Poly1305 library interface
  KCONFIG:=CONFIG_CRYPTO_LIB_POLY1305
  HIDDEN:=1
  FILES:=lib/crypto/libpoly1305.ko
  $(call AddDepends/crypto,+PACKAGE_kmod-crypto-hash:kmod-crypto-hash)
endef

define KernelPackage/crypto-lib-poly1305/config
  imply PACKAGE_kmod-crypto-hash
endef

define KernelPackage/crypto-lib-poly1305/x86_64
  KCONFIG+=CONFIG_CRYPTO_POLY1305_X86_64
  FILES+=arch/x86/crypto/poly1305-x86_64.ko
endef

define KernelPackage/crypto-lib-poly1305/arm
  KCONFIG+=CONFIG_CRYPTO_POLY1305_ARM
  FILES:=arch/arm/crypto/poly1305-arm.ko
endef

KernelPackage/crypto-lib-poly1305/armeb=$(KernelPackage/crypto-lib-poly1305/arm)

define KernelPackage/crypto-lib-poly1305/aarch64
  KCONFIG+=CONFIG_CRYPTO_POLY1305_NEON
  FILES:=arch/arm64/crypto/poly1305-neon.ko
endef

define KernelPackage/crypto-lib-poly1305/mips
  KCONFIG+=CONFIG_CRYPTO_POLY1305_MIPS
  FILES:=arch/mips/crypto/poly1305-mips.ko
endef

KernelPackage/crypto-lib-poly1305/mipsel=$(KernelPackage/crypto-lib-poly1305/mips)
KernelPackage/crypto-lib-poly1305/mips64=$(KernelPackage/crypto-lib-poly1305/mips)
KernelPackage/crypto-lib-poly1305/mips64el=$(KernelPackage/crypto-lib-poly1305/mips)

ifdef KernelPackage/crypto-lib-poly1305/$(ARCH)
  KernelPackage/crypto-lib-poly1305/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-lib-poly1305/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-lib-poly1305))


define KernelPackage/crypto-manager
  TITLE:=CryptoAPI algorithm manager
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_MANAGER \
	CONFIG_CRYPTO_MANAGER2
  FILES:=crypto/cryptomgr.ko
  AUTOLOAD:=$(call AutoLoad,09,cryptomgr,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-manager))


define KernelPackage/crypto-md4
  TITLE:=MD4 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_MD4
  FILES:=crypto/md4.ko
  AUTOLOAD:=$(call AutoLoad,09,md4)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-md4))


define KernelPackage/crypto-md5
  TITLE:=MD5 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_MD5 \
	CONFIG_CRYPTO_MD5_OCTEON \
	CONFIG_CRYPTO_MD5_PPC
  FILES:=crypto/md5.ko
  AUTOLOAD:=$(call AutoLoad,09,md5)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-md5/octeon
  FILES+=arch/mips/cavium-octeon/crypto/octeon-md5.ko
  AUTOLOAD+=$(call AutoLoad,09,octeon-md5)
endef

define KernelPackage/crypto-md5/powerpc
  FILES+=arch/powerpc/crypto/md5-ppc.ko
  AUTOLOAD+=$(call AutoLoad,09,md5-ppc)
endef

ifdef KernelPackage/crypto-md5/$(ARCH)
  KernelPackage/crypto-md5/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-md5/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-md5))


define KernelPackage/crypto-michael-mic
  TITLE:=Michael MIC keyed digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_MICHAEL_MIC
  FILES:=crypto/michael_mic.ko
  AUTOLOAD:=$(call AutoLoad,09,michael_mic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-michael-mic))


define KernelPackage/crypto-misc
  TITLE:=Other CryptoAPI modules
  DEPENDS:=+kmod-crypto-xts +kmod-crypto-user
  KCONFIG:= \
	CONFIG_CRYPTO_USER_API_ENABLE_OBSOLETE=y \
	CONFIG_CRYPTO_CAMELLIA_X86_64 \
	CONFIG_CRYPTO_BLOWFISH_X86_64 \
	CONFIG_CRYPTO_TWOFISH_X86_64 \
	CONFIG_CRYPTO_TWOFISH_X86_64_3WAY \
	CONFIG_CRYPTO_SERPENT_SSE2_X86_64 \
	CONFIG_CRYPTO_CAMELLIA_AESNI_AVX_X86_64 \
	CONFIG_CRYPTO_CAST5_AVX_X86_64 \
	CONFIG_CRYPTO_CAST6_AVX_X86_64 \
	CONFIG_CRYPTO_TWOFISH_AVX_X86_64 \
	CONFIG_CRYPTO_SERPENT_AVX_X86_64 \
	CONFIG_CRYPTO_CAMELLIA_AESNI_AVX2_X86_64 \
	CONFIG_CRYPTO_SERPENT_AVX2_X86_64 \
	CONFIG_CRYPTO_SERPENT_SSE2_586 \
	CONFIG_CRYPTO_ANUBIS \
	CONFIG_CRYPTO_BLOWFISH \
	CONFIG_CRYPTO_CAMELLIA \
	CONFIG_CRYPTO_CAST5 \
	CONFIG_CRYPTO_CAST6 \
	CONFIG_CRYPTO_KHAZAD \
	CONFIG_CRYPTO_SERPENT \
	CONFIG_CRYPTO_TEA \
	CONFIG_CRYPTO_TWOFISH \
	CONFIG_CRYPTO_TWOFISH_COMMON \
	CONFIG_CRYPTO_TWOFISH_586 \
	CONFIG_CRYPTO_WP512
  FILES:= \
	crypto/anubis.ko \
	crypto/camellia_generic.ko \
	crypto/cast_common.ko \
	crypto/cast5_generic.ko \
	crypto/cast6_generic.ko \
	crypto/khazad.ko \
	crypto/tea.ko \
	crypto/twofish_common.ko \
	crypto/wp512.ko \
	crypto/twofish_generic.ko \
	crypto/blowfish_common.ko \
	crypto/blowfish_generic.ko \
	crypto/serpent_generic.ko
  AUTOLOAD:=$(call AutoLoad,10,anubis camellia_generic cast_common \
	cast5_generic cast6_generic khazad tea twofish_common \
	wp512 blowfish_common serpent_generic)
  ifndef CONFIG_TARGET_x86
	AUTOLOAD+= $(call AutoLoad,10,twofish_generic blowfish_generic)
  endif
  $(call AddDepends/crypto)
endef

ifndef CONFIG_x86_64
  define KernelPackage/crypto-misc/x86
    FILES+= \
	arch/x86/crypto/twofish-i586.ko \
	arch/x86/crypto/serpent-sse2-i586.ko \
	crypto/cryptd.ko \
	crypto/crypto_simd.ko
    AUTOLOAD+= $(call AutoLoad,10,cryptd \
	serpent-sse2-i586 twofish-i586 blowfish_generic)
  endef
endif

define KernelPackage/crypto-misc/x86_64
  FILES+= \
	arch/x86/crypto/camellia-x86_64.ko \
	arch/x86/crypto/blowfish-x86_64.ko \
	arch/x86/crypto/twofish-x86_64.ko \
	arch/x86/crypto/twofish-x86_64-3way.ko \
	arch/x86/crypto/serpent-sse2-x86_64.ko \
	arch/x86/crypto/camellia-aesni-avx-x86_64.ko \
	arch/x86/crypto/cast5-avx-x86_64.ko \
	arch/x86/crypto/cast6-avx-x86_64.ko \
	arch/x86/crypto/twofish-avx-x86_64.ko \
	arch/x86/crypto/serpent-avx-x86_64.ko \
	arch/x86/crypto/camellia-aesni-avx2.ko \
	arch/x86/crypto/serpent-avx2.ko
  AUTOLOAD+= $(call AutoLoad,10,camellia-x86_64 \
	camellia-aesni-avx-x86_64 camellia-aesni-avx2 cast5-avx-x86_64 \
	cast6-avx-x86_64 twofish-x86_64 twofish-x86_64-3way \
	twofish-avx-x86_64 blowfish-x86_64 serpent-avx-x86_64 serpent-avx2)
endef

ifdef KernelPackage/crypto-misc/$(ARCH)
  KernelPackage/crypto-misc/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-misc/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-misc))


define KernelPackage/crypto-null
  TITLE:=Null CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_NULL
  FILES:=crypto/crypto_null.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_null)
  $(call AddDepends/crypto, +kmod-crypto-hash)
endef

$(eval $(call KernelPackage,crypto-null))


define KernelPackage/crypto-pcbc
  TITLE:=Propagating Cipher Block Chaining CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_PCBC
  FILES:=crypto/pcbc.ko
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
	lib/mpi/mpi.ko@lt6.5 \
	lib/crypto/mpi/mpi.ko@ge6.5 \
	crypto/akcipher.ko \
	crypto/rsa_generic.ko
  AUTOLOAD:=$(call AutoLoad,10,rsa_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rsa))


define KernelPackage/crypto-rmd160
  TITLE:=RIPEMD160 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_RMD160
  FILES:=crypto/rmd160.ko
  AUTOLOAD:=$(call AutoLoad,09,rmd160)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rmd160))


define KernelPackage/crypto-rng
  TITLE:=CryptoAPI random number generation
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-hmac +kmod-crypto-sha512 +LINUX_6_6:kmod-crypto-sha3
  KCONFIG:= \
	CONFIG_CRYPTO_DRBG \
	CONFIG_CRYPTO_DRBG_HMAC=y \
	CONFIG_CRYPTO_DRBG_HASH=n \
	CONFIG_CRYPTO_DRBG_MENU \
	CONFIG_CRYPTO_JITTERENTROPY \
	CONFIG_CRYPTO_RNG2
  FILES:= \
	crypto/drbg.ko \
	crypto/jitterentropy_rng.ko \
	crypto/rng.ko
  AUTOLOAD:=$(call AutoLoad,09,drbg jitterentropy_rng rng)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rng))


define KernelPackage/crypto-geniv
  TITLE:=CryptoAPI Shared IV generator
  HIDDEN:=1
  DEPENDS:=+kmod-crypto-rng +kmod-crypto-aead @LINUX_6_6
  KCONFIG:=CONFIG_CRYPTO_GENIV
  FILES:=crypto/geniv.ko
  AUTOLOAD:=$(call AutoLoad,09,geniv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-geniv))


define KernelPackage/crypto-seqiv
  TITLE:=CryptoAPI Sequence Number IV Generator
  DEPENDS:=+kmod-crypto-aead +kmod-crypto-rng +LINUX_6_6:kmod-crypto-geniv
  KCONFIG:=CONFIG_CRYPTO_SEQIV
  FILES:=crypto/seqiv.ko
  AUTOLOAD:=$(call AutoLoad,09,seqiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-seqiv))


define KernelPackage/crypto-sha1
  TITLE:=SHA1 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA1 \
	CONFIG_CRYPTO_SHA1_ARM \
	CONFIG_CRYPTO_SHA1_ARM_NEON \
	CONFIG_CRYPTO_SHA1_ARM64_CE \
	CONFIG_CRYPTO_SHA1_OCTEON \
	CONFIG_CRYPTO_SHA1_PPC_SPE \
	CONFIG_CRYPTO_SHA1_SSSE3
  FILES:=crypto/sha1_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha1_generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-sha1/arm
  FILES+=arch/arm/crypto/sha1-arm.ko
  AUTOLOAD+=$(call AutoLoad,09,sha1-arm)
endef

define KernelPackage/crypto-sha1/arm-neon
  $(call KernelPackage/crypto-sha1/arm)
  FILES+=arch/arm/crypto/sha1-arm-neon.ko
  AUTOLOAD+=$(call AutoLoad,09,sha1-arm-neon)
endef

define KernelPackage/crypto-sha1/aarch64-ce
  FILES+=arch/arm64/crypto/sha1-ce.ko
  AUTOLOAD+=$(call AutoLoad,09,sha1-ce)
endef

KernelPackage/crypto-sha1/imx/cortexa7=$(KernelPackage/crypto-sha1/arm-neon)
KernelPackage/crypto-sha1/imx/cortexa9=$(KernelPackage/crypto-sha1/arm-neon)
KernelPackage/crypto-sha1/ipq40xx=$(KernelPackage/crypto-sha1/arm-neon)
KernelPackage/crypto-sha1/mediatek/filogic=$(KernelPackage/crypto-sha1/aarch64-ce)
KernelPackage/crypto-sha1/mediatek/mt7622=$(KernelPackage/crypto-sha1/aarch64-ce)
KernelPackage/crypto-sha1/mvebu/cortexa9=$(KernelPackage/crypto-sha1/arm-neon)
KernelPackage/crypto-sha1/mvebu/cortexa53=$(KernelPackage/crypto-sha1/aarch64-ce)
KernelPackage/crypto-sha1/mvebu/cortexa72=$(KernelPackage/crypto-sha1/aarch64-ce)
KernelPackage/crypto-sha1/qualcommax=$(KernelPackage/crypto-sha1/aarch64-ce)
KernelPackage/crypto-sha1/rockchip/armv8=$(KernelPackage/crypto-sha1/aarch64-ce)

define KernelPackage/crypto-sha1/octeon
  FILES+=arch/mips/cavium-octeon/crypto/octeon-sha1.ko
  AUTOLOAD+=$(call AutoLoad,09,octeon-sha1)
endef

KernelPackage/crypto-sha1/tegra=$(KernelPackage/crypto-sha1/arm)

define KernelPackage/crypto-sha1/mpc85xx
  FILES+=arch/powerpc/crypto/sha1-ppc-spe.ko
  AUTOLOAD+=$(call AutoLoad,09,sha1-ppc-spe)
endef

ifndef CONFIG_TARGET_uml
define KernelPackage/crypto-sha1/x86_64
  FILES+=arch/x86/crypto/sha1-ssse3.ko
  AUTOLOAD+=$(call AutoLoad,09,sha1-ssse3)
endef
endif

ifdef KernelPackage/crypto-sha1/$(ARCH)
  KernelPackage/crypto-sha1/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-sha1/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-sha1))


define KernelPackage/crypto-sha3
  TITLE:=SHA3 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= CONFIG_CRYPTO_SHA3
  FILES:=crypto/sha3_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha3_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha3))


define KernelPackage/crypto-sha256
  TITLE:=SHA224 SHA256 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA256 \
	CONFIG_CRYPTO_SHA256_OCTEON \
	CONFIG_CRYPTO_SHA256_PPC_SPE \
	CONFIG_CRYPTO_SHA256_ARM64 \
	CONFIG_CRYPTO_SHA2_ARM64_CE \
	CONFIG_CRYPTO_SHA256_SSSE3
  FILES:= \
	crypto/sha256_generic.ko \
	lib/crypto/libsha256.ko
  AUTOLOAD:=$(call AutoLoad,09,sha256_generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-sha256/aarch64
  FILES+=arch/arm64/crypto/sha256-arm64.ko
  AUTOLOAD+=$(call AutoLoad,09,sha256-arm64)
endef

define KernelPackage/crypto-sha256/aarch64-ce
  $(call KernelPackage/crypto-sha256/aarch64)
  FILES+=arch/arm64/crypto/sha2-ce.ko
  AUTOLOAD+=$(call AutoLoad,09,sha2-ce)
endef

define KernelPackage/crypto-sha256/octeon
  FILES+=arch/mips/cavium-octeon/crypto/octeon-sha256.ko
  AUTOLOAD+=$(call AutoLoad,09,octeon-sha256)
endef

define KernelPackage/crypto-sha256/mpc85xx
  FILES+=arch/powerpc/crypto/sha256-ppc-spe.ko
  AUTOLOAD+=$(call AutoLoad,09,sha256-ppc-spe)
endef

ifndef CONFIG_TARGET_uml
define KernelPackage/crypto-sha256/x86_64
  FILES+=arch/x86/crypto/sha256-ssse3.ko
  AUTOLOAD+=$(call AutoLoad,09,sha256-ssse3)
endef
endif

KernelPackage/crypto-sha256/mediatek/filogic=$(KernelPackage/crypto-sha256/aarch64-ce)
KernelPackage/crypto-sha256/mediatek/mt7622=$(KernelPackage/crypto-sha256/aarch64-ce)
KernelPackage/crypto-sha256/mvebu/cortexa53=$(KernelPackage/crypto-sha256/aarch64-ce)
KernelPackage/crypto-sha256/mvebu/cortexa72=$(KernelPackage/crypto-sha256/aarch64-ce)
KernelPackage/crypto-sha256/qualcommax=$(KernelPackage/crypto-sha256/aarch64-ce)
KernelPackage/crypto-sha256/rockchip/armv8=$(KernelPackage/crypto-sha256/aarch64-ce)

ifdef KernelPackage/crypto-sha256/$(ARCH)
  KernelPackage/crypto-sha256/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-sha256/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-sha256))


define KernelPackage/crypto-sha512
  TITLE:=SHA512 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:= \
	CONFIG_CRYPTO_SHA512 \
	CONFIG_CRYPTO_SHA512_ARM \
	CONFIG_CRYPTO_SHA512_ARM64 \
	CONFIG_CRYPTO_SHA512_OCTEON \
	CONFIG_CRYPTO_SHA512_SSSE3
  FILES:=crypto/sha512_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha512_generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-sha512/arm
  FILES+=arch/arm/crypto/sha512-arm.ko
  AUTOLOAD+=$(call AutoLoad,09,sha512-arm)
endef

define KernelPackage/crypto-sha512/aarch64
  FILES+=arch/arm64/crypto/sha512-arm64.ko
  AUTOLOAD+=$(call AutoLoad,09,sha512-arm64)
endef

KernelPackage/crypto-sha512/imx/cortexa7=$(KernelPackage/crypto-sha512/arm)
KernelPackage/crypto-sha512/imx/cortexa9=$(KernelPackage/crypto-sha512/arm)
KernelPackage/crypto-sha512/ipq40xx=$(KernelPackage/crypto-sha512/arm)
KernelPackage/crypto-sha512/mvebu/cortexa9=$(KernelPackage/crypto-sha512/arm)

define KernelPackage/crypto-sha512/octeon
  FILES+=arch/mips/cavium-octeon/crypto/octeon-sha512.ko
  AUTOLOAD+=$(call AutoLoad,09,octeon-sha512)
endef

KernelPackage/crypto-sha512/tegra=$(KernelPackage/crypto-sha512/arm)

ifndef CONFIG_TARGET_uml
define KernelPackage/crypto-sha512/x86_64
  FILES+=arch/x86/crypto/sha512-ssse3.ko
  AUTOLOAD+=$(call AutoLoad,09,sha512-ssse3)
endef
endif

ifdef KernelPackage/crypto-sha512/$(ARCH)
  KernelPackage/crypto-sha512/$(CRYPTO_TARGET)=\
	  $(KernelPackage/crypto-sha512/$(ARCH))
endif

$(eval $(call KernelPackage,crypto-sha512))


define KernelPackage/crypto-test
  TITLE:=Test CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_TEST
  FILES:=crypto/tcrypt.ko
  $(call AddDepends/crypto,+kmod-crypto-manager)
endef

$(eval $(call KernelPackage,crypto-test))


define KernelPackage/crypto-user
  TITLE:=CryptoAPI userspace interface
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager +LINUX_6_6:kmod-crypto-rng
  KCONFIG:= \
	CONFIG_CRYPTO_USER \
	CONFIG_CRYPTO_USER_API \
	CONFIG_CRYPTO_USER_API_AEAD \
	CONFIG_CRYPTO_USER_API_HASH \
	CONFIG_CRYPTO_USER_API_RNG \
	CONFIG_CRYPTO_USER_API_SKCIPHER
  FILES:= \
	crypto/af_alg.ko \
	crypto/algif_aead.ko \
	crypto/algif_hash.ko \
	crypto/algif_rng.ko \
	crypto/algif_skcipher.ko \
	crypto/crypto_user.ko
  AUTOLOAD:=$(call AutoLoad,09,af_alg algif_aead algif_hash algif_rng algif_skcipher crypto_user)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-user))


define KernelPackage/crypto-xts
  TITLE:=XTS cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-gf128 +kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_XTS
  FILES:=crypto/xts.ko
  AUTOLOAD:=$(call AutoLoad,09,xts)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xts))


define KernelPackage/crypto-xxhash
  TITLE:=xxHash non-cryptographic hash algorithm
  DEPENDS:=+kmod-crypto-hash +kmod-lib-xxhash
  KCONFIG:=CONFIG_CRYPTO_XXHASH
  FILES:=crypto/xxhash_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,xxhash_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xxhash))

