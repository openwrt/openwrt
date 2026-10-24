# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=exynos4412-odroid-blobs
PKG_VERSION:=2015-11-12
PKG_RELEASE:=1

PKG_LICENSE:=LicenseRef-Proprietary
PKG_LICENSE_FILES:=
PKG_MAINTAINER:=Josef Schlehofer <pepe.schlehofer@gmail.com>

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

BLOBS_URL:=@GITHUB/hardkernel/u-boot/136cae0717c9778dab5fb0cf6d35f65a0a9ec8c5/sd_fuse/

define Download/bl1
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl1.bin
  URL:=$(BLOBS_URL)
  URL_FILE:=bl1.HardKernel
  HASH:=44d9ed14fbb3cc48080685aed658b6cba4d48458ee8bdeb97f4b23b659826b22
endef
$(eval $(call Download,bl1))

define Download/bl2
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-bl2.bin
  URL:=$(BLOBS_URL)
  URL_FILE:=bl2.HardKernel
  HASH:=11206900ff8a16b834eb22954ab027113671bc83d5ce0d8af64d13364461bd32
endef
$(eval $(call Download,bl2))

define Download/tzsw
  FILE:=$(PKG_NAME)-$(PKG_VERSION)-tzsw.bin
  URL:=$(BLOBS_URL)
  URL_FILE:=tzsw.HardKernel
  HASH:=0865c9c82b0e4bb514bff393554d01aca2f38d72572a92dc08e49fa1d8d77ebc
endef
$(eval $(call Download,tzsw))

define Package/exynos4412-odroid-blobs
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_exynos
  TITLE:=Signed boot stages for Exynos4412 ODROID boards
  DEFAULT:=y if TARGET_exynos
endef

define Package/exynos4412-odroid-blobs/description
  Signed BL1, BL2 and TrustZone software from Hardkernel, which the boot ROM
  of the Exynos4412 needs to start U-Boot on the ODROID-U2, ODROID-U3 and
  ODROID-X2.
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(foreach blob,bl1 bl2 tzsw, \
		$(CP) $(DL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(blob).bin \
			$(STAGING_DIR_IMAGE)/exynos4412-odroid-$(blob).bin;)
endef

$(eval $(call BuildPackage,exynos4412-odroid-blobs))
