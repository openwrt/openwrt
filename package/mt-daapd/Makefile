# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mt-daapd
PKG_VERSION:=0.2.4
PKG_RELEASE:=1
PKG_MD5SUM:=2e1cdbe6b94ef153e915806f80a28dca

PKG_SOURCE_URL:=@SF/mt-daapd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MT_DAAPD,mt-daapd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_func_setpgrp_void=yes \
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
			--enable-mdns \
			--enable-howl \
			--with-id3tag="$(STAGING_DIR)/usr" \
			--with-gdbm-includes="$(STAGING_DIR)/usr/include" \
			--with-gdbm-libs="$(STAGING_DIR)/usr/lib" \
			--with-howl-includes="$(STAGING_DIR)/usr/include/howl" \
			--with-howl-libs="$(STAGING_DIR)/usr/lib" \
			--without-static-libs \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_MT_DAAPD):
	install -m0755 -d $(IDIR_MT_DAAPD)/etc
	install -m0644 $(PKG_BUILD_DIR)/contrib/mt-daapd.conf $(IDIR_MT_DAAPD)/etc/
	install -m0644 $(PKG_BUILD_DIR)/contrib/mt-daapd.playlist $(IDIR_MT_DAAPD)/etc/
	install -m0755 -d $(IDIR_MT_DAAPD)/etc/init.d
	install -m0755 ./files/mt-daapd.init $(IDIR_MT_DAAPD)/etc/init.d/mt-daapd
	install -m0755 -d $(IDIR_MT_DAAPD)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/mt-daapd $(IDIR_MT_DAAPD)/usr/share/
	install -m0755 -d $(IDIR_MT_DAAPD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/mt-daapd $(IDIR_MT_DAAPD)/usr/sbin/
	$(RSTRIP) $(IDIR_MT_DAAPD)
	$(IPKG_BUILD) $(IDIR_MT_DAAPD) $(PACKAGE_DIR)

