include ./common-tp-link.mk


define Device/tl-mr10u-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR10U
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
  BOARDNAME := TL-MR10U
  DEVICE_PROFILE := TLMR10U
  TPLINK_HWID := 0x00100101
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-mr10u-v1

define Device/tl-mr11u-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR11U
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR11U
  DEVICE_PROFILE := TLMR11U
  TPLINK_HWID := 0x00110101
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-mr11u-v1

define Device/tl-mr11u-v2
  $(Device/tl-mr11u-v1)
  DEVICE_MODEL := TL-MR11U
  DEVICE_VARIANT := v2
  TPLINK_HWID := 0x00110102
endef
TARGET_DEVICES += tl-mr11u-v2

define Device/tl-mr12u-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR12U
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR13U
  DEVICE_PROFILE := TLMR12U
  TPLINK_HWID := 0x00120101
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-mr12u-v1

define Device/tl-mr13u-v1
  $(Device/tl-mr12u-v1)
  DEVICE_MODEL := TL-MR13U
  DEVICE_VARIANT := v1
  DEVICE_PROFILE := TLMR13U
  TPLINK_HWID := 0x00130101
endef
TARGET_DEVICES += tl-mr13u-v1

define Device/tl-mr3020-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR3020
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR3020
  DEVICE_PROFILE := TLMR3020
  TPLINK_HWID := 0x30200001
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-mr3020-v1

define Device/tl-mr3040-v1
  $(Device/tl-mr3020-v1)
  DEVICE_MODEL := TL-MR3040
  DEVICE_VARIANT := v1
  BOARDNAME := TL-MR3040
  DEVICE_PROFILE := TLMR3040
  TPLINK_HWID := 0x30400001
endef
TARGET_DEVICES += tl-mr3040-v1

define Device/tl-mr3040-v2
  $(Device/tl-mr3040-v1)
  DEVICE_MODEL := TL-MR3040
  DEVICE_VARIANT := v2
  BOARDNAME := TL-MR3040-v2
  TPLINK_HWID := 0x30400002
endef
TARGET_DEVICES += tl-mr3040-v2

define Device/tl-mr3220-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-MR3220
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR3220
  DEVICE_PROFILE := TLMR3220
  TPLINK_HWID := 0x32200001
endef
TARGET_DEVICES += tl-mr3220-v1

define Device/tl-mr3220-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR3220
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR3220-v2
  DEVICE_PROFILE := TLMR3220
  TPLINK_HWID := 0x32200002
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-mr3220-v2

define Device/tl-mr3420-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-MR3420
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR3420
  DEVICE_PROFILE := TLMR3420
  TPLINK_HWID := 0x34200001
endef
TARGET_DEVICES += tl-mr3420-v1

define Device/tl-mr3420-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-MR3420
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-MR3420-v2
  DEVICE_PROFILE := TLMR3420
  TPLINK_HWID := 0x34200002
endef
TARGET_DEVICES += tl-mr3420-v2

define Device/tl-wa701nd-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA701N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA901ND
  DEVICE_PROFILE := TLWA701
  TPLINK_HWID := 0x07010001
endef
TARGET_DEVICES += tl-wa701nd-v1

define Device/tl-wa701nd-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA701N/ND
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WA701ND-v2
  DEVICE_PROFILE := TLWA701
  TPLINK_HWID := 0x07010002
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wa701nd-v2

define Device/tl-wa7210n-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA7210N
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := rssileds kmod-ledtrig-netdev
  BOARDNAME := TL-WA7210N-v2
  DEVICE_PROFILE := TLWA7210
  TPLINK_HWID := 0x72100002
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wa7210n-v2

define Device/tl-wa730re-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA730RE
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA901ND
  DEVICE_PROFILE := TLWA730RE
  TPLINK_HWID := 0x07300001
endef
TARGET_DEVICES += tl-wa730re-v1

define Device/tl-wa750re-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA750RE
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := rssileds
  BOARDNAME := TL-WA750RE
  DEVICE_PROFILE := TLWA750
  TPLINK_HWID := 0x07500001
endef
TARGET_DEVICES += tl-wa750re-v1

