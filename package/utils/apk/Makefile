include $(TOPDIR)/rules.mk

PKG_NAME:=apk
PKG_VERSION:=ca784ceab3f504b7cf9f481ccb76dee021abd76c
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=apk-tools-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://gitlab.alpinelinux.org/alpine/apk-tools/-/archive/$(PKG_VERSION)
PKG_HASH:=5ac1be0b763f7b91d732253b55e266afcc53197133247db63fd14b4a7ba45ad8
PKG_BUILD_DIR:=$(BUILD_DIR)/apk-tools-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/apk-tools-$(PKG_VERSION)

PKG_MAINTAINER:=Paul Spooren <mail@aparcar.org>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
HOST_BUILD_DEPENDS:=libressl/host zlib/host

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/apk
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=apk package manager
  DEPENDS:=+zlib +libopenssl +libpthread @!arc
  URL:=$(PKG_SOURCE_URL)
endef

MAKE_FLAGS += LUA=no
HOST_CFLAGS += -I$(STAGING_DIR_HOSTPKG)/lib/
HOST_LDFLAGS += -pthread
HOST_MAKE_FLAGS += \
	DESTDIR=$(STAGING_DIR_HOSTPKG) \
	LUA=no

define Package/apk/install
	$(INSTALL_DIR) $(1)/lib/apk/db

	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/apk $(1)/bin/apk

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/lib/* $(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/apk.pc \
		$(1)/usr/lib/pkgconfig/

	$(INSTALL_DIR) $(1)/etc/apk/
	echo $(ARCH) > $(1)/etc/apk/arch
	touch $(1)/etc/apk/world
endef

$(eval $(call BuildPackage,apk))
$(eval $(call HostBuild))
