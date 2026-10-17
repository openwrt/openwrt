include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt63368-eth
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Aleksandr Vlasov <vlasov@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt63368-eth
  SUBMENU:=Network Devices
  TITLE:=RT63368 Frame Engine (PDMA) Ethernet driver
  DEPENDS:=@TARGET_econet_en751221 +kmod-mdio-devres
  FILES:=$(PKG_BUILD_DIR)/rt63368-eth.ko
  AUTOLOAD:=$(call AutoLoad,30,rt63368-eth,1)
endef

define KernelPackage/rt63368-eth/description
  Ethernet driver for the RT63365/RT63368 Frame Engine PDMA, as used on the
  ZyXEL Keenetic LTE (DSA conduit). Relies on the bootloader's FE/GSW
  initialisation.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,rt63368-eth))
