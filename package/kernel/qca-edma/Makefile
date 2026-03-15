include $(TOPDIR)/rules.mk

PKG_NAME:=qca-edma
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/qca-edma.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2026-03-14
PKG_SOURCE_VERSION:=1da9c61d0905352e1d80445d0ae725b211b79b69
PKG_MIRROR_HASH:=4065153cdb025183bf3ea7051fc24a41897267f88751d802d688a6808a69227c

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-edma
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_qualcommax +kmod-qca-ppe +kmod-qca-uniphy
  TITLE:=Qualcom NSS dataplane ethernet driver
  FILES:=$(PKG_BUILD_DIR)/qca_edma.ko
  AUTOLOAD:=$(call AutoLoad,31,qca_edma)
endef

define KernelPackage/qca-edma/Description
  NSS dataplane ethernet driver for Qualcom SoCs.
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		-C "$(LINUX_DIR)" \
		M="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,qca-edma))
