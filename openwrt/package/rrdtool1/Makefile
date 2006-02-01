# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rrdtool1
PKG_VERSION:=1.0.50
PKG_RELEASE:=1
PKG_MD5SUM:=c466e2e7df95fa8e318e46437da87686

PKG_SOURCE_URL:=http://people.ee.ethz.ch/~oetiker/webtools/rrdtool/pub/rrdtool-1.0.x
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/rrdtool-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBRRD1,librrd1,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRDCGI1,rrdcgi1,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRDTOOL1,rrdtool1,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{status,cache} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_path_PERL=no \
		rd_cv_ieee_works=yes \
		shrext_cmds=".so" \
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
			--enable-shared=yes \
			--enable-static=yes \
			--disable-rpath \
			--with-gnu-ld \
			--enable-local-zlib \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		shrext_cmds=".so" \
		all install
	touch $@

$(IPKG_RRDTOOL1):
	install -d -m0755 $(IDIR_RRDTOOL1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdtool $(IDIR_RRDTOOL1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdupdate $(IDIR_RRDTOOL1)/usr/bin/
	$(RSTRIP) $(IDIR_RRDTOOL1)
	$(IPKG_BUILD) $(IDIR_RRDTOOL1) $(PACKAGE_DIR)

$(IPKG_RRDCGI1):
	install -d -m0755 $(IDIR_RRDCGI1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/rrdcgi $(IDIR_RRDCGI1)/usr/bin/
	$(RSTRIP) $(IDIR_RRDCGI1)
	$(IPKG_BUILD) $(IDIR_RRDCGI1) $(PACKAGE_DIR)

$(IPKG_LIBRRD1):
	install -d -m0755 $(IDIR_LIBRRD1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librrd.so.* $(IDIR_LIBRRD1)/usr/lib/
	$(RSTRIP) $(IDIR_LIBRRD1)
	$(IPKG_BUILD) $(IDIR_LIBRRD1) $(PACKAGE_DIR)

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
