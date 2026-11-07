# SPDX-License-Identifier: GPL-2.0

include $(TOPDIR)/rules.mk

PKG_NAME:=an7552-gpon
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Lukasz M <lukasz1992m+openwrt@gmail.com>

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/an7552-gpon
  SUBMENU:=Network Devices
  TITLE:=Airoha AN7552/AN7563 xPON (MAC and SerDes PHY)
  DEPENDS:=@TARGET_airoha_an7563
  FILES:=$(PKG_BUILD_DIR)/an7552-gpon.ko
  # Loaded before en7571: the optical front end registers itself here, so
  # the core has to be resident for xpon_register_device() to resolve.
  AUTOLOAD:=$(call AutoLoad,10,an7552-gpon)
endef

define KernelPackage/an7552-gpon/description
 One driver for the AN7552/AN7563 xPON block (DT compatible
 "airoha,an7552-gpon"), both halves of it: the GPON MAC - activation
 O1-O7, PLOAM, T-CONT and GEM port tables, the "pon" network interface,
 OMCI over generic netlink, hardware classification, QoS and AES offload -
 and the PON SerDes PHY with the mode sequence, its interrupt, the burst
 overhead settings and the optical front end line.  Frames are carried by
 the WAN QDMA of airoha-eth; the optical front end is the en7571 driver,
 whose transmitter line is switched through the regulator it registers.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/. $(PKG_BUILD_DIR)/
endef

# kernel.mk already passes KBUILD_EXTRA_SYMBOLS pointing at
# $(KERNEL_BUILD_DIR)/symvers/*.symvers so modpost knows about the exports
# of the drivers this module calls into.
define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		M="$(PKG_BUILD_DIR)" modules
endef

# generic netlink ABI for userspace (gpond), and the optical front end
# interface for the en7571 driver
define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/linux
	$(CP) $(PKG_BUILD_DIR)/uapi/gpon_genl.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/uapi/linux/pon_laser.h \
	      $(STAGING_DIR)/usr/include/linux/
endef

$(eval $(call KernelPackage,an7552-gpon))
