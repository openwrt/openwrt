#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RNXN360RT
	NAME:=Rosewill RNX-N360RT
	PACKAGES:=
endef

define Profile/RNXN360RT/Description
	Package set optimized for the Rosewill RNX-N360RT.
endef
$(eval $(call Profile,RNXN360RT))
