# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
NF_2_6:=1
include $(INCLUDE_DIR)/netfilter.mk

# Block devices

$(eval $(call KMOD_template,LOOP,loop,\
    $(MODULES_DIR)/kernel/drivers/block/loop.ko \
,CONFIG_BLK_DEV_LOOP,,20,loop))

$(eval $(call KMOD_template,NBD,nbd,\
    $(MODULES_DIR)/kernel/drivers/block/nbd.ko \
,CONFIG_BLK_DEV_NBD,,20,nbd))

# Filesystems

$(eval $(call KMOD_template,FS_CIFS,fs-cifs,\
	$(MODULES_DIR)/kernel/fs/cifs/cifs.ko \
,CONFIG_CIFS,kmod-nls-base,30,cifs))

$(eval $(call KMOD_template,FS_MINIX,fs-minix,\
	$(MODULES_DIR)/kernel/fs/minix/minix.ko \
,CONFIG_MINIX_FS,,30,minix))

$(eval $(call KMOD_template,FS_EXT2,fs-ext2,\
	$(MODULES_DIR)/kernel/fs/ext2/ext2.ko \
,CONFIG_EXT2_FS,,30,ext2))

$(eval $(call KMOD_template,FS_EXT3,fs-ext3,\
	$(MODULES_DIR)/kernel/fs/ext3/ext3.ko \
	$(MODULES_DIR)/kernel/fs/jbd/jbd.ko \
,CONFIG_EXT3_FS,,30,jbd ext3))

$(eval $(call KMOD_template,FS_HFSPLUS,fs-hfsplus,\
	$(MODULES_DIR)/kernel/fs/hfsplus/hfsplus.ko \
,CONFIG_HFSPLUS_FS,kmod-nls-base,30,hfsplus))

$(eval $(call KMOD_template,FS_NFS,fs-nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/lockd.ko \
	$(MODULES_DIR)/kernel/fs/nfs/nfs.ko \
	$(MODULES_DIR)/kernel/net/sunrpc/sunrpc.ko \
,CONFIG_NFS_FS,,30,sunrpc lockd nfs))

$(eval $(call KMOD_template,FS_VFAT,fs-vfat,\
	$(MODULES_DIR)/kernel/fs/fat/fat.ko \
	$(MODULES_DIR)/kernel/fs/vfat/vfat.ko \
,CONFIG_VFAT_FS,kmod-nls-base,30,fat vfat))

$(eval $(call KMOD_template,FS_XFS,fs-xfs,\
	$(MODULES_DIR)/kernel/fs/xfs/xfs.ko \
,CONFIG_XFS_FS,,30,xfs))


# Multimedia

$(eval $(call KMOD_template,PWC,pwc,\
	$(MODULES_DIR)/kernel/drivers/usb/media/pwc/pwc.ko \
,CONFIG_USB_PWC,kmod-videodev,63,pwc))

$(eval $(call KMOD_template,VIDEODEV,videodev,\
	$(MODULES_DIR)/kernel/drivers/media/video/v4l1-compat.ko \
	$(MODULES_DIR)/kernel/drivers/media/video/v4l2-common.ko \
	$(MODULES_DIR)/kernel/drivers/media/video/videodev.ko \
,CONFIG_VIDEO_DEV,,62,videodev))


# NLS

$(eval $(call KMOD_template,NLS_BASE,nls-base,\
	$(MODULES_DIR)/kernel/fs/nls/nls_base.ko \
,CONFIG_NLS,,20,nls_base))

$(eval $(call KMOD_template,NLS_CP437,nls-cp437,\
	$(MODULES_DIR)/kernel/fs/nls/nls_cp437.ko \
,CONFIG_NLS_CODEPAGE_437,,20,nls_cp437))

$(eval $(call KMOD_template,NLS_CP850,nls-cp850,\
	$(MODULES_DIR)/kernel/fs/nls/nls_cp850.ko \
,CONFIG_NLS_CODEPAGE_850,,20,nls_cp850))

$(eval $(call KMOD_template,NLS_ISO8859_1,nls-iso8859-1,\
	$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-1.ko \
,CONFIG_NLS_ISO8859_1,,20,nls_iso8859-1))

$(eval $(call KMOD_template,NLS_ISO8859_15,nls-iso8859-15,\
	$(MODULES_DIR)/kernel/fs/nls/nls_iso8859-15.ko \
,CONFIG_NLS_ISO8859_15,,20,nls_iso8859-15))

$(eval $(call KMOD_template,NLS_UTF8,nls-utf8,\
	$(MODULES_DIR)/kernel/fs/nls/nls_utf8.ko \
,CONFIG_NLS_UTF8,,20,nls_utf8))


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


