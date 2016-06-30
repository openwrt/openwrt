define LegacyDevice/ALFAAP120C
	DEVICE_TITLE := ALFA Network AP120C board
	DEVICE_PACKAGES := uboot-envtools
endef
LEGACY_DEVICES += ALFAAP120C

define LegacyDevice/ALFAAP96
	DEVICE_TITLE := ALFA Network AP96 board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-rtc-pcf2123
endef
LEGACY_DEVICES += ALFAAP96

define LegacyDevice/HORNETUB
	DEVICE_TITLE := ALFA Network Hornet-UB board (8MB flash, 32MB ram)
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += HORNETUB

define LegacyDevice/HORNETUBx2
	DEVICE_TITLE := ALFA Network Hornet-UB-x2 board (16MB flash, 64MB ram)
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += HORNETUBx2

define LegacyDevice/ALFANX
	DEVICE_TITLE := ALFA Network N2/N5 board
endef
LEGACY_DEVICES += ALFANX

define LegacyDevice/TUBE2H
	DEVICE_TITLE := ALFA Network Tube2H board
endef
LEGACY_DEVICES += TUBE2H

define LegacyDevice/ALL0305
	DEVICE_TITLE := Allnet ALL0305
	DEVICE_PACKAGES := fconfig kmod-ath5k -kmod-ath9k
endef
LEGACY_DEVICES += ALL0305

define LegacyDevice/ALL0258N
	DEVICE_TITLE := Allnet ALL0258N
	DEVICE_PACKAGES := uboot-envtools rssileds
endef
LEGACY_DEVICES += ALL0258N

define LegacyDevice/ALL0315N
	DEVICE_TITLE := Allnet ALL0315N
	DEVICE_PACKAGES := uboot-envtools rssileds
endef
LEGACY_DEVICES += ALL0315N

define LegacyDevice/Yun
	DEVICE_TITLE := Arduino Yun based on Atheros AR9331
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += Yun

define LegacyDevice/AP113
	DEVICE_TITLE := Atheros AP113 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP113

define LegacyDevice/AP121
	DEVICE_TITLE := Atheros AP121 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP121

define LegacyDevice/AP121MINI
	DEVICE_TITLE := Atheros AP121-MINI reference board
endef
LEGACY_DEVICES += AP121MINI

define LegacyDevice/AP132
	DEVICE_TITLE := Atheros AP132 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP132

define LegacyDevice/AP135
	DEVICE_TITLE := Atheros AP135 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP135

define LegacyDevice/AP136
	DEVICE_TITLE := Atheros AP136 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP136

define LegacyDevice/AP143
	DEVICE_TITLE := Qualcomm Atheros AP143 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP143

define LegacyDevice/AP147
	DEVICE_TITLE := Qualcomm Atheros AP147 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP147

define LegacyDevice/AP152
	DEVICE_TITLE := Qualcomm Atheros AP152 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += AP152

define LegacyDevice/AP81
	DEVICE_TITLE := Atheros AP81 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP81

define LegacyDevice/AP83
	DEVICE_TITLE := Atheros AP83 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 \
		  vsc7385-ucode-ap83 vsc7395-ucode-ap83
endef
LEGACY_DEVICES += AP83

define LegacyDevice/AP96
	DEVICE_TITLE := Atheros AP96 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += AP96

define LegacyDevice/DB120
	DEVICE_TITLE := Atheros DB120 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += DB120

define LegacyDevice/PB42
	DEVICE_TITLE := Atheros PB42 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += PB42

define LegacyDevice/PB44
	DEVICE_TITLE := Atheros PB44 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 \
		  vsc7385-ucode-pb44 vsc7395-ucode-pb44
endef
LEGACY_DEVICES += PB44

define LegacyDevice/PB92
	DEVICE_TITLE := Atheros PB92 reference board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += PB92

define LegacyDevice/A02RBW300N
	DEVICE_TITLE := Atlantis-Land A02-RB-W300N
endef
LEGACY_DEVICES += A02RBW300N

