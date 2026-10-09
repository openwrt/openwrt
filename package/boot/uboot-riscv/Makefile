include $(TOPDIR)/rules.mk

PKG_VERSION:=2025.04
PKG_RELEASE:=1

PKG_HASH:=439d3bef296effd54130be6a731c5b118be7fddd7fcc663ccbc5fb18294d8718

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=riscv
  BUILD_SUBTARGET:=64
endef

define U-Boot/qemu_riscv64_smode
  NAME:=QEMU RISC-V 64-bit (S-mode)
  BUILD_DEVICES:=generic
  UBOOT_CONFIG:=qemu-riscv64_smode
endef

UBOOT_TARGETS := \
	qemu_riscv64_smode

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_MKEFICAPSULE \
	--disable USE_BOOTCOMMAND

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(STAGING_DIR_IMAGE)/u-boot-riscv-qemu.bin
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
