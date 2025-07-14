#
# Copyright (C) 2025
# Licensed under the GPL-2.0+
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python3-netifaces
PKG_VERSION:=0.11.0
PKG_RELEASE:=1
PKG_SOURCE:=netifaces-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://files.pythonhosted.org/packages/source/n/netifaces/
PKG_HASH:=5f658f4a472929b4a456b7d7851230ef70c2ffb31cfcc8b59a025a1c0e16eace

PKG_BUILD_DIR:=$(BUILD_DIR)/netifaces-$(PKG_VERSION)
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(TOPDIR)/feeds/packages/lang/python/python3-package.mk

define Package/python3-netifaces
  SECTION:=lang
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=netifaces module for Python
  URL:=https://pypi.org/project/netifaces/
  DEPENDS:=+python3
endef

define Package/python3-netifaces/description
  Portable access to network interfaces from Python.
endef

$(eval $(call Py3Package,python3-netifaces))
$(eval $(call BuildPackage,python3-netifaces))
