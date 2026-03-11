include $(TOPDIR)/rules.mk

PKG_NAME:=qca-uniphy
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/qca-uniphy.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2026-03-15
PKG_SOURCE_VERSION:=449ea545cf0ae7ccecd4603c06e5faa20339fd6d
PKG_MIRROR_HASH:=17660351977a9e57835307aaa6474801ebe4729f11220149a6cc23f82c4a1afe

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-uniphy
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_qualcommax
  TITLE:=Qualcomm IPQ6018 UNIPHY PCS driver
  FILES:=$(PKG_BUILD_DIR)/qca_uniphy.ko
  AUTOLOAD:=$(call AutoLoad,31,qca_uniphy)
endef

define KernelPackage/qca-uniphy/Description
  PCS driver for the UNIPHY SerDes blocks in Qualcomm IPQ6018 SoCs.
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
