#############################################################
#
# ebtables
#
#############################################################
include $(TOPDIR)/rules.mk

PKG_NAME:=linksys-utils
PKG_VERSION:=3.37.6
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://mywrt.sourceforge.net/files
PKG_SOURCE:=linksys-utils-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PKG_SOURCE_URL)/$(PKG_SOURCE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_IPK): $(PKG_BUILD_DIR)/.unpacked $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/et $(PKG_IPK_DIR)/usr/sbin/
	cp $(PKG_BUILD_DIR)/nas $(PKG_IPK_DIR)/usr/sbin/
	cp $(PKG_BUILD_DIR)/wl $(PKG_IPK_DIR)/usr/sbin/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)
	
$(PACKAGE_DIR):
	mkdir -p $(PACKAGE_DIR)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install:
	$(IPKG) install $(PKG_IPK)
clean:
	rm -rf $(PKG_BUILD_DIR)
