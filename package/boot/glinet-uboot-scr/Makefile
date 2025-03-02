include $(TOPDIR)/rules.mk

PKG_NAME:=glinet-uboot-scr
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/TheRootED24/glinet-uboot-scr.git
PKG_SOURCE_DATE:=2025-02-28

PKG_SOURCE_VERSION:=f0da65a17631b8ecd9f2e0d482ca6bbe812e28ae
PKG_MIRROR_HASH:=0417d9667001e2f71db046bf301ac28940755c72fa01b50a2a1421b1d63e1ad4

PKG_MAINTAINER:=Scott Mercer <TheRootED24@gmail.com>
PKG_LICENSE:=GPL-3.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/glinet-uboot-scr
	SECTION:=utils
	CATEGORY:=Utilities
	SUBMENU:=Boot Loaders
	TITLE:=SELECT <help> FOR PROPER INSTALLATION
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

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/glinet-uboot-scr
	$(CP) \
		$(PKG_BUILD_DIR)/src/*.scr \
		$(1)/usr/include/glinet-uboot-scr/ \

endef
$(eval $(call BuildPackage,glinet-uboot-scr))
