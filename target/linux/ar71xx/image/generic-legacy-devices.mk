define LegacyDevice/A60
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := A40/A60
  DEVICE_PACKAGES := om-watchdog kmod-ath10k-ct ath10k-firmware-qca988x-ct \
    mod-usb-core kmod-usb2
endef
LEGACY_DEVICES += A60

define LegacyDevice/ALFANX
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := N2/N5 board
endef
LEGACY_DEVICES += ALFANX

define LegacyDevice/HORNETUB
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := Hornet-UB board
  DEVICE_VARIANT := 8MB flash, 32MB ram
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += HORNETUB

define LegacyDevice/TUBE2H8M
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := Tube2H board
  DEVICE_VARIANT := 8MB flash
endef
LEGACY_DEVICES += TUBE2H8M

define LegacyDevice/AP96
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP96 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP96

define LegacyDevice/WNDAP360
  DEVICE_VENDOR := Netgear
  DEVICE_MODEL := WNDAP360
endef
LEGACY_DEVICES += WNDAP360

define LegacyDevice/ALFAAP120C
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := AP120C board
endef
LEGACY_DEVICES += ALFAAP120C

define LegacyDevice/ALFAAP96
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := AP96 board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-rtc-pcf2123
endef
LEGACY_DEVICES += ALFAAP96

define LegacyDevice/ALL0258N
  DEVICE_VENDOR := Allnet
  DEVICE_MODEL := ALL0258N
  DEVICE_PACKAGES := rssileds
endef
LEGACY_DEVICES += ALL0258N

define LegacyDevice/ALL0315N
  DEVICE_VENDOR := Allnet
  DEVICE_MODEL := ALL0315N
  DEVICE_PACKAGES := rssileds
endef
LEGACY_DEVICES += ALL0315N

define LegacyDevice/AP121_8M
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP121 reference board
  DEVICE_VARIANT := 8MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP121_8M

define LegacyDevice/AP121_16M
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP121 reference board
  DEVICE_VARIANT := 16MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP121_16M

define LegacyDevice/AP132
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP132 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP132

define LegacyDevice/AP135
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP135 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP135

define LegacyDevice/AP136_010
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP136-010 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP136_010

define LegacyDevice/AP136_020
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := AP136-020 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP136_020

define LegacyDevice/AP143_8M
  DEVICE_VENDOR := Qualcomm Atheros
  DEVICE_MODEL := AP143 reference board
  DEVICE_VARIANT := 8MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP143_8M

define LegacyDevice/AP143_16M
  DEVICE_VENDOR := Qualcomm Atheros
  DEVICE_MODEL := AP143 reference board
  DEVICE_VARIANT := 16MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP143_16M

define LegacyDevice/AP147_010
  DEVICE_VENDOR := Qualcomm Atheros
  DEVICE_MODEL := AP147-010 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP147_010

define LegacyDevice/AP152_16M
  DEVICE_VENDOR := Qualcomm Atheros
  DEVICE_MODEL := AP152 reference board
  DEVICE_VARIANT := 16MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP152_16M

define LegacyDevice/BXU2000N2
  DEVICE_VENDOR := BHU
  DEVICE_MODEL := BXU2000n-2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += BXU2000N2

define LegacyDevice/CAP4200AG
  DEVICE_VENDOR := Senao
  DEVICE_MODEL := CAP4200AG
endef
LEGACY_DEVICES += CAP4200AG

define LegacyDevice/DB120
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := DB120 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += DB120

define LegacyDevice/HORNETUBx2
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := Hornet-UB-x2 board
  DEVICE_VARIANT := 16MB flash, 64MB ram
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += HORNETUBx2

define LegacyDevice/TUBE2H16M
  DEVICE_VENDOR := ALFA Network
  DEVICE_MODEL := Tube2H board
  DEVICE_VARIANT := 16MB flash
endef
LEGACY_DEVICES += TUBE2H16M

define LegacyDevice/DIR505A1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-505
  DEVICE_VARIANT := rev. A1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += DIR505A1

