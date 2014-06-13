#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AWAPN2403
	NAME:=AisaRF AWAPN2403
endef

define Profile/AWAPN2403/Description
	Package set for Allnet ALL0239-3G
endef

$(eval $(call Profile,AWAPN2403))
