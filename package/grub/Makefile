#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=grub
PKG_VERSION:=0.97
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://alpha.gnu.org/gnu/grub
PKG_MD5SUM:=cd3f3eb54446be6003156158d51f4884

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/grub-$(PKG_VERSION)
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

export grub_cv_prog_objcopy_absolute=yes

define Package/grub
  SECTION:=boot
  DEPENDS:=@TARGET_x86
  CATEGORY:=Boot Loaders
  TITLE:=GRand Unified Bootloader
  URL:=http://www.gnu.org/software/grub/
endef

ifeq ($(HOST_OS),Linux)
  NATIVEGRUB-i386:=1
  NATIVEGRUB-i686:=1
  NATIVEGRUB:=$(NATIVEGRUB-$(HOST_ARCH))
endif

ifeq ($(NATIVEGRUB),)
CONFIGURE_FLAGS:= \
	--target=$(GNU_TARGET_NAME) \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME)
else
CONFIGURE_FLAGS:=
endif

ifneq ($(HOST_OS),Darwin)
define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		LDFLAGS="-static" \
		./configure \
		$(CONFIGURE_FLAGS) \
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
		--disable-auto-linux-mem-opt \
	)
endef

#
# ./configure detects whether the host compiler supports
# -fno-stack-protector but only sets STAGE2_CFLAGS accordingly
#
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		GRUB_CFLAGS="\$$$$(STAGE2_CFLAGS)" \
		STAGE1_CFLAGS="\$$$$(STAGE2_CFLAGS)"
endef

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR_HOST)" \
		install
	mv $(STAGING_DIR_HOST)/usr/sbin/grub $(STAGING_DIR_HOST)/bin
endef
endif

$(eval $(call BuildPackage,grub))
