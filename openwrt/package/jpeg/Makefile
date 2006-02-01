# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=jpeg
PKG_VERSION:=6b
PKG_RELEASE:=1
PKG_MD5SUM:=dbd5f3b47ed13132f04c685d608a7547

PKG_SOURCE_URL:=http://www.ijg.org/files/ \
	ftp://ftp.uu.net/graphics/jpeg/
PKG_SOURCE:=$(PKG_NAME)src.v$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBJPEG,libjpeg,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/{include,lib}
	$(MAKE) -C $(PKG_BUILD_DIR) \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) \
		prefix="$(PKG_INSTALL_DIR)/usr" \
		exec_prefix="$(PKG_INSTALL_DIR)/usr" \
		install-headers install-lib
	touch $@

$(IPKG_LIBJPEG):
	install -d -m0755 $(IDIR_LIBJPEG)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjpeg.so.* $(IDIR_LIBJPEG)/usr/lib/
	$(RSTRIP) $(IDIR_LIBJPEG)
	$(IPKG_BUILD) $(IDIR_LIBJPEG) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libjpeg.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/jpeglib.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/j{config,error,morecfg}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjpeg.{a,so*} $(STAGING_DIR)/usr/lib/

install-dev: $(STAGING_DIR)/usr/lib/libjpeg.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/jpeglib.h \
	  $(STAGING_DIR)/usr/include/j{config,error,morecfg}.h \
	  $(STAGING_DIR)/usr/lib/libjpeg.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev

