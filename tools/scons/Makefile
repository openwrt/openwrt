#
# Copyright (C) 2011-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scons
PKG_VERSION:=2.4.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/scons \
		http://fossies.org/linux/misc/
PKG_MD5SUM:=9a0ddf33d9839f04380e0fae87cc4b40

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	./files/pywrap.sh $(HOST_BUILD_DIR)/setup.py install --prefix=$(STAGING_DIR_HOST)
	rm -f $(STAGING_DIR_HOST)/bin/scons*.py
	for bin in $(STAGING_DIR_HOST)/bin/scons*; do \
		mv "$$$$bin" "$$$$bin.py";                \
		cp ./files/pywrap.sh "$$$$bin";           \
	done
endef

$(eval $(call HostBuild))
