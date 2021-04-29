#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ntfs3-oot
PKG_VERSION:=20220501

PKG_SOURCE_URL:=https://codeload.github.com/ptpt52/ntfs3-oot/tar.gz/$(PKG_VERSION)?
PKG_HASH:=3144b3afb754d5fa80093d8e88daae023224ea5c467cb4c8ac784ab47dcea394
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_MAINTAINER:=Chen Minqiang <ptpt52@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

TAR_OPTIONS+= --strip-components 1
TAR_CMD=$(HOST_TAR) -C $(1) $(TAR_OPTIONS)

define KernelPackage/fs-ntfs3-oot
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Filesystems
  TITLE:=NTFS3 kernel module
  FILES:=$(PKG_BUILD_DIR)/ntfs3.ko
  AUTOLOAD:=$(call AutoProbe,ntfs3)
  DEPENDS:= +kmod-nls-base @(LINUX_5_10||LINUX_5_4||LINUX_4_19||LINUX_4_14)
endef

define KernelPackage/ntfs3/description
  This package provides the kernel module for ntfs3.
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(PKG_EXTRA_KCONFIG) \
	CONFIG_NTFS3_FS=m \
	CONFIG_NTFS3_LZX_XPRESS=y \
	modules
endef

$(eval $(call KernelPackage,fs-ntfs3-oot))
