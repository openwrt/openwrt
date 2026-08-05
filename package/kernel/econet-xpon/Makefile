include $(TOPDIR)/rules.mk

PKG_NAME:=econet-xpon
PKG_RELEASE:=1

PKG_MAINTAINER:=AK Sharma <maskachoska@yahoo.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/econet-xpon
  SUBMENU:=Network Devices
  TITLE:=EcoNet EN7528 xPON (GPON) driver
  DEPENDS:=@TARGET_econet +kmod-econet-eth
  FILES:=$(PKG_BUILD_DIR)/econet-xpon.ko
  # No AutoLoad: the driver must not be loaded without an ONU identity. A bare
  # kmodloader autoload comes up with the MediaTek default serial
  # (MTKG/0x00000001); the OLT ranges it to O5 then drops it to O3 with no data
  # service. The econet-xpon-config package ships the init.d loader that loads
  # the module with the gpon_sn / gpon_pw / wan_mac from UCI (config econet-xpon).
endef

define KernelPackage/econet-xpon/description
  GPON xPON MAC + optical PHY driver for EcoNet/Airoha EN7528, ported from the
  vendor tclinux_phoenix GPL source (Linux 2.6.36) to kernel 6.18. The ONU
  serial, PLOAM password and WAN MAC are supplied at runtime via UCI (see the
  econet-xpon-config package); nothing is hardcoded.
endef

# The vendor sources live under src/; copy them into the build dir.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,econet-xpon))
