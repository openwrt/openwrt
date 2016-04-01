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

define Build/ubnt-erx-factory-compat
	echo '21001:6' > $@.compat
	$(TAR) -cf $@ --transform='s/^.*/compat/' $@.compat
	$(RM) $@.compat
endef

define Build/ubnt-erx-factory-kernel
	if [ -e $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE) ]; then \
		$(TAR) -rf $@ --transform='s/^.*/vmlinux.tmp/' $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE); \
		\
		md5sum --binary $(KDIR)/tmp/$(KERNEL_INITRAMFS_IMAGE) | awk '{print $$1}'> $@.md5; \
		$(TAR) -rf $@ --transform='s/^.*/vmlinux.tmp.md5/' $@.md5; \
		$(RM) $@.md5; \
	fi
endef

define Build/ubnt-erx-factory-rootfs
	echo "dummy" > $@.rootfs
	$(TAR) -rf $@ --transform='s/^.*/squashfs.tmp/' $@.rootfs

	md5sum --binary $@.rootfs | awk '{print $$1}'> $@.md5
	$(TAR) -rf $@ --transform='s/^.*/squashfs.tmp.md5/' $@.md5
	$(RM) $@.md5
	$(RM) $@.rootfs
endef

define Build/ubnt-erx-factory-version
	echo '$(BOARD) $(VERSION_CODE) $(VERSION_NUMBER)' > $@.version
	$(TAR) -rf $@ --transform='s/^.*/version.tmp/' $@.version
	$(RM) $@.version
endef

# We need kernel+initrams fit into kernel partition
define Build/ubnt-erx-factory-check-size
	@[ $$(($(subst k,* 1024,$(subst m, * 1024k,$(1))))) -ge "$$($(TAR) -xf $@ vmlinux.tmp -O | wc -c)" ] || { \
		echo "WARNING: Initramfs kernel for image $@ is too big (kernel size: $$($(TAR) -xf $@ vmlinux.tmp -O | wc -c), max size $(1))" >&2; \
		$(RM) -f $@; \
	}

	@[ "$$($(TAR) -xf $@ vmlinux.tmp -O | wc -c)" -gt 0 ] || { \
		echo "WARNING: Kernel for image $@ not found" >&2; \
		$(RM) -f $@; \
	}
endef

ifeq ($(SUBTARGET),mt7621)
  TARGET_DEVICES += mt7621 wsr-600 wsr-1166 dir-860l-b1 firewrt pbr-m1 re6500 sap-g3200u3 ubnt-erx witi wf-2881 zbt-wg2626
endif

define Device/mt7621
  DTS := MT7621
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
endef

define Device/wsr-600
  DTS := WSR-600
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/re6500
  DTS := RE6500
endef

define Device/wsr-1166
  DTS := WSR-1166
  IMAGE/sysupgrade.bin := trx | pad-rootfs
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/dir-860l-b1
  DTS := DIR-860L-B1
  IMAGES += factory.bin
  KERNEL := kernel-bin | patch-dtb | relocate-kernel | lzma | uImage lzma
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

define Device/firewrt
  DTS := FIREWRT
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/pbr-m1
  DTS := PBR-M1
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/sap-g3200u3
  DTS := SAP-G3200U3
endef

define Device/witi
  DTS := WITI
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/zbt-wg2626
  DTS := ZBT-WG2626
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
endef

define Device/wf-2881
  DTS := WF-2881
  BLOCKSIZE := 128KiB
  PAGESIZE := 2048
  FILESYSTEMS := squashfs
  IMAGE_SIZE := 132382720
  KERNEL := $(KERNEL_DTB) | pad-offset 131072 64 | uImage lzma
  IMAGE/sysupgrade.bin := append-kernel | append-ubi | check-size $$$$(IMAGE_SIZE)
endef

define Device/ubnt-erx
  DTS := UBNT-ERX
  FILESYSTEMS := squashfs
  KERNEL_SIZE := 3145728
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGES := sysupgrade.tar $(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),factory-initramfs.tar)
  IMAGE/factory-initramfs.tar := ubnt-erx-factory-compat | \
				 ubnt-erx-factory-kernel | \
				 ubnt-erx-factory-rootfs | \
				 ubnt-erx-factory-version | \
				 ubnt-erx-factory-check-size $$(KERNEL_SIZE)
  IMAGE/sysupgrade.tar := sysupgrade-nand
endef

# FIXME: is this still needed?
define Image/Prepare
#define Build/Compile
	rm -rf $(KDIR)/relocate
	$(CP) ../../generic/image/relocate $(KDIR)
	$(MAKE) -C $(KDIR)/relocate KERNEL_ADDR=$(KERNEL_LOADADDR) CROSS_COMPILE=$(TARGET_CROSS)
	$(CP) $(KDIR)/relocate/loader.bin $(KDIR)/loader.bin
endef
