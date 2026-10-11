include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rt63368-pcie
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Aleksandr Vlasov <vlasov@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rt63368-pcie
  SUBMENU:=Other modules
  TITLE:=RT63368 PCIe host bridge
  DEPENDS:=@TARGET_econet_en751221
  FILES:=$(PKG_BUILD_DIR)/pcie-rt63368.ko
  AUTOLOAD:=$(call AutoLoad,20,pcie-rt63368,1)
endef

define KernelPackage/rt63368-pcie/description
  PCIe host bridge driver for the RT63365/RT63368 (ZyXEL Keenetic LTE).
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(KERNEL_MAKE) M=$(PKG_BUILD_DIR) modules
endef

$(eval $(call KernelPackage,rt63368-pcie))
