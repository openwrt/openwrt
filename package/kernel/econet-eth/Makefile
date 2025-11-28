include $(TOPDIR)/rules.mk

PKG_NAME:=econet-eth
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/cjdelisle/econet_eth.git
PKG_MIRROR_HASH:=c31662d164826012a96269573dde83d4048b84a7191b6a1f56d9c9ed3956675c
PKG_SOURCE_DATE:=2025-11-07
PKG_SOURCE_VERSION:=40aac736a46fec0d96beadd7513c53d1f0459737

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

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

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,econet-eth))
