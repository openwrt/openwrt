# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=sablevm
PKG_VERSION:=1.13
PKG_RELEASE:=1
PKG_MD5SUM:=d5828200032bfe921dd43e450931e38c

PKG_SOURCE_URL:=http://sablevm.org/download/release/$(PKG_VERSION)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_SABLEVM=m

$(eval $(call PKG_template,SABLEVM,sablevm,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -lffi -lltdl -lpopt -lz" \
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
		  --disable-static \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_SABLEVM):
	install -m0755 -d $(IDIR_SABLEVM)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/{java-,}sablevm $(IDIR_SABLEVM)/usr/bin/
	install -m0755 -d $(IDIR_SABLEVM)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsablevm-*.so $(IDIR_SABLEVM)/usr/lib/
	$(RSTRIP) $(IDIR_SABLEVM)
	$(IPKG_BUILD) $(IDIR_SABLEVM) $(PACKAGE_DIR)
