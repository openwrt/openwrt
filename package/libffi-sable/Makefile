# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libffi-sable
PKG_VERSION:=3325
PKG_RELEASE:=3
PKG_MD5SUM:=0c32eaaea0269c5fbe156fcd0be3e5bd

PKG_SOURCE_URL:=http://sablevm.org/download/snapshot/2005-01-21/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_LIBFFI_SABLE=m

$(eval $(call PKG_template,LIBFFI_SABLE,libffi-sable,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
		  --disable-debug \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBFFI_SABLE):
	install -m0755 -d $(IDIR_LIBFFI_SABLE)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libffi.so.* $(IDIR_LIBFFI_SABLE)/usr/lib/
	$(RSTRIP) $(IDIR_LIBFFI_SABLE)
	$(IPKG_BUILD) $(IDIR_LIBFFI_SABLE) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libffi.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ffi{,target}.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libffi.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libffi.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/ffi{,target}.h \
		$(STAGING_DIR)/usr/lib/libffi.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
