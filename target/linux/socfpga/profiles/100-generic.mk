#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
  NAME:=Generic (default)
  PACKAGES:=
endef

define Profile/Generic/Description
 Package set compatible with most Altera SoCFPGA based boards.
endef

$(eval $(call Profile,Generic))
