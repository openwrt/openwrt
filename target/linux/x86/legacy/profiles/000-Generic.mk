#
# Copyright (C) 2006-2019 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
  NAME:=Generic x86/legacy
  PACKAGES:=kmod-3c59x kmod-e100 kmod-e1000 kmod-natsemi kmod-ne2k-pci \
	kmod-pcnet32 kmod-8139too kmod-r8169 kmod-sis900 kmod-tg3 \
	kmod-via-rhine kmod-via-velocity
endef

define Profile/Generic/Description
	Generic Profile for x86 legacy architecutre
endef
$(eval $(call Profile,Generic))
