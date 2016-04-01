#
# RT305X Profiles
#

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

define BuildFirmware/JCG/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-jcgimage -o $(call imgname,$(1),$(2))-factory.bin \
		-u $(call sysupname,$(1),$(2)) -v $(4)
endef

define BuildFirmware/DIR300B1/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),$(2),$(3))
	-mkwrgimg -s $(4) -d /dev/mtdblock/2 \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/DIR300B1/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

define BuildFirmware/DIR615H1/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),dir-615-h1,DIR-615-H1)
	-mksenaofw -e $(call sysupname,$(1),dir-615-h1) \
		-o $(call imgname,$(1),dir-615-h1)-factory.bin \
		-r 0x218 -p 0x30 -t 3
endef
BuildFirmware/DIR615H1/initramfs=$(call BuildFirmware/OF/initramfs,$(1),dir-615-h1,DIR-615-H1)

# sign dap 1350 based images
dap1350_mtd_size=7667712
define BuildFirmware/dap1350/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2),$(3),$(dap1350_mtd_size))
	-mkdapimg -s $(4) \
		-i $(call sysupname,$(1),$(2)) \
		-o $(call imgname,$(1),$(2))-factory.bin
endef
BuildFirmware/dap1350/initramfs=$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3))

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

BuildFirmware/HLKRM04/squashfs=$(call BuildFirmware/Default4M/squashfs,$(1),$(2),$(3),$(4))
define BuildFirmware/HLKRM04/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2),$(3),$(4))
	mkhilinkfw -e -i$(call imgname,$(1),$(2))-uImage.bin -o $(call imgname,$(1),$(2))-factory.bin;
endef

vocore_8mb_mtd_size=8060928
vocore_16mb_mtd_size=16449536
define BuildFirmware/VOCORE/squashfs
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2)-8M,$(3)-8M,$(vocore_8mb_mtd_size))
	$(call BuildFirmware/CustomFlash/$(1),$(1),$(2)-16M,$(3)-16M,$(vocore_16mb_mtd_size))
endef
define BuildFirmware/VOCORE/initramfs
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-8M,$(3)-8M)
	$(call BuildFirmware/OF/initramfs,$(1),$(2)-16M,$(3)-16M)
endef

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

# FIXME: this looks broken
buffalo_whrg300n_mtd_size=3801088
define BuildFirmware/WHRG300N/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),whr-g300n,WHR-G300N)
	# the following line has a bad argument 3 ... the old Makefile was already broken	
	$(call BuildFirmware/Buffalo,$(1),whr-g300n,whr-g300n)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			echo -n -e "# Airstation FirmWare\nrun u_fw\nreset\n\n" | \
				dd bs=512 count=1 conv=sync; \
			dd if=$(call sysupname,$(1),whr-g300n); \
		) > $(KDIR)/whr-g300n-tftp.tmp && \
		buffalo-tftp -i $(KDIR)/whr-g300n-tftp.tmp \
			-o $(call imgname,$(1),whr-g300n)-tftp.bin; \
	fi
endef
BuildFirmware/WHRG300N/initramfs=$(call BuildFirmware/OF/initramfs,$(1),whr-g300n,WHR-G300N)

kernel_size_wl341v3=917504
rootfs_size_wl341v3=2949120
define BuildFirmware/WL-341V3/squashfs
	$(call BuildFirmware/Default4M/$(1),$(1),wl-341v3,WL-341V3)
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
BuildFirmware/WL-341V3/initramfs=$(call BuildFirmware/OF/initramfs,$(1),wl-341v3,WL-341V3)

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



define BuildFirmware/UIMAGE_8M
	$(call MkCombineduImage,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)) $(call mkmtd/$(6),$(mtdlayout_8M)),$(kernel_size_8M),$(rootfs_size_8M),$(7))
