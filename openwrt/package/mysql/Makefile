# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mysql
PKG_VERSION:=4.0.24
PKG_RELEASE:=2
PKG_MD5SUM:=408d3001ed715ddc90009c247e548638

PKG_SOURCE_URL:=\
	http://mysql.he.net/Downloads/MySQL-4.0/ \
	http://www.linorg.usp.br/mysql/Downloads/MySQL-4.0/ \
	http://mysql.planetmirror.com/Downloads/MySQL-4.0/ \
	http://mysql.holywar.net/Downloads/MySQL-4.0/ \
	http://netmirror.org/mirror/mysql.com/Downloads/MySQL-4.0/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBMYSQLCLIENT:=$(PACKAGE_DIR)/libmysqlclient_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBMYSQLCLIENT:=$(PKG_BUILD_DIR)/ipkg/libmysqlclient
INFO_LIBMYSQLCLIENT:=$(IPKG_STATE_DIR)/info/libmysqlclient.list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
		touch configure.in; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch config.h.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C "$(PKG_BUILD_DIR)/libmysql" \
		CC="$(HOSTCC)" \
		LINK="$(HOSTCC) -o conf_to_src -lc" \
		CFLAGS="" \
		conf_to_src
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="include libmysql" \
		all
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/mysql/libmysqlclient.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		SUBDIRS="include libmysql" \
		install
	rm -rf $(STAGING_DIR)/usr/lib/mysql/libmysqlclient.la

$(IPKG_LIBMYSQLCLIENT): $(STAGING_DIR)/usr/lib/mysql/libmysqlclient.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBMYSQLCLIENT) ./ipkg/libmysqlclient.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBMYSQLCLIENT)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/mysql/libmysqlclient.so.* $(IDIR_LIBMYSQLCLIENT)/usr/lib/
	$(STRIP) $(IDIR_LIBMYSQLCLIENT)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBMYSQLCLIENT) $(PACKAGE_DIR)

$(INFO_LIBMYSQLCLIENT): $(IPKG_LIBMYSQLCLIENT)
	$(IPKG) install $(IPKG_LIBMYSQLCLIENT)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBMYSQLCLIENT)
install: $(INFO_LIBMYSQLCLIENT)

clean:
	-$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		SUBDIRS="include libmysql" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBMYSQLCLIENT)
