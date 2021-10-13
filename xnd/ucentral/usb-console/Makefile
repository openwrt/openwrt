include $(TOPDIR)/rules.mk

PKG_NAME:=usb-console
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/usb-console
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=usb-console
  DEPENDS:= +kmod-usb-serial +kmod-usb-serial-cp210x +kmod-usb-serial-ftdi
endef

define Package/usb-console/description
 Allow spawning a console on a usbserial dongle.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/usb-console/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,usb-console))
