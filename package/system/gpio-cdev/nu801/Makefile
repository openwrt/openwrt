# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=nu801
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/chunkeey/nu801.git
PKG_SOURCE_VERSION:=d9942c0ceb949080b93366a9431028de3608e535
PKG_MIRROR_HASH:=859be7dec96d2a0d6ee8b80c6f1a703384940d19caeeb74a4ac0a961b2a985db
PKG_MAINTAINER:=Christian Lamparter <chunkeey@gmail.com>
PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/nu801
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Userspace GPIO Drivers
  DEPENDS:=@TARGET_x86
  KCONFIG:=CONFIG_GPIO_CDEV=y
  TITLE:=NU801 LED Driver
endef

define Package/nu801/description
This package contains a userspace driver to power the NUMEN Tech. NU801 LED Driver.
endef

define Package/nu801/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nu801 $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nu801.init $(1)/etc/init.d/nu801
endef

$(eval $(call BuildPackage,nu801))
