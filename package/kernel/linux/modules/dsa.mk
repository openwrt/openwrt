#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

#
# DSA core
#

DSA_MENU:=DSA Support

define KernelPackage/dsa
  SUBMENU:=$(DSA_MENU)
  TITLE:=DSA support
  KCONFIG:=\
	CONFIG_NET_DSA \
	CONFIG_NET_SWITCHDEV=y \
	CONFIG_NET_DSA_HWMON=n
  DEPENDS:=+kmod-libphy
ifeq ($(CONFIG_OF),y)
  DEPENDS+=+kmod-of-mdio
endif
  FILES:=$(LINUX_DIR)/net/dsa/dsa_core.ko
  AUTOLOAD:=$(call AutoLoad,34,dsa_core)
endef

define KernelPackage/dsa/description
 Kernel module for DSA support.
endef

$(eval $(call KernelPackage,dsa))


define AddDepends/dsa
  SUBMENU:=$(DSA_MENU)
  DEPENDS+=kmod-dsa $(1)
endef


#
# Switch drivers
#

define KernelPackage/dsa-mv88e6060
  TITLE:=Marvell 88E6060 ethernet switch chip support
  KCONFIG:=\
	CONFIG_NET_DSA_MV88E6060 \
	CONFIG_NET_DSA_TAG_TRAILER=y
  FILES:=$(LINUX_DIR)/drivers/net/dsa/mv88e6060.ko
  AUTOLOAD:=$(call AutoLoad,34,mv88e6060)
  $(call AddDepends/dsa)
endef

define KernelPackage/dsa-mv88e6060/description
  This enables support for the Marvell 88E6060 ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6060))


define KernelPackage/dsa-mv88e6xxx
  TITLE:=Marvell 88E6XXX switch support
  KCONFIG:=CONFIG_NET_DSA_MV88E6XXX
  FILES:=$(LINUX_DIR)/drivers/net/dsa/mv88e6xxx_drv.ko
  AUTOLOAD:=$(call AutoLoad,34,mv88e6xxx_drv)
  $(call AddDepends/dsa)
endef

define KernelPackage/dsa-mv88e6xxx/description
  This enables support for the Marvell 88E6XXX ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6xxx))

define AddDepends/dsa-mv88e6xxx
  SUBMENU:=$(DSA_MENU)
  DEPENDS+=kmod-dsa kmod-dsa-mv88e6xxx $(1)
endef


define KernelPackage/dsa-mv88e6131
  TITLE:=Marvell 88E6085/6095/6095F/6131 switch support
  KCONFIG:=\
	CONFIG_NET_DSA_MV88E6131 \
	CONFIG_NET_DSA_TAG_DSA=y
  $(call AddDepends/dsa-mv88e6xxx)
endef

define KernelPackage/dsa-mv88e6131/description
  This enables support for the Marvell 88E6085/6095/6095F/6131 ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6131))


define KernelPackage/dsa-mv88e6123
  TITLE:=Marvell 88E6123/6161/6165 switch support
  KCONFIG:=\
	CONFIG_NET_DSA_MV88E6123_61_65 \
	CONFIG_NET_DSA_TAG_EDSA=y
  $(call AddDepends/dsa-mv88e6xxx)
endef

define KernelPackage/dsa-mv88e6123/description
  This enables support for the Marvell 88E6123/6161/6165 ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6123))


define KernelPackage/dsa-mv88e6171
  TITLE:=Marvell 88E6171/6172 switch support
  KCONFIG:=\
	CONFIG_NET_DSA_MV88E6131 \
	CONFIG_NET_DSA_TAG_DSA=y
  $(call AddDepends/dsa-mv88e6xxx)
endef

define KernelPackage/dsa-mv88e6171/description
  This enables support for the Marvell 88E6171/6172 ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6171))


define KernelPackage/dsa-mv88e6352
  TITLE:=Marvell 88E6176/88E6352 switch support
  KCONFIG:=\
	CONFIG_NET_DSA_MV88E6352 \
	CONFIG_NET_DSA_TAG_DSA=y
  $(call AddDepends/dsa-mv88e6xxx)
endef

define KernelPackage/dsa-mv88e6352/description
  This enables support for the Marvell 88E6176/88E6352 ethernet switch chips.
endef

$(eval $(call KernelPackage,dsa-mv88e6352))
