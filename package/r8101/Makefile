# 
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r8101
PKG_VERSION:=1.014.00
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	ftp://WebUser:nQJ4P7b@202.134.71.22/cn/nic/ \
	ftp://WebUser:nQJ4P7b@66.104.77.130/cn/nic/ \
	ftp://WebUser:nQJ4P7b@61.56.86.122/cn/nic/
PKG_MD5SUM:=dace75093a1439310750029ccebe2c15

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8101
  SUBMENU:=Network Devices
  TITLE:=RealTek RTL-8101E PCIe Fast Ethernet Adapter kernel support
  DEPENDS:=@TARGET_x86
  URL:=http://www.realtek.com.tw/
  FILES:= $(PKG_BUILD_DIR)/src/r8101.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,r8101)
endef

define KernelPackage/r8101/description
 Kernel modules for RealTek RTL-8101E PCI-Express Fast Ethernet adapters.
endef

include $(INCLUDE_DIR)/kernel-defaults.mk

define Build/Configure
endef

define Build/Compile
	chmod u+x $(PKG_BUILD_DIR)/src
	$(MAKE) $(KERNEL_MAKEOPTS) \
		M="$(PKG_BUILD_DIR)/src" \
		modules
endef

$(eval $(call KernelPackage,r8101))
