# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=rtl931x-stack
PKG_RELEASE:=3
PKG_LICENSE:=GPL-2.0-only
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/rtl931x-stack
  SECTION:=net
  CATEGORY:=Network
  TITLE:=RTL931x switch stacking bring-up utility
  DEPENDS:=@(TARGET_realtek_rtl931x||TARGET_realtek_rtl931x_nand) \
	+libnl-tiny +libubox +libubus +libuci +ubus
endef

define Package/rtl931x-stack/description
 An early-boot manager and diagnostic client for bringing up and inspecting
 the experimental RTL931x two-member switch stack.
endef

TARGET_CPPFLAGS += \
	-D_GNU_SOURCE \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(LINUX_DIR)/user_headers/include

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/*.c ./src/*.h $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/rtl931x-stack \
		$(PKG_BUILD_DIR)/stack-cli.c \
		$(PKG_BUILD_DIR)/stack-netlink.c \
		$(TARGET_LDFLAGS) -lnl-tiny
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/rtl931x-stackd \
		$(PKG_BUILD_DIR)/rtl931x-stackd.c \
		$(PKG_BUILD_DIR)/stack-netlink.c \
		$(TARGET_LDFLAGS) -lnl-tiny -lubus -lubox -luci
endef

define Package/rtl931x-stack/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d \
		$(1)/lib/netifd/config-selectors.d $(1)/usr/sbin
	$(INSTALL_CONF) ./files/rtl931x-stack.config \
		$(1)/etc/config/rtl931x-stack
	$(INSTALL_BIN) ./files/rtl931x-stack.init \
		$(1)/etc/init.d/rtl931x-stack
	$(INSTALL_BIN) ./files/rtl931x-stack.selector \
		$(1)/lib/netifd/config-selectors.d/10-rtl931x-stack
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtl931x-stack $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rtl931x-stackd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rtl931x-stack))
