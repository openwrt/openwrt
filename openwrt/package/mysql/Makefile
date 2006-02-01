# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=mysql
PKG_VERSION:=5.0.18
PKG_RELEASE:=1
PKG_MD5SUM:=f18153b0239aaa03fc5a751f2d82cb71

PKG_SOURCE_URL:=\
	http://mysql.planetmirror.com/Downloads/MySQL-5.0/ \
	http://mysql.he.net/Downloads/MySQL-5.0/ \
	http://www.linorg.usp.br/mysql/Downloads/MySQL-5.0/ \
	http://mysql.holywar.net/Downloads/MySQL-5.0/

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBMYSQLCLIENT,libmysqlclient,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
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
		$(DISABLE_LARGEFILE) \
		$(DISABLE_NLS) \
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
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	$(MAKE) -C "$(PKG_BUILD_DIR)/libmysql" \
		CC="$(HOSTCC)" \
		LINK="$(HOSTCC) -o conf_to_src -lc" \
		CFLAGS="" \
		conf_to_src
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="libmysql" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="scripts" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		bin_SCRIPTS="mysql_config" \
		install
	touch $@

$(IPKG_LIBMYSQLCLIENT):
	install -d -m0755 $(IDIR_LIBMYSQLCLIENT)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/mysql/libmysqlclient.so.* $(IDIR_LIBMYSQLCLIENT)/usr/lib/
	$(RSTRIP) $(IDIR_LIBMYSQLCLIENT)
	$(IPKG_BUILD) $(IDIR_LIBMYSQLCLIENT) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/mysql/libmysqlclient.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mysql_config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/mysql $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/mysql $(STAGING_DIR)/usr/lib/
	rm -f $(STAGING_DIR)/usr/lib/mysql/libmysqlclient.la
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/mysql/libmysqlclient.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/bin/mysql_config \
	  $(STAGING_DIR)/usr/include/mysql \
	  $(STAGING_DIR)/usr/lib/mysql \

compile-targets: install-dev
clean-targets: uninstall-dev
