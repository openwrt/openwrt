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

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),initramfs)
endef
endif

ifeq ($(CONFIG_SOC_RT288X),y)
define kernel_entry
-a 0x88000000 -e 0x88000000
endef
else
ifeq ($(CONFIG_SOC_MT7621),y)
define kernel_entry
-a 0x80001000 -e 0x80001000
endef
else
define kernel_entry
-a 0x80000000 -e 0x80000000
endef
endif
endif

define MkCombineduImage
	$(call PatchKernelLzma,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt `expr $(4) - 64` ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big" >&2; \
	else if [ `stat -c%s "$(KDIR)/root.$(1)"` -gt $(5) ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big" >&2; \
	else \
		( dd if=$(KDIR)/vmlinux-$(2).bin.lzma bs=`expr $(4) - 64` conv=sync ; dd if=$(KDIR)/root.$(1) ) > $(KDIR)/vmlinux-$(2).bin.lzma.combined ; \
	fi ; fi
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(2).bin.lzma.combined,$(call sysupname,$(1),$(2)),$(6))
endef


#
# The real magic happens inside these templates
#

define MkImage
	$(eval imagename=$(if $(4),$(4),MIPS OpenWrt Linux-$(LINUX_VERSION)))
	-mkimage -A mips -O linux -T kernel -C $(1) $(call kernel_entry)\
		-n "$(imagename)" \
		-d $(2) $(3)
endef

define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define MkImageSysupgrade/squashfs
	$(eval output_name=$(IMG_PREFIX)-$(2)-$(1)-$(if $(4),$(4),sysupgrade).bin)
	cat $(KDIR)/vmlinux-$(2).uImage $(KDIR)/root.$(1) > $(KDIR)/$(output_name)
	$(call prepare_generic_squashfs,$(KDIR)/$(output_name))
	if [ `stat -c%s "$(KDIR)/$(output_name)"` -gt $(3) ]; then \
		echo "Warning: $(KDIR)/$(output_name) is too big" >&2; \
	else \
		$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name); \
	fi
endef

define PatchKernelLzmaDtb
	cp $(KDIR)/vmlinux$(3) $(KDIR)/vmlinux-$(1)$(3)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb -o $(KDIR)/$(2).dtb ../dts/$(2).dts
	$(STAGING_DIR_HOST)/bin/patch-dtb $(KDIR)/vmlinux-$(1)$(3) $(KDIR)/$(2).dtb
	$(call CompressLzma,$(KDIR)/vmlinux-$(1)$(3),$(KDIR)/vmlinux-$(1)$(3).bin.lzma)
endef

define MkImageLzmaDtb
	$(call PatchKernelLzmaDtb,$(1),$(2),$(4))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(1)$(4).bin.lzma,$(KDIR)/vmlinux-$(1)$(4).uImage,$(3))
endef

define BuildFirmware/OF
	$(call MkImageLzmaDtb,$(2),$(3),$(5))
	$(call MkImageSysupgrade/$(1),$(1),$(2),$(4),$(6))
endef

define BuildFirmware/OF/initramfs
	$(call MkImageLzmaDtb,$(2),$(3),$(4),-initramfs)
	$(CP) $(KDIR)/vmlinux-$(2)-initramfs.uImage $(call imgname,$(1),$(2))-uImage.bin
endef


# Build images for default ralink layout for 4MB flash
# kernel + roots = 0x3b0000
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
ralink_default_fw_size_4M=3866624
BuildFirmware/Default4M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_4M))
BuildFirmware/Default4M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# Build images for default ralink layout for 8MB flash
# kernel + roots = 0x7b0000
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
ralink_default_fw_size_8M=8060928
BuildFirmware/Default8M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_8M),$(4))
BuildFirmware/Default8M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

ralink_default_fw_size_16M=16121856
BuildFirmware/Default16M/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(ralink_default_fw_size_16M),$(4))
BuildFirmware/Default16M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))

# Build images for a custom sized flash layout
# $(1) = squashfs/initramfs
# $(2) = lowercase board name
# $(3) = dts file
# $(4) = kernel + rootfs size
BuildFirmware/CustomFlash/squashfs=$(call BuildFirmware/OF,$(1),$(2),$(3),$(4),$(5),$(6))
BuildFirmware/CustomFlash/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# wrappers for boards that have 4MB and 8MB versions
define BuildFirmware/DefaultDualSize/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2)-4M,$(3)-4M)
	$(call BuildFirmware/Default8M/$(1),$(1),$(2)-8M,$(3)-8M)
