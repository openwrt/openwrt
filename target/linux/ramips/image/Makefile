#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)
endef

define sysupname
$(call imgname,$(1),$(2))-sysupgrade.bin
endef

VMLINUX:=$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage
fs_squash:=squashfs-only
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	fs_squash:=initramfs
	VMLINUX:=$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(IMG_PREFIX)-uImage-initramfs
endif

ifeq ($(CONFIG_RALINK_RT288X),y)
define kernel_entry
-a 0x88000000 -e 0x88000000
endef
else
define kernel_entry
-a 0x80000000 -e 0x80000000
endef
endif

define mkcmdline
board=$(1) console=$(2),$(3)
endef

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).bin.lzma)
endef

define MkImage
	mkimage -A mips -O linux -T kernel -C $(1) $(call kernel_entry)\
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(2) $(3)
endef

define MkImageLzma
	$(call PatchKernelLzma,$(1),$(2))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(1).bin.lzma,$(KDIR)/vmlinux-$(1).uImage)
endef

define CatFiles
	if [ `stat -c%s "$(1)"` -gt $(2) ]; then \
		echo "Warning: $(1) is too big"; \
	else if [ `stat -c%s $(3)` -gt $(4) ]; then \
		echo "Warning: $(3) is too big"; \
	else \
		( dd if=$(1) bs=$(2) conv=sync; dd if=$(3) ) > $(5); \
	fi; fi
endef

define Sysupgrade/KRuImage
	$(call CatFiles,$(KDIR)/vmlinux-$(2).uImage,$(3),$(KDIR)/root.$(1),$(4),$(call sysupname,$(1),$(2)))
endef

define mkmtd
mtdparts=$(1):$(2)
endef

define mkmtd/phys
$(call mkmtd,physmap-flash.0,$(1))
endef

define mkmtd/spi
$(call mkmtd,spi0.0,$(1))
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX).elf
	cp $(KDIR)/vmlinux $(BIN_DIR)/$(VMLINUX).bin
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	cp $(KDIR)/uImage.lzma $(BIN_DIR)/$(UIMAGE).bin
	$(call Image/Build/Initramfs)
endef

define BuildFirmware/Generic
	$(call MkImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(4),$(5))
endef

define BuildFirmware/Generic/initramfs
	$(call MkImageLzma,$(2),$(3))
	$(CP) $(KDIR)/vmlinux-$(2).uImage $(call imgname,$(1),$(2))-uImage.bin
endef

define BuildFirmware/WL341V3
	$(call MkImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(4),$(5))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=/dev/zero bs=195936 count=1; \
			echo "1.01"; \
			dd if=/dev/zero bs=581 count=1; \
			echo -n -e "\x73\x45\x72\x43\x6F\x4D\x6D\x00\x01\x00\x00\x59\x4E\x37\x95\x58\x10\x00\x20\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x03\x00\x00\x80\x00\x00\x00\x00\x03\x00\x00\x10\x12\x00\x00\x00\x10\x73\x45\x72\x43\x6F\x4D\x6D"; \
			dd if=/dev/zero bs=65552 count=1; \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(4) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
			dd if=/dev/zero bs=`expr 4194304 - 262144 - 16 - $(4) - \( \( \( \`stat -c%s $(KDIR)/root.$(1)\` / 65536 \) + 1 \) \* 65536 \)` count=1; \
			echo -n -e "\x11\x03\x80\x00\x10\x12\x90\xF7\x65\x52\x63\x4F\x6D\x4D\x00\x00"; \
		) > $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef

define BuildFirmware/Buffalo
	$(call MkImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(4),$(5))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/vmlinux-$(2).uImage \
			-o $(KDIR)/vmlinux-$(2).uImage.enc; \
		buffalo-enc -p $(3) -v 1.76 \
			-i $(KDIR)/root.$(1) \
			-o $(KDIR)/root.$(2).enc; \
		buffalo-tag -b $(3) -p $(3) -a ram -v 1.76 -m 1.01 \
			-l mlang8 -f 1 -r EU \
			-i $(KDIR)/vmlinux-$(2).uImage.enc \
			-i $(KDIR)/root.$(2).enc \
			-o $(call imgname,$(1),$(2))-factory-EU.bin; \
	fi
endef

define BuildFirmware/Buffalo2
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(4),$(5),$(6))
endef

