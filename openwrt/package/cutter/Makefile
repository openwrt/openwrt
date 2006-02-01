# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cutter
PKG_VERSION:=1.03
PKG_RELEASE:=1
PKG_MD5SUM:=50093db9b64277643969ee75b83ebbd1

PKG_SOURCE_URL:=http://www.lowth.com/cutter/software/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CUTTER,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@
	
$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(PKG_BUILD_DIR)/cutter.c -o $(PKG_BUILD_DIR)/cutter
	install -d -m0755 $(PKG_INSTALL_DIR)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/cutter $(PKG_INSTALL_DIR)/usr/sbin/
	
$(IPKG_CUTTER):
	mkdir -p $(IDIR_CUTTER)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_CUTTER)/
	$(RSTRIP) $(IDIR_CUTTER)/
	$(IPKG_BUILD) $(IDIR_CUTTER) $(PACKAGE_DIR)
