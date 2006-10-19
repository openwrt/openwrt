# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
NF_2_6:=1
include $(INCLUDE_DIR)/netfilter.mk

# Multimedia

$(eval $(call KMOD_template,PWC,pwc,\
	$(MODULES_DIR)/kernel/drivers/usb/media/pwc/pwc.ko \
,CONFIG_USB_PWC,kmod-videodev,63,pwc))

$(eval $(call KMOD_template,VIDEODEV,videodev,\
	$(MODULES_DIR)/kernel/drivers/media/video/v4l1-compat.ko \
	$(MODULES_DIR)/kernel/drivers/media/video/v4l2-common.ko \
	$(MODULES_DIR)/kernel/drivers/media/video/videodev.ko \
,CONFIG_VIDEO_DEV,,62,videodev))


# Network devices

$(eval $(call KMOD_template,NET_AIRO,net-airo,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/airo.ko \
,CONFIG_AIRO,,10,airo))

$(eval $(call KMOD_template,NET_HERMES,net-hermes,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/hermes.ko \
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco.ko \
,CONFIG_HERMES,,10,hermes orinoco))

$(eval $(call KMOD_template,NET_HERMES_PCI,net-hermes-pci,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_pci.ko \
,CONFIG_PCI_HERMES,kmod-net-hermes,11,orinoco_pci))

$(eval $(call KMOD_template,NET_HERMES_PCI,net-hermes-plx,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/orinoco_plx.ko \
,CONFIG_PLX_HERMES,kmod-net-hermes,11,orinoco_plx))

$(eval $(call KMOD_template,NET_PRISM54,net-prism54,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/prism54/prism54.ko \
,CONFIG_PRISM54,,10,prism54))


# PCMCIA/CardBus

$(eval $(call KMOD_template,PCMCIA_CORE,pcmcia-core,\
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia.ko \
	$(MODULES_DIR)/kernel/drivers/pcmcia/pcmcia_core.ko \
	$(MODULES_DIR)/kernel/drivers/pcmcia/yenta_socket.ko \
	$(MODULES_DIR)/kernel/drivers/pcmcia/rsrc_nonstatic.ko \
,CONFIG_PCMCIA,,50,pcmcia_core pcmcia yenta_socket rsrc_nonstatic))

$(eval $(call KMOD_template,PCMCIA_SERIAL,pcmcia-serial,\
	$(MODULES_DIR)/kernel/drivers/serial/serial_cs.ko \
,CONFIG_SERIAL_8250_CS))


# Misc. devices

$(eval $(call KMOD_template,AX25,ax25,\
	$(MODULES_DIR)/kernel/net/ax25/ax25.ko \
	$(MODULES_DIR)/kernel/drivers/net/hamradio/mkiss.ko \
,CONFIG_AX25,,90,ax25 mkiss))

$(eval $(call KMOD_template,BLUETOOTH,bluetooth,\
	$(MODULES_DIR)/kernel/net/bluetooth/bluetooth.ko \
	$(MODULES_DIR)/kernel/net/bluetooth/l2cap.ko \
	$(MODULES_DIR)/kernel/net/bluetooth/sco.ko \
	$(MODULES_DIR)/kernel/net/bluetooth/rfcomm/rfcomm.ko \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_uart.ko \
	$(MODULES_DIR)/kernel/drivers/bluetooth/hci_usb.ko \
,CONFIG_BT))