define BuildFirmware/NW718
	$(call PatchKernelLzma,$(2),$(3))
	mkimage -A mips -O linux -T kernel -C lzma $(call kernel_entry) \
		-n "ARA1B4NCRNW718;1" \
		-d "$(KDIR)/vmlinux-$(2).bin.lzma" \
		"$(KDIR)/vmlinux-$(2).uImage"
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).uImage"` -gt $(4) ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).uImage is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(4) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
		) > $(call imgname,$(1),$(2))-factory.bin; \
	fi; fi
endef

define BuildFirmware/Edimax
	mkedimaximg -i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin \
		-s $(3) -m $(4) -f $(5) -S $(6)
endef

define Image/Build/Template/initramfs/initramfs
	$(call Image/Build/Template/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/squashfs-only/squashfs
	$(call Image/Build/Template/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

mtdlayout_4M=192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,832k(kernel),2944k(rootfs),3776k@0x50000(firmware)
define Image/Build/Template/GENERIC_4M
	$(call BuildFirmware/Generic,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_4M)),851968,3014656)
endef

define Image/Build/Template/GENERIC_4M/initramfs
	$(call BuildFirmware/Generic/initramfs,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_4M)))
endef

mtdlayout_8M=192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,896k(kernel),6976k(rootfs),7872k@0x50000(firmware)
kernel_size_8M=917504
rootfs_size_8M=7143424
define Image/Build/Template/GENERIC_8M
	$(call BuildFirmware/Generic,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)),$(kernel_size_8M),$(rootfs_size_8M))
endef

define Image/Build/Template/GENERIC_8M/initramfs
	$(call BuildFirmware/Generic/initramfs,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)))
endef

mtdlayout_dir300b1=192k(u-boot)ro,64k(devdata)ro,64k(devconf)ro,896k(kernel),2880k(rootfs),3776k@0x50000(firmware)
mtd_dir300b1_kernel_part_size=917504
mtd_dir300b1_rootfs_part_size=2949120
define Image/Build/Template/DIR300B1
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(call mkmtd/phys,$(mtdlayout_dir300b1)),$(mtd_dir300b1_kernel_part_size),$(mtd_dir300b1_rootfs_part_size))
	mkwrgimg -s $(4) -d /dev/mtdblock/2 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef

mtdlayout_nw718=192k(u-boot)ro,128k(config)ro,64k(factory)ro,832k(kernel),2880k(rootfs),3712k@0x60000(firmware)
mtdparts_nw718=$(mkmtd/spi,$(mtdlayout_nw718))
define Image/Build/Template/NW718
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(mtdparts_nw718),851968,2949120)
	$(call BuildFirmware/NW718,$(1),$(2),board=$(3) $(mtdparts_nw718),851968,2949120)
endef

define Image/Build/Profile/WR6202
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,wr6202,WR6202,ttyS1,115200,phys)
endef

mtdlayout_sercomm_4M=128k(u-boot)ro,64k(board-nvram)ro,64k(u-boot-env)ro,832k(kernel),2944k(rootfs),64k(signature-eRcOmM),3776k@0x40000(firmware),4096k@0x0(fullflash)
mtdparts_sercomm_4M=$(call mkmtd/phys,$(mtdlayout_sercomm_4M))
define Image/Build/Template/WL341V3
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(mtdparts_sercomm_4M),851968,3014656)
	$(call BuildFirmware/WL341V3,$(1),$(2),board=$(3) $(mtdparts_sercomm_4M),851968,3014656)
endef

define Image/Build/Profile/WL341V3
	$(call Image/Build/Template/$(fs_squash)/$(1),WL341V3,wl341v3,WL341V3)
endef

define Image/Build/Profile/DIR300B1
	$(call Image/Build/Template/$(fs_squash)/$(1),DIR300B1,dir-300-b1,DIR-300-B1,wrgn23_dlwbr_dir300b)
	$(call Image/Build/Template/$(fs_squash)/$(1),DIR300B1,dir-600-b1,DIR-600-B1,wrgn23_dlwbr_dir600b)
	$(call Image/Build/Template/$(fs_squash)/$(1),DIR300B1,dir-600-b2,DIR-600-B2,wrgn23_dlwbr_dir600b)
endef

