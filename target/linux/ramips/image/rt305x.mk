#
# RT305X Profiles
#

define Device/asl26555-8M
  DTS := ASL26555-8M
  IMAGE_SIZE := 7744k
  SUPPORTED_DEVICES := asl26555
  DEVICE_TITLE := Alpha ASL26555 
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += asl26555-8M


define Device/asl26555-16M
  DTS := ASL26555-16M
  IMAGE_SIZE := 15872k
  SUPPORTED_DEVICES := asl26555
  DEVICE_TITLE := Alpha ASL26555 16M
  DEVICE_PACKAGES :=
endef
TARGET_DEVICES += asl26555-16M


define Device/nixcore-8M
  DTS := NIXCORE-8M
  IMAGE_SIZE := 7872k
  SUPPORTED_DEVICES := nixcore-x1
  DEVICE_TITLE := NixcoreX1 8M
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-i2c-core kmod-i2c-ralink kmod-spi-dev
endef
TARGET_DEVICES += nixcore-8M


define Device/nixcore-16M
  DTS := NIXCORE-16M
  IMAGE_SIZE := 16064k
  SUPPORTED_DEVICES := nixcore-x1
  DEVICE_TITLE := NixcoreX1 16M
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-i2c-core kmod-i2c-ralink kmod-spi-dev
endef
TARGET_DEVICES += nixcore-16M


define Device/vocore-8M
  DTS := VOCORE-8M
  IMAGE_SIZE := 7872k
  SUPPORTED_DEVICES := vocore
  DEVICE_TITLE :=  VoCore 8M
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
		kmod-i2c-core kmod-i2c-ralink \
		kmod-spi-dev
endef
TARGET_DEVICES += vocore-8M


define Device/vocore-16M
  DTS := VOCORE-16M
  IMAGE_SIZE := 16064k
  SUPPORTED_DEVICES := vocore
  DEVICE_TITLE := VoCore 16M
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
		kmod-i2c-core kmod-i2c-ralink \
		kmod-spi-dev
endef
TARGET_DEVICES += vocore-16M


define Device/all5002
  DTS := ALL5002
  IMAGE_SIZE := 32448k
  DEVICE_TITLE := Allnet ALL5002
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport \
          kmod-i2c-core kmod-i2c-gpio kmod-hwmon-lm92 kmod-gpio-pcf857x
endef
TARGET_DEVICES += all5002


define Device/all5003
  DTS := ALL5003
  IMAGE_SIZE := 32448k
  DEVICE_TITLE := Allnet ALL5003
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport \
          kmod-i2c-core kmod-i2c-gpio kmod-hwmon-lm92 kmod-gpio-pcf857x
endef
TARGET_DEVICES += all5003


define Device/atp-52b
  DTS := ATP-52B
  IMAGE_SIZE := 7808k
  DEVICE_TITLE := Argus ATP-52B
endef
TARGET_DEVICES += atp-52b


define Device/f5d8235v2
  DTS := F5D8235_V2
  IMAGE_SIZE := 7744k
  DEVICE_TITLE := Belkin F5D8235 v2
endef
TARGET_DEVICES += f5d8235v2


define Device/f7c027
  DTS := F7C027
  IMAGE_SIZE := 7616k
  DEVICE_TITLE := Belkin F7C027
  DEVICE_PACKAGES := -kmod-usb-core -kmod-usb-dwc2 -kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += f7c027


define Device/air3gii
  DTS := AIR3GII
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := AirLive Air3GII
endef
TARGET_DEVICES += air3gii


define Device/awm003-evb
  DTS := AWM003-EVB
  DEVICE_TITLE := AsiaRF AWM003 EVB
endef
TARGET_DEVICES += awm003-evb


define Device/awmapn2403
  DTS := AWAPN2403
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := AsiaRF AWAPN2403
endef
TARGET_DEVICES += awmapn2403


define Device/bc2
  DTS := BC2
  DEVICE_TITLE := NexAira BC2
endef
TARGET_DEVICES += bc2


define Device/carambola
  DTS := CARAMBOLA
  DEVICE_TITLE := 8devices Carambola
  DEVICE_PACKAGES :=
endef
TARGET_DEVICES += carambola


define Device/d105
  DTS := D105
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Huawei D105
endef
TARGET_DEVICES += d105


define Device/dir-620-a1
  DTS := DIR-620-A1
  DEVICE_TITLE := D-Link DIR-620 A1
endef
TARGET_DEVICES += dir-620-a1


