include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

loadaddr-$(CONFIG_TARGET_airoha_an7563) := 0x80088000
loadaddr-$(CONFIG_TARGET_airoha_an7581) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_an7583) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_en7523) := 0x80200000

# Per-board cloud IDs written into the 512-byte image tag.  oemId is the
# per-unit flashId + device key and is NOT known to the build: only the
# vendor web UI validates it, the bootloader does not, so leave it empty.
DEVICE_VARS += ECONET_HARDWARE_ID
DEVICE_VARS += ECONET_FIRMWARE_ID
DEVICE_VARS += ECONET_OEM_ID

# default platform image(fit) build
define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := Image
  KERNEL = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = kernel-bin | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_LOADADDR = $(loadaddr-y)
  FILESYSTEMS := squashfs
  DEVICE_DTS = $$(SOC)-$(lastword $(subst _, ,$(1)))
  DEVICE_DTS_DIR := ../dts
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
	pad-rootfs | append-metadata
endef

# Prepend the TP-Link/Econet 512-byte image tag (struct IMAGE_TAG,
# tagVersion 0x03000003) that the stock Airoha U-Boot 2014.04 expects at
# MTD_OFS_KERNEL.  Its "flash tpimg" reads the tag, takes kernelLen from
# +0x78 and reloads tag+payload from the same offset, so the FIT lands at
# CONFIG_SYS_LOAD_ADDR + 0x200 and bootcmd's "bootm 81800200" finds it.
#
# Every multi-byte field is little-endian.  mktplinkfw2 is no use here: it
# htonl()s the sizes, which a little-endian bootloader reads byte-swapped.
#
# Arguments: <loadaddr> [<entry>]
define Build/tplink-econet-header
	$(TOPDIR)/scripts/tplink-econet-mkimage.py \
		--load-addr $(word 1,$(1)) \
		--entry-point $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(word 1,$(1))) \
		--output $@.new \
		$(if $(ECONET_HARDWARE_ID),--hardware-id "$(ECONET_HARDWARE_ID)") \
		$(if $(ECONET_FIRMWARE_ID),--firmware-id "$(ECONET_FIRMWARE_ID)") \
		$(if $(ECONET_OEM_ID),--oem-id "$(ECONET_OEM_ID)") \
		$@
	@mv $@.new $@
endef

include $(SUBTARGET).mk

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
