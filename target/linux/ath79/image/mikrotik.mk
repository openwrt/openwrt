include ./common-mikrotik.mk

define Device/mikrotik_routerboard-493g
  $(Device/mikrotik_nand)
  SOC := ar7161
  DEVICE_MODEL := RouterBOARD 493G
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb2
  SUPPORTED_DEVICES += rb-493g
endef
TARGET_DEVICES += mikrotik_routerboard-493g

define Device/mikrotik_routerboard-921gs-5hpacd-15s
  $(Device/mikrotik_nand)
  SOC := qca9558
  DEVICE_MODEL := RouterBOARD 921GS-5HPacD-15s (mANTBox 15s)
  DEVICE_PACKAGES += kmod-ath10k-ct ath10k-firmware-qca988x-ct
  SUPPORTED_DEVICES += rb-921gs-5hpacd-r2
endef
TARGET_DEVICES += mikrotik_routerboard-921gs-5hpacd-15s

define Device/mikrotik_routerboard-922uags-5hpacd
  $(Device/mikrotik_nand)
  SOC := qca9558
  DEVICE_MODEL := RouterBOARD 922UAGS-5HPacD
  DEVICE_PACKAGES += kmod-ath10k-ct ath10k-firmware-qca988x-ct kmod-usb2 \
	kmod-i2c-gpio kmod-sfp
  SUPPORTED_DEVICES += rb-922uags-5hpacd
endef
TARGET_DEVICES += mikrotik_routerboard-922uags-5hpacd

define Device/mikrotik_routerboard-2011il
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011iL
  SUPPORTED_DEVICES += rb-2011il
endef
TARGET_DEVICES += mikrotik_routerboard-2011il

define Device/mikrotik_routerboard-2011ils
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011iLS
  DEVICE_PACKAGES += kmod-i2c-gpio
  SUPPORTED_DEVICES += rb-2011ils
endef
TARGET_DEVICES += mikrotik_routerboard-2011ils

define Device/mikrotik_routerboard-2011l
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011L
  SUPPORTED_DEVICES += rb-2011l
endef
TARGET_DEVICES += mikrotik_routerboard-2011l

define Device/mikrotik_routerboard-2011ls
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011LS
  DEVICE_PACKAGES += kmod-i2c-gpio
  SUPPORTED_DEVICES += rb-2011ls
endef
TARGET_DEVICES += mikrotik_routerboard-2011ls

define Device/mikrotik_routerboard-2011uas
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011UAS
  DEVICE_PACKAGES += kmod-i2c-gpio kmod-usb2
  SUPPORTED_DEVICES += rb-2011uas
endef
TARGET_DEVICES += mikrotik_routerboard-2011uas

define Device/mikrotik_routerboard-2011uas-2hnd
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011UAS-2HnD
  DEVICE_PACKAGES += kmod-i2c-gpio kmod-usb2
  SUPPORTED_DEVICES += rb-2011uas-2hnd
endef
TARGET_DEVICES += mikrotik_routerboard-2011uas-2hnd

define Device/mikrotik_routerboard-2011uias
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011UiAS
  DEVICE_PACKAGES += kmod-i2c-gpio kmod-usb2
  SUPPORTED_DEVICES += rb-2011uias
endef
TARGET_DEVICES += mikrotik_routerboard-2011uias

define Device/mikrotik_routerboard-2011uias-2hnd
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011UiAS-2HnD
  DEVICE_PACKAGES += kmod-i2c-gpio kmod-usb2
  SUPPORTED_DEVICES += rb-2011uias-2hnd
endef
TARGET_DEVICES += mikrotik_routerboard-2011uias-2hnd

define Device/mikrotik_routerboard-lhg-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD LHG 2nD (LHG 2)
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-lhg-2nd

define Device/mikrotik_routerboard-sxt-5nd-r2
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD SXT 5nD r2 (SXT Lite5)
  DEVICE_PACKAGES += rssileds kmod-gpio-beeper
  SUPPORTED_DEVICES += rb-sxt5n
endef
TARGET_DEVICES += mikrotik_routerboard-sxt-5nd-r2

define Device/mikrotik_routerboard-wap-g-5hact2hnd
  $(Device/mikrotik_nor)
  SOC := qca9556
  DEVICE_MODEL := RouterBOARD wAP G-5HacT2HnD (wAP AC)
  IMAGE_SIZE := 16256k
  DEVICE_PACKAGES += kmod-ath10k-ct-smallbuffers ath10k-firmware-qca988x-ct
  SUPPORTED_DEVICES += rb-wapg-5hact2hnd
endef
TARGET_DEVICES += mikrotik_routerboard-wap-g-5hact2hnd

define Device/mikrotik_routerboard-wapr-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD wAPR-2nD (wAP R)
  DEVICE_PACKAGES += rssileds
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-wapr-2nd
