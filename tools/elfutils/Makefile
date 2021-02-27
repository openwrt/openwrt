#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.180
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://sourceware.org/$(PKG_NAME)/ftp/$(PKG_VERSION)
PKG_HASH:=b827b6e35c59d188ba97d7cf148fa8dc6f5c68eb6c5981888dfdbb758c0b569d

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING COPYING-GPLV2 COPYING-LGPLV3
PKG_CPE_ID:=cpe:/a:elfutils_project:elfutils

HOST_BUILD_PARALLEL:=1
HOST_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--program-prefix=eu- \
	--disable-debuginfod \
	--without-bzlib

#define Host/Uninstall
#	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
#endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/lib/pkgconfig/lib{elf,dw}.pc
	rm -rf $(STAGING_DIR_HOST)/include/elfutils
	rm -f $(STAGING_DIR_HOST)/include/{libelf,gelf,nlist,dwarf}.h
	rm -f $(STAGING_DIR_HOST)/lib/lib{elf,dw,asm}{,-*}.{so,so.*,a}
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