define LegacyDevice/DGL5500A1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DGL-5500
  DEVICE_VARIANT := rev. A1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += DGL5500A1

define LegacyDevice/TEW823DRU
  DEVICE_VENDOR := TRENDnet
  DEVICE_MODEL := TEW-823DRU
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += TEW823DRU

define LegacyDevice/DHP1565A1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DHP-1565
  DEVICE_VARIANT := rev. A1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += DHP1565A1

define LegacyDevice/DIR825C1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-825
  DEVICE_VARIANT := rev. C1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += DIR825C1

define LegacyDevice/DIR835A1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-835
  DEVICE_VARIANT := rev. A1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += DIR835A1

define LegacyDevice/TEW732BR
  DEVICE_VENDOR := TRENDnet
  DEVICE_MODEL := TEW-732BR
endef
LEGACY_DEVICES += TEW732BR

define LegacyDevice/WRT160NL
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := WRT160NL
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += WRT160NL

define LegacyDevice/MYNETREXT
  DEVICE_VENDOR := Western Digital
  DEVICE_MODEL := My Net Wi-Fi Range Extender
  DEVICE_PACKAGES := rssileds
endef
LEGACY_DEVICES += MYNETREXT

define LegacyDevice/DIR825B1
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-825
  DEVICE_VARIANT := rev. B1
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += DIR825B1

define LegacyDevice/TEW673GRU
  DEVICE_VENDOR := TRENDnet
  DEVICE_MODEL := TEW-673GRU
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += TEW673GRU

define LegacyDevice/DLRTDEV01
  DEVICE_VENDOR := PowerCloud Systems
  DEVICE_MODEL := dlrtdev01 model
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += DLRTDEV01

define LegacyDevice/dLAN_Hotspot
  DEVICE_VENDOR := Devolo
  DEVICE_MODEL := dLAN Hotspot
endef
LEGACY_DEVICES += dLAN_Hotspot

define LegacyDevice/dLAN_pro_500_wp
  DEVICE_VENDOR := Devolo
  DEVICE_MODEL := dLAN pro 500 Wireless+
endef
LEGACY_DEVICES += dLAN_pro_500_wp

define LegacyDevice/dLAN_pro_1200_ac
  DEVICE_VENDOR := Devolo
  DEVICE_MODEL := dLAN pro 1200+ WiFi ac
  DEVICE_PACKAGES := kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += dLAN_pro_1200_ac

define LegacyDevice/ESR900
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := ESR900
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += ESR900

define LegacyDevice/ESR1750
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := ESR1750
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += ESR1750

define LegacyDevice/EPG5000
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := EPG5000
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += EPG5000

define LegacyDevice/WP543_8M
  DEVICE_VENDOR := Compex
  DEVICE_MODEL := WP543/WPJ543
  DEVICE_VARIANT := 8MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WP543_8M

define LegacyDevice/WP543_16M
  DEVICE_VENDOR := Compex
  DEVICE_MODEL := WP543/WPJ543
  DEVICE_VARIANT := 16MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WP543_16M

define LegacyDevice/WPE72_8M
  DEVICE_VENDOR := Compex
  DEVICE_MODEL := WPE72/WPE72NX
  DEVICE_VARIANT := 8MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WPE72_8M

define LegacyDevice/WPE72_16M
  DEVICE_VENDOR := Compex
  DEVICE_MODEL := WPE72/WPE72NX
  DEVICE_VARIANT := 16MB flash
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WPE72_16M

define LegacyDevice/WNR2200
  DEVICE_VENDOR := Netgear
  DEVICE_MODEL := WNR2200
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WNR2200

define LegacyDevice/OM2P
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := OM2P/OM2P-HS/OM2P-LC
  DEVICE_PACKAGES := om-watchdog
endef
LEGACY_DEVICES += OM2P

define LegacyDevice/OM5P
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := OM5P/OM5P-AN
  DEVICE_PACKAGES := om-watchdog
endef
LEGACY_DEVICES += OM5P

