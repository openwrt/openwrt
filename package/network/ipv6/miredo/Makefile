include $(TOPDIR)/rules.mk

PKG_NAME:=miredo
PKG_VERSION:=1.2.6
PKG_RELEASE:=1

PKG_SOURCE:=miredo-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://www.remlab.net/files/miredo/
PKG_HASH:=fa26d2f4a405415833669e2e2e22677b225d8f83600844645d5683535ea43149
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING
PKG_CONFIG_DEPENDS := 	CONFIG_IPV6 \
			CONFIG_TUN
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS+= \
		--enable-shared \
		--enable-static \
		--disable-binreloc \
		--with-pic \
		--without-libiconv-prefix \
		--without-libintl-prefix

TARGET_CFLAGS+= $(FPIC) \
		-std=gnu99 \
		-O3 \
		-ffunction-sections \
		-fdata-sections \
		-Wno-format-security

TARGET_LDFLAGS += -Wl,--gc-sections

define Package/miredo
	SECTION:=net
	CATEGORY:=Network
	TITLE:=Teredo IPv6 tunneling utility
	URL:=https://www.remlab.net/miredo/
	VERSION:=$(PKG_VERSION)
	DEPENDS:=@IPV6 +libpthread +librt +kmod-tun
endef

define Package/miredo/description
 Miredo is an open-source Teredo IPv6 tunneling software, for Linux and the BSD
 operating systems. It includes functional implementations of all components of
 the Teredo specification (client, relay and server). It is meant to provide
 IPv6 connectivity even from behind NAT devices.
endef

define Package/miredo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/miredo $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/miredo-checkconf $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/miredo
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/miredo/miredo-privproc $(1)/usr/lib/miredo
	$(INSTALL_DIR) $(1)/etc/miredo
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/miredo/miredo.conf $(1)/etc/miredo
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/etc/miredo/client-hook $(1)/etc/miredo
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/miredo.init $(1)/etc/init.d/miredo
endef

$(eval $(call BuildPackage,miredo))
