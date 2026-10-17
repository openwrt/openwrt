#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=f2fs-tools
PKG_VERSION:=1.17.0
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-tools.git
PKG_SOURCE_VERSION:=$(PKG_VERSION)
PKG_MIRROR_HASH:=1b44cf0785d10a4912f3c94d3bd53f883a710c4702b0133feccaa496bbc132ef

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:f2fs-tools_project:f2fs-tools

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/f2fs-tools/Default
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  DEPENDS:=+libf2fs
  URL:=https://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-tools.git
endef

define Package/mkf2fs
  $(Package/f2fs-tools/Default)
  TITLE:=Utility for creating a Flash-Friendly File System (F2FS)
endef

define Package/f2fsck
  $(Package/f2fs-tools/Default)
  TITLE:=Utility for checking/repairing a Flash-Friendly File System (F2FS)
  CONFLICTS:=f2fsck-selinux
  VARIANT:=default
  DEFAULT_VARIANT:=1
endef

define Package/f2fsck-selinux
  $(Package/f2fs-tools/Default)
  TITLE:=Utility for checking/repairing a Flash-Friendly File System (F2FS) with SELinux support
  DEPENDS += +libselinux
  PROVIDES:=f2fsck
  VARIANT:=selinux
endef

define Package/f2fs-tools
  $(Package/f2fs-tools/Default)
  TITLE:=Tools for Flash-Friendly File System (F2FS)
  DEPENDS += +mkf2fs +f2fsck
endef

define Package/libf2fs
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Library for Flash-Friendly File System (F2FS) tools
  DEPENDS:=+libuuid
  ABI_VERSION:=11
endef

CONFIGURE_ARGS += \
	--disable-static \
	--without-blkid \
	--without-lzo2 \
	--without-lz4

CONFIGURE_VARS += \
	ac_cv_file__git=no

CONFIGURE_ARGS += $(if $(filter selinux,$(BUILD_VARIANT)),--with,--without)-selinux

# We share one libf2fs between variants: validate that only fsck uses selinux.
define Build/check-selinux-usage
	grep -rEn $(addprefix --exclude-dir=,fsck patches .pc) \
		$(addprefix --exclude=,configure.ac f2fs_fs.h android_config.h) \
		'libselinux_LIBS|HAVE_LIBSELINUX' "$(PKG_BUILD_DIR)"; test "$$$$?" -eq 1
endef
Hooks/Prepare/Post += Build/check-selinux-usage

define Package/libf2fs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libf2fs.so.* $(1)/usr/lib/
endef

define Package/mkf2fs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mkfs.f2fs $(1)/usr/sbin
endef

define Package/f2fsck/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/fsck.f2fs $(1)/usr/sbin
	$(LN) ../sbin/fsck.f2fs $(1)/usr/sbin/defrag.f2fs
	$(LN) ../sbin/fsck.f2fs $(1)/usr/sbin/dump.f2fs
	$(LN) ../sbin/fsck.f2fs $(1)/usr/sbin/sload.f2fs
	$(LN) ../sbin/fsck.f2fs $(1)/usr/sbin/resize.f2fs
	$(LN) ../sbin/fsck.f2fs $(1)/usr/sbin/f2fslabel
endef

Package/f2fsck-selinux/install = $(Package/f2fsck/install)

define Package/f2fs-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/f2fs_io $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/fibmap.f2fs $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/parse.f2fs $(1)/usr/sbin
endef

$(eval $(call BuildPackage,libf2fs))
$(eval $(call BuildPackage,mkf2fs))
$(eval $(call BuildPackage,f2fsck))
$(eval $(call BuildPackage,f2fsck-selinux))
$(eval $(call BuildPackage,f2fs-tools))
