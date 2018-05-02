#
# Copyright (C) 2006-2017 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wolfssl
PKG_VERSION:=3.14.4
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
# PKG_SOURCE_URL:=https://www.wolfssl.com/
PKG_SOURCE_URL:=https://github.com/wolfSSL/wolfssl/archive/v$(PKG_VERSION)
PKG_HASH:=1da1b45dec4a455716c8547074ad883c737865225f69443bb173c0dc21683fd1

PKG_FIXUP:=libtool
PKG_INSTALL:=1
PKG_USE_MIPS16:=0
PKG_BUILD_PARALLEL:=1
PKG_LICENSE:=GPL-2.0+
PKG_CPE_ID:=cpe:/a:yassl:cyassl

define symdep
$(firstword $(subst :, ,$(1)))
endef

define symdep_upper
$(call toupper,$(call symdep,$(1)))
endef

define confsym
$(firstword $(subst $(comma), ,$(lastword $(subst :, ,$(1)))))
endef

define confsym_upper
$(call toupper,$(call confsym,$(1)))
endef

# Define symbols here that will be grouped later in menuconfig
WOLFSSL_GROUP_DEP_SYMBOLS:= \
	aes asn ecc asm coding rsa tls13

# Note:
#   sym1|sym2|...|synN means that `sym1` enables/disables all sym2..symN symbols
#   sym1:sym2 means that `sym2` depdends on `sym1`
WOLFSSL_DEFAULT_ON_OPTS:= \
	wpas,sha512,fortress,fastmath \
	ocsp,ocspstapling,ocspstapling2 \
	aes \
	aes:aesgcm \
	asn \
	asn:ecc \
	ecc:eccshamir \
	ecc:supportedcurves \
	rsa \
	asm rng harden sha512 errorstrings oldtls memory dh coding \
	md5 sha chacha poly1305 hashdrbg filesystem inline extended-master \
	oldnames asyncthreads

WOLFSSL_DEFAULT_OFF_OPTS:= \
	aes:aesccm \
	aes:aesctr \
	aes:aescfb \
	aes:aeskeywrap \
	asm:armasm \
	asm:intelasm \
	asm:intelrand \
	asm:aesni \
	asm:fast-rsa \
	coding:base16 \
	coding:base64encode \
	crl \
	crl:crl-monitor \
	ecc:ecccustcurves \
	ecc:curve25519 \
	ecc:ed25519 \
	ecc:fpecc \
	ecc:eccencrypt \
	rsa:rsapss \
	tls13 \
	tls13:tls13-draft18 \
	tls13:tls13-draft22 \
	tls13:tls13-draft23 \
	distro 32bit singlethreaded dtls postauth hrrcookie sctp mcast openssh \
	nginx haproxy bump sniffer signal maxstrength leanpsk leantls \
	lowresource bigcache hugecache smallcache savesession savecert writedup \
	atomicuser pkcallbacks xilinx camellia md2 nullcipher ripemd blake2 \
	sessioncerts keygen certgen certreq certext sep hkdf x963kdf dsa \
	compkey psk tlsv10 sslv3 trackmemory des3 idea arc4 cmac xts \
	webserver fips hc128 rabbit sha224 sha3 qsh sni maxfragment alpn \
	truncatedhmac renegotiation-indication secure-renegotiation \
	session-ticket tlsx earlydata pkcs7 ssh scep srp indef smallstack \
	valgrind testcert iopool certservice jni lighty md4 pwdbased script \
	cryptonly fastmath fasthugemath crypttests sp sp-math staticmemory mcapi \
	asynccrypt sessionexport 

PKG_CONFIG_DEPENDS:=\
	$(foreach o,$(subst $(comma), ,$(WOLFSSL_DEFAULT_ON_OPTS)$(WOLFSSL_DEFAULT_OFF_OPTS)), \
		CONFIG_WOLFSSL_HAS_$(call confsym_upper,$(o)))

include $(INCLUDE_DIR)/package.mk