endef
define Image/Build/Profile/ALL02393G
	$(call Image/Build/Template/$(image_type)/$(1),UIMAGE_8M,all0239-3g,ALL0239-3G,ttyS1,57600,phys)
endef


Image/Build/Profile/DIR610A1=$(call BuildFirmware/Seama/$(1),$(1),dir-610-a1,DIR-610-A1,wrgn59_dlob.hans_dir610,$(ralink_default_fw_size_4M))
edimax_3g6200n_mtd_size=3735552
Image/Build/Profile/3G6200N=$(call BuildFirmware/Edimax/$(1),$(1),3g-6200n,3G-6200N,$(edimax_3g6200n_mtd_size),CSYS,3G62,0x50000,0xc0000)
Image/Build/Profile/3G6200NL=$(call BuildFirmware/Edimax/$(1),$(1),3g-6200nl,3G-6200NL,$(edimax_3g6200n_mtd_size),CSYS,3G62,0x50000,0xc0000)
Image/Build/Profile/3G300M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),3g300m,3G300M,$(ralink_default_fw_size_4M),3G150M_SPI Kernel Image,factory)
Image/Build/Profile/A5-V11=$(call BuildFirmware/Poray4M/$(1),$(1),a5-v11,A5-V11)
Image/Build/Profile/AIR3GII=$(call BuildFirmware/Default4M/$(1),$(1),air3gii,AIR3GII)
Image/Build/Profile/ALL0256N=$(call BuildFirmware/DefaultDualSize/$(1),$(1),all0256n,ALL0256N)
allnet_all5002_mtd_size=33226752
Image/Build/Profile/ALL5002=$(call BuildFirmware/CustomFlash/$(1),$(1),all5002,ALL5002,$(allnet_all5002_mtd_size))
Image/Build/Profile/ALL5003=$(call BuildFirmware/CustomFlash/$(1),$(1),all5003,ALL5003,$(allnet_all5002_mtd_size))
argus_atp52b_mtd_size=7995392
Image/Build/Profile/ATP-52B=$(call BuildFirmware/CustomFlash/$(1),$(1),atp-52b,ATP-52B,$(argus_atp52b_mtd_size))
Image/Build/Profile/ASL26555=$(call BuildFirmware/ASL26555/$(1),$(1),asl26555,ASL26555)
Image/Build/Profile/AWM002EVB=$(call BuildFirmware/DefaultDualSize/$(1),$(1),awm002-evb,AWM002-EVB)
Image/Build/Profile/AWM003EVB=$(call BuildFirmware/Default8M/$(1),$(1),awm003-evb,AWM003-EVB)
Image/Build/Profile/AWAPN2403=$(call BuildFirmware/Default4M/$(1),$(1),awmapn2403,AWAPN2403)
Image/Build/Profile/BC2=$(call BuildFirmware/Default8M/$(1),$(1),bc2,BC2)
broadway_mtd_size=7929856
Image/Build/Profile/BROADWAY=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),broadway,BROADWAY,$(broadway_mtd_size),Broadway Kernel Image,factory)
Image/Build/Profile/CARAMBOLA=$(call BuildFirmware/Default8M/$(1),$(1),carambola,CARAMBOLA)
Image/Build/Profile/D105=$(call BuildFirmware/Default4M/$(1),$(1),d105,D105)
Image/Build/Profile/JHR-N805R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n805r,JHR-N805R,29.24)
Image/Build/Profile/JHR-N825R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n825r,JHR-N825R,23.24)
Image/Build/Profile/JHR-N926R=$(call BuildFirmware/JCG/$(1),$(1),jhr-n926r,JHR-N926R,25.24)
define Image/Build/Profile/JCG
	$(call Image/Build/Profile/JHR-N805R,$(1))
	$(call Image/Build/Profile/JHR-N825R,$(1))
	$(call Image/Build/Profile/JHR-N926R,$(1))
