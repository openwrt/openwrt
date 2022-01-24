# Copyright (C) 2022 Sergey V. Lobanov <sergey@lobanov.in>
#
# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=fakeuname
PKG_RELEASE:=$(COMMITCOUNT)
PKG_LICENSE:=GPL-2.0-or-later

PKG_MAINTAINER:=Sergey V. Lobanov <sergey@lobanov.in>

HOST_BUILD_DEPENDS=linux
PKG_HOST_ONLY:=1

define Package/fakeuname
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Fake uname host tool for cross-compile purposes
endef

define Package/fakeuname/description
 Fakeuname is a host tool for cross-compile cross-platform builds
 if configure or/and build scripts check uname output for target 
 build and use invalid build flags. This tool should not be used 
 in normal case if configure/build scripts allow to redefine 
 required values instead of using uname output
endef

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
	$(CP) src/header.py.inc $(HOST_BUILD_DIR)/$(PKG_NAME)
	echo machine = \"$(ARCH)\" >> $(HOST_BUILD_DIR)/$(PKG_NAME)
	echo kernel_release = \"$(LINUX_UNAME_VERSION)\" >> $(HOST_BUILD_DIR)/$(PKG_NAME)
	echo kernel_version = \"#0 $(shell date --date=@$(SOURCE_DATE_EPOCH))\" >> $(HOST_BUILD_DIR)/$(PKG_NAME)
	cat src/footer.py.inc >> $(HOST_BUILD_DIR)/$(PKG_NAME)
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOSTPKG)/lib/$(PKG_NAME)
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/$(PKG_NAME) $(STAGING_DIR_HOSTPKG)/lib/$(PKG_NAME)/uname
endef

define Host/Clean
	rm -rf "$(STAGING_DIR_HOSTPKG)/lib/$(PKG_NAME)"
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,fakeuname))
