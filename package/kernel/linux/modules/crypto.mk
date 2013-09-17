#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

CRYPTO_MENU:=Cryptographic API modules

CRYPTO_MODULES = ALGAPI2=crypto_algapi

CRYPTOMGR_MODULES = \
	AEAD2=aead \
	MANAGER2=cryptomgr \
	BLKCIPHER2=crypto_blkcipher

crypto_confvar=CONFIG_CRYPTO_$(word 1,$(subst =,$(space),$(1)))
crypto_file=$(LINUX_DIR)/crypto/$(word 2,$(subst =,$(space),$(1))).ko
crypto_name=$(if $(findstring y,$($(call crypto_confvar,$(1)))),,$(word 2,$(subst =,$(space),$(1))))

define KernelPackage/crypto-core
  SUBMENU:=$(CRYPTO_MENU)
  TITLE:=Core CryptoAPI modules
  KCONFIG:= \
	CONFIG_CRYPTO=y \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_ALGAPI \
	$(foreach mod,$(CRYPTO_MODULES),$(call crypto_confvar,$(mod)))
  FILES:=$(foreach mod,$(CRYPTO_MODULES),$(call crypto_file,$(mod)))
endef

$(eval $(call KernelPackage,crypto-core))


define AddDepends/crypto
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS+=+kmod-crypto-core $(1)
endef

define KernelPackage/crypto-hash
  TITLE:=CryptoAPI hash support
  KCONFIG:=CONFIG_CRYPTO_HASH
  FILES:=$(LINUX_DIR)/crypto/crypto_hash.ko
  AUTOLOAD:=$(call AutoLoad,02,crypto_hash,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hash))


define KernelPackage/crypto-manager
  TITLE:=CryptoAPI algorithm manager
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-pcompress
  KCONFIG:= \
	CONFIG_CRYPTO_AEAD \
	CONFIG_CRYPTO_BLKCIPHER \
	CONFIG_CRYPTO_MANAGER \
	$(foreach mod,$(CRYPTOMGR_MODULES),$(call crypto_confvar,$(mod)))
  FILES:=$(foreach mod,$(CRYPTOMGR_MODULES),$(call crypto_file,$(mod)))
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-manager))


define KernelPackage/crypto-pcompress
  TITLE:=CryptoAPI Partial (de)compression operations
  KCONFIG:= \
	CONFIG_CRYPTO_PCOMP=y \
	CONFIG_CRYPTO_PCOMP2
  FILES:=$(LINUX_DIR)/crypto/pcompress.ko
  AUTOLOAD:=$(call AutoLoad,09,pcompress)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-pcompress))


define KernelPackage/crypto-user
  TITLE:=CryptoAPI userspace interface
  DEPENDS:=+kmod-crypto-hash +kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_USER_API \
	CONFIG_CRYPTO_USER_API_HASH \
	CONFIG_CRYPTO_USER_API_SKCIPHER
  FILES:= \
	$(LINUX_DIR)/crypto/af_alg.ko \
	$(LINUX_DIR)/crypto/algif_hash.ko \
	$(LINUX_DIR)/crypto/algif_skcipher.ko
  AUTOLOAD:=$(call AutoLoad,09,af_alg algif_hash algif_skcipher)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-user))


define KernelPackage/crypto-wq
  TITLE:=CryptoAPI work queue handling
  KCONFIG:=CONFIG_CRYPTO_WORKQUEUE
  FILES:=$(LINUX_DIR)/crypto/crypto_wq.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_wq)
  $(call AddDepends/crypto)
endef
$(eval $(call KernelPackage,crypto-wq))

define KernelPackage/crypto-rng
  TITLE:=CryptoAPI random number generation
  KCONFIG:=CONFIG_CRYPTO_RNG2
  FILES:= \
	$(LINUX_DIR)/crypto/rng.ko \
	$(LINUX_DIR)/crypto/krng.ko
  AUTOLOAD:=$(call AutoLoad,09,rng krng)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-rng))