mtdlayout_rtg32b1=192k(u-boot)ro,64k(devdata)ro,64k(devconf)ro,832k(kernel),2944k(rootfs),3776k@0x50000(firmware)
mtd_rtg32b1_kernel_part_size=851968
mtd_rtg32b1_rootfs_part_size=3014656
define Image/Build/Template/RTG32B1
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(call mkmtd/spi,$(mtdlayout_rtg32b1)),$(mtd_rtg32b1_kernel_part_size),$(mtd_rtg32b1_rootfs_part_size))
endef

define Image/Build/Profile/RTG32B1
	$(call Image/Build/Template/$(fs_squash)/$(1),RTG32B1,rt-g32-b1,RT-G32-B1)
endef

define Image/Build/Profile/ESR9753
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,esr-9753,ESR-9753,ttyS1,57600,phys)
endef

define Image/Build/Template/FONERA20N
	$(call Image/Build/Template/GENERIC_8M,$(1),fonera20n,FONERA20N,ttyS1,57600,phys)
	$(call BuildFirmware/Edimax,$(1),fonera20n,RSDK,NL1T,0x50000,0xc0000)
endef

define Image/Build/Profile/FONERA20N
	$(call Image/Build/Template/$(fs_squash)/$(1),FONERA20N)
endef

define Image/Build/Profile/BC2
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,bc2,BC2,ttyS1,57600,phys)
endef

define Image/Build/Template/ALL0256N
	$(call BuildFirmware/Generic,$(1),all0256n,board=ALL0256N $(call mkmtd/spi,$(mtdlayout_4M)),851968,3014656)
endef

define Image/Build/Profile/ALL0256N
	$(call Image/Build/Template/$(fs_squash)/$(1),ALL0256)
endef

define Image/Build/Profile/NBG419N
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,nbg-419n,NBG419N,ttyS1,57600,phys)
endef

define Image/Build/Profile/WL351
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,wl-351,WL-351,ttyS1,57600,phys)
endef

define Image/Build/Profile/NW718
	$(call Image/Build/Template/$(fs_squash)/$(1),NW718,nw718,NW718)
endef

define Image/Build/Profile/OMNIEMB
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,omni-emb,OMNI-EMB,ttyS1,57600,phys)
endef

define Image/Build/Profile/PWH2004
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,pwh2004,PWH2004,ttyS1,57600,phys)
endef

define Image/Build/Profile/V22RW2X2
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,v22rw-2x2,V22RW-2X2,ttyS1,57600,phys)
endef

mtdlayout_whrg300n=192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,960k(kernel),2752k(rootfs),64k(user)ro,3712k@0x50000(firmware)
define Image/Build/Template/WHRG300N
	$(call Image/Build/Template/GENERIC_4M,$(1),whr-g300n,WHR-G300N,ttyS1,57600,phys)
	$(call BuildFirmware/Buffalo,$(1),whr-g300n,$(call mkcmdline,WHR-G300N,ttyS1,57600) $(call mkmtd/phys,$(mtdlayout_whrg300n)),983040,3801088)
	( \
		echo -n -e "# Airstation FirmWare\nrun u_fw\nreset\n\n" | \
			dd bs=512 count=1 conv=sync; \
		dd if=$(call sysupname,$(1),whr-g300n); \
	) > $(KDIR)/whr-g300n-tftp.tmp
	buffalo-tftp -i $(KDIR)/whr-g300n-tftp.tmp \
		-o $(call imgname,$(1),whr-g300n)-tftp.bin
endef

define Image/Build/Profile/WHRG300N
	$(call Image/Build/Template/$(fs_squashfs)/$(1),WHRG300N)
endef

define Image/Build/Profile/HW5503G
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,hw550-3g,HW550-3G,ttyS1,57600,phys)
endef

define Image/Build/Profile/MOFI35003GN
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,mofi3500-3gn,MOFI3500-3GN,ttyS1,57600,phys)
endef

define Image/Build/Profile/WCR150GN
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,wcr150gn,WCR150GN,ttyS1,57600,phys)
endef

define Image/Build/Profile/W502U
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,w502u,W502U,ttyS1,115200,phys)
endef

mtdlayout_argus_atp52b=192k(bootloader)ro,64k(config),64k(factory),1152k(kernel),6656k(rootfs),7808k@0x50000(firmware)
mtd_argus_atp52b_kernel_part_size=1179648
mtd_argus_atp52b_rootfs_part_size=6815744
define Image/Build/Template/ARGUS_ATP52B
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(call mkmtd/phys,$(mtdlayout_argus_atp52b)),$(mtd_argus_atp52b_kernel_part_size),$(mtd_argus_atp52b_rootfs_part_size))
endef

