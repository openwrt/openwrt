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

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,UCLIBCXX,uclibc++,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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

$(PKG_BUILD_DIR)/.configured:
	cp ./uclibc++.config $(PKG_BUILD_DIR)/.config
	$(SED) 's,^.*TARGET_ARCH.*,TARGET_ARCH="$(UCLIBC_TARGET_ARCH)",g' $(PKG_BUILD_DIR)/.config
	$(SED) 's,^.*ARCH_CFLAGS.*,ARCH_CFLAGS="-mno-split-addresses $(TARGET_CFLAGS)",g' $(PKG_BUILD_DIR)/.config
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) CROSS="$(TARGET_CROSS)" \
		all
	touch $@

$(IPKG_UCLIBCXX):
	install -d -m0755 $(IDIR_UCLIBCXX)/usr/lib
	cp -fpR $(PKG_BUILD_DIR)/src/libuClibc++*.so* $(IDIR_UCLIBCXX)/usr/lib/
	$(STRIP) $(IDIR_UCLIBCXX)/usr/lib/*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_UCLIBCXX) $(PACKAGE_DIR)

compile: install-dev
install-dev:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) CROSS="$(TARGET_CROSS)" \
		DESTDIR="$(STAGING_DIR)" \
		install
