# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=deco
PKG_VERSION:=39
PKG_RELEASE:=1
PKG_MD5SUM:=f77f60e8be0cae1f814cba1ef61bf4d0

PKG_SOURCE_URL:=@SF/deco
PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tgz

PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DECO,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
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
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_DECO):
	install -m0755 -d $(IDIR_DECO)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/deco $(IDIR_DECO)/usr/bin/deco
	install -m0755 -d $(IDIR_DECO)/usr/lib/deco
	install -m0644 $(PKG_BUILD_DIR)/profile $(IDIR_DECO)/usr/lib/deco/profile
	install -m0644 $(PKG_BUILD_DIR)/menu $(IDIR_DECO)/usr/lib/deco/menu
	$(RSTRIP) $(IDIR_DECO)
	$(IPKG_BUILD) $(IDIR_DECO) $(PACKAGE_DIR)

