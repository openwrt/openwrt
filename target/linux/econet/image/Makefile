include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Target/Description
	Build firmware images for EcoNet MIPS based boards.
endef

DEVICE_VARS += LZMA_TEXT_START LOADER_TYPE TCLINUX_MODEL TRX_LOADADDR
DEVICE_VARS += NAND_BBT_SIZE NAND_OS1_OFFSET NAND_OS2_OFFSET
DEVICE_VARS += NAND_BOOT_FLAG_OFFSET NAND_BOOT_FLAG_OS1 NAND_BOOT_FLAG_OS2 NAND_BOOT_FLAG_MASK
DEVICE_VARS += NAND_CMDLINE_OS1 NAND_CMDLINE_OS2

LOADER_TYPE := bin
LOADADDR := 0x80020000
TRX_LOADADDR := 80020000

define Build/loader-common
	rm -rf $@.src
	$(MAKE) -C lzma-loader \
		PKG_BUILD_DIR="$@.src" \
		TARGET_DIR="$(dir $@)" LOADER_NAME="$(notdir $@)" \
		$(if $(LZMA_TEXT_START),LZMA_TEXT_START=$(LZMA_TEXT_START)) \
		LOADADDR=$(LOADADDR) \
		SUBTARGET=$(SUBTARGET) \
		$(1) compile loader.$(LOADER_TYPE)
	mv "$@.$(LOADER_TYPE)" "$@"
	rm -rf $@.src
endef

define Build/loader-kernel
	$(call Build/loader-common,LOADER_DATA="$@")
endef

define Build/loader-spinand-bmt
	$(call Build/loader-common,NAND_BOOT=1 \
		$(if $(NAND_BBT_SIZE),MAX_BBT_SIZE=$(NAND_BBT_SIZE)) \
		$(if $(NAND_OS1_OFFSET),NAND_OS1_OFFSET=$(NAND_OS1_OFFSET)) \
		$(if $(NAND_OS2_OFFSET),NAND_OS2_OFFSET=$(NAND_OS2_OFFSET)) \
		$(if $(NAND_BOOT_FLAG_OFFSET),NAND_BOOT_FLAG_OFFSET=$(NAND_BOOT_FLAG_OFFSET)) \
		$(if $(NAND_BOOT_FLAG_OS1),NAND_BOOT_FLAG_OS1=$(NAND_BOOT_FLAG_OS1)) \
		$(if $(NAND_BOOT_FLAG_OS2),NAND_BOOT_FLAG_OS2=$(NAND_BOOT_FLAG_OS2)) \
		$(if $(NAND_BOOT_FLAG_MASK),NAND_BOOT_FLAG_MASK=$(NAND_BOOT_FLAG_MASK)) \
		$(if $(NAND_CMDLINE_OS1),NAND_CMDLINE_OS1=$(NAND_CMDLINE_OS1)) \
		$(if $(NAND_CMDLINE_OS2),NAND_CMDLINE_OS2=$(NAND_CMDLINE_OS2)))
endef

# tclinux-trx is the default format used in the SDK
# Optional: pass model string via $(1) for reserved header field
# (supports \n escape). Stock web UI will accept the TRX file if the
# correct model string is specified here
define Build/tclinux-trx
	./tclinux-trx.sh \
		--kernel $@ \
		--rootfs $(IMAGE_ROOTFS) \
		--version $(VERSION_DIST)-$(REVISION) \
		--loadaddr $(TRX_LOADADDR) \
		$(if $(1),--model '$(1)') > $@.new
	mv $@.new $@
endef

define Device/tclinux-lzma-loader
  TRX_LOADADDR := 80002000
  KERNEL := kernel-bin | append-dtb | lzma
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx = loader-spinand-bmt | lzma | pad-to 64k | append-kernel | tclinux-trx $$(TCLINUX_MODEL)
endef

# tclinux bootloader requires LZMA, BUT only provides 7.5MB of space
# to decompress into. So we use vmlinuz and decompress twice.
define Device/Default
  DEVICE_DTS_DIR := ../dts
  KERNEL_SIZE := 7480k
  KERNEL_NAME := vmlinuz.bin
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
