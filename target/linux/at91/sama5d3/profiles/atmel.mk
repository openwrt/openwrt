#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AT91SAMA5D3XPLAINED
	NAME:=Atmel AT91SAMA5D3XPLAINED
endef
define Profile/AT91SAMA5D3XPLAINED/Description
	Atmel AT91SAMA5D3EXPLAINED eval board
endef
$(eval $(call Profile,AT91SAMA5D3XPLAINED))
