#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Image/Build/ZyXEL
	$(call Image/Build/TRXNoloader,$(call imgname,$(1),$(2)).trx,$(1))
endef

#
# ZyXEL Prestige 334WT
#
define Image/Build/Board/P-334WT
	$(call Image/Build/ZyXEL,$(1),p-334wt)
endef

define Image/Build/Board/P-334WT/squashfs
	$(call Image/Build/Board/P-334WT,squashfs)
endef

define Image/Build/Board/P-334WT/jffs2-64k
	$(call Image/Build/Board/P-334WT,jffs2-64k)
endef

define Image/Build/Board/P-334WT/Initramfs
	$(call Image/Build/LZMAKernel/KArgs,p-334wt,bin)
endef

#
# ZyXEL Prestige 335WT
#
define Image/Build/Board/P-335WT
	$(call Image/Build/ZyXEL,$(1),p-335wt)
endef

define Image/Build/Board/P-335WT/squashfs
	$(call Image/Build/Board/P-335WT,$(1))
endef

define Image/Build/Board/P-335WT/jffs2-64k
	$(call Image/Build/Board/P-335WT,$(1))
endef

define Image/Build/Board/P-335WT/Initramfs
	$(call Image/Build/LZMAKernel/KArgs,p-335wt,bin)
endef

#
# Groups
#
define Image/Build/Group/All
	$(call Image/Build/Board/P-334WT/$(1))
	$(call Image/Build/Board/P-335WT/$(1))
endef

#
# Profiles
#
define Image/Build/Profile/Generic
	$(call Image/Build/Group/All,$(1))
endef

define Image/Build/Profile/Atheros
	$(call Image/Build/Group/All,$(1))
endef

define Image/Build/Profile/Texas
	$(call Image/Build/Group/All,$(1))
endef

define Image/Build/Profile/P-334WT
	$(call Image/Build/Board/P-334WT/$(1))
endef

define Image/Build/Profile/P-335WT
	$(call Image/Build/Board/P-335WT/$(1))
endef
