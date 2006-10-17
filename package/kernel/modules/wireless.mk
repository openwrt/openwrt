# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

WIMENU:=Wireless Drivers

define KernelPackage/bcm43xx
  TITLE:=Broadcom BCM43xx driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- bcm43xx
  DEPENDS:=kmod-ieee80211softmac
  KCONFIG:=$(CONFIG_BCM43XX)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/bcm43xx/bcm43xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,50,bcm43xx)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,bcm43xx))

define KernelPackage/ipw2100
  TITLE:=Intel IPw2100 driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- ipw2100
  DEPENDS:=kmod-ieee80211softmac
  KCONFIG:=$(CONFIG_IPW2100)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2100.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,50,ipw2100)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,ipw2100))

define KernelPackage/ipw2200
  TITLE:=Intel IPw2200 driver
  DESCRIPTION:=\\\
	\\\
	Includes: \\\
	- ipw2200
  DEPENDS:=kmod-ieee80211softmac
  KCONFIG:=$(CONFIG_IPW2200)
  FILES:=$(MODULES_DIR)/kernel/drivers/net/wireless/ipw2200.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call Autoload,50,ipw2200)
  SUBMENU:=$(WIMENU)
endef
$(eval $(call KernelPackage,ipw2200))