define KernelPackage/crypto-iv
  TITLE:=CryptoAPI initialization vectors
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-rng +kmod-crypto-wq
  KCONFIG:= CONFIG_CRYPTO_BLKCIPHER2
  FILES:= \
	$(LINUX_DIR)/crypto/eseqiv.ko \
	$(LINUX_DIR)/crypto/chainiv.ko
  AUTOLOAD:=$(call AutoLoad,10,eseqiv chainiv)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-iv))


define KernelPackage/crypto-hw-talitos
  TITLE:=Freescale integrated security engine (SEC) driver
  DEPENDS:=+kmod-crypto-aes +kmod-crypto-manager +kmod-crypto-hash +kmod-random-core
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_TALITOS
  FILES:= \
	$(LINUX_DIR)/drivers/crypto/talitos.ko
  AUTOLOAD:=$(call AutoLoad,09,talitos)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-talitos))


define KernelPackage/crypto-hw-padlock
  TITLE:=VIA PadLock ACE with AES/SHA hw crypto module
  DEPENDS:=+kmod-crypto-aes +kmod-crypto-manager
  KCONFIG:= \
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


define KernelPackage/crypto-hw-geode
  TITLE:=AMD Geode hardware crypto module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_GEODE
  FILES:=$(LINUX_DIR)/drivers/crypto/geode-aes.ko
  AUTOLOAD:=$(call AutoLoad,09,geode-aes)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hw-geode))


define KernelPackage/crypto-hw-hifn-795x
  TITLE:=HIFN 795x crypto accelerator
  DEPENDS:=+kmod-random-core +kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_HIFN_795X \
	CONFIG_CRYPTO_DEV_HIFN_795X_RNG=y
  FILES:=$(LINUX_DIR)/drivers/crypto/hifn_795x.ko
  AUTOLOAD:=$(call AutoLoad,09,hifn_795x)
  $(call AddDepends/crypto,+kmod-crypto-des)
endef

$(eval $(call KernelPackage,crypto-hw-hifn-795x))


define KernelPackage/crypto-hw-ppc4xx
  TITLE:=AMCC PPC4xx hardware crypto module
  DEPENDS:=@TARGET_ppc40x||TARGET_ppc44x
  KCONFIG:= \
	CONFIG_CRYPTO_DEV_PPC4XX
  FILES:=$(LINUX_DIR)/drivers/crypto/amcc/crypto4xx.ko
  AUTOLOAD:=$(call AutoLoad,90,crypto4xx)
  $(call AddDepends/crypto,+kmod-crypto-manager +kmod-crypto-hash)
endef

define KernelPackage/crypto-hw-ppc4xx/description
  Kernel support for the AMCC PPC4xx HW crypto engine.
endef

$(eval $(call KernelPackage,crypto-hw-ppc4xx))


define KernelPackage/crypto-aes
  TITLE:=AES cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_AES CONFIG_CRYPTO_AES_586
  FILES:=$(LINUX_DIR)/crypto/aes_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,aes_generic)
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-aes/x86
  FILES+=$(LINUX_DIR)/arch/x86/crypto/aes-i586.ko
  AUTOLOAD:=$(call AutoLoad,09,aes-i586)
endef

$(eval $(call KernelPackage,crypto-aes))


define KernelPackage/crypto-arc4
  TITLE:=ARC4 (RC4) cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_ARC4
  FILES:=$(LINUX_DIR)/crypto/arc4.ko
  AUTOLOAD:=$(call AutoLoad,09,arc4)
  $(call AddDepends/crypto,+!LINUX_3_3:kmod-crypto-manager)
endef

$(eval $(call KernelPackage,crypto-arc4))


define KernelPackage/crypto-authenc
  TITLE:=Combined mode wrapper for IPsec
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_AUTHENC
  FILES:=$(LINUX_DIR)/crypto/authenc.ko
  AUTOLOAD:=$(call AutoLoad,09,authenc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-authenc))

define KernelPackage/crypto-cbc
  TITLE:=Cipher Block Chaining CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_CBC
  FILES:=$(LINUX_DIR)/crypto/cbc.ko
  AUTOLOAD:=$(call AutoLoad,09,cbc)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-cbc))

define KernelPackage/crypto-crc32c
  TITLE:=CRC32c CRC module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_CRC32C
  FILES:=$(LINUX_DIR)/crypto/crc32c.ko
  AUTOLOAD:=$(call AutoLoad,04,crc32c,1)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-crc32c))


