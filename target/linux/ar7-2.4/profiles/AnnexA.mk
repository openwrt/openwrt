#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AnnexA
  NAME:=ADSL Annex A (default)
  PACKAGES:=kmod-sangam-atm-annex-a ppp-mod-pppoa
endef
$(eval $(call Profile,AnnexA))

