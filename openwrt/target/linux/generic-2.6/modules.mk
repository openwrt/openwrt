NF_2_6:=1
include ../netfilter.mk

# Networking

$(eval $(call KMOD_template,ATM,atm,\
	$(MODULES_DIR)/kernel/net/atm/atm.ko \
	$(MODULES_DIR)/kernel/net/atm/br2684.ko \
	$(MODULES_DIR)/kernel/drivers/atm/atmtcp.ko \
,CONFIG_ATM,,50,atm))

$(eval $(call KMOD_template,GRE,gre,\
	$(MODULES_DIR)/kernel/net/ipv4/ip_gre.ko \
,CONFIG_NET_IPGRE))

$(eval $(call KMOD_template,IMQ,imq,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.ko \
	$(MODULES_DIR)/kernel/drivers/net/imq.ko \
))

$(eval $(call KMOD_template,IPIP,ipip,\
	$(MODULES_DIR)/kernel/net/ipv4/ipip.ko \
,CONFIG_NET_IPIP,,60,ipip))

$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.ko \
,CONFIG_IPV6,,20,ipv6))

$(eval $(call KMOD_template,PPP,ppp,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_async.ko \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.ko \
	$(MODULES_DIR)/kernel/drivers/net/slhc.ko \
	$(MODULES_DIR)/kernel/lib/crc-ccitt.ko \
,CONFIG_PPP))

#$(eval $(call KMOD_template,MPPE,mppe,\
#	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.ko \
#,CONFIG_PPP_MPPE_MPPC))

$(eval $(call KMOD_template,PPPOATM,pppoatm,\
	$(MODULES_DIR)/kernel/net/atm/pppoatm.ko \
,CONFIG_PPPOATM))

$(eval $(call KMOD_template,PPPOE,pppoe,\
	$(MODULES_DIR)/kernel/drivers/net/pppoe.ko \
	$(MODULES_DIR)/kernel/drivers/net/pppox.ko \
,CONFIG_PPPOE))

$(eval $(call KMOD_template,SCHED,sched,\
	$(MODULES_DIR)/kernel/net/sched/*.ko \
))

$(eval $(call KMOD_template,TUN,tun,\
	$(MODULES_DIR)/kernel/drivers/net/tun.ko \
,CONFIG_TUN,,20,tun))


# Filtering / Firewalling

$(eval $(call KMOD_template,ARPTABLES,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.ko \
,CONFIG_IP_NF_ARPTABLES))

$(eval $(call KMOD_template,EBTABLES,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.ko \
,CONFIG_BRIDGE_NF_EBTABLES))

# metapackage for compatibility ...
$(eval $(call KMOD_template,IPTABLES_EXTRA,iptables-extra,\
,,kmod-ipt-conntrack kmod-ipt-extra kmod-ipt-filter kmod-ipt-ipopt kmod-ipt-ipsec kmod-ipt-nat kmod-ipt-nat-extra kmod-ipt-queue kmod-ipt-ulogd))

$(eval $(call KMOD_template,IPT_CONNTRACK,ipt-conntrack,\
	$(foreach mod,$(IPT_CONNTRACK-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_EXTRA,ipt-extra,\
	$(foreach mod,$(IPT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_FILTER,ipt-filter,\
	$(foreach mod,$(IPT_FILTER-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_IPOPT,ipt-ipopt,\
	$(foreach mod,$(IPT_IPOPT-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_IPSEC,ipt-ipsec,\
	$(foreach mod,$(IPT_IPSEC-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_NAT,ipt-nat,\
	$(foreach mod,$(IPT_NAT-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_NAT_EXTRA,ipt-nat-extra,\
	$(foreach mod,$(IPT_NAT_EXTRA-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
,,,40,$(IPT_NAT_EXTRA-m)))

$(eval $(call KMOD_template,IPT_QUEUE,ipt-queue,\
	$(foreach mod,$(IPT_QUEUE-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IPT_ULOG,ipt-ulog,\
	$(foreach mod,$(IPT_ULOG-m),$(MODULES_DIR)/kernel/net/$(mod).ko) \
))

$(eval $(call KMOD_template,IP6TABLES,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.ko \
,CONFIG_IP6_NF_IPTABLES,kmod-ipv6))


# Block devices

$(eval $(call KMOD_template,IDE,ide,\
	$(MODULES_DIR)/kernel/drivers/ide/*.ko \
	$(MODULES_DIR)/kernel/drivers/ide/*/*.ko \
,CONFIG_IDE))

$(eval $(call KMOD_template,LOOP,loop,\
    $(MODULES_DIR)/kernel/drivers/block/loop.ko \
,CONFIG_BLK_DEV_LOOP,,20,loop))

$(eval $(call KMOD_template,NBD,nbd,\
    $(MODULES_DIR)/kernel/drivers/block/nbd.ko \
,CONFIG_BLK_DEV_NBD,,20,nbd))


# Crypto

$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.ko \
))


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

