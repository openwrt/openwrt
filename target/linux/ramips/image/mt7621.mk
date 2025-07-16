#
# MT7621 Profiles
#

include ./common-sercomm.mk
include ./common-tp-link.mk

DEFAULT_SOC := mt7621

DEVICE_VARS += BUFFALO_TRX_MAGIC ELECOM_HWNAME LINKSYS_HWNAME DLINK_HWID

define Image/Prepare
	# For UBI we want only one extra block
	rm -f $(KDIR)/ubi_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/ubi_mark
endef

define Build/append-dlink-covr-metadata
	echo -ne '{"supported_devices": "$(1)", "firmware": "' > $@metadata.tmp
	$(MKHASH) md5 "$@" | head -c32 >> $@metadata.tmp
	echo '"}' >> $@metadata.tmp
	fwtool -I $@metadata.tmp $@
	rm $@metadata.tmp
endef

define Build/append-netis-n6-metadata
	( echo -ne '{ \
		"up_model": "Netis-N6R", \
		"supported_devices": ["mt7621-rfb-ax-nand"], \
		"version": { \
			"dist": "$(call json_quote,$(VERSION_DIST))", \
			"version": "$(call json_quote,$(VERSION_NUMBER))", \
			"revision": "$(call json_quote,$(REVISION))", \
			"board": "$(call json_quote,$(BOARD))" \
		} }' \
	) > $@.metadata.tmp
	fwtool -I $@.metadata.tmp $@
	rm $@.metadata.tmp
endef

define Build/arcadyan-trx
	echo -ne "hsqs" > $@.hsqs
	$(eval trx_magic=$(word 1,$(1)))
	$(STAGING_DIR_HOST)/bin/otrx create $@.trx -M $(trx_magic) -f $@ \
		-a 0x20000 -b 0x420000 -f $@.hsqs -a 1000
	mv $@.trx $@
	dd if=/dev/zero bs=1024 count=1 >> $@.tail
	echo -ne "HDR0" | dd of=$@.tail bs=1 seek=$$((0x10c)) count=4 \
		conv=notrunc 2>/dev/null
	dd if=$@.tail >> $@ 2>/dev/null
	rm $@.hsqs $@.tail
endef

define Build/inteno-y3-header
	BC='$(STAGING_DIR_HOST)/bin/bc' ;\
	ubifsofs="1024" ;\
	ubifs="$$(stat -c%s $@)" ;\
	pkginfoofs="$$(echo $${ubifsofs} + $${ubifs} | $${BC})" ;\
	pkginfo="0" ;\
	scrofs="$$(echo $${pkginfoofs} + $${pkginfo} | $${BC})" ;\
	scr="0" ;\
	sigofs="$$(echo $${scrofs} + $${scr} | $${BC})" ;\
	sig="0" ;\
	md5ofs="$$(echo $${sigofs} + $${sig} | $${BC})" ;\
	md5="32" ;\
	size="$$(echo $${md5ofs} + $${md5} | $${BC})" ;\
	echo "IntenoIopY" > $@.tmp ;\
	echo "version 5" >> $@.tmp ;\
	echo "integrity MD5SUM" >> $@.tmp ;\
	echo "board $(word 1,$(1))" >> $@.tmp ;\
	echo "chip $(patsubst mt%,%,$(SOC:bcm%=%))" >> $@.tmp ;\
	echo "arch all $(CONFIG_TARGET_ARCH_PACKAGES)" >> $@.tmp ;\
	echo "model $(word 1,$(1))" >> $@.tmp ;\
	echo "release $(DEVICE_IMG_PREFIX)" >> $@.tmp ;\
	echo "customer $(if $(CONFIG_VERSION_DIST),$(CONFIG_VERSION_DIST),OpenWrt)" >> $@.tmp ;\
	echo "ubifsofs $${ubifsofs}" >> $@.tmp ;\
	echo "ubifs $${ubifs}" >> $@.tmp ;\
	echo "pkginfoofs $${pkginfoofs}" >> $@.tmp ;\
	echo "pkginfo $${pkginfo}" >> $@.tmp ;\
	echo "scrofs $${scrofs}" >> $@.tmp ;\
	echo "scr $${scr}" >> $@.tmp ;\
	echo "sigofs $${sigofs}" >> $@.tmp ;\
	echo "sig $${sig}" >> $@.tmp ;\
	echo "md5ofs $${md5ofs}" >> $@.tmp ;\
	echo "md5 $${md5}" >> $@.tmp ;\
	echo "size $${size}" >> $@.tmp

	dd if=$@.tmp of=$@.tmp2 bs=1024 count=1 conv=sync
	cat $@.tmp2 $@ > $@.tmp
	rm $@.tmp2
	mv $@.tmp $@
endef

define Build/inteno-bootfs
	rm -rf $@.ubifs-dir
	mkdir -p $@.ubifs-dir/boot

	# populate the boot fs with the dtb and the kernel image
	$(CP) $(KDIR)/image-$(firstword $(DEVICE_DTS)).dtb $@.ubifs-dir/boot/dtb
	$(CP) $@ $@.ubifs-dir/boot/uImage

	# create ubifs
	$(STAGING_DIR_HOST)/bin/mkfs.ubifs ${MKUBIFS_OPTS} -r $@.ubifs-dir/ -o $@.new
	mv $@.new $@
endef

define Build/gemtek-trailer
	printf "%s%08X" ".GEMTEK." "$$(cksum $@ | cut -d ' ' -f1)" >> $@
endef

define Build/h3c-blank-header
	dd if=/dev/zero of=$@.blank bs=160 count=1
	cat $@ >> $@.blank
	mv $@.blank $@
endef

define Build/haier-sim_wr1800k-factory
  mkdir -p "$@.tmp"
  mv "$@" "$@.tmp/UploadBrush-bin.img"
  $(MKHASH) md5 "$@.tmp/UploadBrush-bin.img" | head -c32 > "$@.tmp/check_MD5.txt"
  $(TAR) -czf "$@.tmp.tgz" -C "$@.tmp" UploadBrush-bin.img check_MD5.txt
  $(STAGING_DIR_HOST)/bin/openssl aes-256-cbc -e -salt -in "$@.tmp.tgz" -out "$@" -k QiLunSmartWL
  printf %32s $(DEVICE_MODEL) >> "$@"
  rm -rf "$@.tmp" "$@.tmp.tgz"
endef

define Build/iodata-mstc-header
	( \
		data_size_crc="$$(dd if=$@ ibs=64 skip=1 2>/dev/null | gzip -c | \
			tail -c 8 | od -An -tx8 --endian little | tr -d ' \n')"; \
		echo -ne "$$(echo $$data_size_crc | sed 's/../\\x&/g')" | \
			dd of=$@ bs=8 count=1 seek=7 conv=notrunc 2>/dev/null; \
	)
	dd if=/dev/zero of=$@ bs=4 count=1 seek=1 conv=notrunc 2>/dev/null
	( \
		header_crc="$$(dd if=$@ bs=64 count=1 2>/dev/null | gzip -c | \
			tail -c 8 | od -An -N4 -tx4 --endian little | tr -d ' \n')"; \
		echo -ne "$$(echo $$header_crc | sed 's/../\\x&/g')" | \
			dd of=$@ bs=4 count=1 seek=1 conv=notrunc 2>/dev/null; \
	)
endef

define Build/iodata-mstc-header2
	$(eval model=$(word 1,$(1)))
	$(eval model_id=$(word 2,$(1)))

	( \
		fw_len_crc=$$(gzip -c $@ | tail -c 8 | \
			od -An -tx8 --endian little); \
		printf "\x03\x1d\x61\x29\x07$(model)" | \
			dd bs=21 count=1 conv=sync 2>/dev/null; \
		printf "0.00.000" | dd bs=16 count=1 conv=sync 2>/dev/null; \
		printf "$$(echo $(REVISION) | cut -d- -f1 | head -c8)" | \
			dd bs=9 count=1 conv=sync 2>/dev/null; \
		printf "$(call toupper,$(LINUX_KARCH)) $(VERSION_DIST) Linux-$(LINUX_VERSION)" | \
			dd bs=33 count=1 conv=sync 2>/dev/null; \
		date -d "@$(SOURCE_DATE_EPOCH)" "+%F" | tr -d "\n" | \
			dd bs=15 count=1 conv=sync 2>/dev/null; \
		printf "$$(echo $(model_id) | sed 's/../\\x&/g')" | \
			dd bs=8 count=1 conv=sync 2>/dev/null; \
		printf "$$(echo $$fw_len_crc | sed 's/../\\x&/g')" | \
			dd bs=14 count=1 conv=sync 2>/dev/null; \
		cat $@; \
	) > $@.new
	( \
		header_crc="$$(head -c116 $@.new | gzip -c | tail -c8 | \
			od -An -tx4 -N4 --endian little)"; \
		printf "$$(echo $$header_crc | sed 's/../\\x&/g')"; \
	) | dd of=$@.new bs=4 oflag=seek_bytes seek=110 conv=notrunc

	mv $@.new $@
endef

define Build/znet-header
	$(eval version=$(word 1,$(1)))
	$(eval magic=$(if $(word 2,$(1)),$(word 2,$(1)),ZNET))
	$(eval hdrlen=$(if $(word 3,$(1)),$(word 3,$(1)),0x30))
	( \
		data_size_crc="$$(dd if=$@ 2>/dev/null | gzip -c | \
			tail -c 8 | od -An -N4 -tx4 --endian big | tr -d ' \n')"; \
		payload_len="$$(dd if=$@ bs=4 count=1 skip=1 2>/dev/null | od -An -tdI --endian big | tr -d ' \n')"; \
		payload_size_crc="$$(dd if=$@ ibs=1 count=$$payload_len 2>/dev/null | gzip -c | \
			tail -c 8 | od -An -N4 -tx4 --endian big | tr -d ' \n')"; \
		echo -ne "$(magic)" | dd bs=4 count=1 conv=sync 2>/dev/null; \
		echo -ne "$$(printf '%08x' $$(stat -c%s $@) | fold -s2 | xargs -I {} echo \\x{} | tac | tr -d '\n')" | \
			dd bs=4 count=1 conv=sync 2>/dev/null; \
		echo -ne "$$(echo $$data_size_crc | sed 's/../\\x&/g')" | \
			dd bs=4 count=1 conv=sync 2>/dev/null; \
		echo -ne "$$(echo $$payload_size_crc | sed 's/../\\x&/g')" | \
			dd bs=4 count=1 conv=sync 2>/dev/null; \
		echo -ne "\x12\x34\x56\x78" | dd bs=4 count=1 conv=sync 2>/dev/null; \
		echo -ne "$(version)" | dd bs=28 count=1 conv=sync 2>/dev/null; \
		dd if=/dev/zero bs=$$((0x40000 - $(hdrlen))) count=1 conv=sync 2>/dev/null | tr "\000" "\377"; \
		cat $@; \
	) > $@.new
	mv $@.new $@
endef

define Build/belkin-header
	$(eval magic=$(word 1,$(1)))
	$(eval hw_ver=$(word 2,$(1)))
	$(eval fw_ver=$(word 3,$(1)))

	( \
		type_fw_date=$$(printf "01%02x%02x%02x" \
			$$(date -d "@$(SOURCE_DATE_EPOCH)" "+%-y %-m %-d")); \
		hw_fw_ver=$$(printf "%02x%02x%02x%02x" \
			$(hw_ver) $$(echo $(fw_ver) | cut -d. -f-3 | tr . ' ')); \
		fw_len_crc=$$(gzip -c $@ | tail -c 8 | od -An -tx8 | tr -d ' \n'); \
		fw_crc_len="$${fw_len_crc:8:8}$${fw_len_crc:0:8}"; \
		\
		printf "$(magic)" | dd bs=4 count=1 conv=sync 2>/dev/null; \
		printf "$$(echo $${type_fw_date}$${hw_fw_ver} | \
			sed 's/../\\x&/g')"; \
		printf "$$(echo $${fw_crc_len}$${fw_crc_len} | \
			sed 's/../\\x&/g')"; \
		printf "\x5c\x78\x00\x00"; \
		cat $@; \
	) > $@.new
	mv $@.new $@
endef

define Build/zytrx-header
	$(eval board=$(word 1,$(1)))
	$(eval version=$(word 2,$(1)))
	$(STAGING_DIR_HOST)/bin/zytrx -B '$(board)' -v '$(version)' -i $@ -o $@.new
	mv $@.new $@
endef

define Build/zyxel-nwa-fit
	$(TOPDIR)/scripts/mkits-zyxel-fit.sh \
		$@.its $@ "6b e1 6f e1 ff ff ff ff ff ff"
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $@.its $@.new
	@mv $@.new $@
endef

define Device/dsa-migration
  DEVICE_COMPAT_VERSION := 1.1
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated from swconfig to DSA
endef