define Image/Build/Profile/ARGUS_ATP52B
	$(call Image/Build/Template/$(fs_squash)/$(1),ARGUS_ATP52B,argus_atp52b,ARGUS_ATP52B)
endef

mtdlayout_f5d8235=320k(u-boot)ro,1536k(kernel),6208k(rootfs),64k(nvram),64k(factory),7744k@0x50000(firmware)
mtd_f5d8235_kernel_part_size=1572864
mtd_f5d8235_rootfs_part_size=6356992
define Image/Build/Template/F5D8235
	$(call BuildFirmware/Generic,$(1),$(2),board=$(3) $(call mkmtd/phys,$(mtdlayout_f5d8235)),$(mtd_f5d8235_kernel_part_size),$(mtd_f5d8235_rootfs_part_size))
endef

define Image/Build/Profile/F5D8235V2
	$(call Image/Build/Template/$(fs_squash)/$(1),F5D8235,f5d8235v2,F5D8235_V2)
endef

define Image/Build/Profile/WR5123GN
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,wr512-3gn-4M,WR512-3GN,ttyS1,57600,phys)
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_8M,wr512-3gn-8M,WR512-3GN,ttyS1,57600,phys)
endef

define Image/Build/Profile/SLR7205
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,sl-r7205,SL-R7205,ttyS1,57600,phys)
endef

ifeq ($(CONFIG_RALINK_RT305X),y)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/ARGUS_ATP52B,$(1))
	$(call Image/Build/Profile/BC2,$(1))
	$(call Image/Build/Profile/ALL0256N,$(1))
	$(call Image/Build/Profile/DIR300B1,$(1))
	$(call Image/Build/Profile/ESR9753,$(1))
	$(call Image/Build/Profile/RTG32B1,$(1))
	$(call Image/Build/Profile/FONERA20N,$(1))
	$(call Image/Build/Profile/V22RW2X2,$(1))
	$(call Image/Build/Profile/NBG419N,$(1))
	$(call Image/Build/Profile/NW718,$(1))
	$(call Image/Build/Profile/OMNIEMB,$(1))
	$(call Image/Build/Profile/PWH2004,$(1))
	$(call Image/Build/Profile/SLR7205,$(1))
	$(call Image/Build/Profile/WHRG300N,$(1))
	$(call Image/Build/Profile/WCR150GN,$(1))
	$(call Image/Build/Profile/F5D8235V2,$(1))
	$(call Image/Build/Profile/HW5503G,$(1))
	$(call Image/Build/Profile/MOFI35003GN,$(1))
	$(call Image/Build/Profile/W502U,$(1))
	$(call Image/Build/Profile/WR5123GN,$(1))
	$(call Image/Build/Profile/WL341V3,$(1))
	$(call Image/Build/Profile/WL351,$(1))
	$(call Image/Build/Profile/WR6202,$(1))
endef
endif

define Image/Build/Profile/F5D8235V1
	$(call Image/Build/Template/$(fs_squash)/$(1),F5D8235,f5d8235v1,F5D8235_V1)
endef

define Image/Build/Profile/RTN15
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,rt-n15,RT-N15,ttyS1,57600,phys)
endef

define Image/Build/Profile/V11STFE
	$(call Image/Build/Template/$(fs_squash)/$(1),GENERIC_4M,v11st-fe,V11ST-FE,ttyS1,57600,phys)
endef

mtdlayout_wlitx4ag300n=192k(u-boot)ro,64k(u-boot-env)ro,64k(factory)ro,832k(kernel),2880k(rootfs),64k(user)ro,3712k@0x50000(firmware)
define Image/Build/Template/WLITX4AG300N
	$(call BuildFirmware/Buffalo2,$(1),wli-tx4-ag300n,WLI-TX4-AG300N,$(call mkmtd/phys,$(mtdlayout_wlitx4ag300n)),851968,2949120)
endef

define Image/Build/Profile/WLITX4AG300N
	$(call Image/Build/Template/$(fs_squash)/$(1),WLITX4AG300N)
endef

ifeq ($(CONFIG_RALINK_RT288X),y)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/F5D8235V1,$(1))
	$(call Image/Build/Profile/RTN15,$(1))
	$(call Image/Build/Profile/V11STFE,$(1))
	$(call Image/Build/Profile/WLITX4AG300N,$(1))
endef
endif

define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),initramfs)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
