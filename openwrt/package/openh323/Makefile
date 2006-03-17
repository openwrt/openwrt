# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openh323
PKG_VERSION:=cvs-20051230
PKG_RELEASE:=1
PKG_MD5SUM:=ba1ffd45016f533bec2bb1539b78e43a

PKG_SOURCE_URL:=http://trash.uid0.hu/openwrt/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,OPENH323,openh323,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	rm -rf $(PKG_INSTALL_DIR);
	mkdir -p $(PKG_INSTALL_DIR);
	ln -s $(PKG_BUILD_DIR)/../pwlib-cvs-20051227 $(PKG_BUILD_DIR)/../pwlib; 
	cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
	PWLIBDIR="$(STAGING_DIR)" \
	    ./configure \
	    --prefix=$(STAGING_DIR) \
	    --target=$(GNU_TARGET_NAME) \
	    --host=$(GNU_TARGET_NAME) \
	    --build=$(GNU_HOST_NAME) \
	cd $(PKG_BUILD_DIR)/plugins; rm -rf config.{cache,status}; \
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
	PWLIBDIR="$(STAGING_DIR)" \
	    ./configure \
	    --prefix=$(STAGING_DIR)
	touch $@

$(PKG_BUILD_DIR)/.built:
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
	PWLIBDIR="$(PKG_BUILD_DIR)/../pwlib-cvs-20051227/" \
	$(MAKE) -C $(PKG_BUILD_DIR)/plugins $(TARGET_CONFIGURE_OPTS)
	
	CFLAGS="$(TARGET_CFLAGS)" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
	PWLIBDIR="$(PKG_BUILD_DIR)/../pwlib-cvs-20051227/" \
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) opt install
	touch $@