define Device/tl-wa7510n-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA7510N
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA7510N
  DEVICE_PROFILE := TLWA7510
  TPLINK_HWID := 0x75100001
endef
TARGET_DEVICES += tl-wa7510n-v1

define Device/tl-wa801nd-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA801N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA901ND
  DEVICE_PROFILE := TLWA801
  TPLINK_HWID := 0x08010001
endef
TARGET_DEVICES += tl-wa801nd-v1

define Device/tl-wa801nd-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA801N/ND
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WA801ND-v2
  DEVICE_PROFILE := TLWA801
  TPLINK_HWID := 0x08010002
endef
TARGET_DEVICES += tl-wa801nd-v2

define Device/tl-wa801nd-v3
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA801N/ND
  DEVICE_VARIANT := v3
  BOARDNAME := TL-WA801ND-v3
  DEVICE_PROFILE := TLWA801
  TPLINK_HWID := 0x08010003
endef
TARGET_DEVICES += tl-wa801nd-v3

define Device/tl-wa801nd-v4
  $(Device/tl-wa801nd-v3)
  DEVICE_MODEL := TL-WA801N/ND
  DEVICE_VARIANT := v4
  TPLINK_HWID := 0x08010004
endef
TARGET_DEVICES += tl-wa801nd-v4

define Device/tl-wa830re-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA830RE
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA901ND
  DEVICE_PROFILE := TLWA830
  TPLINK_HWID := 0x08300010
endef
TARGET_DEVICES += tl-wa830re-v1

define Device/tl-wa830re-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA830RE
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WA830RE-v2
  DEVICE_PROFILE := TLWA830
  TPLINK_HWID := 0x08300002
endef
TARGET_DEVICES += tl-wa830re-v2

define Device/tl-wa850re-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA850RE
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := rssileds
  BOARDNAME := TL-WA850RE
  DEVICE_PROFILE := TLWA850
  TPLINK_HWID := 0x08500001
endef
TARGET_DEVICES += tl-wa850re-v1

define Device/tl-wa850re-v2
  $(Device/tplink-safeloader)
  DEVICE_MODEL := TL-WA850RE
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := rssileds
  BOARDNAME := TL-WA850RE-V2
  DEVICE_PROFILE := TLWA850
  TPLINK_BOARD_ID := TLWA850REV2
  TPLINK_HWID := 0x08500002
  TPLINK_HWREV := 0
  IMAGE_SIZE := 3648k
  MTDPARTS := spi0.0:128k(u-boot)ro,1344k(kernel),2304k(rootfs),256k(config)ro,64k(art)ro,3648k@0x20000(firmware)
endef
TARGET_DEVICES += tl-wa850re-v2

define Device/tl-wa855re-v1
  $(Device/tplink-safeloader)
  DEVICE_MODEL := TL-WA855RE
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA855RE-v1
  DEVICE_PROFILE := TLWA855RE
  TPLINK_HWID := 0x08550001
  TPLINK_BOARD_ID := TLWA855REV1
  TPLINK_HWREV := 0
  IMAGE_SIZE := 3648k
  MTDPARTS := spi0.0:128k(u-boot)ro,1344k(kernel),2304k(rootfs),256k(config)ro,64k(art)ro,3648k@0x20000(firmware)
endef
TARGET_DEVICES += tl-wa855re-v1

define Device/tl-wa860re-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA860RE
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA860RE
  DEVICE_PROFILE := TLWA860
  TPLINK_HWID := 0x08600001
endef
TARGET_DEVICES += tl-wa860re-v1

define Device/tl-wa901nd-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA901N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WA901ND
  DEVICE_PROFILE := TLWA901
  TPLINK_HWID := 0x09010001
endef
TARGET_DEVICES += tl-wa901nd-v1

define Device/tl-wa901nd-v2
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WA901N/ND
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WA901ND-v2
  DEVICE_PROFILE := TLWA901
  TPLINK_HWID := 0x09010002
endef
TARGET_DEVICES += tl-wa901nd-v2

