# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipkg
PKG_VERSION:=0.99.163
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ftp.gwdg.de/pub/linux/handhelds/packages/ipkg \
	http://www.handhelds.org/packages/ipkg \
	http://www.gtlib.gatech.edu/pub/handhelds.org/packages/ipkg
PKG_MD5SUM:=0b10ad2924611bccaea8ddf98481a192

PKG_FIXUP = libtool

include $(INCLUDE_DIR)/package.mk

define Package/ipkg
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Itsy package management system
  URL:=http://handhelds.org/moin/moin.cgi/Ipkg
endef

define Package/ipkg/description
 Lightweight package management system
 ipkg is the Itsy Package Management System, for handling
 installation and removal of packages on a system. It can
 recursively follow dependencies and download all packages
 necessary to install a particular package.
 .
 ipkg knows how to install both .ipk and .deb packages.
endef

TARGET_CFLAGS += $(FPIC)
CONFIGURE_ARGS += $(DISABLE_LARGEFILE)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/ipkg/preinst
	if [ -L $${IPKG_INSTROOT}/usr/bin/ipkg ] ; then
		echo -e "Removing busybox ipkg symlink"
		rm $${IPKG_INSTROOT}/usr/bin/ipkg
	fi
endef

define Package/ipkg/postrm
	if [ "$(busybox ipkg 2>&1)" != "busybox: applet not found" ] ; then
		echo -e "Reinstalling busybox ipkg symlink"
		ln -sf ../../bin/busybox $${IPKG_INSTROOT}/usr/bin/ipkg
	fi
endef

define Package/ipkg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ipkg-cl $(1)/usr/bin/ipkg
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipkg.so.* $(1)/usr/lib/
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libipkg $(1)/usr/include/
endef

$(eval $(call BuildPackage,ipkg))
