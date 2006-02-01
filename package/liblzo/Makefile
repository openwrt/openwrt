# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=lzo
PKG_VERSION:=1.08
PKG_RELEASE:=1
PKG_MD5SUM:=ab94d3da364c7cbd5b78d76f1875b0f6

PKG_SOURCE_URL:=http://www.oberhumer.com/opensource/lzo/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBLZO,liblzo,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS_O="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBLZO):
	install -d -m0755 $(IDIR_LIBLZO)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblzo.so.* $(IDIR_LIBLZO)/usr/lib
	$(RSTRIP) $(IDIR_LIBLZO)
	$(IPKG_BUILD) $(IDIR_LIBLZO) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/liblzo.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/lzo*.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblzo.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $(STAGING_DIR)/usr/lib/liblzo.so

install-dev: $(STAGING_DIR)/usr/lib/liblzo.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/include/lzo*.h \
	  $(STAGING_DIR)/usr/lib/liblzo.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
