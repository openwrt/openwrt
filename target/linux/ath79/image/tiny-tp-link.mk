include ./common-tp-link.mk


define Device/tl-wr740n-v2
  $(Device/tplink-4m)
  ATH_SOC := ar7240
  DEVICE_TITLE := TP-Link TL-WR740N/ND v2
  TPLINK_HWID := 0x07400001
  SUPPORTED_DEVICES := tplink,tl-wr740n-v2 tl-wr740n-v2
endef
TARGET_DEVICES += tl-wr740n-v2
