include $(TOPDIR)/rules.mk

PKG_NAME:=apk
PKG_RELEASE:=1

PKG_SOURCE_URL=https://gitlab.alpinelinux.org/alpine/apk-tools.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-04-10
PKG_SOURCE_VERSION:=1a17367a2301b63ec88a2cbc62560fd832a3fc93
PKG_MIRROR_HASH:=b9cdb2499374902b43137ee4040a03eda722b97818aa8ec62feb5cbc6901b463

PKG_VERSION=3.0.0_pre$(subst -,,$(PKG_SOURCE_DATE))

PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_INSTALL:=1

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

MESON_HOST_VARS+=VERSION=$(PKG_VERSION)
MESON_VARS+=VERSION=$(PKG_VERSION)

define Package/apk
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=apk package manager
  DEPENDS:=+zlib +libopenssl
  URL:=$(PKG_SOURCE_URL)
endef

MESON_HOST_ARGS += \
	-Dlua_version=5.1 \
	-Dcompressed-help=false \
	-Ddocs=disabled \
	-Dzstd=false

MESON_ARGS += \
	-Dlua_version=5.1 \
	-Dcompressed-help=false \
	-Ddocs=disabled \
	-Durl_backend=wget \
	-Dzstd=false

HOST_LDFLAGS += \
	-Wl,-rpath $(STAGING_DIR_HOST)/lib

define Package/apk/install
	$(INSTALL_DIR) $(1)/lib/apk/db

	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/apk $(1)/usr/bin/apk

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libapk.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,apk))
$(eval $(call HostBuild))
