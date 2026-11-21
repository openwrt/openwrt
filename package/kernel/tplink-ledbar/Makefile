#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=tplink-ledbar
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Patrick Lawler <patricktlawler@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/leds-tplink-ledbar
  SUBMENU:=LED modules
  TITLE:=TP-Link Archer BE550 LED bar support
  FILES:=$(PKG_BUILD_DIR)/leds-tplink-ledbar.ko
  AUTOLOAD:=$(call AutoProbe,leds-tplink-ledbar,1)
  DEPENDS:=@TARGET_qualcommbe +kmod-i2c-core
endef

define KernelPackage/leds-tplink-ledbar/description
  Driver for the front LED bar microcontroller of the TP-Link Archer BE550.
endef

define Package/tplink-ledbar
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=TP-Link Archer BE550 LED bar service
  DEPENDS:=@TARGET_qualcommbe +kmod-leds-tplink-ledbar
  PKGARCH:=all
endef

define Package/tplink-ledbar/description
  Shows the router's state on the TP-Link Archer BE550 front LED bar
  the way the stock firmware does.
endef

define Package/tplink-ledbar/conffiles
/etc/config/ledbar
/etc/ledbar/choose.sh
endef

define Package/tplink-ledbar/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/etc/hotplug.d/iface \
		$(1)/etc/rc.button $(1)/etc/ledbar $(1)/usr/libexec
	$(INSTALL_CONF) ./files/ledbar.config $(1)/etc/config/ledbar
	$(INSTALL_BIN) ./files/ledbar.init $(1)/etc/init.d/ledbar
	$(INSTALL_DATA) ./files/ledbar.hotplug $(1)/etc/hotplug.d/iface/50-ledbar
	$(INSTALL_BIN) ./files/lights_toggle $(1)/etc/rc.button/lights_toggle
	$(INSTALL_BIN) ./files/ledbar.sh $(1)/usr/libexec/ledbar
	$(INSTALL_DATA) ./files/choose.sh $(1)/etc/ledbar/choose.sh
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,leds-tplink-ledbar))
$(eval $(call BuildPackage,tplink-ledbar))
