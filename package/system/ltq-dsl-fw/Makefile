#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ltq-dsl-fw
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources/
PKG_MD5SUM:=4700a36b66b955b4c5544227267356f4
PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk

define Package/kmod-ltq-dsl-firmware-template
  TITLE+=Firmware Annex-$(1) $(2)
  SECTION:=sys
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  VARIANT:= $(1)-$(2)
  URL:=http://www.lantiq.com/
  DEPENDS:=@(TARGET_lantiq_$(2)||TARGET_lantiq_$(3)) kmod-ltq-dsl-$(2)
endef

Package/kmod-ltq-dsl-firmware-a-danube=$(call Package/kmod-ltq-dsl-firmware-template,a,danube,xway)
Package/kmod-ltq-dsl-firmware-b-danube=$(call Package/kmod-ltq-dsl-firmware-template,b,danube,xway)
Package/kmod-ltq-dsl-firmware-a-ar9=$(call Package/kmod-ltq-dsl-firmware-template,a,ar9,xway)
Package/kmod-ltq-dsl-firmware-b-ar9=$(call Package/kmod-ltq-dsl-firmware-template,b,ar9,xway)
Package/kmod-ltq-dsl-firmware-a-ase=$(call Package/kmod-ltq-dsl-firmware-template,a,ase,ase)
Package/kmod-ltq-dsl-firmware-b-ase=$(call Package/kmod-ltq-dsl-firmware-template,b,ase,ase)

define Build/Compile
	echo
endef

define Package/kmod-ltq-dsl-firmware-$(BUILD_VARIANT)/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) $(PKG_BUILD_DIR)/$(FW_NAME)/ltq-dsl-fw-$(BUILD_VARIANT).bin $(1)/lib/firmware/
	ln -s /lib/firmware/$(FW_NAME)/ltq-dsl-fw-$(BUILD_VARIANT).bin $(1)/lib/firmware/ModemHWE.bin
endef

$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-danube))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-danube))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-ase))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-ase))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-a-ar9))
$(eval $(call BuildPackage,kmod-ltq-dsl-firmware-b-ar9))
