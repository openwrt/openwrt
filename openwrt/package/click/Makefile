include $(TOPDIR)/rules.mk

PKG_NAME:=click
PKG_VERSION:=cvs.2006.02.27
PKG_RELEASE:=0
PKG_MD5SUM:=0479c7e50547cc8fdbfcc60f6eea30b4

ROOFNET_VERSION:=0.0.1

PKG_SOURCE_URL:=http://pdos.csail.mit.edu/~jbicket/click_snapshots/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CLICK,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,ROOFNET,roofnet,$(ROOFNET_VERSION),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CXXFLAGS="-static -O2 -MD" \
		CFLAGS="-static -MD" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		  --prefix=/usr \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --enable-wifi \
		  --disable-linuxmodule \
		  --enable-tools=host \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	touch $@

$(IPKG_CLICK):
	install -d -m0755 $(IDIR_CLICK)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/userlevel/click $(IDIR_CLICK)/usr/bin/click 
	$(CP) $(PKG_BUILD_DIR)/tools/click-align/click-align $(IDIR_CLICK)/usr/bin/click-align
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/read_handler $(IDIR_CLICK)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/write_handler $(IDIR_CLICK)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/srcr.click $(IDIR_CLICK)/usr/bin
	$(RSTRIP) $(IDIR_CLICK)
	$(IPKG_BUILD) $(IDIR_CLICK) $(PACKAGE_DIR)


$(IPKG_ROOFNET):
	install -d -m0755 $(IDIR_ROOFNET)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/gen_config_roofnet.sh $(IDIR_ROOFNET)/usr/bin
	mkdir -p $(IDIR_ROOFNET)/etc/init.d
	install -m 755 ./files/S50roofnet $(IDIR_ROOFNET)/etc/init.d/
	$(IPKG_BUILD) $(IDIR_ROOFNET) $(PACKAGE_DIR)
