#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define AddDepends/crc16
  DEPENDS+= +kmod-lib-crc16 $(1)
endef

define AddDepends/hid
  DEPENDS+= +kmod-hid $(1)
endef

define AddDepends/input
  DEPENDS+= +kmod-input-core $(1)
endef


define SetDepends/rfkill
  DEPENDS:= @(TARGET_ar71xx||TARGET_brcm47xx||TARGET_s3c24xx||TARGET_x86||TARGET_gemini)
endef

define AddDepends/rfkill
  DEPENDS+= +(TARGET_ar71xx||TARGET_brcm47xx||TARGET_s3c24xx||TARGET_x86):kmod-rfkill $(1)
endef
