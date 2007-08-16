#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

EMENU:=Other modules

define KernelPackage/crypto
  TITLE:=CryptoAPI modules
  KCONFIG:= \
	CONFIG_CRYPTO_HMAC \
	CONFIG_CRYPTO_NULL \
	CONFIG_CRYPTO_MD4 \
	CONFIG_CRYPTO_MD5 \
	CONFIG_CRYPTO_SHA1 \
	CONFIG_CRYPTO_SHA256 \
	CONFIG_CRYPTO_SHA512 \
	CONFIG_CRYPTO_WP512 \
	CONFIG_CRYPTO_TGR192 \
	CONFIG_CRYPTO_DES \
	CONFIG_CRYPTO_BLOWFISH \
	CONFIG_CRYPTO_TWOFISH \
	CONFIG_CRYPTO_SERPENT \
	CONFIG_CRYPTO_AES \
	CONFIG_CRYPTO_CAST5 \
	CONFIG_CRYPTO_CAST6 \
	CONFIG_CRYPTO_TEA \
	CONFIG_CRYPTO_ARC4 \
	CONFIG_CRYPTO_KHAZAD \
	CONFIG_CRYPTO_ANUBIS \
	CONFIG_CRYPTO_DEFLATE \
	CONFIG_CRYPTO_MICHAEL_MIC \
	CONFIG_CRYPTO_CRC32C \
	CONFIG_CRYPTO_ECB \
	CONFIG_CRYPTO_BLKCIPHER 
  FILES:=$(LINUX_DIR)/crypto/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
endef
$(eval $(call KernelPackage,crypto))

define KernelPackage/lp
  TITLE:=Parallel port and line printer support
  DEPENDS:=@LINUX_2_4
  KCONFIG:=CONFIG_PARPORT
  FILES:= \
	$(LINUX_DIR)/drivers/parport/parport.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,50, \
  	parport \
  	lp \
  )
endef
$(eval $(call KernelPackage,lp))


define KernelPackage/soundcore
  TITLE:=Sound support
  DESCRIPTION:=Kernel modules for sound support
  KCONFIG:=CONFIG_SOUND
  SUBMENU:=$(EMENU)
endef

define KernelPackage/soundcore/2.4
  FILES:=$(LINUX_DIR)/drivers/sound/soundcore.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore)
endef

