include $(TOPDIR)/rules.mk

PKG_NAME:=r8152
PKG_VERSION:=2.22.1
PKG_RELEASE:=1

PKG_BUILD_PARALLEL:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Rany Hany <rany_hany@riseup.net>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

PKG_TARBALL:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz

define KernelPackage/r8152
  SUBMENU:=Network Devices
  TITLE:=Realtek RTL8152/RTL8153/RTL8156 USB Ethernet driver
  DEPENDS:=@USB_SUPPORT +kmod-usb-core +kmod-libphy
  CONFLICTS:=kmod-usb-net-rtl8152
  FILES:=$(PKG_BUILD_DIR)/r8152.ko
  AUTOLOAD:=$(call AutoProbe,r8152)
endef

define KernelPackage/r8152/description
 Realtek vendor driver for RTL8152/RTL8153/RTL8156
 based USB Ethernet adapters.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xzf $(CURDIR)/$(PKG_TARBALL)
	$(Build/Patch)
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		M="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,r8152))
