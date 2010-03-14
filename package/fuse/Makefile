# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fuse
PKG_VERSION:=2.8.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=517c8384f915e40647bda9f71579fd97

PKG_INSTALL:=1

# Kernel 2.4 Module
FUSE24_VERSION=2.5.3
FUSE24_SOURCE:=$(PKG_NAME)-$(FUSE24_VERSION).tar.gz
FUSE24_MD5SUM:=9c7e8b6606b9f158ae20b8521ba2867c
FUSE24_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(FUSE24_VERSION)
FUSE24_INSTALL_DIR:=$(FUSE24_BUILD_DIR)/ipkg-install
FUSE24_PATCH_DIR:=./patches-kernel24

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

define KernelPackage/fuse
	$(call Package/fuse/Default)
	SUBMENU:=Filesystems
	TITLE+= (kernel module)
	ifeq ($(KERNEL),2.4)
		FILES:=$(FUSE24_INSTALL_DIR)/lib/modules/$(LINUX_VERSION)/kernel/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
		VERSION:=$(LINUX_VERSION)+$(FUSE24_VERSION)-$(PKG_RELEASE)
	else
		KCONFIG:= CONFIG_FUSE_FS
		FILES:=$(LINUX_DIR)/fs/fuse/fuse.$(LINUX_KMOD_SUFFIX)
		VERSION:=$(LINUX_VERSION)-$(LINUX_RELEASE)
	endif
	AUTOLOAD:=$(call AutoLoad,80,fuse)
endef

define KernelPackage/fuse/description
$(call Package/fuse/Default/description)
 This package contains the FUSE kernel module.
endef

define Package/libfuse
$(call Package/fuse/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DEPENDS:=kmod-fuse
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

# kmod 2.4 args
FUSE24_CONFIGURE_ARGS := $(CONFIGURE_ARGS) \
	--disable-lib --disable-util \
	--enable-kernel-module --disable-auto-modprobe \
	--with-kernel="$(LINUX_DIR)"

# generic package uses lib & utils
CONFIGURE_ARGS += --enable-lib --enable-util

####### FUSE24 - START ######
### if we're on kernel 2.4 build a kernel 2.4 compatible fuse module from older version, see also
### http://sourceforge.net/apps/mediawiki/fuse/index.php?title=FAQ#What_version_of_FUSE_do_I_need_to_use_FUSE_with_Linux_2.4.3F
ifeq ($(KERNEL),2.4)

define Download/fuse24
	FILE:=$(FUSE24_SOURCE)
	URL:=$(PKG_SOURCE_URL)
	MD5SUM:=$(FUSE24_MD5SUM)
endef
$(eval $(call Download,fuse24))

define Build/Prepare/fuse24
	rm -rf $(FUSE24_BUILD_DIR)/
	mkdir -p $(FUSE24_BUILD_DIR)/
	$(TAR) -xzf $(DL_DIR)/$(FUSE24_SOURCE) -C $(FUSE24_BUILD_DIR)/..
endef

define Build/Patch/fuse24
	$(call PatchDir,$(FUSE24_BUILD_DIR),$(FUSE24_PATCH_DIR),)
endef

define Build/Configure/fuse24
	(cd $(FUSE24_BUILD_DIR); \
		rm -rf config.{cache,status} ; \
		$(CONFIGURE_VARS) \
		./configure \
			$(FUSE24_CONFIGURE_ARGS) \
	);
endef

define Build/Compile/fuse24
	$(MAKE) -C $(FUSE24_BUILD_DIR)/$(MAKE_PATH) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		all;
endef

define Build/Install/fuse24
	rm -rf $(FUSE24_INSTALL_DIR)
	mkdir -p $(FUSE24_INSTALL_DIR)
	$(MAKE) -C $(FUSE24_BUILD_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		DESTDIR="$(FUSE24_INSTALL_DIR)" \
		install
endef

define Build/Prepare
	@echo "--> Build/Prepare enter"
	$(call Build/Prepare/fuse24)
	$(call Build/Patch/fuse24)
	$(call Build/Prepare/Default)
	@echo "--> Build/Prepare leave"
endef

define Build/Configure
	@echo "--> Build/Configure enter"
	$(call Build/Configure/fuse24)
	$(call Build/Configure/Default)
	@echo "--> Build/Configure leave"
endef

define Build/Compile
	@echo "--> Build/Compile enter"
	$(call Build/Compile/fuse24)
	$(call Build/Compile/Default)
	@echo "--> Build/Compile leave"
endef

define Build/Install
	@echo "--> Build/Install enter"
	$(call Build/Install/fuse24)
	$(call Build/Install/Default)
	@echo "--> Build/Install leave"
endef

endif
####### FUSE24 - END ######

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
$(eval $(call KernelPackage,fuse))