endef
define BuildFirmware/DefaultDualSize/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-4M,$(3)-4M)
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-8M,$(3)-8M)
endef

# Some boards need a special header inside the uImage to make them bootable
define BuildFirmware/CustomFlashFactory/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(4))
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(4),$(5),$(6))
endef
BuildFirmware/CustomFlashFactory/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# sign an image to make it work with edimax tftp recovery
define BuildFirmware/Edimax/squashfs
	$(call BuildFirmware/OF,$(1),$(2),$(3),$(4))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkedimaximg -i $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin \
			-s $(5) -m $(6) -f $(7) -S $(8); \
	fi
endef
BuildFirmware/Edimax/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# build Seama header images
define BuildFirmware/Seama/squashfs
	$(call BuildFirmware/OF,$(1),$(2),$(3),$(5))
	cat $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/root.$(1) > $(KDIR)/img_$(2).$(1).tmp
	if [ `stat -c%s "$(KDIR)/img_$(2).$(1).tmp"` -gt $$$$(($(5) - 64)) ]; then \
		echo "Warning: $(KDIR)/img_$(2).$(1).tmp is too big" >&2; \
	else \
		dd if=$(KDIR)/vmlinux-$(2).bin.lzma of=$(KDIR)/vmlinux-$(2).bin.lzma.padded bs=64k conv=sync; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).bin.lzma.padded bs=1 count=`expr \`stat -c%s $(KDIR)/vmlinux-$(2).bin.lzma.padded\` - 64`; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
		) > $(KDIR)/vmlinux-$(2).tmp; \
		$(STAGING_DIR_HOST)/bin/seama \
			-i $(KDIR)/vmlinux-$(2).tmp \
			-m "dev=/dev/mtdblock/2" -m "type=firmware"; \
		$(STAGING_DIR_HOST)/bin/seama \
			-s $(call imgname,$(1),$(2))-factory.bin \
			-m "signature=$(4)" \
			-i $(KDIR)/vmlinux-$(2).tmp.seama; \
	fi
endef
BuildFirmware/Seama/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# sign Buffalo images
define BuildFirmware/Buffalo
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

# sign trednet / UMedia images
define BuildFirmware/UMedia/squashfs
	$(call BuildFirmware/Default8M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		fix-u-media-header -T 0x46 -B $(4) \
			-i $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/UMedia/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# sign dap 1350 based images
dap1350_mtd_size=7667712
define BuildFirmware/dap1350/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(dap1350_mtd_size))
	-mkdapimg -s $(4) \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/dap1350/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

# Sign Poray images
define BuildFirmware/Poray4M/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkporayfw -B $(3) -F 4M \
			-f $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/Poray4M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/Poray8M/squashfs
	$(call BuildFirmware/Default8M/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		mkporayfw -B $(3) -F 8M \
			-f $(call sysupname,$(1),$(2)) \
			-o $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/Poray8M/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/PorayDualSize/squashfs
	$(call BuildFirmware/DefaultDualSize/$(1),$(1),$(2),$(3))
	if [ -e "$(call sysupname,$(1),$(2)-4M)" ]; then \
		mkporayfw -B $(3) -F 4M \
			-f $(call sysupname,$(1),$(2)-4M) \
			-o $(call imgname,$(1),$(2))-4M-factory.bin; \
	fi
	if [ -e "$(call sysupname,$(1),$(2)-8M)" ]; then \
		mkporayfw -B $(3) -F 8M \
			-f $(call sysupname,$(1),$(2)-8M) \
			-o $(call imgname,$(1),$(2))-8M-factory.bin; \
	fi
endef
BuildFirmware/PorayDualSize/initramfs=$(call BuildFirmware/DefaultDualSize/initramfs,$(1),$(2),$(3))

#
# RT288X Profiles
#

# 0x790000
belkin_f5d8235v1_mtd_size=7929856
Image/Build/Profile/F5D8235V1=$(call BuildFirmware/CustomFlash/$(1),$(1),f5d8235v1,F5D8235_V1,$(belkin_f5d8235v1_mtd_size))

Image/Build/Profile/RTN15=$(call BuildFirmware/Default4M/$(1),$(1),rt-n15,RT-N15)

Image/Build/Profile/V11STFE=$(call BuildFirmware/Default4M/$(1),$(1),v11st-fe,V11STFE)

Image/Build/Profile/WLITX4AG300N=$(call BuildFirmware/Default4M/$(1),$(1),wli-tx4-ag300n,WLI-TX4-AG300N)

ifeq ($(SUBTARGET),rt288x)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/F5D8235V1,$(1))
	$(call Image/Build/Profile/RTN15,$(1))
	$(call Image/Build/Profile/V11STFE,$(1))
	$(call Image/Build/Profile/WLITX4AG300N,$(1))
