# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cifsmount
PKG_VERSION:=1.5
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/cifsmount
PKG_IPK_DIR:=$(PKG_BUILD_DIR)
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

$(PKG_BUILD_DIR)/sbin/mount.cifs:
	mkdir -p $(PKG_BUILD_DIR)/sbin
	$(TARGET_CC) -o $@ mount.cifs.c
	$(STRIP) $@

$(PKG_IPK): $(PKG_BUILD_DIR)/sbin/mount.cifs
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

source:
prepare:
compile: $(PKG_IPK)
install:
	$(IPKG) install $(PKG_IPK)
clean:
	rm -rf $(PKG_BUILD_DIR)
