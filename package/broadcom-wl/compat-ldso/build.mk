# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=brcm-compat-ldso
PKG_VERSION:=0.9.28
PKG_EXTRAVERSION:=.2

PKG_SOURCE:=uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION).tar.bz2
PKG_SOURCE_URL:=http://www.uclibc.org/downloads
PKG_MD5SUM:=959f25286e317f0d9e2103445c5a14c2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/brcm-compat/uClibc-$(PKG_VERSION)$(PKG_EXTRAVERSION)

include $(INCLUDE_DIR)/package.mk

UCLIBC_TARGET_ARCH:=mipsel

define Build/Configure
	$(CP) config $(PKG_BUILD_DIR)/.config
	$(SED) 's,^KERNEL_SOURCE=.*,KERNEL_SOURCE=\"$(LINUX_HEADERS_DIR)\",g' \
		-e 's,.*HAS_FPU.*,HAS_FPU=$(if $(CONFIG_SOFT_FLOAT),n\nUCLIBC_HAS_FLOATS=y\nUCLIBC_HAS_SOFT_FLOAT=y,n),g' \
		-e 's,^.*UCLIBC_HAS_LFS.*,UCLIBC_HAS_LFS=$(if $(CONFIG_LARGEFILE),y,n),g' \
		-e 's,.*DO_C99_MATH.*,DO_C99_MATH=$(if $(CONFIG_C99_MATH),y,n),g' \
		$(PKG_BUILD_DIR)/.config
endef

UCLIBC_MAKE := PATH=$(TARGET_PATH) $(MAKE) -C $(PKG_BUILD_DIR) \
		DEVEL_PREFIX=/ \
		RUNTIME_PREFIX=/ \
		HOSTCC="$(HOSTCC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)"

define Build/Compile
	$(SED) 's,^CROSS=.*,CROSS=$(TARGET_CROSS),g' $(PKG_BUILD_DIR)/Rules.mak
	$(UCLIBC_MAKE) PREFIX= pregen all
endef

define Build/InstallDev
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/ld-uClibc.so.0 $(PKG_INSTALL_DIR)/ld-uClibc.brcm
endef

$(eval $(call Build/DefaultTargets))
