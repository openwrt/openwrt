ifeq ($(CONFIG_PLATFORM_ARM_SUNxI), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUNxI
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
# default setting for A10-EVB mmc0
#EXTRA_CFLAGS += -DCONFIG_WITS_EVB_V13
_PLATFORM_FILES += platform/platform_ARM_SUNxI_sdio.o
endif

ARCH := arm
#CROSS_COMPILE := arm-none-linux-gnueabi-
CROSS_COMPILE=/home/android_sdk/Allwinner/a10/android-jb42/lichee-jb42/buildroot/output/external-toolchain/bin/arm-none-linux-gnueabi-
KVER  := 3.0.8
#KSRC:= ../lichee/linux-3.0/
KSRC=/home/android_sdk/Allwinner/a10/android-jb42/lichee-jb42/linux-3.0
endif
