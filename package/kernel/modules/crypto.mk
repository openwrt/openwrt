#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

CRYPTO_MENU:=Cryptographic API modules

# XXX: added workarounds for modules renamed in 2.6 series:
#  - aes > aes_generic (2.6.24)
#  - blkcipher -> crypto_blkcipher (2.6.25)
#  - des > des_generic (2.6.24)
#  - sha1 > sha1_generic (2.6.24)
#  - sha256 > sha256_generic (2.6.24)
#  - sha512 > sha512_generic (2.6.26)
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.24)),1)
  CRYPTO_GENERIC:=_generic
  AES_SUFFIX:=$(CRYPTO_GENERIC)
  DES_SUFFIX:=$(CRYPTO_GENERIC)
  SHA1_SUFFIX:=$(CRYPTO_GENERIC)
  SHA256_SUFFIX:=$(CRYPTO_GENERIC)
endif
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.25)),1)
  CRYPTO_PREFIX:=crypto_
  BLKCIPHER_PREFIX:=$(CRYPTO_PREFIX)
endif
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.26)),1)
  SHA512_SUFFIX:=$(CRYPTO_GENERIC)
endif

# XXX: added CONFIG_CRYPTO_HMAC to KCONFIG so that CONFIG_CRYPTO_HASH is 
# always set, even if no hash modules are selected
define KernelPackage/crypto-core
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Core CryptoAPI modules
  KCONFIG:= \
	CONFIG_CRYPTO=y \
	CONFIG_CRYPTO_ALGAPI \
	CONFIG_CRYPTO_BLKCIPHER \
	CONFIG_CRYPTO_CBC \
	CONFIG_CRYPTO_DEFLATE \
	CONFIG_CRYPTO_ECB \
	CONFIG_CRYPTO_HASH \
	CONFIG_CRYPTO_HMAC \
	CONFIG_CRYPTO_MANAGER
  FILES:= \
	$(LINUX_DIR)/crypto/crypto_algapi.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/$(BLKCIPHER_PREFIX)blkcipher.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/cbc.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/deflate.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ecb.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/crypto_hash.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/cryptomgr.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01, \
	crypto_algapi \
	cryptomgr \
	crypto_hash \
	$(BLKCIPHER_PREFIX)blkcipher \
	cbc \
	ecb \
	deflate \
  )
endef

define KernelPackage/crypto-core/2.4
  FILES:=$(LINUX_DIR)/crypto/deflate.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01, deflate)
endef

$(eval $(call KernelPackage,crypto-core))


define KernelPackage/crypto-hw-padlock
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=VIA PadLock ACE with AES/SHA hw crypto module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_PADLOCK \
	CONFIG_CRYPTO_DEV_PADLOCK_AES \
	CONFIG_CRYPTO_DEV_PADLOCK_SHA
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/padlock-aes.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/crypto/padlock-sha.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,padlock-aes padlock-sha)
endef

$(eval $(call KernelPackage,crypto-hw-padlock))


define KernelPackage/crypto-hw-geode
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=AMD Geode hardware crypto module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_GEODE
  FILES:=$(LINUX_DIR)/drivers/crypto/geode-aes.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,geode-aes)
endef

$(eval $(call KernelPackage,crypto-hw-geode))


define KernelPackage/crypto-hw-hifn-795x
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=HIFN 795x crypto accelerator
  DEPENDS:=+kmod-crypto-core +kmod-crypto-des
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_HIFN_795X \
	CONFIG_CRYPTO_DEV_HIFN_795X_RNG=y
  FILES:=$(LINUX_DIR)/drivers/crypto/hifn_795x.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,hifn_795x)
endef

$(eval $(call KernelPackage,crypto-hw-hifn-795x))


define KernelPackage/crypto-aes
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=AES cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_AES
  FILES:=$(LINUX_DIR)/crypto/aes$(AES_SUFFIX).$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,aes$(AES_SUFFIX))
endef

define KernelPackage/crypto-aes/x86-2.6
  KCONFIG+=CONFIG_CRYPTO_AES_586
  FILES+=$(LINUX_DIR)/arch/x86/crypto/aes-i586.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,aes$(AES_SUFFIX) aes-i586)
endef

$(eval $(call KernelPackage,crypto-aes))


define KernelPackage/crypto-arc4
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=ARC4 (RC4) cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_ARC4
  FILES:=$(LINUX_DIR)/crypto/arc4.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,arc4)
endef

$(eval $(call KernelPackage,crypto-arc4))


define KernelPackage/crypto-aead
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Authenticated Encryption with Associated Data
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_AEAD
  FILES:=$(LINUX_DIR)/crypto/aead.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,aead)
