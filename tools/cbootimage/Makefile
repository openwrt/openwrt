#
# Copyright (c) 2017-2019 Tomasz Maciej Nowak <tmn505@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cbootimage
PKG_VERSION:=1.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/NVIDIA/cbootimage/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=46cce01f07df977668de9e1d49884d8ed539169994215606a9719d17b9db8804

HOST_BUILD_PARALLEL := 1

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	(cd $(HOST_BUILD_DIR); autoreconf --install --symlink)
	$(call Host/Configure/Default)
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/$(PKG_NAME) \
		$(STAGING_DIR_HOST)/share/man/man1/$(PKG_NAME).1
endef

$(eval $(call HostBuild))
