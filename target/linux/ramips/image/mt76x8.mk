#
# MT76x8 Profiles
#

include ./common-tp-link.mk

DEFAULT_SOC := mt7628an

define Build/elecom-header
	$(eval model_id=$(1))
	( \
		fw_size="$$(printf '%08x' $$(stat -c%s $@))"; \
		echo -ne "$$(echo "031d6129$${fw_size}06000000$(model_id)" | \
			sed 's/../\\x&/g')"; \
		dd if=/dev/zero bs=92 count=1; \
		data_crc="$$(dd if=$@ | gzip -c | tail -c 8 | \
			od -An -N4 -tx4 --endian little | tr -d ' \n')"; \
		echo -ne "$$(echo "$${data_crc}00000000" | sed 's/../\\x&/g')"; \
		dd if=$@; \
	) > $@.new
	mv $@.new $@
endef

define Build/ravpower-wd009-factory
	mkimage -A mips -T standalone -C none -a 0x80010000 -e 0x80010000 \
		-n "OpenWrt Bootloader" -d $(UBOOT_PATH) $@.new
	cat $@ >> $@.new
	@mv $@.new $@
endef

define Build/append-teltonika-metadata
  $(eval model_id=$(1))
	echo \
		'{ \
			"metadata_version": "1.1", \
			"compat_version": "1.0", \
			"version": "", \
			"device_code": [".*"], \
			"hwver": [".*"], \
			"batch": [".*"], \
			"serial": [".*"], \
			"supported_devices":["teltonika,$(model_id)"], \
			"hw_support": { }, \
			"hw_mods": { } \
		}' | fwtool -I - $@
endef

define Device/7links_wlr-12xx
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := 7Links
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs | check-size | append-metadata
endef

define Device/7links_wlr-1230
  $(Device/7links_wlr-12xx)
  DEVICE_MODEL := WLR-1230
endef
TARGET_DEVICES += 7links_wlr-1230

define Device/7links_wlr-1240
  $(Device/7links_wlr-12xx)
  DEVICE_MODEL := WLR-1240
endef
TARGET_DEVICES += 7links_wlr-1240

define Device/alfa-network_awusfree1
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := AWUSFREE1
  DEVICE_PACKAGES := uboot-envtools
  SUPPORTED_DEVICES += awusfree1
endef
TARGET_DEVICES += alfa-network_awusfree1

define Device/asus_rt-ac1200
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AC1200
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-N600
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 kmod-usb-ohci \
	kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += asus_rt-ac1200

define Device/asus_rt-ac1200-v2
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-AC1200
  DEVICE_VARIANT := V2
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-AC750L
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | \
	append-rootfs | pad-rootfs
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
endef
TARGET_DEVICES += asus_rt-ac1200-v2

define Device/asus_rt-n10p-v3
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-N10P
  DEVICE_VARIANT := V3
  DEFAULT := n
endef
TARGET_DEVICES += asus_rt-n10p-v3

define Device/asus_rt-n11p-b1
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-N11P
  DEVICE_VARIANT := B1
  DEVICE_ALT0_VENDOR := ASUS
  DEVICE_ALT0_MODEL := RT-N12+
  DEVICE_ALT0_VARIANT := B1
  DEVICE_ALT1_VENDOR := ASUS
  DEVICE_ALT1_MODEL := RT-N300
  DEVICE_ALT1_VARIANT := B1
  DEFAULT := n
endef
TARGET_DEVICES += asus_rt-n11p-b1

define Device/asus_rt-n12-vp-b1
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := ASUS
  DEVICE_MODEL := RT-N12 VP
  DEVICE_VARIANT := B1
endef
TARGET_DEVICES += asus_rt-n12-vp-b1

define Device/buffalo_wcr-1166ds
  IMAGE_SIZE := 7936k
  BUFFALO_TAG_PLATFORM := MTK
  BUFFALO_TAG_VERSION := 9.99
  BUFFALO_TAG_MINOR := 9.99
  IMAGES += factory.bin
  IMAGE/sysupgrade.bin := trx -M 0x746f435c | pad-rootfs | append-metadata
  IMAGE/factory.bin := trx -M 0x746f435c | pad-rootfs | append-metadata | \
	buffalo-enc WCR-1166DS $$(BUFFALO_TAG_VERSION) -l | \
	buffalo-tag-dhp WCR-1166DS JP JP | buffalo-enc-tag -l | buffalo-dhp-image
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WCR-1166DS
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += wcr-1166ds
endef
TARGET_DEVICES += buffalo_wcr-1166ds

