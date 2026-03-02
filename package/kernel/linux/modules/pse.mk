#
# Copyright (C) 2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PSE_MENU:=PSE (Power Sourcing Equipment) Support

define KernelPackage/pse-hasivo-hs104
  SUBMENU:=$(PSE_MENU)
  TITLE:=Hasivo HS104 PoE PSE controller
  DEPENDS:=+kmod-i2c-core +kmod-regmap-i2c
  KCONFIG:= \
	CONFIG_REGULATOR=y \
	CONFIG_REGULATOR_FIXED_VOLTAGE=y \
	CONFIG_PSE_CONTROLLER=y \
	CONFIG_PSE_REGULATOR=n \
	CONFIG_PSE_PD692X0=n \
	CONFIG_PSE_SI3474=n \
	CONFIG_PSE_TPS23881=n \
	CONFIG_PSE_HASIVO_HS104=y
endef

define KernelPackage/pse-hasivo-hs104/description
  Hasivo HS104 PoE PSE controller support (built-in).
  Supports the HS104PTI/HS104PBI single-chip PoE PSE controllers
  managing 4 delivery channels for 802.3af/at/bt power over I2C.
  Built-in because the PSE controller must probe before MDIO PHYs
  that reference it via pses property, and module loading is too late.
endef

$(eval $(call KernelPackage,pse-hasivo-hs104))
