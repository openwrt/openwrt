#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=broadcom-57xx
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define KernelPackage/brcm-57xx
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_brcm_2_4 +kmod-switch
  TITLE:=Broadcom 57xx ethernet support
  FILES:=$(PKG_BUILD_DIR)/bcm57xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,bcm57xx)
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

ifeq ($(BOARD),brcm-2.4)
  BUILDFLAGS := \
	-DBCMDRIVER -I$(LINUX_DIR)/arch/mips/bcm947xx/include $(TARGET_CPPFLAGS) \
	-DDBG=0 -DBCM_PROC_FS=1 -DT3_JUMBO_RCV_RCB_ENTRY_COUNT=256 -DNICE_SUPPORT \
	-DPCIX_TARGET_WORKAROUND=1 -DINCLUDE_TBI_SUPPORT=1 -DINCLUDE_5701_AX_FIX=1
endif

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

define KernelPackage/brcm-57xx/install
	$(call Package/brcm-57xx/install/Default,$(1))
	#$(INSTALL_DIR) $(1)/etc/modules.d
	#echo "bcm57xx activate_gpio=1" > $(1)/etc/modules.d/30-bcm57xx
endef

$(eval $(call KernelPackage,brcm-57xx))
