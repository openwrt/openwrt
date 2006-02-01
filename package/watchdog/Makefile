# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=watchdog
PKG_VERSION:=5.2.4
PKG_RELEASE:=1
PKG_MD5SUM:=c6ac132d92110eb2c4670d4f684105c3

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/w/watchdog
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WATCHDOG,watchdog,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
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
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS)" \
		INCLS="-I. -I$(STAGING_DIR)/usr/include" \
		LIBS="-L$(STAGING_DIR)/usr/lib" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_WATCHDOG):
	install -d -m0755 $(IDIR_WATCHDOG)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/watchdog.conf $(IDIR_WATCHDOG)/etc/
	install -d -m0755 $(IDIR_WATCHDOG)/etc/init.d
	install -m0755 ./files/watchdog.init $(IDIR_WATCHDOG)/etc/init.d/watchdog
	ln -sf watchdog $(IDIR_WATCHDOG)/etc/init.d/S90watchdog
	install -d -m0755 $(IDIR_WATCHDOG)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/watchdog $(IDIR_WATCHDOG)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/wd_keepalive $(IDIR_WATCHDOG)/usr/sbin/
	$(RSTRIP) $(IDIR_WATCHDOG)
	$(IPKG_BUILD) $(IDIR_WATCHDOG) $(PACKAGE_DIR)
