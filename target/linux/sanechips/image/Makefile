# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# cspboot only boots a kernel that starts with the 32-byte ZTE slot marker.
define Build/zte-slot-marker
	( printf '\063\063\063\063\314\314\314\314\210\210\210\210\335\335\335\335'; \
	  printf '\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377'; \
	  cat $@ ) > $@.new
	mv $@.new $@
endef

# The initramfs image is started from U-Boot after a TFTP download.  U-Boot
# leaves its Ethernet running, with its packet buffers at 0x41000000 and its
# free-buffer list at 0x41800000.  A kernel unpacked at 0x40008000 lands on
# top of them, and a frame that arrives during early boot makes the buffer
# manager read kernel bytes as buffer numbers: it locks up (no RX, no TX)
# and the hardware writes packet data into the kernel.  Load the initramfs
# kernel at 0x48008000 instead (AUTO_ZRELADDR unpacks it there); the RAM
# below 0x48000000 is left unused by this RAM-only system.
define Build/uImage-ramboot
	$(if $(UIMAGE_TIME),SOURCE_DATE_EPOCH="$(UIMAGE_TIME)") \
	mkimage -A $(LINUX_KARCH) -O linux -T kernel -C none \
		-a 0x48008000 -e 0x48008000 \
		-n '$(call toupper,$(LINUX_KARCH)) $(VERSION_DIST) Linux-$(LINUX_VERSION)' \
		-d $@ $@.new
	mv $@.new $@
endef

define Device/Default
  PROFILES := Default
  DEVICE_VENDOR := ZTE
  DEVICE_DTS_DIR := ../dts
  KERNEL_LOADADDR := 0x40008000
  KERNEL_ENTRY := 0x40008000
  KERNEL_NAME := zImage
  KERNEL_SIZE := 4096k
  KERNEL := kernel-bin | append-dtb | uImage none | zte-slot-marker | \
	check-size $$(KERNEL_SIZE)
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  KERNEL_INITRAMFS := kernel-bin | append-dtb | uImage-ramboot
  KERNEL_INITRAMFS_SUFFIX := .uImage
endef

define Device/zte_zxhn-h3600
  DEVICE_MODEL := ZXHN H3600
  DEVICE_ALT0_VENDOR := ZTE
  DEVICE_ALT0_MODEL := ZXHN H1600
  DEVICE_DTS := zx279128s-zte-zxhn-h3600
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7915-firmware kmod-usb3 \
	kmod-usb-dwc3-generic-plat kmod-usb-ledtrig-usbport
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := The flash layout changed to UBI. \
	From an image of the old layout, boot the matching initramfs over \
	TFTP and install sysupgrade.bin without keeping settings, then \
	restore a configuration backup. Do not force an old-layout upgrade.
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += zte_zxhn-h3600

$(eval $(call BuildImage))
