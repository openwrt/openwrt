include $(TOPDIR)/rules.mk

PKG_VERSION:=2023.04
PKG_RELEASE:=1

PKG_HASH:=e31cac91545ff41b71cec5d8c22afd695645cd6e2a442ccdacacd60534069341

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=armvirt
endef

define U-Boot/armvirt
  NAME:=QEMU ARM Virtual Machine 32-bit
  BUILD_SUBTARGET:=32
  BUILD_DEVICES:=generic
  UBOOT_CONFIG:=qemu_arm
endef

define U-Boot/armvirt64
  NAME:=QEMU ARM Virtual Machine 64-bit
  BUILD_SUBTARGET:=64
  BUILD_DEVICES:=generic
  UBOOT_CONFIG:=qemu_arm64
endef

UBOOT_TARGETS := \
	armvirt \
	armvirt64

UBOOT_CUSTOMIZE_CONFIG := \
	--enable CMD_EFIDEBUG

$(eval $(call BuildPackage/U-Boot))
