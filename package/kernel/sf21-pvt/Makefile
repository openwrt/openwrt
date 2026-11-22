#
# Siflower SF21 PVT (process/voltage/temperature) monitor (from the Siflower BSP)
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sf21-pvt
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Mateusz Trzesiński <matjss3346@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/hwmon-sf21-pvt
  SUBMENU:=Hardware Monitoring Support
  TITLE:=Siflower SF21 temperature and voltage monitor
  DEPENDS:=@TARGET_siflower_sf21 +kmod-hwmon-core
  FILES:=$(PKG_BUILD_DIR)/sf_pvt.ko
  AUTOLOAD:=$(call AutoProbe,sf_pvt)
endef

define KernelPackage/hwmon-sf21-pvt/description
 hwmon driver for the on-chip temperature sensor and voltage monitor of the
 Siflower SF21A6826/SF21H8898.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,hwmon-sf21-pvt))