endef
endif



#
# RT305X Profiles
#

Image/Build/Profile/DIR610A1=$(call BuildFirmware/Seama/$(1),$(1),dir-610-a1,DIR-610-A1,wrgn59_dlob.hans_dir610,$(ralink_default_fw_size_4M))

edimax_3g6200n_mtd_size=3735552
Image/Build/Profile/3G6200N=$(call BuildFirmware/Edimax/$(1),$(1),3g-6200n,3G-6200N,$(edimax_3g6200n_mtd_size),CSYS,3G62,0x50000,0xc0000)

Image/Build/Profile/3G6200NL=$(call BuildFirmware/Edimax/$(1),$(1),3g-6200nl,3G-6200NL,$(edimax_3g6200n_mtd_size),CSYS,3G62,0x50000,0xc0000)

Image/Build/Profile/3G300M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),3g300m,3G300M,$(ralink_default_fw_size_4M),3G150M_SPI Kernel Image,factory)

Image/Build/Profile/AIR3GII=$(call BuildFirmware/Default4M/$(1),$(1),air3gii,AIR3GII)

define BuildFirmware/UIMAGE_8M
	$(call MkCombineduImage,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)),$(kernel_size_8M),$(rootfs_size_8M),$(7))
endef
define Image/Build/Profile/ALL02393G
	$(call Image/Build/Template/$(image_type)/$(1),UIMAGE_8M,all0239-3g,ALL0239-3G,ttyS1,57600,phys)
endef

Image/Build/Profile/ALL0256N=$(call BuildFirmware/DefaultDualSize/$(1),$(1),all0256n,ALL0256N)

allnet_all5002_mtd_size=33226752
Image/Build/Profile/ALL5002=$(call BuildFirmware/CustomFlash/$(1),$(1),all5002,ALL5002,$(allnet_all5002_mtd_size))
Image/Build/Profile/ALL5003=$(call BuildFirmware/CustomFlash/$(1),$(1),all5003,ALL5003,$(allnet_all5002_mtd_size))

argus_atp52b_mtd_size=7995392
Image/Build/Profile/ARGUS_ATP52B=$(call BuildFirmware/CustomFlash/$(1),$(1),argus_atp52b,ARGUS_ATP52B,$(argus_atp52b_mtd_size))

asl26555_8mb_mtd_size=7929856
asl26555_16mb_mtd_size=16252928
define BuildFirmware/ASL26555/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2)-8M,$(3)-8M,$(asl26555_8mb_mtd_size))
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2)-16M,$(3)-16M,$(asl26555_16mb_mtd_size))
endef
define BuildFirmware/ASL26555/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-8M,$(3)-8M)
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-16M,$(3)-16M)
endef
Image/Build/Profile/ASL26555=$(call BuildFirmware/ASL26555/$(1),$(1),asl26555,ASL26555)

Image/Build/Profile/AWM002EVB=$(call BuildFirmware/DefaultDualSize/$(1),$(1),awm002-evb,AWM002-EVB)

Image/Build/Profile/BC2=$(call BuildFirmware/Default8M/$(1),$(1),bc2,BC2)

broadway_mtd_size=7929856
Image/Build/Profile/BROADWAY=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),broadway,BROADWAY,$(broadway_mtd_size),Broadway Kernel Image,factory)

Image/Build/Profile/CARAMBOLA=$(call BuildFirmware/Default8M/$(1),$(1),carambola,CARAMBOLA)

Image/Build/Profile/D105=$(call BuildFirmware/Default4M/$(1),$(1),d105,D105)

