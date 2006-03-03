# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=php
PKG_VERSION:=5.0.5
PKG_RELEASE:=1
PKG_MD5SUM:=b5d4ca75bbb11ee5b830fa67213d9f7f

PKG_SOURCE_URL:=http://fr.php.net/distributions/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	install -d -m0755 $$(IDIR_$(1))/usr/lib/php
	install -m0755 $(PKG_BUILD_DIR)/modules/$(2).so $$(IDIR_$(1))/usr/lib/php
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

PKG_CONFIGURE_OPTS:= \
	--enable-shared \
	--disable-static \
	--disable-rpath \
	--disable-debug \
	--without-pear \
	--disable-spl \
	\
	--with-config-file-path=/etc \
	--disable-ipv6 \
	--enable-magic-quotes \
	--enable-memory-limit \
	--disable-short-tags \
	\
	--disable-ctype \
	--disable-dom \
	--enable-ftp=shared \
	--without-gettext \
	--without-iconv \
	--disable-libxml \
	  --without-libxml-dir \
	--disable-mbstring \
	--disable-mbregex \
	--with-openssl=shared,"$(STAGING_DIR)/usr" \
	  --with-kerberos=no \
	  --with-openssl-dir="$(STAGING_DIR)/usr" \
	--enable-session=shared \
	--disable-simplexml \
	--disable-soap \
	--enable-sockets=shared \
	--disable-tokenizer \
	--with-zlib="$(STAGING_DIR)/usr" \
	  --with-zlib-dir="$(STAGING_DIR)/usr" \

ifneq ($(BR2_PACKAGE_PHP5_MOD_CURL),)
PKG_CONFIGURE_OPTS+= --with-curl=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-curl
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_GD),)
PKG_CONFIGURE_OPTS+= --with-gd=shared,"$(STAGING_DIR)/usr" \
	--without-freetype-dir \
	--with-jpeg-dir="$(STAGING_DIR)/usr" \
	--with-png-dir="$(STAGING_DIR)/usr" \
	--without-xpm-dir \
	--without-ttf \
	--without-t1lib	\
	--enable-gd-native-ttf \
	--disable-gd-jis-conv
else
PKG_CONFIGURE_OPTS+= --without-gd
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_GMP),)
PKG_CONFIGURE_OPTS+= --with-gmp=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-gmp
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_LDAP),)
PKG_CONFIGURE_OPTS+= --with-ldap=shared,"$(STAGING_DIR)/usr" \
	--with-ldap-sasl="$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-ldap
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_MYSQL),)
PKG_CONFIGURE_OPTS+= --with-mysql=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-mysql
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_PCRE),)
PKG_CONFIGURE_OPTS+= --with-pcre-regex=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-pcre-regex
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_PGSQL),)
PKG_CONFIGURE_OPTS+= --with-pgsql=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-pgsql
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_SQLITE),)
PKG_CONFIGURE_OPTS+= --with-sqlite=shared,"$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --without-sqlite
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_XML),)
PKG_CONFIGURE_OPTS+= --enable-xml=shared,"$(STAGING_DIR)/usr" \
	--with-libexpat-dir="$(STAGING_DIR)/usr"
else
PKG_CONFIGURE_OPTS+= --disable-xml
endif

