DEVICE_VARS += SERCOMM_KERNEL_OFFSET SERCOMM_ROOTFS_OFFSET
DEVICE_VARS += SERCOMM_KERNEL2_OFFSET SERCOMM_ROOTFS2_OFFSET

define Build/sercomm-append-tail
	printf 16 | dd seek=$$((0x90)) of=$@ bs=1 conv=notrunc 2>/dev/null
	printf 11223344556677889900112233445566 | \
		sed 's/../\\x&/g' | xargs -d . printf >> $@
endef

define Build/sercomm-crypto
	$(TOPDIR)/scripts/sercomm-crypto.py \
		--input-file $@ \
		--key-file $@.key \
		--output-file $@.ser \
		--version $(SERCOMM_SWVER)
	$(STAGING_DIR_HOST)/bin/openssl enc -md md5 -aes-256-cbc \
		-in $@ \
		-out $@.enc \
		-K `cat $@.key` \
		-iv 00000000000000000000000000000000
	dd if=$@.enc >> $@.ser 2>/dev/null
	mv $@.ser $@
	rm -f $@.enc $@.key
endef

define Build/sercomm-factory-cpj
	dd bs=$$((0x1fff00)) count=1 if=$@ of=$@.kernel conv=notrunc \
		2>/dev/null
	dd bs=$$((0x1fff00)) skip=1 if=$@ of=$@.rootfs1 conv=notrunc \
		2>/dev/null
	cp $@.rootfs1 $@.rootfs2
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@.kernel \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-image $@.rootfs1 \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.header1
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@.kernel \
		--kernel-offset $(SERCOMM_KERNEL2_OFFSET) \
		--rootfs-image $@.rootfs2 \
		--rootfs-offset $(SERCOMM_ROOTFS2_OFFSET) \
		--output-header $@.header2
	cat $@.header1 $@.kernel > $@.kernel1
	cat $@.header2 $@.kernel > $@.kernel2
	rm $@.header1 $@.header2 $@.kernel
	$(call Build/sercomm-part-tag-common,kernel $@.kernel1)
	$(call Build/sercomm-part-tag-common,kernel2 $@.kernel2)
	$(call Build/sercomm-part-tag-common,rootfs $@.rootfs1)
	$(call Build/sercomm-part-tag-common,rootfs2 $@.rootfs2)
	cat $@.kernel2 $@.rootfs2 $@.kernel1 $@.rootfs1 > $@
	rm $@.kernel1 $@.rootfs1 $@.kernel2 $@.rootfs2
endef

define Build/sercomm-factory-cqr
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.fhdr \
		--extra-padding-size 0x190
	printf $$(stat -c%s $(IMAGE_KERNEL)) | \
		dd seek=$$((0x70)) of=$@.fhdr bs=1 conv=notrunc 2>/dev/null
	printf $$(($$(stat -c%s $@)-$$(stat -c%s $(IMAGE_KERNEL))-$$((0x200)))) | \
		dd seek=$$((0x80)) of=$@.fhdr bs=1 conv=notrunc 2>/dev/null
	dd if=$@ >> $@.fhdr 2>/dev/null
	mv $@.fhdr $@
endef

define Build/sercomm-kernel
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@ \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.hdr
	dd if=$@ >> $@.hdr 2>/dev/null
	mv $@.hdr $@
endef

define Build/sercomm-kernel-factory
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@ \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.khdr1
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@ \
		--kernel-offset $(SERCOMM_KERNEL2_OFFSET) \
		--rootfs-offset $(SERCOMM_ROOTFS2_OFFSET) \
		--output-header $@.khdr2
	cat $@.khdr1 $@.khdr2 > $@.khdr
	dd if=$@ >> $@.khdr 2>/dev/null
	mv $@.khdr $@
endef

define Build/sercomm-mkhash
	dd bs=$$((0x400)) skip=1 if=$@ conv=notrunc 2>/dev/null | \
		$(MKHASH) md5 | awk '{print $$1}' | tr -d '\n' | \
		dd seek=$$((0x1e0)) of=$@ bs=1 conv=notrunc 2>/dev/null
endef

define Build/sercomm-part-tag
	$(call Build/sercomm-part-tag-common,$(word 1,$(1)) $@)
endef

define Build/sercomm-part-tag-common
	$(eval file=$(word 2,$(1)))
	$(TOPDIR)/scripts/sercomm-partition-tag.py \
		--input-file $(file) \
		--output-file $(file).tmp \
		--part-name $(word 1,$(1)) \
		--part-version $(SERCOMM_SWVER)
	mv $(file).tmp $(file)
endef