define BuildFirmware/DIR300B1/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-mkwrgimg -s $(4) -d /dev/mtdblock/2 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/DIR300B1/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))
Image/Build/Profile/DIR-300-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-300-b1,DIR-300-B1,wrgn23_dlwbr_dir300b)
Image/Build/Profile/DIR-600-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b1,DIR-600-B1,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-600-B2=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b2,DIR-600-B2,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-615-D=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-615-d,DIR-615-D,wrgn23_dlwbr_dir615d)

Image/Build/Profile/DIR-620-A1=$(call BuildFirmware/Default8M/$(1),$(1),dir-620-a1,DIR-620-A1)
Image/Build/Profile/DIR-620-D1=$(call BuildFirmware/Default8M/$(1),$(1),dir-620-d1,DIR-620-D1)

define BuildFirmware/DIR615H1/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),dir-615-h1,DIR-615-H1)
	-mkdir615h1 -e $(call sysupname,$(1),dir-615-h1) \
		-o $(call imgname,$(1),dir-615-h1)-factory.bin
endef
BuildFirmware/DIR615H1/initramfs=$(call BuildFirmware/OF/initramfs,$(1),dir-615-h1,DIR-615-H1)
Image/Build/Profile/DIR615H1=$(call BuildFirmware/DIR615H1/$(1),$(1))

Image/Build/Profile/DAP1350=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350,DAP-1350,RT3052-AP-DAP1350-3)

Image/Build/Profile/DAP1350WW=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350WW,DAP-1350,RT3052-AP-DAP1350WW-3)

define BuildFirmware/DCS930/squashfs
	$(call BuildFirmware/Default4M/squashfs,squashfs,$(2),$(3))
	dd if=$(KDIR)/vmlinux-$(2).bin.lzma of=$(KDIR)/image.$(2).combined bs=1048512 count=1 conv=sync
	cat $(KDIR)/root.squashfs >> $(KDIR)/image.$(2).combined
        $(call MkImage,lzma,$(KDIR)/image.$(2).combined,$(KDIR)/image.$(2))
	$(call prepare_generic_squashfs,$(KDIR)/image.$(2))
	if [ `stat -c%s "$(KDIR)/image.$(2)"` -gt $(ralink_default_fw_size_4M) ]; then \
		echo "Warning: $(KDIR)/image.$(2)  is too big" >&2; \
	else \
		dd if=$(KDIR)/image.$(2) of=$(KDIR)/dcs.tmp bs=64K count=5 conv=sync ;\
		cat $(KDIR)/image.$(2) >> $(KDIR)/dcs.tmp ; \
		dd if=$(KDIR)/dcs.tmp of=$(call imgname,$(1),$(2))-factory.bin bs=4096k count=1 conv=sync ;\
		$(STAGING_DIR_HOST)/bin/mkdcs932 $(call imgname,$(1),$(2))-factory.bin ; \
	fi
endef
BuildFirmware/DCS930/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))
Image/Build/Profile/DCS930=$(call BuildFirmware/DCS930/$(1),$(1),dcs-930,DCS-930)

Image/Build/Profile/ESR-9753=$(call BuildFirmware/Default4M/$(1),$(1),esr-9753,ESR-9753)

Image/Build/Profile/HW550-3G=$(call BuildFirmware/Default8M/$(1),$(1),hw550-3g,HW550-3G)

belkin_f5d8235v2_mtd_size=7929856
Image/Build/Profile/F5D8235V2=$(call BuildFirmware/CustomFlash/$(1),$(1),f5d8235v2,F5D8235_V2,$(belkin_f5d8235v2_mtd_size))

# 0x770000
belkin_f7c027_mtd_size=7798784
Image/Build/Profile/F7C027=$(call BuildFirmware/CustomFlash/$(1),$(1),belkinf7c027,F7C027,$(belkin_f7c027_mtd_size))

Image/Build/Profile/FONERA20N=$(call BuildFirmware/Edimax/$(1),$(1),fonera20n,FONERA20N,$(ralink_default_fw_size_8M),RSDK,NL1T,0x50000,0xc0000)

Image/Build/Profile/RT-N13U=$(call BuildFirmware/Default8M/$(1),$(1),rt-n13u,RT-N13U)

