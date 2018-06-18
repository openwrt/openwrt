#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=2017.03

PKG_HASH:=f54baf3f9325bf444c7905f3a5b6f83680edb1e6e1a4d5f8a5ad80abe885113f

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk

define U-Boot/Default
  BUILD_TARGET:=zynq
  UBOOT_IMAGE:=spl/boot.bin u-boot.img
  UBOOT_CONFIG:=zynq_$(1)
endef

define U-Boot/zc702
  NAME:=Xilinx ZC702 Dev Board
endef

define U-Boot/zc706
  NAME:=Xilinx ZC706 Dev Board
endef

define U-Boot/zed
  NAME:=Avnet Digilent ZedBoard Dev Board
endef

define U-Boot/zybo
  NAME:=Digilent Zybo Dev Board
endef

UBOOT_TARGETS := \
	zc702 \
	zed \
	zybo

Build/Exports:=$(Host/Exports)

$(eval $(call BuildPackage/U-Boot))