endef
Image/Build/Profile/DIR-300-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-300-b1,DIR-300-B1,wrgn23_dlwbr_dir300b)
Image/Build/Profile/DIR-600-B1=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b1,DIR-600-B1,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-600-B2=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-600-b2,DIR-600-B2,wrgn23_dlwbr_dir600b)
Image/Build/Profile/DIR-615-D=$(call BuildFirmware/DIR300B1/$(1),$(1),dir-615-d,DIR-615-D,wrgn23_dlwbr_dir615d)
Image/Build/Profile/DIR-620-A1=$(call BuildFirmware/Default8M/$(1),$(1),dir-620-a1,DIR-620-A1)
Image/Build/Profile/DIR-620-D1=$(call BuildFirmware/Default8M/$(1),$(1),dir-620-d1,DIR-620-D1)
Image/Build/Profile/DIR615H1=$(call BuildFirmware/DIR615H1/$(1),$(1))
Image/Build/Profile/DAP1350=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350,DAP-1350,RT3052-AP-DAP1350-3)
Image/Build/Profile/DAP1350WW=$(call BuildFirmware/dap1350/$(1),$(1),dap-1350WW,DAP-1350,RT3052-AP-DAP1350WW-3)
Image/Build/Profile/DCS930=$(call BuildFirmware/DCS930/$(1),$(1),dcs-930,DCS-930)
Image/Build/Profile/DCS930LB1=$(call BuildFirmware/DCS930/$(1),$(1),dcs-930l-b1,DCS-930L-B1)
Image/Build/Profile/ESR-9753=$(call BuildFirmware/Default4M/$(1),$(1),esr-9753,ESR-9753)
Image/Build/Profile/HW550-3G=$(call BuildFirmware/Default8M/$(1),$(1),hw550-3g,HW550-3G)
belkin_f5d8235v2_mtd_size=7929856
Image/Build/Profile/F5D8235V2=$(call BuildFirmware/CustomFlash/$(1),$(1),f5d8235v2,F5D8235_V2,$(belkin_f5d8235v2_mtd_size))
# 0x770000
belkin_f7c027_mtd_size=7798784
Image/Build/Profile/F7C027=$(call BuildFirmware/CustomFlash/$(1),$(1),f7c027,F7C027,$(belkin_f7c027_mtd_size))
Image/Build/Profile/FONERA20N=$(call BuildFirmware/Edimax/$(1),$(1),fonera20n,FONERA20N,$(ralink_default_fw_size_8M),RSDK,NL1T,0x50000,0xc0000)
Image/Build/Profile/RT-N13U=$(call BuildFirmware/Default8M/$(1),$(1),rt-n13u,RT-N13U)
#Image/Build/Profile/HG255D=$(call BuildFirmware/Default16M/$(1),$(1),hg255d,HG255D)
Image/Build/Profile/FREESTATION5=$(call BuildFirmware/Default8M/$(1),$(1),freestation5,FREESTATION5)
Image/Build/Profile/IP2202=$(call BuildFirmware/Default8M/$(1),$(1),ip2202,IP2202)
Image/Build/Profile/HLKRM04=$(call BuildFirmware/HLKRM04/$(1),$(1),hlk-rm04,HLKRM04,HLK-RM04)
Image/Build/Profile/HT-TM02=$(call BuildFirmware/Default8M/$(1),$(1),ht-tm02,HT-TM02)
Image/Build/Profile/M3=$(call BuildFirmware/Poray4M/$(1),$(1),m3,M3)
Image/Build/Profile/M4=$(call BuildFirmware/PorayDualSize/$(1),$(1),m4,M4)
Image/Build/Profile/MOFI3500-3GN=$(call BuildFirmware/Default8M/$(1),$(1),mofi3500-3gn,MOFI3500-3GN)
# Kernel name should be "Linux Kernel Image" to make the OpenWrt image installable from factory Web UI
Image/Build/Profile/3G150B=$(call BuildFirmware/Default4M/$(1),$(1),3g150b,3G150B,Linux Kernel Image)
Image/Build/Profile/MR102N=$(call BuildFirmware/Default8M/$(1),$(1),mr-102n,MR-102N)
Image/Build/Profile/MPRA1=$(call BuildFirmware/Default4M/$(1),$(1),mpr-a1,MPRA1,Linux Kernel Image)
Image/Build/Profile/MPRA2=$(call BuildFirmware/Default8M/$(1),$(1),mpr-a2,MPRA2,Linux Kernel Image)
Image/Build/Profile/DIR-300-B7=$(call BuildFirmware/Default4M/$(1),$(1),dir-300-b7,DIR-300-B7)
Image/Build/Profile/DIR-320-B1=$(call BuildFirmware/Default8M/$(1),$(1),dir-320-b1,DIR-320-B1)
Image/Build/Profile/NBG-419N=$(call BuildFirmware/Default4M/$(1),$(1),nbg-419n,NBG-419N)
Image/Build/Profile/MZKW300NH2=$(call BuildFirmware/Edimax/$(1),$(1),mzk-w300nh2,MZK-W300NH2,$(mzkw300nh2_mtd_size),CSYS,RN52,0x50000,0xc0000)
Image/Build/Profile/MZKWDPR=$(call BuildFirmware/Default8M/$(1),$(1),mzk-wdpr,MZK-WDPR)
Image/Build/Profile/NCS601W=$(call BuildFirmware/Default8M/$(1),$(1),ncs601W,NCS601W)
nw718_mtd_size=3801088
Image/Build/Profile/NW718=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),nw718m,NW718,$(nw718_mtd_size),ARA1B4NCRNW718;1,factory)
Image/Build/Profile/M2M=$(call BuildFirmware/Default8M/$(1),$(1),m2m,M2M,Linux Kernel Image)
Image/Build/Profile/MINIEMBPLUG=$(call BuildFirmware/Default8M/$(1),$(1),miniembplug,MINIEMBPLUG)
Image/Build/Profile/MINIEMBWIFI=$(call BuildFirmware/Default8M/$(1),$(1),miniembwifi,MINIEMBWIFI)
Image/Build/Profile/PSR-680W=$(call BuildFirmware/Default4M/$(1),$(1),psr-680w,PSR-680W)
Image/Build/Profile/PWH2004=$(call BuildFirmware/Default8M/$(1),$(1),pwh2004,PWH2004)
Image/Build/Profile/PX-4885=$(call BuildFirmware/DefaultDualSize/$(1),$(1),px-4885,PX-4885)
Image/Build/Profile/RT5350F-OLINUXINO=$(call BuildFirmware/Default8M/$(1),$(1),rt5350f-olinuxino,RT5350F-OLINUXINO)
Image/Build/Profile/RT5350F-OLINUXINO-EVB=$(call BuildFirmware/Default8M/$(1),$(1),rt5350f-olinuxino-evb,RT5350F-OLINUXINO-EVB)
Image/Build/Profile/RTG32B1=$(call BuildFirmware/Default4M/$(1),$(1),rt-g32-b1,RT-G32-B1)
Image/Build/Profile/RTN10PLUS=$(call BuildFirmware/Default4M/$(1),$(1),rt-n10-plus,RT-N10-PLUS)
Image/Build/Profile/RUT5XX=$(call BuildFirmware/Default8M/$(1),$(1),rut5xx,RUT5XX)
Image/Build/Profile/SL-R7205=$(call BuildFirmware/Default4M/$(1),$(1),sl-r7205,SL-R7205)
Image/Build/Profile/V22RW-2X2=$(call BuildFirmware/Default4M/$(1),$(1),v22rw-2x2,V22RW-2X2)
Image/Build/Profile/VOCORE=$(call BuildFirmware/VOCORE/$(1),$(1),vocore,VOCORE)
Image/Build/Profile/W150M=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w150m,W150M,$(ralink_default_fw_size_4M),W150M Kernel Image,factory)
Image/Build/Profile/W306R_V20=$(call BuildFirmware/CustomFlashFactory/$(1),$(1),w306r-v20,W306R_V20,$(ralink_default_fw_size_4M),linkn Kernel Image,factory)
Image/Build/Profile/W502U=$(call BuildFirmware/Default8M/$(1),$(1),w502u,W502U)
Image/Build/Profile/WCR150GN=$(call BuildFirmware/Default4M/$(1),$(1),wcr150gn,WCR150GN)
Image/Build/Profile/MZK-DP150N=$(call BuildFirmware/Default4M/$(1),$(1),mzk-dp150n,MZK-DP150N)
Image/Build/Profile/WHRG300N=$(call BuildFirmware/WHRG300N/$(1),$(1))
Image/Build/Profile/WIZARD8800=$(call BuildFirmware/Default8M/$(1),$(1),wizard-8800,WIZARD8800,Linux Kernel Image)
Image/Build/Profile/WIZFI630A=$(call BuildFirmware/Default16M/$(1),$(1),wizfi630a,WIZFI630A)
Image/Build/Profile/WL-330N=$(call BuildFirmware/Default4M/$(1),$(1),wl-330n,WL-330N)
Image/Build/Profile/WL-330N3G=$(call BuildFirmware/Default4M/$(1),$(1),wl-330n3g,WL-330N3G)
Image/Build/Profile/WL-341V3=$(call BuildFirmware/WL-341V3/$(1),$(1))
Image/Build/Profile/WL-351=$(call BuildFirmware/Default4M/$(1),$(1),wl-351,WL-351)
Image/Build/Profile/WNCE2001=$(call BuildFirmware/WNCE2001/$(1),$(1),wnce2001,WNCE2001)
Image/Build/Profile/WR512-3GN=$(call BuildFirmware/DefaultDualSize/$(1),$(1),wr512-3ng,WR512-3GN)
Image/Build/Profile/WT1520=$(call BuildFirmware/PorayDualSize/$(1),$(1),wt1520,WT1520)
Image/Build/Profile/UR-326N4G=$(call BuildFirmware/Default4M/$(1),$(1),ur-326n4g,UR-326N4G)
Image/Build/Profile/UR-336UN=$(call BuildFirmware/Default8M/$(1),$(1),ur-336un,UR-336UN)
Image/Build/Profile/WR6202=$(call BuildFirmware/Default8M/$(1),$(1),wr6202,WR6202)
Image/Build/Profile/X5=$(call BuildFirmware/Poray8M/$(1),$(1),x5,X5)
Image/Build/Profile/X8=$(call BuildFirmware/Poray8M/$(1),$(1),x8,X8)
Image/Build/Profile/XDXRN502J=$(call BuildFirmware/Default4M/$(1),$(1),xdxrn502j,XDXRN502J)