define Device/comfast_cf-wr617ac
  IMAGE_SIZE := 7872k
  DTS := CF-WR617AC
  DEVICE_VENDOR := COMFAST
  DEVICE_MODEL := CF-WR617AC
  DEVICE_PACKAGES := kmod-mt76x2 kmod-rt2800-pci
endef
TARGET_DEVICES += comfast_cf-wr617ac

define Device/comfast_cf-wr758ac
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := COMFAST
  DEVICE_MODEL := CF-WR758AC
  DEVICE_ALT0_VENDOR := Joowin
  DEVICE_ALT0_MODEL := JW-WR758AC
endef

define Device/comfast_cf-wr758ac-v1
  $(Device/comfast_cf-wr758ac)
  DEVICE_PACKAGES := kmod-mt76x2
  DEVICE_VARIANT := V1
  DEVICE_ALT0_VARIANT := V1
  SUPPORTED_DEVICES += joowin,jw-wr758ac-v1
endef
TARGET_DEVICES += comfast_cf-wr758ac-v1

define Device/comfast_cf-wr758ac-v2
  $(Device/comfast_cf-wr758ac)
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
  DEVICE_VARIANT := V2
  DEVICE_ALT0_VARIANT := V2
  SUPPORTED_DEVICES += joowin,jw-wr758ac-v2
endef
TARGET_DEVICES += comfast_cf-wr758ac-v2

define Device/cudy_m1200-v1
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := M1200
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
  UIMAGE_NAME := R22
  SUPPORTED_DEVICES += R22
endef
TARGET_DEVICES += cudy_m1200-v1

define Device/cudy_tr1200-v1
  IMAGE_SIZE := 15872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := TR1200
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport \
	kmod-mt7615e kmod-mt7663-firmware-ap
  SUPPORTED_DEVICES += R46
endef
TARGET_DEVICES += cudy_tr1200-v1

define Device/cudy_wr1000
  IMAGE_SIZE := 7872k
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | check-size | jcg-header 92.122
  JCG_MAXSIZE := 7872k
  DEVICE_VENDOR := Cudy
  DEVICE_MODEL := WR1000
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += wr1000
endef
TARGET_DEVICES += cudy_wr1000

define Device/d-team_pbr-d1
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := PandoraBox
  DEVICE_MODEL := PBR-D1
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += pbr-d1
endef
TARGET_DEVICES += d-team_pbr-d1

define Device/dlink_dap-1325-a1
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DAP-1325 A1
endef
TARGET_DEVICES += dlink_dap-1325-a1

define Device/duzun_dm06
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := DuZun
  DEVICE_MODEL := DM06
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += duzun-dm06
endef
TARGET_DEVICES += duzun_dm06

define Device/elecom_wrc-1167fs
  IMAGE_SIZE := 7360k
  DEVICE_VENDOR := ELECOM
  DEVICE_MODEL := WRC-1167FS
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to 64k | check-size | \
	xor-image -p 29944A25 -x | elecom-header 00228000 | \
	elecom-product-header WRC-1167FS
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += elecom_wrc-1167fs

define Device/glinet_gl-mt300n-v2
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := GL-MT300N
  DEVICE_VARIANT := V2
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += gl-mt300n-v2
endef
TARGET_DEVICES += glinet_gl-mt300n-v2

define Device/glinet_microuter-n300
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := microuter-N300
  SUPPORTED_DEVICES += microuter-n300
endef
TARGET_DEVICES += glinet_microuter-n300

define Device/glinet_vixmini
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := GL.iNet
  DEVICE_MODEL := VIXMINI
  SUPPORTED_DEVICES += vixmini
endef
TARGET_DEVICES += glinet_vixmini

define Device/hak5_wifi-pineapple-mk7
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Hak5
  DEVICE_MODEL := WiFi Pineapple Mark 7
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += wifi-pineapple-mk7
endef
TARGET_DEVICES += hak5_wifi-pineapple-mk7

define Device/hilink_hlk-7628n
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := HILINK
  DEVICE_MODEL := HLK-7628N
endef
TARGET_DEVICES += hilink_hlk-7628n

define Device/hilink_hlk-7688a
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Hi-Link
  DEVICE_MODEL := HLK-7688A
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += hilink_hlk-7688a

define Device/hiwifi_hc5611
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := HiWiFi
  DEVICE_MODEL := HC5611
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += hiwifi_hc5611

define Device/hiwifi_hc5661a
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := HiWiFi
  DEVICE_MODEL := HC5661A
  DEVICE_PACKAGES := kmod-mmc-mtk
  SUPPORTED_DEVICES += hc5661a
