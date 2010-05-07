#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TEW632BRP
	NAME:=TRENDNet TEW-632BRP
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TEW632BRP/Description
	Package set optimized for the TRENDNet TEW-632BRP.
endef

$(eval $(call Profile,TEW632BRP))

define Profile/TEW652BRP
	NAME:=TRENDNet TEW-652BRP
	PACKAGES:=kmod-ath9k
endef

define Profile/TEW652BRP/Description
	Package set optimized for the TRENDNet TEW-652BRP.
endef

$(eval $(call Profile,TEW652BRP))
