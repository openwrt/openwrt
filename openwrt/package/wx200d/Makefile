# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wx200d
PKG_VERSION:=1.3
PKG_RELEASE:=1
PKG_MD5SUM:=0f90c099dddcdd46ac568d707822ec4c

PKG_SOURCE_URL:=@SF/wx200d
PKG_SOURCE:=wx200d-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WX200D,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
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
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
			--without-libiconv-prefix \
			--without-libintl-prefix \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: 
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_WX200D):
	install -d -m0755 $(IDIR_WX200D)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/wx200 \
		$(PKG_INSTALL_DIR)/usr/bin/wxstdout \
		$(PKG_INSTALL_DIR)/usr/bin/wxfilter \
		$(PKG_INSTALL_DIR)/usr/bin/wxdebug $(IDIR_WX200D)/usr/bin/
	install -d -m0755 $(IDIR_WX200D)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/wx200d \
		$(PKG_INSTALL_DIR)/usr/sbin/wxread $(IDIR_WX200D)/usr/sbin/
	$(RSTRIP) $(IDIR_WX200D)
	$(IPKG_BUILD) $(IDIR_WX200D) $(PACKAGE_DIR)