define Image/Build/Profile/Default
	$(call Image/Build/Profile/3G6200N,$(1))
	$(call Image/Build/Profile/3G6200NL,$(1))
	$(call Image/Build/Profile/3G150B,$(1))
	$(call Image/Build/Profile/3G300M,$(1))
	$(call Image/Build/Profile/A5-V11,$(1))
	$(call Image/Build/Profile/AIR3GII,$(1))
	$(call Image/Build/Profile/ALL02393G,$(1))
	$(call Image/Build/Profile/ALL0256N,$(1))
	$(call Image/Build/Profile/ALL5002,$(1))
	$(call Image/Build/Profile/ALL5003,$(1))
	$(call Image/Build/Profile/ASL26555,$(1))
	$(call Image/Build/Profile/ATP-52B,$(1))
	$(call Image/Build/Profile/AWM002EVB,$(1))
	$(call Image/Build/Profile/AWAPN2403,$(1))
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
	$(call Image/Build/Profile/DCS930LB1,$(1))
	$(call Image/Build/Profile/ESR-9753,$(1))
	$(call Image/Build/Profile/F7C027,$(1))
	$(call Image/Build/Profile/F5D8235V2,$(1))
	$(call Image/Build/Profile/FONERA20N,$(1))
	$(call Image/Build/Profile/FREESTATION5,$(1))
