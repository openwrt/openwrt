include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux-ntfs
PKG_RELEASE:=1
PKG_BUILD_PARALLEL:=1

PKG_SOURCE_DATE:=2026-09-04
PKG_SOURCE_URL:=https://github.com/namjaejeon/linux-ntfs
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=d6d09e6f017f45aae062e05e64301f43a2c66ea1
PKG_MIRROR_HASH:=4c42a346217ed7e0294897312f39d7667c27f2fb08edc724f6367fc3b7321456

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

EXTRA_KCONFIG:= \
	CONFIG_NTFS_FS=m \
	CONFIG_NTFS_FS_WOF_COMPRESSION=y

NOSTDINC_FLAGS += \
	$(KERNEL_NOSTDINC_FLAGS) \
	$(patsubst CONFIG_%,-DCONFIG_%=1,$(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

MAKE_OPTS:= \
	M="$(PKG_BUILD_DIR)" \
	NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	+$(KERNEL_MAKE) $(PKG_JOBS) $(MAKE_OPTS) modules
endef

$(eval $(call KernelPackage,fs-ntfs))
