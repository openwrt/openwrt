#############################################################
#
# ebtables
#
#############################################################
include $(TOPDIR)/rules.mk

PKG_NAME:=ebtables
PKG_VERSION:=2.0.6
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://umn.dl.sourceforge.net/sourceforge/ebtables/
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/ebtables-v$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PKG_SOURCE_URL)/$(PKG_SOURCE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/ebtables: $(PKG_BUILD_DIR)/.unpacked
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall"

$(PKG_IPK): $(PKG_BUILD_DIR)/ebtables
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/ebtables $(PKG_IPK_DIR)/usr/sbin/
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install:
	$(IPKG) install $(PKG_IPK)
clean:
	rm -rf $(PKG_BUILD_DIR)
