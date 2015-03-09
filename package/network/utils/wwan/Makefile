include $(TOPDIR)/rules.mk

PKG_NAME:=wwan
PKG_VERSION:=2014-07-17
PKG_RELEASE=1

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/wwan
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Generic OpenWrt 3G/4G proto handler
endef

define Build/Compile
	true
endef

define Package/wwan/install
	$(INSTALL_DIR) $(1)/lib/netifd/proto/
	$(CP) ./files/wwan.sh $(1)/lib/netifd/proto/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_BIN) ./files/wwan.usb $(1)/etc/hotplug.d/usb/00_wwan.sh
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usbmisc
	$(INSTALL_BIN) ./files/wwan.usbmisc $(1)/etc/hotplug.d/usbmisc/00_wwan.sh
	$(INSTALL_DIR) $(1)/lib/network/wwan/
	$(INSTALL_DATA) ./files/data/* $(1)/lib/network/wwan/
endef

$(eval $(call BuildPackage,wwan))
