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

define Build/tplink-jffs2
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 \
		--little-endian \
		--no-cleanmarkers \
		--eraseblock=$(patsubst %k,%KiB,$(BLOCKSIZE)) \
		--root=$(1) \
		--output=$(2) \
		--squash \
		--compression-mode=none

endef

define Build/tplink-jffs2-incremental
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2\
		--little-endian \
		--no-cleanmarkers \
		--eraseblock=$(patsubst %k,%KiB,$(BLOCKSIZE)) \
		--root=$(1) \
		--output=$(3) \
		--squash \
		--compression-mode=none \
		--incremental=$(2)

endef

define Build/tplink-jffs2-kernel
	mv $@ $@.kernel

	rm -rf $@-kernel
	mkdir -p $@-kernel
	
	# generate the tag_kernel file
	echo "dummy" > $@-kernel/dummy_file
	echo "dummy" > $@-kernel/dummy_file2
	
	#size=$(shell stat -c%s $@.kernel); \
	#totalsize="$$(( $$size + $$size + 1024 ))"; \
	#mktag_kernel -s "$$totalsize" -o $@-kernel/tag_kernel
	mktag_kernel -s $(1) -o $@-kernel/tag_kernel
	#mkdir -p $@-kernel/etc/
	#ln -s /tag_kernel $@-kernel/etc/tag_kernel

	echo "dummy" > $@-kernel/dummy_file3
	echo "dummy" > $@-kernel/dummy_file4

	$(call Build/tplink-jffs2,$@-kernel,$@-base)
	
	cp $@.kernel $@
	$(call Build/uImage-tplink-c9,standalone '$(call toupper,$(LINUX_KARCH)) $(VERSION_DIST) Linux-$(LINUX_VERSION)')
	# Add vmlinuz as the last file in the JFFS2 partition
	cp $@ $@-kernel/imageuboot
	
	cp $@.kernel $@
	$(call Build/uImage-tplink-c9,firmware 'OS IMAGE ($(VERSION_DIST))')
	cp $@ $@-kernel/vmlinuz
	
	$(call Build/tplink-jffs2-incremental,$@-kernel,$@-base,$@-addon)
	
	cat $@-base $@-addon > $@
	# The JFFS2 partition creation should result in the following
	# layout:
	# 1) tag_kernel
	# 2) uboot_image
	# 3) vmlinuz 
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
