# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)
PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bridge
PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

define Package/bridge
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
TITLE:=Ethernet bridging configuration utility
DESCRIPTION:=Manage ethernet bridging; a way to connect networks together to\\\
form a larger network.
URL:=http://bridge.sourceforge.net/
endef

define Build/Configure
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
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
		--with-linux-headers=$(LINUX_DIR)
endef


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $@
endef

define Package/busybox/install
	mkdir -p $(IDIR_BRIDGE)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/brctl/brctl $(IDIR_BRIDGE)/usr/sbin/
	$(STRIP) $(IDIR_BRIDGE)/usr/sbin/brctl
	$(IPKG_BUILD) $(IDIR_BRIDGE) $(PACKAGE_DIR)
endef

$(eval $(call BuildPackage,bridge))
