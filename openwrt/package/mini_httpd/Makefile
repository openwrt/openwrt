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
$(eval $(call PKG_template,MINI_HTTPD_HTPASSWD,mini-httpd-htpasswd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,MINI_HTTPD_MATRIXSSL,mini-httpd-matrixssl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,MINI_HTTPD_OPENSSL,mini-httpd-openssl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))


$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/matrixssl_helper.{c,h} $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.built:
	# with MatrixSSL
ifneq ($(BR2_PACKAGE_MINI_HTTPD_MATRIXSSL),)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		SSL_DEFS="-DUSE_SSL -DHAVE_MATRIXSSL" \
		SSL_INC="-I$(STAGING_DIR)/usr/include" \
		SSL_LIBS="-L$(STAGING_DIR)/usr/lib -lmatrixssl" \
		SSL_OBJS="matrixssl_helper.o" \
		all
	(cd $(PKG_BUILD_DIR); mv mini_httpd mini_httpd-matrixssl; )
	$(MAKE) -C $(PKG_BUILD_DIR) clean
endif
	# with OpenSSL
ifneq ($(BR2_PACKAGE_MINI_HTTPD_OPENSSL),)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		SSL_DEFS="-DUSE_SSL -DHAVE_OPENSSL" \
		SSL_INC="-I$(STAGING_DIR)/usr/include" \
		SSL_LIBS="-L$(STAGING_DIR)/usr/lib -lssl -lcrypto" \
		all
	(cd $(PKG_BUILD_DIR); mv mini_httpd mini_httpd-openssl; )
	$(MAKE) -C $(PKG_BUILD_DIR) clean
endif
	# without SSL
ifneq ($(BR2_PACKAGE_MINI_HTTPD),)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		all
endif
	touch $@

$(IPKG_MINI_HTTPD):
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc
	install -m0644 ./files/mini_httpd.conf $(IDIR_MINI_HTTPD)/etc/mini_httpd.conf
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc/default
	install -m0644 ./files/mini_httpd.default $(IDIR_MINI_HTTPD)/etc/default/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD)/etc/init.d
	install -m0755 ./files/mini_httpd.init $(IDIR_MINI_HTTPD)/etc/init.d/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/mini_httpd $(IDIR_MINI_HTTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_MINI_HTTPD)
	$(IPKG_BUILD) $(IDIR_MINI_HTTPD) $(PACKAGE_DIR)

$(IPKG_MINI_HTTPD_HTPASSWD):
	install -d -m0755 $(IDIR_MINI_HTTPD_HTPASSWD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/htpasswd $(IDIR_MINI_HTTPD_HTPASSWD)/usr/sbin/
	$(RSTRIP) $(IDIR_MINI_HTTPD_HTPASSWD)
	$(IPKG_BUILD) $(IDIR_MINI_HTTPD_HTPASSWD) $(PACKAGE_DIR)

$(IPKG_MINI_HTTPD_MATRIXSSL):
	install -d -m0755 $(IDIR_MINI_HTTPD_MATRIXSSL)/etc
	install -m0644 ./files/mini_httpd-ssl.conf $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/mini_httpd.conf
	install -m0600 ./files/mini_httpd.pem $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/
	install -d -m0755 $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/default
	install -m0644 ./files/mini_httpd.default $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/default/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/init.d
	install -m0755 ./files/mini_httpd.init $(IDIR_MINI_HTTPD_MATRIXSSL)/etc/init.d/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD_MATRIXSSL)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/mini_httpd-matrixssl $(IDIR_MINI_HTTPD_MATRIXSSL)/usr/sbin/mini_httpd
	$(RSTRIP) $(IDIR_MINI_HTTPD_MATRIXSSL)
	$(IPKG_BUILD) $(IDIR_MINI_HTTPD_MATRIXSSL) $(PACKAGE_DIR)

$(IPKG_MINI_HTTPD_OPENSSL):
	install -d -m0755 $(IDIR_MINI_HTTPD_OPENSSL)/etc
	install -m0644 ./files/mini_httpd-ssl.conf $(IDIR_MINI_HTTPD_OPENSSL)/etc/mini_httpd.conf
	install -m0600 ./files/mini_httpd.pem $(IDIR_MINI_HTTPD_OPENSSL)/etc/
	install -d -m0755 $(IDIR_MINI_HTTPD_OPENSSL)/etc/default
	install -m0644 ./files/mini_httpd.default $(IDIR_MINI_HTTPD_OPENSSL)/etc/default/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD_OPENSSL)/etc/init.d
	install -m0755 ./files/mini_httpd.init $(IDIR_MINI_HTTPD_OPENSSL)/etc/init.d/mini_httpd
	install -d -m0755 $(IDIR_MINI_HTTPD_OPENSSL)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/mini_httpd-openssl $(IDIR_MINI_HTTPD_OPENSSL)/usr/sbin/mini_httpd
	$(RSTRIP) $(IDIR_MINI_HTTPD_OPENSSL)
	$(IPKG_BUILD) $(IDIR_MINI_HTTPD_OPENSSL) $(PACKAGE_DIR)
