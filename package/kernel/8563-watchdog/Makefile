include $(TOPDIR)/rules.mk

PKG_NAME:=8563-watchdog
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/8563-watchdog
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=8563 watchdog
  URL:=
endef

define Package/8563-watchdog/description
 This package contains the hw watchdog script for PCF8563 timer interrupt.
endef

define Build/Compile
endef

define Package/8563-watchdog/install
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_BIN) ./files/8563-watchdog.init $(1)/etc/init.d/8563-watchdog
	$(INSTALL_BIN) ./files/8563-watchdog $(1)/sbin/8563-watchdog
endef

$(eval $(call BuildPackage,8563-watchdog))
