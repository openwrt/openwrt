ifeq ($(CONFIG_PLATFORM_NV_TK1), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_NV_TK1
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_P2P_IPS -DCONFIG_PLATFORM_ANDROID
# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK
EXTRA_CFLAGS += -DRTW_VENDOR_EXT_SUPPORT
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ARCH ?= arm

CROSS_COMPILE := /mnt/newdisk/android_sdk/nvidia_tk1/android_L/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
KSRC :=/mnt/newdisk/android_sdk/nvidia_tk1/android_L/out/target/product/shieldtablet/obj/KERNEL/
MODULE_NAME = wlan
endif