define LegacyDevice/F9K1115V2
	DEVICE_TITLE := Belkin AC1750DB (F9K1115V2)
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb3 kmod-ledtrig-usbdev \
		kmod-ath10k
endef
LEGACY_DEVICES += F9K1115V2

define LegacyDevice/BXU2000N2
	DEVICE_TITLE := BHU BXU2000n-2
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += BXU2000N2

define LegacyDevice/WZRHPG300NH
	DEVICE_TITLE := Buffalo WZR-HP-G300NH
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZRHPG300NH

define LegacyDevice/WZRHPG300NH2
	DEVICE_TITLE := Buffalo WZR-HP-G300NH2
	DEVICE_PACKAGES := kmod-ath9k wpad-mini kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZRHPG300NH2

define LegacyDevice/WZRHPAG300H
	DEVICE_TITLE := Buffalo WZR-HP-AG300H
	DEVICE_PACKAGES := kmod-usb-ohci kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZRHPAG300H

define LegacyDevice/WZRHPG450H
	DEVICE_TITLE := Buffalo WZR-HP-G450H
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZRHPG450H

define LegacyDevice/WZR450HP2
	DEVICE_TITLE := Buffalo WZR-450HP2
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZR450HP2

define LegacyDevice/WZR600DHP
	DEVICE_TITLE := Buffalo WZR-600DHP
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WZR600DHP

define LegacyDevice/WHRG301N
	DEVICE_TITLE := Buffalo WHR-G301N
endef
LEGACY_DEVICES += WHRG301N

define LegacyDevice/WHRHPG300N
	DEVICE_TITLE := Buffalo WHR-HP-G300N
endef
LEGACY_DEVICES += WHRHPG300N

define LegacyDevice/WHRHPGN
	DEVICE_TITLE := Buffalo WHR-HP-GN
endef
LEGACY_DEVICES += WHRHPGN

define LegacyDevice/WLAEAG300N
	DEVICE_TITLE := Buffalo WLAE-AG300N
	DEVICE_PACKAGES := kmod-ledtrig-netdev
endef
LEGACY_DEVICES += WLAEAG300N

define LegacyDevice/WP543
	DEVICE_TITLE := Compex WP543/WPJ543
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WP543

define LegacyDevice/WPE72
	DEVICE_TITLE := Compex WPE72/WPE72NX
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += WPE72

define LegacyDevice/WPJ342
	DEVICE_TITLE := Compex WPJ342
endef
LEGACY_DEVICES += WPJ342

define LegacyDevice/WPJ344
	DEVICE_TITLE := Compex WPJ344
endef
LEGACY_DEVICES += WPJ344

define LegacyDevice/WPJ531
	DEVICE_TITLE := Compex WPJ531
endef
LEGACY_DEVICES += WPJ531

define LegacyDevice/WPJ558
	DEVICE_TITLE := Compex WPJ558
endef
LEGACY_DEVICES += WPJ558

define LegacyDevice/dLAN_Hotspot
	DEVICE_TITLE := devolo dLAN Hotspot
	DEVICE_PACKAGES := open-plc-utils open-plc-utils-chkpib open-plc-utils-getpib open-plc-utils-modpib \
		open-plc-utils-setpib open-plc-utils-hpavkey open-plc-utils-plchost \
		open-plc-utils-plctool open-plc-utils-pibdump open-plc-utils-plcstat \
		ebtables hostapd-utils wifitoggle wpad
endef
LEGACY_DEVICES += dLAN_Hotspot

define LegacyDevice/dLAN_pro_500_wp
	DEVICE_TITLE := devolo dLAN pro 500 Wireless+
	DEVICE_PACKAGES := open-plc-utils open-plc-utils-chkpib open-plc-utils-getpib open-plc-utils-modpib \
		open-plc-utils-setpib open-plc-utils-hpavkey open-plc-utils-amphost \
		open-plc-utils-plctool open-plc-utils-pibdump open-plc-utils-plcstat \
		ebtables hostapd-utils wifitoggle wpad
endef
LEGACY_DEVICES += dLAN_pro_500_wp

