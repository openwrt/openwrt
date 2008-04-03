#
# Copyright (C) 2007,2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Image/Build/ZyXEL
	$(call Image/Build/TRXNoloader,$(call imgname,$(1),$(2)).trx,$(1))
endef

define Image/Build/Template/ZyXEL/Initramfs
	$(call Image/Build/LZMAKernel/KArgs,$(1),bin)
endef

#
# Profiles
#
define Image/Build/Profile/P334WT
	$(call Image/Build/Template/ZyXEL/$(1),p-334wt)
endef

define Image/Build/Profile/P335WT
	$(call Image/Build/Template/ZyXEL/$(1),p-335wt)
endef

define Image/Build/Profile/Generic
	$(call Image/Build/Profile/P334WT,$(1))
	$(call Image/Build/Profile/P335WT,$(1))
endef