define Device/dir-620-d1
  DTS := DIR-620-D1
  DEVICE_TITLE := D-Link DIR-620 D1
endef
TARGET_DEVICES += dir-620-d1


define Device/dwr-512-b
  DTS := DWR-512-B
  DEVICE_TITLE := D-Link DWR-512 B
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-i2c-core kmod-i2c-ralink kmod-spi-dev \
			kmod-usb-serial kmod-usb-serial-option kmod-usb-serial-wwan comgt
endef
TARGET_DEVICES += dwr-512-b


define Device/esr-9753
  DTS := ESR-9753
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := EnGenius ESR-9753
endef
TARGET_DEVICES += esr-9753


define Device/hw550-3g
  DTS := HW550-3G
  DEVICE_TITLE := Aztech HW550-3G
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += hw550-3g


define Device/rt-n13u
  DTS := RT-N13U
  DEVICE_TITLE := Asus RT-N13U
  DEVICE_PACKAGES := kmod-leds-gpio kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef
TARGET_DEVICES += rt-n13u


define Device/freestation5
  DTS := FREESTATION5
  DEVICE_TITLE := ARC Wireless FreeStation
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-rt2500-usb kmod-rt2800-usb kmod-rt2x00-usb
endef
TARGET_DEVICES += freestation5


define Device/ip2202
  DTS := IP2202
  DEVICE_TITLE := Poray IP2202
endef
TARGET_DEVICES += ip2202


define Device/ht-tm02
  DTS := HT-TM02
  DEVICE_TITLE := HooToo HT-TM02
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += ht-tm02


define Device/mofi3500-3gn
  DTS := MOFI3500-3GN
  DEVICE_TITLE := MoFi Network MOFI3500-3GN
endef
TARGET_DEVICES += mofi3500-3gn


define Device/3g150b
  DTS := 3G150B
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  UIMAGE_NAME:= Linux Kernel Image
  DEVICE_TITLE := Tenda 3G150B
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += 3g150b


define Device/mr-102n
  DTS := MR-102N
  DEVICE_TITLE := AXIMCom MR-102N
endef
TARGET_DEVICES += mr-102n


define Device/mpr-a1
  DTS := MPRA1
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  UIMAGE_NAME:= Linux Kernel Image
  DEVICE_TITLE := HAME MPR-A1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-netdev
endef
TARGET_DEVICES += mpr-a1


define Device/mpr-a2
  DTS := MPRA2
  UIMAGE_NAME:= Linux Kernel Image
  DEVICE_TITLE := HAME MPR-A2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-netdev
endef
TARGET_DEVICES += mpr-a2


define Device/dir-300-b7
  DTS := DIR-300-B7
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := D-Link DIR-300 B7
endef
TARGET_DEVICES += dir-300-b7


define Device/dir-320-b1
  DTS := DIR-320-B1
  DEVICE_TITLE := D-Link DIR-320 B1
endef
TARGET_DEVICES += dir-320-b1


define Device/nbg-419n
  DTS := NBG-419N
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := ZyXEL NBG-419N
endef
TARGET_DEVICES += nbg-419n


define Device/nbg-419n2
  DTS := NBG-419N2
  IMAGE_SIZE := $(ralink_default_fw_size_8M)
  DEVICE_TITLE := ZyXEL NBG-419N2
endef
TARGET_DEVICES += nbg-419n2


define Device/mzk-wdpr
  DTS := MZK-WDPR
  DEVICE_TITLE := Planex MZK-WDPR
endef
TARGET_DEVICES += mzk-wdpr


define Device/ncs601W
  DTS := NCS601W
  DEVICE_TITLE := Wansview NCS601W
  DEVICE_PACKAGES := kmod-video-core kmod-video-uvc \
		kmod-usb-core kmod-usb-ohci
endef
TARGET_DEVICES += ncs601W


define Device/m2m
  DTS := M2M
  UIMAGE_NAME:= Linux Kernel Image
  DEVICE_TITLE := Intenso Memory 2 Move
  DEVICE_PACKAGES := kmod-ledtrig-netdev kmod-ledtrig-timer \
		kmod-usb-core kmod-usb2 kmod-usb-storage kmod-scsi-core \
		kmod-fs-ext4 kmod-fs-vfat block-mount
endef
TARGET_DEVICES += m2m


define Device/miniembplug
  DTS := MINIEMBPLUG
  DEVICE_TITLE := Omnima MiniEMBPlug
endef
TARGET_DEVICES += miniembplug


