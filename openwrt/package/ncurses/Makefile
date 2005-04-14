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
        ftp://ftp.digex.net/pub/gnu/$(PKG_NAME) \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBNCURSES:=$(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBNCURSES:=$(PKG_BUILD_DIR)/ipkg/lib$(PKG_NAME)
INFO_LIBNCURSES:=$(IPKG_STATE_DIR)/info/lib$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache config.status; \
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
		--with-terminfo-dirs=/usr/share/terminfo \
		--with-default-terminfo-dir=/usr/share/terminfo \
		--without-ada \
		--without-cxx \
		--without-cxx-binding \
		--without-progs \
		--without-profile \
		--without-debug \
		--disable-rpath \
		--enable-echo \
		--enable-const \
		--enable-overwrite \
		--with-shared \
		--with-normal \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		BUILD_CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
		HOSTCCFLAGS="\$$(CPPFLAGS)"
	mkdir -p $(PKG_BUILD_DIR)/terminfo
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_BUILD_DIR)/install" \
		install
	touch $(PKG_BUILD_DIR)/.built
	
$(STAGING_DIR)/usr/lib/libncurses.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(IPKG_LIBNCURSES): $(STAGING_DIR)/usr/lib/libncurses.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBNCURSES) lib$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBNCURSES)/usr/lib
	cp -fp $(STAGING_DIR)/usr/lib/lib{ncurses,panel}*.so.* $(IDIR_LIBNCURSES)/usr/lib/
	$(STRIP) $(IDIR_LIBNCURSES)/usr/lib/*.so*
	mkdir -p $(IDIR_LIBNCURSES)/usr/share/terminfo
	for f in a/ansi d/dumb l/linux s/screen v/vt100 x/xterm x/xterm-color; do \
		mkdir -p $(IDIR_LIBNCURSES)/usr/share/terminfo/`dirname $${f}`; \
		cp -fpR \
			$(PKG_BUILD_DIR)/install/usr/share/terminfo/$${f} \
			$(IDIR_LIBNCURSES)/usr/share/terminfo/$${f}; \
	done
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBNCURSES) $(PACKAGE_DIR)

$(INFO_LIBNCURSES): $(IPKG_LIBNCURSES)
	$(IPKG) install $(IPKG_LIBNCURSES)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBNCURSES)
install: $(INFO_LIBNCURSES)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBNCURSES)
