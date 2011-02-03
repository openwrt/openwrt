#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=u-boot

PKG_VERSION:=2010.03
PKG_MD5SUM:=2bf5ebf497dddc52440b1ea386cc1332
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/.configured
  STAMP_BUILT:=$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/.built
endif

define Package/uboot-lantiq-template
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_lantiq
  URL:=http://www.denx.de/wiki/U-Boot
  VARIANT:=$(1)
  TITLE:=$(1) ($(2))
  MAINTAINER:=John Crispin <blogic@openwrt.org>
endef

#Lantiq
Package/uboot-lantiq-easy50712_DDR166M=$(call Package/uboot-lantiq-template,easy50712_DDR166M,NOR)
Package/uboot-lantiq-easy50712_DDR166M_ramboot=$(call Package/uboot-lantiq-template,easy50712_DDR166M_ramboot,RAM)
Package/uboot-lantiq-easy50812_DDR166M=$(call Package/uboot-lantiq-template,easy50812_DDR166M,NOR)
Package/uboot-lantiq-easy50812_DDR166M_ramboot=$(call Package/uboot-lantiq-template,easy50812_DDR166M_ramboot,RAM)

DDR_CONFIG_easy50712_DDR166M_ramboot:=easy50712_DDR166M
DDR_CONFIG_easy50812_DDR166M_ramboot:=easy50812

#Arcadyan
Package/uboot-lantiq-arv4518PW=$(call Package/uboot-lantiq-template,arv4518PW_flash,NOR)
Package/uboot-lantiq-arv4518PW_ramboot=$(call Package/uboot-lantiq-template,arv4518PW_ramboot,RAM)
Package/uboot-lantiq-arv4518PW_brnboot=$(call Package/uboot-lantiq-template,arv4518PW_brnboot,BRN)
Package/uboot-lantiq-arv452CPW=$(call Package/uboot-lantiq-template,arv452CPW_flash,NOR)
Package/uboot-lantiq-arv452CPW_ramboot=$(call Package/uboot-lantiq-template,arv452CPW_ramboot,RAM)
Package/uboot-lantiq-arv452CPW_brnboot=$(call Package/uboot-lantiq-template,arv452CPW_brnboot,BRN)
Package/uboot-lantiq-arv752DPW=$(call Package/uboot-lantiq-template,arv752DPW_flash,NOR)
Package/uboot-lantiq-arv752DPW_ramboot=$(call Package/uboot-lantiq-template,arv752DPW_ramboot,RAM)
Package/uboot-lantiq-arv752DPW_brnboot=$(call Package/uboot-lantiq-template,arv752DPW_brnboot,BRN)
Package/uboot-lantiq-arv752DPW22=$(call Package/uboot-lantiq-template,arv752DPW22_flash,NOR)
Package/uboot-lantiq-arv752DPW22_ramboot=$(call Package/uboot-lantiq-template,arv752DPW22_ramboot,RAM)
Package/uboot-lantiq-arv752DPW22_brnboot=$(call Package/uboot-lantiq-template,arv752DPW22_brnboot,BRN)

DDR_CONFIG_arv4518_ramboot:=arcadyan_psc166
DDR_CONFIG_arv452C_ramboot:=arcadyan_psc166
DDR_CONFIG_arv752DPW_ramboot:=arcadyan_psc166
DDR_CONFIG_arv752DPW22_ramboot:=arcadyan_psc166

define Build/Prepare
	$(PKG_UNPACK)
	cp -r $(CP_OPTS) $(FILES_DIR)/* $(PKG_BUILD_DIR)/
	$(Build/Patch)
	find $(PKG_BUILD_DIR) -name .svn | $(XARGS) rm -rf
endef

UBOOT_MAKE_OPTS:= \
	CROSS_COMPILE=$(TARGET_CROSS) \
	ENDIANNESS= \
	V=1

define Build/Configure/Target
	$(MAKE) -s -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		O=$(PKG_BUILD_DIR)/$(BUILD_VARIANT) \
		$(1)_config
endef

define Build/Configure
	$(call Build/Configure/Target,$(BUILD_VARIANT))
endef

define Build/Compile/Target
	$(MAKE) -s -C $(PKG_BUILD_DIR) \
		$(UBOOT_MAKE_OPTS) \
		O=$(PKG_BUILD_DIR)/$(1) \
		all
endef

define Build/Compile
	$(call Build/Compile/Target,$(BUILD_VARIANT))
endef

define Package/uboot-lantiq-$(BUILD_VARIANT)/install
	mkdir -p $(1)
	dd \
		if=$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/u-boot.bin \
		of=$(1)/u-boot.bin \
		bs=64k conv=sync
ifneq ($(findstring ramboot,$(BUILD_VARIANT)),)
	if [ -e $(DDR_CONFIG_$(BUILD_VARIANT)).conf ]; then \
		perl ./gct \
			$(DDR_CONFIG_$(BUILD_VARIANT)).conf \
			$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/u-boot.srec \
			$(1)/u-boot.asc; \
	fi
endif
endef

$(eval $(call BuildPackage,uboot-lantiq-easy50712_DDR166M))
$(eval $(call BuildPackage,uboot-lantiq-easy50712_DDR166M_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-easy50812_DDR166M))
$(eval $(call BuildPackage,uboot-lantiq-easy50812_DDR166M_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22_ramboot))