endef
TARGET_DEVICES += hiwifi_hc5661a

define Device/hiwifi_hc5761a
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := HiWiFi
  DEVICE_MODEL := HC5761A
  DEVICE_PACKAGES := kmod-mmc-mtk kmod-mt76x0e kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += hiwifi_hc5761a

define Device/hiwifi_hc5861b
  IMAGE_SIZE := 15808k
  DEVICE_VENDOR := HiWiFi
  DEVICE_MODEL := HC5861B
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += hiwifi_hc5861b

define Device/iptime_a3
  IMAGE_SIZE := 7936k
  UIMAGE_NAME := a3
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A3
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += iptime_a3

define Device/iptime_a604m
  IMAGE_SIZE := 7936k
  UIMAGE_NAME := a604m
  DEVICE_VENDOR := ipTIME
  DEVICE_MODEL := A604M
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += iptime_a604m

define Device/jotale_js76x8
  DEVICE_VENDOR := Jotale
  DEVICE_MODEL := JS76x8
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef

define Device/jotale_js76x8-8m
  $(Device/jotale_js76x8)
  IMAGE_SIZE := 7872k
  DEVICE_VARIANT := 8M
endef
TARGET_DEVICES += jotale_js76x8-8m

define Device/jotale_js76x8-16m
  $(Device/jotale_js76x8)
  IMAGE_SIZE := 16064k
  DEVICE_VARIANT := 16M
endef
TARGET_DEVICES += jotale_js76x8-16m

define Device/jotale_js76x8-32m
  $(Device/jotale_js76x8)
  IMAGE_SIZE := 32448k
  DEVICE_VARIANT := 32M
endef
TARGET_DEVICES += jotale_js76x8-32m

define Device/keenetic_kn-1221
  BLOCKSIZE := 64k
  IMAGE_SIZE := 29440k
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1221
  DEVICE_PACKAGES := kmod-usb2
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size 14720k | zyimage -d 0x801221 -v "KN-1221"
endef
TARGET_DEVICES += keenetic_kn-1221

define Device/keenetic_kn-1613
  IMAGE_SIZE := 15073280
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1613
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 0x801613 -v "KN-1613"
endef
TARGET_DEVICES += keenetic_kn-1613

define Device/keenetic_kn-1711
  BLOCKSIZE := 64k
  IMAGE_SIZE := 15073280
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1711
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap kmod-usb2
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 0x801711 -v "KN-1711"
endef
TARGET_DEVICES += keenetic_kn-1711

define Device/keenetic_kn-1713
  BLOCKSIZE := 64k
  IMAGE_SIZE := 15073280
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-1713
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap kmod-usb2
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 0x801713 -v "KN-1713"
endef
TARGET_DEVICES += keenetic_kn-1713

define Device/keenetic_kn-3211
  IMAGE_SIZE := 31488k
  DEVICE_VENDOR := Keenetic
  DEVICE_MODEL := KN-3211
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 0x803211 -v "KN-3211"
endef
TARGET_DEVICES += keenetic_kn-3211

define Device/kroks_kndrt31r16
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Kroks
  DEVICE_MODEL := Rt-Cse5 UW DRSIM
  DEVICE_ALT0_VENDOR := Kroks
  DEVICE_ALT0_MODEL := KNdRt31R16
  DEVICE_PACKAGES := kmod-usb2
  SUPPORTED_DEVICES += kndrt31r16
endef
TARGET_DEVICES += kroks_kndrt31r16

define Device/kroks_kndrt31r19
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Kroks
  DEVICE_MODEL := Rt-Pot mXw DS RSIM
  DEVICE_ALT0_VENDOR := Kroks
  DEVICE_ALT0_MODEL := KNdRt31R19
  DEVICE_PACKAGES := kmod-usb2 uqmi
  SUPPORTED_DEVICES += kndrt31r19
endef
TARGET_DEVICES += kroks_kndrt31r19

define Device/linksys_e5400
  IMAGE_SIZE := 16000k
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := E5400
  DEVICE_ALT0_VENDOR := Linksys
  DEVICE_ALT0_MODEL := E2500
  DEVICE_ALT0_VARIANT := v4
  DEVICE_ALT1_VENDOR := Linksys
  DEVICE_ALT1_MODEL := E5300
  DEVICE_ALT2_VENDOR := Linksys
  DEVICE_ALT2_MODEL := E5350
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += linksys_e5400

