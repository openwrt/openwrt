#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TEW691GR
	NAME:=TRENDnet TEW-691GR
	PACKAGES:=swconfig
endef

define Profile/TEW691GR/Description
	Package set compatible with the TRENDnet TEW-691GR board.
endef
$(eval $(call Profile,TEW691GR))


define Profile/TEW692GR
	NAME:=TRENDnet TEW-692GR
	PACKAGES:=swconfig
endef

define Profile/TEW692GR/Description
	Package set compatible with the TRENDnet TEW-692GR board.
endef
$(eval $(call Profile,TEW692GR))
