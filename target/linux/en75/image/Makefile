include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Target/Description
	Build firmware images for EN75xx MIPS based boards.
endef

define Build/tclinux-trx
  ./tclinux-trx.sh $@ $(IMAGE_ROOTFS) $(VERSION_DIST)-$(REVISION) > $@.new
	mv $@.new $@
endef

# tclinux bootloader requires LZMA, BUT only provides 7.5MB of space
# to decompress into. So we use vmlinuz and decompress twice.
define Device/Default
  DEVICE_DTS_DIR := ../dts
  KERNEL_SIZE := 7480k
  KERNEL_NAME := vmlinuz.bin
  KERNEL := kernel-bin | append-dtb
  IMAGES += tclinux.trx kernel.bin
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
  IMAGE/kernel.bin := append-kernel
endef

define Device/smartfiber_xp8421-b
  DEVICE_VENDOR := SmartFiber
  DEVICE_MODEL := XP8421-B
  DEVICE_DTS := en751221_smartfiber_xp8421-b
endef

TARGET_DEVICES += smartfiber_xp8421-b

$(eval $(call BuildImage))
