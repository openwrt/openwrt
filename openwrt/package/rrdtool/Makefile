# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rrdtool
PKG_VERSION:=1.2.11
PKG_RELEASE:=1
PKG_MD5SUM:=d61c5755cb77207f9ad3584b26e8bf08

PKG_SOURCE_URL:=http://people.ee.ethz.ch/~oetiker/webtools/rrdtool/pub
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBRRD,librrd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRDCGI,rrdcgi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRDTOOL,rrdtool,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/freetype2 -I$(STAGING_DIR)/usr/include/libart-2.0" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib " \
		LIBS="-lnotimpl" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		rd_cv_ieee_works=yes \
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
			--disable-rpath \
			--with-gnu-ld \
			--enable-rrdcgi \
			--disable-mmap \
			--disable-perl \
			--disable-tcl \
			--disable-python \
			--without-x \
			--with-rrd-default-font=/usr/share/rrdtool/fonts/DejaVuSansMono-Roman.ttf \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_RRDTOOL):
	install -d -m0755 $(IDIR_RRDTOOL)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdtool $(IDIR_RRDTOOL)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdupdate $(IDIR_RRDTOOL)/usr/bin/
	$(RSTRIP) $(IDIR_RRDTOOL)
	$(IPKG_BUILD) $(IDIR_RRDTOOL) $(PACKAGE_DIR)

$(IPKG_RRDCGI):
	install -d -m0755 $(IDIR_RRDCGI)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdcgi $(IDIR_RRDCGI)/usr/bin/
	$(RSTRIP) $(IDIR_RRDCGI)
	$(IPKG_BUILD) $(IDIR_RRDCGI) $(PACKAGE_DIR)

$(IPKG_LIBRRD):
	install -d -m0755 $(IDIR_LIBRRD)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librrd.so.* $(IDIR_LIBRRD)/usr/lib/
	install -d -m0755 $(IDIR_LIBRRD)/usr/share/rrdtool/fonts
	$(CP) $(PKG_BUILD_DIR)/src/DejaVuSansMono-Roman.ttf \
		$(IDIR_LIBRRD)/usr/share/rrdtool/fonts/
	$(RSTRIP) $(IDIR_LIBRRD)
	$(IPKG_BUILD) $(IDIR_LIBRRD) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/librrd.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/rrd.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librrd.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librrd.so* $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/librrd.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/rrd.h \
		$(STAGING_DIR)/usr/lib/librrd.a \
		$(STAGING_DIR)/usr/lib/librrd.so* \

compile-targets: install-dev
clean-targets: uninstall-dev
