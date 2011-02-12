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
  DEPENDS+= +!TARGET_xburst:kmod-crc16 $(1)
endef


define SetDepends/hid
  DEPENDS:= @!TARGET_x86
endef

define AddDepends/hid
  DEPENDS+= +!TARGET_x86:kmod-hid $(1)

endef


define SetDepends/input
  DEPENDS:= @!TARGET_x86
endef

define AddDepends/input
  DEPENDS+= +!TARGET_x86:kmod-input-core $(1)
endef


define SetDepends/rfkill
  DEPENDS:= @(TARGET_ar71xx||TARGET_brcm47xx||TARGET_s3c24xx||TARGET_x86||TARGET_gemini)
endef

define AddDepends/rfkill
  DEPENDS+= +(TARGET_ar71xx||TARGET_brcm47xx||TARGET_s3c24xx||TARGET_x86):kmod-rfkill $(1)
endef
