#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/efi-vars
  SUBMENU:=$(OTHER_MENU)
  TITLE:=EFI VARS modules
  DEPENDS:=@EFI_IMAGES
  KCONFIG:= \
	CONFIG_EFI=y \
	CONFIG_EFI_STUB=n \
	CONFIG_ACPI_BGRT=y \
	CONFIG_EFI_FAKE_MEMMAP=y \
	CONFIG_EFI_MAX_FAKE_MEM=8 \
	CONFIG_EFI_VARS=m \
	CONFIG_EFIVAR_FS=y \
	CONFIG_EFI_PGT_DUMP=n \
	CONFIG_EARLY_PRINTK_EFI=y \
	CONFIG_FB_EFI=y \
	CONFIG_FB_SIMPLE=y
  FILES:=$(LINUX_DIR)/fs/efivarfs/efivarfs.ko
  AUTOLOAD:=$(call AutoLoad,0,efi-vars)
endef

$(eval $(call KernelPackage,efi-vars))

define KernelPackage/rdc321x-wdt
  SUBMENU:=$(OTHER_MENU)
  TITLE:=RDC321x watchdog
  DEPENDS:=@TARGET_x86_rdc
  KCONFIG:=CONFIG_RDC321X_WDT
  FILES:=$(LINUX_DIR)/drivers/$(WATCHDOG_DIR)/rdc321x_wdt.ko
  AUTOLOAD:=$(call AutoLoad,50,rdc321x_wdt)
endef

define KernelPackage/rdc321x-wdt/description
  RDC-321x watchdog driver
endef

$(eval $(call KernelPackage,rdc321x-wdt))