define Device/nand
  $(Device/dsa-migration)
  BLOCKSIZE := 128k
  KERNEL_SIZE := 4096k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/adslr_g7
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ADSLR
  DEVICE_MODEL := G7
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += adslr_g7

define Device/afoundry_ew1200
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := AFOUNDRY
  DEVICE_MODEL := EW1200
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mt76x2 kmod-mt7603 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += ew1200
endef
TARGET_DEVICES += afoundry_ew1200

define Device/alfa-network_ax1800rm
  $(Device/dsa-migration)
  IMAGE_SIZE := 15488k
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := AX1800RM
  DEVICE_PACKAGES := kmod-mt7915-firmware
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES += recovery.bin
  IMAGE/recovery.bin := append-kernel | append-rootfs | pad-rootfs | check-size
endef
TARGET_DEVICES += alfa-network_ax1800rm

define Device/alfa-network_quad-e4g
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := Quad-E4G
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-usb3 \
	-wpad-basic-mbedtls
  SUPPORTED_DEVICES += quad-e4g
endef
TARGET_DEVICES += alfa-network_quad-e4g

define Device/ampedwireless_ally_common
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Amped Wireless
  DEVICE_PACKAGES := kmod-mt7615-firmware
  IMAGE_SIZE := 32768k
  KERNEL_INITRAMFS := $$(KERNEL) | edimax-header -s CSYS -m RN68 -f 0x001c0000 -S 0x01100000
endef

define Device/ampedwireless_ally-r1900k
  $(Device/ampedwireless_ally_common)
  DEVICE_MODEL := ALLY-R1900K
  DEVICE_PACKAGES += kmod-usb3
endef
TARGET_DEVICES += ampedwireless_ally-r1900k

define Device/ampedwireless_ally-00x19k
  $(Device/ampedwireless_ally_common)
  DEVICE_MODEL := ALLY-00X19K
endef
TARGET_DEVICES += ampedwireless_ally-00x19k

define Device/arcadyan_we410443
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := WE410443
  IMAGE_SIZE := 32128k
  KERNEL_SIZE := 4352k
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none | arcadyan-trx 0x746f435d
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-rootfs | pad-rootfs | check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += arcadyan_we410443

define Device/arcadyan_we420223-99
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Arcadyan
  DEVICE_MODEL := WE420223-99
  DEVICE_ALT0_VENDOR := KPN
  DEVICE_ALT0_MODEL := Experia WiFi
  IMAGE_SIZE := 32128k
  KERNEL_SIZE := 4352k
  UBINIZE_OPTS := -E 5
  PAGESIZE := 1
  SUBPAGESIZE := 1
  VID_HDR_OFFSET := 64
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none | arcadyan-trx 0x746f435d
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none
  IMAGES += factory.trx
  IMAGE/factory.trx := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | check-size
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += arcadyan_we420223-99

define Device/asiarf_ap7621-001
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := AsiaRF
  DEVICE_MODEL := AP7621-001
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt76x2 kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
endef
TARGET_DEVICES += asiarf_ap7621-001

define Device/asiarf_ap7621-nv1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := AsiaRF
  DEVICE_MODEL := AP7621-NV1
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt76x2 kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
endef
TARGET_DEVICES += asiarf_ap7621-nv1

define Device/asus_rp-ac56
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RP-AC56
  IMAGE_SIZE := 16000k
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 \
	kmod-sound-mt7620 -uboot-envtools
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
        check-size | append-metadata
endef
TARGET_DEVICES += asus_rp-ac56

define Device/asus_rp-ac87
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RP-AC87
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7615-firmware rssileds -uboot-envtools
endef
TARGET_DEVICES += asus_rp-ac87

define Device/asus_rt-ac57u-v1
  $(Device/dsa-migration)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AC57U
  DEVICE_VARIANT := v1
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-AC1200GU
  IMAGE_SIZE := 16064k
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += asus,rt-ac57u
endef
TARGET_DEVICES += asus_rt-ac57u-v1

define Device/asus_rt-ac65p
  $(Device/nand)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AC65P
  IMAGE_SIZE := 51200k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7615-firmware
endef
TARGET_DEVICES += asus_rt-ac65p

define Device/asus_rt-ac85p
  $(Device/nand)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AC85P
  IMAGE_SIZE := 51200k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7615-firmware
endef
TARGET_DEVICES += asus_rt-ac85p

define Device/asus_rt-n56u-b1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-N56U
  DEVICE_VARIANT := B1
  IMAGE_SIZE := 16064k
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += asus_rt-n56u-b1

define Device/asus_rt-ax53u
  $(Device/nand)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AX53U
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-AX1800U
  IMAGE_SIZE := 51200k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += asus_rt-ax53u

define Device/asus_rt-ax54
  $(Device/nand)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AX54
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-AX54HP
  DEVICE_ALT1_VENDOR := ASUS
  DEVICE_ALT1_MODEL := RT-AX1800HP
  DEVICE_ALT2_VENDOR := ASUS
  DEVICE_ALT2_MODEL := RT-AX1800S
  IMAGE_SIZE := 51200k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += asus_rt-ax54

define Device/asus_4g-ax56
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := 4G-AX56
  IMAGE_SIZE := 51200k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3 kmod-usb-serial-option \
	kmod-usb-net-cdc-ncm
endef
TARGET_DEVICES += asus_4g-ax56

define Device/beeline_smartbox-flash
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Beeline
  DEVICE_MODEL := SmartBox Flash
  IMAGE_SIZE := 32768k
  KERNEL_SIZE := 4352k
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none | arcadyan-trx 0x746f435d | pad-to $$(KERNEL_SIZE)
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none
  IMAGES += factory.trx
  IMAGE/factory.trx := append-kernel | append-ubi | check-size
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7615-firmware uencrypt-mbedtls
endef
TARGET_DEVICES += beeline_smartbox-flash

define Device/beeline_smartbox-giga
  $(Device/sercomm_dxx)
  IMAGE_SIZE := 24576k
  SERCOMM_HWID := DBE
  SERCOMM_HWVER := 10100
  SERCOMM_SWVER := 1001
  DEVICE_VENDOR := Beeline
  DEVICE_MODEL := SmartBox GIGA
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb3
endef
TARGET_DEVICES += beeline_smartbox-giga

define Device/beeline_smartbox-pro
  $(Device/sercomm_s1500)
  DEVICE_VENDOR := Beeline
  DEVICE_MODEL := SmartBox PRO
  DEVICE_ALT0_VENDOR := Sercomm
  DEVICE_ALT0_MODEL := S1500 AWI
  IMAGE_SIZE := 34816k
  IMAGE/factory.img := append-kernel | sercomm-kernel-factory | \
	sercomm-reset-slot1-chksum | append-ubi | check-size | \
	sercomm-factory-cqr | sercomm-append-tail | sercomm-mkhash
  SERCOMM_HWID := AWI
  SERCOMM_HWVER := 10000
  SERCOMM_ROOTFS2_OFFSET := 0x3d00000
  SERCOMM_SWVER := 2020
endef
TARGET_DEVICES += beeline_smartbox-pro

define Device/beeline_smartbox-turbo
  $(Device/sercomm_dxx)
  IMAGE_SIZE := 32768k
  SERCOMM_HWID := DF3
  SERCOMM_HWVER := 10200
  SERCOMM_SWVER := 1004
  DEVICE_VENDOR := Beeline
  DEVICE_MODEL := SmartBox TURBO
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3
endef
TARGET_DEVICES += beeline_smartbox-turbo

define Device/beeline_smartbox-turbo-plus
  $(Device/sercomm_cxx)
  IMAGE_SIZE := 32768k
  SERCOMM_HWID := CQR
  SERCOMM_HWVER := 10000
  SERCOMM_SWVER := 2010
  DEVICE_VENDOR := Beeline
  DEVICE_MODEL := SmartBox TURBO+
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3
endef
TARGET_DEVICES += beeline_smartbox-turbo-plus

define Device/belkin_rt1800
  $(Device/nand)
  IMAGE_SIZE := 49152k
  DEVICE_VENDOR := Belkin
  DEVICE_MODEL := RT1800
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | check-size | belkin-header RT18 1 9.9.9
endef
TARGET_DEVICES += belkin_rt1800

define Device/buffalo_wsr-1166dhp
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE/sysupgrade.bin := trx | pad-rootfs | append-metadata
  IMAGE_SIZE := 15936k
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WSR-1166DHP
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 -uboot-envtools
  SUPPORTED_DEVICES += wsr-1166
endef
TARGET_DEVICES += buffalo_wsr-1166dhp

define Device/buffalo_wsr-2533dhpl
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 7936k
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WSR-2533DHPL
  DEVICE_ALT0_VENDOR := Buffalo
  DEVICE_ALT0_MODEL := WSR-2533DHP
  IMAGE/sysupgrade.bin := trx | pad-rootfs | append-metadata
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += buffalo_wsr-2533dhpl

define Device/buffalo_wsr-2533dhplx
  $(Device/dsa-migration)
  DEVICE_VENDOR := Buffalo
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  BUFFALO_TAG_PLATFORM := MTK
  BUFFALO_TAG_VERSION := 9.99
  BUFFALO_TAG_MINOR := 9.99
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  KERNEL_SIZE := 6144k
  IMAGES += factory.bin factory-uboot.bin
  IMAGE/factory.bin = append-ubi | \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $$$$@ $(KDIR)/ubi_mark |\
	buffalo-enc $$(DEVICE_MODEL) 9.99 -l | \
	buffalo-tag-dhp $$(DEVICE_MODEL) JP JP | buffalo-enc-tag -l | \
	buffalo-dhp-image
  IMAGE/factory-uboot.bin := append-ubi | \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $$$$@ $(KDIR)/ubi_mark | append-metadata
  IMAGE/sysupgrade.bin := \
	buffalo-trx $$$$(BUFFALO_TRX_MAGIC) $(KDIR)/tmp/$$(DEVICE_NAME).null | \
	sysupgrade-tar kernel=$$$$@ | append-metadata
endef

define Device/buffalo_wsr-2533dhpl2
  $(Device/buffalo_wsr-2533dhplx)
  DEVICE_MODEL := WSR-2533DHPL2
  BUFFALO_TRX_MAGIC := 0x324c4850
  IMAGE_SIZE := 62592k
endef
TARGET_DEVICES += buffalo_wsr-2533dhpl2

define Device/buffalo_wsr-2533dhpls
  $(Device/buffalo_wsr-2533dhplx)
  DEVICE_MODEL := WSR-2533DHPLS
  BUFFALO_TRX_MAGIC := 0x534c4844
  IMAGE_SIZE := 24576k
endef
TARGET_DEVICES += buffalo_wsr-2533dhpls

define Device/buffalo_wsr-600dhp
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WSR-600DHP
  DEVICE_PACKAGES := kmod-mt7603 kmod-rt2800-pci -uboot-envtools
  SUPPORTED_DEVICES += wsr-600
endef
TARGET_DEVICES += buffalo_wsr-600dhp

define Device/bolt_arion
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := BOLT
  DEVICE_MODEL := Arion
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2
endef
TARGET_DEVICES += bolt_arion

define Device/comfast_cf-e390ax
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := COMFAST
  DEVICE_MODEL := CF-E390AX
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
endef
TARGET_DEVICES += comfast_cf-e390ax

define Device/comfast_cf-ew72-v2
    $(Device/dsa-migration)
    $(Device/uimage-lzma-loader)
    IMAGE_SIZE := 15808k
    DEVICE_VENDOR := COMFAST
    DEVICE_MODEL := CF-EW72 V2
    DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
        -uboot-envtools
    IMAGES += factory.bin
    IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
        check-size | append-metadata
    IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
endef
TARGET_DEVICES += comfast_cf-ew72-v2

define Device/confiabits_mt7621-v1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Confiabits
  DEVICE_MODEL := MT7621
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
endef
TARGET_DEVICES += confiabits_mt7621-v1

define Device/cudy_m1300-v2
  $(Device/dsa-migration)
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := M1300
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  UIMAGE_NAME := R15
  SUPPORTED_DEVICES += R15
endef
TARGET_DEVICES += cudy_m1300-v2

define Device/cudy_m1800
  $(Device/dsa-migration)
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := M1800
  IMAGE_SIZE := 16064k
  UIMAGE_NAME := R17
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += cudy_m1800

define Device/cudy_wr1300-v1
  $(Device/dsa-migration)
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR1300
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += cudy,wr1300 R10
endef
TARGET_DEVICES += cudy_wr1300-v1

define Device/cudy_wr1300-v2
  $(Device/dsa-migration)
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR1300
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  SUPPORTED_DEVICES += cudy,wr1300 R23
endef
TARGET_DEVICES += cudy_wr1300-v2

define Device/cudy_wr1300-v3
  $(Device/dsa-migration)
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR1300
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  SUPPORTED_DEVICES += cudy,wr1300 R30
endef
TARGET_DEVICES += cudy_wr1300-v3

