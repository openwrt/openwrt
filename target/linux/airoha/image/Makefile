include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

loadaddr-$(CONFIG_TARGET_airoha_an7581) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_an7583) := 0x80200000
loadaddr-$(CONFIG_TARGET_airoha_en7523) := 0x80200000

define Build/airoha-image-tcboot
  $(STAGING_DIR_HOST)/bin/fiptool create \
		--tb-fw $(STAGING_DIR_IMAGE)/$(word 1,$1)-bl2.bin $@.bl2.fip
  $(STAGING_DIR_HOST)/bin/fiptool create \
		--soc-fw $(STAGING_DIR_IMAGE)/$(word 1,$1)-bl31.lzma \
		--nt-fw $(STAGING_DIR_IMAGE)/$(word 2,$1)-u-boot.lzma $@.bl31-u-boot.fip
  [ "$$(stat -c "%s" "$@.bl2.fip")" -le 129024 ] || { \
	echo "ERROR: bl2 exceeds the maximum size 129024 bytes" >&2; \
	exit 1; \
  }
  [ "$$(stat -c "%s" "$@.bl31-u-boot.fip")" -le 376828 ] || { \
	echo "ERROR: bl31 exceeds the maximum size 376828 bytes" >&2; \
	exit 1; \
  }
  dd if=/dev/zero bs=524288 count=1 | tr '\000' '\377' > "$@"
  dd if=/dev/zero of="$@" bs=2048 count=1 conv=notrunc
  dd if=$@.bl2.fip of="$@" bs=2048 seek=1 conv=notrunc
  dd if=$@.bl31-u-boot.fip of="$@" bs=131072 seek=1 conv=notrunc
  head -c 507900 "$@" | gzip -n -c | tail -c 8 | head -c 4 | \
	$(STAGING_DIR_HOST)/bin/xorimage -p ff -x | dd of="$@" bs=1 seek=507900 conv=notrunc
  rm -f $@.bl2.fip $@.bl31-u-boot.fip
endef

define Build/airoha-image-tclinux
  $(STAGING_DIR_HOST)/bin/fiptool create \
		--soc-fw $(STAGING_DIR_IMAGE)/$(word 1,$1)-bl31.lzma \
		--nt-fw $(STAGING_DIR_IMAGE)/$(word 2,$1)-u-boot.lzma $@.fip
  cp "$@" "$@.fit"
  $(TOPDIR)/scripts/ubinize-image.sh --part fip=:$@.fip --part fit=:"$@.fit" "$@.new" -p 128KiB -m 2048 -E 5
  mv "$@.new" "$@"
  rm -f "$@.fip" "$@.fit"
endef

# default all platform image(fit) build
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

include $(SUBTARGET).mk

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))