#Image/Build/Profile/HG255D=$(call BuildFirmware/Default16M/$(1),$(1),hg255d,HG255D)

Image/Build/Profile/FREESTATION5=$(call BuildFirmware/Default8M/$(1),$(1),freestation5,FREESTATION5)

Image/Build/Profile/IP2202=$(call BuildFirmware/Default8M/$(1),$(1),ip2202,IP2202)

Image/Build/Profile/M3=$(call BuildFirmware/Poray4M/$(1),$(1),m3,M3)

Image/Build/Profile/M4=$(call BuildFirmware/PorayDualSize/$(1),$(1),m4,M4)

Image/Build/Profile/MOFI3500-3GN=$(call BuildFirmware/Default8M/$(1),$(1),mofi3500-3gn,MOFI3500-3GN)

# Kernel name should be "Linux Kernel Image" to make the OpenWrt image installable from factory Web UI
Image/Build/Profile/MPRA1=$(call BuildFirmware/Default4M/$(1),$(1),mpr-a1,MPRA1,Linux Kernel Image)
Image/Build/Profile/MPRA2=$(call BuildFirmware/Default8M/$(1),$(1),mpr-a2,MPRA2,Linux Kernel Image)

Image/Build/Profile/DIR-300-B7=$(call BuildFirmware/Default4M/$(1),$(1),dir-300-b7,DIR-300-B7)
Image/Build/Profile/DIR-320-B1=$(call BuildFirmware/Default8M/$(1),$(1),dir-320-b1,DIR-320-B1)

Image/Build/Profile/NBG-419N=$(call BuildFirmware/Default4M/$(1),$(1),nbg-419n,NBG-419N)

Image/Build/Profile/MZKW300NH2=$(call BuildFirmware/Edimax/$(1),$(1),mzk-w300nh2,MZK-W300NH2,$(mzkw300nh2_mtd_size),CSYS,RN52,0x50000,0xc0000)

nw718_mtd_size=3801088
Image/Build/Profile/NW718=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),nw718m,NW718,$(nw718_mtd_size),ARA1B4NCRNW718;1,factory)

Image/Build/Profile/OMNI-EMB=$(call BuildFirmware/Default8M/$(1),$(1),omni-emb,OMNI-EMB)

Image/Build/Profile/PSR-680W=$(call BuildFirmware/Default4M/$(1),$(1),psr-680w,PSR-680W)

Image/Build/Profile/PWH2004=$(call BuildFirmware/Default8M/$(1),$(1),pwh2004,PWH2004)

Image/Build/Profile/RTG32B1=$(call BuildFirmware/Default4M/$(1),$(1),rt-g32-b1,RT-G32-B1)

Image/Build/Profile/RTN10PLUS=$(call BuildFirmware/Default4M/$(1),$(1),rt-n10-plus,RT-N10-PLUS)

Image/Build/Profile/SL-R7205=$(call BuildFirmware/Default4M/$(1),$(1),sl-r7205,SL-R7205)

Image/Build/Profile/V22RW-2X2=$(call BuildFirmware/Default4M/$(1),$(1),v22rw-2x2,V22RW-2X2)

Image/Build/Profile/W150M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w150m,W150M,$(ralink_default_fw_size_4M),W150M Kernel Image,factory)

Image/Build/Profile/W306R_V20=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w306r-v20,W306R_V20,$(ralink_default_fw_size_4M),linkn Kernel Image,factory)

Image/Build/Profile/W502U=$(call BuildFirmware/Default8M/$(1),$(1),w502u,W502U)

Image/Build/Profile/WCR150GN=$(call BuildFirmware/Default4M/$(1),$(1),wcr150gn,WCR150GN)

buffalo_whrg300n_mtd_size=3801088
define BuildFirmware/WHRG300N/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),whr-g300n,WHR-G300N)
	# the following line has a bad argument 3 ... the old Makefile was already broken	
	$(call BuildFirmware/Buffalo,$(1),whr-g300n,whr-g300n)
	( \
		echo -n -e "# Airstation FirmWare\nrun u_fw\nreset\n\n" | \
			dd bs=512 count=1 conv=sync; \
		dd if=$(call sysupname,$(1),whr-g300n); \
	) > $(KDIR)/whr-g300n-tftp.tmp
	buffalo-tftp -i $(KDIR)/whr-g300n-tftp.tmp \
		-o $(call imgname,$(1),whr-g300n)-tftp.bin
