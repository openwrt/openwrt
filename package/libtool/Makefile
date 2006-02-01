# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libtool
PKG_VERSION:=1.5.14
PKG_RELEASE:=1
PKG_MD5SUM:=049bf67de9b0eb75cd943dafe3d749ec

PKG_SOURCE_URL:=@GNU/libtool
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBLTDL,libltdl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBLTDL):
	install -m0755 -d $(IDIR_LIBLTDL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libltdl.so.* $(IDIR_LIBLTDL)/usr/lib/
	$(RSTRIP) $(IDIR_LIBLTDL)
	$(IPKG_BUILD) $(IDIR_LIBLTDL) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libltdl.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ltdl.h $(STAGING_DIR)/usr/include/
	mkdir -p $(I_LIBLTDL_DEV)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libltdl.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libltdl.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/ltdl.h \
		$(STAGING_DIR)/usr/lib/libltdl.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
