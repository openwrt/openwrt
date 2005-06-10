# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=picocom
PKG_VERSION:=1.4
PKG_RELEASE:=1
PKG_MD5SUM:=08fcc5f6bb9e7676a2569386d5ea9f70

PKG_SOURCE_URL:=http://efault.net/npat/hacks/picocom/dist/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PICOCOM,picocom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		CFLAGS="$(TARGET_CFLAGS) -L$(STAGING_DIR)/usr/lib" \
		picocom
	touch $@

$(IPKG_PICOCOM):
	install -d -m0755 $(IDIR_PICOCOM)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/picocom $(IDIR_PICOCOM)/usr/bin/
	$(RSTRIP) $(IDIR_PICOCOM)
	$(IPKG_BUILD) $(IDIR_PICOCOM) $(PACKAGE_DIR)