endef
BuildFirmware/WHRG300N/initramfs=$(call BuildFirmware/OF/initramfs,$(1),whr-g300n,WHR-G300N)
Image/Build/Profile/WHRG300N=$(call BuildFirmware/WHRG300N/$(1),$(1))

Image/Build/Profile/WL_330N=$(call BuildFirmware/Default4M/$(1),$(1),wl-330n,WL_330N)

Image/Build/Profile/WL_330N3G=$(call BuildFirmware/Default4M/$(1),$(1),wl-330n3g,WL_330N3G)

kernel_size_wl341v3=917504
rootfs_size_wl341v3=2949120
define BuildFirmware/WL341V3/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),wl341v3,WL341V3)
	# This code looks broken and really needs to be converted to C
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=/dev/zero bs=195936 count=1; \
			echo "1.01"; \
			dd if=/dev/zero bs=581 count=1; \
			echo -n -e "\x73\x45\x72\x43\x6F\x4D\x6D\x00\x01\x00\x00\x59\x4E\x37\x95\x58\x10\x00\x20\x00\x28\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x03\x00\x00\x80\x00\x00\x00\x00\x03\x00\x00\x10\x12\x00\x00\x00\x10\x73\x45\x72\x43\x6F\x4D\x6D"; \
			dd if=/dev/zero bs=65552 count=1; \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=$(kernel_size_wl341v3) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k conv=sync; \
			dd if=/dev/zero bs=`expr 4194304 - 262144 - 16 - $(kernel_size_wl341v3) - \( \( \( \`stat -c%s $(KDIR)/root.$(1)\` / 65536 \) + 1 \) \* 65536 \)` count=1; \
			echo -n -e "\x11\x03\x80\x00\x10\x12\x90\xF7\x65\x52\x63\x4F\x6D\x4D\x00\x00"; \
		) > $(call imgname,$(1),$(2))-factory.bin; \
	fi
endef
BuildFirmware/WL341V3/initramfs=$(call BuildFirmware/OF/initramfs,$(1),wl341v3,WL341V3)
Image/Build/Profile/WL341V3=$(call BuildFirmware/WL341V3/$(1),$(1))

Image/Build/Profile/WL-351=$(call BuildFirmware/Default4M/$(1),$(1),wl-351,WL-351)

define BuildFirmware/WNCE2001/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-mkdapimg -s RT3052-AP-WNCE2001-3 -r WW -v 1.0.0.99 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory-worldwide.bin
	-mkdapimg -s RT3052-AP-WNCE2001-3 -r NA -v 1.0.0.99 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory-northamerica.bin
endef
BuildFirmware/WNCE2001/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))
Image/Build/Profile/WNCE2001=$(call BuildFirmware/WNCE2001/$(1),$(1),wnce2001,WNCE2001)

Image/Build/Profile/WR512-3GN=$(call BuildFirmware/DefaultDualSize/$(1),$(1),wr512-3ng,WR512-3GN)

Image/Build/Profile/UR-326N4G=$(call BuildFirmware/Default4M/$(1),$(1),ur-326n4g,UR-326N4G)

Image/Build/Profile/UR-336UN=$(call BuildFirmware/Default8M/$(1),$(1),ur-336un,UR-336UN)

Image/Build/Profile/WR6202=$(call BuildFirmware/Default8M/$(1),$(1),wr6202,WR6202)

Image/Build/Profile/X5=$(call BuildFirmware/Poray8M/$(1),$(1),x5,X5)

Image/Build/Profile/X8=$(call BuildFirmware/Poray8M/$(1),$(1),x8,X8)

Image/Build/Profile/XDXRN502J=$(call BuildFirmware/Default4M/$(1),$(1),xdxrn502j,XDXRN502J)

