# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=click
PKG_VERSION:=cvs.2006.03.02
PKG_RELEASE:=1
PKG_MD5SUM:=7aed4ba94926010d76febb797b288e98

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
	(cd $(PKG_BUILD_DIR); \
		./configure; \
		cp include/click/config.h include/click/config-host.h; \
		rm -rf config.{cache,status} ; \
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
		  --enable-tools=mixed \
	   	  --disable-dynamic-linking \
	)
	touch $@


# don't build the full click (it's huge), but only include
# the elements needed for roofnet
$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		BUILD_CXX="g++ -include $(PKG_BUILD_DIR)/include/click/config-host.h" \
		tools elementmap.xml
	(cd $(PKG_BUILD_DIR)/userlevel; \
		../tools/click-mkmindriver/click-mkmindriver -p roofnet -C .. \
			-f $(PKG_BUILD_DIR)/conf/wifi/sample.click \
			-A --all -E IPNameInfo -E FromHost -E ToHost -E Discard; \
		$(MAKE) -f Makefile.roofnet; \
	)
	touch $@

$(IPKG_CLICK):
	install -d -m0755 $(IDIR_CLICK)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/userlevel/roofnetclick $(IDIR_CLICK)/usr/bin/click 
	$(CP) $(PKG_BUILD_DIR)/tools/click-align/click-align $(IDIR_CLICK)/usr/bin/click-align
	$(RSTRIP) $(IDIR_CLICK)
	$(IPKG_BUILD) $(IDIR_CLICK) $(PACKAGE_DIR)


$(IPKG_ROOFNET):
	install -d -m0755 $(IDIR_ROOFNET)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/gen_config_roofnet.sh $(IDIR_ROOFNET)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/srcr.click $(IDIR_ROOFNET)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/read_handler $(IDIR_ROOFNET)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/wifi/write_handler $(IDIR_ROOFNET)/usr/bin
	mkdir -p $(IDIR_ROOFNET)/etc/init.d
	install -m 755 ./files/S50roofnet $(IDIR_ROOFNET)/etc/init.d/
	$(IPKG_BUILD) $(IDIR_ROOFNET) $(PACKAGE_DIR)
