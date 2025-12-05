DEVICE_VARS += NEC_FW_TYPE

define Build/nec-usbaterm-fw
  $(STAGING_DIR_HOST)/bin/nec-usbatermfw $@.new -t $(NEC_FW_TYPE) $(1)
  mv $@.new $@
endef

define Device/nec-netbsd-aterm
  DEVICE_VENDOR := NEC
  LOADER_TYPE := bin
  LZMA_TEXT_START := 0x82800000
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | uImage none
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | uImage none
  ARTIFACTS := uboot.bin
ifeq ($(IB),)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  COMPILE := loader-$(1).bin
  COMPILE/loader-$(1).bin := loader-okli-compile
  ARTIFACTS += initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	pad-to 4 skip=16 | \
	nec-usbaterm-fw -f 0x0003 -d $$(KDIR)/loader-$(1).bin -d $$$$@ | check-size
endif
endif
  UBOOT_PATH := $$(STAGING_DIR_IMAGE)/$$(SOC)_nec_aterm-u-boot.bin
  ARTIFACT/uboot.bin := append-uboot | check-size 128k
  DEVICE_PACKAGES := kmod-usb2 -uboot-envtools
endef
