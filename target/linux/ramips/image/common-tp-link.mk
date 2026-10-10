DEVICE_VARS += TPLINK_FLASHLAYOUT TPLINK_HWID TPLINK_HWREV TPLINK_HWREVADD
DEVICE_VARS += TPLINK_HVERSION TPLINK_BOARD_ID TPLINK_HEADER_VERSION

define Build/tplink-v1-okli-image
	cp $(IMAGE_KERNEL) $(IMAGE_ROOTFS).$(word 2,$(1))
	cat $(IMAGE_ROOTFS) >> $(IMAGE_ROOTFS).$(word 2,$(1))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(TPLINK_HWID) -W $(TPLINK_HWREV) -F $(TPLINK_FLASHLAYOUT) \
		-N "$(VERSION_DIST)" -V $(REVISION) -m $(TPLINK_HEADER_VERSION) \
		-k "$(KDIR)/loader-$(word 1,$(1)).$(LOADER_TYPE)" -E $(KERNEL_LOADADDR) \
		-r $(IMAGE_ROOTFS).$(word 2,$(1)) -o $@.new -j -X 0x40000 \
		-a $(call rootfs_align,$(FILESYSTEM)) $(wordlist 3,$(words $(1)),$(1)) \
		$(if $(findstring sysupgrade,$(word 2,$(1))),-s) && mv $@.new $@ || rm -f $@
	rm -f $(IMAGE_ROOTFS).$(word 2,$(1))
endef

define Build/tplink-v2-okli-factory
	cp $(IMAGE_KERNEL) $(IMAGE_ROOTFS).okli
	cat $(IMAGE_ROOTFS) >> $(IMAGE_ROOTFS).okli
	-if $(STAGING_DIR_HOST)/bin/mktplinkfw2 -e \
		-H $(TPLINK_HWID) -W $(TPLINK_HWREV) -w $(TPLINK_HWREVADD) \
		-F "$(TPLINK_FLASHLAYOUT)" -T $(TPLINK_HVERSION) -V "ver. 2.0" \
		-L $(LZMA_TEXT_START) -E $(LZMA_TEXT_START) -R 0xfe00 \
		-k "$(KDIR)/loader-$(1).$(LOADER_TYPE)" \
		-r $(IMAGE_ROOTFS).okli -s -o $(IMAGE_ROOTFS).okli.inner \
	&& loader_len=$$(stat -c%s "$(KDIR)/loader-$(1).$(LOADER_TYPE)") \
	&& outer_kernel_ofs=$$(( (loader_len + 512 + 3) / 4 * 4 )) \
	&& prefix_len=$$(( ($(LOADER_FLASH_OFFS) - 0x20000) - 0xfe00 - outer_kernel_ofs )) \
	&& { [ "$$prefix_len" -ge 0 ] || { \
		echo "ERROR: LOADER_FLASH_OFFS $(LOADER_FLASH_OFFS) is too small for this loader" >&2; \
		false; \
	}; } \
	&& truncate -s "$$prefix_len" $(IMAGE_ROOTFS).okli.prefix \
	&& cat $(IMAGE_ROOTFS).okli.inner >> $(IMAGE_ROOTFS).okli.prefix \
	&& $(STAGING_DIR_HOST)/bin/mktplinkfw2 \
		-H $(TPLINK_HWID) -W $(TPLINK_HWREV) \
		-w $(TPLINK_HWREVADD) -F "$(TPLINK_FLASHLAYOUT)" \
		-T $(TPLINK_HVERSION) -V "ver. 2.0" -a 0x4 -j \
		-L $(LZMA_TEXT_START) -E $(LZMA_TEXT_START) \
		-k "$(KDIR)/loader-$(1).$(LOADER_TYPE)" \
		-r $(IMAGE_ROOTFS).okli.prefix -o $@.new -e \
	&& cat $@.new >> $@; then :; else rm -f $@; fi
	# The outer image above (-a 0x4) places its own "kernel" (the same
	# OKLI loader) right after its header, then rounds that up to a
	# 4-byte boundary to get the outer rootfs offset; the zero prefix
	# has to make up the remaining distance so that the inner image's
	# own rootfs offset (0xfe00, from -R above -- where the real,
	# OKLI-magic-tagged kernel starts) lands exactly at
	# LOADER_FLASH_OFFS on flash. factory.bin itself starts at flash
	# offset 0x20000 (the first 128k, "boot", is never touched by it):
	#   prefix_len = (LOADER_FLASH_OFFS - 0x20000) - 0xfe00 - outer_kernel_ofs
	rm -f $@.new $(IMAGE_ROOTFS).okli $(IMAGE_ROOTFS).okli.inner $(IMAGE_ROOTFS).okli.prefix
