# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=irssi
PKG_VERSION:=0.8.10
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://irssi.org/files/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)a.tar.bz2
PKG_MD5SUM:=38e616bccb6a34ff6d91690317c2fa19
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IRSSI,irssi,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		ac_cv_c_bigendian=no \
		ac_cv_sizeof_off_t=8 \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--program-prefix="" \
		--with-perl=no \
		--with-glib1 \
		--with-gnu-ld \
		--with-textui \
		--without-terminfo \
		--without-bot \
		--without-file-offset-size \
		--without-ssl \
		$(DISABLE_NLS) \
		--disable-ssl \
		--disable-ipv6 \
		--disable-proxy \
		--with-glib-prefix=$(STAGING_DIR)/usr \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_IRSSI): 
	mkdir -p $(IDIR_IRSSI)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/fe-text/$(PKG_NAME) $(IDIR_IRSSI)/usr/bin/
	$(STRIP) $(IDIR_IRSSI)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_IRSSI) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
