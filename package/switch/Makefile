# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-switch
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/kmod-switch
SECTION:=drivers
CATEGORY:=Drivers
DEPENDS:=@LINUX_2_6_BRCM||LINUX_2_4_BRCM
DEFAULT:=y
TITLE:=Switch drivers
DESCRIPTION:=Switch drivers for ADM6996L and BCM53XX RoboSwitch
VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="-DBCMGPIO2" \
		modules
endef
	
define Package/kmod-switch/install
	install -m0755 -d $(1)/lib/network/
	install -m0755 ./files/switch.sh $(1)/lib/network/
	install -m0755 -d $(1)/etc/modules.d/
	printf 'switch-core\nswitch-robo\nswitch-adm\n' > $(1)/etc/modules.d/20-switch
	install -m0755 -d $(1)/lib/modules/$(LINUX_VERSION)
	$(CP) $(PKG_BUILD_DIR)/*.$(LINUX_KMOD_SUFFIX) \
		$(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call BuildPackage,kmod-switch))
