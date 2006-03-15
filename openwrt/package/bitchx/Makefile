# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bitchx
PKG_VERSION:=1.1
PKG_RELEASE:=1
PKG_MD5SUM:=611d2dda222f00c10140236f4c331572

PKG_SOURCE_URL:=http://bitchx.org/files/source/
PKG_SOURCE:=ircii-pana-$(PKG_VERSION)-final.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/BitchX
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BITCHX,bitchx,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		--target=$(GNU_HOST_NAME) \
		--host=mipsel-linux-uclibc-gcc \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	mkdir -p $(PKG_INSTALL_DIR)/tmp
	mkdir -p $(PKG_INSTALL_DIR)/etc/init.d
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx
	mkdir -p $(PKG_INSTALL_DIR)/usr/share/man/man1
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/translation
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/plugins
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/help
	install -c $(PKG_BUILD_DIR)/source/BitchX $(PKG_INSTALL_DIR)/usr/bin/BitchX-1.1-final
	rm -f $(PKG_INSTALL_DIR)/usr/bin/BitchX
	ln -s /usr/bin/BitchX-1.1-final $(PKG_INSTALL_DIR)/usr/bin/BitchX 
	install -c $(PKG_BUILD_DIR)/BitchX.help $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.help
	install -c $(PKG_BUILD_DIR)/BitchX.ircnames $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.ircnames
	install -c $(PKG_BUILD_DIR)/BitchX.quit $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.quit
	install -c $(PKG_BUILD_DIR)/BitchX.reasons $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.kick
	install -c $(PKG_BUILD_DIR)/source/wserv $(PKG_INSTALL_DIR)/usr/lib/bx/wserv
	install -c $(PKG_BUILD_DIR)/source/scr-bx $(PKG_INSTALL_DIR)/usr/bin/scr-bx
	cp -fpR $(PKG_BUILD_DIR)/script $(PKG_INSTALL_DIR)/usr/lib/bx/
	cp -fpR $(PKG_BUILD_DIR)/translation $(PKG_INSTALL_DIR)/usr/lib/bx/
	bzip2 $(PKG_INSTALL_DIR)/usr/lib/bx/translation/*
	cp -f ./files/bitchxrc $(PKG_INSTALL_DIR)/tmp/.bitchxrc
	cp -pf ./files/S98bitchxrc $(PKG_INSTALL_DIR)/etc/init.d/S98bitchxrc
	touch $@

$(IPKG_BITCHX):
	install -d -m0755 $(IDIR_BITCHX)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/* $(IDIR_BITCHX)/usr/bin
	install -d -m0755 $(IDIR_BITCHX)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/* $(IDIR_BITCHX)/usr/lib
	install -d $(IDIR_BITCHX)/etc/init.d
	cp -fp $(PKG_INSTALL_DIR)/etc/init.d/S98bitchxrc $(IDIR_BITCHX)/etc/init.d
	install -d -m0755 $(IDIR_BITCHX)/tmp
	cp -fp $(PKG_INSTALL_DIR)/tmp/.bitchxrc $(IDIR_BITCHX)/tmp
	$(RSTRIP) $(IDIR_BITCHX)
	$(IPKG_BUILD) $(IDIR_BITCHX) $(PACKAGE_DIR)