define LegacyDevice/dLAN_pro_1200_ac
	DEVICE_TITLE := devolo dLAN pro 1200+ WiFi ac
	DEVICE_PACKAGES := open-plc-utils open-plc-utils-chkpib open-plc-utils-getpib open-plc-utils-modpib \
		open-plc-utils-setpib open-plc-utils-hpavkey open-plc-utils-plchost \
		open-plc-utils-plctool open-plc-utils-pibdump open-plc-utils-plcstat \
		ebtables hostapd-utils wifitoggle wpad ip-full kmod-ath10k kmod-leds-gpio \
		kmod-ledtrig-gpio
endef
LEGACY_DEVICES += dLAN_pro_1200_ac

define LegacyDevice/DHP1565A1
	DEVICE_TITLE := D-Link DHP-1565 rev. A1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += DHP1565A1

define LegacyDevice/DIR505A1
	DEVICE_TITLE := D-Link DIR-505 rev. A1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += DIR505A1

define LegacyDevice/DIR600A1
	DEVICE_TITLE := D-Link DIR-600 rev. A1
endef
LEGACY_DEVICES += DIR600A1

define LegacyDevice/DIR601A1
	DEVICE_TITLE := D-Link DIR-601 rev. A1
endef
LEGACY_DEVICES += DIR601A1

define LegacyDevice/FR54RTR
	DEVICE_TITLE := Frys FR-54RTR
endef
LEGACY_DEVICES += FR54RTR

define LegacyDevice/DIR601B1
	DEVICE_TITLE := D-Link DIR-601 rev. B1
endef
LEGACY_DEVICES += DIR601B1

define LegacyDevice/DIR615C1
	DEVICE_TITLE := D-Link DIR-615 rev. C1
endef
LEGACY_DEVICES += DIR615C1

define LegacyDevice/DIR615E1
	DEVICE_TITLE := D-Link DIR-615 rev. E1
endef
LEGACY_DEVICES += DIR615E1

define LegacyDevice/DIR615E4
	DEVICE_TITLE := D-Link DIR-615 rev. E4
endef
LEGACY_DEVICES += DIR615E4

define LegacyDevice/DIR615IX
	DEVICE_TITLE := D-Link DIR-615 rev. I1
endef
LEGACY_DEVICES += DIR615IX

define LegacyDevice/DIR825B1
	DEVICE_TITLE := D-Link DIR-825 rev. B1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += DIR825B1

define LegacyDevice/DIR825C1
	DEVICE_TITLE := D-Link DIR-825 rev. C1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += DIR825C1

define LegacyDevice/DIR835A1
	DEVICE_TITLE := D-Link DIR-835 rev. A1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += DIR835A1

define LegacyDevice/DGL5500A1
	DEVICE_TITLE := D-Link DGL-5500 rev. A1
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ath10k
endef
LEGACY_DEVICES += DGL5500A1

define LegacyDevice/DRAGINO2
	DEVICE_TITLE := DRAGINO2
	DEVICE_PACKAGES := kmod-ath9k kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += DRAGINO2

define LegacyDevice/EAP300V2
	DEVICE_TITLE := EnGenius EAP300V2
endef
LEGACY_DEVICES += EAP300V2

define LegacyDevice/ESR900
	DEVICE_TITLE := EnGenius ESR900
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += ESR900

define LegacyDevice/ESR1750
        DEVICE_TITLE := EnGenius ESR1750
        DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k wpad-mini
endef
LEGACY_DEVICES += ESR1750

define LegacyDevice/EPG5000
        DEVICE_TITLE := EnGenius EPG5000
        DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k wpad-mini
endef
LEGACY_DEVICES += EPG5000

define LegacyDevice/EWDORIN
	DEVICE_TITLE := Embedded Wireless Dorin Platform
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev kmod-usb-storage \
		kmod-fs-vfat kmod-fs-msdos kmod-fs-ntfs kmod-fs-ext4 \
		kmod-nls-cp437 kmod-nls-cp850 kmod-nls-cp852 kmod-nls-iso8859-1 kmod-nls-utf8
endef
LEGACY_DEVICES += EWDORIN

