#
# Copyright (C) 2022 Ye Jiaqiang, Tony Ambardar
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ply
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/iovisor/ply.git
PKG_MIRROR_HASH:=518d94fa46329bf35a0d33a05b054384c30f21c32afb58b4aafc42b3f62e511c
PKG_SOURCE_DATE:=2025-08-22
PKG_SOURCE_VERSION:=53cd6c4575c9055f36468f9453f5e51ba0aad20b

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=COPYING
PKG_MAINTAINER:=Tony Ambardar <itugrok@yahoo.com>

include $(INCLUDE_DIR)/package.mk

define Package/ply
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Light-weight dynamic tracer for Linux
  URL:=https://github.com/iovisor/ply.git
  DEPENDS:= \
	@KERNEL_BPF_EVENTS @KERNEL_DYNAMIC_FTRACE @KERNEL_DEBUG_FS \
	@mips||mipsel||mips64||mips64el||x86_64||powerpc||arm_v7||aarch64||riscv||riscv64
endef

define Package/ply/description
  A light-weight dynamic tracer for Linux that leverages the kernel's
  BPF VM in concert with kprobes and tracepoints to attach probes to
  arbitrary points in the kernel.
endef

CONFIGURE_ARGS += --enable-shared=yes --enable-static=no

define Build/Prepare
	$(call Build/Prepare/Default)
	cd $(PKG_BUILD_DIR) && exec ./autogen.sh
endef

define Package/ply/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ply/.libs/ply $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/src/libply/.libs/libply.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ply))
