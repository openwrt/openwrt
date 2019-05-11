ifeq ($(SUBTARGET),cortexa53)

define Device/globalscale_espressobin
  $(call Device/Default-arm64)
  DEVICE_TITLE := ESPRESSObin (Marvell Armada 3700 Community Board)
  DEVICE_DTS := armada-3720-espressobin
  SUPPORTED_DEVICES := globalscale,espressobin
endef
TARGET_DEVICES += globalscale_espressobin

define Device/marvell_armada-3720-db
  $(call Device/Default-arm64)
  DEVICE_TITLE := Marvell Armada 3720 Development Board DB-88F3720-DDR3
  DEVICE_DTS := armada-3720-db
  SUPPORTED_DEVICES := marvell,armada-3720-db
endef
TARGET_DEVICES += marvell_armada-3720-db

endif
