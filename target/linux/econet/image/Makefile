include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += TRX_MODEL TRX_LOADADDR

define Target/Description
	Build firmware images for EcoNet MIPS based boards.
endef

# tclinux-trx is the default format used in the SDK
define Build/tclinux-trx
  ./tclinux-trx.sh --kernel $@ --rootfs $(IMAGE_ROOTFS) \
    --version $(VERSION_DIST)-$(REVISION) --endian $(TRX_ENDIAN) \
    $(if $(TRX_MODEL),--model $(TRX_MODEL)) \
    $(if $(TRX_LOADADDR),--loadaddr $(TRX_LOADADDR)) > $@.new
	mv $@.new $@
endef

# Build TRX without rootfs, for use with UBI root
define Build/kernel-trx
  ./tclinux-trx.sh --kernel $@ \
    --version $(VERSION_DIST)-$(REVISION) --endian $(TRX_ENDIAN) \
    $(if $(TRX_MODEL),--model $(TRX_MODEL)) \
    $(if $(TRX_LOADADDR),--loadaddr $(TRX_LOADADDR)) > $@.new
	mv $@.new $@
endef

# If TCSUPPORT_FREE_BOOTBASE is enabled, bootloader will decompress the kernel
# at 0x80002000 instead of 0x80020000.
#
# You can tell it by looking at the bootloader version string
# " at %s version %s free bootbase".
#
# The stub is a single MIPS "j 0x80020000" instruction (word 0x08008000) that
# trampolines from the low decompress address to the real kernel entry. Its
# BYTE order must match the target's endianness. Keyed off TRX_MODEL's own
# TRX_ENDIAN rather than CONFIG_BIG_ENDIAN: TRX_ENDIAN is the variable this
# same file already uses for the image's actual endianness (see
# Build/tclinux-trx above), and on a TP-Link EX20v (TRX_ENDIAN=le) build the
# previous CONFIG_BIG_ENDIAN-keyed version produced the wrong byte order,
# which decodes to a different instruction and jumps to a garbage address
# (observed as CP0_EPC=0 Undefined Exception on the EX20v's serial console).
define Build/tclinux-free-bootbase-jump
	@[ "$(KERNEL_LOADADDR)" = 0x80020000 ] || { \
		$(call ERROR_MESSAGE,WARNING: KERNEL_LOADADDR changed); \
		rm -f $@; \
	}
	printf '$(if $(filter le,$(TRX_ENDIAN)),\000\200\000\010,\010\000\200\000)' > $@.new
	dd if=/dev/zero bs=122876 count=1 >> $@.new
	cat $@ >> $@.new
	mv $@.new $@
endef

# EX20v's free bootbase does NOT read the zytrx "HDR2" format that
# tclinux-trx.sh emits; it reads a proprietary 256-byte tag recovered from
# the stock tclinux.bin. See ex20v-tctag.py for the tag layout and which
# fields differ per image.
define Build/ex20v-tctag
  ./ex20v-tctag.py $@ $@.new
	mv $@.new $@
endef

# tclinux bootloader requires LZMA, BUT only provides 7.5MB of space
# to decompress into. So we use vmlinuz and decompress twice.
define Device/Default
  DEVICE_DTS_DIR := ../dts
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  # decompression buffer limit, the size of LZMA-compressed data is read from
  # the trx header
  KERNEL_SIZE := 7480k
  KERNEL_NAME := vmlinuz.bin
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb
  UBINIZE_OPTS := -E 5
  TRX_MODEL :=
  TRX_LOADADDR :=
endef

# Kernel in a raw TRX partition, root squashfs in UBI. FACTORY_SIZE must be the
# size of the partition the .trx is written into; that write is linear, so an
# oversized image overruns it into the next partition.
define Device/tclinux-ubi
  KERNEL_SIZE := 4096k
  KERNEL := kernel-bin | append-dtb | lzma | kernel-trx
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := tclinux.trx sysupgrade.bin
  IMAGE/tclinux.trx := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | \
    check-size $$$$(FACTORY_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
