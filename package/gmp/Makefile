# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=4.1.4
PKG_RELEASE:=1
PKG_MD5SUM:=0aa7d3b3f5b5ec5951e7dddd6f65e891

PKG_SOURCE_URL:=ftp://ftp.gnu.org/gnu/gmp/ \
	ftp://ftp.belnet.be/mirror/ftp.gnu.org/gnu/gmp/ \
	ftp://ftp.mirror.nl/pub/mirror/gnu/gmp/ \
	http://mirror.switch.ch/ftp/mirror/gnu/gmp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBGMP,libgmp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
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

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBGMP):
	install -m0755 -d $(IDIR_LIBGMP)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libgmp.so.* $(IDIR_LIBGMP)/usr/lib/
	$(RSTRIP) $(IDIR_LIBGMP)
	$(IPKG_BUILD) $(IDIR_LIBGMP) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libgmp.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	cp -fpR $(PKG_INSTALL_DIR)/usr/include/gmp* $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libgmp.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libgmp.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/gmp* \
		$(STAGING_DIR)/usr/lib/libgmp.{a,so*} \

compile: install-dev
clean: uninstall-dev