define LegacyDevice/OM5PAC
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := OM5P-AC
  DEVICE_PACKAGES := kmod-ath10k-ct ath10k-firmware-qca988x-ct om-watchdog
endef
LEGACY_DEVICES += OM5PAC

define LegacyDevice/MR600
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := MR600
  DEVICE_PACKAGES := om-watchdog
endef
LEGACY_DEVICES += MR600

define LegacyDevice/MR900
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := MR900
  DEVICE_PACKAGES := om-watchdog
endef
LEGACY_DEVICES += MR900

define LegacyDevice/MR1750
  DEVICE_VENDOR := Open-Mesh
  DEVICE_MODEL := MR1750
  DEVICE_PACKAGES := om-watchdog kmod-ath10k-ct ath10k-firmware-qca988x-ct
endef
LEGACY_DEVICES += MR1750

define LegacyDevice/ALL0305
  DEVICE_VENDOR := Allnet
  DEVICE_MODEL := ALL0305
  DEVICE_PACKAGES := fconfig kmod-ath5k -kmod-ath9k
endef
LEGACY_DEVICES += ALL0305

define LegacyDevice/EAP7660D
  DEVICE_VENDOR := Senao
  DEVICE_MODEL := EAP7660D
endef
LEGACY_DEVICES += EAP7660D

define LegacyDevice/JA76PF
  DEVICE_VENDOR := jjPlus
  DEVICE_MODEL := JA76PF
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-hwmon-core kmod-i2c-core kmod-hwmon-lm75
endef
LEGACY_DEVICES += JA76PF

define LegacyDevice/JA76PF2
  DEVICE_VENDOR := jjPlus
  DEVICE_MODEL := JA76PF2
endef
LEGACY_DEVICES += JA76PF2

define LegacyDevice/JWAP003
  DEVICE_VENDOR := jjPlus
  DEVICE_MODEL := JWAP003
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += JWAP003

define LegacyDevice/PB42
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := PB42 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += PB42

define LegacyDevice/PB44
  DEVICE_VENDOR := Atheros
  DEVICE_MODEL := PB44 reference board
  DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
	  vsc7385-ucode-pb44 vsc7395-ucode-pb44
endef
LEGACY_DEVICES += PB44

define LegacyDevice/MZKW04NU
  DEVICE_VENDOR := Planex
  DEVICE_MODEL := MZK-W04NU
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += MZKW04NU

define LegacyDevice/MZKW300NH
  DEVICE_VENDOR := Planex
  DEVICE_MODEL := MZK-W300NH
endef
LEGACY_DEVICES += MZKW300NH

define LegacyDevice/EAP300V2
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := EAP300V2
endef
LEGACY_DEVICES += EAP300V2

define LegacyDevice/WRT400N
  DEVICE_VENDOR := Linksys
  DEVICE_MODEL := WRT400N
endef
LEGACY_DEVICES += WRT400N

define LegacyDevice/WZRHPG300NH
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-HP-G300NH
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZRHPG300NH

define LegacyDevice/WZRHPG300NH2
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-HP-G300NH2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZRHPG300NH2

define LegacyDevice/WZRHPAG300H
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-HP-AG300H
  DEVICE_PACKAGES := kmod-usb-ohci kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZRHPAG300H

define LegacyDevice/WZRHPG450H
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-HP-G450H
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZRHPG450H

define LegacyDevice/WZR600DHP
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-600DHP
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZR600DHP

define LegacyDevice/WZR450HP2
  DEVICE_VENDOR := Buffalo
  DEVICE_MODEL := WZR-450HP2
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-ledtrig-usbport
endef
LEGACY_DEVICES += WZR450HP2

define LegacyDevice/ZCN1523H28
  DEVICE_VENDOR := Zcomax
  DEVICE_MODEL := ZCN-1523H-2-8
endef
LEGACY_DEVICES += ZCN1523H28

define LegacyDevice/ZCN1523H516
  DEVICE_VENDOR := Zcomax
  DEVICE_MODEL := ZCN-1523H-5-16
endef
LEGACY_DEVICES += ZCN1523H516