endef

define Build/uImage-tplink-c9
	mkimage \
		-A $(LINUX_KARCH) \
		-O linux \
		-T $(word 1,$(1)) \
		-C none \
		-a $(KERNEL_LOADADDR) \
		-e $(KERNEL_LOADADDR) \
		-n $(wordlist 2,$(words $(1)),$(1)) \
		-d $@ $@.new
	mv $@.new $@
endef

define Device/tplink-v1
  DEVICE_VENDOR := TP-Link
  TPLINK_FLASHLAYOUT :=
  TPLINK_HWID :=
  TPLINK_HWREV := 0x1
  TPLINK_HEADER_VERSION := 1
  KERNEL := $(KERNEL_DTB)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | tplink-v1-header -e -O
  IMAGES += factory.bin
  IMAGE/factory.bin := tplink-v1-image factory -e -O
  IMAGE/sysupgrade.bin := tplink-v1-image sysupgrade -e -O | check-size | \
	append-metadata
endef

define Device/tplink-v1-okli
  $(Device/tplink-v1)
  LOADER_TYPE := bin
  LOADER_FLASH_OFFS := 0x21000
  COMPILE := loader-$(1).bin
  COMPILE/loader-$(1).bin := loader-okli-compile | pad-to 64k | lzma | pad-to 3584
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma -M 0x4f4b4c49
  IMAGE/factory.bin := tplink-v1-okli-image $(1) factory -e -O
  IMAGE/sysupgrade.bin := tplink-v1-okli-image $(1) sysupgrade -e -O | check-size | \
	append-metadata
endef

define Device/tplink-v2
  DEVICE_VENDOR := TP-Link
  TPLINK_FLASHLAYOUT :=
  TPLINK_HWID :=
  TPLINK_HWREV := 0x1
  TPLINK_HWREVADD := 0x0
  TPLINK_HVERSION := 3
  KERNEL := $(KERNEL_DTB)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | tplink-v2-header -e
  IMAGES += factory.bin
  IMAGE/factory.bin := tplink-v2-image -e
  IMAGE/sysupgrade.bin := tplink-v2-image -s -e | check-size | \
	append-metadata
endef

define Device/tplink-v2-okli
  $(Device/tplink-v2)
  LOADER_TYPE := bin
  LZMA_TEXT_START := 0x80a00000
  COMPILE := loader-$(1).bin
  COMPILE/loader-$(1).bin := loader-okli-compile | pad-to 64k | lzma | pad-to 3584
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma -M 0x4f4b4c49 | pad-to 64k
  IMAGE/factory.bin := tplink-v2-okli-factory $(1)
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
endef

define Device/tplink-safeloader
  DEVICE_VENDOR := TP-Link
  TPLINK_BOARD_ID :=
  TPLINK_HWID := 0x0
  TPLINK_HWREV := 0x0
  TPLINK_HEADER_VERSION := 1
  KERNEL := $(KERNEL_DTB) | tplink-v1-header -e -O
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := append-rootfs | tplink-safeloader sysupgrade | \
	check-size | append-metadata
  IMAGE/factory.bin := append-rootfs | tplink-safeloader factory
endef
