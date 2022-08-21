# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=genext2fs
PKG_VERSION:=1.4.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/bestouff/$(PKG_NAME)/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=0d706f08b11051aae00fe67d88b7aac1fdd0d5d96163704508e21fb6460f899d

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS = \
	--target=$(GNU_HOST_NAME) \
	--host=$(GNU_HOST_NAME) \
	--build=$(GNU_HOST_NAME) \
	--program-prefix="" \
	--program-suffix="" \
	--prefix=/usr \
	--exec-prefix=/usr \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	--libexecdir=/usr/lib \
	--sysconfdir=/etc \
	--datadir=/usr/share \
	--localstatedir=/var \
	--mandir=/usr/man \
	--infodir=/usr/info \

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) \
		CFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		all
endef

define Host/Install
	install -m0755 $(HOST_BUILD_DIR)/genext2fs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/genext2fs
endef

$(eval $(call HostBuild))
