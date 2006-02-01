# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := glib
PKG_VERSION := 1.2.10
PKG_RELEASE := 1
PKG_MD5SUM := 6fe30dad87c77b91b632def29dd69ef9

PKG_SOURCE_URL := ftp://ftp.gtk.org/pub/gtk/v1.2 \
	http://ftp.gwdg.de/pub/misc/grafik/gimp/gtk/v1.2/ \
	http://ftp.esat.net/mirrors/ftp.gimp.org/pub/gtk/v1.2/ \
	http://www.mirrorservice.org/sites/ftp.gimp.org/pub/gtk/v1.2/
PKG_SOURCE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT := zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,GLIB1,glib1,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		glib_cv_sizeof_gmutex=24 \
		ac_cv_sizeof_char=1 \
		ac_cv_sizeof_short=2 \
		ac_cv_sizeof_int=4 \
		ac_cv_sizeof_long=4 \
		ac_cv_sizeof_long_long=8 \
		ac_cv_sizeof_void_p=4 \
		glib_cv_prog_cc_ansi_proto="no" \
		glib_cv_has__inline="yes" \
		glib_cv_has__inline__="yes" \
		glib_cv_hasinline="yes" \
		glib_cv_sane_realloc="yes" \
		glib_cv_va_copy="no" \
		glib_cv___va_copy="yes" \
		glib_cv_va_val_copy="yes" \
		glib_cv_rtldglobal_broken="no" \
		glib_cv_uscore="no" \
		ac_cv_func_getpwuid_r="yes" \
		glib_cv_func_pthread_mutex_trylock_posix="yes" \
		glib_cv_func_pthread_cond_timedwait_posix="yes" \
		glib_cv_sizeof_gmutex="24" \
		glib_cv_byte_contents_gmutex="0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0" \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --sbindir=/usr/sbin \
		  --libexecdir=/usr/lib \
		  --sysconfdir=/etc \
		  --datadir=/usr/share \
		  --localstatedir=/var \
		  --mandir=/usr/man \
		  --infodir=/usr/info \
		  $(DISABLE_NLS) \
		  $(DISABLE_LARGEFILE) \
		  --enable-shared=yes \
		  --enable-static=yes \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $@

$(IPKG_GLIB1):
	install -m0755 -d $(IDIR_GLIB1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libg{lib,module,thread}-1.2.so.* $(IDIR_GLIB1)/usr/lib/
	$(RSTRIP) $(IDIR_GLIB1)
	$(IPKG_BUILD) $(IDIR_GLIB1) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libglib.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/glib-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/glib-1.2 $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/glibconfig-sysdefs.h $(STAGING_DIR)/usr/include/glib-1.2/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libg{lib,module,thread}.{a,so} $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libg{lib,module,thread}-*.so.* $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/glib $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/g{lib,module,thread}.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	touch $(STAGING_DIR)/usr/lib/libglib.so

install-dev: $(STAGING_DIR)/usr/lib/libglib.so

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/bin/glib-config
	rm -rf $(STAGING_DIR)/usr/include/glib-1.2
	rm -rf $(STAGING_DIR)/usr/lib/libg{lib,module,thread}.{a,so}
	rm -rf $(STAGING_DIR)/usr/lib/libg{lib,module,thread}-*.so.*
	rm -rf $(STAGING_DIR)/usr/lib/glib
	rm -rf $(STAGING_DIR)/usr/lib/pkgconfig/g{lib,module,thread}.pc

compile-targets: install-dev
clean-targets: uninstall-dev