define KernelPackage/crypto-des
  TITLE:=DES/3DES cipher CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_DES
  FILES:=$(LINUX_DIR)/crypto/des_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,des_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-des))


define KernelPackage/crypto-deflate
  TITLE:=Deflate compression CryptoAPI module
  DEPENDS:=+kmod-lib-zlib
  KCONFIG:=CONFIG_CRYPTO_DEFLATE
  FILES:=$(LINUX_DIR)/crypto/deflate.ko
  AUTOLOAD:=$(call AutoLoad,09,deflate)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-deflate))


define KernelPackage/crypto-ecb
  TITLE:=Electronic CodeBook CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:=CONFIG_CRYPTO_ECB
  FILES:=$(LINUX_DIR)/crypto/ecb.ko
  AUTOLOAD:=$(call AutoLoad,09,ecb)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ecb))


define KernelPackage/crypto-hmac
  TITLE:=HMAC digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_HMAC
  FILES:=$(LINUX_DIR)/crypto/hmac.ko
  DEPENDS:=+kmod-crypto-manager
  AUTOLOAD:=$(call AutoLoad,09,hmac)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-hmac))


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
  KCONFIG:=CONFIG_CRYPTO_MD5
  FILES:=$(LINUX_DIR)/crypto/md5.ko
  AUTOLOAD:=$(call AutoLoad,09,md5)
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


define KernelPackage/crypto-sha1
  TITLE:=SHA1 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_SHA1
  FILES:=$(LINUX_DIR)/crypto/sha1_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha1_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha1))


define KernelPackage/crypto-sha256
  TITLE:=SHA224 SHA256 digest CryptoAPI module
  DEPENDS:=+kmod-crypto-hash
  KCONFIG:=CONFIG_CRYPTO_SHA256
  FILES:=$(LINUX_DIR)/crypto/sha256_generic.ko
  AUTOLOAD:=$(call AutoLoad,09,sha256_generic)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-sha256))


ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.6.0)),1)
camellia_mod_suffix=_generic
endif
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.7.0)),1)
cast56_mod_suffix=_generic
endif

define KernelPackage/crypto-misc
  TITLE:=Other CryptoAPI modules
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_ANUBIS \
	CONFIG_CRYPTO_BLOWFISH \
	CONFIG_CRYPTO_CAMELLIA \
	CONFIG_CRYPTO_CAST5 \
	CONFIG_CRYPTO_CAST6 \
	CONFIG_CRYPTO_FCRYPT \
	CONFIG_CRYPTO_KHAZAD \
	CONFIG_CRYPTO_SERPENT \
	CONFIG_CRYPTO_SHA512 \
	CONFIG_CRYPTO_TEA \
	CONFIG_CRYPTO_TGR192 \
	CONFIG_CRYPTO_TWOFISH \
	CONFIG_CRYPTO_TWOFISH_COMMON \
	CONFIG_CRYPTO_TWOFISH_586 \
	CONFIG_CRYPTO_WP512
  FILES:= \
	$(LINUX_DIR)/crypto/anubis.ko \
	$(LINUX_DIR)/crypto/camellia$(camellia_mod_suffix).ko \
	$(if $(call kernel_patchver_ge,3.7),$(LINUX_DIR)/crypto/cast_common.ko) \
	$(LINUX_DIR)/crypto/cast5$(cast56_mod_suffix).ko \
	$(LINUX_DIR)/crypto/cast6$(cast56_mod_suffix).ko \
	$(LINUX_DIR)/crypto/fcrypt.ko \
	$(LINUX_DIR)/crypto/khazad.ko \
	$(LINUX_DIR)/crypto/sha512_generic.ko \
	$(LINUX_DIR)/crypto/tea.ko \
	$(LINUX_DIR)/crypto/tgr192.ko \
	$(LINUX_DIR)/crypto/twofish_common.ko \
	$(LINUX_DIR)/crypto/wp512.ko \
	$(LINUX_DIR)/crypto/twofish_generic.ko \
	$(LINUX_DIR)/crypto/blowfish_common.ko \
	$(LINUX_DIR)/crypto/blowfish_generic.ko \
	$(LINUX_DIR)/crypto/serpent_generic.ko
  $(call AddDepends/crypto)
