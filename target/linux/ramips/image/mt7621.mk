#
# MT7621 Profiles
#

define Build/seama
	$(STAGING_DIR_HOST)/bin/seama -i $@ $(1)
	mv $@.seama $@
endef

define Build/seama-seal
	$(call Build/seama,-s $@.seama $(1))
endef

define Build/ubnt-erx-factory-image
	if [ -e $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE) ]; then \
		echo '21001:6' > $(1).compat; \
		$(TAR) -cf $(1) --transform='s/^.*/compat/' $(1).compat; \
		\
		$(TAR) -rf $(1) --transform='s/^.*/vmlinux.tmp/' $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE); \
		md5sum --binary $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE) | awk '{print $$1}'> $(1).md5; \
		$(TAR) -rf $(1) --transform='s/^.*/vmlinux.tmp.md5/' $(1).md5; \
		\
		echo "dummy" > $(1).rootfs; \
		$(TAR) -rf $(1) --transform='s/^.*/squashfs.tmp/' $(1).rootfs; \
		\
		md5sum --binary $(1).rootfs | awk '{print $$1}'> $(1).md5; \
		$(TAR) -rf $(1) --transform='s/^.*/squashfs.tmp.md5/' $(1).md5; \
		\
		echo '$(BOARD) $(VERSION_CODE) $(VERSION_NUMBER)' > $(1).version; \
		$(TAR) -rf $(1) --transform='s/^.*/version.tmp/' $(1).version; \
		\
		$(CP) $(1) $(BIN_DIR)/; \
	fi
endef

define Device/mt7621
  DTS := MT7621
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
endef
TARGET_DEVICES += mt7621

define Device/wsr-600
  DTS := WSR-600
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += wsr-600

define Device/re6500
  DTS := RE6500
endef
TARGET_DEVICES += re6500

define Device/wsr-1166
  DTS := WSR-1166
  IMAGE/sysupgrade.bin := trx | pad-rootfs
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += wsr-1166

define Device/dir-860l-b1
  DTS := DIR-860L-B1
  IMAGES += factory.bin
  KERNEL := kernel-bin | patch-dtb | relocate-kernel | lzma | uImage lzma
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  IMAGE/sysupgrade.bin := \
	append-kernel | pad-offset 65536 64 | append-rootfs | \
	seama -m "dev=/dev/mtdblock/2" -m "type=firmware" | \
	pad-rootfs | check-size $$$$(IMAGE_SIZE)
  IMAGE/factory.bin := \
	append-kernel | pad-offset 65536 64 | append-rootfs | pad-rootfs -x 64 | \
	seama -m "dev=/dev/mtdblock/2" -m "type=firmware" | \
	seama-seal -m "signature=wrgac13_dlink.2013gui_dir860lb" | \
	check-size $$$$(IMAGE_SIZE)
endef
TARGET_DEVICES += dir-860l-b1

define Device/firewrt
  DTS := FIREWRT
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += firewrt

define Device/pbr-m1
  DTS := PBR-M1
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += pbr-m1

define Device/sap-g3200u3
  DTS := SAP-G3200U3
endef
TARGET_DEVICES += sap-g3200u3

define Device/witi
  DTS := WITI
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += witi

define Device/wndr3700v5
  DTS := WNDR3700V5
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += wndr3700v5

define Device/zbt-wg2626
  DTS := ZBT-WG2626
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += zbt-wg2626

define Device/zbt-wg3526
  DTS := ZBT-WG3526
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef
TARGET_DEVICES += zbt-wg3526

define Device/wf-2881
  DTS := WF-2881
  BLOCKSIZE := 128KiB
  PAGESIZE := 2048
  FILESYSTEMS := squashfs
  IMAGE_SIZE := 132382720
  KERNEL := $(KERNEL_DTB) | pad-offset 131072 64 | uImage lzma
  IMAGE/sysupgrade.bin := append-kernel | append-ubi | check-size $$$$(IMAGE_SIZE)
endef
TARGET_DEVICES += wf-2881

define Device/ubnt-erx
  DTS := UBNT-ERX
  FILESYSTEMS := squashfs
  KERNEL_SIZE := 3145728
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGES := sysupgrade.tar
  KERNEL_INITRAMFS := $$(KERNEL) | check-size $$(KERNEL_SIZE) | \
			ubnt-erx-factory-image $(KDIR)/tmp/$$(KERNEL_INITRAMFS_PREFIX)-factory.tar
  IMAGE/sysupgrade.tar := sysupgrade-nand
endef
TARGET_DEVICES += ubnt-erx

# FIXME: is this still needed?
define Image/Prepare
#define Build/Compile
	rm -rf $(KDIR)/relocate
	$(CP) ../../generic/image/relocate $(KDIR)
	$(MAKE) -C $(KDIR)/relocate KERNEL_ADDR=$(KERNEL_LOADADDR) CROSS_COMPILE=$(TARGET_CROSS)
	$(CP) $(KDIR)/relocate/loader.bin $(KDIR)/loader.bin
endef
