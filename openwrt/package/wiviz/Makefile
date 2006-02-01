# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wiviz
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WIVIZ,wiviz,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CCOPTS="$(TARGET_CFLAGS)" \
		INCLUDE="-I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib"
	touch $@

$(IPKG_WIVIZ): 
	mkdir -p $(IDIR_WIVIZ)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_WIVIZ)/usr/sbin/
	$(RSTRIP) $(IDIR_WIVIZ)
	$(CP) ./files/* $(IDIR_WIVIZ)
	find $(IDIR_WIVIZ) -name CVS | xargs rm -rf
	find $(IDIR_WIVIZ) -name .svn | xargs rm -rf
	$(IPKG_BUILD) $(IDIR_WIVIZ) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
