define Build/an7581-emmc-bl2-bl31-uboot
  head -c $$((0x800)) /dev/zero > $@
  cat $(STAGING_DIR_IMAGE)/an7581_$1-bl2.fip >> $@
  dd if=$(STAGING_DIR_IMAGE)/an7581_$1-bl31-u-boot.fip of=$@ bs=1 seek=$$((0x20000)) conv=notrunc
endef

define Build/an7581-preloader
  cat $(STAGING_DIR_IMAGE)/an7581_$1-bl2.fip >> $@
endef

define Build/an7581-bl31-uboot
  cat $(STAGING_DIR_IMAGE)/an7581_$1-bl31-u-boot.fip >> $@
endef

define Build/an7581-chainloader
  $(INSTALL_DIR) $(KDIR)/chainload-fit-$(notdir $@)
  @if [ -f "$(STAGING_DIR_IMAGE)/an7581_$1-u-boot.bin.lzma" ]; then \
    KERNEL="$(STAGING_DIR_IMAGE)/an7581_$1-u-boot.bin.lzma"; \
    COMP="lzma"; \
  else \
    KERNEL="$(STAGING_DIR_IMAGE)/an7581_$1-u-boot.bin"; \
    COMP="none"; \
  fi; \
  $(TOPDIR)/scripts/mkits.sh \
    -D $(DEVICE_NAME) \
    -o $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    -k $$KERNEL \
    -C $$COMP \
    -a 0x80200000 -e 0x80200000 \
    -c conf-uboot \
    -A arm64 -v u-boot \
    -d $(STAGING_DIR_IMAGE)/an7581_$1-u-boot.dtb \
    -s 0x82000000
  PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) \
    $(STAGING_DIR_HOST)/bin/mkimage \
    -D "-i $(KDIR)/chainload-fit-$(notdir $@)" \
    -f $(KDIR)/chainload-fit-$(notdir $@)/u-boot.its \
    $(STAGING_DIR_IMAGE)/an7581_$1-chainload-u-boot.itb
  cat $(STAGING_DIR_IMAGE)/an7581_$1-chainload-u-boot.itb >> $@
endef

define Device/FitImageLzma
	KERNEL_SUFFIX := -uImage.itb
	KERNEL = kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/airoha_an7581-evb
  $(call Device/FitImageLzma)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7581 Evaluation Board (SNAND)
  DEVICE_PACKAGES := kmod-leds-pwm kmod-i2c-an7581 kmod-pwm-airoha kmod-input-gpio-keys-polled
  DEVICE_DTS := an7581-evb
  DEVICE_DTS_CONFIG := config@1
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | pad-rootfs | append-metadata
  ARTIFACT/preloader.bin := an7581-preloader rfb
  ARTIFACT/bl31-uboot.fip := an7581-bl31-uboot rfb
  ARTIFACTS := preloader.bin bl31-uboot.fip
endef
TARGET_DEVICES += airoha_an7581-evb

define Device/airoha_an7581-evb-emmc
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7581 Evaluation Board (EMMC)
  DEVICE_DTS := an7581-evb-emmc
  DEVICE_PACKAGES := kmod-i2c-an7581
  ARTIFACT/preloader.bin := an7581-preloader rfb
  ARTIFACT/bl31-uboot.fip := an7581-bl31-uboot rfb
  ARTIFACTS := preloader.bin bl31-uboot.fip
endef
TARGET_DEVICES += airoha_an7581-evb-emmc
