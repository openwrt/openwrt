# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r8101
PKG_VERSION:=1.012.00
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://WebUser:nQJ4P7b@202.65.194.212/cn/nic/
PKG_MD5SUM:=eb0645af023926f3ab8d0d40f68748ef

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8101
  SUBMENU:=Network Devices
  TITLE:=RealTek RTL-8101E PCIe Fast Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86 @!LINUX_2_6_31
  URL:=http://www.realtek.com.tw/
  FILES:= $(PKG_BUILD_DIR)/src/r8101.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,r8101)
endef

define KernelPackage/r8101/description
 Kernel modules for RealTek RTL-8101E PCI-Express Fast Ethernet adapters.
endef

define Build/Compile
	chmod u+x $(PKG_BUILD_DIR)/src
	$(MAKE) -C $(LINUX_DIR) \
		SUBDIRS="$(PKG_BUILD_DIR)/src" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		modules
endef

$(eval $(call KernelPackage,r8101))
