# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := mysql
PKG_VERSION := 4.0.24
PKG_RELEASE := 2
PKG_MD5SUM := 408d3001ed715ddc90009c247e548638

PKG_SOURCE_SITE := \
	http://mysql.he.net/Downloads/MySQL-4.0/ \
	http://www.linorg.usp.br/mysql/Downloads/MySQL-4.0/ \
	http://mysql.planetmirror.com/Downloads/MySQL-4.0/ \
	http://mysql.holywar.net/Downloads/MySQL-4.0/ \
	http://netmirror.org/mirror/mysql.com/Downloads/MySQL-4.0/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/libmysqlclient_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		touch configure.in; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch config.h.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		OPTIMIZE_CFLAGS="$(TARGET_CFLAGS)" \
		OPTIMIZE_CXXFLAGS="$(TARGET_CFLAGS)" \
		ac_atomic_add=yes \
		ac_atomic_sub=yes \
		ac_cv_sys_restartable_syscalls=yes \
		ac_cv_conv_longlong_to_float=yes \
		mysql_cv_compress=yes \
		mysql_cv_gethostname_style=glibc2 \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/ \
		--exec-prefix=/ \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--enable-shared \
		--enable-static \
		--disable-assembler \
		--with-pthread \
		--without-raid \
		--with-unix-socket-path=/tmp \
		--without-libwrap \
		--without-pstack \
		--with-low-memory \
		--without-server \
		--without-embedded-server \
		--without-query-cache \
		--without-mysqlfs \
		--without-vio \
		--without-openssl \
		--without-docs \
		--without-bench \
		--without-readline \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libmysql/.libs/libmysqlclient.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C "$(PKG_BUILD_DIR)/libmysql" \
		CC="$(HOSTCC)" \
		LINK="$(HOSTCC) -o conf_to_src -lc" \
		CFLAGS="" \
		conf_to_src
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="include libmysql" \
		all

$(STAGING_DIR)/include/mysql/mysql.h: $(PKG_BUILD_DIR)/libmysql/.libs/libmysqlclient.so
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		SUBDIRS="include libmysql" \
		install

$(PKG_IPK): $(PKG_BUILD_DIR)/libmysql/.libs/libmysqlclient.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) libmysqlclient.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp $< $(PKG_IPK_DIR)/usr/lib/
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libmysqlclient.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/include/mysql/mysql.h $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libmysqlclient.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
