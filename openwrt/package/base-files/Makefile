# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=base-files
PKG_RELEASE:=6
PKG_BUILD_DIR:=$(BUILD_DIR)/base-files

REV:=${shell svn info | grep Revision | cut -d ' ' -f 2}
ifeq ($(REV),)
REV:=0
endif

include $(TOPDIR)/package/rules.mk

IDIR_BASE:=$(PKG_BUILD_DIR)/base-files
IPKG_BASE:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_RELEASE)_$(ARCH).ipk

UCLIBC_VERSION:=${shell cat $(STAGING_DIR)/uclibc_version}
IDIR_UCLIBC:=$(PKG_BUILD_DIR)/uclibc
IPKG_UCLIBC:=$(PACKAGE_DIR)/uclibc_$(UCLIBC_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

LIBGCC_VERSION:=${shell cat $(STAGING_DIR)/gcc_version}
IDIR_LIBGCC:=$(PKG_BUILD_DIR)/libgcc
IPKG_LIBGCC:=$(PACKAGE_DIR)/libgcc_$(LIBGCC_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

PACKAGES:=$(IPKG_BASE) $(IPKG_UCLIBC) $(IPKG_LIBGCC)

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR) $(PACKAGE_DIR)
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	touch $@

$(IPKG_BASE):
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_BASE) ipkg/$(PKG_NAME).control $(PKG_RELEASE) $(ARCH)
	cp -a ./default/* $(IDIR_BASE)
	$(SED) 's,$$R,r$(REV),g' $(IDIR_BASE)/etc/banner
	mkdir -p $(IDIR_BASE)/jffs
	mkdir -p $(IDIR_BASE)/dev
	mkdir -p $(IDIR_BASE)/proc
	mkdir -p $(IDIR_BASE)/tmp
	mkdir -p $(IDIR_BASE)/lib
	mkdir -p $(IDIR_BASE)/usr/lib
	mkdir -p $(IDIR_BASE)/usr/bin
	ln -sf /tmp/resolv.conf $(IDIR_BASE)/etc/resolv.conf
	rm -rf $(IDIR_BASE)/var
	ln -sf /tmp $(IDIR_BASE)/var
	-find $(IDIR_BASE) -type d -name CVS | xargs rm -rf
	-find $(IDIR_BASE) -type d -name .svn | xargs rm -rf
	-find $(IDIR_BASE) -name '.#*' | xargs rm -f
	mkdir -p $(IDIR_BASE)/etc
	-grep \^BR2_SYSCONF $(TOPDIR)/.config > $(IDIR_BASE)/etc/sysconf
	$(IPKG_BUILD) $(IDIR_BASE) $(PACKAGE_DIR)

$(IPKG_UCLIBC):
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_UCLIBC) ipkg/uclibc.control $(UCLIBC_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_UCLIBC)/lib
	for file in ld-uClibc libc libcrypt libdl libm libnsl libresolv librt libuClibc libutil; do \
		cp -fpR $(STAGING_DIR)/lib/$$file.so.* $(IDIR_UCLIBC)/lib/; \
		cp -fpR $(STAGING_DIR)/lib/$$file-$(UCLIBC_VERSION).so $(IDIR_UCLIBC)/lib/; \
	done
	-$(STRIP) $(IDIR_UCLIBC)/lib/*
	$(IPKG_BUILD) $(IDIR_UCLIBC) $(PACKAGE_DIR)

$(IPKG_LIBGCC):
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBGCC) ipkg/libgcc.control $(LIBGCC_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBGCC)/lib
	cp -fpR $(STAGING_DIR)/lib/libgcc_s.so.* $(IDIR_LIBGCC)/lib/
	-$(STRIP) $(IDIR_LIBGCC)/lib/*
	$(IPKG_BUILD) $(IDIR_LIBGCC) $(PACKAGE_DIR)

package-clean:
	rm -f $(PACKAGES)
	
clean-targets: package-clean
prepare:
compile-targets: $(PACKAGES)
install-targets: compile
	mkdir -p $(TARGET_DIR)
	$(IPKG) install $(PACKAGES)
