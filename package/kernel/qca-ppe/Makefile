include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ppe
PKG_RELEASE:=1

PKG_SOURCE_URL:=https://github.com/openwrt/qca-ppe.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2026-05-16
PKG_SOURCE_VERSION:=acf30c0fcb04a240d427a26c5efad55622a61652
PKG_MIRROR_HASH:=33e937cdaa5fec260a7dc9b46be21825f3f18004b660a34daa7003e1f4bedca5

PKG_BUILD_PARALLEL:=1
PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-ppe
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_qualcommax +kmod-qca-uniphy +kmod-phylink
  KCONFIG:=CONFIG_FWNODE_PCS=y
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
