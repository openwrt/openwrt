include ./common-ubnt.mk

define Device/ubnt_airrouter
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := AirRouter
  SUPPORTED_DEVICES += airrouter
endef
TARGET_DEVICES += ubnt_airrouter

define Device/ubnt_nanobridge-m
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := NanoBridge M
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += bullet-m
endef
TARGET_DEVICES += ubnt_nanobridge-m

define Device/ubnt_bullet-m-ar7240
  $(Device/ubnt-xm)
  SOC := ar7240
  DEVICE_MODEL := Bullet M
  DEVICE_VARIANT := XM (AR7240)
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += bullet-m
endef
TARGET_DEVICES += ubnt_bullet-m-ar7240

define Device/ubnt_bullet-m-ar7241
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := Bullet M
  DEVICE_VARIANT := XM (AR7241)
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += bullet-m ubnt,bullet-m
endef
TARGET_DEVICES += ubnt_bullet-m-ar7241

define Device/ubnt_picostation-m
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := Picostation M
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += bullet-m
endef
TARGET_DEVICES += ubnt_picostation-m

define Device/ubnt_nanostation-m
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := Nanostation M
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += nanostation-m
endef
TARGET_DEVICES += ubnt_nanostation-m

define Device/ubnt_nanostation-loco-m
  $(Device/ubnt-xm)
  SOC := ar7241
  DEVICE_MODEL := Nanostation Loco M
  DEVICE_PACKAGES += rssileds
  SUPPORTED_DEVICES += bullet-m
endef
TARGET_DEVICES += ubnt_nanostation-loco-m
