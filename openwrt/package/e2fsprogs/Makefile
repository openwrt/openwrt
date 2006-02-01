# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.38
PKG_RELEASE:=1
PKG_MD5SUM:=d774d4412bfb80d12cf3a4fdfd59de5a

PKG_SOURCE_URL:=@SF/e2fsprogs
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,E2FSPROGS,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
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
			--sbindir=/sbin \
			--sysconfdir=/etc \
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--with-gnu-ld \
			--enable-elf-shlibs \
			--enable-dynamic-e2fsck \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR)/util \
		BUILDCC="$(HOSTCC)" \
		CFLAGS="" \
		subst
	$(MAKE) -C $(PKG_BUILD_DIR) \
		BUILDCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@
	touch $@

$(IPKG_E2FSPROGS):
	mkdir -p $(IDIR_E2FSPROGS)/sbin
	$(CP) $(PKG_INSTALL_DIR)/sbin/e2fsck $(IDIR_E2FSPROGS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/mke2fs $(IDIR_E2FSPROGS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/mkfs.ext{2,3} $(IDIR_E2FSPROGS)/sbin/
	$(CP) $(PKG_INSTALL_DIR)/sbin/tune2fs $(IDIR_E2FSPROGS)/sbin/
	mkdir -p $(IDIR_E2FSPROGS)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{blkid,com_err,e2p,ext2fs,uuid}.so.* $(IDIR_E2FSPROGS)/usr/lib/
	$(RSTRIP) $(IDIR_E2FSPROGS)
	$(IPKG_BUILD) $(IDIR_E2FSPROGS) $(PACKAGE_DIR)
