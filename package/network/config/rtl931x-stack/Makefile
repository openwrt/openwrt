# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl931x-stack
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/rtl931x-stack
  SECTION:=net
  CATEGORY:=Network
  TITLE:=RTL931x switch stacking bring-up utility
  DEPENDS:=@(TARGET_realtek_rtl931x||TARGET_realtek_rtl931x_nand) +libnl-tiny
endef

define Package/rtl931x-stack/description
 A standalone generic-netlink client for bringing up and inspecting the
 experimental RTL931x two-member switch stack.
endef

TARGET_CPPFLAGS += \
	-D_GNU_SOURCE \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(LINUX_DIR)/user_headers/include

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/rtl931x-stack.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/rtl931x-stack \
		$(PKG_BUILD_DIR)/rtl931x-stack.c \
		$(TARGET_LDFLAGS) -lnl-tiny
endef

define Package/rtl931x-stack/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtl931x-stack $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rtl931x-stack))
