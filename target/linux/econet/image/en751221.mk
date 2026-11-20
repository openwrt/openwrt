TRX_ENDIAN := be

define Device/chinamobile_gs3101
  DEVICE_VENDOR := ChinaMobile
  DEVICE_MODEL := GS3101
  DEVICE_DTS := en7526f_chinamobile_gs3101
  KERNEL_SIZE := 4096k
  NAND_SIZE := 256m
  KERNEL := kernel-bin | append-dtb | lzma | kernel-trx
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := tclinux.trx sysupgrade.bin
  IMAGE/tclinux.trx := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603
endef
TARGET_DEVICES += chinamobile_gs3101

define Device/en751221_generic
  DEVICE_VENDOR := EN751221 Family
  DEVICE_MODEL := Initramfs Image
  DEVICE_TITLE := EN751221 Initramfs Image
  DEVICE_DESCRIPTION := In-memory build for testing and recovery of EN751221 SoCs
  DEVICE_DTS := en751221_generic
endef
TARGET_DEVICES += en751221_generic

define Device/huawei_hg2821t-u
  DEVICE_VENDOR := Huawei
  DEVICE_MODEL := HG2821T-U
  DEVICE_DTS := en751221_huawei_hg2821t-u
  KERNEL_DECOMPRESSED_SIZE := 7672k
  KERNEL_SIZE := 4096k
  IMAGE_SIZE := 57344k
  FACTORY_SIZE := 40960k
  NAND_SIZE := 256m
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | \
    check-size $$(KERNEL_DECOMPRESSED_SIZE) | lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := kernel.bin rootfs.bin sysupgrade.bin
  IMAGE/kernel.bin := append-kernel
  IMAGE/rootfs.bin := append-ubi | check-size $$(FACTORY_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-usb3
endef
TARGET_DEVICES += huawei_hg2821t-u

define Device/nokia_g240g-e
  DEVICE_VENDOR := Nokia
  DEVICE_MODEL := G-240G-E
  DEVICE_DTS := en751221_nokia_g240g-e
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
  DEVICE_PACKAGES := kmod-usb3
endef
TARGET_DEVICES += nokia_g240g-e

define Device/smartfiber_xp8421-b
  DEVICE_VENDOR := SmartFiber
  DEVICE_MODEL := XP8421-B
  DEVICE_DTS := en751221_smartfiber_xp8421-b
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603 kmod-mt76x2
endef
TARGET_DEVICES += smartfiber_xp8421-b

# NOTE: This will not work for upgrading from factory because it requires a cryptographic signature
#       however, it it can be flashed, then it will boot correctly.
define Device/tplink_archer-vr1200v-v2
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := Archer vr1200v
  DEVICE_VARIANT := v2
  TPLINK_FLASHLAYOUT := 16Mmtk
  TPLINK_HWID := 0x0b473502
  TPLINK_HWREV := 0x0006007c
  TPLINK_HWREVADD := 0x0
  TPLINK_HVERSION := 3
  DEVICE_DTS := en751221_tplink_archer-vr1200v-v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | lzma | pad-to 4193792 | append-rootfs | \
    tplink-v2-header -R 0x400000
endef
TARGET_DEVICES += tplink_archer-vr1200v-v2

define Device/zyxel_pmg5617ga
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := PMG5617GA
  DEVICE_DTS := en751221_zyxel_pmg5617ga
  IMAGES := tclinux.trx
  IMAGE/tclinux.trx := append-kernel | lzma | tclinux-trx
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603 kmod-mt76x2
endef
TARGET_DEVICES += zyxel_pmg5617ga
# Append to target/linux/econet/image/en751221.mk
# ZTE ZXHN H267A (EcoNet EN751221). Builds a RAM-boot initramfs image, a
# combined tclinux.bin the bootloader's recovery page splits across NAND
# (kernel -> "kernel1", rootfs -> "rootfs_ro"; UBI lives past kernel1), and
# a sysupgrade.bin for in-place updates. The kernel is wrapped in the
# TrendChip "free bootbase" tclinux format the stock bootloader accepts
# (256-byte header + LZMA payload, via tclinux-free-bootbase.py); the
# tclinux-free-bootbase-jump trampoline reconciles the bootbase's
# decompress target with OpenWrt's kernel entry point. The bootloader
# CRC-checks the rootfs slot on every boot using the length + CRC32 carried
# in the kernel header, so the rootfs stays a plain, immutable squashfs
# (the UBI overlay lives in its own partition instead).
#
# Requires CONFIG_TARGET_ROOTFS_SQUASHFS=y.

# Guards against the rootfs outgrowing "rootfs_ro" (FACTORY_SIZE) -- the
# same check a standalone rootfs.bin's own check-size step used to carry.
define Build/h267a-check-rootfs-size
	@imagesize="$$(stat -c%s $(IMAGE_ROOTFS))"; \
	limitsize="$$(($(call exp_units,$(FACTORY_SIZE))))"; \
	[ $$limitsize -ge $$imagesize ] || \
		$(call ERROR_MESSAGE,    WARNING: rootfs is too big for rootfs_ro: $$imagesize > $$limitsize)
endef

# sysupgrade.bin's kernel member needs the tclinux header (256-byte header +
# LZMA payload) $(IMAGE_KERNEL) doesn't have on its own -- fslen/fscrc must
# match the tar's own "root" member, since platform.sh writes it verbatim to
# the kernel1 partition and the bootloader CRC-checks the paired rootfs slot
# every boot.
define Build/h267a-sysupgrade-tar
	./tclinux-free-bootbase.py --payload $(IMAGE_KERNEL) --out $(KDIR)/tmp/$(DEVICE_NAME)-syskernel --rootfs $(IMAGE_ROOTFS)
	sh $(TOPDIR)/scripts/sysupgrade-tar.sh --board $(DEVICE_NAME) \
		--kernel $(KDIR)/tmp/$(DEVICE_NAME)-syskernel --rootfs $(IMAGE_ROOTFS) $@
endef

# The recovery page's tclinux.bin upload wants kernel+rootfs concatenated
# into one file. $(IMAGE_KERNEL) is the raw, unwrapped kernel payload --
# wrap it with the same tclinux header h267a-sysupgrade-tar gives its own
# kernel member, then append rootfs verbatim.
define Build/h267a-tclinux-combined
	./tclinux-free-bootbase.py --payload $(IMAGE_KERNEL) --out $(KDIR)/tmp/$(DEVICE_NAME)-tclinux-kernel --rootfs $(IMAGE_ROOTFS)
	cat $(KDIR)/tmp/$(DEVICE_NAME)-tclinux-kernel $(IMAGE_ROOTFS) > $@
endef

define Device/zte_zxhn-h267a
  DEVICE_VENDOR := ZTE
  DEVICE_MODEL := ZXHN H267A
  DEVICE_DESCRIPTION := RAM-boot initramfs plus flashable tclinux NAND images
  DEVICE_DTS := en751221_zte_zxhn-h267a
  # WiFi: MT7592N (mt7603e) + MT7612EN (mt76x2e), PCIe variants as on
  # sibling boards. wpad-basic-mbedtls listed explicitly: no wpad variant
  # is in DEFAULT_PACKAGES(.router), so without it the radios probe but
  # nothing runs AP/supplicant. Also supplies the mbedtls stack luci-ssl
  # reuses.
  # ubi-utils: on-device inspection for the UBI-volume rootfs_data.
  # luci-ssl: LuCI preinstalled with HTTPS (px5g self-signed cert).
  # kmod-gpio-button-hotplug: binds the reset/wps/wlan gpio-keys-polled
  # node to /etc/rc.button/*; nothing else here pulls it in.
  # USB: kmod-usb2 + kmod-usb-ohci drive the legacy EHCI/OHCI host blocks
  # (see the board DTS USB section for why the SSUSB xHCI is unused).
  # kmod-usb-storage(-extras)/block-mount plus vfat/exfat/ext4/ntfs3 +
  # nls codepages for USB storage; usbutils for lsusb. USB-serial
  # (ftdi/pl2303/ch341/cp210x) for console dongles, kmod-usb-printer +
  # p910nd + luci-app-p910nd for feature parity with stock. USB LTE/3G
  # modem baseline (qmi/cdc-ether/cdc-mbim/rndis + matching proto
  # packages), untested against specific hardware. kmod-usb-ledtrig-
  # usbport: USB panel LED uses the kernel's own trigger.
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-gpio-button-hotplug \
	kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport usbutils \
	kmod-usb-storage kmod-usb-storage-extras block-mount \
	kmod-fs-vfat kmod-fs-exfat kmod-fs-ext4 kmod-fs-ntfs3 \
	kmod-nls-cp437 kmod-nls-iso8859-1 kmod-nls-utf8 \
	kmod-usb-printer p910nd luci-app-p910nd \
	kmod-usb-serial kmod-usb-serial-option kmod-usb-serial-ftdi \
	kmod-usb-serial-pl2303 kmod-usb-serial-ch341 kmod-usb-serial-cp210x \
	kmod-usb-net kmod-usb-net-qmi-wwan kmod-usb-net-cdc-ether \
	kmod-usb-net-cdc-mbim kmod-usb-net-rndis \
	uqmi luci-proto-qmi umbim luci-proto-mbim comgt-ncm luci-proto-3g \
	ubi-utils luci-ssl wpad-basic-mbedtls
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  # The rootfs flashes to "rootfs_ro"; UBI lives past kernel1 in its own
  # partition, and files/lib/preinit/79_h267a_ubi_volume creates the
  # rootfs_data volume in it on first boot.
  FACTORY_SIZE := 20480k
  IMAGES := sysupgrade.bin tclinux.bin
  IMAGE/sysupgrade.bin := h267a-check-rootfs-size | h267a-sysupgrade-tar | append-metadata
  # Ready to upload as-is after a rename to "tclinux.bin" -- the recovery
  # page's upload form checks the filename.
  IMAGE/tclinux.bin := h267a-check-rootfs-size | h267a-tclinux-combined
endef
TARGET_DEVICES += zte_zxhn-h267a
