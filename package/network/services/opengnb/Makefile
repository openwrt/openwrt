include $(TOPDIR)/rules.mk

PKG_NAME:=opengnb
PKG_VERSION:=1.6.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/opengnb/opengnb/archive/refs/tags/ver$(PKG_VERSION).tar.gz?
PKG_HASH:=ed354e0c532c920473b7472ac2edc7a6379ac364f0a98887f9e3aef9215aaa0d

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-ver$(PKG_VERSION)

PKG_MAINTAINER:=OpenGNB Developer <hollidgelongsun157@gmail.com>
PKG_LICENSE:=GPL-3.0
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/opengnb
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  TITLE:=OpenGNB - P2P Decentralized Virtual Network
  URL:=https://github.com/opengnb/opengnb
  DEPENDS:=+libpthread +kmod-tun
endef

define Package/opengnb/description
  OpenGNB is an open source P2P decentralized Software Defined Virtual Network
  with extreme intranet penetration capability.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I. -I./src -I./libs -I./libs/miniupnpc -I./libs/libnatpmp -I./libs/zlib -D_GNU_SOURCE -DNO_GZIP=1 -DGNB_OPENWRT_BUILD=1" \
		LDFLAGS="$(TARGET_LDFLAGS) -pthread" \
		-f Makefile.openwrt \
		all
endef

define Package/opengnb/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gnb $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gnb_crypto $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gnb_ctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,opengnb))
