# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libpthread
PKG_VERSION:=0.9.27
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBPTHREAD:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBPTHREAD:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBPTHREAD:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

.NOTPARALLEL:

$(IPKG_LIBPTHREAD): $(STAGING_DIR)/lib/libpthread.so
	rm -rf $(PKG_BUILD_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBPTHREAD) ./ipkg/$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBPTHREAD)/lib
	cp -fpR $(STAGING_DIR)/lib/libpthread.so.* $(IDIR_LIBPTHREAD)/lib/
	cp -fpR $(STAGING_DIR)/lib/libpthread-*.so $(IDIR_LIBPTHREAD)/lib/
	$(STRIP) $(IDIR_LIBPTHREAD)/lib/lib*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPTHREAD) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(IPKG_LIBPTHREAD)
	$(IPKG) install $(IPKG_LIBPTHREAD)

source:
prepare:
compile: $(IPKG_LIBPTHREAD)
install: $(INFO_LIBPTHREAD)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBPTHREAD)
