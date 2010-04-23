#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# XXX: xburst has CONFIG_CRC16=y because of CONFIG_UBIFS_FS=y

define SetDepends/crc16
  DEPENDS:= @!TARGET_xburst
endef

define AddDepends/crc16
  DEPENDS+= +!TARGET_xburst:kmod-crc16
endef
