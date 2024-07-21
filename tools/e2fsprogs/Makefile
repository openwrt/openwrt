#
# Copyright (C) 2010-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_CPE_ID:=cpe:/a:e2fsprogs_project:e2fsprogs
PKG_VERSION:=1.47.0
PKG_HASH:=0b4fe723d779b0927fb83c9ae709bc7b40f66d7df36433bef143e41c54257084

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@KERNEL/linux/kernel/people/tytso/e2fsprogs/v$(PKG_VERSION)/

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

ifneq ($(shell $(HOSTCC) --version | grep clang),)
  HOST_CFLAGS += -D__GNUC_PREREQ\(...\)=0 -Dloff_t=off_t
endif
HOST_CFLAGS += $(HOST_FPIC)

HOST_CONFIGURE_ARGS += \
	--disable-elf-shlibs \
	--disable-libuuid \
	--disable-tls \
	--disable-nls \
	--enable-threads=pth \
	--disable-fuse2fs

define Host/Prepare
	$(call Host/Prepare/Default)
	rm -rf $(HOST_BUILD_DIR)/doc
endef

define Host/Install
	$(call Host/Install/Default)
	$(SED) 's|^DIR=.*|DIR=$$$${STAGING_DIR_HOST}/share/et|' $(STAGING_DIR_HOST)/bin/compile_et
	$(SED) 's|^DIR=.*|DIR=$$$${STAGING_DIR_HOST}/share/ss|' $(STAGING_DIR_HOST)/bin/mk_cmds
endef

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