ifeq ($(SUBTARGET),rt305x)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/3G6200N,$(1))
	$(call Image/Build/Profile/3G6200NL,$(1))
	$(call Image/Build/Profile/3G300M,$(1))
	$(call Image/Build/Profile/AIR3GII,$(1))
	$(call Image/Build/Profile/ALL02393G,$(1))
	$(call Image/Build/Profile/ALL0256N,$(1))
	$(call Image/Build/Profile/ALL5002,$(1))
	$(call Image/Build/Profile/ALL5003,$(1))
	$(call Image/Build/Profile/ARGUS_ATP52B,$(1))
	$(call Image/Build/Profile/ASL26555,$(1))
	$(call Image/Build/Profile/AWM002EVB,$(1))
	$(call Image/Build/Profile/BC2,$(1))
	$(call Image/Build/Profile/BROADWAY,$(1))
	$(call Image/Build/Profile/CARAMBOLA,$(1))
	$(call Image/Build/Profile/D105,$(1))
	$(call Image/Build/Profile/DIR-300-B1,$(1))
	$(call Image/Build/Profile/DIR-600-B1,$(1))
	$(call Image/Build/Profile/DIR-300-B7,$(1))
	$(call Image/Build/Profile/DIR-320-B1,$(1))
	$(call Image/Build/Profile/DIR-600-B2,$(1))
	$(call Image/Build/Profile/DIR610A1,$(1))
	$(call Image/Build/Profile/DIR-615-D,$(1))
	$(call Image/Build/Profile/DIR-620-A1,$(1))
	$(call Image/Build/Profile/DIR-620-D1,$(1))
	$(call Image/Build/Profile/DIR615H1,$(1))
	$(call Image/Build/Profile/DAP1350,$(1))
	$(call Image/Build/Profile/DAP1350WW,$(1))
	$(call Image/Build/Profile/DCS930,$(1))
	$(call Image/Build/Profile/ESR-9753,$(1))
	$(call Image/Build/Profile/F7C027,$(1))
	$(call Image/Build/Profile/F5D8235V2,$(1))
	$(call Image/Build/Profile/FONERA20N,$(1))
	$(call Image/Build/Profile/FREESTATION5,$(1))
#	$(call Image/Build/Profile/HG255D,$(1))
	$(call Image/Build/Profile/HW550-3G,$(1))
	$(call Image/Build/Profile/IP2202,$(1))
	$(call Image/Build/Profile/M3,$(1))
	$(call Image/Build/Profile/M4,$(1))
	$(call Image/Build/Profile/MOFI3500-3GN,$(1))
	$(call Image/Build/Profile/MPRA2,$(1))
	$(call Image/Build/Profile/MZKW300NH2,$(1))
	$(call Image/Build/Profile/NBG-419N,$(1))
	$(call Image/Build/Profile/NW718,$(1))
	$(call Image/Build/Profile/OMNI-EMB,$(1))
	$(call Image/Build/Profile/PSR-680W,$(1))
	$(call Image/Build/Profile/PWH2004,$(1))
	$(call Image/Build/Profile/RTG32B1,$(1))
	$(call Image/Build/Profile/RTN10PLUS,$(1))
	$(call Image/Build/Profile/RT-N13U,$(1))
	$(call Image/Build/Profile/SL-R7205,$(1))
	$(call Image/Build/Profile/UR-326N4G,$(1))
	$(call Image/Build/Profile/V22RW-2X2,$(1))
	$(call Image/Build/Profile/W150M,$(1))
	$(call Image/Build/Profile/W306R_V20,$(1))
	$(call Image/Build/Profile/W502U,$(1))
	$(call Image/Build/Profile/WCR150GN,$(1))
	$(call Image/Build/Profile/WHRG300N,$(1))
	$(call Image/Build/Profile/WL_330N,$(1))
	$(call Image/Build/Profile/WL_330N3G,$(1))
	$(call Image/Build/Profile/WL341V3,$(1))
	$(call Image/Build/Profile/WL-351,$(1))
	$(call Image/Build/Profile/WNCE2001,$(1))
	$(call Image/Build/Profile/WR512-3GN,$(1))
	$(call Image/Build/Profile/WR6202,$(1))
	$(call Image/Build/Profile/X5,$(1))
	$(call Image/Build/Profile/X8,$(1))
	$(call Image/Build/Profile/XDXRN502J,$(1))
endef
endif



#
# RT3662/RT3883 Profiles
#
Image/Build/Profile/DIR645=$(call BuildFirmware/Seama/$(1),$(1),dir-645,DIR-645,wrgn39_dlob.hans_dir645,$(ralink_default_fw_size_8M))

