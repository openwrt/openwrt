#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=secilc
PKG_VERSION:=3.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/SELinuxProject/selinux/releases/download/$(PKG_VERSION)
PKG_HASH:=1fb7de005d35ae0ac6ff8f8fc955d08a15b5c1a67e961b97ef987576af0db11a
HOST_BUILD_DEPENDS:=libsepol/host

PKG_MAINTAINER:=Dominick Grift <dominick.grift@defensec.nl>
PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/nls.mk
include $(INCLUDE_DIR)/package.mk

HOST_LDFLAGS+=-Wl,-rpath=$(STAGING_DIR_HOSTPKG)/lib
HOST_MAKE_FLAGS += \
	DESTDIR=$(STAGING_DIR_HOSTPKG) \
	PREFIX=

define Package/secilc
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=SELinux Common Intermediate Language (CIL) Compiler
	URL:=https://selinuxproject.github.io/
	DEPENDS:=+libsepol
endef

define Package/secilc/description
	The SELinux CIL Compiler is a compiler that converts the CIL language as
	described on the CIL design wiki into a kernel binary policy file.
	Please see the CIL Design Wiki at:
	https://github.com/SELinuxProject/cil/wiki/
	for more information about the goals and features on the CIL language.
endef

define Build/Compile
	$(call Build/Compile/Default,secilc)
endef

define Host/Compile
	$(call Host/Compile/Default,secilc)
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOSTPKG)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/secilc $(STAGING_DIR_HOSTPKG)/bin
endef

define Package/secilc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/secilc $(1)/usr/bin
endef

$(eval $(call BuildPackage,secilc))
$(eval $(call HostBuild))