define Device/cudy_wr2100
  $(Device/dsa-migration)
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR2100
  IMAGE_SIZE := 15872k
  UIMAGE_NAME := R11
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += cudy_wr2100

define Device/cudy_r700
  $(Device/dsa-migration)
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := R700
  IMAGE_SIZE := 15872k
  UIMAGE_NAME := R29
  DEVICE_PACKAGES := -uboot-envtools
endef
TARGET_DEVICES += cudy_r700

define Device/cudy_x6-v1
  $(Device/dsa-migration)
  IMAGE_SIZE := 32256k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := X6
  DEVICE_VARIANT := v1
  UIMAGE_NAME := R13
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  SUPPORTED_DEVICES += cudy,x6 R13
endef
TARGET_DEVICES += cudy_x6-v1

define Device/cudy_x6-v2
  $(Device/dsa-migration)
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := X6
  DEVICE_VARIANT := v2
  UIMAGE_NAME := R30
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  SUPPORTED_DEVICES += cudy,x6 R30
endef
TARGET_DEVICES += cudy_x6-v2

define Device/dlink_covr-x1860-a1
  $(Device/dsa-migration)
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 40960k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := COVR-X1860
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES := kmod-mt7915-firmware
  UBINIZE_OPTS := -E 5
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.bin recovery.bin
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  IMAGE/recovery.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size
  IMAGE/factory.bin := $$(IMAGE/recovery.bin) | \
	append-dlink-covr-metadata $$(DEVICE_MODEL) | \
	dlink-sge-image $$(DEVICE_MODEL)
endef
TARGET_DEVICES += dlink_covr-x1860-a1

define Device/dlink_dxx-1xx0-x1
  DEVICE_VENDOR := D-Link
  DEVICE_PACKAGES := kmod-mt7615-firmware rssileds -uboot-envtools
  IMAGE_SIZE := 16064k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | \
    pad-rootfs -x 60 | append-md5sum-ascii-salted ffff | \
    append-string $$$$(DLINK_HWID) | check-size
endef

define Device/dlink_dap-1620-b1
  $(Device/dlink_dxx-1xx0-x1)
  DEVICE_MODEL := DAP-1620
  DEVICE_VARIANT := B1
  DLINK_HWID := MT76XMT7621-RP-PR2475-NA
endef
TARGET_DEVICES += dlink_dap-1620-b1

define Device/dlink_dap-x1860-a1
  $(Device/nand)
  IMAGE_SIZE := 53248k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DAP-X1860
  DEVICE_VARIANT := A1
  KERNEL_SIZE := 8192k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size | elx-header 011b0060 8844A2D168B45A2D
  DEVICE_PACKAGES := kmod-mt7915-firmware rssileds -uboot-envtools
endef
TARGET_DEVICES += dlink_dap-x1860-a1

define Device/dlink_dir-8xx-a1
  $(Device/dsa-migration)
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := D-Link
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  KERNEL := $$(KERNEL) | uimage-sgehdr
  IMAGES += recovery.bin factory.bin
  IMAGE/recovery.bin := append-kernel | append-rootfs | check-size
  IMAGE/factory.bin := $$(IMAGE/recovery.bin) | dlink-sge-image $$$$(DEVICE_MODEL)
endef

define Device/dlink_dir-8xx-r1
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := D-Link
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  KERNEL_INITRAMFS := $$(KERNEL)
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
endef

define Device/dlink_dir_nand_128m
  $(Device/nand)
  IMAGE_SIZE := 40960k
  DEVICE_VENDOR := D-Link
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  KERNEL := $$(KERNEL) | uimage-sgehdr
  IMAGES += recovery.bin
  IMAGE/recovery.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
endef

define Device/dlink_dir-1935-a1
  $(Device/dlink_dir-8xx-a1)
  DEVICE_MODEL := DIR-1935
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES += kmod-usb3
endef
TARGET_DEVICES += dlink_dir-1935-a1

define Device/dlink_dir-1960-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-1960
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-1960-a1

define Device/dlink_dir-2055-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_PACKAGES += -kmod-usb-ledtrig-usbport
  DEVICE_MODEL := DIR-2055
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-2055-a1

define Device/dlink_dir-2150-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-2150
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES += kmod-mt7603 -kmod-usb3 -kmod-usb-ledtrig-usbport
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/recovery.bin) | dlink-sge-image $$(DEVICE_MODEL)
endef
TARGET_DEVICES += dlink_dir-2150-a1

define Device/dlink_dir-2150-r1
  $(Device/nand)
  IMAGE_SIZE := 129536k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-2150
  DEVICE_VARIANT := R1
  DEVICE_PACKAGES :=  -uboot-envtools kmod-mt7603 kmod-mt7615-firmware kmod-usb3
  KERNEL := $$(KERNEL)
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size | sign-dlink-ru e6587b35a6b34e07bedeca23e140322f
endef
TARGET_DEVICES += dlink_dir-2150-r1

define Device/dlink_dir-2640-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-2640
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-2640-a1

define Device/dlink_dir-2660-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-2660
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-2660-a1

define Device/dlink_dir-3040-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-3040
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-3040-a1

define Device/dlink_dir-3060-a1
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-3060
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-3060-a1

define Device/dlink_dir-853-a1
  $(Device/dlink_dir-8xx-a1)
  DEVICE_MODEL := DIR-853
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES += kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += dlink_dir-853-a1

define Device/dlink_dir-853-a3
  $(Device/dlink_dir_nand_128m)
  DEVICE_MODEL := DIR-853
  DEVICE_VARIANT := A3
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/recovery.bin) | dlink-sge-image $$(DEVICE_MODEL)
endef
TARGET_DEVICES += dlink_dir-853-a3

define Device/dlink_dir-853-r1
  $(Device/dlink_dir-8xx-r1)
  DEVICE_MODEL := DIR-853
  DEVICE_VARIANT := R1
  DEVICE_PACKAGES += kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += dlink_dir-853-r1

define Device/dlink_dir-860l-b1
  $(Device/dsa-migration)
  $(Device/seama-lzma-loader)
  SEAMA_SIGNATURE := wrgac13_dlink.2013gui_dir860lb
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-860L
  DEVICE_VARIANT := B1
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
  SUPPORTED_DEVICES += dir-860l-b1
endef
TARGET_DEVICES += dlink_dir-860l-b1

define Device/dlink_dir-867-a1
  $(Device/dlink_dir-8xx-a1)
  DEVICE_MODEL := DIR-867
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-867-a1

define Device/dlink_dir-878-a1
  $(Device/dlink_dir-8xx-a1)
  DEVICE_MODEL := DIR-878
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-878-a1

define Device/dlink_dir-878-r1
  $(Device/dlink_dir-8xx-r1)
  DEVICE_MODEL := DIR-878
  DEVICE_VARIANT := R1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | check-size | \
	sign-dlink-ru 57c5375741c30ca9ebcb36713db4ba51 \
	ab0dff19af8842cdb70a86b4b68d23f7
endef
TARGET_DEVICES += dlink_dir-878-r1

define Device/dlink_dir-882-a1
  $(Device/dlink_dir-8xx-a1)
  DEVICE_MODEL := DIR-882
  DEVICE_VARIANT := A1
  DEVICE_PACKAGES += kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += dlink_dir-882-a1

define Device/dlink_dir-882-r1
  $(Device/dlink_dir-8xx-r1)
  DEVICE_MODEL := DIR-882
  DEVICE_VARIANT := R1
  DEVICE_PACKAGES += kmod-usb3 kmod-usb-ledtrig-usbport
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | check-size | \
	sign-dlink-ru 57c5375741c30ca9ebcb36713db4ba51 \
	ab0dff19af8842cdb70a86b4b68d23f7
endef
TARGET_DEVICES += dlink_dir-882-r1

define Device/dlink_dra-1360-a1
  $(Device/dlink_dxx-1xx0-x1)
  DEVICE_MODEL := DRA-1360
  DEVICE_VARIANT := A1
  DLINK_HWID := MT76XMT7621-RP-RA1360-NA
endef
TARGET_DEVICES += dlink_dra-1360-a1

define Device/dual-q_h721
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Dual-Q
  DEVICE_MODEL := H721
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
endef
TARGET_DEVICES += dual-q_h721

define Device/d-team_newifi-d2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := D-Team
  DEVICE_MODEL := Newifi D2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += d-team_newifi-d2

define Device/d-team_pbr-m1
  $(Device/dsa-migration)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := PandoraBox
  DEVICE_MODEL := PBR-M1
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mt7603 kmod-mt76x2 kmod-mmc-mtk \
	kmod-usb3 kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += pbr-m1
endef
TARGET_DEVICES += d-team_pbr-m1

# Branded version of Genexis / Inteno EX400 (difference is one LED)
define Device/dna_valokuitu-plus-ex400
  $(Device/genexis_pulse-ex400/common)
  DEVICE_VENDOR := DNA
  DEVICE_MODEL := Valokuitu Plus EX400
endef
TARGET_DEVICES += dna_valokuitu-plus-ex400

define Device/edimax_ra21s
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Edimax
  DEVICE_MODEL := RA21S
  DEVICE_ALT0_VENDOR := Edimax
  DEVICE_ALT0_MODEL := Gemini RA21S
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elx-header 02020040 8844A2D168B45A2D
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += edimax_ra21s

define Device/edimax_re23s
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15680k
  DEVICE_VENDOR := Edimax
  DEVICE_MODEL := RE23S
  DEVICE_ALT0_VENDOR := Edimax
  DEVICE_ALT0_MODEL := Gemini RE23S
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	edimax-header -s CSYS -m RN76 -f 0x70000 -S 0x01100000 | pad-rootfs | \
	check-size | append-metadata
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | \
	edimax-header -s CSYS -m RN76 -f 0x70000 -S 0x01100000 | pad-rootfs | \
	check-size
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += edimax_re23s

define Device/edimax_rg21s
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Edimax
  DEVICE_MODEL := Gemini AC2600 RG21S
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elx-header 02020038 8844A2D168B45A2D
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += edimax_rg21s

define Device/elecom_wrc-gs
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ELECOM
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elecom-wrc-gs-factory $$$$(ELECOM_HWNAME) 0.00 -N | \
	append-string MT7621_ELECOM_$$$$(ELECOM_HWNAME)
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef

define Device/elecom_wmc-m1267gst2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 24576k
  DEVICE_MODEL := WMC-M1267GST2
  ELECOM_HWNAME := WMC-DLGST2
endef
TARGET_DEVICES += elecom_wmc-m1267gst2

define Device/elecom_wmc-s1267gs2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 24576k
  DEVICE_MODEL := WMC-S1267GS2
  ELECOM_HWNAME := WMC-DLGST2
endef
TARGET_DEVICES += elecom_wmc-s1267gs2

define Device/elecom_wmc-x1800gst
  $(Device/nand)
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WMC-X1800GST
  KERNEL_SIZE := 15360k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	check-size $$$$(KERNEL_SIZE) | elecom-wrc-gs-factory WMC-2LX 0.00 -N | \
	append-string MT7621_ELECOM_WMC-2LX
endif
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += elecom_wmc-x1800gst

define Device/elecom_wrc-1167ghbk2-s
  $(Device/dsa-migration)
  IMAGE_SIZE := 15488k
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-1167GHBK2-S
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elecom-wrc-gs-factory WRC-1167GHBK2-S 0.00
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += elecom_wrc-1167ghbk2-s

define Device/elecom_wrc-1167gs2-b
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-1167GS2-B
  ELECOM_HWNAME := WRC-1167GS2
endef
TARGET_DEVICES += elecom_wrc-1167gs2-b

define Device/elecom_wrc-1167gst2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 24576k
  DEVICE_MODEL := WRC-1167GST2
  ELECOM_HWNAME := WRC-1167GST2
endef
TARGET_DEVICES += elecom_wrc-1167gst2

define Device/elecom_wrc-1750gs
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-1750GS
  ELECOM_HWNAME := WRC-1750GS
endef
TARGET_DEVICES += elecom_wrc-1750gs

define Device/elecom_wrc-1750gst2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 24576k
  DEVICE_MODEL := WRC-1750GST2
  ELECOM_HWNAME := WRC-1750GST2
endef
TARGET_DEVICES += elecom_wrc-1750gst2

define Device/elecom_wrc-1750gsv
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-1750GSV
  ELECOM_HWNAME := WRC-1750GSV
endef
TARGET_DEVICES += elecom_wrc-1750gsv

define Device/elecom_wrc-1900gst
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-1900GST
  ELECOM_HWNAME := WRC-1900GST
endef
TARGET_DEVICES += elecom_wrc-1900gst

define Device/elecom_wrc-2533ghbk2-t
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-2533GHBK2-T
  IMAGE_SIZE := 7808k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elx-header 0107003b 8844A2D168B45A2D | \
	elecom-product-header WRC-2533GHBK2-T
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += elecom_wrc-2533ghbk2-t

