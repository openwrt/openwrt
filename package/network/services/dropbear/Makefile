#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dropbear
PKG_VERSION:=2013.59
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://matt.ucc.asn.au/dropbear/releases/
PKG_MD5SUM:=6c1e6c2c297f4034488ffc95e8b7e6e9

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE libtomcrypt/LICENSE libtommath/LICENSE

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dropbear/Default
  URL:=http://matt.ucc.asn.au/dropbear/
endef

define Package/dropbear
  $(call Package/dropbear/Default)
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=Small SSH2 client/server
endef

define Package/dropbear/description
 A small SSH2 server/client designed for small memory environments.
endef

define Package/dropbear/conffiles
/etc/dropbear/dropbear_rsa_host_key
/etc/dropbear/dropbear_dss_host_key 
/etc/config/dropbear 
endef

define Package/dropbearconvert
  $(call Package/dropbear/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utility for converting SSH keys
endef

CONFIGURE_ARGS += \
	--with-shared \
	--disable-pam \
	--enable-openpty \
	--enable-syslog \
	$(if $(CONFIG_SHADOW_PASSWORDS),,--disable-shadow) \
	--disable-lastlog \
	--disable-utmp \
	--disable-utmpx \
	--disable-wtmp \
	--disable-wtmpx \
	--disable-loginfunc \
	--disable-pututline \
	--disable-pututxline \
	--disable-zlib \
	--enable-bundled-libtom

TARGET_CFLAGS += -DARGTYPE=3 -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		PROGRAMS="dropbear dbclient dropbearkey scp" \
		MULTI=1 SCPPROGRESS=1
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		PROGRAMS="dropbearconvert"
endef

define Package/dropbear/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dropbearmulti $(1)/usr/sbin/dropbear
	$(INSTALL_DIR) $(1)/usr/bin
	ln -sf ../sbin/dropbear $(1)/usr/bin/scp
	ln -sf ../sbin/dropbear $(1)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(1)/usr/bin/dbclient
	ln -sf ../sbin/dropbear $(1)/usr/bin/dropbearkey
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/dropbear.config $(1)/etc/config/dropbear
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dropbear.init $(1)/etc/init.d/dropbear
	$(INSTALL_DIR) $(1)/usr/lib/opkg/info
	$(INSTALL_DIR) $(1)/etc/dropbear
	touch $(1)/etc/dropbear/dropbear_rsa_host_key
	touch $(1)/etc/dropbear/dropbear_dss_host_key
endef

define Package/dropbearconvert/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dropbearconvert $(1)/usr/bin/dropbearconvert
endef

$(eval $(call BuildPackage,dropbear))
$(eval $(call BuildPackage,dropbearconvert))
