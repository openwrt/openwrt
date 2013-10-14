include $(TOPDIR)/rules.mk

PKG_NAME:=ubox
PKG_VERSION:=2013-10-14
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/luci2/ubox.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=f56267fc14f39f055bc287658fb5200e9cb40784
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
  DEPENDS:=+libubox +ubusd +ubus +libuci +USE_EGLIBC:librt
  TITLE:=OpenWrt system helper toolbox
endef

define Package/block-mount
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Block device mounting and checking
  DEPENDS:=+ubox +libubox +libuci
endef

define Package/ubox/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{mount_root,kmodloader} $(1)/sbin/
	ln -s /sbin/mount_root $(1)/sbin/switch2jffs
	ln -s /sbin/mount_root $(1)/sbin/jffs2reset
	ln -s /sbin/mount_root $(1)/sbin/jffs2mark
	ln -s /sbin/kmodloader $(1)/usr/sbin/rmmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/insmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/lsmod
	ln -s /sbin/kmodloader $(1)/usr/sbin/modinfo
	ln -s /sbin/kmodloader $(1)/usr/sbin/modprobe
endef

define Package/block-mount/install
	$(INSTALL_DIR) $(1)/sbin $(1)/usr/sbin $(1)/etc/hotplug.d/block $(1)/etc/init.d/ $(1)/etc/uci-defaults/

	$(INSTALL_BIN) ./files/fstab.init $(1)/etc/init.d/fstab
	$(INSTALL_DATA) ./files/fstab.default $(1)/etc/uci-defaults/10-fstab
	$(INSTALL_DATA) ./files/mount.hotplug $(1)/etc/hotplug.d/block/10-mount

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/block $(1)/sbin/
	ln -s /sbin/block $(1)/usr/sbin/swapon
	ln -s /sbin/block $(1)/usr/sbin/swapoff

endef

$(eval $(call BuildPackage,ubox))
$(eval $(call BuildPackage,block-mount))