define Device/tl-wa901nd-v3
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA901N/ND
  DEVICE_VARIANT := v3
  BOARDNAME := TL-WA901ND-v3
  DEVICE_PROFILE := TLWA901
  TPLINK_HWID := 0x09010003
endef
TARGET_DEVICES += tl-wa901nd-v3

define Device/tl-wa901nd-v4
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WA901N/ND
  DEVICE_VARIANT := v4
  BOARDNAME := TL-WA901ND-v4
  DEVICE_PROFILE := TLWA901
  TPLINK_HWID := 0x09010004
  IMAGE/factory.bin := append-rootfs | mktplinkfw factory -C EU
endef
TARGET_DEVICES += tl-wa901nd-v4

define Device/tl-wa901nd-v5
  $(Device/tl-wa901nd-v4)
  DEVICE_MODEL := TL-WA901N/ND
  DEVICE_VARIANT := v5
  BOARDNAME := TL-WA901ND-v5
  TPLINK_HWID := 0x09010005
endef
TARGET_DEVICES += tl-wa901nd-v5

define Device/tl-wdr3320-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WDR3320
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
  BOARDNAME := TL-WDR3320-v2
  DEVICE_PROFILE := TLWDR3320V2
  TPLINK_HWID := 0x33200002
  TPLINK_HEADER_VERSION := 2
endef
TARGET_DEVICES += tl-wdr3320-v2

define Device/tl-wr1041n-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR1041N
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WR1041N-v2
  DEVICE_PROFILE := TLWR1041
  TPLINK_HWID := 0x10410002
endef
TARGET_DEVICES += tl-wr1041n-v2

define Device/tl-wr703n-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR703N
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
  BOARDNAME := TL-WR703N
  DEVICE_PROFILE := TLWR703
  TPLINK_HWID := 0x07030101
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wr703n-v1

define Device/tl-wr710n-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR710N
  DEVICE_VARIANT := v2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
  BOARDNAME := TL-WR710N
  DEVICE_PROFILE := TLWR710
  TPLINK_HWID := 0x07100002
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wr710n-v2

define Device/tl-wr720n-v3
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR720N
  DEVICE_VARIANT := v3
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
  BOARDNAME := TL-WR720N-v3
  DEVICE_PROFILE := TLWR720
  TPLINK_HWID := 0x07200103
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wr720n-v3

define Device/tl-wr720n-v4
  $(Device/tl-wr720n-v3)
  DEVICE_MODEL := TL-WR720N
  DEVICE_VARIANT := v4
  TPLINK_HWID := 0x07200104
endef
TARGET_DEVICES += tl-wr720n-v4

define Device/tl-wr740n-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR740N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WR741ND
  DEVICE_PROFILE := TLWR740
  TPLINK_HWID := 0x07400001
endef
TARGET_DEVICES += tl-wr740n-v1

define Device/tl-wr740n-v3
  $(Device/tl-wr740n-v1)
  DEVICE_MODEL := TL-WR740N/ND
  DEVICE_VARIANT := v3
  TPLINK_HWID := 0x07400003
endef
TARGET_DEVICES += tl-wr740n-v3

define Device/tl-wr740n-v4
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR740N/ND
  DEVICE_VARIANT := v4
  BOARDNAME := TL-WR741ND-v4
  DEVICE_PROFILE := TLWR740
  TPLINK_HWID := 0x07400004
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wr740n-v4

define Device/tl-wr740n-v5
  $(Device/tl-wr740n-v4)
  DEVICE_MODEL := TL-WR740N/ND
  DEVICE_VARIANT := v5
  TPLINK_HWID := 0x07400005
endef
TARGET_DEVICES += tl-wr740n-v5

define Device/tl-wr740n-v6
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR740N/ND
  DEVICE_VARIANT := v6
  BOARDNAME := TL-WR740N-v6
  DEVICE_PROFILE := TLWR740
  TPLINK_HWID := 0x07400006
endef
TARGET_DEVICES += tl-wr740n-v6

define Device/tl-wr741nd-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR741N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WR741ND
  DEVICE_PROFILE := TLWR741
  TPLINK_HWID := 0x07410001
