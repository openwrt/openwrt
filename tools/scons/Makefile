#
# Copyright (C) 2011-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scons
PKG_VERSION:=3.0.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/scons \
		http://fossies.org/linux/misc/
PKG_HASH:=df676f23dc6d4bfa384fc389d95dcd21ab907e6349d4c848958ba4befb73c73e

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
	cd $(HOST_BUILD_DIR) && $(PYTHON) bootstrap.py build/scons
endef

define Host/Install
	cp ./files/pywrap.sh $(HOST_BUILD_DIR)/build/scons/
	cd $(HOST_BUILD_DIR)/build/scons && \
	    ./pywrap.sh setup.py install --prefix=$(STAGING_DIR_HOST)
	rm -f $(STAGING_DIR_HOST)/bin/scons*.py
	for bin in $(STAGING_DIR_HOST)/bin/scons*; do \
		mv "$$$$bin" "$$$$bin.py";                \
		cp ./files/pywrap.sh "$$$$bin";           \
	done
endef

$(eval $(call HostBuild))
