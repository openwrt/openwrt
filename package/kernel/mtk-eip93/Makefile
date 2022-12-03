#
# Copyright (C) 2006-2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=mtk-eip93
PKG_RELEASE:=1.5

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/crypto-hw-eip93
	SECTION:=kernel
	CATEGORY:=Kernel modules
	SUBMENU:=Cryptographic API modules
	DEPENDS:= @TARGET_ramips_mt7621 +kmod-crypto-manager +kmod-crypto-authenc +kmod-crypto-des +kmod-ipsec +kmod-ipsec4
	KCONFIG:=
	TITLE:=MTK EIP93 crypto module
	FILES:=$(PKG_BUILD_DIR)/crypto-hw-eip93.ko
	AUTOLOAD:=$(call AutoProbe,crypto-hw-eip93)
	MENU:=1
endef

define KernelPackage/crypto-hw-eip93/config
if PACKAGE_kmod-crypto-hw-eip93

comment "Build Options"

config CRYPTO_DEV_EIP93
	tristate
		
config CRYPTO_DEV_EIP93_AES
	bool "Register AES algorithm implementatons with the Crypto API"
	default y
	select CRYPTO_DEV_EIP93
	help
	  Selecting this will offload AES - ECB, CBC and CTR crypto
	  to the EIP-93 crypto engine.

config CRYPTO_DEV_EIP93_DES
	bool "Register DES algorithm implementatons with the Crypto API"
	default y
	select CRYPTO_DEV_EIP93
	help
	  Selecting this will offload DES / 3DES - ECB and CBC crypto
	  to the EIP-93 crypto engine.

config CRYPTO_DEV_EIP93_AEAD
	bool "Register AEAD algorithm implementatons with the Crypto API"
	default y
	select CRYPTO_DEV_EIP93
	help
	  Selecting this will offload AEAD authenc(hmac(x), cipher(y))
	  to the EIP-93 crypto engine.

config CRYPTO_DEV_EIP93_HASH
	bool "Register HASH algorithm implementatons with the Crypto API"
	default n
	select CRYPTO_DEV_EIP93
	help
	  Selecting this will offload SHA1, SHA224 and SHA256 hash algorithm
	  and HMAC(SHA1), HMAC(SHA224) and HMAC(SHA256) to the EIP-93 crypto
	  engine.

config CRYPTO_DEV_EIP93_PRNG
	bool "Register PRNG device with the Crypto API"
	default n
	select CRYPTO_DEV_EIP93
	help
	  Selecting this will add the ANSI X9.31 Pseudo Random Number Generator
	  to the EIP-93 crypto engine.

config CRYPTO_DEV_EIP93_IPSEC
	bool "Register IPSec ESP hardware offloading"
	default n
	select CRYPTO_DEV_EIP93
	select CRYPTO_DEV_EIP93_PRNG
	select XFRM_OFFLOAD
	help
	  Selecting this will add ESP HW offloading for IPSec
	  to the EIP-93 crypto engine. Requires IPSec offload
	  to be selected with the Mediatek Ethernet Driver

config CRYPTO_DEV_EIP93_POLL
	bool "Disable interrupt and use polling. (experimental)"
	default n
	help
 	  Selecting this will disable interrupts and use polling for
	  the EIP-93 crypto engine. (increases CPU load but improves
	  OpenSSL user-space crypto.

endif
endef

EXTRA_KCONFIG:= 

ifdef CONFIG_CRYPTO_DEV_EIP93
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_AES
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_AES=y
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_SKCIPHER=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_DES
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_DES=y
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_SKCIPHER=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_AEAD
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_AEAD=y
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_HMAC=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_HASH
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_HASH=y
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_HMAC=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_PRNG
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_PRNG=y
endif

ifdef CONFIG_CRYPTO_DEV_EIP93_IPSEC
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_IPSEC=y
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_HMAC=y
endif
ifdef CONFIG_CRYPTO_DEV_EIP93_POLL
	EXTRA_KCONFIG += CONFIG_CRYPTO_DEV_EIP93_POLL=y
endif

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,crypto-hw-eip93))
