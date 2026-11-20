# SPDX-License-Identifier: GPL-2.0-only

define Build/dlink-dap2690-rootfs-tag
	printf '%s' '--PaCkImGs--' >> $@
	dd if=/dev/zero bs=4 count=1 >> $@
	echo -ne "$$(printf '%08x' $$(stat -c%s $(IMAGE_ROOTFS)) | \
		fold -s2 | xargs -I {} echo \\x{} | tr -d '\n')" >> $@
	dd if=/dev/zero bs=12 count=1 >> $@
endef

define Build/dlink-dap2690-wrgg
	$(STAGING_DIR_HOST)/bin/mkwrggimg -b \
		-i $@ -o $@.new -d /dev/mtdblock/1 \
		-m dap2690 -s wapnd06_dkbs_dap2690 \
		-v $(VERSION_DIST) -B $(REVISION)
	mv $@.new $@
endef

define Device/dlink_dap-2690-a1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DAP-2690
  DEVICE_VARIANT := A1
  BOARD_NAME := dlink,dap-2690-a1
  DEVICE_DTS := cn5010_dlink_dap-2690-a1
  DEVICE_PACKAGES += kmod-ath9k kmod-gpio-button-hotplug kmod-leds-gpio \
	wpad-basic-mbedtls -kmod-usb-dwc3-octeon -mkf2fs -e2fsprogs
  KERNEL := kernel-bin | strip-kernel | patch-cmdline | append-dtb-to-elf | \
	lzma-no-dict
  KERNEL_INITRAMFS := kernel-bin | strip-kernel | patch-cmdline | append-dtb-to-elf
  KERNEL_DEPENDS := $$(wildcard $(DTS_DIR)/$(DEVICE_DTS).dts)
  CMDLINE := console=ttyS0,115200 root=/dev/mtdblock5 rootfstype=squashfs
  IMAGES := factory.bin sysupgrade.bin
  IMAGE_SIZE := 15616k
  KERNEL_SIZE := 4096k
  IMAGE/factory.bin := append-kernel | pad-to 4096k | \
	dlink-dap2690-rootfs-tag | append-rootfs | dlink-dap2690-wrgg | check-size
  IMAGE/sysupgrade.bin := append-kernel | pad-to 4096k | \
	dlink-dap2690-rootfs-tag | append-rootfs | dlink-dap2690-wrgg | \
	check-size | append-metadata
endef
TARGET_DEVICES += dlink_dap-2690-a1