define Device/elecom_wrc-2533ghbk-i
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-2533GHBK-I
  IMAGE_SIZE := 9856k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elx-header 0107002d 8844A2D168B45A2D | \
	elecom-product-header WRC-2533GHBK-I
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += elecom_wrc-2533ghbk-i

define Device/elecom_wrc-2533gs2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-2533GS2
  ELECOM_HWNAME := WRC-2533GS2
endef
TARGET_DEVICES += elecom_wrc-2533gs2

define Device/elecom_wrc-2533gst
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 11264k
  DEVICE_MODEL := WRC-2533GST
  ELECOM_HWNAME := WRC-2533GST
endef
TARGET_DEVICES += elecom_wrc-2533gst

define Device/elecom_wrc-2533gst2
  $(Device/elecom_wrc-gs)
  IMAGE_SIZE := 24576k
  DEVICE_MODEL := WRC-2533GST2
  ELECOM_HWNAME := WRC-2533GST2
endef
TARGET_DEVICES += elecom_wrc-2533gst2

define Device/elecom_wrc-x1800gs
  $(Device/nand)
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-X1800GS
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	znet-header 4.04(XVF.1)b90 COMC 0x68 | elecom-product-header WRC-X1800GS
  KERNEL_INITRAMFS := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 51456k
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	znet-header 4.04(XVF.1)b90 COMC 0x68 | elecom-product-header WRC-X1800GS | \
	check-size
endif
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += elecom_wrc-x1800gs

define Device/elecom_wsc-x1800gs
  $(Device/nand)
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WSC-X1800GS
  KERNEL_SIZE := 15360k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	check-size $$$$(KERNEL_SIZE) | elecom-wrc-gs-factory WMC-2LX 0.00 -N | \
	append-string MT7621_ELECOM_WMC-2LX
endif
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += elecom_wsc-x1800gs

define Device/etisalat_s3
  $(Device/sercomm_dxx)
  IMAGE_SIZE := 32768k
  SERCOMM_HWID := DDK
  SERCOMM_HWVER := 10000
  SERCOMM_SWVER := 4009
  DEVICE_VENDOR := Etisalat
  DEVICE_MODEL := S3
  DEVICE_ALT0_VENDOR := Sercomm
  DEVICE_ALT0_MODEL := S3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3
endef
TARGET_DEVICES += etisalat_s3

define Device/firefly_firewrt
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Firefly
  DEVICE_MODEL := FireWRT
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
  SUPPORTED_DEVICES += firewrt
endef
TARGET_DEVICES += firefly_firewrt

define Device/gehua_ghl-r-001
  $(Device/dsa-migration)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := GeHua
  DEVICE_MODEL := GHL-R-001
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += gehua_ghl-r-001

define Device/gemtek_wvrtm-1xxacn
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 122368k
  DEVICE_VENDOR := Gemtek
  DEVICE_PACKAGES := kmod-gpio-nxp-74hc164 kmod-spi-gpio \
  kmod-usb3 -uboot-envtools 
endef

define Device/gemtek_wvrtm-127acn
  $(Device/gemtek_wvrtm-1xxacn)
  DEVICE_MODEL := WVRTM-127ACN
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt76x2
endef
TARGET_DEVICES += gemtek_wvrtm-127acn

define Device/gemtek_wvrtm-130acn
  $(Device/gemtek_wvrtm-1xxacn)
  DEVICE_MODEL := WVRTM-130ACN
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += gemtek_wvrtm-130acn

# Common definitions shared between genexis_pulse-ex400 and dna_valokuitu-plus-ex400
define Device/genexis_pulse-ex400/common
  $(Device/dsa-migration)
  IMAGE_SIZE := 117m
  PAGESIZE := 2048
  MKUBIFS_OPTS := --min-io-size=$$(PAGESIZE) --leb-size=124KiB --max-leb-cnt=96 \
    --log-lebs=2 --space-fixup --squash-uids
  KERNEL := kernel-bin | lzma | uImage lzma
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  IMAGES += factory.bin
  IMAGE/factory.bin := append-image-stage initramfs-kernel.bin | \
	inteno-bootfs | inteno-y3-header EX400 | append-md5sum-ascii-salted
endif
  IMAGE/sysupgrade.bin := append-kernel | inteno-bootfs | pad-to 10M | \
    sysupgrade-tar kernel=$$$$@ | check-size | append-metadata
  DEVICE_IMG_NAME = $$(DEVICE_IMG_PREFIX)-$$(2)
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3 kmod-keyboard-sx951x kmod-button-hotplug
endef

define Device/genexis_pulse-ex400
  $(Device/genexis_pulse-ex400/common)
  DEVICE_VENDOR := Genexis
  DEVICE_MODEL := Pulse EX400
  DEVICE_ALT0_VENDOR := Inteno
  DEVICE_ALT0_MODEL := Pulse EX400
endef
TARGET_DEVICES += genexis_pulse-ex400

define Device/glinet_gl-mt1300
  $(Device/dsa-migration)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT1300
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += glinet_gl-mt1300

define Device/gnubee_gb-pc1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := GnuBee
  DEVICE_MODEL := GB-PC1
  DEVICE_PACKAGES := kmod-ata-ahci kmod-usb3 kmod-mmc-mtk \
	-wpad-basic-mbedtls -uboot-envtools
  IMAGE_SIZE := 32448k
endef
TARGET_DEVICES += gnubee_gb-pc1

define Device/gnubee_gb-pc2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := GnuBee
  DEVICE_MODEL := GB-PC2
  DEVICE_PACKAGES := kmod-ata-ahci kmod-usb3 kmod-mmc-mtk \
	-wpad-basic-mbedtls -uboot-envtools
  IMAGE_SIZE := 32448k
endef
TARGET_DEVICES += gnubee_gb-pc2

define Device/hanyang_hyc-g920
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Hanyang
  DEVICE_MODEL := CJ-Hello HYC-G920
  IMAGE_SIZE := 15744k
  DEVICE_PACKAGES := kmod-usb3 kmod-mt76x2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += hanyang_hyc-g920

define Device/h3c_tx180x
  $(Device/nand)
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 120832k
  KERNEL_LOADADDR := 0x82000000
  KERNEL_INITRAMFS := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL := $$(KERNEL_INITRAMFS) | h3c-blank-header
  DEVICE_VENDOR := H3C
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef

define Device/h3c_tx1800-plus
  $(Device/h3c_tx180x)
  DEVICE_MODEL := TX1800 Plus
endef
TARGET_DEVICES += h3c_tx1800-plus

define Device/h3c_tx1801-plus
  $(Device/h3c_tx180x)
  DEVICE_MODEL := TX1801 Plus
endef
TARGET_DEVICES += h3c_tx1801-plus

define Device/h3c_tx1806
  $(Device/h3c_tx180x)
  DEVICE_MODEL := TX1806
endef
TARGET_DEVICES += h3c_tx1806

define Device/haier-sim_wr1800k
  $(Device/nand)
  IMAGE_SIZE := 125440k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	haier-sim_wr1800k-factory
endif
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef

define Device/haier_har-20s2u1
  $(Device/haier-sim_wr1800k)
  DEVICE_VENDOR := Haier
  DEVICE_MODEL := HAR-20S2U1
endef
TARGET_DEVICES += haier_har-20s2u1

define Device/hilink_hlk-7621a-evb
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := HiLink
  DEVICE_MODEL := HLK-7621A evaluation board
  DEVICE_PACKAGES += kmod-mt76x2 kmod-usb3 -uboot-envtools
  IMAGE_SIZE := 32448k
endef
TARGET_DEVICES += hilink_hlk-7621a-evb

define Device/hiwifi_hc5962
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32768k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_VENDOR := HiWiFi
  DEVICE_MODEL := HC5962
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += hiwifi_hc5962

define Device/humax_e10
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15936k
  DEVICE_VENDOR := HUMAX
  DEVICE_MODEL := E10
  DEVICE_ALT0_VENDOR := HUMAX
  DEVICE_ALT0_MODEL := QUANTUM E10
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | \
	edimax-header -s CSYS -m EA03 -f 0x70000 -S 0x01100000 | pad-rootfs | \
	check-size | append-metadata
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | \
	edimax-header -s CSYS -m EA03 -f 0x70000 -S 0x01100000 | \
	check-size | zip upg -P f013c26cf0a320fb71d03356dcb6bb63
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += humax_e10

define Device/huasifei_ws1208v2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Huasifei
  DEVICE_MODEL := WS1208V2
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mt7603 kmod-mt76x2 kmod-mmc-mtk \
	kmod-usb3 kmod-usb-net-cdc-mbim kmod-usb-net-qmi-wwan \
	kmod-usb-serial-option -uboot-envtools
endef
TARGET_DEVICES += huasifei_ws1208v2

define Device/iodata_wn-ax1167gr
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15552k
  DEVICE_VENDOR := I-O DATA
  DEVICE_MODEL := WN-AX1167GR
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	check-size 7680k | senao-header -r 0x30a -p 0x1055 -t 4
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-ax1167gr

define Device/iodata_nand
  $(Device/nand)
  DEVICE_VENDOR := I-O DATA
  IMAGE_SIZE := 51200k
  LOADER_TYPE := bin
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | lzma | uImage lzma
endef

# The OEM webinterface expects an kernel with initramfs which has the uImage
# header field ih_name.
# We don't want to set the header name field for the kernel include in the
# sysupgrade image as well, as this image shouldn't be accepted by the OEM
# webinterface. It will soft-brick the board.

define Device/iodata_wn-ax1167gr2
  $(Device/iodata_nand)
  DEVICE_MODEL := WN-AX1167GR2
  KERNEL_INITRAMFS := $(KERNEL_DTB) | loader-kernel | lzma | \
	uImage lzma -M 0x434f4d42 -n '3.10(XBC.1)b10' | iodata-mstc-header
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-ax1167gr2

define Device/iodata_wn-ax2033gr
  $(Device/iodata_nand)
  DEVICE_MODEL := WN-AX2033GR
  KERNEL_INITRAMFS := $(KERNEL_DTB) | loader-kernel | lzma | \
	uImage lzma -M 0x434f4d42 -n '3.10(VST.1)C10' | iodata-mstc-header
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-ax2033gr

define Device/iodata_wn-deax1800gr
  $(Device/dsa-migration)
  DEVICE_VENDOR := I-O DATA
  DEVICE_MODEL := WN-DEAX1800GR
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  KERNEL_SIZE := 6144k
  IMAGE_SIZE := 47104k
  UBINIZE_OPTS := -E 5
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	check-size | xor-image -p 29944a25120984c2 -x | \
	iodata-mstc-header2 WN-DEAX1800GR 00021003
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += iodata_wn-deax1800gr

define Device/iodata_wn-dx1167r
  $(Device/iodata_nand)
  DEVICE_MODEL := WN-DX1167R
  KERNEL_INITRAMFS := $(KERNEL_DTB) | loader-kernel | lzma | \
	uImage lzma -M 0x434f4d43 -n '3.10(XIK.1)b10' | iodata-mstc-header
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-dx1167r

define Device/iodata_wn-dx1200gr
  $(Device/iodata_nand)
  DEVICE_MODEL := WN-DX1200GR
  KERNEL_INITRAMFS := $(KERNEL_DTB) | loader-kernel | lzma | \
	uImage lzma -M 0x434f4d43 -n '3.10(XIQ.0)b20' | iodata-mstc-header
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
endef
TARGET_DEVICES += iodata_wn-dx1200gr

define Device/iodata_wn-dx2033gr
  $(Device/iodata_nand)
  DEVICE_MODEL := WN-DX2033GR
  KERNEL_INITRAMFS := $(KERNEL_DTB) | loader-kernel | lzma | \
	uImage lzma -M 0x434f4d42 -n '3.10(XID.0)b30' | iodata-mstc-header
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-dx2033gr

define Device/iodata_wn-gx300gr
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 7616k
  DEVICE_VENDOR := I-O DATA
  DEVICE_MODEL := WN-GX300GR
  DEVICE_PACKAGES := kmod-mt7603 -uboot-envtools
endef
TARGET_DEVICES += iodata_wn-gx300gr

define Device/iodata_wnpr2600g
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := I-O DATA
  DEVICE_MODEL := WNPR2600G
  IMAGE_SIZE := 13952k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | \
	elx-header 0104003a 8844A2D168B45A2D
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iodata_wnpr2600g

define Device/iptime_a3002mesh
  $(Device/dsa-migration)
  IMAGE_SIZE := 16128k
  UIMAGE_NAME := a3002me
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A3002MESH
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += iptime_a3002mesh

define Device/iptime_a3004ns-dual
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16128k
  UIMAGE_NAME := a3004nd
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A3004NS-dual
  DEVICE_PACKAGES := kmod-usb3 kmod-mt76x2 kmod-usb-ledtrig-usbport \
	-uboot-envtools
