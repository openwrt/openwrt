#
# AN7552 Profile
#

define Device/an7552-evb
  $(Device/FitImage)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7552 Evaluation Board
endef
TARGET_DEVICES += an7552-evb

define Device/an7563-evb
  $(Device/FitImage)
  DEVICE_VENDOR := Airoha
  DEVICE_MODEL := AN7563 Evaluation Board
endef
TARGET_DEVICES += an7563-evb
