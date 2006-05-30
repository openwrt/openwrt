include ../netfilter.mk

# Networking

$(eval $(call KMOD_template,ATM,atm,\
	$(MODULES_DIR)/kernel/net/atm/atm.o \
	$(MODULES_DIR)/kernel/net/atm/br2684.o \
,CONFIG_ATM,,50,atm))

$(eval $(call KMOD_template,GRE,gre,\
	$(MODULES_DIR)/kernel/net/ipv4/ip_gre.o \
,CONFIG_NET_IPGRE))

$(eval $(call KMOD_template,IMQ,imq,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/*IMQ*.o \
	$(MODULES_DIR)/kernel/drivers/net/imq.o \
))
$(eval $(call KMOD_template,IPIP,ipip,\
	$(MODULES_DIR)/kernel/net/ipv4/ipip.o \
,CONFIG_NET_IPIP,,60,ipip))

$(eval $(call KMOD_template,IPV6,ipv6,\
	$(MODULES_DIR)/kernel/net/ipv6/ipv6.o \
,CONFIG_IPV6,,20,ipv6))

$(eval $(call KMOD_template,PPP,ppp,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_async.o \
	$(MODULES_DIR)/kernel/drivers/net/ppp_generic.o \
	$(MODULES_DIR)/kernel/drivers/net/slhc.o \
	$(MODULES_DIR)/kernel/drivers/net/pppox.o \
,CONFIG_PPP))

$(eval $(call KMOD_template,MPPE,mppe,\
	$(MODULES_DIR)/kernel/drivers/net/ppp_mppe_mppc.o \
,CONFIG_PPP_MPPE_MPPC))

$(eval $(call KMOD_template,PPPOATM,pppoatm,\
	$(MODULES_DIR)/kernel/net/atm/pppoatm.o \
,CONFIG_PPPOATM))

$(eval $(call KMOD_template,PPPOE,pppoe,\
	$(MODULES_DIR)/kernel/drivers/net/pppoe.o \
,CONFIG_PPPOE))

$(eval $(call KMOD_template,SCHED,sched,\
	$(MODULES_DIR)/kernel/net/sched/*.o \
))

$(eval $(call KMOD_template,TUN,tun,\
	$(MODULES_DIR)/kernel/drivers/net/tun.o \
,CONFIG_TUN,,20,tun))

# Filtering / Firewalling

$(eval $(call KMOD_template,ARPTABLES,arptables,\
	$(MODULES_DIR)/kernel/net/ipv4/netfilter/arp*.o \
,CONFIG_IP_NF_ARPTABLES))

$(eval $(call KMOD_template,EBTABLES,ebtables,\
	$(MODULES_DIR)/kernel/net/bridge/netfilter/*.o \
,CONFIG_BRIDGE_NF_EBTABLES))

# metapackage for compatibility ...
$(eval $(call KMOD_template,IPTABLES_EXTRA,iptables-extra,\
,,kmod-ipt-conntrack kmod-ipt-extra kmod-ipt-filter kmod-ipt-ipopt kmod-ipt-ipsec kmod-ipt-nat kmod-ipt-nat-extra kmod-ipt-queue kmod-ipt-ulogd))

$(eval $(call KMOD_template,IPT_CONNTRACK,ipt-conntrack,\
	$(foreach mod,$(IPT_CONNTRACK-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_EXTRA,ipt-extra,\
	$(foreach mod,$(IPT_EXTRA-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_FILTER,ipt-filter,\
	$(foreach mod,$(IPT_FILTER-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_IPOPT,ipt-ipopt,\
	$(foreach mod,$(IPT_IPOPT-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_IPSEC,ipt-ipsec,\
	$(foreach mod,$(IPT_IPSEC-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_NAT,ipt-nat,\
	$(foreach mod,$(IPT_NAT-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_NAT_EXTRA,ipt-nat-extra,\
	$(foreach mod,$(IPT_NAT_EXTRA-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
,,,40,$(IPT_NAT_EXTRA-m)))

$(eval $(call KMOD_template,IPT_QUEUE,ipt-queue,\
	$(foreach mod,$(IPT_QUEUE-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IPT_ULOG,ipt-ulog,\
	$(foreach mod,$(IPT_ULOG-m),$(MODULES_DIR)/kernel/net/ipv4/netfilter/$(mod).o) \
))

$(eval $(call KMOD_template,IP6TABLES,ip6tables,\
	$(MODULES_DIR)/kernel/net/ipv6/netfilter/ip*.o \
,CONFIG_IP6_NF_IPTABLES,kmod-ipv6))


# Block devices

$(eval $(call KMOD_template,IDE,ide,\
	$(MODULES_DIR)/kernel/drivers/ide/*.o \
	$(MODULES_DIR)/kernel/drivers/ide/*/*.o \
,CONFIG_IDE,,20,pdc202xx_old ide-core ide-detect ide-disk))

