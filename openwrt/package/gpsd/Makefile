# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gpsd
PKG_VERSION:=2.30
PKG_RELEASE:=1
PKG_MD5SUM:=dde177174878e8ae6db15f8010da46dd

PKG_SOURCE_URL:=http://download.berlios.de/gpsd/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,GPSD,gpsd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	( cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti -nostdinc++"  \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-luClibc++ -lc -lm -lnotimpl" \
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
			--with-gnu-ld \
			--without-x \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CXXLINK="\$$(LINK)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_GPSD):
	install -d -m0755 $(IDIR_GPSD)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgps.so.* $(IDIR_GPSD)/usr/lib/
	install -d -m0755 $(IDIR_GPSD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/gpsd $(IDIR_GPSD)/usr/sbin/
	$(RSTRIP) $(IDIR_GPSD)
	$(IPKG_BUILD) $(IDIR_GPSD) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libgps.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gps{,d}.h $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libgpsmm.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgps.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libgps.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/gps{,d}.h \
		$(STAGING_DIR)/usr/include/libgpsmm.h \
		$(STAGING_DIR)/usr/lib/libgps.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
