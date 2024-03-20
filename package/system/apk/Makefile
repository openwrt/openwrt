include $(TOPDIR)/rules.mk

PKG_NAME:=apk
PKG_RELEASE:=1

PKG_SOURCE_DATE:=2024-03-16
PKG_SOURCE_VERSION:=f2f10d7e851ed3ff2a239c89c1a6a3612adbd63f
PKG_HASH:=964bbc71462630a5418a20ccd3e725eb0b065a6232d4aee16db745d95a04c1ff

PKG_VERSION:=3.0.0_pre0
PKG_SOURCE:=apk-tools-v$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_URL:=https://gitlab.alpinelinux.org/alpine/apk-tools/-/archive/$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR:=$(BUILD_DIR)/apk-tools-$(PKG_SOURCE_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/apk-tools-$(PKG_SOURCE_VERSION)

PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_INSTALL:=1

HOST_BUILD_PREFIX:=$(STAGING_DIR_HOST)
HOST_BUILD_DEPENDS:=lua/host
PKG_BUILD_DEPENDS:=$(HOST_BUILD_DEPENDS)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/meson.mk

define Package/apk
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=apk package manager
  DEPENDS:=+zlib +libopenssl +libpthread
  URL:=$(PKG_SOURCE_URL)
endef

MESON_HOST_ARGS += \
	-Dlua_version=5.1 \
	-Dcompressed-help=false \
	-Ddocs=disabled \
	-Dzstd=False
MESON_ARGS += \
	-Dlua_version=5.1 \
	-Dcompressed-help=false \
	-Ddocs=disabled \
	-Dzstd=False
	
HOST_LDFLAGS += \
	-Wl,-rpath $(STAGING_DIR_HOST)/lib \
	-lpthread

TARGET_CFLAGS += $(FPIC)
HOST_CFLAGS += $(HOST_FPIC)

define Package/apk/install
	$(INSTALL_DIR) $(1)/lib/apk/db

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/apk $(1)/usr/bin/apk

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libapk.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,apk))
$(eval $(call HostBuild))
