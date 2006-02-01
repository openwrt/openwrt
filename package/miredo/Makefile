# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=miredo
PKG_VERSION:=0.5.3
PKG_RELEASE:=1
PKG_MD5SUM:=bbbbf60db1346af0bbba6602067ed7ab

PKG_SOURCE_URL:=http://people.via.ecp.fr/~rem/miredo/v0.5/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MIREDO,miredo,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,MIREDO_SERVER,miredo-server,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	( cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"  \
		CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti -nostdinc++"  \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="-luClibc++ -lc -lm" \
		ac_cv_file__proc_self_maps=yes \
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
			--disable-shared \
			--enable-static \
			--with-gnu-ld \
			--disable-rpath \
			--disable-chroot \
			--enable-teredo-client \
			--enable-teredo-relay \
			--enable-teredo-server \
			--enable-miredo-user=root \
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

$(IPKG_MIREDO):
	install -d -m0755 $(IDIR_MIREDO)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/miredo.conf-dist $(IDIR_MIREDO)/etc/miredo.conf
	install -d -m0755 $(IDIR_MIREDO)/etc/init.d
	install -m0755 ./files/miredo.init $(IDIR_MIREDO)/etc/init.d/miredo
	install -d -m0755 $(IDIR_MIREDO)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/miredo $(IDIR_MIREDO)/usr/sbin/
	$(RSTRIP) $(IDIR_MIREDO)
	$(IPKG_BUILD) $(IDIR_MIREDO) $(PACKAGE_DIR)

$(IPKG_MIREDO_SERVER):
	install -d -m0755 $(IDIR_MIREDO_SERVER)/etc/
	$(CP) $(PKG_INSTALL_DIR)/etc/miredo-server.conf-dist $(IDIR_MIREDO_SERVER)/etc/miredo-server.conf
	install -d -m0755 $(IDIR_MIREDO_SERVER)/etc/init.d
	install -m0755 ./files/miredo-server.init $(IDIR_MIREDO_SERVER)/etc/init.d/miredo-server
	install -d -m0755 $(IDIR_MIREDO_SERVER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/miredo-server $(IDIR_MIREDO_SERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_MIREDO_SERVER)
	$(IPKG_BUILD) $(IDIR_MIREDO_SERVER) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
