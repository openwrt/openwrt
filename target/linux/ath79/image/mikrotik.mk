include ./common-mikrotik.mk

define Device/mikrotik_routerboard-2011uias-2hnd
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 2011UiAS-2HnD
  DEVICE_PACKAGES += kmod-usb2 kmod-i2c-gpio kmod-sfp kmod-gpio-beeper
  SUPPORTED_DEVICES += rb-2011l rb-2011il rb-2011ils \
	rb-2011uas rb-2011uas-2hnd rb-2011uias \
	rb-2011uias-2hnd rb-2011uias-2hnd-r2
endef
TARGET_DEVICES += mikrotik_routerboard-2011uias-2hnd

define Device/mikrotik_routerboard-493g
  $(Device/mikrotik_nand)
  SOC := ar7161
  DEVICE_MODEL := RouterBOARD 493G
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb2
  SUPPORTED_DEVICES += rb-493g
endef
TARGET_DEVICES += mikrotik_routerboard-493g

define Device/mikrotik_routerboard-750-r2
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD 750 r2 (hEX lite)
  IMAGE_SIZE := 16256k
  SUPPORTED_DEVICES += rb-750-r2
endef
TARGET_DEVICES += mikrotik_routerboard-750-r2

define Device/mikrotik_routerboard-911-lite
  $(Device/mikrotik_nor)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 911 Lite2/Lite5 (2Hn/5Hn)
  DEVICE_PACKAGES += rssileds
  IMAGE_SIZE := 16256k
  SUPPORTED_DEVICES += rb-911-2hn rb-911-5hn
endef
TARGET_DEVICES += mikrotik_routerboard-911-lite

define Device/mikrotik_routerboard-911g-xhpnd
  $(Device/mikrotik_nand)
  SOC := ar9342
  DEVICE_MODEL := RouterBOARD 911G-2HPnD
  DEVICE_ALT0_VENDOR := Mikrotik
  DEVICE_ALT0_MODEL := RouterBOARD 911G-5HPnD
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += rb-912uag-2hpnd
endef
TARGET_DEVICES += mikrotik_routerboard-911g-xhpnd

define Device/mikrotik_routerboard-912uag-2hpnd
  $(Device/mikrotik_nand)
  SOC := ar9342
  DEVICE_MODEL := RouterBOARD 912UAG-2HPnD
  DEVICE_ALT0_VENDOR := Mikrotik
  DEVICE_ALT0_MODEL := RouterBOARD 912UAG-5HPnD
  DEVICE_PACKAGES += kmod-usb-ehci kmod-usb2 rssileds
  SUPPORTED_DEVICES += rb-912uag-2hpnd
endef
TARGET_DEVICES += mikrotik_routerboard-912uag-2hpnd

define Device/mikrotik_routerboard-911g-5hpacd
  $(Device/mikrotik_nand)
  SOC := qca9558
  DEVICE_MODEL := RouterBOARD 911G-5HPacD
  DEVICE_PACKAGES += kmod-ath10k-ct ath10k-firmware-qca988x-ct
  SUPPORTED_DEVICES += rb-921gs-5hpacd-r2
endef
TARGET_DEVICES += mikrotik_routerboard-911g-5hpacd

define Device/mikrotik_routerboard-921gs-5hpacd-15s
  $(Device/mikrotik_nand)
  SOC := qca9558
  DEVICE_MODEL := RouterBOARD 921GS-5HPacD-15s (mANTBox 15s)
  DEVICE_PACKAGES += kmod-ath10k-ct ath10k-firmware-qca988x-ct kmod-i2c-gpio \
	kmod-sfp
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

define Device/mikrotik_routerboard-951g-2hnd
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 951G-2HnD
  DEVICE_PACKAGES += kmod-gpio-beeper kmod-usb-ohci kmod-usb2
  SUPPORTED_DEVICES += rb-951g-2hnd
endef
TARGET_DEVICES += mikrotik_routerboard-951g-2hnd

define Device/mikrotik_routerboard-951ui-2hnd
  $(Device/mikrotik_nand)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD 951Ui-2HnD
  DEVICE_PACKAGES += kmod-usb-ohci kmod-usb2
  SUPPORTED_DEVICES += rb-951ui-2hnd
endef
TARGET_DEVICES += mikrotik_routerboard-951ui-2hnd

define Device/mikrotik_routerboard-951ui-2nd
  $(Device/mikrotik_nor)
  SOC := qca9531
  DEVICE_MODEL := RouterBOARD 951Ui-2nD (hAP)
  IMAGE_SIZE := 16256k
  SUPPORTED_DEVICES += rb-951ui-2nd
endef
TARGET_DEVICES += mikrotik_routerboard-951ui-2nd

define Device/mikrotik_routerboard-952ui-5ac2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD 952Ui-5ac2nD (hAP ac lite)
  DEVICE_PACKAGES += kmod-ath10k-ct-smallbuffers ath10k-firmware-qca9887-ct
  IMAGE_SIZE := 16256k
  SUPPORTED_DEVICES += rb-952ui-5ac2nd
endef
TARGET_DEVICES += mikrotik_routerboard-952ui-5ac2nd

define Device/mikrotik_routerboard-962uigs-5hact2hnt
  $(Device/mikrotik_nor)
  SOC := qca9558
  DEVICE_MODEL := RouterBOARD 962UiGS-5HacT2HnT (hAP ac)
  DEVICE_PACKAGES += kmod-ath10k-ct ath10k-firmware-qca988x-ct kmod-usb2 \
	kmod-gpio-beeper kmod-i2c-gpio kmod-sfp
  IMAGE_SIZE := 16256k
  SUPPORTED_DEVICES += rb-962uigs-5hact2hnt
endef
TARGET_DEVICES += mikrotik_routerboard-962uigs-5hact2hnt

define Device/mikrotik_routerboard-lhg-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD LHG 2nD (LHG 2)
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-lhg-2nd

define Device/mikrotik_routerboard-lhg-5nd
  $(Device/mikrotik_nor)
  SOC := ar9344
  DEVICE_MODEL := RouterBOARD LHG 5nD (LHG 5)
  DEVICE_PACKAGES += rssileds
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-lhg-5nd

define Device/mikrotik_routerboard-map-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD mAP-2nD (mAP)
  DEVICE_PACKAGES += kmod-usb2 kmod-ledtrig-gpio
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-map-2nd

define Device/mikrotik_routerboard-mapl-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD mAPL-2nD (mAP lite)
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-mapl-2nd

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
  DEVICE_PACKAGES += kmod-usb2 rssileds
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-wapr-2nd

define Device/mikrotik_routerboard-wap-2nd
  $(Device/mikrotik_nor)
  SOC := qca9533
  DEVICE_MODEL := RouterBOARD wAP-2nD (wAP)
  IMAGE_SIZE := 16256k
endef
TARGET_DEVICES += mikrotik_routerboard-wap-2nd
