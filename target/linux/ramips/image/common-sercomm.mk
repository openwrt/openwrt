DEVICE_VARS += SERCOMM_KERNEL_OFFSET SERCOMM_ROOTFS_OFFSET
DEVICE_VARS += SERCOMM_KERNEL2_OFFSET SERCOMM_ROOTFS2_OFFSET

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

define Build/sercomm-factory-cqr
	$(TOPDIR)/scripts/sercomm-pid.py \
		--hw-version $(SERCOMM_HWVER) \
		--hw-id $(SERCOMM_HWID) \
		--sw-version $(SERCOMM_SWVER) \
		--pid-file $@.fhdr
	printf $$(stat -c%s $(IMAGE_KERNEL)) | \
		dd seek=$$((0x70)) of=$@.fhdr bs=1 conv=notrunc 2>/dev/null
	printf $$(($$(stat -c%s $@)-$$(stat -c%s $(IMAGE_KERNEL))-$$((0x200)))) | \
		dd seek=$$((0x80)) of=$@.fhdr bs=1 conv=notrunc 2>/dev/null
	dd bs=$$((0x200)) skip=1 if=$@ conv=notrunc 2>/dev/null | \
		$(MKHASH) md5 | awk '{print $$1}' | tr -d '\n' | \
		dd seek=$$((0x1e0)) of=$@.fhdr bs=1 conv=notrunc 2>/dev/null
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

define Build/sercomm-prepend-tagged-kernel
	$(CP) $(IMAGE_KERNEL) $(IMAGE_KERNEL).tagged
	$(call Build/sercomm-part-tag-common,$(word 1,$(1)) \
		$(IMAGE_KERNEL).tagged)
	dd if=$@ >> $(IMAGE_KERNEL).tagged 2>/dev/null
	mv $(IMAGE_KERNEL).tagged $@
endef

define Device/sercomm
  $(Device/nand)
  LOADER_TYPE := bin
  KERNEL_SIZE := 6144k
  KERNEL_LOADADDR := 0x81001000
  LZMA_TEXT_START := 0x82800000
  SERCOMM_KERNEL_OFFSET := 0x400100
  SERCOMM_ROOTFS_OFFSET := 0x1000000
  IMAGES += factory.img
endef

define Device/sercomm_cxx
  $(Device/sercomm)
  SERCOMM_KERNEL2_OFFSET := 0xa00100
  SERCOMM_ROOTFS2_OFFSET := 0x3000000
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma
  IMAGE/sysupgrade.bin := append-kernel | sercomm-kernel | \
	sysupgrade-tar kernel=$$$$@ | append-metadata
  IMAGE/factory.img := append-kernel | sercomm-kernel-factory | \
	append-ubi | sercomm-factory-cqr
endef

define Device/sercomm_dxx
  $(Device/sercomm)
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma -a0 | \
	uImage lzma | sercomm-kernel
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	lzma -a0 | uImage lzma
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/factory.img := append-ubi | check-size | \
	sercomm-part-tag rootfs | sercomm-prepend-tagged-kernel kernel | \
	gzip | sercomm-payload | sercomm-crypto
endef
