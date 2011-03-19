#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PCMCIA_MENU:=PCMCIA support

define KernelPackage/pcmcia-core
  SUBMENU:=$(PCMCIA_MENU)
  TITLE:=PCMCIA/CardBus support
  DEPENDS:=@PCMCIA_SUPPORT
  KCONFIG:= \
	CONFIG_PCMCIA \
	CONFIG_CARDBUS \
	CONFIG_PCCARD \
	PCMCIA_DEBUG=n
endef

define KernelPackage/pcmcia-core/2.4
#  KCONFIG:= \
#	CONFIG_PCMCIA \
#	CONFIG_CARDBUS
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.ko \
	$(LINUX_DIR)/drivers/pcmcia/ds.ko \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.ko
  AUTOLOAD:=$(call AutoLoad,25,pcmcia_core ds yenta_socket)
endef

define KernelPackage/pcmcia-core/2.6
#  KCONFIG:= \
#	CONFIG_PCCARD \
#	CONFIG_PCMCIA \
#	PCMCIA_DEBUG=n
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_core.ko \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia.ko
  AUTOLOAD:=$(call AutoLoad,25,pcmcia_core pcmcia)
endef

define KernelPackage/pcmcia-core/description
 Kernel support for PCMCIA/CardBus controllers
endef

$(eval $(call KernelPackage,pcmcia-core))


define KernelPackage/pcmcia-yenta
  SUBMENU:=$(PCMCIA_MENU)
  TITLE:=yenta socket driver
  DEPENDS:=kmod-pcmcia-core
  KCONFIG:= \
	CONFIG_PCCARD_NONSTATIC \
	CONFIG_YENTA
# For Linux 2.6.35+
ifneq ($(wildcard $(LINUX_DIR)/drivers/pcmcia/pcmcia_rsrc.ko),)
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/pcmcia_rsrc.ko \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.ko
  AUTOLOAD:=$(call AutoLoad,41,pcmcia_rsrc yenta_socket)
else
  FILES:= \
	$(LINUX_DIR)/drivers/pcmcia/rsrc_nonstatic.ko \
	$(LINUX_DIR)/drivers/pcmcia/yenta_socket.ko
  AUTOLOAD:=$(call AutoLoad,41,rsrc_nonstatic yenta_socket)
endif
endef

$(eval $(call KernelPackage,pcmcia-yenta))


define KernelPackage/pcmcia-serial
  SUBMENU:=$(PCMCIA_MENU)
  TITLE:=Serial devices support
  DEPENDS:=kmod-pcmcia-core
  KCONFIG:= \
	CONFIG_PCMCIA_SERIAL_CS \
	CONFIG_SERIAL_8250_CS
  ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.38)),1)
    FILES:=$(LINUX_DIR)/drivers/tty/serial/serial_cs.ko
  else
    FILES:=$(LINUX_DIR)/drivers/serial/serial_cs.ko
  endif
  AUTOLOAD:=$(call AutoLoad,45,serial_cs)
endef

define KernelPackage/pcmcia-serial/description
 Kernel support for PCMCIA/CardBus serial devices
endef

$(eval $(call KernelPackage,pcmcia-serial))
