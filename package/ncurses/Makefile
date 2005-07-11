# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=5.2
PKG_RELEASE:=6
PKG_MD5SUM:=464d6a49010cf2a6eb9ce59a264d4d47

PKG_SOURCE_URL:=\
	ftp://ftp.gnu.org/pub/gnu/$(PKG_NAME) \
	ftp://gatekeeper.dec.com/pub/GNU/$(PKG_NAME) \
        ftp://ftp.uu.net/archive/systems/gnu/$(PKG_NAME) \
        ftp://ftp.eu.uu.net/pub/gnu/$(PKG_NAME) \
        ftp://ftp.funet.fi/pub/gnu/prep/$(PKG_NAME) \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/$(PKG_NAME) \
        ftp://ftp.digex.net/pub/gnu/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBNCURSES,libncurses,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.prepared
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		./configure \
		--target=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
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
		--enable-echo \
		--enable-const \
		--enable-overwrite \
		--disable-rpath \
		--without-ada \
		--without-cxx \
		--without-cxx-binding \
		--without-debug \
		--without-profile \
		--with-progs \
		--with-normal \
		--without-shared \
		--with-terminfo-dirs=/usr/share/terminfo \
		--with-default-terminfo-dir=/usr/share/terminfo \
	);
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		libs install.progs
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_linux_vers="2" \
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
		--enable-echo \
		--enable-const \
		--enable-overwrite \
		--disable-rpath \
		--without-ada \
		--without-cxx \
		--without-cxx-binding \
		--without-debug \
		--without-profile \
		--without-progs \
		--with-normal \
		--with-shared \
		--with-terminfo-dirs=/usr/share/terminfo \
		--with-default-terminfo-dir=/usr/share/terminfo \
	);
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BUILD_CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
		HOSTCCFLAGS="\$$(CPPFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		libs install.libs install.data
	touch $@

$(IPKG_LIBNCURSES):
	install -d -m0755 $(IDIR_LIBNCURSES)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel}*.so.* $(IDIR_LIBNCURSES)/usr/lib/
	install -d -m0755 $(IDIR_LIBNCURSES)/usr/share/terminfo
	for f in a/ansi d/dumb l/linux s/screen v/vt100 v/vt102 x/xterm x/xterm-color; do \
		install -d -m0755 $(IDIR_LIBNCURSES)/usr/share/terminfo/`dirname $${f}`; \
		cp -fpR $(PKG_INSTALL_DIR)/usr/share/terminfo/$${f} \
			$(IDIR_LIBNCURSES)/usr/share/terminfo/$${f}; \
	done
	$(RSTRIP) $(IDIR_LIBNCURSES)
	$(IPKG_BUILD) $(IDIR_LIBNCURSES) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libncurses.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/curses.h $(STAGING_DIR)/usr/include/
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/{ncurses,panel}.h $(STAGING_DIR)/usr/include/
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/panel.h $(STAGING_DIR)/usr/include/
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/term{,cap}.h $(STAGING_DIR)/usr/include/
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/unctrl.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libcurses.so $(STAGING_DIR)/usr/lib/
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/lib{ncurses,panel}.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libncurses.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/curses.h \
		$(STAGING_DIR)/usr/include/{ncurses,panel}.h \
		$(STAGING_DIR)/usr/include/panel.h \
		$(STAGING_DIR)/usr/include/term{,cap}.h \
		$(STAGING_DIR)/usr/include/unctrl.h \
		$(STAGING_DIR)/usr/lib/libcurses.so \
		$(STAGING_DIR)/usr/lib/lib{ncurses,panel}.{a,so*} \

compile: install-dev
clean: uninstall-dev

