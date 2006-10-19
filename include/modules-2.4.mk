# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(INCLUDE_DIR)/netfilter.mk


# Multimedia

$(eval $(call KMOD_template,PWC,pwc,\
	$(MODULES_DIR)/kernel/drivers/usb/pwc.o \
,CONFIG_USB_PWC,kmod-videodev,63,pwc))

$(eval $(call KMOD_template,VIDEODEV,videodev,\
	$(MODULES_DIR)/kernel/drivers/media/video/videodev.o \
,CONFIG_VIDEO_DEV,,62,videodev))


# Network devices

$(eval $(call KMOD_template,NET_AIRO,net-airo,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/airo.o \
,CONFIG_AIRO,,10,airo))

$(eval $(call KMOD_template,NET_HERMES,net-hermes,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/hermes.o \
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco.o \
,CONFIG_HERMES,,10,hermes orinoco))

$(eval $(call KMOD_template,NET_HERMES_PCI,net-hermes-pci,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_pci.o \
,CONFIG_PCI_HERMES,kmod-net-hermes,11,orinoco_pci))

$(eval $(call KMOD_template,NET_HERMES_PCI,net-hermes-plx,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_plx.o \
,CONFIG_PLX_HERMES,kmod-net-hermes,11,orinoco_plx))

$(eval $(call KMOD_template,NET_PRISM54,net-prism54,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/prism54/prism54.o \
,CONFIG_PRISM54,,10,prism54))


# PCMCIA/CardBus

$(eval $(call KMOD_template,PCMCIA_CORE,pcmcia-core,\
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia_core.o \
	$(MODULES_DIR)/kernel/drivers/pcmcia/yenta_socket.o \
	$(MODULES_DIR)/kernel/drivers/pcmcia/ds.o \
,CONFIG_PCMCIA,,50,pcmcia_core yenta_socket ds))

$(eval $(call KMOD_template,PCMCIA_SERIAL,pcmcia-serial,\
	$(MODULES_DIR)/kernel/drivers/char/pcmcia/serial_cs.o \
,CONFIG_PCMCIA_SERIAL_CS,kmod-pcmcia-core,51,serial_cs))


# Misc. devices

$(eval $(call KMOD_template,AX25,ax25,\
	$(MODULES_DIR)/kernel/net/ax25/ax25.o \
	$(MODULES_DIR)/kernel/drivers/net/hamradio/mkiss.o \
,CONFIG_AX25,,90,ax25 mkiss))

$(eval $(call KMOD_template,BLUETOOTH,bluetooth,\
	$(MODULES_DIR)/kernel/net/bluetooth/*.o \
	$(MODULES_DIR)/kernel/net/bluetooth/rfcomm/*.o \
	$(MODULES_DIR)/kernel/drivers/bluetooth/*.o \
,CONFIG_BLUEZ))

$(eval $(call KMOD_template,SOFTDOG,softdog,\
	$(MODULES_DIR)/kernel/drivers/char/softdog.o \
,CONFIG_SOFT_WATCHDOG,,95,softdog))