endef
TARGET_DEVICES += iptime_a3004ns-dual

define Device/iptime_a3004t
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  FILESYSTEMS := squashfs
  IMAGE_SIZE := 129280k
  UIMAGE_NAME := a3004t
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A3004T
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += iptime_a3004t

define Device/iptime_a6004ns-m
  $(Device/dsa-migration)
  IMAGE_SIZE := 16128k
  UIMAGE_NAME := a6004nm
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A6004NS-M
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += iptime_a6004ns-m

define Device/iptime_a6ns-m
  $(Device/dsa-migration)
  IMAGE_SIZE := 16128k
  UIMAGE_NAME := a6nm
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A6ns-M
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += iptime_a6ns-m

define Device/iptime_a8004t
  $(Device/dsa-migration)
  IMAGE_SIZE := 16128k
  UIMAGE_NAME := a8004t
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A8004T
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += iptime_a8004t

define Device/iptime_ax2004m
  $(Device/nand)
  IMAGE_SIZE := 121344k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  ARTIFACTS := initramfs-factory.bin
  ARTIFACT/initramfs-factory.bin := append-image-stage initramfs-kernel.bin | \
	check-size | iptime-crc32 ax2004m
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := AX2004M
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-usb3
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE := NMBM has been newly enabled, and its flash block \
	mapping might be incompatible with existing installation. \
	New installation with factory image via recovery mode is recommended.
endef
TARGET_DEVICES += iptime_ax2004m

define Device/iptime_t5004
  $(Device/nand)
  IMAGE_SIZE := 129280k
  UIMAGE_NAME := t5004
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := T5004
  DEVICE_PACKAGES := -wpad-basic-mbedtls
endef
TARGET_DEVICES += iptime_t5004

define Device/jcg_jhr-ac876m
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | jcg-header 89.1
  JCG_MAXSIZE := 16064k
  DEVICE_VENDOR := JCG
  DEVICE_MODEL := JHR-AC876M
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += jcg_jhr-ac876m

define Device/jcg_q20
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 91136k
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_VENDOR := JCG
  DEVICE_MODEL := Q20
  DEVICE_PACKAGES := kmod-mt7915-firmware
endef
TARGET_DEVICES += jcg_q20

define Device/jcg_y2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | jcg-header 95.1
  JCG_MAXSIZE := 16064k
  DEVICE_VENDOR := JCG
  DEVICE_MODEL := Y2
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += jcg_y2

define Device/jdcloud_re-cp-02
  $(Device/dsa-migration)
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := JD-Cloud
  DEVICE_MODEL := RE-CP-02
  DEVICE_PACKAGES := kmod-mt7915-firmware kmod-mmc-mtk
endef
TARGET_DEVICES += jdcloud_re-cp-02

define Device/jdcloud_re-sp-01b
  $(Device/dsa-migration)
  IMAGE_SIZE := 27328k
  DEVICE_VENDOR := JDCloud
  DEVICE_MODEL := RE-SP-01B
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware \
	kmod-mmc-mtk kmod-usb3
endef
TARGET_DEVICES += jdcloud_re-sp-01b

define Device/keenetic_kn-1910
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 29097984
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1910
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | zyimage -d 0x801910 -v "KN-1910"
endef
TARGET_DEVICES += keenetic_kn-1910

define Device/keenetic_kn-3010
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 31488k
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3010
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 0x803010 -v "KN-3010"
endef
TARGET_DEVICES += keenetic_kn-3010

define Device/keenetic_kn-3510
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 121088k
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3510
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size | zyimage -d 0x803510 -v "KN-3510"
endef
TARGET_DEVICES += keenetic_kn-3510

define Device/lenovo_newifi-d1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Lenovo
  DEVICE_MODEL := Newifi D1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 kmod-mmc-mtk \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += newifi-d1
endef
TARGET_DEVICES += lenovo_newifi-d1

define Device/linksys_e5600
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 26624k
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E5600
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | check-size | gemtek-trailer
endef
TARGET_DEVICES += linksys_e5600

define Device/linksys_e7350
  $(Device/belkin_rt1800)
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E7350
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | check-size | belkin-header GOLF 1 9.9.9
endef
TARGET_DEVICES += linksys_e7350

define Device/linksys_ea7xxx
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 36864k
  DEVICE_VENDOR := Linksys
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7615-firmware
  IMAGES := sysupgrade.bin factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | \
	append-ubi | check-size | linksys-image type=$$$$(LINKSYS_HWNAME)
endef

define Device/linksys_ea6350-v4
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA6350
  DEVICE_VARIANT := v4
  LINKSYS_HWNAME := EA6350
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7663-firmware-ap
endef
TARGET_DEVICES += linksys_ea6350-v4

define Device/linksys_ea7300-v1
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA7300
  DEVICE_VARIANT := v1
  LINKSYS_HWNAME := EA7300
endef
TARGET_DEVICES += linksys_ea7300-v1

define Device/linksys_ea7300-v2
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA7300
  DEVICE_VARIANT := v2
  LINKSYS_HWNAME := EA7300v2
  DEVICE_PACKAGES += kmod-mt7603
endef
TARGET_DEVICES += linksys_ea7300-v2

define Device/linksys_ea7500-v2
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA7500
  DEVICE_VARIANT := v2
  LINKSYS_HWNAME := EA7500v2
endef
TARGET_DEVICES += linksys_ea7500-v2

define Device/linksys_ea8100-v1
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA8100
  DEVICE_VARIANT := v1
  LINKSYS_HWNAME := EA8100
endef
TARGET_DEVICES += linksys_ea8100-v1

define Device/linksys_ea8100-v2
  $(Device/linksys_ea7xxx)
  DEVICE_MODEL := EA8100
  DEVICE_VARIANT := v2
  LINKSYS_HWNAME := EA8100v2
endef
TARGET_DEVICES += linksys_ea8100-v2

define Device/linksys_re6500
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := RE6500
  DEVICE_PACKAGES := kmod-mt76x2 -uboot-envtools
  SUPPORTED_DEVICES += re6500
endef
TARGET_DEVICES += linksys_re6500

define Device/linksys_re7000
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := RE7000
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware
endef
TARGET_DEVICES += linksys_re7000

define Device/maginon_mc-1200ac
  $(Device/dsa-migration)
  DEVICE_VENDOR := Maginon
  DEVICE_MODEL := MC-1200AC
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap kmod-usb3 -uboot-envtools
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs | check-size | append-metadata
  IMAGE_SIZE := 15552k
endef
TARGET_DEVICES += maginon_mc-1200ac

define Device/mediatek_ap-mt7621a-v60
  $(Device/dsa-migration)
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Mediatek
  DEVICE_MODEL := AP-MT7621A-V60 EVB
  DEVICE_PACKAGES := kmod-usb3 kmod-mmc-mtk kmod-sound-mt7620 \
	-wpad-basic-mbedtls -uboot-envtools
endef
TARGET_DEVICES += mediatek_ap-mt7621a-v60

define Device/mediatek_mt7621-eval-board
  $(Device/dsa-migration)
  IMAGE_SIZE := 15104k
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7621 EVB
  DEVICE_PACKAGES := -wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += mt7621
endef
TARGET_DEVICES += mediatek_mt7621-eval-board

define Device/meig_slt866
  $(Device/dsa-migration)
  IMAGE_SIZE := 15104k
  DEVICE_VENDOR := MeiG
  DEVICE_MODEL := SLT866
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb-net-qmi-wwan kmod-usb-serial-option kmod-usb3 \
	kmod-usb-net-rndis
endef
TARGET_DEVICES += meig_slt866

define Device/mercusys_mr70x-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MR70X
  DEVICE_ALT0_VENDOR := MERCUSYS
  DEVICE_ALT0_MODEL := MR1800X
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := MR70X
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += mercusys_mr70x-v1

define Device/MikroTik
  $(Device/dsa-migration)
  DEVICE_VENDOR := MikroTik
  IMAGE_SIZE := 16128k
  DEVICE_PACKAGES := kmod-usb3 -uboot-envtools
  KERNEL_NAME := vmlinuz
  KERNEL := kernel-bin | append-dtb-elf
  IMAGE/sysupgrade.bin := append-kernel | yaffs-filesystem -L | \
	pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size | \
	append-metadata
endef

define Device/mikrotik_ltap-2hnd
  $(Device/MikroTik)
  DEVICE_MODEL := LtAP-2HnD
  DEVICE_PACKAGES += kmod-ath9k kmod-pps-gpio rssileds
endef
TARGET_DEVICES += mikrotik_ltap-2hnd

define Device/mikrotik_routerboard-750gr3
  $(Device/MikroTik)
  DEVICE_MODEL := RouterBOARD 750Gr3
  DEVICE_PACKAGES += -wpad-basic-mbedtls
  SUPPORTED_DEVICES += mikrotik,rb750gr3
endef
TARGET_DEVICES += mikrotik_routerboard-750gr3

define Device/mikrotik_routerboard-760igs
  $(Device/MikroTik)
  DEVICE_MODEL := RouterBOARD 760iGS
  DEVICE_PACKAGES += kmod-sfp -wpad-basic-mbedtls
endef
TARGET_DEVICES += mikrotik_routerboard-760igs

define Device/mikrotik_routerboard-m11g
  $(Device/MikroTik)
  DEVICE_MODEL := RouterBOARD M11G
  DEVICE_PACKAGES := -wpad-basic-mbedtls
  SUPPORTED_DEVICES += mikrotik,rbm11g
endef
TARGET_DEVICES += mikrotik_routerboard-m11g

define Device/mikrotik_routerboard-m33g
  $(Device/MikroTik)
  DEVICE_MODEL := RouterBOARD M33G
  DEVICE_PACKAGES := -wpad-basic-mbedtls
  SUPPORTED_DEVICES += mikrotik,rbm33g
endef
TARGET_DEVICES += mikrotik_routerboard-m33g

define Device/mofinetwork_mofi5500-5gxelte
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 27656k
  DEVICE_VENDOR := MoFi Network
  DEVICE_MODEL := MOFI5500-5GXeLTE
  DEVICE_PACKAGES := kmod-usb3 kmod-mmc-mtk kmod-mt7615-firmware \
	kmod-usb-net-qmi-wwan kmod-usb-net-cdc-mbim
  SUPPORTED_DEVICES += mofi5500 # Needed in order to flash through Mofi stock firmware
endef
TARGET_DEVICES += mofinetwork_mofi5500-5gxelte

define Device/mqmaker_witi
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := MQmaker
  DEVICE_MODEL := WiTi
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mt76x2 kmod-mmc-mtk kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += witi mqmaker,witi-256m mqmaker,witi-512m
endef
TARGET_DEVICES += mqmaker_witi

define Device/mtc_wr1201
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := MTC
  DEVICE_MODEL := Wireless Router WR1201
  KERNEL_INITRAMFS := $(KERNEL_DTB) | uImage lzma -n 'WR1201_8_128'
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += mtc_wr1201

define Device/mts_wg430223
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := MTS
  DEVICE_MODEL := WG430223
  IMAGE_SIZE := 32768k
  KERNEL_SIZE := 4352k
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none | arcadyan-trx 0x53485231 | pad-to $$(KERNEL_SIZE)
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none
  IMAGES += factory.trx
  IMAGE/factory.trx := append-kernel | append-ubi | check-size
  DEVICE_PACKAGES := kmod-mt7615-firmware uencrypt-mbedtls
endef
TARGET_DEVICES += mts_wg430223

define Device/netgear_eax12
  $(Device/nand)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := EAX12
  DEVICE_ALT0_VENDOR := NETGEAR
  DEVICE_ALT0_MODEL := EAX11
  DEVICE_ALT0_VARIANT := v2
  DEVICE_ALT1_VENDOR := NETGEAR
  DEVICE_ALT1_MODEL := EAX15
  DEVICE_ALT1_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  NETGEAR_ENC_MODEL := EAX12
  NETGEAR_ENC_REGION := US
  NETGEAR_ENC_HW_ID_LIST := 1010000004540000_NETGEAR
  NETGEAR_ENC_MODEL_LIST := EAX12;EAX11v2;EAX15v2
  IMAGE_SIZE := 57344k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_eax12

define Device/netgear_ex6150
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := EX6150
  DEVICE_PACKAGES := kmod-mt76x2 -uboot-envtools
  NETGEAR_BOARD_ID := U12H318T00_NETGEAR
  IMAGE_SIZE := 14848k
  IMAGES += factory.chk
  IMAGE/factory.chk := $$(sysupgrade_bin) | check-size | netgear-chk
endef
TARGET_DEVICES += netgear_ex6150

