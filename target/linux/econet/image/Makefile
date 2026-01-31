include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Target/Description
	Build firmware images for EcoNet MIPS based boards.
endef

# tclinux-trx is the default format used in the SDK
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
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
