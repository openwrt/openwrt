# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wifidog
PKG_VERSION:=1.1.3_beta2
PKG_RELEASE:=1
PKG_MD5SUM:=ba1611c7eb959f9c6f93e12b97d70b82

PKG_SOURCE_URL:= @SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WIFIDOG,wifidog,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
		ac_cv_func_setvbuf_reversed=no \
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.built
	
$(IPKG_WIFIDOG):
	install -m0755 -d $(IDIR_WIFIDOG)/etc/init.d
	install -m0755 ./files/$(PKG_NAME).init $(IDIR_WIFIDOG)/etc/init.d/S65wifidog
	install -m0644 ./files/wifidog.conf $(IDIR_WIFIDOG)/etc/
	install -m0755 -d $(IDIR_WIFIDOG)/usr/bin
	install -m0755 -d $(IDIR_WIFIDOG)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/scripts/init.d/wifidog $(IDIR_WIFIDOG)/usr/bin/wifidog-init
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/wifidog $(IDIR_WIFIDOG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/wdctl $(IDIR_WIFIDOG)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libhttpd.so* $(IDIR_WIFIDOG)/usr/lib/
	$(RSTRIP) $(IDIR_WIFIDOG)
	$(IPKG_BUILD) $(IDIR_WIFIDOG) $(PACKAGE_DIR)

