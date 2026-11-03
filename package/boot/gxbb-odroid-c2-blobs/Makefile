# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gxbb-odroid-c2-blobs
PKG_VERSION:=2019-08-23
PKG_RELEASE:=1

PKG_MAINTAINER:=Fil Dunsky <filipp.dunsky@gmail.com>
PKG_LICENSE:=LicenseRef-Proprietary
PKG_LICENSE_FILES:=

PKG_HOST_ONLY:=1
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

BLOBS_URL:=@GITHUB/hardkernel/u-boot/fac4d2da0a1b61dfdeaca0034a45151ff5983fb8/

define Download/bl1.bin.hardkernel
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl1.bin.hardkernel
  URL:=$(BLOBS_URL)sd_fuse/
  URL_FILE:=bl1.bin.hardkernel
  HASH:=15cf3aa6c6bd1f296de6f1e08de68c200f95f80197765a587743d6b4edf06435
endef
$(eval $(call Download,bl1.bin.hardkernel))

define Download/bl2.package
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl2.package
  URL:=$(BLOBS_URL)fip/gxb/
  URL_FILE:=bl2.package
  HASH:=60f71e945ed198d73722d1f3d7c1a83b3b03c91d4ea7727becaa78df4a69483d
endef
$(eval $(call Download,bl2.package))

define Download/bl30.bin
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl30.bin
  URL:=$(BLOBS_URL)fip/gxb/
  URL_FILE:=bl30.bin
  HASH:=82962cffd3cb7734e787126db92b1d90a55b8b6f5e9d239be7726841e36ccdaf
endef
$(eval $(call Download,bl30.bin))

define Download/bl301.bin
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl301.bin
  URL:=$(BLOBS_URL)fip/gxb/
  URL_FILE:=bl301.bin
  HASH:=696a42490ff43e505ca101f5b9cea14267f5cb62ac4ab495157d39e55f268051
endef
$(eval $(call Download,bl301.bin))

define Download/bl31.bin
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl31.bin
  URL:=$(BLOBS_URL)fip/gxb/
  URL_FILE:=bl31.bin
  HASH:=9e82511a5831a832ad4a4b3d1ddad3445973c570a8d8bee92081282b8c3ef060
endef
$(eval $(call Download,bl31.bin))

define Download/fip_create
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-fip_create
  URL:=$(BLOBS_URL)fip/
  URL_FILE:=fip_create
  HASH:=0470d54769d96e8f9cf91d433f2dcc1c4fa641a4e45e1f27fbc686813ba96512
endef
$(eval $(call Download,fip_create))

define Download/aml_encrypt_gxb
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-aml_encrypt_gxb
  URL:=$(BLOBS_URL)fip/gxb/
  URL_FILE:=aml_encrypt_gxb
  HASH:=9a682c396f7be1b423c2219fa42a130f11b72e7ed0708f4f4956dd6283bf6469
endef
$(eval $(call Download,aml_encrypt_gxb))

define Package/gxbb-odroid-c2-blobs
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=Signed boot stages for the Amlogic GXBB ODROID-C2
  URL:=https://github.com/hardkernel/u-boot
  BUILDONLY:=1
endef

define Package/gxbb-odroid-c2-blobs/description
  Signed BL1 and BL2, the BL30, BL301 and BL31 firmware and the
  fip_create and aml_encrypt_gxb host tools from Hardkernel, which are
  needed to wrap U-Boot into an image that the boot ROM of the Amlogic
  GXBB on the ODROID-C2 accepts.
endef

define Host/Compile
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/$(PKG_NAME)
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-bl1.bin.hardkernel \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/bl1.bin.hardkernel
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-bl2.package \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/bl2.package
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-bl30.bin \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/bl30.bin
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-bl301.bin \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/bl301.bin
	$(INSTALL_DATA) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-bl31.bin \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/bl31.bin
	$(INSTALL_BIN) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-fip_create \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/fip_create
	$(INSTALL_BIN) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-aml_encrypt_gxb \
		$(STAGING_DIR_HOST)/share/$(PKG_NAME)/aml_encrypt_gxb
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,gxbb-odroid-c2-blobs))
