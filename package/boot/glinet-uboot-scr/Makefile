include $(TOPDIR)/rules.mk

PKG_NAME:=glinet-uboot-scr
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/TheRootED24/glinet-uboot-scr.git
PKG_MIRROR_HASH:=28971e9b76e3735bff687bdd8836e6c02b6cd5e769155e60b9365c82dec9d5a0
PKG_SOURCE_VERSION:=9be98e03cce950334b88324a52ed6cb583ec2df0
PKG_SOURCE_DATE:=2025-03-05

PKG_MAINTAINER:=Scott Mercer <TheRootED24@gmail.com>
PKG_LICENSE:=GPL-3.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/glinet-uboot-scr
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:= Uboot flash.scr Files
	URL:=https://github.com/TheRootED24/glinet-uboot-scr.git
endef

define Package/glinet-uboot-scr/description
  This package contains the uboot scr file needed to
  enable uboot webui recovery and firmware upgrades
  for OpenWrt Firmwares.

           REQUIRED INSTALLATION NOTES

  This package MUST be included <*> in the firmware to
  effectively enable uboot webui recovery and upgrades
  for Openwrt.

  Installation of this package post-build will have
  zero effect and will not enable uboot upgrades and
  recovery options for Openwrt Firmwares.
endef

MAKE_PATH:=src

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/glinet-uboot-scr
	$(CP) \
		$(PKG_BUILD_DIR)/src/*.scr \
		$(1)/usr/include/glinet-uboot-scr/
endef

$(eval $(call BuildPackage,glinet-uboot-scr))

