TRX_ENDIAN := le

define Device/quantum_c6500xk-v1
  $(Device/FitImageVmlinuz)
  DEVICE_VENDOR := Quantum
  DEVICE_MODEL := C6500XK
  DEVICE_VARIANT := 1.0

  DEVICE_ALT0_VENDOR := Axon
  DEVICE_ALT0_MODEL := C6500XK
  DEVICE_ALT0_VARIANT := 1.0

  DEVICE_PACKAGES += omci-agent kmod-phy-aquantia
endef
TARGET_DEVICES += quantum_c6500xk-v1