$(eval $(call PKG_template,PHP5_CLI,php5-cli,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_CGI,php5-cgi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_FASTCGI,php5-fastcgi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_template,PHP5_MOD_CURL,php5-mod-curl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_FTP,php5-mod-ftp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_GD,php5-mod-gd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_GMP,php5-mod-gmp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_LDAP,php5-mod-ldap,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_MYSQL,php5-mod-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_OPENSSL,php5-mod-openssl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_PCRE,php5-mod-pcre,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_PGSQL,php5-mod-pgsql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_SESSION,php5-mod-session,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_SOCKETS,php5-mod-sockets,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_SQLITE,php5-mod-sqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PHP5_MOD_XML,php5-mod-xml,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,PHP5_MOD_CURL,curl))
$(eval $(call PKG_mod_template,PHP5_MOD_FTP,ftp))
$(eval $(call PKG_mod_template,PHP5_MOD_GD,gd))
$(eval $(call PKG_mod_template,PHP5_MOD_GMP,gmp))
$(eval $(call PKG_mod_template,PHP5_MOD_LDAP,ldap))
$(eval $(call PKG_mod_template,PHP5_MOD_MYSQL,mysql))
$(eval $(call PKG_mod_template,PHP5_MOD_OPENSSL,openssl))
$(eval $(call PKG_mod_template,PHP5_MOD_PCRE,pcre))
$(eval $(call PKG_mod_template,PHP5_MOD_PGSQL,pgsql))
$(eval $(call PKG_mod_template,PHP5_MOD_SESSION,session))
$(eval $(call PKG_mod_template,PHP5_MOD_SOCKETS,sockets))
$(eval $(call PKG_mod_template,PHP5_MOD_SQLITE,sqlite))
$(eval $(call PKG_mod_template,PHP5_MOD_XML,xml))


$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		LIBS="-lcrypto -lssl" \
		php_cv_cc_rpath="no" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  $(PKG_CONFIGURE_OPTS) \
		  --enable-cli \
		  --disable-cgi \
		  --disable-fastcgi \
		  --enable-force-cgi-redirect \
		  --enable-discard-path \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cli/php $(PKG_BUILD_DIR)/php-cli
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		LIBS="-lcrypto -lssl" \
		php_cv_cc_rpath="no" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  $(PKG_CONFIGURE_OPTS) \
		  --disable-cli \
		  --enable-cgi \
		  --disable-fastcgi \
		  --enable-force-cgi-redirect \
		  --enable-discard-path \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cgi/php $(PKG_BUILD_DIR)/php-cgi
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		LIBS="-lcrypto -lssl" \
		php_cv_cc_rpath="no" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/sbin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  $(PKG_CONFIGURE_OPTS) \
		  --disable-cli \
		  --enable-cgi \
		  --enable-fastcgi \
		  --enable-force-cgi-redirect \
		  --enable-discard-path \
	);
	$(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cgi/php $(PKG_BUILD_DIR)/php-fastcgi
	touch $@

$(IPKG_PHP5_CLI):
	install -m0755 -d $(IDIR_PHP5_CLI)/etc
	install -m0644 ./files/php.ini $(IDIR_PHP5_CLI)/etc/
	install -m0755 -d $(IDIR_PHP5_CLI)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/php-cli $(IDIR_PHP5_CLI)/usr/bin/php
	$(RSTRIP) $(IDIR_PHP5_CLI)
	$(IPKG_BUILD) $(IDIR_PHP5_CLI) $(PACKAGE_DIR)

$(IPKG_PHP5_CGI):
	install -m0755 -d $(IDIR_PHP5_CGI)/etc
	install -m0644 ./files/php.ini $(IDIR_PHP5_CGI)/etc/
	install -m0755 -d $(IDIR_PHP5_CGI)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/php-cgi $(IDIR_PHP5_CGI)/usr/bin/php
	$(RSTRIP) $(IDIR_PHP5_CGI)
	$(IPKG_BUILD) $(IDIR_PHP5_CGI) $(PACKAGE_DIR)

$(IPKG_PHP5_FASTCGI):
	install -m0755 -d $(IDIR_PHP5_FASTCGI)/etc
	install -m0644 ./files/php.ini $(IDIR_PHP5_FASTCGI)/etc/
	install -m0755 -d $(IDIR_PHP5_FASTCGI)/etc/init.d
	install -m0755 ./files/php.init $(IDIR_PHP5_FASTCGI)/etc/init.d/php
	install -m0755 -d $(IDIR_PHP5_FASTCGI)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/php-fastcgi $(IDIR_PHP5_FASTCGI)/usr/sbin/php
	$(RSTRIP) $(IDIR_PHP5_FASTCGI)
	$(IPKG_BUILD) $(IDIR_PHP5_FASTCGI) $(PACKAGE_DIR)