$(eval $(call KMOD_template,SOUNDCORE,soundcore,\
	$(MODULES_DIR)/kernel/sound/soundcore.ko \
,CONFIG_SOUND,,30,soundcore))

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

$(eval $(call KMOD_template,NET_3C59X,net-3c59x,\
	$(MODULES_DIR)/kernel/drivers/net/3c59x.ko \
,CONFIG_VORTEX,,10,3c59x))

$(eval $(call KMOD_template,NET_8139TOO,net-8139too,\
	$(MODULES_DIR)/kernel/drivers/net/8139too.ko \
,CONFIG_8139TOO,,10,8139too))

$(eval $(call KMOD_template,NET_AIRO,net-airo,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/airo.ko \
,CONFIG_AIRO,,10,airo))

$(eval $(call KMOD_template,NET_E100,net-e100,\
	$(MODULES_DIR)/kernel/drivers/net/e100.ko \
,CONFIG_E100,,10,e100))

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

$(eval $(call KMOD_template,NET_NATSEMI,net-natsemi,\
	$(MODULES_DIR)/kernel/drivers/net/natsemi.ko \
,CONFIG_NATSEMI,,10,natsemi))

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


# USB

$(eval $(call KMOD_template,USB,usb-core,\
	$(MODULES_DIR)/kernel/drivers/usb/core/usbcore.ko \
,CONFIG_USB,,50,usbcore))

$(eval $(call KMOD_template,USB_OHCI,usb-ohci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ohci-hcd.ko \
,CONFIG_USB_OHCI_HCD,kmod-usb-core,60,ohci-hcd))

$(eval $(call KMOD_template,USB_UHCI,usb-uhci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/uhci-hcd.ko \
,CONFIG_USB_UHCI_HCD,kmod-usb-core,60,uhci-hcd))

$(eval $(call KMOD_template,USB2,usb2,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ehci-hcd.ko \
,CONFIG_USB_EHCI_HCD,kmod-usb-core,60,ehci-hcd))

$(eval $(call KMOD_template,USB_ACM,usb-acm,\
	$(MODULES_DIR)/kernel/drivers/usb/class/cdc-acm.ko \
,CONFIG_USB_ACM))

$(eval $(call KMOD_template,USB_AUDIO,usb-audio,\
	$(MODULES_DIR)/kernel/drivers/usb/class/audio.ko \
,CONFIG_USB_AUDIO,kmod-soundcore kmod-usb-core,61,audio))

$(eval $(call KMOD_template,USB_PRINTER,usb-printer,\
	$(MODULES_DIR)/kernel/drivers/usb/class/usblp.ko \
,CONFIG_USB_PRINTER,kmod-usb-core,60,usblp))

$(eval $(call KMOD_template,USB_SERIAL,usb-serial,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/usbserial.ko \
,CONFIG_USB_SERIAL,kmod-usb-core,60,usbserial))

$(eval $(call KMOD_template,USB_SERIAL_BELKIN,usb-serial-belkin,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/belkin_sa.ko \
,CONFIG_USB_SERIAL_BELKIN,kmod-usb-serial,61,belkin_sa))

$(eval $(call KMOD_template,USB_SERIAL_FTDI,usb-serial-ftdi,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/ftdi_sio.ko \
,CONFIG_USB_SERIAL_FTDI_SIO,kmod-usb-serial,61,ftdi_sio))

$(eval $(call KMOD_template,USB_SERIAL_MCT_U232,usb-serial-mct-u232,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/mct_u232.ko \
,CONFIG_USB_SERIAL_MCT_U232,kmod-usb-serial,61,mct_u232))

$(eval $(call KMOD_template,USB_SERIAL_PL2303,usb-serial-pl2303,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/pl2303.ko \
,CONFIG_USB_SERIAL_PL2303,kmod-usb-serial,61,pl2303))

$(eval $(call KMOD_template,USB_SERIAL_VISOR,usb-serial-visor,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/visor.ko \
,CONFIG_USB_SERIAL_VISOR,kmod-usb-serial,61,visor))

$(eval $(call KMOD_template,USB_STORAGE,usb-storage,\
	$(MODULES_DIR)/kernel/drivers/scsi/scsi_mod.ko \
	$(MODULES_DIR)/kernel/drivers/scsi/sd_mod.ko \
	$(MODULES_DIR)/kernel/drivers/usb/storage/usb-storage.ko \
,CONFIG_USB_STORAGE,kmod-usb-core,60,scsi_mod sd_mod usb-storage))

$(eval $(call KMOD_template,USB_ATM,usb-atm,\
        $(MODULES_DIR)/kernel/drivers/usb/atm/usbatm.ko \
	,CONFIG_USB_ATM,kmod-usb-atm,50,usbatm))

$(eval $(call KMOD_template,USB_ATM_SPEEDTOUCH,usb-atm-speedtouch,\
        $(MODULES_DIR)/kernel/drivers/usb/atm/speedtch.ko \
	,CONFIG_USB_SPEEDTOUCH,kmod-usb-atm-speedtouch,50,speedtch))

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


