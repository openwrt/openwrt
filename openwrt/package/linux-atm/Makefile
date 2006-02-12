# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=linux-atm
PKG_VERSION:=2.4.1
PKG_RELEASE:=1
PKG_MD5SUM:=84fef49cc39ff2605204246666f65864

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/linux-atm/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LINUX_ATM,linux-atm,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,BR2684CTL,br2684ctl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
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
		  --includedir=/usr/include/ \
		  --infodir=/usr/share/info \
		  --libdir=/usr/lib \
		  --libexecdir=/usr/lib \
		  --localstatedir=/var \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LINUX_ATM): 
	install -d -m0755 $(IDIR_LINUX_ATM)/usr/lib
	cp -f $(PKG_INSTALL_DIR)/usr/lib/libatm.so.1 $(IDIR_LINUX_ATM)/usr/lib
	$(RSTRIP) $(IDIR_LINUX_ATM)/
	$(IPKG_BUILD) $(IDIR_LINUX_ATM) $(PACKAGE_DIR)

$(IPKG_BR2684CTL): 
	install -d -m0755 $(IDIR_BR2684CTL)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/br2684ctl $(IDIR_BR2684CTL)/usr/sbin/
	install -d -m0755 $(IDIR_BR2684CTL)/etc/hotplug.d/net
	install -m0644 ./files/br2684.init $(IDIR_BR2684CTL)/etc/hotplug.d/net/30-br2684
	$(RSTRIP) $(IDIR_BR2684CTL)/
	$(IPKG_BUILD) $(IDIR_BR2684CTL) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libatm.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/atm{,d,sap}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libatm.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libatm.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/atm{,d,sap}.h \
		$(STAGING_DIR)/usr/lib/libatm.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

