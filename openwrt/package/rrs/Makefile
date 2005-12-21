# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rrs
PKG_VERSION:=1.70
PKG_RELEASE:=1
PKG_MD5SUM:=b400d03c0e39e3e78a7327ba78f789f0

PKG_SOURCE_URL:=http://www.cycom.se/uploads/36/19 

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RRS,rrs,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRS_NOSSL,rrs-nossl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
ifneq ($(BR2_PACKAGE_RRS),)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -fno-builtin -fno-rtti -nostdinc++" \
		LDFLAGS="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -luClibc++ -lc -lm -lutil -lssl -lcrypto" \
		generic
	( cd $(PKG_BUILD_DIR); mv rrs rrs-ssl; )
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		clean
endif
ifneq ($(BR2_PACKAGE_RRS_NOSSL),)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -fno-builtin -fno-rtti -nostdinc++" \
		LDFLAGSNOSSL="-nodefaultlibs -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -luClibc++ -lc -lm -lutil" \
		generic-nossl
	( cd $(PKG_BUILD_DIR); mv rrs rrs-nossl; )
endif
	touch $@

$(IPKG_RRS):
	install -d -m0755 $(IDIR_RRS)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/rrs-ssl $(IDIR_RRS)/usr/bin/rrs
	$(RSTRIP) $(IDIR_RRS)
	$(IPKG_BUILD) $(IDIR_RRS) $(PACKAGE_DIR)

$(IPKG_RRS_NOSSL):
	install -d -m0755 $(IDIR_RRS_NOSSL)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/rrs-nossl $(IDIR_RRS_NOSSL)/usr/bin/rrs
	$(RSTRIP) $(IDIR_RRS_NOSSL)
	$(IPKG_BUILD) $(IDIR_RRS_NOSSL) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
