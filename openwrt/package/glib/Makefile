# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := glib
PKG_VERSION := 1.2.10
PKG_RELEASE := 1
PKG_MD5SUM := 6fe30dad87c77b91b632def29dd69ef9

PKG_SOURCE_SITE := ftp://ftp.gtk.org/pub/gtk/v1.2
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/glib1.2_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
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
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
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
		--enable-shared=yes \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libglib.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  $(TARGET_CONFIGURE_OPTS)

$(STAGING_DIR)/lib/libglib.so: $(PKG_BUILD_DIR)/libglib.so
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  DESTDIR="$(STAGING_DIR)" \
	  install
	cp $(PKG_BUILD_DIR)/glibconfig-sysdefs.h $(STAGING_DIR)/usr/include/glib-1.2/

$(PKG_IPK): $(STAGING_DIR)/lib/libglib.so
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/glib glib.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/glib/usr/lib
	cp -a $(PKG_BUILD_DIR)/.libs/*.so* $(PKG_BUILD_DIR)/ipkg/glib/usr/lib
	cp -a $(PKG_BUILD_DIR)/*/.libs/*.so* $(PKG_BUILD_DIR)/ipkg/glib/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/glib/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/glib $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/glib.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libglib.so $(PKG_IPK)
install: $(PKG_ZLIB) $(IPKG_STATE_DIR)/info/glib.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libglib* $(STAGING_DIR)/include/glib*
	rm -f $(PACKAGE_DIR)/glib\*.ipk

