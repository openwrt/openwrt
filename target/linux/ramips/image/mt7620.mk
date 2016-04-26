#
# MT7620A Profiles
#

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

# $(1): (ignored)
# $(2): lowercase board name
# $(3): uppercase board name (must match DTS filename w/o extension)
# $(4): erase block size
# $(5): hardware id for mkdniimg
# $(6): maximum image size
define BuildFirmware/Netgear/squashfs
	$(call PatchKernelLzmaDtb,$(2),$(3))
	# Pad kernel to eraseblock boundary, minus 2 uImage headers (=128 bytes):
	# bs = (eraseblock * (1 + (128 + kernelsize)/eraseblock)) - 128
	dd if=$(KDIR)/vmlinux-$(2).bin.lzma \
		of=$(KDIR)/vmlinux-$(2).bin.lzma.tmp \
		bs=`expr \( $(4) \* \( 1 + \( 128 + \`wc -c < $(KDIR)/vmlinux-$(2).bin.lzma\` \) / $(4) \) \) - 128` \
		count=1 conv=sync

	$(call MkImage,lzma,$(KDIR)/vmlinux-$(2).bin.lzma.tmp,$(KDIR)/vmlinux-$(2).uImage)
	cat ex2700-fakeroot.uImage >> $(KDIR)/vmlinux-$(2).uImage
	$(call MkImageSysupgrade/squashfs,squashfs,$(2),$(6))

	$(STAGING_DIR_HOST)/bin/mkdniimg \
		-B $(3) -H $(5) -v OpenWrt \
		-i $(call imgname,squashfs,$(2))-sysupgrade.bin \
		-o $(call imgname,squashfs,$(2))-factory.bin
endef

define MkImageTpl/squashfs
	$(eval output_name=$(IMG_PREFIX)-$(2)-$(1)-$(if $(4),$(4),sysupgrade).bin)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw2 -V "ver. 2.0" -B "$(2)" -j \
		-o $(KDIR)/$(output_name) \
		-k $(KDIR)/vmlinux-$(1)$(4).bin.lzma \
		-r $(KDIR)/root.$(1) && \
		$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name)
endef
define MkImageTpl/initramfs
	$(eval output_name=$(IMG_PREFIX)-$(2)-$(1).bin)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw2 -V "ver. 2.0" -B "$(2)" -c \
		-o $(KDIR)/$(output_name) \
		-k $(KDIR)/vmlinux-$(1).bin.lzma && \
		$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name)
endef
define BuildFirmware/OF/tplink
	$(call PatchKernelLzmaDtb,$(1),$(2),$(4))
	$(call MkImageTpl/$(1),$(1),$(2),$(4),$(5))
endef
define BuildFirmware/OF/tplink/initramfs
	$(call PatchKernelLzmaDtb,$(2),$(3),-initramfs)
	$(call MkImageTpl/$(1),$(1),$(2),$(4),$(5))
endef
BuildFirmware/Tplink/squashfs=$(call BuildFirmware/OF/tplink,$(1),$(2),$(3),$(4))
BuildFirmware/Tplink/initramfs=$(call BuildFirmware/OF/tplink/initramfs,$(1),$(2),$(3),$(4))

define BuildFirmware/WRH-300CR/squashfs
	$(call BuildFirmware/Default16M/squashfs,$(1),$(2),$(3))
	cp $(call sysupname,$(1),$(2)) $(KDIR)/v_0.0.0.bin
	( \
		$(STAGING_DIR_HOST)/bin/md5sum $(KDIR)/v_0.0.0.bin | \
			sed 's/ .*//' && \
		echo 458 \
	) | $(STAGING_DIR_HOST)/bin/md5sum | \
		sed 's/ .*//' > $(KDIR)/v_0.0.0.md5
	$(STAGING_DIR_HOST)/bin/tar -cf $(call imgname,$(1),$(2))-factory.bin -C $(KDIR) v_0.0.0.bin v_0.0.0.md5
endef
BuildFirmware/WRH-300CR/initramfs=$(call BuildFirmware/Default16M/initramfs,$(1),$(2),$(3))


