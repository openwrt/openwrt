# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=quagga
PKG_VERSION:=0.98.5
PKG_RELEASE:=1
PKG_MD5SUM:=ec09c1ec624aea98e18aa19282666784

PKG_SOURCE_URL:=http://www.quagga.net/download/ \
                http://www.de.quagga.net/download/ \
                http://www.uk.quagga.net/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,QUAGGA,quagga,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_BGPD,quagga-bgpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_OSPFD,quagga-ospfd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_OSPF6D,quagga-ospf6d,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_RIPD,quagga-ripd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_RIPNGD,quagga-ripngd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_VTYSH,quagga-vtysh,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib \
		--localstatedir=/var/run/quagga \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc/quagga \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--disable-static \
		--enable-ipv6 \
		--enable-vtysh \
		--enable-user=quagga \
		--enable-group=quagga \
		--enable-multipath=8 \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR=$(PKG_INSTALL_DIR) \
		all install
	touch $@

$(IPKG_QUAGGA):
	install -d -m0755 $(IDIR_QUAGGA)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libzebra.so.* $(IDIR_QUAGGA)/usr/lib/
	install -d -m0755 $(IDIR_QUAGGA)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/zebra $(IDIR_QUAGGA)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/watchquagga $(IDIR_QUAGGA)/usr/sbin/
	# avoid /etc being set to 0750
	install -d -m0755 $(IDIR_QUAGGA)/etc/quagga/
	chmod 0750 $(IDIR_QUAGGA)/etc/quagga/
	install -d -m0755 $(IDIR_QUAGGA)/etc/init.d/
	install -m0755 ./files/quagga.init $(IDIR_QUAGGA)/etc/init.d/quagga
	ln -sf quagga $(IDIR_QUAGGA)/etc/init.d/S49quagga
	install -d -m0755 $(IDIR_QUAGGA)/var/run/quagga
	$(RSTRIP) $(IDIR_QUAGGA)
	$(IPKG_BUILD) $(IDIR_QUAGGA) $(PACKAGE_DIR)

$(IPKG_QUAGGA_BGPD):
	install -d -m0755 $(IDIR_QUAGGA_BGPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/bgpd $(IDIR_QUAGGA_BGPD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_BGPD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_BGPD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_OSPFD):
	install -d -m0755 $(IDIR_QUAGGA_OSPFD)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libospf.so.* $(IDIR_QUAGGA_OSPFD)/usr/lib/
	install -d -m0755 $(IDIR_QUAGGA_OSPFD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ospfd $(IDIR_QUAGGA_OSPFD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_OSPFD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_OSPFD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_OSPF6D):
	install -d -m0755 $(IDIR_QUAGGA_OSPF6D)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ospf6d $(IDIR_QUAGGA_OSPF6D)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_OSPF6D)
	$(IPKG_BUILD) $(IDIR_QUAGGA_OSPF6D) $(PACKAGE_DIR)

$(IPKG_QUAGGA_RIPD):
	install -d -m0755 $(IDIR_QUAGGA_RIPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ripd $(IDIR_QUAGGA_RIPD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_RIPD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_RIPD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_RIPNGD):
	install -d -m0755 $(IDIR_QUAGGA_RIPNGD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ripngd $(IDIR_QUAGGA_RIPNGD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_RIPNGD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_RIPNGD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_VTYSH):
	install -d -m0755 $(IDIR_QUAGGA_VTYSH)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/vtysh $(IDIR_QUAGGA_VTYSH)/usr/bin/
	$(RSTRIP) $(IDIR_QUAGGA_VTYSH)
	$(IPKG_BUILD) $(IDIR_QUAGGA_VTYSH) $(PACKAGE_DIR)
