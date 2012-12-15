# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-hcd
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/ltq-hcd-$(BUILD_VARIANT)

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-hcd-template
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=USB Support
  TITLE:=USB driver for $(1)
  URL:=http://www.lantiq.com/
  VARIANT:=$(1)
  DEPENDS:=@TARGET_lantiq_$(2) +kmod-usb-core
  FILES:=$(PKG_BUILD_DIR)/ltq_hcd_$(1).ko
  AUTOLOAD:=$(call AutoLoad,50,ltq_hcd_$(1))
endef

KernelPackage/ltq-hcd-ase=$(call KernelPackage/ltq-hcd-template,ase,ase)
KernelPackage/ltq-hcd-danube=$(call KernelPackage/ltq-hcd-template,danube,xway)
KernelPackage/ltq-hcd-ar9=$(call KernelPackage/ltq-hcd-template,ar9,xway)
KernelPackage/ltq-hcd-vr9=$(call KernelPackage/ltq-hcd-template,vr9,xway)

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	cd $(LINUX_DIR); \
		ARCH=mips CROSS_COMPILE="$(KERNEL_CROSS)" \
		$(MAKE) BUILD_VARIANT=$(BUILD_VARIANT) M=$(PKG_BUILD_DIR) V=1 modules
endef

$(eval $(call KernelPackage,ltq-hcd-ase))
$(eval $(call KernelPackage,ltq-hcd-danube))
$(eval $(call KernelPackage,ltq-hcd-ar9))
$(eval $(call KernelPackage,ltq-hcd-vr9))
