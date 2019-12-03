#
# Copyright (C) 2017 Lede
# Copyright (C) 2019 Samik Gupta <samik.gupta@broadcom.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default Profile (all drivers)
  PACKAGES:=kmod-bnxt
  PRIORITY := 1
endef

define Profile/Default/Description
  Default profile with package set compatible with most boards.
endef
$(eval $(call Profile,Default))