endef

$(eval $(call KernelPackage,crypto-aead))

define KernelPackage/crypto-authenc
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Combined mode wrapper for IPsec
  DEPENDS:=+kmod-crypto-core +kmod-crypto-aead
  KCONFIG:=CONFIG_CRYPTO_AUTHENC
  FILES:=$(LINUX_DIR)/crypto/authenc.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,authenc)
endef

$(eval $(call KernelPackage,crypto-authenc))

define KernelPackage/crypto-des
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=DES/3DES cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_DES
  FILES:=$(LINUX_DIR)/crypto/des$(DES_SUFFIX).$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,des$(DES_SUFFIX))
endef

$(eval $(call KernelPackage,crypto-des))


define KernelPackage/crypto-hmac
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=HMAC digest CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_HMAC
  FILES:=$(LINUX_DIR)/crypto/hmac.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,hmac)
endef

$(eval $(call KernelPackage,crypto-hmac))


define KernelPackage/crypto-md5
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=MD5 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_MD5
  FILES:=$(LINUX_DIR)/crypto/md5.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,md5)
endef

$(eval $(call KernelPackage,crypto-md5))


define KernelPackage/crypto-michael-mic
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Michael MIC keyed digest CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_MICHAEL_MIC
  FILES:=$(LINUX_DIR)/crypto/michael_mic.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,michael_mic)
endef

$(eval $(call KernelPackage,crypto-michael-mic))


define KernelPackage/crypto-sha1
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=SHA1 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_SHA1
  FILES:=$(LINUX_DIR)/crypto/sha1$(SHA1_SUFFIX).$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,sha1$(SHA1_SUFFIX))
endef

$(eval $(call KernelPackage,crypto-sha1))


define KernelPackage/crypto-misc
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Other CryptoAPI modules
  DEPENDS:=+kmod-crypto-core
  KCONFIG:= \
	CONFIG_CRYPTO_ANUBIS \
	CONFIG_CRYPTO_BLOWFISH \
	CONFIG_CRYPTO_CAMELLIA \
	CONFIG_CRYPTO_CAST5 \
	CONFIG_CRYPTO_CAST6 \
	CONFIG_CRYPTO_CRC32C \
	CONFIG_CRYPTO_FCRYPT \
	CONFIG_CRYPTO_KHAZAD \
	CONFIG_CRYPTO_MD4 \
	CONFIG_CRYPTO_SERPENT \
	CONFIG_CRYPTO_SHA256 \
	CONFIG_CRYPTO_SHA512 \
	CONFIG_CRYPTO_TEA \
	CONFIG_CRYPTO_TGR192 \
	CONFIG_CRYPTO_TWOFISH \
	CONFIG_CRYPTO_TWOFISH_586 \
	CONFIG_CRYPTO_WP512
  FILES:= \
	$(LINUX_DIR)/crypto/anubis.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/blowfish.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/cast5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/cast6.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/khazad.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/md4.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/serpent.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/sha256$(SHA256_SUFFIX).$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/sha512$(SHA512_SUFFIX).$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/tea.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/twofish.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/wp512.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/crypto-misc/2.6
  FILES+= \
	$(LINUX_DIR)/crypto/camellia.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/crc32c.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/fcrypt.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/tgr192.$(LINUX_KMOD_SUFFIX)
endef

$(eval $(call KernelPackage,crypto-misc))

define KernelPackage/crypto-ocf
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=OCF modules
  DEPENDS:=+kmod-crypto-core @!TARGET_uml
  KCONFIG:= \
	CONFIG_OCF_OCF \
	CONFIG_OCF_CRYPTODEV \
	CONFIG_OCF_CRYPTOSOFT \
	CONFIG_OCF_FIPS=y \
	CONFIG_OCF_RANDOMHARVEST=y
  FILES:= \
	$(LINUX_DIR)/crypto/ocf/ocf.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ocf/cryptodev.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/crypto/ocf/cryptosoft.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09, \
	ocf \
	cryptodev \
	cryptosoft \
  )
endef

$(eval $(call KernelPackage,crypto-ocf))


define KernelPackage/crypto-null
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Null CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_NULL
  FILES:=$(LINUX_DIR)/crypto/crypto_null.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,crypto_null)
endef

$(eval $(call KernelPackage,crypto-null))


define KernelPackage/crypto-test
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Test CryptoAPI module
  DEPENDS:=+kmod-crypto-core
  KCONFIG:=CONFIG_CRYPTO_TEST
  FILES:=$(LINUX_DIR)/crypto/tcrypt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,09,tcrypt)
endef

$(eval $(call KernelPackage,crypto-test))