define LegacyDevice/JA76PF
	DEVICE_TITLE := jjPlus JA76PF
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-hwmon-core kmod-i2c-core kmod-hwmon-lm75
endef
LEGACY_DEVICES += JA76PF

define LegacyDevice/JA76PF2
	DEVICE_TITLE := jjPlus JA76PF2
endef
LEGACY_DEVICES += JA76PF2

define LegacyDevice/JWAP003
	DEVICE_TITLE := jjPlus JWAP003
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += JWAP003

define LegacyDevice/WRT160NL
	DEVICE_TITLE := Linksys WRT160NL
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2
endef
LEGACY_DEVICES += WRT160NL

define LegacyDevice/WRT400N
	DEVICE_TITLE := Linksys WRT400N
endef
LEGACY_DEVICES += WRT400N

define LegacyDevice/MR12
	DEVICE_TITLE := Meraki MR12
	DEVICE_PACKAGES := kmod-spi-gpio kmod-ath9k
endef
LEGACY_DEVICES += MR12

define LegacyDevice/MR16
	DEVICE_TITLE := Meraki MR16
	DEVICE_PACKAGES := kmod-spi-gpio kmod-ath9k
endef
LEGACY_DEVICES += MR16

define LegacyDevice/SMART-300
        DEVICE_TITLE := NC-LINK SMART-300
endef
LEGACY_DEVICES += SMART-300

define LegacyDevice/WNDAP360
        DEVICE_TITLE := NETGEAR WNDAP360
endef
LEGACY_DEVICES += WNDAP360

define LegacyDevice/WNR2000V3
	DEVICE_TITLE := NETGEAR WNR2000V3
	DEVICE_PACKAGES :=  -kmod-usb-core -kmod-usb-ohci -kmod-usb2 -kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNR2000V3

define LegacyDevice/WNR2000V4
	DEVICE_TITLE := NETGEAR WNR2000V4
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNR2000V4

define LegacyDevice/WNR612V2
	DEVICE_TITLE := NETGEAR WNR612V2 / On Networks N150
endef
LEGACY_DEVICES += WNR612V2

define LegacyDevice/WNR1000V2
	DEVICE_TITLE := NETGEAR WNR1000V2
endef
LEGACY_DEVICES += WNR1000V2

define LegacyDevice/WNR2200
  DEVICE_TITLE := NETGEAR WNR2200
  DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNR2200

define LegacyDevice/WPN824N
	DEVICE_TITLE := NETGEAR WPN824N
endef
LEGACY_DEVICES += WPN824N

define LegacyDevice/OM5P
	DEVICE_TITLE := OpenMesh OM5P/OM5P-AN
	DEVICE_PACKAGES := kmod-ath9k om-watchdog
endef
LEGACY_DEVICES += OM5P

define LegacyDevice/OM5PAC
	DEVICE_TITLE := OpenMesh OM5P-AC/OM5P-ACv2
	DEVICE_PACKAGES := kmod-ath9k kmod-ath10k om-watchdog
endef
LEGACY_DEVICES += OM5PAC

define LegacyDevice/MR600
        DEVICE_TITLE := OpenMesh MR600
        DEVICE_PACKAGES := kmod-ath9k om-watchdog
endef
LEGACY_DEVICES += MR600

define LegacyDevice/MR900
        DEVICE_TITLE := OpenMesh MR900/MR900v2
        DEVICE_PACKAGES := kmod-ath9k om-watchdog
endef
LEGACY_DEVICES += MR900

define LegacyDevice/MR1750
        DEVICE_TITLE := OpenMesh MR1750/MR1750v2
        DEVICE_PACKAGES := kmod-ath9k kmod-ath10k
endef
LEGACY_DEVICES += MR1750

define LegacyDevice/OPENMESH
	DEVICE_TITLE := OpenMesh products
	DEVICE_PACKAGES := kmod-ath9k kmod-ath10k om-watchdog
endef
LEGACY_DEVICES += OPENMESH

define LegacyDevice/UBDEV01
	DEVICE_TITLE := PowerCloud Systems ubdev01 model