define Device/netgear_sercomm_nand
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGES += factory.img kernel.bin rootfs.bin
  IMAGE/factory.img := pad-extra 2048k | append-kernel | pad-to 6144k | \
	append-ubi | pad-to $$$$(BLOCKSIZE) | sercom-footer | pad-to 128 | \
	zip $$$$(SERCOMM_HWNAME).bin | sercom-seal
  IMAGE/kernel.bin := append-kernel
  IMAGE/rootfs.bin := append-ubi | check-size
  DEVICE_VENDOR := NETGEAR
  DEVICE_PACKAGES := kmod-mt7603 kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
endef

define Device/netgear_r6220
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6220
  SERCOMM_HWNAME := R6220
  SERCOMM_HWID := AYA
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0086
  IMAGE_SIZE := 28672k
  DEVICE_PACKAGES += kmod-mt76x2
  SUPPORTED_DEVICES += r6220
endef
TARGET_DEVICES += netgear_r6220


define Device/netgear_r6260
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6260
  SERCOMM_HWNAME := R6260
  SERCOMM_HWID := CHJ
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0052
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6260

define Device/netgear_r6350
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6350
  SERCOMM_HWNAME := R6350
  SERCOMM_HWID := CHJ
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0052
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6350

define Device/netgear_r6700-v2
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6700
  DEVICE_VARIANT := v2
  DEVICE_ALT0_VENDOR := NETGEAR
  DEVICE_ALT0_MODEL := Nighthawk AC2400
  DEVICE_ALT0_VARIANT := v1
  DEVICE_ALT1_VENDOR := NETGEAR
  DEVICE_ALT1_MODEL := Nighthawk AC2100
  DEVICE_ALT1_VARIANT := v1
  SERCOMM_HWNAME := R6950
  SERCOMM_HWID := BZV
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x1032
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6700-v2

define Device/netgear_r6800
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6800
  SERCOMM_HWNAME := R6950
  SERCOMM_HWID := BZV
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0062
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6800

define Device/netgear_r6850
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6850
  SERCOMM_HWNAME := R6850
  SERCOMM_HWID := CHJ
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0052
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6850

define Device/netgear_r6900-v2
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R6900
  DEVICE_VARIANT := v2
  SERCOMM_HWNAME := R6950
  SERCOMM_HWID := BZV
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x1032
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r6900-v2

define Device/netgear_r7200
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R7200
  SERCOMM_HWNAME := R6950
  SERCOMM_HWID := BZV
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x1032
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r7200

define Device/netgear_r7450
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := R7450
  SERCOMM_HWNAME := R6950
  SERCOMM_HWID := BZV
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x1032
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_r7450

define Device/netgear_wac104
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := WAC104
  SERCOMM_HWNAME := WAC104
  SERCOMM_HWID := CAY
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0006
  IMAGE_SIZE := 28672k
  DEVICE_PACKAGES += kmod-mt76x2
endef
TARGET_DEVICES += netgear_wac104

define Device/netgear_wac124
  $(Device/netgear_sercomm_nand)
  DEVICE_MODEL := WAC124
  SERCOMM_HWNAME := WAC124
  SERCOMM_HWID := CTL
  SERCOMM_HWVER := A003
  SERCOMM_SWVER := 0x0402
  IMAGE_SIZE := 40960k
  DEVICE_PACKAGES += kmod-mt7615-firmware
endef
TARGET_DEVICES += netgear_wac124

define Device/netgear_wax202
  $(Device/nand)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WAX202
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  NETGEAR_ENC_MODEL := WAX202
  NETGEAR_ENC_REGION := US
  IMAGE_SIZE := 38912k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_wax202

define Device/netgear_wax214v2
  $(Device/nand)
  DEVICE_VENDOR := NETGEAR
  DEVICE_MODEL := WAX214v2
  DEVICE_PACKAGES := kmod-mt7915-firmware
  NETGEAR_ENC_MODEL := WAX214v2
  NETGEAR_ENC_REGION := US
  IMAGE_SIZE := 38912k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel 0x80001000 | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | \
	append-squashfs4-fakeroot
  IMAGES += factory.img
  IMAGE/factory.img := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | netgear-encrypted-factory
endef
TARGET_DEVICES += netgear_wax214v2

define Device/netgear_wndr3700-v5
  $(Device/dsa-migration)
  $(Device/netgear_sercomm_nor)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15232k
  DEVICE_MODEL := WNDR3700
  DEVICE_VARIANT := v5
  SERCOMM_HWNAME := WNDR3700v5
  SERCOMM_HWID := AYB
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x1054
  SERCOMM_PAD := 320k
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += wndr3700v5
endef
TARGET_DEVICES += netgear_wndr3700-v5

define Device/netis_n6
  $(Device/dsa-migration)
  $(Device/nand)
  IMAGE_SIZE := 121344k
  DEVICE_VENDOR := netis
  DEVICE_MODEL := N6
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size | append-netis-n6-metadata
  DEVICE_PACKAGES += kmod-mt7915-firmware kmod-usb-ledtrig-usbport \
	kmod-usb3
endef
TARGET_DEVICES += netis_n6

define Device/netis_wf2881
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  FILESYSTEMS := squashfs
  IMAGE_SIZE := 129280k
  UIMAGE_NAME := WF2881_0.0.00
  KERNEL_INITRAMFS := $$(KERNEL) | netis-tail WF2881
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_VENDOR := NETIS
  DEVICE_MODEL := WF2881
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
endef
TARGET_DEVICES += netis_wf2881

define Device/openfi_5pro
  $(Device/dsa-migration)
  IMAGE_SIZE := 65216k
  DEVICE_VENDOR := OpenFi
  DEVICE_MODEL := 5Pro
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap kmod-usb3 \
	kmod-mmc-mtk
endef
TARGET_DEVICES += openfi_5pro

define Device/oraybox_x3a
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15360k
  DEVICE_VENDOR := OrayBox
  DEVICE_MODEL := X3A
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += oraybox_x3a

define Device/phicomm_k2p
  $(Device/dsa-migration)
  IMAGE_SIZE := 15744k
  DEVICE_VENDOR := Phicomm
  DEVICE_MODEL := K2P
  DEVICE_ALT0_VENDOR := Phicomm
  DEVICE_ALT0_MODEL := KE 2P
  SUPPORTED_DEVICES += k2p
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += phicomm_k2p

define Device/planex_vr500
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 65216k
  DEVICE_VENDOR := Planex
  DEVICE_MODEL := VR500
  DEVICE_PACKAGES := kmod-usb3 -wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += vr500
endef
TARGET_DEVICES += planex_vr500

define Device/raisecom_msg1500-x-00
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 129280k
  DEVICE_VENDOR := RAISECOM
  DEVICE_MODEL := MSG1500
  DEVICE_VARIANT := X.00
  DEVICE_ALT0_VENDOR := Nokia
  DEVICE_ALT0_MODEL := A-040W-Q
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += raisecom_msg1500-x-00

define Device/renkforce_ws-wn530hp3-a
  $(Device/dsa-migration)
  DEVICE_VENDOR := Renkforce
  DEVICE_MODEL := WS-WN530HP3-A
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  IMAGE/sysupgrade.bin := append-kernel | pad-to 65536 | append-rootfs | \
	check-size | append-metadata
  IMAGE_SIZE := 15040k
endef
TARGET_DEVICES += renkforce_ws-wn530hp3-a

define Device/rostelecom_rt-fe-1a
  $(Device/sercomm_dxx)
  IMAGE_SIZE := 24576k
  SERCOMM_HWID := CX4
  SERCOMM_HWVER := 11300
  SERCOMM_SWVER := 2010
  DEVICE_VENDOR := Rostelecom
  DEVICE_MODEL := RT-FE-1A
  DEVICE_ALT0_VENDOR := Sercomm
  DEVICE_ALT0_MODEL := RT-FE-1A
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware
endef
TARGET_DEVICES += rostelecom_rt-fe-1a

define Device/rostelecom_rt-sf-1
  $(Device/sercomm_dxx)
  IMAGE_SIZE := 32768k
  SERCOMM_HWID := DKG
  SERCOMM_HWVER := 10110
  SERCOMM_SWVER := 1026
  DEVICE_VENDOR := Rostelecom
  DEVICE_MODEL := RT-SF-1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3
endef
TARGET_DEVICES += rostelecom_rt-sf-1

define Device/ruijie_rg-ew1200g-pro-v1.1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := Ruijie
  DEVICE_MODEL := RG-EW1200G PRO
  DEVICE_VARIANT := v1.1
  DEVICE_PACKAGES := kmod-mt7615-firmware
endef
TARGET_DEVICES += ruijie_rg-ew1200g-pro-v1.1

define Device/samknows_whitebox-v8
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := SamKnows
  DEVICE_MODEL := Whitebox 8
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += sk-wb8
endef
TARGET_DEVICES += samknows_whitebox-v8

define Device/sercomm_na502
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 20480k
  DEVICE_VENDOR := SERCOMM
  DEVICE_MODEL := NA502
  DEVICE_PACKAGES := kmod-mt76x2 kmod-mt7603 kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += sercomm_na502

define Device/sercomm_na502s
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 20971520
  DEVICE_VENDOR := SERCOMM
  DEVICE_MODEL := NA502S
  DEVICE_PACKAGES := kmod-mt76x2 kmod-mt7603 kmod-usb3 kmod-usb-serial \
		kmod-usb-serial-xr -uboot-envtools
endef
TARGET_DEVICES += sercomm_na502s

define Device/sim_simax1800t
  $(Device/haier-sim_wr1800k)
  DEVICE_VENDOR := SIM
  DEVICE_MODEL := SIMAX1800T
endef
TARGET_DEVICES += sim_simax1800t

define Device/snr_snr-cpe-me1
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15040k
  DEVICE_VENDOR := SNR
  DEVICE_MODEL := SNR-CPE-ME1
  UIMAGE_NAME := SNR-CPE-ME1-5GHZ-MT
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x0e kmod-usb3 \
	kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += snr_snr-cpe-me1

define Device/snr_snr-cpe-me2-lite
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := SNR
  DEVICE_MODEL := SNR-CPE-ME2-Lite
  UIMAGE_NAME := $$(DEVICE_MODEL)
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
endef
TARGET_DEVICES += snr_snr-cpe-me2-lite

define Device/snr_snr-cpe-me2-sfp
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := SNR
  DEVICE_MODEL := SNR-CPE-ME2-SFP
  UIMAGE_NAME := $$(DEVICE_MODEL)
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 kmod-sfp \
	kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += snr_snr-cpe-me2-sfp

define Device/storylink_sap-g3200u3
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 7872k
  UIMAGE_NAME := SAP-G3200
  DEVICE_VENDOR := STORYLiNK
  DEVICE_MODEL := SAP-G3200U3
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
  SUPPORTED_DEVICES += sap-g3200u3
endef
TARGET_DEVICES += storylink_sap-g3200u3

define Device/telco-electronics_x1
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Telco Electronics
  DEVICE_MODEL := X1
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603 kmod-mt76x2 -uboot-envtools
endef
TARGET_DEVICES += telco-electronics_x1

define Device/tenbay_t-mb5eu-v01
  $(Device/dsa-migration)
  DEVICE_VENDOR := Tenbay
  DEVICE_MODEL := T-MB5EU-V01
  DEVICE_DTS_CONFIG := config@1
  DEVICE_PACKAGES += kmod-mt7915-firmware kmod-usb3 -uboot-envtools
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 15808k
  SUPPORTED_DEVICES += mt7621-dm2-t-mb5eu-v01-nor
endef
TARGET_DEVICES += tenbay_t-mb5eu-v01

define Device/thunder_timecloud
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Thunder
  DEVICE_MODEL := Timecloud
  DEVICE_PACKAGES := kmod-usb3 -wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += timecloud
endef
TARGET_DEVICES += thunder_timecloud

define Device/totolink_a7000r
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  UIMAGE_NAME := C8340R1C-9999
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := A7000R
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += totolink_a7000r

define Device/totolink_x5000r
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  UIMAGE_NAME := C8343R-9999
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := X5000R
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += totolink_x5000r

define Device/tozed_zlt-s12-pro
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := TOZED
  DEVICE_MODEL := ZLT S12 PRO
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 comgt-ncm -uboot-envtools
endef
TARGET_DEVICES += tozed_zlt-s12-pro

define Device/tplink_archer-ax23-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := Archer AX23
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := ARCHER-AX23-V1
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_archer-ax23-v1

define Device/tplink_archer-a6-v3
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := Archer A6
  DEVICE_VARIANT := V3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e \
	kmod-mt7663-firmware-ap -uboot-envtools
  TPLINK_BOARD_ID := ARCHER-A6-V3
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_archer-a6-v3

define Device/tplink_archer-c6-v3
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := Archer C6
  DEVICE_VARIANT := V3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  TPLINK_BOARD_ID := ARCHER-C6-V3
  KERNEL := $(KERNEL_DTB) | uImage lzma
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_archer-c6-v3

