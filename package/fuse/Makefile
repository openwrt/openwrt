# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fuse
PKG_VERSION:=2.9.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=894ee11674f89a915ae87524aed55bc4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/fuse/Default
  TITLE:=FUSE
  URL:=http://fuse.sourceforge.net/
endef

define Package/fuse/Default/description
 FUSE (Filesystem in UserSpacE)
endef

define Package/fuse-utils
$(call Package/fuse/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libfuse
  TITLE+= (utilities)
  SUBMENU:=Filesystem
endef

define Package/fuse-utils/description
$(call Package/fuse/Default/description)
 This package contains the FUSE utilities.
 - fusermount
 - ulockmgr_server
endef

define Package/libfuse
$(call Package/fuse/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DEPENDS:=+kmod-fuse +libpthread
  SUBMENU:=Filesystem
endef

define Package/libfuse/description
$(call Package/fuse/Default/description)
 This package contains the FUSE shared libraries, needed by other programs.
 - libfuse
 - libulockmgr
endef

# generic args
CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--disable-example \
	--disable-mtab

# generic package uses lib & utils
CONFIGURE_ARGS += --enable-lib --enable-util

define Build/InstallDev
	@echo "--> Build/InstallDev enter"
	mkdir -p $(1)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/{fuse,*.h} $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/*.{a,so*} $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/fuse.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${includedir}/fuse,,g' $(1)/usr/lib/pkgconfig/fuse.pc
	$(SED) 's,-L$$$${libdir},,g' $(1)/usr/lib/pkgconfig/fuse.pc
	@echo "--> Build/InstallDev leave"
endef

define Package/fuse-utils/install
	@echo "--> Package/fuse-utils/install enter"
	$(INSTALL_DIR) $(1)/usr/bin
	# use cp and keep fusermount sticky bit
	$(FIND) $(PKG_INSTALL_DIR)/usr/bin/ -type f -exec $(CP) -a {} $(1)/usr/bin/ \;
	@echo "--> Package/fuse-utils/install leave"
endef

define Package/libfuse/install
	@echo "--> Package/libfuse/install enter"
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so.* $(1)/usr/lib/
	@echo "--> Package/libfuse/install leave"
endef

define Package/kmod-fuse/install
	@echo "--> Package/kmod-fuse/install enter/leave"
endef

$(eval $(call BuildPackage,fuse-utils))
$(eval $(call BuildPackage,libfuse))
