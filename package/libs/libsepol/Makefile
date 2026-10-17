#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsepol
PKG_VERSION:=3.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/SELinuxProject/selinux/releases/download/$(PKG_VERSION)
PKG_HASH:=79f3d2c88f44b7eb5cf54d9792e03232297e17f97a179163f2750099a00f164d
PKG_LICENSE:=LGPL-2.1-or-later
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Thomas Petazzoni <thomas.petazzoni@bootlin.com>
PKG_CPE_ID:=cpe:/a:selinuxproject:libsepol

PKG_BUILD_FLAGS:=no-lto

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define Package/libsepol
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=SELinux binary policy manipulation library
  URL:=https://selinuxproject.github.io/
endef

define Package/libsepol/description
	Libsepol is the binary policy manipulation library. It doesn't
	depend upon or use any of the other SELinux components.
endef

define Package/chkcon
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libsepol
  TITLE:=libsepol chkcon security context validation tool
  URL:=https://selinuxproject.github.io/
endef

define Package/chkcon/description
	chkcon - determine if a security context is valid for a given binary policy
endef

HOST_MAKE_FLAGS += \
	PREFIX=$(STAGING_DIR_HOSTPKG) \
	SHLIBDIR=$(STAGING_DIR_HOSTPKG)/lib

MAKE_FLAGS += \
	SHLIBDIR=/usr/lib \
	OS=Linux

define Build/Compile
	$(call Build/Compile/Default,all)
endef

define Build/Install
	$(call Build/Install/Default,install)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libsepol.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,/usr/include,$$$${prefix}/include,g' $(1)/usr/lib/pkgconfig/libsepol.pc
	$(SED) 's,/usr/lib,$$$${exec_prefix}/lib,g' $(1)/usr/lib/pkgconfig/libsepol.pc
endef

define Package/libsepol/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsepol.so.* $(1)/usr/lib/
endef

define Package/chkcon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/chkcon $(1)/usr/bin/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libsepol))
$(eval $(call BuildPackage,chkcon))
