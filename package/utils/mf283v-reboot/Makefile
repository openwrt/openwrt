# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mf283v-reboot
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Dmitry Sinyavin <sinyavin@gmail.com>
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/mf283v-reboot
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ZTE MF283V reboot-mode helper
  DEPENDS:=@TARGET_mdm9607
endef

define Package/mf283v-reboot/description
 Invoke the Linux RESTART2 reboot syscall with a named MF283V boot mode.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/mf283v-reboot.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-Wall -Wextra -Werror -o $(PKG_BUILD_DIR)/mf283v-reboot \
		$(PKG_BUILD_DIR)/mf283v-reboot.c
endef

define Package/mf283v-reboot/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mf283v-reboot $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,mf283v-reboot))
