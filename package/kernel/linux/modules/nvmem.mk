#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

NVMEM_MENU:=NVMEM Support

define KernelPackage/ubi-nvmem
  SUBMENU:=$(NVMEM_MENU)
  TITLE:=UBI virtual NVMEM
  KCONFIG:= \
	CONFIG_MTD_UBI_NVMEM
  FILES:=$(LINUX_DIR)/drivers/mtd/ubi/nvmem.ko
  AUTOLOAD:=$(call AutoProbe,nvmem,1)
endef

define KernelPackage/ubi-nvmem/description
  Kernel module for exposing UBI volumes as NVMEM providers.
endef

$(eval $(call KernelPackage,ubi-nvmem))


define KernelPackage/nvmem-layout-uboot-env
  SUBMENU:=$(NVMEM_MENU)
  TITLE:=U-Boot environment variables layout
  KCONFIG:= \
	CONFIG_NVMEM_LAYOUT_U_BOOT_ENV
  FILES:= \
	$(LINUX_DIR)/drivers/nvmem/layouts/u-boot-env.ko
  AUTOLOAD:=$(call AutoProbe,u-boot-env,1)
endef

define KernelPackage/nvmem-layout-uboot-env/description
  Kernel module that exposes U-Boot environment variables through a NVMEM layout.
endef

$(eval $(call KernelPackage,nvmem-layout-uboot-env))