define Device/miniembwifi
  DTS := MINIEMBWIFI
  DEVICE_TITLE := Omnima MiniEMBWiFi
endef
TARGET_DEVICES += miniembwifi


define Device/psr-680w
  DTS := PSR-680W
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Petatel PSR-680W Wireless 3G Router
endef
TARGET_DEVICES += psr-680w


define Device/pwh2004
  DTS := PWH2004
  DEVICE_TITLE := Prolink PWH2004
  DEVICE_PACKAGES :=
endef
TARGET_DEVICES += pwh2004


define Device/rt5350f-olinuxino
  DTS := RT5350F-OLINUXINO
  DEVICE_TITLE := RT5350F-OLinuXino
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
		kmod-i2c-core kmod-i2c-ralink \
		kmod-spi-dev
endef
TARGET_DEVICES += rt5350f-olinuxino


define Device/rt5350f-olinuxino-evb
  DTS := RT5350F-OLINUXINO-EVB
  DEVICE_TITLE := RT5350F-OLinuXino-EVB
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
		kmod-i2c-core kmod-i2c-ralink \
		kmod-spi-dev
endef
TARGET_DEVICES += rt5350f-olinuxino-evb


define Device/rt-g32-b1
  DTS := RT-G32-B1
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Asus RT-G32 B1
endef
TARGET_DEVICES += rt-g32-b1


define Device/rt-n10-plus
  DTS := RT-N10-PLUS
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Asus RT-N10+
endef
TARGET_DEVICES += rt-n10-plus


define Device/rut5xx
  DTS := RUT5XX
  DEVICE_TITLE := Teltonika RUT5XX
endef
TARGET_DEVICES += rut5xx


define Device/sl-r7205
  DTS := SL-R7205
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Skyline SL-R7205 Wireless 3G Router
endef
TARGET_DEVICES += sl-r7205


define Device/tew-714tru
  DTS := TEW-714TRU
  DEVICE_TITLE := TRENDnet TEW-714TRU
endef
TARGET_DEVICES += tew-714tru


define Device/v22rw-2x2
  DTS := V22RW-2X2
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Ralink AP-RT3052-V22RW-2X2
endef
TARGET_DEVICES += v22rw-2x2


define Device/w502u
  DTS := W502U
  DEVICE_TITLE := ALFA Networks W502U
endef
TARGET_DEVICES += w502u


define Device/wcr150gn
  DTS := WCR150GN
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Sparklan WCR-150GN
endef
TARGET_DEVICES += wcr150gn


define Device/mzk-dp150n
  DTS := MZK-DP150N
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Planex MZK-DP150N
  DEVICE_PACKAGES := kmod-spi-dev
endef
TARGET_DEVICES += mzk-dp150n


define Device/wizard-8800
  DTS := WIZARD8800
  UIMAGE_NAME:= Linux Kernel Image
  DEVICE_TITLE := EasyAcc WIZARD 8800
endef
TARGET_DEVICES += wizard-8800


define Device/wizfi630a
  DTS := WIZFI630A
  IMAGE_SIZE := $(ralink_default_fw_size_16M)
  DEVICE_TITLE := WIZnet WizFi630A
endef
TARGET_DEVICES += wizfi630a


define Device/wl-330n
  DTS := WL-330N
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Asus WL-330N
endef
TARGET_DEVICES += wl-330n


define Device/wl-330n3g
  DTS := WL-330N3G
  BLOCKSIZE := 4k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Asus WL-330N3G
  DEVICE_PACKAGES :=
endef
TARGET_DEVICES += wl-330n3g


define Device/wl-351
  DTS := WL-351
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Sitecom WL-351 v1
  DEVICE_PACKAGES := kmod-switch-rtl8366rb kmod-swconfig swconfig
endef
TARGET_DEVICES += wl-351


define Device/ur-326n4g
  DTS := UR-326N4G
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := UPVEL UR-326N4G
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += ur-326n4g


define Device/ur-336un
  DTS := UR-336UN
  DEVICE_TITLE := UPVEL UR-336UN
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-dwc2 kmod-usb-ledtrig-usbport
endef
TARGET_DEVICES += ur-336un


define Device/wr6202
  DTS := WR6202
  DEVICE_TITLE := AWB WR6202
endef
TARGET_DEVICES += wr6202


define Device/xdxrn502j
  DTS := XDXRN502J
  BLOCKSIZE := 64k
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := XDX RN502J
endef
TARGET_DEVICES += xdxrn502j