define Package/libwolfssl
  SECTION:=libs
  SUBMENU:=SSL
  CATEGORY:=Libraries
  TITLE:=wolfSSL library
  URL:=http://www.wolfssl.com/
  MAINTAINER:=Alexandru Ardelean <ardeleanalex@gmail.com>
  MENU:=1
  PROVIDES:=libcyassl
endef

define Package/libwolfssl/description
wolfSSL (formerly CyaSSL) is an SSL library optimized for small
footprint, both on disk and for memory use.
endef

define boolMenuConfig
config WOLFSSL_HAS_$(call confsym_upper,$(1))
	bool "Include $(call confsym_upper,$(1)) support"
	default $(2)
	$(if $(findstring :,$(1)),depends on WOLFSSL_HAS_$(call symdep_upper,$(1))$(newline))

endef

define boolMenuConfigGroup
$(if $(findstring $(call symdep,$(2)),$(1)),$(call boolMenuConfig,$(2),$(3)))
endef

define boolMenuConfigSingle
$(if $(findstring $(call symdep,$(1)),$(WOLFSSL_GROUP_DEP_SYMBOLS)),,$(call boolMenuConfig,$(1),$(2)))
endef

define Package/libwolfssl/config
if PACKAGE_libwolfssl
# Grouped options first
$(foreach group,$(WOLFSSL_GROUP_DEP_SYMBOLS),\
$(foreach opt,$(subst $(comma), ,$(WOLFSSL_DEFAULT_ON_OPTS)),$(call boolMenuConfigGroup,$(group),$(opt),y)) \
$(foreach opt,$(subst $(comma), ,$(WOLFSSL_DEFAULT_OFF_OPTS)),$(call boolMenuConfigGroup,$(group),$(opt),n)))
# Non-grouped options
$(foreach opt,$(subst $(comma), ,$(WOLFSSL_DEFAULT_ON_OPTS)),$(call boolMenuConfigSingle,$(opt),y))\
$(foreach opt,$(subst $(comma), ,$(WOLFSSL_DEFAULT_OFF_OPTS)),$(call boolMenuConfigSingle,$(opt),n))
endif
endef

TARGET_CFLAGS += $(FPIC)

# --enable-stunnel needed for OpenSSL API compatibility bits
# --disable-opensslcoexist - requires that OpenSSL exist to build
# --disable-optflags - we want to use OpenWrt's CFLAGS
# --disable-stacksize - stack size info on examples (we disable examples)
# --disable-selftest - requires CAVP selftest license
CONFIGURE_ARGS += \
	--enable-opensslextra \
	--enable-stunnel \
	--disable-opensslcoexist \
	--disable-optflags \
	--disable-stacksize \
	--disable-selftest \
	--disable-examples

ifeq ($(CONFIG_IPV6),y)
CONFIGURE_ARGS += \
        --enable-ipv6
else
CONFIGURE_ARGS += \
	--disable-ipv6
endif

define configureArgs
ifeq ($(CONFIG_WOLFSSL_HAS_$(call confsym_upper,$(1))),y)
CONFIGURE_ARGS += --enable-$(call confsym,$(1))
else
CONFIGURE_ARGS += --disable-$(call confsym,$(1))
endif
endef

$(foreach opt,$(subst $(comma), ,$(WOLFSSL_DEFAULT_ON_OPTS)$(WOLFSSL_DEFAULT_OFF_OPTS)), \
	$(eval $(call configureArgs,$(opt))))

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwolfssl.{so*,la} $(1)/usr/lib/
	ln -s libwolfssl.so $(1)/usr/lib/libcyassl.so
	ln -s libwolfssl.la $(1)/usr/lib/libcyassl.la

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig
endef

define Package/libwolfssl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwolfssl.so* $(1)/usr/lib/
	ln -s libwolfssl.so $(1)/usr/lib/libcyassl.so
endef

$(eval $(call BuildPackage,libwolfssl))
