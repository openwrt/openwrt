# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pipacs
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PIPACS,pipacs,$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
	    $(TARGET_CONFIGURE_OPTS) \
	    CFLAGS="$(TARGET_CFLAGS) -I $(STAGING_DIR)/usr/include" \
	    all
	touch $@

$(IPKG_PIPACS):
	mkdir -p $(IDIR_PIPACS)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/pipacs $(IDIR_PIPACS)/usr/bin
	$(RSTRIP) $(IDIR_PIPACS)
	$(IPKG_BUILD) $(IDIR_PIPACS) $(PACKAGE_DIR)
