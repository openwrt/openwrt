#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=musl
PKG_VERSION:=$(call qstrip,$(CONFIG_MUSL_VERSION))
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_VERSION_0.9.8:=e6dcebd5efa2d390f0a24dc11444024e9fd1990c
PKG_SOURCE_VERSION_0.9.9:=a57ac9bf64ba202bd94d1b81bddd93b850c7b9d6

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.musl-libc.org/musl
PKG_SOURCE_SUBDIR=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_SOURCE_VERSION_$(PKG_VERSION))
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
LIBC_SO_VERSION:=$(PKG_VERSION)
PATCH_DIR:=$(PATH_PREFIX)/patches-$(PKG_VERSION)
CONFIG_DIR:=$(PATH_PREFIX)/config-$(PKG_VERSION)

HOST_BUILD_DIR:=$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/toolchain-build.mk

MUSL_CONFIGURE:= \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	$(HOST_BUILD_DIR)/configure \
		--prefix=/ \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--disable-gcc-wrapper

ifeq ($(CONFIG_MUSL_ENABLE_DEBUG),y)
MUSL_CONFIGURE+= \
	--enable-debug
endif

define Host/Prepare
	$(call Host/Prepare/Default)
	$(if $(strip $(QUILT)), \
		cd $(HOST_BUILD_DIR); \
		if $(QUILT_CMD) next >/dev/null 2>&1; then \
			$(QUILT_CMD) push -a; \
		fi
	)
	ln -snf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME)
endef

define Host/Configure
	( cd $(HOST_BUILD_DIR); rm -f config.cache; \
		$(MUSL_CONFIGURE) \
	);
endef


define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(BUILD_DIR_TOOLCHAIN)/$(PKG_NAME) \
		$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev
endef
