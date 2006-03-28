# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=fuse
PKG_VERSION:=2.5.2
PKG_RELEASE:=1
PKG_MD5SUM:=ea565debe6c7486963bef05c45c50361

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBFUSE,libfuse,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FUSE_UTILS,fuse-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); \
		rm -rf config.{cache,status} ; \
		touch configure.in ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch include/config.h.in ; \
		touch configure ; \
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
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--disable-kernel-module \
			--enable-lib \
			--enable-util \
			--disable-example \
			--disable-auto-modprobe \
			--with-kernel=$(LINUX_DIR) \
			--disable-mtab \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBFUSE):
	mkdir -p $(IDIR_LIBFUSE)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(IDIR_LIBFUSE)/usr/lib/
	$(RSTRIP) $(IDIR_LIBFUSE)
	$(IPKG_BUILD) $(IDIR_LIBFUSE) $(PACKAGE_DIR)

$(IPKG_FUSE_UTILS):
	mkdir -p $(IDIR_FUSE_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fusermount $(IDIR_FUSE_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_FUSE_UTILS)
	$(IPKG_BUILD) $(IDIR_FUSE_UTILS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libfuse.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/fuse $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/fuse.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfuse.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir}/fuse,,g' $(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc
	
install-dev: $(STAGING_DIR)/usr/lib/libfuse.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/fuse \
		$(STAGING_DIR)/usr/include/fuse.h \
		$(STAGING_DIR)/usr/lib/libfuse.a \
		$(STAGING_DIR)/usr/lib/libfuse.so* \
		$(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc \

compile-targets: install-dev
clean-targets: uninstall-dev
