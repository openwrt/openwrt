# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ssltunnel
PKG_VERSION:=1.15
PKG_RELEASE:=1
PKG_MD5SUM:=bfe529edb9b6c97bc03d213676b1f35d

PKG_SOURCE_URL:=http://www.hsc.fr/ressources/outils/ssltunnel/download
PKG_SOURCE:=${PKG_NAME}-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SSLTUNNEL,ssltunnel,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS)" INCLS="-I. -I$(STAGING_DIR)/usr/include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_SSLTUNNEL):
	install -m0755 -d $(IDIR_SSLTUNNEL)/usr/bin
	install -m0755 -d $(IDIR_SSLTUNNEL)/usr/sbin
	install -m0755 -d $(IDIR_SSLTUNNEL)/usr/lib
	install -m0755 $(PKG_INSTALL_DIR)/usr/bin/pppclient $(IDIR_SSLTUNNEL)/usr/bin/
	install -m0755 $(PKG_INSTALL_DIR)/usr/sbin/pppwho $(IDIR_SSLTUNNEL)/usr/sbin/
	install -m0755 $(PKG_INSTALL_DIR)/usr/lib/pppserver $(IDIR_SSLTUNNEL)/usr/lib/
	$(RSTRIP) $(IDIR_SSLTUNNEL)
	$(IPKG_BUILD) $(IDIR_SSLTUNNEL) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
