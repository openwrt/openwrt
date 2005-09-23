# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pptpd
PKG_VERSION:=1.2.3
PKG_RELEASE:=2
PKG_MD5SUM:=a521e40ca304b0c125cc25f9b9d03324

PKG_SOURCE_URL:=@SF/poptop
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PPTPD,pptpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  --with-pppd-ip-alloc \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS) -DSBINDIR=\\\"/usr/sbin\\\"" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install" \
		all install
	touch $@

$(IPKG_PPTPD):
	install -d -m0755 $(IDIR_PPTPD)/etc
	install -m0644 ./files/pptpd.conf $(IDIR_PPTPD)/etc/
	install -d -m0755 $(IDIR_PPTPD)/etc/init.d
	install -m0755 ./files/pptpd.init $(IDIR_PPTPD)/etc/init.d/pptpd
	install -d -m0755 $(IDIR_PPTPD)/etc/ppp
	install -m0644 ./files/options.pptpd $(IDIR_PPTPD)/etc/ppp/
	install -d -m0755 $(IDIR_PPTPD)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/pptpd $(IDIR_PPTPD)/usr/sbin/
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/pptpctrl $(IDIR_PPTPD)/usr/sbin/
	$(RSTRIP) $(IDIR_PPTPD)
	$(IPKG_BUILD) $(IDIR_PPTPD) $(PACKAGE_DIR)
