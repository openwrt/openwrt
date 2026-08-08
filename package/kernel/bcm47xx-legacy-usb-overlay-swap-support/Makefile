# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bcm47xx-legacy-usb-overlay-swap-support
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/bcm47xx-legacy-usb-overlay-swap-support
  SECTION:=kernel
  CATEGORY:=Kernel
  TITLE:=BCM47xx legacy USB overlay/swap support
  DEPENDS:=@TARGET_bcm47xx_legacy +block-mount +kmod-fs-ext4 +kmod-usb-storage
  HIDDEN:=1
  KCONFIG:=CONFIG_SWAP=y
endef

define Package/bcm47xx-legacy-usb-overlay-swap-support/description
 USB overlay/swap boot policy, kernel swap support and USB swap shutdown guard
 for BCM47xx legacy USB images.
endef

define Build/Compile
endef

define Package/bcm47xx-legacy-usb-overlay-swap-support/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/etc/rc.d
	$(INSTALL_CONF) ./files/fstab $(1)/etc/config/fstab
	$(INSTALL_BIN) ./files/usb-swap-shutdown-safe.init $(1)/etc/init.d/usb-swap-shutdown-safe
	$(INSTALL_BIN) ./files/K89usb-swap-shutdown-safe $(1)/etc/rc.d/K89usb-swap-shutdown-safe
endef

$(eval $(call BuildPackage,bcm47xx-legacy-usb-overlay-swap-support))
