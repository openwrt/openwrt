include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-diskman
LUCI_NAME:=luci-app-diskman
PKG_VERSION:=v0.2.13
PKG_RELEASE:=beta
PKG_MAINTAINER:=lisaac <https://github.com/lisaac/luci-app-diskman>
PKG_LICENSE:=AGPL-3.0

LUCI_TITLE:=Disk Manager interface for LuCI

LUCI_DEPENDS:=+luci-compat +luci-lib-ipkg +e2fsprogs +parted +smartmontools +blkid \
	+kmod-fs-vfat +dosfstools +kmod-fs-msdos +kmod-nls-base +kmod-nls-utf8 +kmod-nls-cp932 +kmod-nls-cp936 +kmod-nls-cp950 \
	+kmod-fs-exfat +exfat-mkfs +exfat-fsck \
	+kmod-fs-ntfs \
	+kmod-fs-btrfs \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ntfs_3g_utils:ntfs-3g-utils \
	+PACKAGE_$(PKG_NAME)_INCLUDE_btrfs_progs:btrfs-progs \
	+PACKAGE_$(PKG_NAME)_INCLUDE_lsblk:lsblk \
	+PACKAGE_$(PKG_NAME)_INCLUDE_mdadm:mdadm \
	+PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_raid456:mdadm \
	+PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_raid456:kmod-md-raid456 \
	+PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_linears:mdadm \
	+PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_linears:kmod-md-linear

LUCI_DESCRIPTION:=Disk Manager interface for LuCI

define Package/$(PKG_NAME)/config
config PACKAGE_$(PKG_NAME)_INCLUDE_ntfs_3g_utils
	depends on PACKAGE_$(PKG_NAME)
	bool "Include ntfs-3g-utils"
	default y
config PACKAGE_$(PKG_NAME)_INCLUDE_btrfs_progs
	depends on PACKAGE_$(PKG_NAME)
	bool "Include btrfs-progs"
	default y
config PACKAGE_$(PKG_NAME)_INCLUDE_lsblk
	depends on PACKAGE_$(PKG_NAME)
	bool "Include lsblk"
	default y
config PACKAGE_$(PKG_NAME)_INCLUDE_mdadm
	depends on PACKAGE_$(PKG_NAME)
	bool "Include mdadm"
	default n
config PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_raid456
	depends on PACKAGE_$(PKG_NAME)_INCLUDE_mdadm
	bool "Include kmod-md-raid456"
	default n
config PACKAGE_$(PKG_NAME)_INCLUDE_kmod_md_linears
	depends on PACKAGE_$(PKG_NAME)_INCLUDE_mdadm
	bool "Include kmod-md-linear"
	default n
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
