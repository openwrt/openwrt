# $Id: Makefile 2767 2005-12-25 02:10:14Z wbx $

include $(TOPDIR)/rules.mk
include $(TOPDIR)/package/kernel.mk

PKG_NAME:=ueagle-atm
PKG_VERSION:=1.0d1
PKG_RELEASE:=1
PKG_MD5SUM:=5da1fceb15168f4c7ad407a3e38ddecb

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://castet.matthieu.free.fr/eagle/release/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

#FIXME: add dependency on kmod-usb-core
define Package/kmod-ueagle-atm
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=@LINUX_2_6_BRCM @PACKAGE_KMOD_USB
TITLE:=Driver for Eagle 8051 based USB ADSL modems
DESCRIPTION:=Driver for Eagle 8051 based USB ADSL modems
URL:=http://www.eagle-usb.org/
VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TARGET="$(HAL_TARGET)" \
		KERNELDIR="$(LINUX_DIR)" \
		all
endef

define Package/kmod-ueagle-atm/install
	install -d -m0755 $(1)/lib/modules/$(LINUX_VERSION)
	$(CP)	$(PKG_BUILD_DIR)/driver/ueagle-atm.$(LINUX_KMOD_SUFFIX) \
		$(PKG_BUILD_DIR)/usbatm_iso/usbatm.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)/
	install -d -m0755 $(1)/etc/modules.d
	install -m0644 ./files/$(PKG_NAME).modules \
		$(1)/etc/modules.d/10-$(PKG_NAME)
endef

$(eval $(call BuildPackage,kmod-ueagle-atm))

