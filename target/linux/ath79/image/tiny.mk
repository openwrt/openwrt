include ./common-buffalo.mk
include ./common-nec.mk
include ./common-senao.mk

define Device/buffalo_whr-g301n
  $(Device/buffalo_common)
  SOC := ar7240
  DEVICE_MODEL := WHR-G301N
  BUFFALO_PRODUCT := WHR-G301N
  IMAGE_SIZE := 3712k
  SUPPORTED_DEVICES += whr-g301n
  DEFAULT := n
endef
TARGET_DEVICES += buffalo_whr-g301n

define Device/dlink_dap-1720-a1
  $(Device/seama)
  SOC := qca9563
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DAP-1720
  DEVICE_VARIANT := A1
  IMAGE_SIZE := 15872k
  SEAMA_SIGNATURE := wapac28_dlink.2015_dap1720
  DEVICE_PACKAGES := -swconfig ath10k-firmware-qca988x-ct \
	kmod-ath10k-ct-smallbuffers rssileds
endef
TARGET_DEVICES += dlink_dap-1720-a1

define Device/dlink_dir-615-e4
  SOC := ar7240
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-615
  DEVICE_VARIANT := E4
  IMAGE_SIZE := 3776k
  FACTORY_IMAGE_SIZE := 3456k
  IMAGES += factory.bin
  IMAGE/default := append-kernel | append-rootfs | pad-rootfs
  IMAGE/sysupgrade.bin := $$(IMAGE/default) | check-size | append-metadata
  IMAGE/factory.bin := $$(IMAGE/default) | \
	check-size $$$$(FACTORY_IMAGE_SIZE) | pad-to $$$$(FACTORY_IMAGE_SIZE) | \
	append-string "AP99-AR7240-RT-091105-05"
  SUPPORTED_DEVICES += dir-615-e4
  DEFAULT := n
endef
TARGET_DEVICES += dlink_dir-615-e4

define Device/dlink_dir-859-a
  $(Device/seama)
  SOC := qca9563
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-859
  IMAGE_SIZE := 15872k
  SEAMA_SIGNATURE := wrgac37_dlink.2013gui_dir859
  DEVICE_PACKAGES := ath10k-firmware-qca988x-ct kmod-ath10k-ct-smallbuffers
endef

define Device/dlink_dir-859-a1
  $(Device/dlink_dir-859-a)
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += dlink_dir-859-a1

define Device/dlink_dir-859-a3
  $(Device/dlink_dir-859-a)
  DEVICE_VARIANT := A3
endef
TARGET_DEVICES += dlink_dir-859-a3

define Device/dlink_dir-869-a1
  $(Device/seama)
  SOC := qca9563
  DEVICE_VENDOR := D-Link
  DEVICE_MODEL := DIR-869
  DEVICE_VARIANT := A1
  IMAGE_SIZE := 15872k
  SEAMA_SIGNATURE := wrgac54_dlink.2015_dir869
  DEVICE_PACKAGES := ath10k-firmware-qca988x-ct kmod-ath10k-ct-smallbuffers
  SUPPORTED_DEVICES += dir-869-a1
endef
TARGET_DEVICES += dlink_dir-869-a1

define Device/engenius_eap350-v1
  $(Device/senao_loader_okli)
  BLOCKSIZE := 4k
  SOC := ar7242
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := EAP350
  DEVICE_VARIANT := v1
  IMAGE_SIZE := 4928k
  LOADER_FLASH_OFFS := 0x1a0000
  SENAO_IMGNAME := senao-eap350
  DEFAULT := n
endef
TARGET_DEVICES += engenius_eap350-v1

define Device/engenius_ecb350-v1
  $(Device/senao_loader_okli)
  BLOCKSIZE := 4k
  SOC := ar7242
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := ECB350
  DEVICE_VARIANT := v1
  IMAGE_SIZE := 4928k
  LOADER_FLASH_OFFS := 0x1a0000
  SENAO_IMGNAME := senao-ecb350
  DEFAULT := n
endef
TARGET_DEVICES += engenius_ecb350-v1

define Device/engenius_enh202-v1
  $(Device/senao_loader_okli)
  SOC := ar7240
  DEVICE_VENDOR := EnGenius
  DEVICE_MODEL := ENH202
  DEVICE_VARIANT := v1
  DEVICE_PACKAGES := rssileds
  IMAGE_SIZE := 4928k
  LOADER_FLASH_OFFS := 0x1a0000
  SENAO_IMGNAME := senao-enh202
  DEFAULT := n
endef
TARGET_DEVICES += engenius_enh202-v1

define Device/nec_wf1200hp
  DEVICE_MODEL := Aterm WF1200HP
  SOC := ar9344
  BLOCKSIZE := 4k
  IMAGE_SIZE := 7936k
  NEC_FW_TYPE := H047
  $(Device/nec-netbsd-aterm)
  DEVICE_PACKAGES := kmod-ath10k-ct-smallbuffers ath10k-firmware-qca988x-ct \
	-uboot-envtools
  UBOOT_PATH := $$(STAGING_DIR_IMAGE)/$$(SOC)_nec_aterm_fe-u-boot.bin
endef
TARGET_DEVICES += nec_wf1200hp

define Device/nec_wg600hp
  DEVICE_MODEL := Aterm WG600HP
  SOC := ar9344
  BLOCKSIZE := 4k
  IMAGE_SIZE := 7936k
  NEC_FW_TYPE := H044
  $(Device/nec-netbsd-aterm)
endef
TARGET_DEVICES += nec_wg600hp

define Device/nec_wr8750n
  SOC := ar9344
  DEVICE_MODEL := Aterm WR8750N
  BLOCKSIZE := 4k
  IMAGE_SIZE := 7936k
  NEC_FW_TYPE := H033a
  $(Device/nec-netbsd-aterm)
endef
TARGET_DEVICES += nec_wr8750n

define Device/nec_wr9500n
  SOC := ar9344
  DEVICE_MODEL := Aterm WR9500N
  BLOCKSIZE := 4k
  IMAGE_SIZE := 16128k
  NEC_FW_TYPE := H033
  $(Device/nec-netbsd-aterm)
endef
TARGET_DEVICES += nec_wr9500n

define Device/pqi_air-pen
  SOC := ar9330
  DEVICE_VENDOR := PQI
  DEVICE_MODEL := Air-Pen
  DEVICE_PACKAGES := kmod-usb-chipidea2
  IMAGE_SIZE := 7680k
  SUPPORTED_DEVICES += pqi-air-pen
  DEFAULT := n
endef
TARGET_DEVICES += pqi_air-pen