define Device/tplink_archer-c6u-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := Archer C6U
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb3 kmod-usb-ledtrig-usbport -uboot-envtools
  KERNEL := $(KERNEL_DTB) | uImage lzma
  TPLINK_BOARD_ID := ARCHER-C6U-V1
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_archer-c6u-v1

define Device/tplink_deco-m4r-v4
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := Deco M4R
  DEVICE_VARIANT := v4
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  KERNEL := $(KERNEL_DTB) | uImage lzma
  TPLINK_BOARD_ID := DECO-M4R-V4
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_deco-m4r-v4

define Device/tplink_eap235-wall-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := EAP235-Wall
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  TPLINK_BOARD_ID := EAP235-WALL-V1
  IMAGE_SIZE := 13440k
  IMAGE/factory.bin := append-rootfs | tplink-safeloader factory | \
	pad-extra 128
endef
TARGET_DEVICES += tplink_eap235-wall-v1

define Device/tplink_eap613-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := EAP613
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := EAP610-V3
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | pad-to 64k
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
  IMAGE_SIZE := 13248k
endef
TARGET_DEVICES += tplink_eap613-v1

define Device/tplink_eap615-wall-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := EAP615-Wall
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := EAP615-WALL-V1
  KERNEL := kernel-bin | lzma -d22 | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | pad-to 64k
  KERNEL_INITRAMFS := kernel-bin | lzma -d22 | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb with-initrd
  IMAGE_SIZE := 13248k
endef
TARGET_DEVICES += tplink_eap615-wall-v1

define Device/tplink_ec330-g5u-v1
  $(Device/nand)
  LOADER := bin
  IMAGE_SIZE := 49152k
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := EC330-G5u
  DEVICE_ALT0_VENDOR := TP-Link
  DEVICE_ALT0_MODEL := Archer C9ERT
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb-ledtrig-usbport kmod-usb3
  KERNEL := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage-tplink-c9 standalone '$(call toupper,$(LINUX_KARCH)) \
		$(VERSION_DIST) Linux-$(LINUX_VERSION)' | \
	uImage-tplink-c9 firmware 'OS IMAGE ($(VERSION_DIST))'
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | loader-kernel | \
	uImage none
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | \
	append-ubi | check-size
endef
TARGET_DEVICES += tplink_ec330-g5u-v1

define Device/tplink_er605-v2
  $(Device/nand)
  DEVICE_COMPAT_VERSION := 1.2
  DEVICE_COMPAT_MESSAGE := Config cannot be migrated because interface names have changed
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := ER605
  DEVICE_VARIANT := v2
  DEVICE_ALT0_VENDOR := TP-Link
  DEVICE_ALT0_MODEL := FR205
  DEVICE_ALT0_VARIANT := v1
  DEVICE_PACKAGES := -wpad-basic-mbedtls kmod-usb3 -uboot-envtools
  KERNEL_IN_UBI := 1
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 127744k
endef
TARGET_DEVICES += tplink_er605-v2

define Device/tplink_ex220-v1
  $(Device/dsa-migration)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := EX220
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := EX220-V1
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_ex220-v1

define Device/tplink_ex220-v2
  $(Device/dsa-migration)
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := EX220
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  TPLINK_BOARD_ID := EX220-V2
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 15744k
endef
TARGET_DEVICES += tplink_ex220-v2

define Device/tplink_mr600-v2-eu
  $(Device/dsa-migration)
  $(Device/tplink-v2)
  DEVICE_MODEL := MR600
  DEVICE_VARIANT := v2 (EU)
  TPLINK_FLASHLAYOUT := 16Mltq
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
		kmod-usb-net-qmi-wwan uqmi kmod-usb3 -uboot-envtools
  IMAGE/factory.bin := tplink-v2-image -e -a 0x10000
  IMAGE/sysupgrade.bin := tplink-v2-image -s -e -a 0x10000 | check-size | \
	append-metadata
  KERNEL := $(KERNEL_DTB) | uImage lzma
  KERNEL_INITRAMFS := $$(KERNEL) | tplink-v2-header
  TPLINK_BOARD_ID := MR600-V2-EU
  IMAGE_SIZE := 16384k
endef
TARGET_DEVICES += tplink_mr600-v2-eu

define Device/tplink_re350-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := RE350
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 -uboot-envtools
  TPLINK_BOARD_ID := RE350-V1
  IMAGE_SIZE := 6016k
  SUPPORTED_DEVICES += re350-v1
endef
TARGET_DEVICES += tplink_re350-v1

define Device/tplink_re500-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := RE500
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  TPLINK_BOARD_ID := RE500-V1
  IMAGE_SIZE := 14208k
endef
TARGET_DEVICES += tplink_re500-v1

define Device/tplink_re650-v1
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := RE650
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  TPLINK_BOARD_ID := RE650-V1
  IMAGE_SIZE := 14208k
endef
TARGET_DEVICES += tplink_re650-v1

define Device/tplink_re650-v2
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := RE650
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7615-firmware -uboot-envtools
  TPLINK_BOARD_ID := RE650-V2
  IMAGE_SIZE := 7994k
endef
TARGET_DEVICES += tplink_re650-v2

define Device/tplink_tl-wpa8631p-v3
  $(Device/dsa-migration)
  $(Device/tplink-safeloader)
  DEVICE_MODEL := TL-WPA8631P
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
  TPLINK_BOARD_ID := TL-WPA8631P-V3
  IMAGE_SIZE := 7232k
endef
TARGET_DEVICES += tplink_tl-wpa8631p-v3

define Device/ubnt_edgerouter_common
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Ubiquiti
  IMAGE_SIZE := 259840k
  FILESYSTEMS := squashfs
  KERNEL_SIZE := 6144k
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  DEVICE_PACKAGES += -wpad-basic-mbedtls -uboot-envtools
  DEVICE_COMPAT_VERSION := 2.0
  DEVICE_COMPAT_MESSAGE :=  Partition table has been changed due to kernel size restrictions. \
    Refer to the wiki page for instructions to migrate to the new layout: \
    https://openwrt.org/toh/ubiquiti/edgerouter_x_er-x_ka
endef

define Device/ubnt_edgerouter-x
  $(Device/ubnt_edgerouter_common)
  DEVICE_MODEL := EdgeRouter X
  SUPPORTED_DEVICES += ubnt-erx ubiquiti,edgerouterx
endef
TARGET_DEVICES += ubnt_edgerouter-x

define Device/ubnt_edgerouter-x-sfp
  $(Device/ubnt_edgerouter_common)
  DEVICE_MODEL := EdgeRouter X SFP
  DEVICE_ALT0_VENDOR := Ubiquiti
  DEVICE_ALT0_MODEL := EdgePoint R6
  DEVICE_PACKAGES += kmod-i2c-algo-pca kmod-gpio-pca953x kmod-sfp
  SUPPORTED_DEVICES += ubnt-erx-sfp ubiquiti,edgerouterx-sfp
endef
TARGET_DEVICES += ubnt_edgerouter-x-sfp

define Device/ubnt_unifi-6-lite
  $(Device/dsa-migration)
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi U6 Lite
  DEVICE_DTS_CONFIG := config@1
  DEVICE_DTS_LOADADDR := 0x87000000
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7915-firmware -uboot-envtools
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 15424k
endef
TARGET_DEVICES += ubnt_unifi-6-lite

define Device/ubnt_unifi-flexhd
  $(Device/dsa-migration)
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi FlexHD
  DEVICE_DTS_CONFIG := config@2
  DEVICE_DTS_LOADADDR := 0x87000000
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7615-firmware kmod-leds-ubnt-ledbar \
	-uboot-envtools
  IMAGE_SIZE := 15552k
endef
TARGET_DEVICES += ubnt_unifi-flexhd

define Device/ubnt_unifi-nanohd
  $(Device/dsa-migration)
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi nanoHD
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7615-firmware -uboot-envtools
  IMAGE_SIZE := 15552k
endef
TARGET_DEVICES += ubnt_unifi-nanohd

define Device/ubnt_usw-flex
  $(Device/dsa-migration)
  DEVICE_VENDOR := Ubiquiti
  DEVICE_MODEL := UniFi Switch Flex
  DEVICE_DTS_CONFIG := config@1
  DEVICE_DTS_LOADADDR := 0x87000000
  DEVICE_PACKAGES += -uboot-envtools
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE_SIZE := 14720k
endef
TARGET_DEVICES += ubnt_usw-flex

define Device/unielec_u7621-01-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := UniElec
  DEVICE_MODEL := U7621-01
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += unielec_u7621-01-16m

define Device/unielec_u7621-06-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := UniElec
  DEVICE_MODEL := U7621-06
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += u7621-06-256M-16M unielec,u7621-06-256m-16m
endef
TARGET_DEVICES += unielec_u7621-06-16m

define Device/unielec_u7621-06-32m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := UniElec
  DEVICE_MODEL := U7621-06
  DEVICE_VARIANT := 32M
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += unielec,u7621-06-32m
endef
TARGET_DEVICES += unielec_u7621-06-32m

define Device/unielec_u7621-06-64m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 65216k
  DEVICE_VENDOR := UniElec
  DEVICE_MODEL := U7621-06
  DEVICE_VARIANT := 64M
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-usb3 \
	-wpad-basic-mbedtls -uboot-envtools
  SUPPORTED_DEVICES += unielec,u7621-06-512m-64m
endef
TARGET_DEVICES += unielec_u7621-06-64m

define Device/wavlink_wl-wn531a6
  $(Device/dsa-migration)
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN531A6
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3 -uboot-envtools
  IMAGE_SIZE := 15040k
endef
TARGET_DEVICES += wavlink_wl-wn531a6

define Device/wavlink_wl-wn533a8
  $(Device/dsa-migration)
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN533A8
  KERNEL_INITRAMFS_SUFFIX := -WN533A8$$(KERNEL_SUFFIX)
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 -uboot-envtools
  IMAGE_SIZE := 15040k
endef
TARGET_DEVICES += wavlink_wl-wn533a8

define Device/wavlink_ws-wn572hp3-4g
  $(Device/dsa-migration)
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WS-WN572HP3
  DEVICE_VARIANT := 4G
  IMAGE_SIZE := 15040k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs | check-size | append-metadata
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb3 kmod-usb-net-rndis comgt-ncm -uboot-envtools
endef
TARGET_DEVICES += wavlink_ws-wn572hp3-4g

define Device/wavlink_wl-wn573hx1
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN573HX1
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | \
	check-size | append-metadata
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
endef
TARGET_DEVICES += wavlink_wl-wn573hx1

define Device/wevo_11acnas
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  UIMAGE_NAME := 11AC-NAS-Router(0.0.0)
  DEVICE_VENDOR := WeVO
  DEVICE_MODEL := 11AC NAS Router
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += 11acnas
endef
TARGET_DEVICES += wevo_11acnas

define Device/wevo_w2914ns-v2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  UIMAGE_NAME := W2914NS-V2(0.0.0)
  DEVICE_VENDOR := WeVO
  DEVICE_MODEL := W2914NS
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += w2914nsv2
endef
TARGET_DEVICES += wevo_w2914ns-v2

define Device/wifire_s1500-nbn
  $(Device/sercomm_s1500)
  DEVICE_VENDOR := WiFire
  DEVICE_MODEL := S1500.NBN
  DEVICE_ALT0_VENDOR := Sercomm
  DEVICE_ALT0_MODEL := S1500 BUC
  IMAGE_SIZE := 51200k
  IMAGE/factory.img := append-kernel | sercomm-kernel-factory | \
	sercomm-reset-slot1-chksum | append-ubi | check-size | \
	sercomm-factory-cqr | sercomm-pid-setbit 0x13 | sercomm-mkhash | \
	sercomm-crypto
  SERCOMM_HWID := BUC
  SERCOMM_HWVER := 10000
  SERCOMM_ROOTFS2_OFFSET := 0x4d00000
  SERCOMM_SWVER := 2015
endef
TARGET_DEVICES += wifire_s1500-nbn

define Device/winstars_ws-wn536p3
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Winstars
  DEVICE_MODEL := WS-WN536P3
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += winstars_ws-wn536p3

define Device/winstars_ws-wn583a6
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Winstars
  DEVICE_MODEL := WS-WN583A6
  DEVICE_ALT0_VENDOR := Gemeita
  DEVICE_ALT0_MODEL := AC2100
  KERNEL_INITRAMFS_SUFFIX := -WN583A6$$(KERNEL_SUFFIX)
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware -uboot-envtools
endef
TARGET_DEVICES += winstars_ws-wn583a6

define Device/wodesys_wd-r1802u
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := Wodesys
  DEVICE_MODEL := WD-R1802U
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
  SUPPORTED_DEVICES += mt7621-rfb-ax-nor
endef
TARGET_DEVICES += wodesys_wd-r1802u

