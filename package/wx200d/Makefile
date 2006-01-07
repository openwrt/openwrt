# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wx200d
PKG_VERSION:=1.3
PKG_RELEASE:=1
PKG_MD5SUM:=0f90c099dddcdd46ac568d707822ec4c
PKG_SOURCE_URL:=@SF/wx200d
PKG_SOURCE:=wx200d-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat

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
			--prefix=/usr \
			--without-libiconv-prefix \
			--without-libintl-prefix \
			--disable-nls \
	);
	touch $@

$(PKG_BUILD_DIR)/.built: 
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR) -DSYSV -DPOSIX -DHAS_ITIMER -DLINUX -DHASSELECT -DHASTZ"
	touch $@

$(IPKG_WX200D):
	install -d -m0755 $(IDIR_WX200D)/usr/bin
	install -m0644 $(PKG_BUILD_DIR)/wx200 \
		$(PKG_BUILD_DIR)/wxstdout \
		$(PKG_BUILD_DIR)/wxfilter \
		$(PKG_BUILD_DIR)/wxdebug $(IDIR_WX200D)/usr/bin/
	mkdir -p $(IDIR_WX200D)/usr/sbin/
	install -m0644 $(PKG_BUILD_DIR)/wx200d \
		$(PKG_BUILD_DIR)/wxread $(IDIR_WX200D)/usr/sbin/
	$(RSTRIP) $(IDIR_WX200D)
	$(IPKG_BUILD) $(IDIR_WX200D) $(PACKAGE_DIR)
