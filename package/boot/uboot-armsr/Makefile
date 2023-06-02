include $(TOPDIR)/rules.mk

PKG_VERSION:=2023.04
PKG_RELEASE:=1

PKG_HASH:=e31cac91545ff41b71cec5d8c22afd695645cd6e2a442ccdacacd60534069341

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=armsr
endef

define U-Boot/qemu_armv7
  NAME:=QEMU ARM Virtual Machine 32-bit
  BUILD_SUBTARGET:=armv7
  BUILD_DEVICES:=generic
  UBOOT_CONFIG:=qemu_arm
endef

define U-Boot/qemu_armv8
  NAME:=QEMU ARM Virtual Machine 64-bit
  BUILD_SUBTARGET:=armv8
  BUILD_DEVICES:=generic
  UBOOT_CONFIG:=qemu_arm64
endef

UBOOT_TARGETS := \
	qemu_armv7 \
	qemu_armv8

UBOOT_CUSTOMIZE_CONFIG := \
	--enable CMD_EFIDEBUG

$(eval $(call BuildPackage/U-Boot))
