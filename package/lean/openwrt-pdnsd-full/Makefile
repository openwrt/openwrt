include $(TOPDIR)/rules.mk

PKG_NAME:=pdnsd
PKG_VERSION:=1.2.9a-par
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/shadowsocksrr/pdnsd.git
PKG_SOURCE_VERSION:=a8e46ccba7b0fa2230d6c42ab6dcd92926f6c21d
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

PKG_FIXUP:=autoreconf
PKG_USE_MIPS16:=0
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pdnsd-full
	SECTION:=net
	CATEGORY:=Network
	DEPENDS:=+libpthread
	TITLE:=Proxy DNS Server
endef

define Package/pdnsd-full/conffiles
/etc/pdnsd.conf
endef

define Package/pdnsd-full/description
  pdnsd, is an IPv6 capable proxy DNS server with permanent caching (the cache
  contents are written to hard disk on exit) that is designed to cope with
  unreachable or down DNS servers (for example in dial-in networking).

  pdnsd can be used with applications that do dns lookups, eg on startup, and
  can't be configured to change that behaviour, to prevent the often
  minute-long hangs (or even crashes) that result from stalled dns queries.
endef

CONFIGURE_ARGS += --enable-ipv6 --enable-ipv6-startup --enable-tcp-subseq --with-cachedir=/var/pdnsd --with-query-method=tcpudp

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

# Use Link time optimization
TARGET_CFLAGS += -flto
TARGET_LDFLAGS += -Wl,-flto

define Package/pdnsd-full/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pdnsd $(1)/usr/sbin/pdnsd-full
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pdnsd-ctl/pdnsd-ctl $(1)/usr/bin/pdnsd-ctl-full
	#$(INSTALL_DIR) $(1)/etc
	#$(INSTALL_CONF) ./files/pdnsd.conf $(1)/etc/pdnsd.conf
	# $(INSTALL_CONF) $(PKG_BUILD_DIR)/doc/pdnsd.conf $(1)/etc/)
endef

$(eval $(call BuildPackage,pdnsd-full))