endef
LEGACY_DEVICES += UBDEV01

define LegacyDevice/DLRTDEV01
	DEVICE_TITLE := PowerCloud Systems dlrtdev01 model
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += DLRTDEV01

define LegacyDevice/MZKW04NU
	DEVICE_TITLE := Planex MZK-W04NU
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += MZKW04NU

define LegacyDevice/MZKW300NH
	DEVICE_TITLE := Planex MZK-W300NH
endef
LEGACY_DEVICES += MZKW300NH

define LegacyDevice/RW2458N
	DEVICE_TITLE := Redwave RW2458N
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-ath9k wpad-mini
endef
LEGACY_DEVICES += RW2458N

define LegacyDevice/CAP4200AG
	DEVICE_TITLE := Senao CAP4200AG
	DEVICE_PACKAGES := kmod-ath9k
endef
LEGACY_DEVICES += CAP4200AG

define LegacyDevice/EAP7660D
	DEVICE_TITLE := Senao EAP7660D
endef
LEGACY_DEVICES += EAP7660D

define LegacyDevice/WLR8100
        DEVICE_TITLE := Sitecom WLR-8100
        DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev kmod-usb3
endef
LEGACY_DEVICES += WLR8100

define LegacyDevice/TEW632BRP
	DEVICE_TITLE := TRENDNet TEW-632BRP
endef
LEGACY_DEVICES += TEW632BRP

define LegacyDevice/TEW652BRP
	DEVICE_TITLE := TRENDNet TEW-652BRP
endef
LEGACY_DEVICES += TEW652BRP

define LegacyDevice/TEW673GRU
	DEVICE_TITLE := TRENDNet TEW-673GRU
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2
endef
LEGACY_DEVICES += TEW673GRU

define LegacyDevice/TEW712BR
	DEVICE_TITLE := TRENDNet TEW-712BR
endef
LEGACY_DEVICES += TEW712BR

define LegacyDevice/TEW732BR
	DEVICE_TITLE := TRENDNet TEW-732BR
endef
LEGACY_DEVICES += TEW732BR

define LegacyDevice/TEW823DRU
	DEVICE_TITLE := TRENDNet TEW-823DRU
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-ath10k
endef
LEGACY_DEVICES += TEW823DRU

define LegacyDevice/DR344
	DEVICE_TITLE := Wallys DR344
endef
LEGACY_DEVICES += DR344

define LegacyDevice/MYNETREXT
	DEVICE_TITLE := WD My Net Wi-Fi Range Extender
	DEVICE_PACKAGES := rssileds
endef
LEGACY_DEVICES += MYNETREXT

define LegacyDevice/WRTNODE2Q
	DEVICE_TITLE := WRTnode2Q board
	DEVICE_PACKAGES := kmod-usb-core kmod-usb2 kmod-usb-storage
endef
LEGACY_DEVICES += WRTNODE2Q

define LegacyDevice/ZCN1523H28
	DEVICE_TITLE := Zcomax ZCN-1523H-2-8
endef
LEGACY_DEVICES += ZCN1523H28

define LegacyDevice/ZCN1523H516
	DEVICE_TITLE := Zcomax ZCN-1523H-5-16
endef
LEGACY_DEVICES += ZCN1523H516

define LegacyDevice/NBG_460N_550N_550NH
	DEVICE_TITLE := Zyxel NBG 460N/550N/550NH
	DEVICE_PACKAGES := kmod-rtc-pcf8563
endef
LEGACY_DEVICES += NBG_460N_550N_550NH

define LegacyDevice/R6100
	DEVICE_TITLE := NETGEAR R6100
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += R6100

define LegacyDevice/WNDR4300
	DEVICE_TITLE := NETGEAR WNDR3700v4/WNDR4300
	DEVICE_PACKAGES := kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef
LEGACY_DEVICES += WNDR4300

define LegacyDevice/NBG6716
	DEVICE_TITLE := Zyxel NBG 6716
	DEVICE_PACKAGES := kmod-rtc-pcf8563 kmod-ath10k
endef
LEGACY_DEVICES += NBG6716
