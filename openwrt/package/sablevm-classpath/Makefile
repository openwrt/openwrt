# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=sablevm-classpath
PKG_VERSION:=1.13
PKG_RELEASE:=1
PKG_MD5SUM:=9476bb7a0ad67c6cfa353d06f094cdce

PKG_SOURCE_URL:=http://sablevm.org/download/release/$(PKG_VERSION)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

BR2_PACKAGE_LIBSABLEVM_CLASSPATH=m
BR2_PACKAGE_SABLEVM_CLASSPATH_FULL=m
BR2_PACKAGE_SABLEVM_CLASSPATH_MINI=m

$(eval $(call PKG_template,LIBSABLEVM_CLASSPATH,libsablevm-classpath,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SABLEVM_CLASSPATH_FULL,sablevm-classpath-full,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SABLEVM_CLASSPATH_MINI,sablevm-classpath-mini,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  --disable-static \
		  --disable-gtk-peer \
		  --disable-gtk-cairo \
		  --without-x \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	$(CP) ./files/mini.classlist $(PKG_BUILD_DIR)/lib/
	cd $(PKG_BUILD_DIR)/lib ; fastjar -Mcf mini.jar -@ < mini.classlist
	touch $@

$(IPKG_LIBSABLEVM_CLASSPATH):
	install -d -m0755 $(IDIR_LIBSABLEVM_CLASSPATH)/usr/lib/sablevm-classpath
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sablevm-classpath/libjava*.so $(IDIR_LIBSABLEVM_CLASSPATH)/usr/lib/sablevm-classpath/
	install -d -m0755 $(IDIR_LIBSABLEVM_CLASSPATH)/usr/lib/security
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/security/classpath.security $(IDIR_LIBSABLEVM_CLASSPATH)/usr/lib/security/
	$(RSTRIP) $(IDIR_LIBSABLEVM_CLASSPATH)
	$(IPKG_BUILD) $(IDIR_LIBSABLEVM_CLASSPATH) $(PACKAGE_DIR)

$(IPKG_SABLEVM_CLASSPATH_FULL):
	install -d -m0755 $(IDIR_SABLEVM_CLASSPATH_FULL)/usr/share/sablevm-classpath
	$(CP) $(PKG_INSTALL_DIR)/usr/share/sablevm-classpath/{libclasspath,resources}.jar \
		$(IDIR_SABLEVM_CLASSPATH_FULL)/usr/share/sablevm-classpath/
	$(IPKG_BUILD) $(IDIR_SABLEVM_CLASSPATH_FULL) $(PACKAGE_DIR)
	
$(IPKG_SABLEVM_CLASSPATH_MINI):
	install -d -m0755 $(IDIR_SABLEVM_CLASSPATH_MINI)/usr/share/sablevm-classpath
	install -m0755 $(PKG_BUILD_DIR)/lib/mini.jar \
		$(IDIR_SABLEVM_CLASSPATH_MINI)/usr/share/sablevm-classpath/libclasspath.jar
	$(IPKG_BUILD) $(IDIR_SABLEVM_CLASSPATH_MINI) $(PACKAGE_DIR)
