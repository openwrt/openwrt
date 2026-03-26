include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

loadaddr-$(CONFIG_TARGET_airoha_an7581) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_an7583) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_en7523) := 0x80200000

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_LOADADDR = $(loadaddr-y)
  FILESYSTEMS := squashfs
  DEVICE_DTS = $$(SUBTARGET)-$(subst -,_,$(1))
  DEVICE_DTS_DIR := ../dts
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
    pad-rootfs | append-metadata
endef

define Device/FitImage
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endef

define Device/FitImageLzma
  KERNEL_UFFIX := -uImage.itb
  KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_NAME := Image
endef

define Device/ubi_firmware_chainload
  $(Build/FitImage)
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBOOTENV_IN_UBI := 1
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd | pad-to 128k
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | \
    fit gzip $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb external-static-with-rootfs | \
    append-metadata
  DEVICE_PACKAGES += fitblk
endef

define Build/staging-copy-file
  cat $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1 > $@
endef

define Build/uboot-chainloader
  $(INSTALL_DIR) $(KDIR)/chainload-fit-$(notdir $@)
  @if [ -f "$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin.lzma" ]; then \
    KERNEL="$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin.lzma"; \
    COMP="lzma"; \
  else \
    KERNEL="$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin"; \
    COMP="none"; \
  fi; \
  $(TOPDIR)/scripts/mkits.sh \
    -D $(DEVICE_NAME) \
    -o $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    -k $$KERNEL \
    -C $$COMP \
    -a $(loadaddr-y) -e $(loadaddr-y) \
    -c conf-uboot \
    -A $(ARCH) -v u-boot \
    -d $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.dtb \
    -s 0x82000000
  PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) \
    $(STAGING_DIR_HOST)/bin/mkimage \
    -D "-i $(KDIR)/chainload-fit-$(notdir $@)" \
    -f $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-chainload-u-boot.itb
  cat $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-chainload-u-boot.itb >> $@
endef

define Build/an75xx-chainloader
  $(INSTALL_DIR) $(KDIR)/chainload-fit-$(notdir $@)
  @if [ -f "$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin.lzma" ]; then \
    KERNEL="$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin.lzma"; \
    COMP="lzma"; \
  else \
    KERNEL="$(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.bin"; \
    COMP="none"; \
  fi; \
  $(TOPDIR)/scripts/mkits.sh \
    -D $(DEVICE_NAME) \
    -o $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    -k $$KERNEL \
    -C $$COMP \
    -a $(KERNEL_LOADADDR) -e $(KERNEL_LOADADDR) \
    -c conf-uboot \
    -A $(ARCH) -v u-boot \
    -d $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-u-boot.dtb \
    -s 0x82000000
  PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) \
    $(STAGING_DIR_HOST)/bin/mkimage \
    -D "-i $(KDIR)/chainload-fit-$(notdir $@)" \
    -f $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-chainload-u-boot.itb
  cat $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1-chainload-u-boot.itb >> $@
endef

include $(SUBTARGET).mk

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