Image/Build/Profile/E1700=$(call BuildFirmware/UMedia/$(1),$(1),e1700,E1700,0x013326)
ex2700_mtd_size=3866624
Image/Build/Profile/EX2700=$(call BuildFirmware/Netgear/$(1),$(1),ex2700,EX2700,65536,29764623+4+0+32+2x2+0,$(ex2700_mtd_size))
Image/Build/Profile/MT7620a=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a,MT7620a)
Image/Build/Profile/MT7620a_MT7610e=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_mt7610e,MT7620a_MT7610e)
Image/Build/Profile/MT7620a_MT7530=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_mt7530,MT7620a_MT7530)
Image/Build/Profile/MT7620a_V22SG=$(call BuildFirmware/Default8M/$(1),$(1),mt7620a_v22sg,MT7620a_V22SG)
br100_mtd_size=8126464
Image/Build/Profile/AI-BR100=$(call BuildFirmware/CustomFlash/$(1),$(1),ai-br100,AI-BR100,$(br100_mtd_size),Ai-BR)
Image/Build/Profile/RP-N53=$(call BuildFirmware/Default8M/$(1),$(1),rp-n53,RP-N53)
whr_300hp2_mtd_size=7012352
Image/Build/Profile/WHR300HP2=$(call BuildFirmware/CustomFlash/$(1),$(1),whr-300hp2,WHR-300HP2,$(whr_300hp2_mtd_size))
Image/Build/Profile/WHR600D=$(call BuildFirmware/CustomFlash/$(1),$(1),whr-600d,WHR-600D,$(whr_300hp2_mtd_size))
whr_1166d_mtd_size=15400960
Image/Build/Profile/WHR1166D=$(call BuildFirmware/CustomFlash/$(1),$(1),whr-1166d,WHR-1166D,$(whr_1166d_mtd_size))
dlink810l_mtd_size=6881280
Image/Build/Profile/CF-WR800N=$(call BuildFirmware/Default8M/$(1),$(1),cf-wr800n,CF-WR800N)
Image/Build/Profile/CS-QR10=$(call BuildFirmware/Default8M/$(1),$(1),cs-qr10,CS-QR10)
Image/Build/Profile/DIR-810L=$(call BuildFirmware/CustomFlash/$(1),$(1),dir-810l,DIR-810L,$(dlink810l_mtd_size))
na930_mtd_size=20971520
Image/Build/Profile/NA930=$(call BuildFirmware/CustomFlash/$(1),$(1),na930,NA930,$(na930_mtd_size))
Image/Build/Profile/DB-WRT01=$(call BuildFirmware/Default8M/$(1),$(1),db-wrt01,DB-WRT01)
Image/Build/Profile/MZK-750DHP=$(call BuildFirmware/Default8M/$(1),$(1),mzk-750dhp,MZK-750DHP)
Image/Build/Profile/MZK-EX300NP=$(call BuildFirmware/Default8M/$(1),$(1),mzk-ex300np,MZK-EX300NP)
Image/Build/Profile/HC5661=$(call BuildFirmware/Default16M/$(1),$(1),hc5661,HC5661)
Image/Build/Profile/HC5761=$(call BuildFirmware/Default16M/$(1),$(1),hc5761,HC5761)
Image/Build/Profile/HC5861=$(call BuildFirmware/Default16M/$(1),$(1),hc5861,HC5861)
Image/Build/Profile/OY-0001=$(call BuildFirmware/Default16M/$(1),$(1),oy-0001,OY-0001)
Image/Build/Profile/PSG1208=$(call BuildFirmware/Default8M/$(1),$(1),psg1208,PSG1208)
Image/Build/Profile/Y1=$(call BuildFirmware/Default16M/$(1),$(1),y1,Y1)
Image/Build/Profile/Y1S=$(call BuildFirmware/Default16M/$(1),$(1),y1s,Y1S)
Image/Build/Profile/MLW221=$(call BuildFirmware/Default16M/$(1),$(1),mlw221,MLW221)
Image/Build/Profile/MLWG2=$(call BuildFirmware/Default16M/$(1),$(1),mlwg2,MLWG2)
Image/Build/Profile/WMR-300=$(call BuildFirmware/Default8M/$(1),$(1),wmr-300,WMR-300)
Image/Build/Profile/RT-N14U=$(call BuildFirmware/Default8M/$(1),$(1),rt-n14u,RT-N14U)
Image/Build/Profile/WRH-300CR=$(call BuildFirmware/WRH-300CR/$(1),$(1),wrh-300cr,WRH-300CR)
Image/Build/Profile/WRTNODE=$(call BuildFirmware/Default16M/$(1),$(1),wrtnode,WRTNODE)
Image/Build/Profile/WT3020=$(call BuildFirmware/PorayDualSize/$(1),$(1),wt3020,WT3020)
Image/Build/Profile/MIWIFI-MINI=$(call BuildFirmware/Default16M/$(1),$(1),miwifi-mini,MIWIFI-MINI)
Image/Build/Profile/GL-MT300A=$(call BuildFirmware/Default16M/$(1),$(1),gl-mt300a,GL-MT300A)
Image/Build/Profile/GL-MT300N=$(call BuildFirmware/Default16M/$(1),$(1),gl-mt300n,GL-MT300N)
Image/Build/Profile/GL-MT750=$(call BuildFirmware/Default16M/$(1),$(1),gl-mt750,GL-MT750)
Image/Build/Profile/ZTE-Q7=$(call BuildFirmware/Default8M/$(1),$(1),zte-q7,ZTE-Q7)
Image/Build/Profile/YOUKU-YK1=$(call BuildFirmware/Default16M/$(1),$(1),youku-yk1,YOUKU-YK1)
Image/Build/Profile/ZBT-WA05=$(call BuildFirmware/Default8M/$(1),$(1),zbt-wa05,ZBT-WA05)
Image/Build/Profile/ZBT-WE826=$(call BuildFirmware/Default16M/$(1),$(1),zbt-we826,ZBT-WE826)
Image/Build/Profile/ZBT-WR8305RT=$(call BuildFirmware/Default8M/$(1),$(1),zbt-wr8305rt,ZBT-WR8305RT)
Image/Build/Profile/ArcherC20i=$(call BuildFirmware/Tplink/$(1),$(1),ArcherC20i,ArcherC20i)
microwrt_mtd_size=16515072
Image/Build/Profile/MicroWRT=$(call BuildFirmware/CustomFlash/$(1),$(1),microwrt,MicroWRT,$(microwrt_mtd_size))
Image/Build/Profile/TINY-AC=$(call BuildFirmware/Default8M/$(1),$(1),tiny-ac,TINY-AC)


