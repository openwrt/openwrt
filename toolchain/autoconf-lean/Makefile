# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=autoconf-lean
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2012-09-01
PKG_SOURCE_URL:=https://github.com/GregorR/autoconf-lean
PKG_SOURCE_VERSION:=bd81206959881c511b28e16e81b0a281fd15bd02

PKG_HASH:=e728ed296fe9f069a0e005003c3d6b2dde3d9cad453422a10d6558616d304cc8

HOST_FIXUP := autoreconf

HOST_BUILD_PREFIX:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIG_SITE:=$(CURDIR)/config.site

CONFIGURE_PATH := config-site-generator

HOST_CONFIGURE_VARS := \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"

HOST_CONFIGURE_ARGS := \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--program-prefix="" \
	--program-suffix="" \
	--prefix=$(TOOLCHAIN_DIR) \
	--exec-prefix=$(TOOLCHAIN_DIR) \
	--bindir=$(TOOLCHAIN_DIR)/bin \
	--sbindir=$(TOOLCHAIN_DIR)/sbin \
	--libexecdir=$(TOOLCHAIN_DIR)/lib

HOST_MAKE_VARS :=

HOST_MAKE_FLAGS :=

define Host/Configure
	$(call Host/Configure/Default,,,config-site-generator)
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/config-site-generator
endef

define Host/Install
	grep -v '/' \
		$(HOST_BUILD_DIR)/config-site-generator/config.site \
		> $(TOOLCHAIN_DIR)/config.site
endef

$(eval $(call HostBuild))
