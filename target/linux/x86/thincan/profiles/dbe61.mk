#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DBE61
  NAME:=DBE61
endef

define Profile/DBE61/Description
	Compile OpenWrt for Artec ThinCan DBE61

	ThinCan DBE61 is a thin-client featuring:
	  * AMD Geode LX700
	  * AMD CS5535 companion device
	  * 64MB or 256MB NAND Flash or none
	  * 128MB or 256MB DDR SDRAM
	  * Realtek RTL8139

	http://wiki.thincan.org/DBE61
endef

$(eval $(call Profile,DBE61))
