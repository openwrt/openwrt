# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=knock
PKG_VERSION:=0.5
PKG_RELEASE:=1
PKG_MD5SUM:=ca09d61458974cff90a700aba6120891

PKG_SOURCE_URL:=http://www.zeroflux.org/knock/files/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KNOCK,knock,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,KNOCKD,knockd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_lib_pcap_pcap_open_live=yes \
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
			--sysconfdir=/etc/kismet \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--disable-static \
			--disable-rpath \
			--with-gnu-ld \
	);
	touch $@
	
$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@
	
$(IPKG_KNOCK):
	install -d -m0755 $(IDIR_KNOCK)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/knock $(IDIR_KNOCK)/usr/bin/
	$(RSTRIP) $(IDIR_KNOCK)
	$(IPKG_BUILD) $(IDIR_KNOCK) $(PACKAGE_DIR)

$(IPKG_KNOCKD):
	install -d -m0755 $(IDIR_KNOCKD)/etc
	install -m0600 $(PKG_INSTALL_DIR)/etc/knockd.conf $(IDIR_KNOCKD)/etc/
	install -d -m0755 $(IDIR_KNOCKD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/knockd $(IDIR_KNOCKD)/usr/sbin/
	$(RSTRIP) $(IDIR_KNOCKD)
	$(IPKG_BUILD) $(IDIR_KNOCKD) $(PACKAGE_DIR)
