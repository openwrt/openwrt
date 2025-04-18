include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=switch-rtl8373
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/switch-rtl8373
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Realtek RTL8373 switch support
  DEPENDS:=+kmod-i2c-gpio
  FILES:= $(PKG_BUILD_DIR)/rtl8373.ko
  AUTOLOAD:=$(call AutoProbe,rtl8373)
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Compile
endef

$(eval $(call KernelPackage,switch-rtl8373))
