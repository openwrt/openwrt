# $Id$
	
include $(TOPDIR)/rules.mk

PKG_NAME:=ser2net
PKG_VERSION:=2.3
PKG_RELEASE:=1
PKG_MD5SUM:=5f83a3e8aec18331cb61069dccdfba47
 
PKG_SOURCE_URL:=@SF/ser2net/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
	
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SER2NET,ser2net,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS)" \
	./configure \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix=/usr \
	--sysconfdir=/etc \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	all install
	touch $@
 
$(IPKG_SER2NET):
	install -d -m0755 $(IDIR_SER2NET)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/ser2net $(IDIR_SER2NET)/usr/sbin
	install -d -m0755 $(IDIR_SER2NET)/etc
	cp -fpR $(PKG_BUILD_DIR)/ser2net.conf $(IDIR_SER2NET)/etc
	$(RSTRIP) $(IDIR_SER2NET)
	$(IPKG_BUILD) $(IDIR_SER2NET) $(PACKAGE_DIR)