define Device/xiaomi_nand_separate
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Xiaomi
  IMAGES += kernel1.bin rootfs0.bin
  IMAGE/kernel1.bin := append-kernel
  IMAGE/rootfs0.bin := append-ubi | check-size
endef

define Device/xiaomi_mi-router-3g
  $(Device/xiaomi_nand_separate)
  DEVICE_MODEL := Mi Router 3G
  IMAGE_SIZE := 124416k
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt76x2 kmod-usb3 kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += R3G mir3g xiaomi,mir3g
endef
TARGET_DEVICES += xiaomi_mi-router-3g

define Device/xiaomi_mi-router-3g-v2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 14848k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 3G
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 -uboot-envtools
  SUPPORTED_DEVICES += xiaomi,mir3g-v2
endef
TARGET_DEVICES += xiaomi_mi-router-3g-v2

define Device/xiaomi_mi-router-3-pro
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 255488k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 3 Pro
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += xiaomi,mir3p
endef
TARGET_DEVICES += xiaomi_mi-router-3-pro

define Device/xiaomi_mi-router-4
  $(Device/xiaomi_nand_separate)
  DEVICE_MODEL := Mi Router 4
  IMAGE_SIZE := 124416k
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt76x2
endef
TARGET_DEVICES += xiaomi_mi-router-4

define Device/xiaomi_mi-router-4a-gigabit
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 14848k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4A
  DEVICE_VARIANT := Gigabit Edition
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 -uboot-envtools
endef
TARGET_DEVICES += xiaomi_mi-router-4a-gigabit

define Device/xiaomi_mi-router-4a-gigabit-v2
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 14784k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4A
  DEVICE_VARIANT := Gigabit Edition v2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap \
	-uboot-envtools
endef
TARGET_DEVICES += xiaomi_mi-router-4a-gigabit-v2

define Device/xiaomi_mi-router-ac2100
  $(Device/xiaomi_nand_separate)
  DEVICE_MODEL := Mi Router AC2100
  IMAGE_SIZE := 120320k
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7615-firmware
endef
TARGET_DEVICES += xiaomi_mi-router-ac2100

define Device/xiaomi_mi-router-cr660x
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  DEVICE_VENDOR := Xiaomi
  IMAGE_SIZE := 128512k
  IMAGES += firmware.bin
  IMAGE/firmware.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | \
	check-size
  DEVICE_PACKAGES += kmod-mt7915-firmware
endef

define Device/xiaomi_mi-router-cr6606
  $(Device/xiaomi_mi-router-cr660x)
  DEVICE_MODEL := Mi Router CR6606
endef
TARGET_DEVICES += xiaomi_mi-router-cr6606

define Device/xiaomi_mi-router-cr6608
  $(Device/xiaomi_mi-router-cr660x)
  DEVICE_MODEL := Mi Router CR6608
endef
TARGET_DEVICES += xiaomi_mi-router-cr6608

define Device/xiaomi_mi-router-cr6609
  $(Device/xiaomi_mi-router-cr660x)
  DEVICE_MODEL := Mi Router CR6609
endef
TARGET_DEVICES += xiaomi_mi-router-cr6609

define Device/xiaomi_redmi-router-ac2100
  $(Device/xiaomi_nand_separate)
  DEVICE_MODEL := Redmi Router AC2100
  IMAGE_SIZE := 120320k
  DEVICE_PACKAGES += kmod-mt7603 kmod-mt7615-firmware
endef
TARGET_DEVICES += xiaomi_redmi-router-ac2100

define Device/xiaoyu_xy-c5
  $(Device/dsa-migration)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := XiaoYu
  DEVICE_MODEL := XY-C5
  DEVICE_PACKAGES := kmod-ata-ahci kmod-usb3 -wpad-basic-mbedtls \
	-uboot-envtools
endef
TARGET_DEVICES += xiaoyu_xy-c5

define Device/xzwifi_creativebox-v1
  $(Device/dsa-migration)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := CreativeBox
  DEVICE_MODEL := v1
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mt7603 kmod-mt76x2 kmod-mmc-mtk \
	kmod-usb3 -wpad-basic-mbedtls -uboot-envtools
endef
TARGET_DEVICES += xzwifi_creativebox-v1

define Device/youhua_wr1200js
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := YouHua
  DEVICE_MODEL := WR1200JS
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += youhua_wr1200js

define Device/youku_yk-l2
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Youku
  DEVICE_MODEL := YK-L2
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  UIMAGE_MAGIC := 0x12291000
  UIMAGE_NAME := 400000000000000000003000
endef
TARGET_DEVICES += youku_yk-l2

define Device/yuncore_ax820
  $(Device/dsa-migration)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := YunCore
  DEVICE_MODEL := AX820
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += yuncore_ax820

define Device/yuncore_fap640
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := YunCore
  DEVICE_MODEL := FAP640
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += yuncore_fap640

define Device/yuncore_fap690
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := YunCore
  DEVICE_MODEL := FAP690
  DEVICE_PACKAGES := kmod-mt7915-firmware -uboot-envtools
endef
TARGET_DEVICES += yuncore_fap690

define Device/yuncore_g720
  $(Device/dsa-migration)
  DEVICE_VENDOR := YunCore
  DEVICE_MODEL := G720
  DEVICE_PACKAGES := kmod-mt7915-firmware
  IMAGE_SIZE := 15808k
  SUPPORTED_DEVICES += mt7621-rfb-ax-nor
endef
TARGET_DEVICES += yuncore_g720

define Device/z-router_zr-2660
  $(Device/dsa-migration)
  $(Device/nand)
  DEVICE_VENDOR := Z-ROUTER
  DEVICE_MODEL := ZR-2660
  DEVICE_ALT0_VENDOR := Routerich
  DEVICE_ALT0_MODEL := AX1800
  IMAGE_SIZE := 90112k
  KERNEL_LOADADDR := 0x82000000
  KERNEL := kernel-bin | relocate-kernel $(loadaddr-y) | lzma | \
	fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  DEVICE_PACKAGES += kmod-mt7915-firmware kmod-usb3 -uboot-envtools
endef
TARGET_DEVICES += z-router_zr-2660

define Device/zbtlink_zbt-we1326
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WE1326
  DEVICE_ALT0_VENDOR := Wiflyer
  DEVICE_ALT0_MODEL := WF3526-P
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 kmod-mmc-mtk \
	-uboot-envtools
  SUPPORTED_DEVICES += zbt-we1326
endef
TARGET_DEVICES += zbtlink_zbt-we1326

define Device/zbtlink_zbt-we3526
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WE3526
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += zbtlink_zbt-we3526

define Device/zbtlink_zbt-wg1602-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG1602
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += zbtlink_zbt-wg1602-16m

define Device/zbtlink_zbt-wg1602-v04-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG1602-V04
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 kmod-usb3 \
        kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += zbtlink_zbt-wg1602-v04-16m

define Device/zbtlink_zbt-wg1602-v04-32m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32128k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG1602-V04
  DEVICE_VARIANT := 32M
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 kmod-usb3 \
        kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += zbtlink_zbt-wg1602-v04-32m

define Device/zbtlink_zbt-wg1608-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG1608
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt7615e \
	kmod-mt7663-firmware-ap kmod-usb3 kmod-usb-ledtrig-usbport \
	-uboot-envtools
endef
TARGET_DEVICES += zbtlink_zbt-wg1608-16m

define Device/zbtlink_zbt-wg1608-32m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG1608
  DEVICE_VARIANT := 32M
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt7603 kmod-mt7615e \
	kmod-mt7663-firmware-ap kmod-usb3 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += zbtlink_zbt-wg1608-32m

define Device/zbtlink_zbt-wg2626
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG2626
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += zbt-wg2626
endef
TARGET_DEVICES += zbtlink_zbt-wg2626

define Device/zbtlink_zbt-wg3526-16m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG3526
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 \
	kmod-usb3 kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += zbt-wg3526 zbt-wg3526-16M
endef
TARGET_DEVICES += zbtlink_zbt-wg3526-16m

define Device/zbtlink_zbt-wg3526-32m
  $(Device/dsa-migration)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WG3526
  DEVICE_VARIANT := 32M
  DEVICE_PACKAGES := kmod-ata-ahci kmod-mmc-mtk kmod-mt7603 kmod-mt76x2 \
	kmod-usb3 kmod-usb-ledtrig-usbport -uboot-envtools
  SUPPORTED_DEVICES += ac1200pro zbt-wg3526-32M
endef
TARGET_DEVICES += zbtlink_zbt-wg3526-32m

define Device/zio_freezio
  $(Device/dsa-migration)
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ZIO
  DEVICE_MODEL := FREEZIO
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt76x2 kmod-usb3 \
	kmod-usb-ledtrig-usbport -uboot-envtools
endef
TARGET_DEVICES += zio_freezio

define Device/zyxel_lte3301-plus
  $(Device/nand)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := LTE3301-PLUS
  KERNEL_SIZE := 31488k
  DEVICE_PACKAGES := kmod-mt7615-firmware kmod-usb3 kmod-usb-ledtrig-usbport \
	kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi
  KERNEL := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) $$(VERSION_DIST)-$$(REVISION)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) 9.99(ABQU.1)$$(VERSION_DIST)-recovery
  KERNEL_INITRAMFS_SUFFIX := -recovery.bin
endef
TARGET_DEVICES += zyxel_lte3301-plus

define Device/zyxel_lte5398-m904
  $(Device/nand)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := LTE5398-M904
  KERNEL_SIZE := 31488k
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7615-firmware kmod-usb3 uqmi \
  kmod-usb-net-qmi-wwan kmod-usb-serial-option kmod-usb-ledtrig-usbport
  KERNEL := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) $$(VERSION_DIST)-$$(REVISION)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) 9.99(ABQU.1)$$(VERSION_DIST)-recovery
  KERNEL_INITRAMFS_SUFFIX := -recovery.bin
endef
TARGET_DEVICES += zyxel_lte5398-m904

define Device/zyxel_lte7490-m904
  $(Device/nand)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := LTE7490-M904
  KERNEL_SIZE := 31488k
  DEVICE_PACKAGES := kmod-mt7603 kmod-usb3 kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi
  KERNEL := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) $$(VERSION_DIST)-$$(REVISION)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | uImage lzma | \
	zytrx-header $$(DEVICE_MODEL) 9.99(ABQY.9)$$(VERSION_DIST)-recovery
  KERNEL_INITRAMFS_SUFFIX := -recovery.bin
endef
TARGET_DEVICES += zyxel_lte7490-m904

define Device/zyxel_nr7101
  $(Device/nand)
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := NR7101
  KERNEL_SIZE := 31488k
  DEVICE_PACKAGES := kmod-mt7603 kmod-usb3 kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi
  KERNEL := $(KERNEL_DTB) | uImage lzma | zytrx-header $$(DEVICE_MODEL) $$(VERSION_DIST)-$$(REVISION)
  KERNEL_INITRAMFS := $(KERNEL_DTB) | uImage lzma | zytrx-header $$(DEVICE_MODEL) 9.99(ABUV.9)$$(VERSION_DIST)-recovery
  KERNEL_INITRAMFS_SUFFIX := -recovery.bin
endef
TARGET_DEVICES += zyxel_nr7101

define Device/zyxel_nwa-ax
  $(Device/nand)
  DEVICE_VENDOR := Zyxel
  KERNEL_SIZE := 8192k
  DEVICE_PACKAGES := kmod-mt7915-firmware zyxel-bootconfig
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  IMAGES += factory.bin ramboot-factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi | zyxel-nwa-fit
  IMAGE/ramboot-factory.bin := append-kernel | pad-to $$(KERNEL_SIZE) | append-ubi
endef

define Device/zyxel_nwa50ax
  $(Device/zyxel_nwa-ax)
  DEVICE_MODEL := NWA50AX
endef
TARGET_DEVICES += zyxel_nwa50ax

define Device/zyxel_nwa55axe
  $(Device/zyxel_nwa-ax)
  DEVICE_MODEL := NWA55AXE
endef
TARGET_DEVICES += zyxel_nwa55axe

define Device/zyxel_wap6805
  $(Device/nand)
  $(Device/uimage-lzma-loader)
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := WAP6805
  DEVICE_PACKAGES := kmod-mt7603 kmod-mt7621-qtn-rgmii -uboot-envtools
  KERNEL := $$(KERNEL/lzma-loader) | uImage none | uimage-padhdr 160
endef
TARGET_DEVICES += zyxel_wap6805

define Device/zyxel_wsm20
  $(Device/nand)
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 41943040
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := WSM20
  DEVICE_PACKAGES := kmod-mt7915-firmware
  KERNEL := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | znet-header V1.00(ABZF.0)C0
  KERNEL_INITRAMFS := kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb | znet-header V1.00(ABZF.0)C0
endef
TARGET_DEVICES += zyxel_wsm20