define Device/mediatek_linkit-smart-7688
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := LinkIt Smart 7688
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci uboot-envtools kmod-mmc-mtk
  SUPPORTED_DEVICES += linkits7688 linkits7688d
endef
TARGET_DEVICES += mediatek_linkit-smart-7688

define Device/mediatek_mt7628an-eval-board
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := MediaTek
  DEVICE_MODEL := MT7628 EVB
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += mt7628
endef
TARGET_DEVICES += mediatek_mt7628an-eval-board

define Device/mercury_mac1200r-v2
  IMAGE_SIZE := 7936k
  DEVICE_VENDOR := Mercury
  DEVICE_MODEL := MAC1200R
  DEVICE_VARIANT := v2.0
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += mac1200rv2
endef
TARGET_DEVICES += mercury_mac1200r-v2

define Device/mercusys_mb130-4g-v1
$(Device/tplink-v2)
  IMAGE_SIZE := 14912k
  DEVICE_VENDOR := MERCUSYS
  DEVICE_MODEL := MB130-4G
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt76x0e kmod-usb2 kmod-mt7663-firmware-ap kmod-mt7615e \
                     kmod-usb-serial-option kmod-usb-net-cdc-ether
  TPLINK_FLASHLAYOUT := 16MLmtk
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += mercusys_mb130-4g-v1

define Device/minew_g1-c
  IMAGE_SIZE := 15744k
  DEVICE_VENDOR := Minew
  DEVICE_MODEL := G1-C
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport kmod-usb-serial-cp210x
  SUPPORTED_DEVICES += minew-g1c
endef
TARGET_DEVICES += minew_g1-c

define Device/motorola_mwr03
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Motorola
  DEVICE_MODEL := MWR03
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += motorola_mwr03

define Device/netgear_r6020
  $(Device/netgear_sercomm_nor)
  IMAGE_SIZE := 7104k
  DEVICE_MODEL := R6020
  DEVICE_PACKAGES := kmod-mt76x2
  SERCOMM_HWNAME := R6020
  SERCOMM_HWID := CFR
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0040
  SERCOMM_PAD := 576k
endef
TARGET_DEVICES += netgear_r6020

define Device/netgear_r6080
  $(Device/netgear_sercomm_nor)
  IMAGE_SIZE := 7552k
  DEVICE_MODEL := R6080
  DEVICE_PACKAGES := kmod-mt76x2
  SERCOMM_HWNAME := R6080
  SERCOMM_HWID := CFR
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0040
  SERCOMM_PAD := 576k
endef
TARGET_DEVICES += netgear_r6080

define Device/netgear_r6120
  $(Device/netgear_sercomm_nor)
  IMAGE_SIZE := 15744k
  DEVICE_MODEL := R6120
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 kmod-usb-ohci
  SERCOMM_HWNAME := R6120
  SERCOMM_HWID := CGQ
  SERCOMM_HWVER := A001
  SERCOMM_SWVER := 0x0040
  SERCOMM_PAD := 576k
endef
TARGET_DEVICES += netgear_r6120

define Device/onion_omega2
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Onion
  DEVICE_MODEL := Omega2
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci uboot-envtools
  SUPPORTED_DEVICES += omega2
endef
TARGET_DEVICES += onion_omega2

define Device/onion_omega2p
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Onion
  DEVICE_MODEL := Omega2+
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci uboot-envtools kmod-mmc-mtk
  SUPPORTED_DEVICES += omega2p
endef
TARGET_DEVICES += onion_omega2p

define Device/oraybox_x1
  IMAGE_SIZE := 15360k
  DEVICE_VENDOR := OrayBox
  DEVICE_MODEL := X1
  DEVICE_PACKAGES:= kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += oraybox_x1

define Device/rakwireless_rak633
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Rakwireless
  DEVICE_MODEL := RAK633
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += rakwireless_rak633

define Device/ravpower_rp-wd009
  IMAGE_SIZE := 14272k
  DEVICE_VENDOR := RAVPower
  DEVICE_MODEL := RP-WD009
  UBOOT_PATH := $(STAGING_DIR_IMAGE)/mt7628_ravpower_rp-wd009-u-boot.bin
  DEVICE_PACKAGES := kmod-mt76x0e kmod-usb2 kmod-usb-ohci \
	kmod-mmc-mtk kmod-i2c-mt7628 ravpower-mcu
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | ravpower-wd009-factory
endef
TARGET_DEVICES += ravpower_rp-wd009

define Device/skylab_skw92a
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Skylab
  DEVICE_MODEL := SKW92A
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += skylab_skw92a

