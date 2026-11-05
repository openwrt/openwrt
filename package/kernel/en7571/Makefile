include $(TOPDIR)/rules.mk

PKG_NAME:=en7571
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Lukasz M <lukasz1992m+openwrt@gmail.com>

# The PON core is what this driver registers with (xpon_register_device() and
# the symbols it calls back), so it has to be built first: the exported symbols
# modpost resolves against come from that module's symvers, and <linux/pon_laser.h>
# from its Build/InstallDev.
PKG_BUILD_DEPENDS:=an7552-gpon

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/en7571
  SUBMENU:=Other modules
  TITLE:=EN7571 GPON optical transceiver (laser)
  DEPENDS:=@TARGET_airoha_an7563 +kmod-hwmon-core +kmod-an7552-gpon
  FILES:=$(PKG_BUILD_DIR)/en7571.ko
  # Loaded after an7552-gpon, which this driver registers with; see there.
  AUTOLOAD:=$(call AutoLoad,30,en7571)
endef

define KernelPackage/en7571/description
 I2C driver for the Airoha EN7571 GPON optical transceiver (DT compatible
 "airoha,en7571", usually on i2c @0x70).  Applies the on-module calibration
 matrix at probe (flash magic, T0C/T1C, laser bias/modulation, APD, LOS/SD),
 registers the transmitter line as the "en7571-laser" regulator, and exports
 DDMI through hwmon (Vcc, temperature, TX bias/power, RX power); debugfs
 en7571-<dev>/a2 shows them as an SFF-8472 A2h page.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		STAGING_INC="$(STAGING_DIR)/usr/include" \
		M="$(PKG_BUILD_DIR)" modules
endef

# <linux/pon_laser.h> is staged by an7552-gpon (uapi/linux/), so this package
# has to be built after it; PKG_BUILD_DEPENDS above orders that, and the
# symbols xpon_register_device() and friends resolve against come from the
# same module.

$(eval $(call KernelPackage,en7571))
