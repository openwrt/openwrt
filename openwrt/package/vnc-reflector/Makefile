include $(TOPDIR)/rules.mk

PKG_NAME:=vnc-reflector
PKG_VERSION:=1.2.4
PKG_RELEASE:=1
PKG_MD5SUM:=
PKG_SOURCE_URL:=@SF/vnc-reflector
PKG_SOURCE:=vnc_reflector-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/vnc_reflector
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VNC_REFLECTOR,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) STAGING_DIR=$(STAGING_DIR) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR)
	install -d $(PKG_INSTALL_DIR)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/vncreflector $(PKG_INSTALL_DIR)/usr/sbin/
	touch $@

$(IPKG_VNC_REFLECTOR):
	mkdir -p $(IDIR_VNC_REFLECTOR)
	cp -fpR $(PKG_INSTALL_DIR)/* $(IDIR_VNC_REFLECTOR)/
	$(RSTRIP) $(IDIR_VNC_REFLECTOR)
	$(IPKG_BUILD) $(IDIR_VNC_REFLECTOR) $(PACKAGE_DIR)
