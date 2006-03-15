# $Id: Makefile 3112 2006-02-01 23:53:19Z mbm $

include $(TOPDIR)/rules.mk

PKG_NAME:=fetchmail
PKG_VERSION:=6.2.5
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.catb.org/~esr/fetchmail/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=9956b30139edaa4f5f77c4d0dbd80225
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,FETCHMAIL,fetchmail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		ac_cv_c_bigendian=no \
		ac_cv_sizeof_off_t=8 \
		ac_cv_sizeof_int=4 \
		ac_cv_sizeof_long=4 \
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
		--enable-fallback=procmail \
		$(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_FETCHMAIL): 
	mkdir -p $(IDIR_FETCHMAIL)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_FETCHMAIL)/usr/bin/
	$(RSTRIP) $(IDIR_FETCHMAIL)
	$(IPKG_BUILD) $(IDIR_FETCHMAIL) $(PACKAGE_DIR)
