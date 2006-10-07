# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gdbserver
PKG_VERSION:=6.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

GDB_DIR:=$(TOOL_BUILD_DIR)/gdb-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/gdbserver
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@GDB@
  TITLE:=Remote Server for the GNU Debugger
  DESCRIPTION:=GDBSERVER is a program that allows you to run GDB on a different machine\\\
than the one which is running the program being debugged.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		$(GDB_DIR)/gdb/gdbserver/configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/bin \
		  --sbindir=/usr/sbin \
		  --libexecdir=/usr/lib \
		  --sysconfdir=/etc \
		  --datadir=/usr/share \
		  --localstatedir=/var \
		  --mandir=/usr/man \
		  --infodir=/usr/info \
		  $(DISABLE_NLS) \
		  $(DISABLE_LARGEFILE) \
	);
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/gdbserver/install	
	install -m0755 -d $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/gdbserver $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gdbserver))

