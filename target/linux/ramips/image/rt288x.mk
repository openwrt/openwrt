#
# RT288X Profiles
#

define Build/gemtek-header
	mkheader_gemtek $@ $@.new $(1) && mv $@.new $@
endef

define Build/airlink-header
	mkwrgimg -i $@ \
		-d "/dev/mtdblock/2" \
		-s "wrgn16a_airlink_ar670w" \
		-o $@.new && mv $@.new $@
endef

define Device/ar670w
  DTS := AR670W
  DEVICE_TITLE := Airlink AR670W
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  KERNEL := $(KERNEL_DTB)
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | gemtek-header ar725w
endef
TARGET_DEVICES += ar725w

define Device/ar725w
  DTS := AR725W
  DEVICE_TITLE := Gemtek AR725W
  IMAGES += factory.bin
  IMAGE/factory.bin := $$(IMAGE/sysupgrade.bin) | gemtek-header ar725w
endef
TARGET_DEVICES += ar725w

belkin_f5d8235v1_mtd_size=7929856
define Device/f5d8235v1
  DTS := F5D8235_V1
  IMAGE_SIZE := $(belkin_f5d8235v1_mtd_size)
  DEVICE_TITLE := Belkin F5D8235 V1
  DEVICE_PACKAGES := kmod-switch-rtl8366s
endef
TARGET_DEVICES += f5d8235v1

define Device/rt-n15
  DTS := RT-N15
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Asus RT-N15
  DEVICE_PACKAGES := kmod-switch-rtl8366s
endef
TARGET_DEVICES += rt-n15

define Device/v11st-fe
  DTS := V11STFE
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Ralink V11ST-FE
endef
TARGET_DEVICES += v11st-fe

define Device/wli-tx4-ag300n
  DTS := WLI-TX4-AG300N
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Buffalo WLI-TX4-AG300N
endef
TARGET_DEVICES += wli-tx4-ag300n

define Device/wzr-agl300nh
  DTS := WZR-AGL300NH
  IMAGE_SIZE := $(ralink_default_fw_size_4M)
  DEVICE_TITLE := Buffalo WZR-AGL300NH
  DEVICE_PACKAGES := kmod-switch-rtl8366s
endef
TARGET_DEVICES += wzr-agl300nh
