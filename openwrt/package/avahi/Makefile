# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=avahi
PKG_VERSION:=0.6.10
PKG_RELEASE:=1
PKG_MD5SUM:=cfb9e05c138dff4c430a35eee6229e8a

PKG_SOURCE_URL:=http://avahi.org/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AVAHI_DAEMON,avahi-daemon,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,AVAHI_DNSCONFD,avahi-dnsconfd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LIBAVAHI,libavahi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DNDEBUG" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		ac_cv_func_malloc_0_nonnull=yes \
		ac_cv_func_realloc_0_nonnull=yes \
		ac_cv_func_memcmp_working=yes \
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
			--disable-rpath \
			--with-gnu-ld \
			--disable-glib \
			--disable-qt3 \
			--disable-qt4 \
			--disable-gtk \
			--disable-dbus \
			--enable-expat \
			--enable-libdaemon \
			--disable-python \
			--disable-pygtk \
			--disable-python-dbus \
			--disable-mono \
			--disable-monodoc \
			--disable-doxygen-doc \
			--disable-doxygen-dot \
			--disable-doxygen-man \
			--disable-doxygen-rtf \
			--disable-doxygen-xml \
			--disable-doxygen-chm \
			--disable-doxygen-chi \
			--disable-doxygen-html \
			--disable-doxygen-ps \
			--disable-doxygen-pdf \
			--disable-xmltoman \
			--with-distro=none \
			--with-avahi-user=nobody \
			--with-avahi-group=nogroup \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_AVAHI_DAEMON):
	install -d -m0755 $(IDIR_AVAHI_DAEMON)/etc/avahi
	install -m0644 ./files/avahi-daemon.conf $(IDIR_AVAHI_DAEMON)/etc/avahi/
	install -d -m0755 $(IDIR_AVAHI_DAEMON)/etc/avahi/services
	install -m0644 ./files/service-http $(IDIR_AVAHI_DAEMON)/etc/avahi/services/http.service
	install -m0644 ./files/service-ssh $(IDIR_AVAHI_DAEMON)/etc/avahi/services/ssh.service
	install -d -m0755 $(IDIR_AVAHI_DAEMON)/etc/init.d
	install -m0755 ./files/avahi-daemon.init $(IDIR_AVAHI_DAEMON)/etc/init.d/avahi-daemon
	install -d -m0755 $(IDIR_AVAHI_DAEMON)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/avahi-daemon $(IDIR_AVAHI_DAEMON)/usr/sbin/
	$(RSTRIP) $(IDIR_AVAHI_DAEMON)
	$(IPKG_BUILD) $(IDIR_AVAHI_DAEMON) $(PACKAGE_DIR)

$(IPKG_AVAHI_DNSCONFD):
	install -d -m0755 $(IDIR_AVAHI_DNSCONFD)/etc/avahi
	$(CP) $(PKG_INSTALL_DIR)/etc/avahi/avahi-dnsconfd.action $(IDIR_AVAHI_DNSCONFD)/etc/avahi/
	install -d -m0755 $(IDIR_AVAHI_DNSCONFD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/avahi-dnsconfd $(IDIR_AVAHI_DNSCONFD)/usr/sbin/
	$(RSTRIP) $(IDIR_AVAHI_DNSCONFD)
	$(IPKG_BUILD) $(IDIR_AVAHI_DNSCONFD) $(PACKAGE_DIR)

$(IPKG_LIBAVAHI):
	install -d -m0755 $(IDIR_LIBAVAHI)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-common.so.* $(IDIR_LIBAVAHI)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-core.so.* $(IDIR_LIBAVAHI)/usr/lib/
	$(RSTRIP) $(IDIR_LIBAVAHI)
	$(IPKG_BUILD) $(IDIR_LIBAVAHI) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/avahi.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/avahi-common $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/avahi-core $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-common.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-common.so* $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-core.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libavahi-core.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/avahi-core.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/avahi-core.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/avahi-core.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/avahi.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/avahi-common \
		$(STAGING_DIR)/usr/include/avahi-core \
		$(STAGING_DIR)/usr/lib/libavahi-common.a \
		$(STAGING_DIR)/usr/lib/libavahi-common.so* \
		$(STAGING_DIR)/usr/lib/libavahi-core.a \
		$(STAGING_DIR)/usr/lib/libavahi-core.so* \
		$(STAGING_DIR)/usr/lib/pkgconfig/avahi-core.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