define Device/tama_w06
  IMAGE_SIZE := 15040k
  DEVICE_VENDOR := Tama
  DEVICE_MODEL := W06
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += tama_w06

define Device/totolink_a3
  IMAGE_SIZE := 7936k
  UIMAGE_NAME := za3
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := A3
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += totolink_a3

define Device/totolink_lr1200
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := TOTOLINK
  DEVICE_MODEL := LR1200
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 uqmi
endef
TARGET_DEVICES += totolink_lr1200

define Device/tplink_archer-c20-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := Archer C20
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0xc200004
  TPLINK_HWREVADD := 0x4
  DEVICE_PACKAGES := kmod-mt76x0e
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
  SUPPORTED_DEVICES += tplink,c20-v4
endef
TARGET_DEVICES += tplink_archer-c20-v4

define Device/tplink_archer-c20-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7616k
  DEVICE_MODEL := Archer C20
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8MSUmtk
  TPLINK_HWID := 0xc200005
  TPLINK_HWREVADD := 0x5
  DEVICE_PACKAGES := kmod-mt76x0e
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += tplink_archer-c20-v5

define Device/tplink_archer-c50-v3
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := Archer C50
  DEVICE_VARIANT := v3
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x001D9BA4
  TPLINK_HWREV := 0x79
  TPLINK_HWREVADD := 0x1
  DEVICE_PACKAGES := kmod-mt76x2
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
  SUPPORTED_DEVICES += tplink,c50-v3
endef
TARGET_DEVICES += tplink_archer-c50-v3

define Device/tplink_archer-c50-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7616k
  DEVICE_MODEL := Archer C50
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8MSUmtk
  TPLINK_HWID := 0x001D589B
  TPLINK_HWREV := 0x93
  TPLINK_HWREVADD := 0x2
  DEVICE_PACKAGES := kmod-mt76x2
  IMAGES := sysupgrade.bin
  SUPPORTED_DEVICES += tplink,c50-v4
endef
TARGET_DEVICES += tplink_archer-c50-v4

define Device/tplink_archer-c50-v6
  $(Device/tplink-v2)
  IMAGE_SIZE := 7616k
  DEVICE_MODEL := Archer C50
  DEVICE_VARIANT := v6 (CA/EU/RU)
  TPLINK_FLASHLAYOUT := 8MSUmtk
  TPLINK_HWID := 0x0C500006
  TPLINK_HWREVADD := 0x6
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += tplink_archer-c50-v6

define Device/tplink_archer-mr200-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7872k
  DEVICE_MODEL := Archer MR200
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8MLmtk
  TPLINK_HWID := 0x20000005
  TPLINK_HWREV := 0x5
  TPLINK_HWREVADD := 0x5
  DEVICE_PACKAGES := kmod-mt76x0e uqmi kmod-usb2 kmod-usb-serial-option
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_archer-mr200-v5

define Device/tplink_archer-mr200-v6
  $(Device/tplink-v2)
  IMAGE_SIZE := 15936k
  DEVICE_MODEL := Archer MR200
  DEVICE_VARIANT := v6
  TPLINK_FLASHLAYOUT := 16Mmtk
  TPLINK_HWID := 0x20000006
  TPLINK_HWREV := 0x6
  TPLINK_HWREVADD := 0x6
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap \
	kmod-usb2 kmod-usb-serial-option uqmi
  KERNEL := kernel-bin | append-dtb | lzma -d22
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := sysupgrade.bin
endef
TARGET_DEVICES += tplink_archer-mr200-v6

define Device/tplink_re200-v2
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE200
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt76x0e
  TPLINK_BOARD_ID := RE200-V2
endef
TARGET_DEVICES += tplink_re200-v2

define Device/tplink_re200-v3
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE200
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-mt76x0e
  TPLINK_BOARD_ID := RE200-V3
endef
TARGET_DEVICES += tplink_re200-v3

define Device/tplink_re200-v4
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE200
  DEVICE_VARIANT := v4
  DEVICE_PACKAGES := kmod-mt76x0e
  TPLINK_BOARD_ID := RE200-V4
endef
TARGET_DEVICES += tplink_re200-v4

define Device/tplink_re205-v3
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE205
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-mt76x0e
  TPLINK_BOARD_ID := RE205-V3
endef
TARGET_DEVICES += tplink_re205-v3

define Device/tplink_re220-v2
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE220
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-mt76x0e
  TPLINK_BOARD_ID := RE220-V2
endef
TARGET_DEVICES += tplink_re220-v2