#	$(call Image/Build/Profile/HG255D,$(1))
	$(call Image/Build/Profile/HLKRM04,$(1))
	$(call Image/Build/Profile/HT-TM02,$(1))
	$(call Image/Build/Profile/HW550-3G,$(1))
	$(call Image/Build/Profile/IP2202,$(1))
	$(call Image/Build/Profile/JHR-N805R,$(1))
	$(call Image/Build/Profile/JHR-N825R,$(1))
	$(call Image/Build/Profile/JHR-N926R,$(1))
	$(call Image/Build/Profile/M2M,$(1))
	$(call Image/Build/Profile/M3,$(1))
	$(call Image/Build/Profile/M4,$(1))
	$(call Image/Build/Profile/MOFI3500-3GN,$(1))
	$(call Image/Build/Profile/MR102N,$(1))
	$(call Image/Build/Profile/MPRA1,$(1))
	$(call Image/Build/Profile/MPRA2,$(1))
	$(call Image/Build/Profile/MZKW300NH2,$(1))
	$(call Image/Build/Profile/MZKWDPR,$(1))
	$(call Image/Build/Profile/NBG-419N,$(1))
	$(call Image/Build/Profile/NCS601W,$(1))
	$(call Image/Build/Profile/NW718,$(1))
	$(call Image/Build/Profile/MINIEMBWIFI,$(1))
	$(call Image/Build/Profile/MINIEMBPLUG,$(1))
	$(call Image/Build/Profile/PSR-680W,$(1))
	$(call Image/Build/Profile/PWH2004,$(1))
	$(call Image/Build/Profile/PX-4885,$(1))
	$(call Image/Build/Profile/RT5350F-OLINUXINO,$(1))
	$(call Image/Build/Profile/RT5350F-OLINUXINO-EVB,$(1))
	$(call Image/Build/Profile/RTG32B1,$(1))
	$(call Image/Build/Profile/RTN10PLUS,$(1))
	$(call Image/Build/Profile/RT-N13U,$(1))
	$(call Image/Build/Profile/RUT5XX,$(1))
	$(call Image/Build/Profile/SL-R7205,$(1))
	$(call Image/Build/Profile/UR-326N4G,$(1))
	$(call Image/Build/Profile/V22RW-2X2,$(1))
	$(call Image/Build/Profile/VOCORE,$(1))
	$(call Image/Build/Profile/W150M,$(1))
	$(call Image/Build/Profile/W306R_V20,$(1))
	$(call Image/Build/Profile/W502U,$(1))
	$(call Image/Build/Profile/WCR150GN,$(1))
	$(call Image/Build/Profile/WHRG300N,$(1))
	$(call Image/Build/Profile/WIZARD8800,$(1))
	$(call Image/Build/Profile/WIZFI630A,$(1))
	$(call Image/Build/Profile/WL-330N,$(1))
	$(call Image/Build/Profile/WL-330N3G,$(1))
	$(call Image/Build/Profile/WL-341V3,$(1))
	$(call Image/Build/Profile/WL-351,$(1))
	$(call Image/Build/Profile/WNCE2001,$(1))
	$(call Image/Build/Profile/WR512-3GN,$(1))
	$(call Image/Build/Profile/WR6202,$(1))
	$(call Image/Build/Profile/WT1520,$(1))
	$(call Image/Build/Profile/X5,$(1))
	$(call Image/Build/Profile/X8,$(1))
	$(call Image/Build/Profile/XDXRN502J,$(1))
	$(call Image/Build/Profile/MZK-DP150N,$(1))

endef
