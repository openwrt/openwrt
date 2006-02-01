# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=microcom
PKG_VERSION:=1.02
PKG_FILEVERSION:=102
PKG_RELEASE:=1
PKG_MD5SUM:=c7817035dc41cb02e7cfb565cf9b7401

PKG_SOURCE_URL:=http://microcom.port5.com/
PKG_SOURCE:=m$(PKG_FILEVERSION).tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MICROCOM,microcom,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	# the .tar.gz is different - no subdirectory, so do this manually
	zcat $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR) $(TAR_OPTIONS) -
	touch $@

$(PKG_BUILD_DIR)/.configured:
	$(CP) $(PKG_BUILD_DIR)/Makefile  $(PKG_BUILD_DIR)/Makefile.tmp
	sed 's~gcc~$$(CC)~' $(PKG_BUILD_DIR)/Makefile.tmp > $(PKG_BUILD_DIR)/Makefile
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) CC=$(TARGET_CC)
	touch $@

$(IPKG_MICROCOM):
	install -d -m0755 $(IDIR_MICROCOM)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/microcom $(IDIR_MICROCOM)/usr/bin
	$(RSTRIP) $(IDIR_MICROCOM)
	$(IPKG_BUILD) $(IDIR_MICROCOM) $(PACKAGE_DIR)

