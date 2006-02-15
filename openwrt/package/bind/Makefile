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
PKG_INIT_LEVEL:=60

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BIND_SERVER,bind-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_CLIENT,bind-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_TOOLS,bind-tools,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_RNDC,bind-rndc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_CHECK,bind-check,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_DNSSEC,bind-dnssec,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_HOST,bind-host,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BIND_DIG,bind-dig,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))


$(PKG_BUILD_DIR)/.configured:
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
		  --with-randomdev=/dev/urandom \
		  --disable-threads \
		  --with-openssl=$(STAGING_DIR)/usr/ \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/dns \
		CC="$(HOSTCC)" \
		CFLAGS="-O2" \
		gen
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_BIND_SERVER):
	install -d -m0755 $(IDIR_BIND_SERVER)/usr/sbin
	install -d -m0755 $(IDIR_BIND_SERVER)/etc/init.d
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/named $(IDIR_BIND_SERVER)/usr/sbin/
	$(CP) files/bind $(IDIR_BIND_SERVER)/etc
	$(CP) files/named.init $(IDIR_BIND_SERVER)/etc/init.d/S$(PKG_INIT_LEVEL)named
	find $(IDIR_BIND_SERVER)/etc/bind -name ".svn" | xargs rm -rf
	$(RSTRIP) $(IDIR_BIND_SERVER)
	$(IPKG_BUILD) $(IDIR_BIND_SERVER) $(PACKAGE_DIR)

$(IPKG_BIND_CLIENT):
	install -d -m0755 $(IDIR_BIND_CLIENT)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nsupdate $(IDIR_BIND_CLIENT)/usr/bin/
	$(RSTRIP) $(IDIR_BIND_CLIENT)
	$(IPKG_BUILD) $(IDIR_BIND_CLIENT) $(PACKAGE_DIR)

$(IPKG_BIND_TOOLS):
	install -d -m0755 $(IDIR_BIND_TOOLS)/usr/bin
	install -d -m0755 $(IDIR_BIND_TOOLS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/dig $(IDIR_BIND_TOOLS)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/host $(IDIR_BIND_TOOLS)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dnssec-keygen $(IDIR_BIND_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dnssec-signzone $(IDIR_BIND_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/named-checkconf $(IDIR_BIND_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/named-checkzone $(IDIR_BIND_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rndc $(IDIR_BIND_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rndc-confgen $(IDIR_BIND_TOOLS)/usr/sbin/
	$(RSTRIP) $(IDIR_BIND_TOOLS)
	$(IPKG_BUILD) $(IDIR_BIND_TOOLS) $(PACKAGE_DIR)

$(IPKG_BIND_RNDC):
	install -d -m0755 $(IDIR_BIND_RNDC)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rndc $(IDIR_BIND_RNDC)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rndc-confgen $(IDIR_BIND_RNDC)/usr/sbin/
	$(RSTRIP) $(IDIR_BIND_RNDC)
	$(IPKG_BUILD) $(IDIR_BIND_RNDC) $(PACKAGE_DIR)

$(IPKG_BIND_CHECK):
	install -d -m0755 $(IDIR_BIND_CHECK)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/named-checkconf $(IDIR_BIND_CHECK)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/named-checkzone $(IDIR_BIND_CHECK)/usr/sbin/
	$(RSTRIP) $(IDIR_BIND_CHECK)
	$(IPKG_BUILD) $(IDIR_BIND_CHECK) $(PACKAGE_DIR)

$(IPKG_BIND_DNSSEC):
	install -d -m0755 $(IDIR_BIND_DNSSEC)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dnssec-keygen $(IDIR_BIND_DNSSEC)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/dnssec-signzone $(IDIR_BIND_DNSSEC)/usr/sbin/
	$(RSTRIP) $(IDIR_BIND_DNSSEC)
	$(IPKG_BUILD) $(IDIR_BIND_DNSSEC) $(PACKAGE_DIR)

$(IPKG_BIND_HOST):
	install -d -m0755 $(IDIR_BIND_HOST)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/host $(IDIR_BIND_HOST)/usr/bin/
	$(RSTRIP) $(IDIR_BIND_HOST)
	$(IPKG_BUILD) $(IDIR_BIND_HOST) $(PACKAGE_DIR)

$(IPKG_BIND_DIG):
	install -d -m0755 $(IDIR_BIND_DIG)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/dig $(IDIR_BIND_DIG)/usr/bin/
	$(RSTRIP) $(IDIR_BIND_DIG)
	$(IPKG_BUILD) $(IDIR_BIND_DIG) $(PACKAGE_DIR)
