# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rp-pppoe
PKG_VERSION:=3.5
PKG_RELEASE:=1
PKG_MD5SUM:=

PKG_SOURCE_URL:=http://roaringpenguin.com/penguin/pppoe/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PPPOE_CLIENT,pppoe-client,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPPOE_RELAY,pppoe-relay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPPOE_SERVER,pppoe-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,PPPOE_SNIFF,pppoe-sniff,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR)/src ; rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_setvbuf_reversed=no \
		ac_cv_sizeof_unsigned_short=2 \
		ac_cv_sizeof_unsigned_int=4 \
		ac_cv_sizeof_unsigned_long=4 \
		ac_cv_linux_kernel_pppoe=yes \
		ac_cv_pack_bitfields_reversed=yes \
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
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_PPPOE_CLIENT):
	install -d -m0755 $(IDIR_PPPOE_CLIENT)/etc/init.d
	install -m0755 ./files/pppoe-client.init $(IDIR_PPPOE_CLIENT)/etc/init.d/pppoe-client
	install -d -m0755 $(IDIR_PPPOE_CLIENT)/etc/ppp
	install -m0644 $(PKG_INSTALL_DIR)/etc/pppoe.conf $(IDIR_PPPOE_CLIENT)/etc/
	install -d -m0755 $(IDIR_PPPOE_CLIENT)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/pppoe $(IDIR_PPPOE_CLIENT)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/adsl-* $(IDIR_PPPOE_CLIENT)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPOE_CLIENT)
	$(IPKG_BUILD) $(IDIR_PPPOE_CLIENT) $(PACKAGE_DIR)

$(IPKG_PPPOE_RELAY):
	install -d -m0755 $(IDIR_PPPOE_RELAY)/etc/default
	install -m0755 ./files/pppoe-relay.default $(IDIR_PPPOE_RELAY)/etc/default/pppoe-relay
	install -d -m0755 $(IDIR_PPPOE_RELAY)/etc/init.d
	install -m0755 ./files/pppoe-relay.init $(IDIR_PPPOE_RELAY)/etc/init.d/pppoe-relay
	install -d -m0755 $(IDIR_PPPOE_RELAY)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/pppoe-relay $(IDIR_PPPOE_RELAY)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPOE_RELAY)
	$(IPKG_BUILD) $(IDIR_PPPOE_RELAY) $(PACKAGE_DIR)

$(IPKG_PPPOE_SERVER):
	install -d -m0755 $(IDIR_PPPOE_SERVER)/etc/default
	install -m0755 ./files/pppoe-server.default $(IDIR_PPPOE_SERVER)/etc/default/pppoe-server
	install -d -m0755 $(IDIR_PPPOE_SERVER)/etc/init.d
	install -m0755 ./files/pppoe-server.init $(IDIR_PPPOE_SERVER)/etc/init.d/pppoe-server
	install -d -m0755 $(IDIR_PPPOE_SERVER)/etc/ppp
	install -m0644 $(PKG_INSTALL_DIR)/etc/ppp/pppoe-server-options $(IDIR_PPPOE_SERVER)/etc/ppp/
	install -d -m0755 $(IDIR_PPPOE_SERVER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/pppoe-server $(IDIR_PPPOE_SERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPOE_SERVER)
	$(IPKG_BUILD) $(IDIR_PPPOE_SERVER) $(PACKAGE_DIR)

$(IPKG_PPPOE_SNIFF):
	install -d -m0755 $(IDIR_PPPOE_SNIFF)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/pppoe-sniff $(IDIR_PPPOE_SNIFF)/usr/sbin/
	$(RSTRIP) $(IDIR_PPPOE_SNIFF)
	$(IPKG_BUILD) $(IDIR_PPPOE_SNIFF) $(PACKAGE_DIR)

