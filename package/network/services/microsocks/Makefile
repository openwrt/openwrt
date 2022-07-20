include $(TOPDIR)/rules.mk

PKG_NAME:=microsocks
PKG_VERSION:=1.0.3
PKG_RELEASE:=1
PKG_SOURCE:=microsocks-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://ftp.barfooze.de/pub/sabotage/tarballs/
PKG_HASH:=02449160c505ff7ea1bcb71ae781dafcfe66ab95287f3a4fca11621b74b2d7ac
PKG_LICENSE:=MIT
PKG_MAINTAINER:=Mateusz Korniak <matkorgithubcom@ant.gliwice.pl>

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/microsocks
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SOCKS5 TCP/IP only proxy
  DEPENDS:=+libc +libpthread
endef

define Package/microsocks/description
  SOCKS5 only TCP/IP only proxy server
endef


define Package/microsocks/conffiles
/etc/config/microsocks
endef

# define Build/Compile
# 	# $(MAKE) clean
# 	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"	
# endef

define Package/microsocks/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/microsocks.config $(1)/etc/config/microsocks
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/microsocks.init $(1)/etc/init.d/microsocks
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/microsocks $(1)/usr/bin
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,microsocks))