define KernelPackage/soundcore/2.6
  FILES:= \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/core/*.$(LINUX_KMOD_SUFFIX) \
	$(if $(CONFIG_SND_MIXER_OSS)$(CONFIG_SND_PCM_OSS),$(LINUX_DIR)/sound/core/oss/*.$(LINUX_KMOD_SUFFIX))
  AUTOLOAD:=$(call AutoLoad,30,soundcore snd snd-page-alloc snd-hwdep snd-rawmidi snd-timer snd-pcm $(if $(CONFIG_SND_MIXER_OSS),snd-mixer-oss) $(if $(CONFIG_SND_PCM_OSS),snd-pcm-oss))
endef

define KernelPackage/soundcore/uml-2.6
  FILES:= \
	$(LINUX_DIR)/arch/um/drivers/hostaudio.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,soundcore hostaudio)
endef

$(eval $(call KernelPackage,soundcore))


define KernelPackage/loop
  TITLE:=Loopback device support
  DESCRIPTION:=Kernel module for loopback device support
  KCONFIG:=CONFIG_BLK_DEV_LOOP
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,loop)
  FILES:=$(LINUX_DIR)/drivers/block/loop.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,loop))

define KernelPackage/nbd
  TITLE:=Network block device support
  DESCRIPTION:=Kernel module for network block device support
  KCONFIG:=CONFIG_BLK_DEV_NBD
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,nbd)
  FILES:=$(LINUX_DIR)/drivers/block/nbd.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,nbd))

define KernelPackage/capi
  TITLE:=CAPI Support
  DESCRIPTION:=Kernel module for basic CAPI support
  KCONFIG:=CONFIG_ISDN CONFIG_ISDN_CAPI CONFIG_ISDN_CAPI_CAPI20
  DEPENDS:=@LINUX_2_6
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,kernelcapi capi)
  FILES:=$(LINUX_DIR)/drivers/isdn/capi/*capi.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,capi))

define KernelPackage/pcmcia-core
  TITLE:=PCMCIA/CardBus support
  DESCRIPTION:=Kernel support for PCMCIA/CardBus controllers
  DEPENDS:=@PCMCIA_SUPPORT
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_PCMCIA CONFIG_PCCARD CONFIG_PCMCIA_AU1X00
endef

ifneq ($(CONFIG_LINUX_2_6_AU1000),)
  PCMCIA_SOCKET_DRIVER:=au1x00_ss
else
  PCMCIA_SOCKET_DRIVER:=yenta_socket
endif

define KernelPackage/pcmcia-core/2.4
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/$(PCMCIA_SOCKET_DRIVER).$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/ds.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core $(PCMCIA_SOCKET_DRIVER) ds)
endef

define KernelPackage/pcmcia-core/2.6
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/$(PCMCIA_SOCKET_DRIVER).$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/pcmcia/rsrc_nonstatic.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core pcmcia rsrc_nonstatic $(PCMCIA_SOCKET_DRIVER))
endef
$(eval $(call KernelPackage,pcmcia-core))


define KernelPackage/pcmcia-serial
  TITLE:=Serial devices support
  DESCRIPTION:=Kernel support for PCMCIA/CardBus serial devices
  DEPENDS:=kmod-pcmcia-core
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,45,serial_cs)
  KCONFIG:=CONFIG_PCMCIA_SERIAL_CS CONFIG_SERIAL_8250_CS
endef

define KernelPackage/pcmcia-serial/2.4
  FILES:=$(LINUX_DIR)/drivers/char/pcmcia/serial_cs.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/pcmcia-serial/2.6
  FILES:=$(LINUX_DIR)/drivers/serial/serial_cs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,pcmcia-serial))


define KernelPackage/bluetooth
  TITLE:=Bluetooth support
  DEPENDS:=@USB_SUPPORT
  DESCRIPTION:=Kernel support for Bluetooth devices
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_BLUEZ CONFIG_BT CONFIG_USB_BLUETOOTH
 endef

 define KernelPackage/bluetooth/2.4
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluez.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluez l2cap sco rfcomm bnep hci_uart hci_usb)
endef

define KernelPackage/bluetooth/2.6
  FILES:= \
	$(LINUX_DIR)/net/bluetooth/bluetooth.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/net/bluetooth/bnep/bnep.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm bnep hci_uart hci_usb)
endef
$(eval $(call KernelPackage,bluetooth))

define KernelPackage/mmc
  TITLE:=MMC/SD Card Support
  DEPENDS:=@LINUX_2_6_AT91
  DESCRIPTION:=Kernel support for MMC/SD cards
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_MMC
  FILES:= \
	$(LINUX_DIR)/drivers/mmc/mmc_core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mmc/mmc_block.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/mmc/at91_mci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,mmc_core mmc_block at91_mci)
endef
$(eval $(call KernelPackage,mmc))

define KernelPackage/softdog
  TITLE:=Software watchdog driver
  DESCRIPTION:=Software watchdog driver
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_SOFT_WATCHDOG
  AUTOLOAD:=$(call AutoLoad,50,softdog)
endef

define KernelPackage/softdog/2.4
  FILES:=$(LINUX_DIR)/drivers/char/softdog.o
endef

define KernelPackage/softdog/2.6
  FILES:=$(LINUX_DIR)/drivers/char/watchdog/softdog.ko
endef
$(eval $(call KernelPackage,softdog))


define KernelPackage/videodev
  TITLE=Video4Linux support
  DESCRIPTION:=Kernel modules for Video4Linux support
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_VIDEO_DEV
  FILES:=$(LINUX_DIR)/drivers/media/video/*.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/videodev/2.4
  AUTOLOAD:=$(call AutoLoad,60,videodev)
endef

define KernelPackage/videodev/2.6
  AUTOLOAD:=$(call AutoLoad,60,v4l2-common v4l1-compat compat_ioctl32 videodev)
endef
$(eval $(call KernelPackage,videodev))

define KernelPackage/leds-net48xx
  TITLE:=Soekris Net48xx LED support
  DESCRIPTION:=Kernel module for Soekris Net48xx LEDs
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_LEDS_NET48XX
  FILES:=$(LINUX_DIR)/drivers/leds/leds-net48xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,leds-net48xx)
endef
$(eval $(call KernelPackage,leds-net48xx))

define KernelPackage/nsc-gpio
  TITLE:=Natsemi GPIO support
  DESCRIPTION:=Kernel module for Natsemi GPIO
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_NSC_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/nsc_gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,nsc_gpio)
endef
$(eval $(call KernelPackage,nsc-gpio))

define KernelPackage/scx200-gpio
  TITLE:=Natsemi SCX200 GPIO support
  DESCRIPTION:=Kernel module for SCX200 GPIO
  DEPENDS:=kmod-nsc-gpio @LINUX_2_6_X86
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_SCx200_GPIO
  FILES:=$(LINUX_DIR)/drivers/char/scx200_gpio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,scx200_gpio)
endef
$(eval $(call KernelPackage,scx200-gpio))

define KernelPackage/scx200-wdt
  TITLE:=Natsemi SCX200 Watchdog support
  DESCRIPTION:=Kernel module for SCX200 Watchdog
  DEPENDS:=@LINUX_2_6_X86
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_SC1200_WDT
  FILES:=$(LINUX_DIR)/drivers/char/watchdog/scx200_wdt.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,scx200_wdt)
endef
$(eval $(call KernelPackage,scx200-wdt))

define KernelPackage/hwmon
  TITLE:=Hardware monitoring support
  DESCRIPTION:=Kernel modules for hardware monitoring
  DEPENDS:=@LINUX_2_6
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_HWMON_VID
  FILES:= \
	$(LINUX_DIR)/drivers/hwmon/hwmon.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/hwmon/hwmon-vid.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,hwmon hwmon-vid)
endef
$(eval $(call KernelPackage,hwmon))

define KernelPackage/hwmon-pc87360
  TITLE:=PC87360 monitoring support
  DESCRIPTION:=Kernel modules for PC87360 chips
  DEPENDS:=kmod-hwmon
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_SENSORS_PC87360
  FILES:=$(LINUX_DIR)/drivers/hwmon/pc87360.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,pc87360)
endef
$(eval $(call KernelPackage,hwmon-pc87360))

define KernelPackage/input-core
  TITLE:=Input device core
  DESCRIPTION:=Kernel modules for support of input device
  SUBMENU:=$(EMENU)
  DEPENDS:=@LINUX_2_6
  KCONFIG:=CONFIG_INPUT
  FILES:=$(LINUX_DIR)/drivers/input/input-core.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,input-core)
endef
$(eval $(call KernelPackage,input-core))

define KernelPackage/input-evdev
  TITLE:=Input even device
  DESCRIPTION:=Kernel modules for support of input device events
  DEPENDS:=+kmod-input-core
  SUBMENU:=$(EMENU)
  KCONFIG:=CONFIG_INPUT_EVDEV
  FILES:=$(LINUX_DIR)/drivers/input/evdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,60,evdev)
endef
$(eval $(call KernelPackage,input-evdev))

