# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bc
PKG_VERSION:=1.06
PKG_RELEASE:=1
PKG_MD5SUM:=d44b5dddebd8a7a7309aea6c36fda117

PKG_SOURCE_URL:=http://ftp.gnu.org/pub/gnu/bc
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BC,bc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,DC,dc,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_BC): 
	mkdir -p $(IDIR_BC)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/bc/bc $(IDIR_BC)/usr/bin/bc
	$(STRIP) $(IDIR_BC)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_BC) $(PACKAGE_DIR)

$(IPKG_DC): 
	mkdir -p $(IDIR_DC)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/dc/dc $(IDIR_DC)/usr/bin/dc
	$(STRIP) $(IDIR_DC)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_DC) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
