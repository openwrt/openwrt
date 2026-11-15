include $(TOPDIR)/rules.mk

PKG_NAME:=econet-eth
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/cjdelisle/econet_eth.git
PKG_SOURCE_DATE:=2026-11-17
PKG_MIRROR_HASH:=294056b8edf5f722daa0afa181523763e952561cfc5e02141c6c7748732a4499
PKG_SOURCE_VERSION:=d08094fbecfc7438fb371ee62c34faccfac17a03

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/econet-eth-mt7530
  SUBMENU:=Network Devices
  TITLE:=EcoNet EN751221 Ethernet Driver + MT7530 Switch
  DEPENDS:=@TARGET_econet_en751221 +kmod-econet-eth
  FILES:= $(PKG_BUILD_DIR)/mt7530.ko \
    $(PKG_BUILD_DIR)/mt7530-mmio.ko \
    $(PKG_BUILD_DIR)/mt7530-mdio.ko
  AUTOLOAD:=$(call AutoLoad,90,mt7530 mt7530-mdio mt7530-mmio)
endef

define KernelPackage/econet-eth
  SUBMENU:=Network Devices
  TITLE:=EcoNet EN751221 Ethernet Driver
  DEPENDS:=@TARGET_econet
  FILES:=$(PKG_BUILD_DIR)/econet-eth.ko
  AUTOLOAD:=$(call AutoLoad,90,econet-eth)
endef

define KernelPackage/econet-eth/description
  Out-of-tree ethernet driver for EcoNet EN751221 devices.
endef

define KernelPackage/econet-eth-mt7530/description
  Out-of-tree driver for EcoNet EN751221 MT7530 switch.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,econet-eth))
$(eval $(call KernelPackage,econet-eth-mt7530))