define Device/tplink_re305-v1
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 6016k
  DEVICE_MODEL := RE305
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt76x2
  TPLINK_BOARD_ID := RE305-V1
endef
TARGET_DEVICES += tplink_re305-v1

define Device/tplink_re305-v3
  $(Device/tplink-safeloader)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := RE305
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-mt76x2
  TPLINK_BOARD_ID := RE305-V3
endef
TARGET_DEVICES += tplink_re305-v3

define Device/tplink_re365-v1
  $(Device/tplink-safeloader)
  DEVICE_MODEL := RE365
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-mt76x2
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | check-size | append-metadata
  IMAGE_SIZE := 7680k
  KERNEL_SIZE := 6016k
  TPLINK_BOARD_ID := RE365
endef
TARGET_DEVICES += tplink_re365-v1

define Device/tplink_tl-mr3020-v3
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-MR3020
  DEVICE_VARIANT := v3
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x30200003
  TPLINK_HWREV := 0x3
  TPLINK_HWREVADD := 0x3
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-mr3020-v3

define Device/tplink_tl-mr3420-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-MR3420
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x34200005
  TPLINK_HWREV := 0x5
  TPLINK_HWREVADD := 0x5
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-mr3420-v5

define Device/tplink_tl-mr6400-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-MR6400
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x64000004
  TPLINK_HWREV := 0x4
  TPLINK_HWREVADD := 0x4
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport \
	kmod-usb-serial-option kmod-usb-net-qmi-wwan uqmi
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-mr6400-v4

define Device/tplink_tl-mr6400-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-MR6400
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x64000005
  TPLINK_HWREV := 0x5
  TPLINK_HWREVADD := 0x5
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport \
	kmod-usb-serial-option kmod-usb-net-qmi-wwan uqmi
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-mr6400-v5

define Device/tplink_tl-wa801nd-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WA801ND
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08010005
  TPLINK_HWREVADD := 0x5
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wa801nd-v5

define Device/tplink_tl-wr802n-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR802N
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08020004
  TPLINK_HWREVADD := 0x4
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wr802n-v4

define Device/tplink_tl-wr840n-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR840N
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08400004
  TPLINK_HWREVADD := 0x4
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
  SUPPORTED_DEVICES += tl-wr840n-v4
endef
TARGET_DEVICES += tplink_tl-wr840n-v4

define Device/tplink_tl-wr840n-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 3904k
  DEVICE_MODEL := TL-WR840N
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 4Mmtk
  TPLINK_HWID := 0x08400005
  TPLINK_HWREVADD := 0x5
  IMAGES := sysupgrade.bin
  SUPPORTED_DEVICES += tl-wr840n-v5
  DEFAULT := n
endef
TARGET_DEVICES += tplink_tl-wr840n-v5

define Device/tplink_tl-wr841n-v13
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR841N
  DEVICE_VARIANT := v13
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08410013
  TPLINK_HWREV := 0x268
  TPLINK_HWREVADD := 0x13
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
  SUPPORTED_DEVICES += tl-wr841n-v13
endef
TARGET_DEVICES += tplink_tl-wr841n-v13

define Device/tplink_tl-wr841n-v14
  $(Device/tplink-v2)
  IMAGE_SIZE := 3968k
  DEVICE_MODEL := TL-WR841N
  DEVICE_VARIANT := v14
  TPLINK_FLASHLAYOUT := 4MLmtk
  TPLINK_HWID := 0x08410014
  TPLINK_HWREVADD := 0x14
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 64k | $$(IMAGE/factory.bin)
  DEFAULT := n
endef
TARGET_DEVICES += tplink_tl-wr841n-v14

define Device/tplink_tl-wr842n-v5
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR842N
  DEVICE_VARIANT := v5
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08420005
  TPLINK_HWREV := 0x5
  TPLINK_HWREVADD := 0x5
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wr842n-v5

define Device/tplink_tl-wr850n-v2
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR850N
  DEVICE_VARIANT := v2
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x08500002
  TPLINK_HWREVADD := 0x2
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wr850n-v2

define Device/tplink_tl-wr902ac-v3
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR902AC
  DEVICE_VARIANT := v3
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x000dc88f
  TPLINK_HWREV := 0x89
  TPLINK_HWREVADD := 0x1
  DEVICE_PACKAGES := kmod-mt76x0e kmod-usb2 kmod-usb-ohci \
	kmod-usb-ledtrig-usbport
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wr902ac-v3

