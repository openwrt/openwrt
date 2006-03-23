# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=procmail
PKG_VERSION:=3.22
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.procmail.org/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=1678ea99b973eb77eda4ecf6acae53f1
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PROCMAIL,procmail,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)/src -f ../Makefile.openwrt  \
		CC=$(TARGET_CC)

	touch $@

$(IPKG_PROCMAIL): 
	mkdir -p $(IDIR_PROCMAIL)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/procmail $(IDIR_PROCMAIL)/usr/bin/ 
	$(CP) $(PKG_BUILD_DIR)/src/formail $(IDIR_PROCMAIL)/usr/bin/
	$(RSTRIP) $(IDIR_PROCMAIL)
	$(IPKG_BUILD) $(IDIR_PROCMAIL) $(PACKAGE_DIR)
