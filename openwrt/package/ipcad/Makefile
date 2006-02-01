# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipcad
PKG_VERSION:=3.7
PKG_RELEASE:=1
PKG_MD5SUM:=34c542c99593dfe9d5b408deffcfe70e

PKG_SOURCE_URL:=@SF/ipcad
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPCAD,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		touch cfglex.c cslex.c; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
			--disable-rpath \
			--with-gnu-ld \
			--with-psrc=pcap \
			--with-pcap-include="$(STAGING_DIR)/usr/include" \
			--with-pcap-libraries="$(STAGING_DIR)/usr/lib" \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR=$(PKG_INSTALL_DIR) \
		all install
	touch $@

$(IPKG_IPCAD):
	install -d -m0755 $(IDIR_IPCAD)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/ipcad.conf $(IDIR_IPCAD)/etc/
	install -d -m0755 $(IDIR_IPCAD)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ipcad $(IDIR_IPCAD)/usr/bin/
	$(RSTRIP) $(IDIR_IPCAD)
	$(IPKG_BUILD) $(IDIR_IPCAD) $(PACKAGE_DIR)
