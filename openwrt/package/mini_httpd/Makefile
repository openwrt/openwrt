# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mini-httpd
PKG_VERSION:=1.19
PKG_RELEASE:=1
PKG_MD5SUM:=792a529dfe974355aad8ba6c80e54e7a

PKG_SOURCE_URL:=http://www.acme.com/software/mini_httpd/
PKG_SOURCE:=mini_httpd-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/mini_httpd-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MINI_HTTPD,mini-httpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))


$(PKG_BUILD_DIR)/.configured:
	cp -f ./files/matrixssl_helper.{c,h} $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		SSL_INC="-I$(STAGING_DIR)/usr/include" \
		SSL_LIBS="-L$(STAGING_DIR)/usr/lib -lmatrixssl" \
		all
	touch $@

$(IPKG_MINI_HTTPD):
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc
	install -m0644 ./files/mini_httpd.conf $(IDIR_MINI_HTTPD)/etc/
	install -m0600 ./files/mini_httpd.pem $(IDIR_MINI_HTTPD)/etc/
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc/default
	install -m0644 ./files/mini_httpd.default $(IDIR_MINI_HTTPD)/etc/default/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc/init.d
	install -m0755 ./files/mini_httpd.init $(IDIR_MINI_HTTPD)/etc/init.d/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/mini_httpd $(IDIR_MINI_HTTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_MINI_HTTPD)
	$(IPKG_BUILD) $(IDIR_MINI_HTTPD) $(PACKAGE_DIR)
