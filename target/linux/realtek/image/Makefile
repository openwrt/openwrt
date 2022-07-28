# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR = 0x80000000
KERNEL_ENTRY = 0x80000400

DEVICE_VARS += ZYXEL_VERS DLINK_KERNEL_PART_SIZE
DEVICE_VARS += CAMEO_KERNEL_PART CAMEO_ROOTFS_PART CAMEO_CUSTOMER_SIGNATURE CAMEO_BOARD_VERSION

define Build/zyxel-vers
       ( echo VERS;\
       for hw in $(ZYXEL_VERS); do\
               echo -n "V9.99($$hw.0) | ";\
               date -d @$(SOURCE_DATE_EPOCH) +%m/%d/%Y;\
       done ) >> $@
endef

define Build/dlink-cameo
	$(SCRIPT_DIR)/cameo-tag.py $@ $(DLINK_KERNEL_PART_SIZE)
endef

define Build/dlink-version
	echo -n "OpenWrt" >> $@
	dd if=/dev/zero bs=25 count=1 >> $@
endef

define Build/dlink-headers
        dd if=$@ bs=$(DLINK_KERNEL_PART_SIZE) count=1 of=$@.kernel_part; \
        dd if=$@ bs=$(DLINK_KERNEL_PART_SIZE) skip=1 of=$@.rootfs_part; \
        $(SCRIPT_DIR)/cameo-imghdr.py $@.kernel_part $@.kernel_part.hex \
                "$(DEVICE_MODEL)" os $(CAMEO_KERNEL_PART) \
                $(CAMEO_CUSTOMER_SIGNATURE) \
                $(CAMEO_BOARD_VERSION) \
                $(KERNEL_LOADADDR); \
        $(SCRIPT_DIR)/cameo-imghdr.py $@.rootfs_part $@.rootfs_part.hex \
                "$(DEVICE_MODEL)" squashfs $(CAMEO_ROOTFS_PART) \
                $(CAMEO_CUSTOMER_SIGNATURE) \
                $(CAMEO_BOARD_VERSION); \
        cat $@.kernel_part.hex $@.rootfs_part.hex > $@
endef

define Device/Default
  PROFILES = Default
  KERNEL := kernel-bin | append-dtb | gzip | uImage gzip
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | uImage gzip
  DEVICE_DTS_DIR := ../dts-$(KERNEL_PATCHVER)
  DEVICE_DTS = $$(SOC)_$(1)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 64k | append-rootfs | pad-rootfs | \
	check-size | append-metadata
endef

# "NGE" refers to the uImage magic
define Device/netgear_nge
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  SOC := rtl8380
  IMAGE_SIZE := 14848k
  UIMAGE_MAGIC := 0x4e474520
  DEVICE_VENDOR := NETGEAR
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