$(eval $(call KMOD_template,LOOP,loop,\
    $(MODULES_DIR)/kernel/drivers/block/loop.o \
,CONFIG_BLK_DEV_LOOP,,20,loop))

$(eval $(call KMOD_template,NBD,nbd,\
    $(MODULES_DIR)/kernel/drivers/block/nbd.o \
,CONFIG_BLK_DEV_NBD,,20,nbd))


# Crypto

ifneq ($(wildcard $(MODULES_DIR)/kernel/crypto/*.o),)
CONFIG_CRYPTO:=m
endif
$(eval $(call KMOD_template,CRYPTO,crypto,\
	$(MODULES_DIR)/kernel/crypto/*.o \
,CONFIG_CRYPTO))


# Filesystems

$(eval $(call KMOD_template,FS_CIFS,fs-cifs,\
	$(MODULES_DIR)/kernel/fs/cifs/cifs.o \
,CONFIG_CIFS,,30,cifs))

$(eval $(call KMOD_template,FS_MINIX,fs-minix,\
	$(MODULES_DIR)/kernel/fs/minix/*.o \
,CONFIG_MINIX_FS,,30,minix))

$(eval $(call KMOD_template,FS_EXT2,fs-ext2,\
	$(MODULES_DIR)/kernel/fs/ext2/*.o \
,CONFIG_EXT2_FS,,30,ext2))

$(eval $(call KMOD_template,FS_EXT3,fs-ext3,\
	$(MODULES_DIR)/kernel/fs/ext3/*.o \
	$(MODULES_DIR)/kernel/fs/jbd/*.o \
,CONFIG_EXT3_FS,,30,jbd ext3))

$(eval $(call KMOD_template,FS_HFSPLUS,fs-hfsplus,\
	$(MODULES_DIR)/kernel/fs/hfsplus/*.o \
,CONFIG_HFSPLUS_FS,,30,hfsplus))

$(eval $(call KMOD_template,FS_NFS,fs-nfs,\
	$(MODULES_DIR)/kernel/fs/lockd/*.o \
	$(MODULES_DIR)/kernel/fs/nfs/*.o \
	$(MODULES_DIR)/kernel/net/sunrpc/*.o \
,CONFIG_NFS_FS,,30,sunrpc lockd nfs))

$(eval $(call KMOD_template,FS_VFAT,fs-vfat,\
	$(MODULES_DIR)/kernel/fs/vfat/vfat.o \
	$(MODULES_DIR)/kernel/fs/fat/fat.o \
,CONFIG_VFAT_FS,,30,fat vfat))

$(eval $(call KMOD_template,FS_XFS,fs-xfs,\
	$(MODULES_DIR)/kernel/fs/xfs/*.o \
,CONFIG_XFS_FS,,30,xfs))


# Multimedia

$(eval $(call KMOD_template,PWC,pwc,\
	$(MODULES_DIR)/kernel/drivers/usb/pwc.o \
,CONFIG_USB_PWC,kmod-videodev,63,pwc))

$(eval $(call KMOD_template,SOUNDCORE,soundcore,\
	$(MODULES_DIR)/kernel/drivers/sound/soundcore.o \
,CONFIG_SOUND,,30,soundcore))

$(eval $(call KMOD_template,VIDEODEV,videodev,\
	$(MODULES_DIR)/kernel/drivers/media/video/videodev.o \
,CONFIG_VIDEO_DEV,,62,videodev))


# Network devices

$(eval $(call KMOD_template,NET_3C59X,net-3c59x,\
	$(MODULES_DIR)/kernel/drivers/net/3c59x.o \
,CONFIG_VORTEX,,10,3c59x))

$(eval $(call KMOD_template,NET_8139TOO,net-8139too,\
	$(MODULES_DIR)/kernel/drivers/net/8139too.o \
	$(MODULES_DIR)/kernel/drivers/net/mii.o \
,CONFIG_8139TOO,,10,mii 8139too))

$(eval $(call KMOD_template,NET_AIRO,net-airo,\
	$(MODULES_DIR)/kernel/drivers/net/wireless/airo.o \
,CONFIG_AIRO,,10,airo))

$(eval $(call KMOD_template,NET_E100,net-e100,\
	$(MODULES_DIR)/kernel/drivers/net/e100.o \
,CONFIG_E100,,10,e100))

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

$(eval $(call KMOD_template,NET_NATSEMI,net-natsemi,\
	$(MODULES_DIR)/kernel/drivers/net/natsemi.o \
,CONFIG_NATSEMI,,10,natsemi))

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


# USB

$(eval $(call KMOD_template,USB,usb-core,\
	$(MODULES_DIR)/kernel/drivers/usb/usbcore.o \
,CONFIG_USB,,50,usbcore))

$(eval $(call KMOD_template,USB_OHCI,usb-ohci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/usb-ohci.o \
,CONFIG_USB_OHCI,kmod-usb-core,60,usb-ohci))

$(eval $(call KMOD_template,USB_UHCI,usb-uhci,\
	$(MODULES_DIR)/kernel/drivers/usb/host/uhci.o \
,CONFIG_USB_UHCI_ALT,kmod-usb-core,60,uhci))

$(eval $(call KMOD_template,USB2,usb2,\
	$(MODULES_DIR)/kernel/drivers/usb/host/ehci-hcd.o \
,CONFIG_USB_EHCI_HCD,kmod-usb-core,60,ehci-hcd))

$(eval $(call KMOD_template,USB_ACM,usb-acm,\
	$(MODULES_DIR)/kernel/drivers/usb/acm.o \
,CONFIG_USB_ACM))

$(eval $(call KMOD_template,USB_AUDIO,usb-audio,\
	$(MODULES_DIR)/kernel/drivers/usb/audio.o \
,CONFIG_USB_AUDIO,kmod-soundcore kmod-usb-core,61,audio))

$(eval $(call KMOD_template,USB_PRINTER,usb-printer,\
	$(MODULES_DIR)/kernel/drivers/usb/printer.o \
,CONFIG_USB_PRINTER,kmod-usb-core,60,printer))

$(eval $(call KMOD_template,USB_SERIAL,usb-serial,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/usbserial.o \
,CONFIG_USB_SERIAL,kmod-usb-core,60,usbserial))

$(eval $(call KMOD_template,USB_SERIAL_BELKIN,usb-serial-belkin,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/belkin_sa.o \
,CONFIG_USB_SERIAL_BELKIN,kmod-usb-serial,61,belkin_sa))

$(eval $(call KMOD_template,USB_SERIAL_FTDI,usb-serial-ftdi,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/ftdi_sio.o \
,CONFIG_USB_SERIAL_FTDI_SIO,kmod-usb-serial,61,ftdi_sio))

$(eval $(call KMOD_template,USB_SERIAL_MCT_U232,usb-serial-mct-u232,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/mct_u232.o \
,CONFIG_USB_SERIAL_MCT_U232,kmod-usb-serial,61,mct_u232))

$(eval $(call KMOD_template,USB_SERIAL_PL2303,usb-serial-pl2303,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/pl2303.o \
,CONFIG_USB_SERIAL_PL2303,kmod-usb-serial,61,pl2303))

$(eval $(call KMOD_template,USB_SERIAL_VISOR,usb-serial-visor,\
	$(MODULES_DIR)/kernel/drivers/usb/serial/visor.o \
,CONFIG_USB_SERIAL_VISOR,kmod-usb-serial,61,visor))

$(eval $(call KMOD_template,USB_STORAGE,usb-storage,\
	$(MODULES_DIR)/kernel/drivers/scsi/*.o \
	$(MODULES_DIR)/kernel/drivers/usb/storage/*.o \
,CONFIG_USB_STORAGE,kmod-usb-core,60,scsi_mod sd_mod usb-storage))


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



