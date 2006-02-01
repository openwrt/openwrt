# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libevent
PKG_VERSION:=1.1
PKG_RELEASE:=1
PKG_MD5SUM:=a5bd281aeb41bdaa48fbbf0495423d20

PKG_SOURCE_URL:=http://www.monkey.org/~provos/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBEVENT,libevent,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
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
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  --enable-shared \
		  --enable-static \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBEVENT):
	install -d -m0755 $(IDIR_LIBEVENT)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent-$(PKG_VERSION).so.* $(IDIR_LIBEVENT)/usr/lib/
	$(RSTRIP) $(IDIR_LIBEVENT)
	$(IPKG_BUILD) $(IDIR_LIBEVENT) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libevent.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/event.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent.{a,so} $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libevent-$(PKG_VERSION).so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libevent.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/event.h \
	  $(STAGING_DIR)/usr/lib/libevent.{a,so} \
	  $(STAGING_DIR)/usr/lib/libevent-$(PKG_VERSION).so* \

compile-targets: install-dev
clean-targets: uninstall-dev
