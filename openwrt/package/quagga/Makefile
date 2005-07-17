# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=quagga
PKG_VERSION:=0.98.3
PKG_RELEASE:=1
PKG_MD5SUM:=68be5e911e4d604c0f5959338263356e

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
$(eval $(call PKG_template,LIBOSPF,libospf,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_OSPFD,quagga-ospfd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_OSPF6D,quagga-ospf6d,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_RIPD,quagga-ripd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_RIPNGD,quagga-ripngd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,QUAGGA_ZEBRA,quagga-zebra,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
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
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib \
		--localstatedir=/var \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--disable-static \
		--enable-ipv6 \
		--disable-vtysh \
		--enable-user=quagga \
		--enable-group=quagga \
		--sysconfdir=/etc/quagga \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR=$(PKG_INSTALL_DIR) \
		all install
	touch $@

$(IPKG_QUAGGA):
	install -d -m0755 $(IDIR_QUAGGA)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libzebra.so.* $(IDIR_QUAGGA)/usr/lib/
	$(RSTRIP) $(IDIR_QUAGGA)
	$(IPKG_BUILD) $(IDIR_QUAGGA) $(PACKAGE_DIR)

$(IPKG_QUAGGA_BGPD):
	install -d -m0755 $(IDIR_QUAGGA_BGPD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/bgpd $(IDIR_QUAGGA_BGPD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_BGPD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_BGPD) $(PACKAGE_DIR)

$(IPKG_LIBOSPF):
	install -d -m0755 $(IDIR_LIBOSPF)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libospf.so.* $(IDIR_LIBOSPF)/usr/lib/
	$(RSTRIP) $(IDIR_LIBOSPF)
	$(IPKG_BUILD) $(IDIR_LIBOSPF) $(PACKAGE_DIR)

$(IPKG_QUAGGA_OSPFD):
	install -d -m0755 $(IDIR_QUAGGA_OSPFD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ospfd $(IDIR_QUAGGA_OSPFD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_OSPFD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_OSPFD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_OSPF6D):
	install -d -m0755 $(IDIR_QUAGGA_OSPF6D)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ospfd $(IDIR_QUAGGA_OSPF6D)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_OSPF6D)
	$(IPKG_BUILD) $(IDIR_QUAGGA_OSPF6D) $(PACKAGE_DIR)

$(IPKG_QUAGGA_RIPD):
	install -d -m0755 $(IDIR_QUAGGA_RIPD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ripd $(IDIR_QUAGGA_RIPD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_RIPD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_RIPD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_RIPNGD):
	install -d -m0755 $(IDIR_QUAGGA_RIPNGD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ripngd $(IDIR_QUAGGA_RIPNGD)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_RIPNGD)
	$(IPKG_BUILD) $(IDIR_QUAGGA_RIPNGD) $(PACKAGE_DIR)

$(IPKG_QUAGGA_ZEBRA):
	install -d -m0755 $(IDIR_QUAGGA_ZEBRA)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/zebra $(IDIR_QUAGGA_ZEBRA)/usr/sbin/
	$(RSTRIP) $(IDIR_QUAGGA_ZEBRA)
	$(IPKG_BUILD) $(IDIR_QUAGGA_ZEBRA) $(PACKAGE_DIR)