endef

define KernelPackage/crypto-misc/x86
  FILES+=$(LINUX_DIR)/arch/x86/crypto/twofish-i586.ko
endef

$(eval $(call KernelPackage,crypto-misc))


define KernelPackage/crypto-ocf
  TITLE:=OCF modules
  DEPENDS:=+@OPENSSL_ENGINE_CRYPTO @!TARGET_uml +kmod-crypto-manager
  KCONFIG:= \
	CONFIG_OCF_OCF \
	CONFIG_OCF_CRYPTODEV \
	CONFIG_OCF_CRYPTOSOFT \
	CONFIG_OCF_FIPS=y \
	CONFIG_OCF_RANDOMHARVEST=y
  FILES:= \
	$(LINUX_DIR)/crypto/ocf/ocf.ko \
	$(LINUX_DIR)/crypto/ocf/cryptodev.ko \
	$(LINUX_DIR)/crypto/ocf/cryptosoft.ko
  AUTOLOAD:=$(call AutoLoad,09, \
	ocf \
	cryptodev \
	cryptosoft \
  )
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ocf))


define KernelPackage/crypto-ocf-hifn7751
  TITLE:=OCF support for Hifn 6500/7751/7811/795x, Invertex AEON and NetSec 7751 devices
  DEPENDS:=+@OPENSSL_ENGINE_CRYPTO @PCI_SUPPORT @!TARGET_uml kmod-crypto-ocf
  KCONFIG:=CONFIG_OCF_HIFN
  FILES:=$(LINUX_DIR)/crypto/ocf/hifn/hifn7751.ko
  AUTOLOAD:=$(call AutoLoad,10,hifn7751)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ocf-hifn7751))


define KernelPackage/crypto-ocf-hifnhipp
  TITLE:=OCF support for Hifn 7855/8155 devices
  DEPENDS:=+@OPENSSL_ENGINE_CRYPTO @PCI_SUPPORT @!TARGET_uml kmod-crypto-ocf
  KCONFIG:=CONFIG_OCF_HIFNHIPP
  FILES:=$(LINUX_DIR)/crypto/ocf/hifn/hifnHIPP.ko
  AUTOLOAD:=$(call AutoLoad,10,hifnHIPP)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-ocf-hifnhipp))


define KernelPackage/crypto-null
  TITLE:=Null CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_NULL
  FILES:=$(LINUX_DIR)/crypto/crypto_null.ko
  AUTOLOAD:=$(call AutoLoad,09,crypto_null)
  $(call AddDepends/crypto,+kmod-crypto-manager)
endef

$(eval $(call KernelPackage,crypto-null))


define KernelPackage/crypto-test
  TITLE:=Test CryptoAPI module
  KCONFIG:=CONFIG_CRYPTO_TEST
  FILES:=$(LINUX_DIR)/crypto/tcrypt.ko
  $(call AddDepends/crypto,+kmod-crypto-manager)
endef

$(eval $(call KernelPackage,crypto-test))


define KernelPackage/crypto-xts
  TITLE:=XTS cipher CryptoAPI module
  DEPENDS:=+kmod-crypto-manager
  KCONFIG:= \
	CONFIG_CRYPTO_GF128MUL \
	CONFIG_CRYPTO_XTS
  FILES:= \
	$(LINUX_DIR)/crypto/xts.ko \
	$(LINUX_DIR)/crypto/gf128mul.ko
  AUTOLOAD:=$(call AutoLoad,09, \
	gf128mul \
	xts \
  )
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-xts))


define KernelPackage/crypto-mv-cesa
  TITLE:=Marvell crypto engine
  DEPENDS:=+kmod-crypto-manager +kmod-crypto-aes @TARGET_kirkwood||TARGET_orion||TARGET_mvebu
  KCONFIG:=CONFIG_CRYPTO_DEV_MV_CESA
  FILES:=$(LINUX_DIR)/drivers/crypto/mv_cesa.ko
  AUTOLOAD:=$(call AutoLoad,09,mv_cesa)
  $(call AddDepends/crypto)
endef

$(eval $(call KernelPackage,crypto-mv-cesa))
