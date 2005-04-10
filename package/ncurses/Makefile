# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=5.2
PKG_RELEASE:=6
PKG_MD5SUM:=464d6a49010cf2a6eb9ce59a264d4d47

PKG_SOURCE_URL:=ftp://ftp.gnu.org/pub/gnu/$(PKG_NAME) \
	ftp://gatekeeper.dec.com/pub/GNU/$(PKG_NAME) \
        ftp://ftp.uu.net/archive/systems/gnu/$(PKG_NAME) \
        ftp://ftp.eu.uu.net/pub/gnu/$(PKG_NAME) \
        ftp://ftp.funet.fi/pub/gnu/prep/$(PKG_NAME) \
        ftp://ftp.leo.org/pub/comp/os/unix/gnu/$(PKG_NAME) \
        ftp://ftp.digex.net/pub/gnu/$(PKG_NAME) 

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_linux_vers="2" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
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
	
$(STAGING_DIR)/usr/include/ncurses.h: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(PKG_IPK): $(PKG_BUILD_DIR)/.built
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) libncurses.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -a $(PKG_BUILD_DIR)/lib/lib*.so.* $(PKG_IPK_DIR)/usr/lib/
	mkdir -p $(PKG_IPK_DIR)/usr/share/terminfo
	for f in a/ansi d/dumb l/linux s/screen v/vt100 x/xterm x/xterm-color; do \
		mkdir -p $(PKG_IPK_DIR)/usr/share/terminfo/`dirname $${f}`; \
		cp -fpR \
			$(PKG_BUILD_DIR)/install/usr/share/terminfo/$${f} \
			$(PKG_IPK_DIR)/usr/share/terminfo/$${f}; \
	done
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(STAGING_DIR)/usr/include/ncurses.h $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)
