include $(TOPDIR)/rules.mk

PKG_NAME:=signify
PKG_VERSION:=29
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/aperezdc/$(PKG_NAME)/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=8d255ad88290c9b675711f47cc7336b736f6a7adcccd822a0c1a03e3d53de41c

PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>
PKG_LICENSE:=ISC
PKG_LICENSE_FILES:=COPYING

PKG_BUILD_PARALLEL:=1
HOST_BUILD_PARALLEL:=1
PKG_INSTALL:=1

HOST_BUILD_DEPENDS:=libbsd/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

define Package/signify-full
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+libbsd
  TITLE:=OpenWrt signature generation and verification utility
  PROVIDES:=signify
  VARIANT:=full
endef

define Package/signify
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=
  TITLE:=OpenWrt signature verification utility
  PROVIDES:=signify
  CONFLICTS:=signify-full
  VARIANT:=tiny
endef

define Package/signify/description
  OpenBSD tool to signs and verify signatures on files. Portable version.
endef

ifeq ($(BUILD_VARIANT),tiny)
  MAKE_VARS += VERIFY_ONLY=1
endif

define Package/signify/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/signify $(1)/usr/bin/signify
endef

Package/signify-full/install = $(Package/signify/install)

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/signify $(STAGING_DIR_HOST)/bin/signify
endef

$(eval $(call BuildPackage,signify))
$(eval $(call BuildPackage,signify-full))
$(eval $(call HostBuild))