endef
TARGET_DEVICES += tl-wr741nd-v1

define Device/tl-wr741nd-v2
  $(Device/tl-wr741nd-v1)
  DEVICE_MODEL := TL-WR741N/ND
  DEVICE_VARIANT := v2
endef
TARGET_DEVICES += tl-wr741nd-v2

define Device/tl-wr741nd-v4
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR741N/ND
  DEVICE_VARIANT := v4
  BOARDNAME := TL-WR741ND-v4
  DEVICE_PROFILE := TLWR741
  TPLINK_HWID := 0x07410004
  CONSOLE := ttyATH0,115200
endef
TARGET_DEVICES += tl-wr741nd-v4

define Device/tl-wr741nd-v5
  $(Device/tl-wr741nd-v4)
  DEVICE_MODEL := TL-WR741N/ND
  DEVICE_VARIANT := v5
  TPLINK_HWID := 0x07400005
endef
TARGET_DEVICES += tl-wr741nd-v5

define Device/tl-wr743nd-v1
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR743N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WR741ND
  DEVICE_PROFILE := TLWR743
  TPLINK_HWID := 0x07430001
endef
TARGET_DEVICES += tl-wr743nd-v1

define Device/tl-wr743nd-v2
  $(Device/tl-wr741nd-v4)
  DEVICE_MODEL := TL-WR743N/ND
  DEVICE_VARIANT := v2
  DEVICE_PROFILE := TLWR743
  TPLINK_HWID := 0x07430002
endef
TARGET_DEVICES += tl-wr743nd-v2

define Device/tl-wr802n-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR802N
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WR802N-v1
  DEVICE_PROFILE := TLWR802
  TPLINK_HWID := 0x08020001
  TPLINK_HWREV := 1
endef
TARGET_DEVICES += tl-wr802n-v1

define Device/tl-wr802n-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR802N
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WR802N-v2
  DEVICE_PROFILE := TLWR802
  TPLINK_HWID := 0x08020002
  TPLINK_HWREV := 2
  IMAGES += factory-us.bin factory-eu.bin
  IMAGE/factory-us.bin := append-rootfs | mktplinkfw factory -C US
  IMAGE/factory-eu.bin := append-rootfs | mktplinkfw factory -C EU
endef
TARGET_DEVICES += tl-wr802n-v2

define Device/tl-wr840n-v2
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR840N
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WR840N-v2
  DEVICE_PROFILE := TLWR840
  TPLINK_HWID := 0x08400002
  IMAGES += factory-eu.bin
  IMAGE/factory-eu.bin := append-rootfs | mktplinkfw factory -C EU
endef
TARGET_DEVICES += tl-wr840n-v2

define Device/tl-wr840n-v3
  $(Device/tl-wr840n-v2)
  DEVICE_MODEL := TL-WR840N
  DEVICE_VARIANT := v3
  BOARDNAME := TL-WR840N-v3
  TPLINK_HWID := 0x08400003
endef
TARGET_DEVICES += tl-wr840n-v3

define Device/tl-wr841-v1.5
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v1.5
  BOARDNAME := TL-WR841N-v1.5
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410002
  TPLINK_HWREV := 2
endef
TARGET_DEVICES += tl-wr841-v1.5

define Device/tl-wr841-v3
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v3
  BOARDNAME := TL-WR941ND
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410003
  TPLINK_HWREV := 3
endef
TARGET_DEVICES += tl-wr841-v3

define Device/tl-wr841-v5
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v5
  BOARDNAME := TL-WR741ND
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410005
endef
TARGET_DEVICES += tl-wr841-v5

define Device/tl-wr841-v7
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v7
  BOARDNAME := TL-WR841N-v7
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410007
endef
TARGET_DEVICES += tl-wr841-v7

define Device/tl-wr841-v8
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v8
  BOARDNAME := TL-WR841N-v8
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410008
endef
TARGET_DEVICES += tl-wr841-v8

define Device/tl-wr841-v9
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v9
  BOARDNAME := TL-WR841N-v9
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410009
endef
TARGET_DEVICES += tl-wr841-v9

