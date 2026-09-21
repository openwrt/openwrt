include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux-ntfs
PKG_RELEASE:=1
PKG_BUILD_PARALLEL:=1

PKG_SOURCE_DATE:=2026-09-21
PKG_SOURCE_URL:=https://github.com/namjaejeon/linux-ntfs
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=fdfea0c9a7b4e294daffa839e76bb41d69ec192c
PKG_MIRROR_HASH:=8644abf36f439e731b4857d574e2ecd96ebfa7ea548a70d5fb26a22a783d51c7

PKG_LICENSE:=GPL-2.0-or-later

PKG_MAINTAINER:=Qingfang Deng <dqfext@gmail.com>

include $(INCLUDE_DIR)/package.mk

define KernelPackage/fs-ntfs
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Filesystems
  TITLE:=NTFS file system support
  DEPENDS:=+kmod-nls-base
  URL:=$(PKG_SOURCE_URL)
  FILES:=$(PKG_BUILD_DIR)/ntfs.ko
  AUTOLOAD:=$(call AutoProbe,ntfs)
endef

define KernelPackage/fs-ntfs/description
  NTFS is the file system of Microsoft Windows NT, 2000, XP and 2003.

  This allows you to mount devices formatted with the ntfs file system.
endef

NOSTDINC_FLAGS += \
	$(KERNEL_NOSTDINC_FLAGS)

EXTRA_KCONFIG:= \
	CONFIG_NTFS_FS=m \
	CONFIG_NTFS_FS_WOF_COMPRESSION=y

MAKE_OPTS:= \
	M="$(PKG_BUILD_DIR)" \
	NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,fs-ntfs))
