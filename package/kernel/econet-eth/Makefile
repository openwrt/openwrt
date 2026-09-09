include $(TOPDIR)/rules.mk

PKG_NAME:=econet-eth
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/cjdelisle/econet_eth.git
PKG_SOURCE_DATE:=2026-10-21
PKG_MIRROR_HASH:=2b159ad96a84dd454cfccbaf6cd93466611f7036ff0377335a87998195018b73
PKG_SOURCE_VERSION:=3a8bcd2e639872368f103fc6423d6eb6c3245684

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/econet-eth
  SUBMENU:=Network Devices
  TITLE:=EcoNet EN751221 Ethernet Driver
  DEPENDS:=@TARGET_econet
  FILES:=$(PKG_BUILD_DIR)/econet-eth.ko \
    $(PKG_BUILD_DIR)/mt7530.ko \
    $(PKG_BUILD_DIR)/mt7530-mmio.ko \
    $(PKG_BUILD_DIR)/mt7530-mdio.ko
  AUTOLOAD:=$(call AutoLoad,90,econet-eth mt7530 mt7530-mdio mt7530-mmio)
endef

define KernelPackage/econet-eth/description
  Out-of-tree ethernet driver for EcoNet EN751221 devices.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,econet-eth))