define Device/tplink_tl-wr902ac-v4
  $(Device/tplink-v2)
  IMAGE_SIZE := 7808k
  DEVICE_MODEL := TL-WR902AC
  DEVICE_VARIANT := v4
  TPLINK_FLASHLAYOUT := 8Mmtk
  TPLINK_HWID := 0x000dc88f
  TPLINK_HWREV := 0x89
  TPLINK_HWREVADD := 0x1
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap kmod-usb2 kmod-usb-ohci \
	kmod-usb-ledtrig-usbport
  IMAGES := sysupgrade.bin tftp-recovery.bin
  IMAGE/tftp-recovery.bin := pad-extra 128k | $$(IMAGE/factory.bin)
endef
TARGET_DEVICES += tplink_tl-wr902ac-v4

define Device/unielec_u7628-01-16m
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := UniElec
  DEVICE_MODEL := U7628-01
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += u7628-01-128M-16M unielec,u7628-01-128m-16m
endef
TARGET_DEVICES += unielec_u7628-01-16m

define Device/vocore_vocore2
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := VoCore
  DEVICE_MODEL := VoCore2
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport \
	kmod-mmc-mtk
  SUPPORTED_DEVICES += vocore2
endef
TARGET_DEVICES += vocore_vocore2

define Device/vocore_vocore2-lite
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := VoCore
  DEVICE_MODEL := VoCore2-Lite
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport \
	kmod-mmc-mtk
  SUPPORTED_DEVICES += vocore2lite
endef
TARGET_DEVICES += vocore_vocore2-lite

define Device/wavlink_wl-wn531a3
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN531A3
  DEVICE_ALT0_VENDOR := Wavlink
  DEVICE_ALT0_MODEL := QUANTUM D4
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += wl-wn531a3
endef
TARGET_DEVICES += wavlink_wl-wn531a3

define Device/wavlink_wl-wn570ha1
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN570HA1
  DEVICE_PACKAGES := kmod-mt76x0e
endef
TARGET_DEVICES += wavlink_wl-wn570ha1

define Device/wavlink_wl-wn575a3
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN575A3
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += wl-wn575a3
endef
TARGET_DEVICES += wavlink_wl-wn575a3

define Device/wavlink_wl-wn576a2
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN576A2
  DEVICE_ALT0_VENDOR := Silvercrest
  DEVICE_ALT0_MODEL := SWV 733 B1
  DEVICE_PACKAGES := kmod-mt76x0e
endef
TARGET_DEVICES += wavlink_wl-wn576a2

define Device/wavlink_wl-wn577a2
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN577A2
  DEVICE_ALT0_VENDOR := Maginon
  DEVICE_ALT0_MODEL := WLR-755
  DEVICE_PACKAGES := kmod-mt76x0e
endef
TARGET_DEVICES += wavlink_wl-wn577a2

define Device/wavlink_wl-wn578a2
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Wavlink
  DEVICE_MODEL := WL-WN578A2
  DEVICE_ALT0_VENDOR := SilverCrest
  DEVICE_ALT0_MODEL := SWV 733 A2
  DEVICE_PACKAGES := kmod-mt76x0e
endef
TARGET_DEVICES += wavlink_wl-wn578a2

define Device/widora_neo-16m
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Widora
  DEVICE_MODEL := Widora-NEO
  DEVICE_VARIANT := 16M
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += widora-neo
endef
TARGET_DEVICES += widora_neo-16m

define Device/widora_neo-32m
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := Widora
  DEVICE_MODEL := Widora-NEO
  DEVICE_VARIANT := 32M
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += widora_neo-32m

define Device/wiznet_wizfi630s
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := WIZnet
  DEVICE_MODEL := WizFi630S
  SUPPORTED_DEVICES += wizfi630s
endef
TARGET_DEVICES += wiznet_wizfi630s

define Device/wrtnode_wrtnode2p
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := WRTnode
  DEVICE_MODEL := WRTnode 2P
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci kmod-usb-ledtrig-usbport
  SUPPORTED_DEVICES += wrtnode2p
endef
TARGET_DEVICES += wrtnode_wrtnode2p

define Device/wrtnode_wrtnode2r
  IMAGE_SIZE := 32448k
  DEVICE_VENDOR := WRTnode
  DEVICE_MODEL := WRTnode 2R
  DEVICE_PACKAGES := kmod-usb2 kmod-usb-ohci
  SUPPORTED_DEVICES += wrtnode2r
endef
TARGET_DEVICES += wrtnode_wrtnode2r

define Device/xiaomi_mi-router-4a-100m
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4A
  DEVICE_VARIANT := 100M Edition
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += xiaomi,mir4a-100m
endef
TARGET_DEVICES += xiaomi_mi-router-4a-100m

