# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=base-files
PKG_RELEASE:=7
PKG_BUILD_DIR:=$(BUILD_DIR)/base-files

BOARD:=$(shell LANG=C grep ^CONFIG_LINUX_2_._ $(TOPDIR)/.config|sed -e "s/=y//" \
	-e "s/.*_//"| tr "[:upper:]" "[:lower:]")
KERNEL:=$(shell LANG=C grep ^CONFIG_LINUX_2_._ $(TOPDIR)/.config|sed -e "s/=y//" \
	-e "s/CONFIG_LINUX_//" -e "s/_BRCM//" -e "s/_/./"| tr "[:upper:]" "[:lower:]")

REV:=$(shell LANG=C svn info | awk '/^Revision:/ { print$$2 }' )
ifeq ($(REV),)
REV:=0
endif

include $(TOPDIR)/package/rules.mk

#FIXME: use the version from ./toolchain/uclibc
UCLIBC_VERSION:=0.9.28
LIBGCC_VERSION:=$(CONFIG_GCC_VERSION)

define Package/base-files
 SECTION:=base
 CATEGORY:=Base system
 DEFAULT:=y
 TITLE:=OpenWrt system scripts
 DESCRIPTION:=OpenWrt system scripts
endef

define Package/libgcc
 SECTION:=libs
 CATEGORY:=Libraries
 DEFAULT:=m, y if GCC_VERSION_4_0_2 || GCC_VERSION_4_0_3 || GCC_VERSION_4_1_0
 TITLE:=GCC support library
 VERSION:=$(LIBGCC_VERSION)-$(PKG_RELEASE)
 DESCRIPTION:=$(TITLE)
endef

define Package/uclibc
 $(call Package/base-files)
 VERSION:=$(UCLIBC_VERSION)-$(PKG_RELEASE)
 TITLE:=C library
 DESCRIPTION:=C library for embedded systems
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define Package/base-files/install
	$(CP) ./default/* $(1)
	$(SED) 's,$$$$R,r$(REV),g' $(1)/etc/banner
	$(SED) 's,$$$$S,$(BOARD)-$(KERNEL),g' $(1)/etc/ipkg.conf
	mkdir -p $(1)/dev
	mkdir -p $(1)/etc/crontabs
	mkdir -p $(1)/jffs
	mkdir -p $(1)/lib
	mkdir -p $(1)/mnt
	mkdir -p $(1)/proc
	mkdir -p $(1)/tmp
	mkdir -p $(1)/usr/lib
	mkdir -p $(1)/usr/bin
	mkdir -p $(1)/sys
	mkdir -p $(1)/www
	ln -sf /tmp/resolv.conf $(1)/etc/resolv.conf
	ln -sf /proc/mounts $(1)/etc/mtab
	rm -f $(1)/var
	ln -sf /tmp $(1)/var
	-find $(1) -type d -name CVS | xargs rm -rf
	-find $(1) -type d -name .svn | xargs rm -rf
	-find $(1) -name '.#*' | xargs rm -f
	mkdir -p $(1)/etc
endef

define Package/uclibc/install
	mkdir -p $(1)/lib
	for file in ld-uClibc libc libcrypt libdl libm libnsl libresolv librt libuClibc libutil; do \
		$(CP) $(STAGING_DIR)/lib/$$$$file.so.* $(1)/lib/; \
		$(CP) $(STAGING_DIR)/lib/$$$$file-$(UCLIBC_VERSION).so $(1)/lib/; \
	done
endef

define Package/libgcc/install
	mkdir -p $(1)/lib
	$(CP) $(STAGING_DIR)/lib/libgcc_s.so.* $(1)/lib/
endef

$(eval $(call BuildPackage,base-files))
$(eval $(call BuildPackage,libgcc))
$(eval $(call BuildPackage,uclibc))
