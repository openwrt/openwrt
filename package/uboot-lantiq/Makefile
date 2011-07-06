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

#Siemens
Package/uboot-lantiq-gigaSX76X_DDRsamsung166=$(call Package/uboot-lantiq-template,gigaSX76X_DDRsamsung166,NOR)

#Arcadyan
Package/uboot-lantiq-arv3527P_flash=$(call Package/uboot-lantiq-template,arv3527P_flash,NOR)
Package/uboot-lantiq-arv3527P_ramboot=$(call Package/uboot-lantiq-template,arv3527P_ramboot,RAM)
Package/uboot-lantiq-arv3527P_brnboot=$(call Package/uboot-lantiq-template,arv3527P_brnboot,BRN)
Package/uboot-lantiq-arv4518PW_flash=$(call Package/uboot-lantiq-template,arv4518PW_flash,NOR)
Package/uboot-lantiq-arv4518PW_ramboot=$(call Package/uboot-lantiq-template,arv4518PW_ramboot,RAM)
Package/uboot-lantiq-arv4518PW_brnboot=$(call Package/uboot-lantiq-template,arv4518PW_brnboot,BRN)
Package/uboot-lantiq-arv4520PW_flash=$(call Package/uboot-lantiq-template,arv4520PW_flash,NOR)
Package/uboot-lantiq-arv4520PW_ramboot=$(call Package/uboot-lantiq-template,arv4520PW_ramboot,RAM)
Package/uboot-lantiq-arv4520PW_brnboot=$(call Package/uboot-lantiq-template,arv4520PW_brnboot,BRN)
Package/uboot-lantiq-arv4525PW_flash=$(call Package/uboot-lantiq-template,arv4525PW_flash,NOR)
Package/uboot-lantiq-arv4525PW_ramboot=$(call Package/uboot-lantiq-template,arv4525PW_ramboot,RAM)
Package/uboot-lantiq-arv4525PW_brnboot=$(call Package/uboot-lantiq-template,arv4525PW_brnboot,BRN)
Package/uboot-lantiq-arv452CPW_flash=$(call Package/uboot-lantiq-template,arv452CPW_flash,NOR)
Package/uboot-lantiq-arv452CPW_ramboot=$(call Package/uboot-lantiq-template,arv452CPW_ramboot,RAM)
Package/uboot-lantiq-arv452CPW_brnboot=$(call Package/uboot-lantiq-template,arv452CPW_brnboot,BRN)
Package/uboot-lantiq-arv752DPW_flash=$(call Package/uboot-lantiq-template,arv752DPW_flash,NOR)
Package/uboot-lantiq-arv752DPW_ramboot=$(call Package/uboot-lantiq-template,arv752DPW_ramboot,RAM)
Package/uboot-lantiq-arv752DPW_brnboot=$(call Package/uboot-lantiq-template,arv752DPW_brnboot,BRN)
Package/uboot-lantiq-arv752DPW22_flash=$(call Package/uboot-lantiq-template,arv752DPW22_flash,NOR)
Package/uboot-lantiq-arv752DPW22_ramboot=$(call Package/uboot-lantiq-template,arv752DPW22_ramboot,RAM)
Package/uboot-lantiq-arv752DPW22_brnboot=$(call Package/uboot-lantiq-template,arv752DPW22_brnboot,BRN)

DDR_CONFIG_arv3527P_ramboot:=arcadyan_psc166_32
DDR_CONFIG_arv4518PW_ramboot:=arcadyan_psc166_64
DDR_CONFIG_arv4520PW_ramboot:=arcadyan_psc166_32
DDR_CONFIG_arv4525PW_ramboot:=arcadyan_psc166_32
DDR_CONFIG_arv452CPW_ramboot:=arcadyan_psc166_32
DDR_CONFIG_arv752DPW_ramboot:=arcadyan_psc166_64
DDR_CONFIG_arv752DPW22_ramboot:=arcadyan_psc166_64

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
ifneq ($(findstring ramboot,$(BUILD_VARIANT)),)
	dd \
		if=$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/u-boot.bin \
		of=$(1)/u-boot.bin \
		bs=64k conv=sync
else
	dd \
		if=$(PKG_BUILD_DIR)/$(BUILD_VARIANT)/u-boot-bootstrap.bin \
		of=$(1)/u-boot-bootstrap.bin \
		bs=64k conv=sync
endif
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
$(eval $(call BuildPackage,uboot-lantiq-gigaSX76X_DDRsamsung166))
#$(eval $(call BuildPackage,uboot-lantiq-arv3527P_flash))
#$(eval $(call BuildPackage,uboot-lantiq-arv3527P_brnboot))
#$(eval $(call BuildPackage,uboot-lantiq-arv3527P_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4518PW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4520PW_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv4520PW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4520PW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4525PW_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv4525PW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv4525PW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv452CPW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW_ramboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22_flash))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22_brnboot))
$(eval $(call BuildPackage,uboot-lantiq-arv752DPW22_ramboot))