define Build/sercomm-payload
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.pid \
		--extra-padding-size 0x10 \
		--extra-padding-first-byte 0x0a
	$(TOPDIR)/scripts/sercomm-payload.py \
		--input-file $@ \
		--output-file $@.tmp \
		--pid-file $@.pid
	mv $@.tmp $@
	rm $@.pid
endef

define Build/sercomm-pid-setbit
	printf 1 | dd seek=$$(($(1))) of=$@ bs=1 conv=notrunc 2>/dev/null
endef

define Build/sercomm-prepend-tagged-kernel
	$(CP) $(IMAGE_KERNEL) $(IMAGE_KERNEL).tagged
	$(call Build/sercomm-part-tag-common,$(word 1,$(1)) \
		$(IMAGE_KERNEL).tagged)
	dd if=$@ >> $(IMAGE_KERNEL).tagged 2>/dev/null
	mv $(IMAGE_KERNEL).tagged $@
endef

define Build/sercomm-reset-slot1-chksum
	printf "\xff\xff\xff\xff" | \
		dd of=$@ seek=$$((0x118)) bs=1 conv=notrunc 2>/dev/null
endef

define Build/sercomm-sysupgrade-cpj
	dd bs=$$((0x1fff00)) count=1 if=$@ of=$@.kernel conv=notrunc \
		2>/dev/null
	dd bs=$$((0x1fff00)) skip=1 if=$@ of=$@.rootfs conv=notrunc \
		2>/dev/null
	$(TOPDIR)/scripts/sercomm-kernel-header.py \
		--kernel-image $@.kernel \
		--kernel-offset $(SERCOMM_KERNEL_OFFSET) \
		--rootfs-image $@.rootfs \
		--rootfs-offset $(SERCOMM_ROOTFS_OFFSET) \
		--output-header $@.header
	cat $@.header $@.kernel $@.rootfs > $@
	rm $@.header $@.kernel $@.rootfs
endef

define Device/sercomm
  $(Device/nand)
  LOADER_TYPE := bin
  IMAGES += factory.img
endef

define Device/sercomm_cpj
  SOC := mt7620a
  DEVICE_VENDOR := Rostelecom
  DEVICE_ALT0_VENDOR := Sercomm
  IMAGE_SIZE := 7743k
  SERCOMM_HWID := CPJ
  SERCOMM_HWVER := 10000
  SERCOMM_SWVER := 1001
  SERCOMM_KERNEL_OFFSET := 0x70100
  SERCOMM_ROOTFS_OFFSET := 0x270000
  SERCOMM_KERNEL2_OFFSET := 0x800100
  SERCOMM_ROOTFS2_OFFSET := 0xa00000
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	sercomm-sysupgrade-cpj | pad-rootfs | check-size | \
	append-metadata
ifeq ($(IB),)
  ARTIFACTS := initramfs-factory.img
endif
  DEVICE_PACKAGES := kmod-mt76x2
endef

define Device/sercomm_cxx_dxx
  $(Device/sercomm)
  KERNEL_SIZE := 6144k
  KERNEL_LOADADDR := 0x81001000
  LZMA_TEXT_START := 0x82800000
  SERCOMM_KERNEL_OFFSET := 0x400100
  SERCOMM_ROOTFS_OFFSET := 0x1000000
endef

define Device/sercomm_cxx
  $(Device/sercomm_cxx_dxx)
  SERCOMM_KERNEL2_OFFSET := 0xa00100
  SERCOMM_ROOTFS2_OFFSET := 0x3000000
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma
  IMAGE/sysupgrade.bin := append-kernel | sercomm-kernel | \
	sysupgrade-tar kernel=$$$$@ | append-metadata
  IMAGE/factory.img := append-kernel | sercomm-kernel-factory | \
	append-ubi | sercomm-factory-cqr | sercomm-mkhash
endef

define Device/sercomm_dxx
  $(Device/sercomm_cxx_dxx)
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma | sercomm-kernel
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	lzma -a0 | uImage lzma
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory.img := append-ubi | check-size | \
	sercomm-part-tag rootfs | sercomm-prepend-tagged-kernel kernel | \
	gzip | sercomm-payload | sercomm-crypto
endef

define Device/sercomm_s1500
  $(Device/sercomm)
  SERCOMM_KERNEL_OFFSET := 0x1700100
  SERCOMM_ROOTFS_OFFSET := 0x1f00000
  SERCOMM_KERNEL2_OFFSET := 0x1b00100
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	lzma -a0 | uImage lzma
  IMAGE/sysupgrade.bin := append-kernel | sercomm-kernel | \
	sysupgrade-tar kernel=$$$$@ | append-metadata
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3
endef
