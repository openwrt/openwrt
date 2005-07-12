# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ppp
PKG_VERSION:=2.4.3
PKG_RELEASE:=5
PKG_MD5SUM:=848f6c3cafeb6074ffeb293c3af79b7c

PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/ppp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

define PKG_mod_template

$$(IPKG_$(1)):
	install -d -m0755 $$(IDIR_$(1))/usr/lib/pppd/$(PKG_VERSION)
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/$(2).so \
		$$(IDIR_$(1))/usr/lib/pppd/$(PKG_VERSION)/
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,PPP,ppp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPP_MOD_PPPOE,ppp-mod-pppoe,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPP_MOD_RADIUS,ppp-mod-radius,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,CHAT,chat,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPPDUMP,pppdump,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPPSTATS,pppstats,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_mod_template,PPP_MOD_PPPOE,rp-pppoe))
$(eval $(call PKG_mod_template,PPP_MOD_RADIUS,radius))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		COPTS="$(TARGET_CFLAGS)" \
		HAVE_INET6="1" \
		DESTDIR="$(PKG_INSTALL_DIR)/usr" \
		all install
	touch $@

$(IPKG_PPP):
	install -d -m0755 $(IDIR_PPP)/etc/ppp
	ln -sf /tmp/resolv.conf $(IDIR_PPP)/etc/ppp/resolv.conf
	install -m0600 ./files/etc/ppp/chap-secrets $(IDIR_PPP)/etc/ppp/
	install -m0644 ./files/etc/ppp/options $(IDIR_PPP)/etc/ppp/
	install -d -m0755 $(IDIR_PPP)/etc/ppp/peers
	install -m0644 ./files/etc/ppp/peers/sample $(IDIR_PPP)/etc/ppp/peers/
	install -d -m0755 $(IDIR_PPP)/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppd $(IDIR_PPP)/usr/sbin/
	install -d -m0755 $(IDIR_PPP)/usr/lib/pppd
	$(RSTRIP) $(IDIR_PPP)
	$(IPKG_BUILD) $(IDIR_PPP) $(PACKAGE_DIR)
	
$(IDIR_PPP_MOD_PPPOE)/etc/init.d/S50pppoe:
	install -d -m0755 $(IDIR_PPP_MOD_PPPOE)/sbin
	install -m0755 ./files/ifup.pppoe $(IDIR_PPP_MOD_PPPOE)/sbin/

$(IPKG_PPP_MOD_PPPOE): $(IDIR_PPP_MOD_PPPOE)/etc/init.d/S50pppoe

$(IDIR_PPP_MOD_RADIUS)/etc/ppp/radius.conf:

	install -d -m0755 $(IDIR_PPP_MOD_RADIUS)/etc/ppp
	install -m644 ./files/etc/ppp/radius.conf $(IDIR_PPP_MOD_RADIUS)/etc/ppp/
	install -d -m0755 $(IDIR_PPP_MOD_RADIUS)/etc/ppp/radius
	install -m644 ./files/etc/ppp/radius/dictionary* \
		$(IDIR_PPP_MOD_RADIUS)/etc/ppp/radius/
	install -m600 ./files/etc/ppp/radius/servers \
		$(IDIR_PPP_MOD_RADIUS)/etc/ppp/radius/

$(IPKG_PPP_MOD_RADIUS): $(IDIR_PPP_MOD_RADIUS)/etc/ppp/radius.conf

$(IPKG_CHAT):
	install -d -m0755 $(IDIR_CHAT)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/chat $(IDIR_CHAT)/usr/sbin/
	$(RSTRIP) $(IDIR_CHAT)
	$(IPKG_BUILD) $(IDIR_CHAT) $(PACKAGE_DIR)

$(IPKG_PPPDUMP):
	install -d -m0755 $(IDIR_PPPDUMP)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/pppdump $(IDIR_PPPDUMP)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPDUMP)
	$(IPKG_BUILD) $(IDIR_PPPDUMP) $(PACKAGE_DIR)

$(IPKG_PPPSTATS): $(IPKG_PPP)
	install -d -m0755 $(IDIR_PPPSTATS)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/pppstats $(IDIR_PPPSTATS)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPSTATS)
	$(IPKG_BUILD) $(IDIR_PPPSTATS) $(PACKAGE_DIR)

