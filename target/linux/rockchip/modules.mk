# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/scsi-ufs-rockchip
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Rockchip UFS host controller support
  DEPENDS:=+kmod-scsi-ufs-platform
  KCONFIG:=CONFIG_SCSI_UFS_ROCKCHIP
  FILES:=$(LINUX_DIR)/drivers/ufs/host/ufs-rockchip.ko
  AUTOLOAD:=$(call AutoLoad,45,ufs-rockchip,1)
endef

define KernelPackage/scsi-ufs-rockchip/description
  Support for the Rockchip specific additions to UFSHCD platform
endef

$(eval $(call KernelPackage,scsi-ufs-rockchip))
