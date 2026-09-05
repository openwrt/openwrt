include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

loadaddr-$(CONFIG_TARGET_airoha_an7581) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_an7583) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_en7523) := 0x80208000
loadaddr-$(CONFIG_TARGET_airoha_en751221) := 0x80020000
loadaddr-$(CONFIG_TARGET_airoha_en751627) := 0x80020000
loadaddr-$(CONFIG_TARGET_airoha_en7528) := 0x80020000

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_LOADADDR = $(loadaddr-y)
  SOC = $$(SUBTARGET)
  FILESYSTEMS := squashfs
  DEVICE_DTS = $$(SUBTARGET)-$(subst -,_,$(1))
  DEVICE_DTS_DIR := ../dts
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
    pad-rootfs | append-metadata
endef

# tclinux-trx is the image format used by the EN751221 vendor bootloader.
define Build/tclinux-trx
	./tclinux-trx.sh --kernel $@ --rootfs $(IMAGE_ROOTFS) \
		--version $(VERSION_DIST)-$(REVISION) --endian $(TRX_ENDIAN) \
		$(if $(TRX_MODEL),--model $(TRX_MODEL)) > $@.new
	mv $@.new $@
endef

# Build a TRX containing only the kernel for devices whose rootfs lives in UBI.
define Build/kernel-trx
	./tclinux-trx.sh --kernel $@ \
		--version $(VERSION_DIST)-$(REVISION) --endian $(TRX_ENDIAN) \
		$(if $(TRX_MODEL),--model $(TRX_MODEL)) > $@.new
	mv $@.new $@
endef

# Free-bootbase variants decompress at 0x80002000. Add a jump to the normal
# EN751221 kernel load address before compressing the image.
define Build/tclinux-free-bootbase-jump
	@[ "$(KERNEL_LOADADDR)" = 0x80020000 ] || { \
		$(call ERROR_MESSAGE,WARNING: KERNEL_LOADADDR changed); \
		rm -f $@; \
	}
	printf '\010\000\200\000' > $@.new
	dd if=/dev/zero bs=122876 count=1 >> $@.new
	cat $@ >> $@.new
	mv $@.new $@
endef

define Device/FitImage
  KERNEL_NAME := Image
  KERNEL = kernel-bin | gzip | \
    fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
    KERNEL_INITRAMFS = kernel-bin | gzip | \
    fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb
endef

define Device/FitImageLzma
  KERNEL_SUFFIX := -uImage.itb
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
endef

define Device/FitImageVmlinuz
  KERNEL_NAME := vmlinuz.bin
  KERNEL = kernel-bin | \
    fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS = kernel-bin | \
    fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
endef

define Device/RouterBoot
  DEVICE_VENDOR := MikroTik
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 7168k
  KERNEL_NAME := vmlinuz
  KERNEL_INITRAMFS_NAME := vmlinuz-initramfs
  KERNEL_INITRAMFS_SUFFIX := -routerboot.elf
  KERNEL_INITRAMFS := kernel-bin | append-dtb-elf | \
    routerboot-elf 0x08000000
  KERNEL := kernel-bin | append-dtb-elf | \
    routerboot-elf 0x08000000
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | \
    sysupgrade-tar | append-metadata
  DEVICE_PACKAGES += yafut mtd-utils
endef

define Device/BootUboot
  DEVICE_PACKAGES += uboot-envtools
  ARTIFACTS := uboot-bootloader.bin
  ARTIFACT/uboot-bootloader.bin := staging-copy-file $(1)-preload_u-boot.bin | \
    fill-zero 0x80000
endef

# Make image with new u-boot part version and ubi partion support
define Device/Uboot-FitImage
  $(Device/FitImage)
  $(Device/BootUboot)
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBOOTENV_IN_UBI := 1
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | gzip | \
    fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb with-initrd | pad-to 128k
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | \
    fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb external-static-with-rootfs | \
    append-metadata
  DEVICE_PACKAGES += fitblk
endef

define Device/ubi_firmware_chainload
  $(Device/FitImage)
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBOOTENV_IN_UBI := 1
  KERNEL_IN_UBI := 1
  KERNEL := kernel-bin | gzip
  KERNEL_INITRAMFS := kernel-bin | lzma | \
    fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb with-initrd | pad-to 128k
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | \
    fit gzip $$(KDIR)/image-$$(DEVICE_DTS).dtb external-static-with-rootfs | \
    append-metadata
  DEVICE_PACKAGES += fitblk
endef

define Device/tcboot_trx
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  # decompression buffer limit, the size of LZMA-compressed data is read from
  # the trx header
  KERNEL_SIZE := 7480k
  KERNEL_NAME := vmlinuz.bin
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb
  UBINIZE_OPTS := -E 5
endef

# Kernel in a raw TRX partition, root squashfs in UBI. FACTORY_SIZE must be the
# size of the partition the .trx is written into; that write is linear, so an
# oversized image overruns it into the next partition.
define Device/tclinux-ubi
  $(Device/tcboot_trx)
  KERNEL_SIZE := 4096k
  KERNEL := kernel-bin | append-dtb | lzma | kernel-trx
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := tclinux.trx sysupgrade.bin
  IMAGE/tclinux.trx := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | \
    check-size $$$$(FACTORY_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Build/staging-copy-file
  cat $(STAGING_DIR_IMAGE)/$(SUBTARGET)_$1 > $@
endef

define Build/fill-zero
  dd if=/dev/zero count=$$(( $1 - $$(stat -c%s $@) ))B >> $@
endef

define Build/routerboot-elf
  $(TARGET_CROSS)objcopy \
    --change-addresses=$(1) \
    $@ $@.new
  mv $@.new $@
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