define Image/Build/Profile/Default
	$(call Image/Build/Profile/E1700,$(1))
	$(call Image/Build/Profile/EX2700,$(1))
	$(call Image/Build/Profile/MT7620a,$(1))
	$(call Image/Build/Profile/MT7620a_MT7610e,$(1))
	$(call Image/Build/Profile/MT7620a_MT7530,$(1))
	$(call Image/Build/Profile/MT7620a_V22SG,$(1))
	$(call Image/Build/Profile/AI-BR100,$(1))
	$(call Image/Build/Profile/CF-WR800N,$(1))
	$(call Image/Build/Profile/CS-QR10,$(1))
	$(call Image/Build/Profile/RP-N53,$(1))
	$(call Image/Build/Profile/DIR-810L,$(1))
	$(call Image/Build/Profile/WHR300HP2,$(1))
	$(call Image/Build/Profile/WHR600D,$(1))
	$(call Image/Build/Profile/WHR1166D,$(1))
	$(call Image/Build/Profile/DB-WRT01,$(1))
	$(call Image/Build/Profile/MZK-750DHP,$(1))
	$(call Image/Build/Profile/MZK-EX300NP,$(1))
	$(call Image/Build/Profile/NA930,$(1))
	$(call Image/Build/Profile/HC5661,$(1))
	$(call Image/Build/Profile/HC5761,$(1))
	$(call Image/Build/Profile/HC5861,$(1))
	$(call Image/Build/Profile/OY-0001,$(1))
	$(call Image/Build/Profile/PSG1208,$(1))
	$(call Image/Build/Profile/Y1,$(1))
	$(call Image/Build/Profile/Y1S,$(1))
	$(call Image/Build/Profile/MLW221,$(1))
	$(call Image/Build/Profile/MLWG2,$(1))
	$(call Image/Build/Profile/WMR-300,$(1))
	$(call Image/Build/Profile/RT-N14U,$(1))
	$(call Image/Build/Profile/WRH-300CR,$(1))
	$(call Image/Build/Profile/WRTNODE,$(1))
	$(call Image/Build/Profile/WT3020,$(1))
	$(call Image/Build/Profile/MIWIFI-MINI,$(1))
	$(call Image/Build/Profile/GL-MT300A,$(1))
	$(call Image/Build/Profile/GL-MT300N,$(1))
	$(call Image/Build/Profile/GL-MT750,$(1))
	$(call Image/Build/Profile/ZTE-Q7,$(1))
	$(call Image/Build/Profile/YOUKU-YK1,$(1))
	$(call Image/Build/Profile/ZBT-WA05,$(1))
	$(call Image/Build/Profile/ZBT-WE826,$(1))
	$(call Image/Build/Profile/ZBT-WR8305RT,$(1))
	$(call Image/Build/Profile/ArcherC20i,$(1))
	$(call Image/Build/Profile/MicroWRT,$(1))
	$(call Image/Build/Profile/TINY-AC,$(1))
endef
