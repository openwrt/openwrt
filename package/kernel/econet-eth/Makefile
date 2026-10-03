include $(TOPDIR)/rules.mk

PKG_NAME:=econet-eth
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/cjdelisle/econet_eth.git
PKG_SOURCE_DATE:=2026-09-22
PKG_MIRROR_HASH:=040b717d8e096278c2eeac1519ef3fb988a19fb2e317127a273d938f6e5fcf36
PKG_SOURCE_VERSION:=df20a0548c9142ef80ee613a81da90885b57e6c5

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
