# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=sandbox-test
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/sandbox-test
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Build sandbox escape test
  DEPENDS:=@DEVEL @BUILD_SANDBOX
endef

define Package/sandbox-test/description
  Test package that attempts to escape the build sandbox from each of
  Build/Configure, Build/Compile and Build/Install. Every write/network
  attempt should fail when CONFIG_BUILD_SANDBOX=y.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/sandbox-test.mk $(PKG_BUILD_DIR)/
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) -f sandbox-test.mk configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) -f sandbox-test.mk all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) -f sandbox-test.mk install
endef

define Package/sandbox-test/install
	:
endef

$(eval $(call BuildPackage,sandbox-test))
