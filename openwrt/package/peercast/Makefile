# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=peercast
PKG_VERSION:=0.1214
PKG_RELEASE:=1
PKG_MD5SUM:=4e8449cde7135ad97f788e67e1bd64ee

PKG_SOURCE_URL:=http://www.cti.ecp.fr/~beauxir5/peercast
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PEERCAST,peercast,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -nostdinc++ -nodefaultlibs" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-nodefaultlibs -luClibc++ -lpthread" \
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
			--disable-static \
			--with-gnu-ld \
			--enable-staticbin \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_PEERCAST):
	install -m0755 -d $(IDIR_PEERCAST)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/peercast $(IDIR_PEERCAST)/usr/sbin/
	install -m0755 -d $(IDIR_PEERCAST)/usr/share/peercast/html
	$(CP) $(PKG_INSTALL_DIR)/usr/share/peercast/html/en $(IDIR_PEERCAST)/usr/share/peercast/html/
	install -m0755 -d $(IDIR_PEERCAST)/etc
	install -m0644 ./files/peercast.ini $(IDIR_PEERCAST)/etc/
	install -m0755 -d $(IDIR_PEERCAST)/etc/init.d
	install -m0755 ./files/peercast.init $(IDIR_PEERCAST)/etc/init.d/peercast
	ln -sf peercast $(IDIR_PEERCAST)/etc/init.d/S60peercast
	$(RSTRIP) $(IDIR_PEERCAST)
	$(IPKG_BUILD) $(IDIR_PEERCAST) $(PACKAGE_DIR)
