include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=xs2184
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/xs2184
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Chipup xs2184 PSE support
  DEPENDS:=+kmod-i2c-gpio
  FILES:=$(PKG_BUILD_DIR)/xs2184.ko
  AUTOLOAD:=$(call AutoProbe,xs2184)
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,xs2184))
