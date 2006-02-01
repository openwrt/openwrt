# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipsec-tools
PKG_VERSION:=0.6.4
PKG_RELEASE:=1
PKG_MD5SUM:=d0242a943c82c0cbf28005966ff35e21

PKG_SOURCE_URL:=@SF/ipsec-tools
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPSEC_TOOLS,ipsec-tools,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		touch configure.ac; \
		touch aclocal.m4; \
		touch Makefile.in; \
		touch config.h.in; \
		touch configure; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
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
			--with-gnu-ld \
			--with-kernel-headers="$(LINUX_DIR)/include" \
			--without-readline \
			--with-openssl="$(STAGING_DIR)/usr" \
			--without-libradius \
			--without-libpam \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_IPSEC_TOOLS):
	install -d -m0755 $(IDIR_IPSEC_TOOLS)/etc
	$(CP) $(PKG_BUILD_DIR)/src/racoon/samples/racoon.conf $(IDIR_IPSEC_TOOLS)/etc/
	install -d -m0755 $(IDIR_IPSEC_TOOLS)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipsec.so.* $(IDIR_IPSEC_TOOLS)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libracoon.so.* $(IDIR_IPSEC_TOOLS)/usr/lib/
	install -d -m0755 $(IDIR_IPSEC_TOOLS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/plainrsa-gen $(IDIR_IPSEC_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoon $(IDIR_IPSEC_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/racoonctl $(IDIR_IPSEC_TOOLS)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/setkey $(IDIR_IPSEC_TOOLS)/usr/sbin/
	$(RSTRIP) $(IDIR_IPSEC_TOOLS)
	$(IPKG_BUILD) $(IDIR_IPSEC_TOOLS) $(PACKAGE_DIR)
