# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libgcc
PKG_VERSION:=$(BR2_GCC_VERSION)
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_LIBGCC:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBGCC:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBGCC:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

.NOTPARALLEL:

$(IPKG_LIBGCC): $(STAGING_DIR)/lib/libgcc_s.so
	rm -rf $(PKG_BUILD_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBGCC) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBGCC)/lib
	cp -fp $(STAGING_DIR)/lib/libgcc_s.so.* $(IDIR_LIBGCC)/lib/
	$(STRIP) $(IDIR_LIBGCC)/lib/lib*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBGCC) $(PACKAGE_DIR)

$(INFO_LIBGCC): $(IPKG_LIBGCC)
	$(IPKG) install $(IPKG_LIBGCC)

source:
prepare:
compile: $(IPKG_LIBGCC)
install: $(INFO_LIBGCC)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBGCC)
