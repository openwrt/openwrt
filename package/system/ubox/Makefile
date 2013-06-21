include $(TOPDIR)/rules.mk

PKG_NAME:=ubox
PKG_VERSION:=2013-06-20
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/ubox.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=1b19fc217e33e9b2fc2fab6f9552da740d03a98a
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
CMAKE_INSTALL:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=John Crispin <blogic@openwrt.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

TARGET_LDFLAGS += $(if $(CONFIG_USE_EGLIBC),-lrt)

define Package/ubox
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=+ubusd +ubus +libuci +USE_EGLIBC:librt
  TITLE:=OpenWrt system helper toolbox
endef

define Package/block-mount
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Block device mounting and checking
  DEPENDS:=+ubox
  MENU:=1
endef

define Package/ubox/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin

	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/{mount_root,kmodloader,lsbloader,board} $(1)/sbin/
	ln -s /sbin/mount_root $(1)/sbin/switch2jffs
	ln -s /sbin/kmodloader $(1)/usr/sbin/lsmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/modinfo
endef

define Package/block-mount/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin $(1)/etc/hotplug.d/block

	$(CP) ./files/mount.hotplug $(1)/etc/hotplug.d/block/10-mount
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/block $(1)/sbin/
	ln -s /sbin/block $(1)/usr/sbin/swapon
	ln -s /sbin/block $(1)/usr/sbin/swapoff
endef

$(eval $(call BuildPackage,ubox))
$(eval $(call BuildPackage,block-mount))
