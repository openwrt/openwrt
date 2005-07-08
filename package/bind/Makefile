# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bind
PKG_VERSION:=9.3.1
PKG_RELEASE:=1
PKG_MD5SUM:=9ff3204eea27184ea0722f37e43fc95d

PKG_SOURCE_URL:=ftp://ftp.isc.org/isc/bind9/9.3.1/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BIND,bind,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		./configure \
		  --target=$(GNU_HOST_NAME) \
		  --host=$(GNU_HOST_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --datadir=/usr/share \
		  --includedir=/usr/include \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  --without-openssl \
		  --without-ipv6 \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	);
	
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/isc 
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/dns
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/isc clean
	cp $(PKG_BUILD_DIR)/lib/dns/gen $(PKG_BUILD_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/dns clean
	cp $(PKG_BUILD_DIR)/gen $(PKG_BUILD_DIR)/lib/dns/gen
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
		  --enable-shared \
		  --enable-static \
		  --enable-ipv6 \
		  --with-randomdev=/dev/random \
		  --disable-threads \
		  --with-openssl=$(STAGING_DIR)/usr/ \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built

$(IPKG_BIND):
	install -d -m0755 $(IDIR_BIND)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/named $(IDIR_BIND)/usr/sbin/
	$(RSTRIP) $(IDIR_BIND)
	$(IPKG_BUILD) $(IDIR_BIND) $(PACKAGE_DIR)
