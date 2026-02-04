ifneq ($(CONFIG_USE_APK),)
  DEFAULT_PACKAGES += apk-mbedtls
else
  DEFAULT_PACKAGES += opkg
endif
