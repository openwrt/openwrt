# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cbtt
PKG_VERSION:=20060211
PKG_RELEASE:=1
PKG_MD5SUM:=19abea2817c49ccc9438e9671c402984

PKG_SOURCE_URL:=@SF/bnbtusermods 
PKG_SOURCE:=$(PKG_NAME)80-$(PKG_VERSION)-src.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)80-$(PKG_VERSION)-src
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CBTT,cbtt,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,CBTT_MYSQL,cbtt-mysql,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		LFLAGS="-L$(STAGING_DIR)/usr/lib -L $(STAGING_DIR)/usr/lib/mysql -L. -lz -lmysqlclient" \
		CFLAGS="-I$(STAGING_DIR)/usr/include"
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/bnbt $(PKG_INSTALL_DIR)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/bnbtmysql $(PKG_INSTALL_DIR)/usr/bin/
	touch $@

$(IPKG_CBTT):
	install -d -m0755 $(IDIR_CBTT)/usr/bin
	install -m0755 $(PKG_INSTALL_DIR)/usr/bin/bnbt $(IDIR_CBTT)/usr/bin/
	$(RSTRIP) $(IDIR_CBTT)
	$(IPKG_BUILD) $(IDIR_CBTT) $(PACKAGE_DIR)

$(IPKG_CBTT_MYSQL):
	install -d -m0755 $(IDIR_CBTT_MYSQL)/usr/bin
	install -m0755 $(PKG_INSTALL_DIR)/usr/bin/bnbtmysql $(IDIR_CBTT_MYSQL)/usr/bin/
	$(RSTRIP) $(IDIR_CBTT_MYSQL)
	$(IPKG_BUILD) $(IDIR_CBTT_MYSQL) $(PACKAGE_DIR)
