include $(TOPDIR)/rules.mk

PKG_NAME:=qca-uniphy
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/qca-uniphy.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2026-05-16
PKG_SOURCE_VERSION:=214a3c0d293a763400fd6853e000a3e09f0c7751
PKG_MIRROR_HASH:=6993cd43dd6509576d9f755ac4d77dc34188631198c1431076352397040a84b0

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-uniphy
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Qualcomm 802.11ax UNIPHY PCS driver
  DEPENDS:=@TARGET_qualcommax
  KCONFIG:=CONFIG_FWNODE_PCS=y
  FILES:=$(PKG_BUILD_DIR)/qca_uniphy.ko
  AUTOLOAD:=$(call AutoLoad,31,qca_uniphy)
endef

define KernelPackage/qca-uniphy/Description
  PCS driver for the UNIPHY SerDes blocks in Qualcomm 802.11ax SoCs.
  Provides PSGMII and SGMII interface modes and registers the UNIPHY
  PLL clock providers needed for port clock muxing.
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		-C "$(LINUX_DIR)" \
		M="$(PKG_BUILD_DIR)" \
		modules
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/qca_uniphy.h $(1)/usr/include/
endef

$(eval $(call KernelPackage,qca-uniphy))