define Device/tl-wr841-v10
  $(Device/tl-wr841-v9)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v10
  TPLINK_HWID := 0x08410010
endef
TARGET_DEVICES += tl-wr841-v10

define Device/tl-wr841-v11
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v11
  BOARDNAME := TL-WR841N-v11
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08410011
  IMAGES += factory-us.bin factory-eu.bin
  IMAGE/factory-us.bin := append-rootfs | mktplinkfw factory -C US
  IMAGE/factory-eu.bin := append-rootfs | mktplinkfw factory -C EU
endef
TARGET_DEVICES += tl-wr841-v11

define Device/tl-wr841-v12
  $(Device/tl-wr841-v11)
  DEVICE_MODEL := TL-WR841N/ND
  DEVICE_VARIANT := v12
  TPLINK_HWID := 0x08410012
endef
TARGET_DEVICES += tl-wr841-v12

define Device/tl-wr843nd-v1
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR843N/ND
  DEVICE_VARIANT := v1
  BOARDNAME := TL-WR841N-v8
  DEVICE_PROFILE := TLWR843
  TPLINK_HWID := 0x08430001
endef
TARGET_DEVICES += tl-wr843nd-v1

define Device/tl-wr847n-v8
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR847N/ND
  DEVICE_VARIANT := v8
  BOARDNAME := TL-WR841N-v8
  DEVICE_PROFILE := TLWR841
  TPLINK_HWID := 0x08470008
endef
TARGET_DEVICES += tl-wr847n-v8

define Device/tl-wr940n-v4
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR940N
  DEVICE_VARIANT := v4
  BOARDNAME := TL-WR940N-v4
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09400004
  IMAGES += factory-us.bin factory-eu.bin factory-br.bin
  IMAGE/factory-us.bin := append-rootfs | mktplinkfw factory -C US
  IMAGE/factory-eu.bin := append-rootfs | mktplinkfw factory -C EU
  IMAGE/factory-br.bin := append-rootfs | mktplinkfw factory -C BR
endef
TARGET_DEVICES += tl-wr940n-v4

define Device/tl-wr940n-v6
  $(Device/tl-wr940n-v4)
  DEVICE_MODEL := TL-WR940N
  DEVICE_VARIANT := v6
  BOARDNAME := TL-WR940N-v6
  TPLINK_HWID := 0x09400006
endef
TARGET_DEVICES += tl-wr940n-v6

define Device/tl-wr941nd-v2
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v2
  BOARDNAME := TL-WR941ND
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09410002
  TPLINK_HWREV := 2
endef
TARGET_DEVICES += tl-wr941nd-v2

define Device/tl-wr941nd-v3
  $(Device/tl-wr941nd-v2)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v3
endef
TARGET_DEVICES += tl-wr941nd-v3

define Device/rnx-n360rt
  $(Device/tl-wr941nd-v2)
  DEVICE_VENDOR := Rosewill
  DEVICE_MODEL := RNX-N360RT
  DEVICE_PROFILE := RNXN360RT
  TPLINK_HWREV := 0x00420001
endef
TARGET_DEVICES += rnx-n360rt

define Device/tl-wr941nd-v4
  $(Device/tplink-4m)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v4
  BOARDNAME := TL-WR741ND
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09410004
endef
TARGET_DEVICES += tl-wr941nd-v4

define Device/tl-wr941nd-v5
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v5
  BOARDNAME := TL-WR941ND-v5
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09410005
endef
TARGET_DEVICES += tl-wr941nd-v5

define Device/tl-wr941nd-v6
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v6
  BOARDNAME := TL-WR941ND-v6
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09410006
endef
TARGET_DEVICES += tl-wr941nd-v6

# Chinese version (unlike European) is similar to the TL-WDR3500
define Device/tl-wr941nd-v6-cn
  $(Device/tplink-4mlzma)
  DEVICE_MODEL := TL-WR941N/ND
  DEVICE_VARIANT := v6 (CN)
  BOARDNAME := TL-WDR3500
  DEVICE_PROFILE := TLWR941
  TPLINK_HWID := 0x09410006
endef
TARGET_DEVICES += tl-wr941nd-v6-cn
