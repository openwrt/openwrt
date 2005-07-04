# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=uClibc++
PKG_VERSION:=0.1.11
PKG_RELEASE:=1
PKG_MD5SUM:=8a8e0ee8fefef150c08a649fd7da04aa

PKG_SOURCE_URL:=http://cxx.uclibc.org/src/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tbz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

IPKG_LIBCXX:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_LIBCXX:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_LIBCXX:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

UCLIBC_TARGET_ARCH:=$(shell echo $(ARCH) | sed -e s'/-.*//' \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh64/sh/' \
		-e 's/sh[234].*/sh/' \
		-e 's/mips.*/mips/' \
		-e 's/mipsel.*/mips/' \
		-e 's/cris.*/cris/' \
)


$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.config: $(PKG_BUILD_DIR)/.prepared
	cp ./uclibc++.config $(PKG_BUILD_DIR)/.config
	$(SED) 's,^.*TARGET_ARCH.*,TARGET_ARCH="$(UCLIBC_TARGET_ARCH)",g' $(PKG_BUILD_DIR)/.config
	$(SED) 's,^.*ARCH_CFLAGS.*,ARCH_CFLAGS="-mno-split-addresses $(TARGET_CFLAGS)",g' $(PKG_BUILD_DIR)/.config
	

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/lib/libuClibc++.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(IPKG_LIBCXX): $(STAGING_DIR)/lib/libuClibc++.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_LIBCXX) ./$(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_LIBCXX)/usr/lib
	cp -fpR $(PKG_BUILD_DIR)/src/libuClibc++*.so* $(IDIR_LIBCXX)/usr/lib/
	$(STRIP) $(IDIR_LIBCXX)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBCXX) $(PACKAGE_DIR)

$(INFO_LIBCXX): $(IPKG_LIBCXX)
	$(IPKG) install $(IPKG_LIBCXX)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_LIBCXX)
install: $(INFO_LIBCXX)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_LIBCXX)
	rm -f $(STAGING_DIR)/lib/libuClibc++.so* 
