#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AnnexB
  NAME:=ADSL Annex B
  PACKAGES:=kmod-sangam-atm-annex-b ppp-mod-pppoa
endef
$(eval $(call Profile,AnnexB))