define Device/xiaomi_mi-router-4a-100m-intl
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4A
  DEVICE_VARIANT := 100M International Edition
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += xiaomi,mir4a-100m-intl
endef
TARGET_DEVICES += xiaomi_mi-router-4a-100m-intl

define Device/xiaomi_mi-router-4a-100m-intl-v2
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4A
  DEVICE_VARIANT := 100M International Edition V2
  DEVICE_PACKAGES := kmod-mt7615e kmod-mt7663-firmware-ap
endef
TARGET_DEVICES += xiaomi_mi-router-4a-100m-intl-v2

define Device/xiaomi_mi-router-4c
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := Mi Router 4C
  DEVICE_PACKAGES := uboot-envtools
endef
TARGET_DEVICES += xiaomi_mi-router-4c

define Device/xiaomi_miwifi-3a
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := MiWiFi 3A
  DEVICE_PACKAGES := kmod-mt76x2
endef
TARGET_DEVICES += xiaomi_miwifi-3a

define Device/xiaomi_miwifi-3c
  IMAGE_SIZE := 15104k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := MiWiFi 3C
  DEVICE_PACKAGES := uboot-envtools
endef
TARGET_DEVICES += xiaomi_miwifi-3c

define Device/xiaomi_miwifi-nano
  IMAGE_SIZE := 16064k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := MiWiFi Nano
  DEVICE_PACKAGES := uboot-envtools
  SUPPORTED_DEVICES += miwifi-nano
endef
TARGET_DEVICES += xiaomi_miwifi-nano

define Device/xiaomi_mi-ra75
  IMAGE_SIZE := 14976k
  DEVICE_VENDOR := Xiaomi
  DEVICE_MODEL := MiWiFi Range Extender AC1200 
  DEVICE_VARIANT := RA75
  DEVICE_PACKAGES := kmod-mt76x2
  SUPPORTED_DEVICES += xiaomi,mira75
endef
TARGET_DEVICES += xiaomi_mi-ra75

define Device/yuncore_cpe200
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Yuncore
  DEVICE_MODEL := CPE200
  DEVICE_PACKAGES := -kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap kmod-mt7663-firmware-sta
endef
TARGET_DEVICES += yuncore_cpe200

define Device/yuncore_m300
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Yuncore
  DEVICE_MODEL := M300
endef
TARGET_DEVICES += yuncore_m300

define Device/zbtlink_zbt-we1226
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WE1226
endef
TARGET_DEVICES += zbtlink_zbt-we1226

define Device/zbtlink_zbt-we2426-b
  IMAGE_SIZE := 7872k
  DEVICE_VENDOR := Zbtlink
  DEVICE_MODEL := ZBT-WE2426-B
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 kmod-usb-ohci
endef
TARGET_DEVICES += zbtlink_zbt-we2426-b

define Device/zyxel_keenetic-extra-ii
  IMAGE_SIZE := 29824k
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := Keenetic Extra II
  DEVICE_PACKAGES := kmod-mt76x2 kmod-usb2 kmod-usb-ohci \
	kmod-usb-ledtrig-usbport
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(sysupgrade_bin) | pad-to $$$$(BLOCKSIZE) | \
	check-size | zyimage -d 6162 -v "ZyXEL Keenetic Extra II"
endef
TARGET_DEVICES += zyxel_keenetic-extra-ii

define Device/teltonika_rut200
  DEVICE_VENDOR := Teltonika
  DEVICE_MODEL := RUT200
  DEVICE_VARIANT := v1-v4
  IMAGE_SIZE := 15424k
  BLOCKSIZE := 64k
  DEVICE_PACKAGES +=kmod-mt76x2 kmod-usb2 kmod-usb-ohci kmod-usb-serial-option kmod-usb-net-cdc-ether
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size | append-teltonika-metadata rut2m
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += teltonika_rut200

define Device/teltonika_rut241
  DEVICE_VENDOR := Teltonika
  DEVICE_MODEL := RUT241
  DEVICE_VARIANT := v1-v4
  IMAGE_SIZE := 15424k
  BLOCKSIZE := 64k
  DEVICE_PACKAGES += uqmi kmod-mt76x2 kmod-usb2 kmod-usb-ohci kmod-usb-serial-option
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size | append-teltonika-metadata rut2m
  IMAGE/sysupgrade.bin := append-kernel | pad-to $$$$(BLOCKSIZE) | append-rootfs | pad-rootfs | check-size | append-metadata
endef
TARGET_DEVICES += teltonika_rut241
