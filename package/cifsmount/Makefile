# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cifsmount
PKG_VERSION:=1.5
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/cifsmount

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CIFSMOUNT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $@
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/mount.cifs mount.cifs.c
	$(STRIP) $(PKG_BUILD_DIR)/mount.cifs
	touch $@

$(IPKG_CIFSMOUNT):
	mkdir -p $(IDIR_CIFSMOUNT)/sbin
	install -m 0755 $(PKG_BUILD_DIR)/mount.cifs $(IDIR_CIFSMOUNT)/sbin/mount.cifs
	$(IPKG_BUILD) $(IDIR_CIFSMOUNT) $(PACKAGE_DIR)
