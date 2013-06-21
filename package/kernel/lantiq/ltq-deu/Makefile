# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-deu
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/ltq-deu-$(BUILD_VARIANT)

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ltq-deu-template
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Cryptographic API modules
  TITLE:=deu driver for $(1)
  URL:=http://www.lantiq.com/
  VARIANT:=$(1)
  DEPENDS:=@TARGET_lantiq_xway
  FILES:=$(PKG_BUILD_DIR)/ltq_deu_$(1).ko $(PKG_BUILD_DIR)/ltq_deu_testmgr.ko
  AUTOLOAD:=$(call AutoLoad,52,ltq_deu_$(1))
endef

KernelPackage/ltq-deu-danube=$(call KernelPackage/ltq-deu-template,danube)
KernelPackage/ltq-deu-ar9=$(call KernelPackage/ltq-deu-template,ar9)
KernelPackage/ltq-deu-vr9=$(call KernelPackage/ltq-deu-template,vr9)

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

$(eval $(call KernelPackage,ltq-deu-danube))
$(eval $(call KernelPackage,ltq-deu-ar9))
$(eval $(call KernelPackage,ltq-deu-vr9))
