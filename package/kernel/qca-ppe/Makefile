include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ppe
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/qca-ppe.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2026-03-14
PKG_SOURCE_VERSION:=0fb5022ce41521b9c2dce68e8c8d446686c4c2a2
PKG_MIRROR_HASH:=e0f532d2db923e564b68aaab13638d7994b9ae9f5ab7584bf6bb02ec796035a1

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-ppe
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_qualcommax +kmod-qca-uniphy
  TITLE:=Qualcomm PPE switch driver
  FILES:=$(PKG_BUILD_DIR)/qca_ppe.ko
  AUTOLOAD:=$(call AutoLoad,32,qca_ppe)
endef

define KernelPackage/qca-ppe/Description
  Driver for Qualcomm PPE (Packet Processing Engine) switches.
endef

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) \
		-C "$(LINUX_DIR)" \
		M="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,qca-ppe))