omniembhpm_mtd_size=16449536
Image/Build/Profile/OMNIEMBHPM=$(call BuildFirmware/CustomFlash/$(1),$(1),omni-emb-hpm,OMNI-EMB-HPM,$(omniembhpm_mtd_size))

Image/Build/Profile/RTN56U=$(call BuildFirmware/Default8M/$(1),$(1),rt-n56u,RTN56U)

Image/Build/Profile/TEW691GR=$(call BuildFirmware/UMedia/$(1),$(1),tew-691gr,TEW-691GR,0x026910)

Image/Build/Profile/TEW692GR=$(call BuildFirmware/UMedia/$(1),$(1),tew-692gr,TEW-692GR,0x026920)

ifeq ($(SUBTARGET),rt3883)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/DIR645,$(1))
	$(call Image/Build/Profile/OMNIEMBHPM,$(1))
	$(call Image/Build/Profile/RTN56U,$(1))
	$(call Image/Build/Profile/TEW691GR,$(1))
	$(call Image/Build/Profile/TEW692GR,$(1))
endef
endif

#
# MT7620A Profiles
#

Image/Build/Profile/MT7620a=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a,MT7620a)
Image/Build/Profile/MT7620a_MT7610e=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_mt7610e,MT7620a_MT7610e)
Image/Build/Profile/MT7620a_MT7530=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_mt7530,MT7620a_MT7530)
Image/Build/Profile/MT7620a_V22SG=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_v22sg,MT7620a_V22SG)
Image/Build/Profile/RP-N53=$(call BuildFirmware/Default8M/$(1),$(1),rp_n53,RP-N53)
whr_300hp2_mtd_size=7012352
Image/Build/Profile/WHR-300HP2=$(call BuildFirmware/CustomFlash/$(1),$(1),whr-300hp2,WHR-300HP2,$(whr_300hp2_mtd_size))
Image/Build/Profile/WHR-600D=$(call BuildFirmware/CustomFlash/$(1),$(1),whr-600d,WHR-600D,$(whr_300hp2_mtd_size))
dlink810l_mtd_size=6881280
Image/Build/Profile/DIR-810L=$(call BuildFirmware/CustomFlash/$(1),$(1),dir-810l,DIR-810L,$(dlink810l_mtd_size))

ifeq ($(SUBTARGET),mt7620a)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7620a,$(1))
	$(call Image/Build/Profile/MT7620a_MT7610e,$(1))
	$(call Image/Build/Profile/MT7620a_MT7530,$(1))
	$(call Image/Build/Profile/MT7620a_V22SG,$(1))
	$(call Image/Build/Profile/RP-N53,$(1))
	$(call Image/Build/Profile/DIR-810L,$(1))
	$(call Image/Build/Profile/WHR-300HP2,$(1))
	$(call Image/Build/Profile/WHR-600D,$(1))
endef
endif


#
# MT7620N Profiles
#

Image/Build/Profile/WMR300=$(call BuildFirmware/Default8M/$(1),$(1),wmr-300,WMR300)

ifeq ($(SUBTARGET),mt7620n)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/WMR300,$(1))
endef
endif


#
# MT7621 Profiles
#

Image/Build/Profile/MT7621=$(call BuildFirmware/Default4M/$(1),$(1),mt7621,MT7621)

ifeq ($(SUBTARGET),mt7621)
define Image/Build/Profile/Default
	$(call Image/Build/Profile/MT7621,$(1))
endef
endif


#
# Generic Targets
#
define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX).elf
	cp $(KDIR)/vmlinux $(BIN_DIR)/$(VMLINUX).bin
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	cp $(KDIR)/uImage.lzma $(BIN_DIR)/$(UIMAGE).bin
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	cp $(KDIR)/vmlinux-initramfs.elf $(BIN_DIR)/$(VMLINUX)-initramfs.elf
	cp $(KDIR)/vmlinux-initramfs $(BIN_DIR)/$(VMLINUX)-initramfs.bin
	$(call CompressLzma,$(KDIR)/vmlinux-initramfs,$(KDIR)/vmlinux-initramfs.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux-initramfs.bin.lzma,$(KDIR)/uImage-initramfs.lzma)
	cp $(KDIR)/uImage-initramfs.lzma $(BIN_DIR)/$(UIMAGE)-initramfs.bin
endif
	$(call Image/Build/Initramfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
