# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pwlib
PKG_VERSION:=cvs-20051227
PKG_RELEASE:=1
PKG_MD5SUM:=7d5b8e9ab61a05658a5630f91505420d

PKG_SOURCE_URL:=http://trash.uid0.hu/openwrt
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PWLIB,pwlib,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
	$(TARGET_CONFIGURE_OPTS) \
	    ./configure \
	    --prefix=$(STAGING_DIR) \
	    --target=$(GNU_TARGET_NAME) \
	    --host=$(GNU_TARGET_NAME) \
	    --build=$(GNU_HOST_NAME) \
	    --enable-openh323 \
	    --disable-openldap \
	    --disable-openssl \
	    --disable-ftp \
	    --disable-qos \
	    --disable-expat \
	    --disable-jabber \
	    --disable-xmlrpc \
	    --disable-ipv6 \
	    --disable-sdl \
	    --disable-video \
	    --disable-socks \
	    --disable-ftp \
	    --disable-snmp \
	    --disable-telnet \
	    --disable-serial
	    --disable-pop3smtp \
	    --disable-http \
	    --disable-httpsvc
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		all install
	touch $@
