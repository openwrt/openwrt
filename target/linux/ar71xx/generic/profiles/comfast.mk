#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CF-E316N-V2
        NAME:=COMFAST CF-E316N v2
        PACKAGES:=
endef

define Profile/CF-E316N-V2/Description
        Package set optimised for the COMFAST CF-E316N v2
        by Shenzhen Four Seas Global Link Network Technology Co., Ltd
        aka CF-E316V2, CF-E316N-V2 and CF-E316Nv2.0 (no FCC ID)
endef

$(eval $(call Profile,CF-E316N-V2))
