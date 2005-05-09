# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=fuse
PKG_VERSION:=2.2.1
PKG_RELEASE:=1
PKG_MD5SUM:=250d89b9c7b6ecf531df60c67f75737d

PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,KMOD_FUSE,kmod-fuse,$(LINUX_VERSION)+$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LIBFUSE,libfuse,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,FUSE_UTILS,fuse-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
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
		  --libexecdir=/usr/lib/locate \
		  --localstatedir=/var/lib \
		  --mandir=/usr/share/man \
		  --sbindir=/usr/sbin \
		  --sysconfdir=/etc \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  --enable-shared \
		  --enable-static \
		  --enable-kernel-module \
		  --enable-lib \
		  --enable-util \
		  --disable-example \
		  --disable-auto-modprobe \
		  --with-kernel=$(LINUX_DIR) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $(PKG_BUILD_DIR)/.built

$(PKG_BUILD_DIR)/.installed: $(PKG_BUILD_DIR)/.built
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	touch $(PKG_BUILD_DIR)/.installed

$(IPKG_KMOD_FUSE): $(IDIR_KMOD_FUSE)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	mkdir -p $(IDIR_KMOD_FUSE)/lib/modules/$(LINUX_VERSION)
	cp -fpR $(PKG_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.o \
	 $(IDIR_KMOD_FUSE)/lib/modules/$(LINUX_VERSION)/
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_KMOD_FUSE) $(PACKAGE_DIR)

$(IPKG_LIBFUSE): $(IDIR_LIBFUSE)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	mkdir -p $(IDIR_LIBFUSE)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libfuse.so.* $(IDIR_LIBFUSE)/usr/lib/
	chmod 0644 $(IDIR_LIBFUSE)/usr/lib/lib*.so*
	$(RSTRIP) $(IDIR_LIBFUSE)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBFUSE) $(PACKAGE_DIR)

$(IPKG_FUSE_UTILS): $(IDIR_FUSE_UTILS)/CONTROL/control $(PKG_BUILD_DIR)/.installed
	mkdir -p $(IDIR_FUSE_UTILS)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/fusermount $(IDIR_FUSE_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_FUSE_UTILS)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_FUSE_UTILS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libfuse.so: $(PKG_BUILD_DIR)/.installed
	mkdir -p $(STAGING_DIR)/usr/include
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/fuse* $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libfuse.{a,so*} $(STAGING_DIR)/usr/lib/
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	
install-dev: $(STAGING_DIR)/usr/lib/libfuse.so

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/fuse*
	rm -rf $(STAGING_DIR)/usr/lib/libfuse.{a,so*}
	rm -rf $(STAGING_DIR)/usr/lib/pkgconfig/fuse.pc

install: install-dev
clean: uninstall-dev
