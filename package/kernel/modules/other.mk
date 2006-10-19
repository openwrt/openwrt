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
	$(CONFIG_CRYPTO_HMAC) \
	$(CONFIG_CRYPTO_NULL) \
	$(CONFIG_CRYPTO_MD4) \
	$(CONFIG_CRYPTO_MD5) \
	$(CONFIG_CRYPTO_SHA1) \
	$(CONFIG_CRYPTO_SHA256) \
	$(CONFIG_CRYPTO_SHA512) \
	$(CONFIG_CRYPTO_WP512) \
	$(CONFIG_CRYPTO_TGR192) \
	$(CONFIG_CRYPTO_DES) \
	$(CONFIG_CRYPTO_BLOWFISH) \
	$(CONFIG_CRYPTO_TWOFISH) \
	$(CONFIG_CRYPTO_SERPENT) \
	$(CONFIG_CRYPTO_AES) \
	$(CONFIG_CRYPTO_CAST5) \
	$(CONFIG_CRYPTO_CAST6) \
	$(CONFIG_CRYPTO_TEA) \
	$(CONFIG_CRYPTO_ARC4) \
	$(CONFIG_CRYPTO_KHAZAD) \
	$(CONFIG_CRYPTO_ANUBIS) \
	$(CONFIG_CRYPTO_DEFLATE) \
	$(CONFIG_CRYPTO_MICHAEL_MIC) \
	$(CONFIG_CRYPTO_CRC32C)
  FILES:=$(MODULES_DIR)/kernel/crypto/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
endef
$(eval $(call KernelPackage,crypto))

define KernelPackage/ide-core
  TITLE:=Kernel support for IDE
  DESCRIPTION:=\
	Useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	- ide-core \\\
	- ide-detect \\\
	- ide-disk
  KCONFIG:=$(CONFIG_IDE)
  FILES:=$(MODULES_DIR)/kernel/drivers/ide/*.$(LINUX_KMOD_SUFFIX)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-detect ide-disk)
endef
$(eval $(call KernelPackage,ide-core))

define KernelPackage/ide-pdc202xx
  TITLE:=PDC202xx IDE driver
  SUBMENU:=$(EMENU)
  KCONFIG:=$(CONFIG_BLK_DEV_PDC202XX_OLD)
  FILES:=$(MODULES_DIR)/kernel/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef
$(eval $(call KernelPackage,ide-pdc202xx))


define KernelPackage/lp
  TITLE:=Parallel port and line printer support
  KCONFIG:=$(CONFIG_PARPORT)
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/parport/parport.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
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
  KCONFIG:=$(CONFIG_SOUND)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,soundcore)
endef

define KernelPackage/soundcore/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/sound/soundcore.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/soundcore/2.6
  FILES:=$(MODULES_DIR)/kernel/sound/soundcore.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,soundcore))


define KernelPackage/loop
  TITLE:=Loopback device support
  DESCRIPTION:=Kernel module for loopback device support
  KCONFIG:=$(CONFIG_BLK_DEV_LOOP)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,loop)
  FILES:=$(MODULES_DIR)/kernel/drivers/block/loop.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,loop))

define KernelPackage/nbd
  TITLE:=Network block device support
  DESCRIPTION:=Kernel module for network block device support
  KCONFIG:=$(CONFIG_BLK_DEV_NBD)
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,30,nbd)
  FILES:=$(MODULES_DIR)/kernel/drivers/block/nbd.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,nbd))


define KernelPackage/pcmcia-core
  TITLE:=PCMCIA/CardBus support
  DESCRIPTION:=Kernel support for PCMCIA/CardBus controllers
  SUBMENU:=$(EMENU)
  KCONFIG:=$(CONFIG_PCMCIA)
endef

define KernelPackage/pcmcia-core/2.4
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/pcmcia/yenta_socket.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/pcmcia/ds.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core yenta_socket ds)
endef

define KernelPackage/pcmcia-core/2.6
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia_core.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/pcmcia/yenta_socket.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/pcmcia/rsrc_nonstatic.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,pcmcia_core pcmcia yenta_socket rsrc_nonstatic)
endef
$(eval $(call KernelPackage,pcmcia-core))


define KernelPackage/pcmcia-serial
  TITLE:=Serial devices support
  DESCRIPTION:=Kernel support for PCMCIA/CardBus serial devices
  DEPENDS:=kmod-pcmcia-core
  SUBMENU:=$(EMENU)
  AUTOLOAD:=$(call AutoLoad,45,serial_cs)
endef

define KernelPackage/pcmcia-serial/2.4
  KCONFIG:=$(CONFIG_PCMCIA_SERIAL_CS)
  FILES:=$(MODULES_DIR)/kernel/drivers/char/pcmcia/serial_cs.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/pcmcia-serial/2.6
  KCONFIG:=$(CONFIG_SERIAL_8250_CS)
  FILES:=$(MODULES_DIR)/kernel/drivers/serial/serial_cs.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,pcmcia-serial))


define KernelPackage/bluetooth
  TITLE:=Bluetooth support
  DESCRIPTION:=Kernel support for Bluetooth devices
  SUBMENU:=$(EMENU)
 endef

 define KernelPackage/bluetooth/2.4
  KCONFIG:=$(CONFIG_BLUEZ)
  FILES:= \
	$(MODULES_DIR)/kernel/net/bluetooth/bluez.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluez l2cap sco rfcomm hci_uart hci_usb)
endef

define KernelPackage/bluetooth/2.6
  KCONFIG:=$(CONFIG_BT)
  FILES:= \
	$(MODULES_DIR)/kernel/net/bluetooth/bluetooth.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/l2cap.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/sco.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/net/bluetooth/rfcomm/rfcomm.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_uart.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_usb.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,90,bluetooth l2cap sco rfcomm hci_uart hci_usb)
endef
$(eval $(call KernelPackage,bluetooth))


define KernelPackage/softdog
  TITLE:=Software watchdog driver
  DESCRIPTION:=Software watchdog driver
  SUBMENU:=$(EMENU)
  KCONFIG:=$(CONFIG_SOFT_WATCHDOG)
  FILES:=$(MODULES_DIR)/kernel/drivers/char/softdog.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,softdog)
endef

define KernelPackage/softdog/2.4
  FILES:=$(MODULES_DIR)/kernel/drivers/char/softdog.o
endef

define KernelPackage/softdog/2.6
  FILES:=$(MODULES_DIR)/kernel/drivers/char/watchdog/softdog.ko
endef

$(eval $(call KernelPackage,softdog))


define KernelPackage/videodev
  TITLE=Video4Linux support
  DESCRIPTION:=Kernel modules for Video4Linux support
  SUBMENU:=$(EMENU)
  KCONFIG:=$(CONFIG_VIDEO_DEV)
  FILES:=$(MODULES_DIR)/kernel/drivers/media/video/*.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/videodev/2.4
  AUTOLOAD:=$(call AutoLoad,60,videodev)
endef

define KernelPackage/videodev/2.6
  AUTOLOAD:=$(call AutoLoad,60,v4l2-common videodev)
endef
$(eval $(call KernelPackage,videodev))



