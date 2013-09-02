#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE = 64k 128k 256k

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define rootfs_align
$(patsubst %-256k,0x40000,$(patsubst %-128k,0x20000,$(patsubst %-64k,0x10000,$(patsubst squashfs-%,0x4,$(1)))))
endef

define sysupname
$(call imgname,$(1),$(2))-sysupgrade.bin
endef

define factoryname
$(call imgname,$(1),$(2))-factory.bin
endef

COMMA:=,

define mkcmdline
$(if $(1),board=$(1) )$(if $(2),console=$(2)$(COMMA)$(3))
endef

SINGLE_PROFILES:=

define SingleProfile
  define Image/Build/Profile/$(3)
	$$(call Image/Build/Template/$(2)/$$(1),$(1),$(4),$$(call mkcmdline,$(5),$(6),$(7)),$(8),$(9),$(10),$(11),$(12))
  endef
  SINGLE_PROFILES += $(3)
endef

define MultiProfile
  define Image/Build/Profile/$(1)
	$(foreach p,$(2),
		$$(call Image/Build/Profile/$p,$$(1))
	)
  endef
endef

LOADER_MAKE := $(NO_TRACE_MAKE) -C lzma-loader KDIR=$(KDIR)

KDIR_TMP:=$(KDIR)/tmp
VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux
UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(3) $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux$(4) $(KDIR_TMP)/vmlinux$(4)-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR_TMP)/vmlinux$(4)-$(1) "$(strip $(2))"
	$(call CompressLzma,$(KDIR_TMP)/vmlinux$(4)-$(1),$(KDIR_TMP)/vmlinux$(4)-$(1).bin.lzma,$(3))
endef

define PatchKernelGzip
	cp $(KDIR)/vmlinux$(3) $(KDIR_TMP)/vmlinux$(3)-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR_TMP)/vmlinux$(3)-$(1) "$(strip $(2))"
	gzip -9 -c $(KDIR_TMP)/vmlinux$(3)-$(1) > $(KDIR_TMP)/vmlinux$(3)-$(1).bin.gz
endef

define MkuImage
	mkimage -A mips -O linux -T kernel -a 0x80060000 -C $(1) $(2) \
		-e 0x80060000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(3) $(4)
endef

define MkuImageLzma
	$(call PatchKernelLzma,$(1),$(2),$(3),$(4))
	$(call MkuImage,lzma,,$(KDIR_TMP)/vmlinux$(4)-$(1).bin.lzma,$(KDIR_TMP)/vmlinux$(4)-$(1).uImage)
endef

define MkuImageLzma/initramfs
	$(call PatchKernelLzma,$(1),$(2),,-initramfs)
	$(call MkuImage,lzma,,$(KDIR_TMP)/vmlinux-initramfs-$(1).bin.lzma,$(call imgname,initramfs,$(1))-uImage.bin)
endef

define MkuImageGzip
	$(call PatchKernelGzip,$(1),$(2))
	$(call MkuImage,gzip,,$(KDIR_TMP)/vmlinux-$(1).bin.gz,$(KDIR_TMP)/vmlinux-$(1).uImage)
endef

define MkuImageGzip/initramfs
	$(call PatchKernelGzip,$(1),$(2),-initramfs)
	$(call MkuImage,gzip,,$(KDIR_TMP)/vmlinux-initramfs-$(1).bin.gz,$(call imgname,initramfs,$(1))-uImage.bin)
endef

define MkuImageOKLI
	$(call MkuImage,lzma,-M 0x4f4b4c49,$(KDIR)/vmlinux.bin.lzma,$(KDIR_TMP)/vmlinux-$(1).okli)
endef

define CatFiles
	if [ $(2) -gt 262144 -a `stat -c%s "$(1)"` -gt $(2) ]; then \
		echo "Warning: $(1) is too big" >&2; \
	else if [ `stat -c%s $(3)` -gt $(4) ]; then \
		echo "Warning: $(3) is too big" >&2; \
	else \
		( dd if=$(1) bs=$(2) conv=sync; dd if=$(3) ) > $(5); \
	fi; fi
endef

define Sysupgrade/KR
	$(call CatFiles,$(2),$(3),$(KDIR)/root.$(1),$(4),$(call sysupname,$(1),$(5)))
endef

define Sysupgrade/KRuImage
	$(call CatFiles,$(KDIR_TMP)/vmlinux-$(2).uImage,$(3),$(KDIR)/root.$(1),$(4),$(call sysupname,$(1),$(2)))
endef

define Sysupgrade/RKuImage
	$(call CatFiles,$(KDIR)/root.$(1),$(4),$(KDIR_TMP)/vmlinux-$(2).uImage,$(3),$(call sysupname,$(1),$(2)))
endef

define Image/BuildLoader
	-rm -rf $(KDIR)/lzma-loader
	$(LOADER_MAKE) LOADER=loader-$(1).$(2) KERNEL_CMDLINE="$(3)"\
		LZMA_TEXT_START=0x80a00000 LOADADDR=0x80060000 \
		LOADER_DATA="$(KDIR)/vmlinux$(5).bin.lzma" BOARD="$(1)" \
		compile loader.$(2)
	-$(CP) $(KDIR)/loader-$(1).$(2) $(KDIR)/loader-$(1)$(5).$(2)
endef

define Image/BuildLoaderAlone
	-rm -rf $(KDIR)/lzma-loader
	$(LOADER_MAKE) LOADER=loader-$(1).$(2) KERNEL_CMDLINE="$(3)" \
		LZMA_TEXT_START=0x80a00000 LOADADDR=0x80060000 \
		BOARD="$(1)" FLASH_OFFS=$(4) FLASH_MAX=$(5) \
		compile loader.$(2)
endef

define Build/Clean
	$(LOADER_MAKE) clean
endef


alfa_ap96_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,256k(u-boot-env)ro,13312k(rootfs),2048k(kernel),512k(caldata)ro,15360k@0x80000(firmware)
alfa_mtdlayout_8M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6144k(rootfs),1600k(kernel),64k(nvram),64k(art)ro,7744k@0x50000(firmware)
alfa_mtdlayout_16M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,15936k(firmware),64k(nvram),64k(art)ro
all0258n_mtdlayout=mtdparts=spi0.0:256k(u-boot),64k(u-boot-env),6272k(firmware),1536k(failsafe),64k(art)
all0315n_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,256k(u-boot-env),13568k(firmware),2048k(failsafe),256k(art)ro
ap96_mtdlayout=mtdparts=spi0.0:192k(u-boot)ro,64k(u-boot-env)ro,6144k(rootfs),1728k(kernel),64k(art)ro,7872k@0x40000(firmware)
ap113_mtd_layout=mtdparts=spi0.0:64k(u-boot),3008k(rootfs),896k(uImage),64k(NVRAM),64k(ART),3904k@0x10000(firmware)
ap121_mtdlayout_2M=mtdparts=spi0.0:64k(u-boot)ro,1216k(rootfs),704k(kernel),64k(art)ro,1920k@0x10000(firmware)
ap121_mtdlayout_4M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,2752k(rootfs),896k(kernel),64k(nvram),64k(art)ro,3648k@0x50000(firmware)
carambola2_mtdlayout_16M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,15936k(firmware),64k(nvram),64k(art)ro
ap132_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1408k(kernel),6400k(rootfs),64k(art),7808k@0x50000(firmware)
ap135_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,14528k(rootfs),1472k(kernel),64k(art)ro,16000k@0x50000(firmware)
ap136_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6336k(rootfs),1408k(kernel),64k(mib0),64k(art)ro,7744k@0x50000(firmware)
cameo7240_mtdlayout=mtdparts=spi0.0:192k(u-boot)ro,64k(nvram)ro,3712k(firmware),64k(mac)ro,64k(art)ro
cameo913x_mtdlayout=mtdparts=spi0.0:128k(u-boot)ro,64k(config)ro,3840k(firmware),64k(art)ro
cameo933x_mtdlayout=mtdparts=spi0.0:64k(u-boot)ro,64k(art)ro,64k(mac)ro,64k(nvram)ro,192k(language)ro,3648k(firmware)
cameo933x_mtdlayout_8M=mtdparts=spi0.0:64k(u-boot)ro,64k(art)ro,64k(mac)ro,64k(nvram)ro,256k(language)ro,7680k@0x80000(firmware)
cameo934x_mtdlayout=mtdparts=spi0.0:64k(uboot)ro,64k(nvram)ro,15936k(firmware),192k(lang)ro,64k(mac)ro,64k(art)ro
cap4200ag_mtdlayout=mtdparts=spi0.0:256k(u-boot),64k(u-boot-env),320k(custom)ro,1536k(kernel),12096k(rootfs),2048k(failsafe),64k(art),13632k@0xa0000(firmware)
db120_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6336k(rootfs),1408k(kernel),64k(nvram),64k(art)ro,7744k@0x50000(firmware)
dir825b1_mtdlayout=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,6208k(firmware),64k(caldata)ro,1600k(unknown)ro,64k@0x7f0000(caldata_copy)
dir825b1_mtdlayout_fat=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,7808k(firmware),64k(caldata)ro,64k@0x660000(caldata_orig),6208k@0x50000(firmware_orig)
ew-dorin_mtdlayout_4M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env),3712k(firmware),64k(art)
dlrtdev_mtdlayout=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,6208k(firmware),64k(caldata)ro,640k(certs),960k(unknown)ro,64k@0x7f0000(caldata_copy)
dlrtdev_mtdlayout_fat=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,7168k(firmware),640k(certs),64k(caldata)ro,64k@0x660000(caldata_orig),6208k@0x50000(firmware_orig)
pb92_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,2752k(rootfs),896k(kernel),64k(nvram),64k(art)ro,3648k@0x50000(firmware)
planex_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1024k(kernel),6656k(rootfs),128k(art)ro,7744k@0x50000(firmware)
ubntxm_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7552k(firmware),256k(cfg)ro,64k(EEPROM)ro
uap_pro_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1536k(kernel),14208k(rootfs),256k(cfg)ro,64k(EEPROM)ro,15744k@0x50000(firmware)
ubdev_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,7488k(firmware),64k(certs),256k(cfg)ro,64k(EEPROM)ro
whrhpg300n_mtdlayout=mtdparts=spi0.0:248k(u-boot)ro,8k(u-boot-env)ro,3712k(firmware),64k(art)ro
wndap360_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1728k(kernel),6016k(rootfs),64k(nvram)ro,64k(art)ro,7744k@0x50000(firmware)
wnr2000v3_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1024k(kernel),2688k(rootfs),64k(art)ro,3712k@0x50000(firmware)
wndr3700_mtdlayout=mtdparts=spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,1152k(kernel),6528k(rootfs),64k(art)ro,7680k@0x70000(firmware)
wndr3700v2_mtdlayout=mtdparts=spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,1152k(kernel),14720k(rootfs),64k(art)ro,15872k@0x70000(firmware)
wndr4300_mtdlayout=mtdparts=ar934x-nfc:256k(u-boot)ro,256k(u-boot-env)ro,256k(caldata),512k(pot),2048k(language),512k(config),3072k(traffic_meter),1152k(kernel),24448k(rootfs),25600k@0x6c0000(firmware),256k(caldata_backup),-(reserved)
zcn1523h_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6208k(rootfs),1472k(kernel),64k(configure)ro,64k(mfg)ro,64k(art)ro,7680k@0x50000(firmware)

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(VMLINUX).elf
	cp $(KDIR)/vmlinux $(VMLINUX).bin
	dd if=$(KDIR)/vmlinux.bin.lzma of=$(VMLINUX).lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(VMLINUX).gz bs=65536 conv=sync
	$(call MkuImage,gzip,,$(KDIR)/vmlinux.bin.gz,$(UIMAGE)-gzip.bin)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux.bin.lzma,$(UIMAGE)-lzma.bin)
	cp $(KDIR)/loader-generic.elf $(VMLINUX)-lzma.elf
	-mkdir -p $(KDIR_TMP)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	cp $(KDIR)/vmlinux-initramfs.elf $(VMLINUX)-initramfs.elf
	cp $(KDIR)/vmlinux-initramfs $(VMLINUX)-initramfs.bin
	dd if=$(KDIR)/vmlinux-initramfs.bin.lzma of=$(VMLINUX)-initramfs.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux-initramfs.bin.gz of=$(VMLINUX)-initramfs.gz bs=65536 conv=sync
	$(call MkuImage,gzip,,$(KDIR)/vmlinux-initramfs.bin.gz,$(UIMAGE)-initramfs-gzip.bin)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux-initramfs.bin.lzma,$(UIMAGE)-initramfs-lzma.bin)
	cp $(KDIR)/loader-generic-initramfs.elf $(VMLINUX)-initramfs-lzma.elf
	$(call Image/Build/Initramfs)
endif
endef

define Image/Build/WRT400N
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),1310720,6488064)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		wrt400n $(KDIR_TMP)/vmlinux-$(2).uImage $(KDIR)/root.$(1) $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/DIR825B1
	$(call MkuImageLzma,$(2),$(3) $(dir825b1_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,5308416)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=$(call sysupname,$(1),$(2)); \
			echo -n "$(4)"; \
		) > $(call imgname,$(1),$(2))-backup-loader.bin; \
		if [ `stat -c%s $(call sysupname,$(1),$(2))` -gt 4194304 ]; then \
			echo "Warning: $(call sysupname,$(1),$(2)) is too big" >&2; \
		else \
			( \
				dd if=$(call sysupname,$(1),$(2)) bs=4096k conv=sync; \
				echo -n "$(5)"; \
			) > $(call factoryname,$(1),$(2)); \
		fi; \
	fi
	$(call MkuImageLzma,$(2)-fat,$(3) $(dir825b1_mtdlayout_fat))
	$(call CatFiles,$(KDIR_TMP)/vmlinux-$(2)-fat.uImage,65536,$(KDIR)/root.$(1),6946816,$(KDIR_TMP)/$(2)-fat.bin)
	if [ -e "$(KDIR_TMP)/$(2)-fat.bin" ]; then \
		echo -n "" > $(KDIR_TMP)/$(2)-fat.dummy; \
		sh $(TOPDIR)/scripts/combined-image.sh \
			"$(KDIR_TMP)/$(2)-fat.bin" \
			"$(KDIR_TMP)/$(2)-fat.dummy" \
			$(call sysupname,$(1),$(2)-fat); \
	fi
endef

define Image/Build/DLRTDEV
	$(call MkuImageLzma,$(2),$(3) $(dlrtdev_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,5308416)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=$(call sysupname,$(1),$(2)); \
			echo -n "$(4)"; \
		) > $(call imgname,$(1),$(2))-backup-loader.bin; \
		if [ `stat -c%s $(call sysupname,$(1),$(2))` -gt 4194304 ]; then \
			echo "Warning: $(call sysupname,$(1),$(2)) is too big" >&2; \
		else \
			( \
				dd if=$(call sysupname,$(1),$(2)) bs=4096k conv=sync; \
				echo -n "$(5)"; \
			) > $(call factoryname,$(1),$(2)); \
		fi; \
	fi
	$(call MkuImageLzma,$(2)-fat,$(3) $(dlrtdev_mtdlayout_fat))
	$(call CatFiles,$(KDIR_TMP)/vmlinux-$(2)-fat.uImage,65536,$(KDIR)/root.$(1),6946816,$(KDIR_TMP)/$(2)-fat.bin)
	if [ -e "$(KDIR_TMP)/$(2)-fat.bin" ]; then \
		echo -n "" > $(KDIR_TMP)/$(2)-fat.dummy; \
		sh $(TOPDIR)/scripts/combined-image.sh \
			"$(KDIR_TMP)/$(2)-fat.bin" \
			"$(KDIR_TMP)/$(2)-fat.dummy" \
			$(call sysupname,$(1),$(2)-fat); \
	fi
endef

define Image/Build/WZRHP64K
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,31850496)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			echo -n -e "# Airstation Public Fmt1\x00\x00\x00\x00\x00\x00\x00\x00"; \
			dd if=$(call sysupname,$(1),$(2)); \
		) > $(call imgname,$(1),$(2))-tftp.bin; \
		buffalo-enc -p $(4) -v 1.99 \
			-i $(call sysupname,$(1),$(2)) \
			-o $(KDIR_TMP)/$(2).enc; \
		buffalo-tag -b $(4) -p $(4) -a ath -v 1.99 -m 1.01 -l mlang8 \
			-w 3 -c 0x80041000 -d 0x801e8000 -f 1 -r M_ \
			-i $(KDIR_TMP)/$(2).enc \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/WZRHP64K/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/WZRHP128K
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),131072,31850496)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			echo -n -e "# Airstation Public Fmt1\x00\x00\x00\x00\x00\x00\x00\x00"; \
			dd if=$(call sysupname,$(1),$(2)); \
		) > $(call imgname,$(1),$(2))-tftp.bin; \
		buffalo-enc -p $(4) -v 1.99 \
			-i $(call sysupname,$(1),$(2)) \
			-o $(KDIR_TMP)/$(2).enc; \
		buffalo-tag -b $(4) -p $(4) -a ath -v 1.99 -m 1.01 -l mlang8 \
			-w 3 -c 0x80041000 -d 0x801e8000 -f 1 -r M_ \
			-i $(KDIR_TMP)/$(2).enc \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/WZRHP128K/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/WHRHPG300N
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,2818048)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			echo -n -e "# Airstation Public Fmt1\x00\x00\x00\x00\x00\x00\x00\x00"; \
			dd if=$(call sysupname,$(1),$(2)); \
		) > $(call imgname,$(1),$(2))-tftp.bin; \
		buffalo-enc -p $(5) -v 1.99 \
			-i $(call sysupname,$(1),$(2)) \
			-o $(KDIR_TMP)/$(2).enc; \
		buffalo-tag -b $(5) -p $(5) -a ath -v 1.99 -m 1.01 -l mlang8 \
			-w 3 -c 0x80041000 -d 0x801e8000 -f 1 -r M_ \
			-i $(KDIR_TMP)/$(2).enc \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/WHRHPG300N/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/Cameo
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(5),$(6))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=$(KDIR_TMP)/vmlinux-$(2).uImage bs=$(5) conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=$(6) conv=sync; \
			echo -n $(7); \
		) > $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/CameoHornet
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(5),$(6))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		for r in $(9); do \
			[ -n "$$$$r" ] && dashr="-$$$$r" || dashr=; \
			[ -z "$$$$r" ] && r="DEF"; \
			mkcameofw -M HORNET -R "$$$$r" -S $(7) -V $(8) -c \
				-K $(10) -I $(11) \
				-k "$(call sysupname,$(1),$(2))" \
				-o $(call imgname,$(1),$(2))-factory$$$$dashr.bin; \
		done; \
	fi
endef

define Image/Build/Cameo913x
	$(call Image/Build/Cameo,$(1),$(2),$(3),$(cameo913x_mtdlayout),65536,2883584,$(4))
endef

define Image/Build/Cameo913x/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo913x_mtdlayout))
endef

define Image/Build/Cameo7240
	$(call Image/Build/Cameo,$(1),$(2),$(3),$(cameo7240_mtdlayout),65536,2752512,$(4))
endef

define Image/Build/Cameo7240/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo7240_mtdlayout))
endef

define Image/Build/Cameo933x
	$(call Image/Build/CameoHornet,$(1),$(2),$(3),$(cameo933x_mtdlayout),65536,2686976,$(4),$(5),$(6),0xe0000,3735552)
endef

define Image/Build/Cameo933x/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo933x_mtdlayout))
endef

define Image/Build/Cameo933x_8M
	$(call Image/Build/CameoHornet,$(1),$(2),$(3),$(cameo933x_mtdlayout_8M),65536,6422528,$(4),$(5),$(6),0x100000,7864320)
endef

define Image/Build/Cameo933x_8M/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo933x_mtdlayout_8M))
endef

define Image/Build/Cameo934x
	$(call Image/Build/Cameo,$(1),$(2),$(3),$(cameo934x_mtdlayout),65536,15007718,$(4))
endef

define Image/Build/Cameo934x/initramfs
	$(call Image/Build/Cameo,$(1),$(2),$(3) $(cameo934x_mtdlayout))
endef

define Image/Build/Ath
	$(call Sysupgrade/$(7),$(1),$(2),$(5),$(6))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		dd if=$(KDIR_TMP)/vmlinux-$(2).uImage \
			of=$(call imgname,kernel,$(2)).bin bs=64k conv=sync; \
		dd if=$(KDIR)/root.$(1) \
			of=$(call imgname,$(1),$(2)-rootfs).bin bs=128k conv=sync; \
	fi
endef

define Image/Build/AthGzip
	$(call MkuImageGzip,$(2),$(3) $(4))
	$(call Image/Build/Ath,$(1),$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/AthGzip/initramfs
	$(call MkuImageGzip/initramfs,$(2),$(3) $(4))
endef

define Image/Build/AthLzma
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Image/Build/Ath,$(1),$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/AthLzma/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/PB4X
	$(call PatchKernelLzma,$(2),$(3))
	dd if=$(KDIR_TMP)/vmlinux-$(2).bin.lzma \
	   of=$(call imgname,kernel,$(2)).bin bs=64k conv=sync
	dd if=$(KDIR)/root.$(1) \
	   of=$(call imgname,$(1),$(2)-rootfs).bin bs=128k conv=sync
	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(call imgname,kernel,$(2)).bin" \
		"$(call imgname,$(1),$(2)-rootfs).bin" \
		$(call sysupname,$(1),$(2))
endef


define Image/Build/MyLoader
	$(call PatchKernelLzma,$(2),$(3))
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) -s $(4) \
		-p0x030000:0xe0000:al:0x80060000:kernel:$(KDIR_TMP)/vmlinux-$(2).bin.lzma \
		-p0x110000:0:::rootfs:$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2))-$(5)-factory.img
endef

define Image/Build/UAPPRO
	$(call MkuImageLzma,$(2),$(3) $(uap_pro_mtdlayout))
	-rm -rf $(KDIR_TMP)/$(2)
	mkdir -p $(KDIR_TMP)/$(2)/image
	cp $(KDIR_TMP)/vmlinux-$(2).uImage $(KDIR_TMP)/$(2)/image/kernel0
	$(STAGING_DIR_HOST)/bin/mkfs.jffs2 \
		--pad --big-endian --squash-uids -v -e 64KiB \
		-o $(KDIR_TMP)/$(2)-kernel.jffs2 \
		-d $(KDIR_TMP)/$(2)/image \
		2>&1 1>/dev/null | awk '/^.+$$$$/'
	-rm -rf $(KDIR_TMP)/$(2)
	$(STAGING_DIR_HOST)/bin/padjffs2 $(KDIR_TMP)/$(2)-kernel.jffs2 64
	$(call CatFiles,$(KDIR_TMP)/$(2)-kernel.jffs2,1572864,$(KDIR)/root.$(1),14548992,$(call sysupname,$(1),$(2)))
	-$(STAGING_DIR_HOST)/bin/mkfwimage2 -f 0x9f000000 \
		-v $(5).$(6).v6.0.0-OpenWrt-$(REVISION) \
		-p jffs2:0x50000:0xf60000:0:0:$(call sysupname,$(1),$(2)) \
		-o $(call factoryname,$(1),$(2))
	-rm -f $(KDIR_TMP)/$(2)-kernel.jffs2
endef

define Image/Build/UAPPRO/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(uap_pro_mtdlayout))
endef

define Image/Build/UBNTXM
	$(call MkuImageLzma,$(2),$(3) $(ubntxm_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,6684672)
	dd if=$(KDIR_TMP)/vmlinux-$(2).uImage of=$(KDIR_TMP)/vmlinux-$(2).uImage.bin bs=64k conv=sync
	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(4) -v $(5).$(6).v6.0.0-OpenWrt-$(REVISION) \
		-k $(KDIR_TMP)/vmlinux-$(2).uImage.bin \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(call factoryname,$(1),$(2))
endef

define Image/Build/UBDEV
	$(call MkuImageLzma,$(2),$(3) $(ubdev_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),65536,6684672)
	dd if=$(KDIR_TMP)/vmlinux-$(2).uImage of=$(KDIR_TMP)/vmlinux-$(2).uImage.bin bs=64k conv=sync
	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(4) -v $(5).$(6).v6.0.0-OpenWrt-$(REVISION) \
		-k $(KDIR_TMP)/vmlinux-$(2).uImage.bin \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(call factoryname,$(1),$(2))
endef

define Image/Build/UBNT
	$(call PatchKernelLzma,$(2),$(3))
	dd if=$(KDIR_TMP)/vmlinux-$(2).bin.lzma of=$(KDIR_TMP)/vmlinux-$(2).lzma bs=64k conv=sync
	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(4) -v $(5).$(6).OpenWrt.$(REVISION) \
		-k $(KDIR_TMP)/vmlinux-$(2).lzma \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(call factoryname,$(1),$(2))
	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(KDIR_TMP)/vmlinux-$(2).lzma" \
		"$(BIN_DIR)/$(IMG_PREFIX)-root.$(1)" \
  $(call sysupname,$(1),$(2))
endef

define Image/Build/Planex/loader
	$(call Image/BuildLoaderAlone,$(1),gz,$(2) $(planex_mtdlayout),0x52000,0)
endef

define Image/Build/Planex
	[ -e "$(KDIR)/loader-$(2).gz" ]
	$(call MkuImage,gzip,,$(KDIR)/loader-$(2).gz,$(KDIR_TMP)/vmlinux-$(2).uImage)
	$(call MkuImageOKLI,$(2))
	( \
		dd if=$(KDIR_TMP)/vmlinux-$(2).uImage bs=8k count=1 conv=sync; \
		dd if=$(KDIR_TMP)/vmlinux-$(2).okli; \
	) > $(KDIR_TMP)/kernel-$(2).bin
	$(call Sysupgrade/KR,$(1),$(KDIR_TMP)/kernel-$(2).bin,1048576,6750208,$(2))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		$(STAGING_DIR_HOST)/bin/mkplanexfw \
			-B $(2) \
			-v 2.00.00 \
			-i $(call sysupname,$(1),$(2)) \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/ALFA
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/RKuImage,$(1),$(2),$(5),$(6))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		rm -rf $(KDIR)/$(1); \
		mkdir -p $(KDIR)/$(1); \
		cd $(KDIR)/$(1); \
		cp $(KDIR_TMP)/vmlinux-$(2).uImage $(KDIR)/$(1)/$(7); \
		cp $(KDIR)/root.$(1) $(KDIR)/$(1)/$(8); \
		$(TAR) zcf $(call factoryname,$(1),$(2)) -C $(KDIR)/$(1) $(7) $(8); \
		( \
			echo WRM7222C | dd bs=32 count=1 conv=sync; \
			echo -ne '\xfe'; \
		) >> $(call factoryname,$(1),$(2)); \
	fi
endef

define Image/Build/ALFA/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/TPLINKOLD/loader
	$(call Image/BuildLoaderAlone,$(1),gz,$(2),0x22000,0)
endef

define Image/Build/TPLINKOLD
	[ -e "$(KDIR)/loader-$(2).gz" ]
	$(call MkuImageOKLI,$(2))
	( \
		dd if=$(KDIR)/loader-$(2).gz bs=7680 count=1 conv=sync; \
		dd if=$(KDIR_TMP)/vmlinux-$(2).okli conv=sync; \
	) > $(KDIR_TMP)/kernel-$(2).bin
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION)\
		-k $(KDIR_TMP)/kernel-$(2).bin \
		-r $(KDIR)/root.$(1) \
		-o $(call factoryname,$(1),$(2))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR_TMP)/kernel-$(2).bin \
		-r $(KDIR)/root.$(1) \
		-o $(call sysupname,$(1),$(2))
endef

define Image/Build/TPLINKOLD/initramfs
	$(call Image/BuildLoader,$(2),gz,$(3),0x80060000,-initramfs)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR)/loader-$(2)-initramfs.gz \
		-o $(call imgname,$(1),$(2))-initramfs-uImage.bin
endef

define Image/Build/TPLINK/loader
	$(call Image/BuildLoaderAlone,$(1),gz,$(2),0x22000,0)
endef

define Image/Build/TPLINK
	[ -e "$(KDIR)/loader-$(2).gz" ]
	$(call MkuImageOKLI,$(2))
	( \
		dd if=$(KDIR)/loader-$(2).gz bs=7680 count=1 conv=sync; \
		dd if=$(KDIR_TMP)/vmlinux-$(2).okli conv=sync; \
	) > $(KDIR_TMP)/kernel-$(2).bin
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION)\
		-k $(KDIR_TMP)/kernel-$(2).bin \
		-r $(KDIR)/root.$(1) \
		-a $(call rootfs_align,$(1)) -j \
		-o $(call factoryname,$(1),$(2))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR_TMP)/kernel-$(2).bin \
		-r $(KDIR)/root.$(1) \
		-a $(call rootfs_align,$(1)) -j \
		-o $(call sysupname,$(1),$(2))
endef

define Image/Build/TPLINK/initramfs
	$(call Image/BuildLoader,$(2),gz,$(3),0x80060000,-initramfs)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR)/loader-$(2)-initramfs.gz \
		-o $(call imgname,$(1),$(2))-uImage.bin
endef

define Image/Build/TPLINK-LZMA
	$(call PatchKernelLzma,$(2),$(3))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) \
		-k $(KDIR_TMP)/vmlinux-$(2).bin.lzma \
		-r $(KDIR)/root.$(1) \
		-a $(call rootfs_align,$(1)) -j \
		-o $(call factoryname,$(1),$(2))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) -s \
		-k $(KDIR_TMP)/vmlinux-$(2).bin.lzma \
		-r $(KDIR)/root.$(1) \
		-a $(call rootfs_align,$(1)) -j \
		-o $(call sysupname,$(1),$(2))
endef

define Image/Build/TPLINK-LZMA/initramfs
	$(call PatchKernelLzma,$(2),$(3),,-initramfs)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) -s \
		-k $(KDIR_TMP)/vmlinux-initramfs-$(2).bin.lzma \
		-o $(call imgname,$(1),$(2))-uImage.bin
endef

define Image/Build/CyberTAN
	$(call MkuImageGzip,$(2),$(3))
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/image.tmp -f $(KDIR_TMP)/vmlinux-$(2).uImage \
		-x 32 -a 0x10000 -x -32 -f $(KDIR)/root.$(1)
	-$(STAGING_DIR_HOST)/bin/addpattern -B $(2) -v v$(4) \
		-i $(KDIR)/image.tmp \
		-o $(call sysupname,$(1),$(2))
	-$(STAGING_DIR_HOST)/bin/addpattern -B $(2) -v v$(4) -g \
		-i $(KDIR)/image.tmp \
		-o $(call factoryname,$(1),$(2))
	rm $(KDIR)/image.tmp
endef

define Image/Build/Netgear/Build_uImage
	$(call MkuImageLzma,$(1),$(2) $(3),-d20)
	-rm -rf $(KDIR)/$(1)
	mkdir -p $(KDIR)/$(1)/image
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR_TMP)/vmlinux-$(1).uImage \
		$(KDIR)/$(1)/image/uImage \
		$(4)
	$(STAGING_DIR_HOST)/bin/mksquashfs-lzma \
		$(KDIR)/$(1) $(KDIR_TMP)/vmlinux-$(1).uImage.squashfs.tmp \
		-nopad -noappend -root-owned -be
	-rm -rf $(KDIR)/$(1)
	mkimage -A mips -O linux -T filesystem -C none \
		-a 0xbf070000 -e 0xbf070000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR_TMP)/vmlinux-$(1).uImage.squashfs.tmp \
		$(KDIR_TMP)/vmlinux-$(1).uImage.squashfs.tmp2
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR_TMP)/vmlinux-$(1).uImage.squashfs.tmp2 \
		$(KDIR_TMP)/vmlinux-$(1).uImage.squashfs \
		$(4)
	-rm -f $(KDIR_TMP)/vmlinux-$(1).uImage.squashfs.tmp*
endef

define Image/Build/Netgear/Estimate_uImage
	$(call Image/Build/Netgear/Build_uImage,$(1)_est,$(2),$(3),$(4))
	( \
		set -e; \
		kk=`echo '$(3)' | sed -e 's/.*[:,]\([0-9]*\)k(kernel).*/\1/'`; \
		rk=`echo '$(3)' | sed -e 's/.*[:,]\([0-9]*\)k(rootfs).*/\1/'`; \
		let 'tk = kk + rk'; \
		s=`stat -c'%s' '$(KDIR_TMP)/vmlinux-$(1)_est.uImage.squashfs'`; \
		c=`echo '$(3)' | wc -c`; \
		let 'kk = (((s + c) / (64 * 1024) + 1) * 64)'; \
		let 'rk = tk - kk'; \
		echo '$(3)' | sed -e "s/[0-9]*k(kernel)/$$$${kk}k(kernel)/" \
				  -e "s/[0-9]*k(rootfs)/$$$${rk}k(rootfs)/" > \
			'$(KDIR_TMP)/$(1)_mtdparts'; \
		let 'k = kk * 1024'; \
		echo "$$$${k}" > '$(KDIR_TMP)/$(1)_kernel_maxsize'; \
		let 'r = rk * 1024'; \
		echo "$$$${r}" > '$(KDIR_TMP)/$(1)_rootfs_maxsize'; \
	)
	-rm -f $(KDIR_TMP)/vmlinux-$(1)_est \
	       $(KDIR_TMP)/vmlinux-$(1)_est.bin.lzma \
	       $(KDIR_TMP)/vmlinux-$(1)_est.uImage \
	       $(KDIR_TMP)/vmlinux-$(1)_est.uImage.squashfs
endef

define Image/Build/Netgear
	$(call Image/Build/Netgear/Estimate_uImage,$(2),$(3),$(4),$(5))
	$(call Image/Build/Netgear/Build_uImage,$(2),$(3),`cat $(KDIR_TMP)/$(2)_mtdparts`,$(5))
	if [ `stat -c%s '$(KDIR_TMP)/vmlinux-$(2).uImage.squashfs'` -gt \
	     `cat '$(KDIR_TMP)/$(2)_kernel_maxsize'` ]; then \
                echo "$(KDIR_TMP)/vmlinux-$(2).uImage.squashfs is too big" >& 2; \
	else if [ `stat -c%s '$(KDIR)/root.$(1)'` -gt \
	     `cat '$(KDIR_TMP)/$(2)_rootfs_maxsize'` ]; then \
                echo "$(KDIR)/root.$(1) is too big" >& 2; \
	else \
		( \
			set -e; \
			dd if=$(KDIR_TMP)/vmlinux-$(2).uImage.squashfs \
			bs=`cat '$(KDIR_TMP)/$(2)_kernel_maxsize'` conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=64k; \
		) > $(call sysupname,$(1),$(2)); \
		for r in $(7) ; do \
			[ -n "$$$$r" ] && dashr="-$$$$r" || dashr= ; \
			$(STAGING_DIR_HOST)/bin/mkdniimg \
				-B $(6) -v OpenWrt.$(REVISION) -r "$$$$r" $(8) \
				-i $(call sysupname,$(1),$(2)) \
				-o $(call imgname,$(1),$(2))-factory$$$$dashr.img; \
		done; \
	fi; fi
endef

define Image/Build/Netgear/initramfs
	$(call MkuImageLzma,$(2),$(3) $(4),,-initramfs)
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR_TMP)/vmlinux-initramfs-$(2).uImage \
		$(call imgname,$(1),$(2))-initramfs-uImage.bin \
		$(5)
endef

define Image/Build/NetgearNAND/initramfs
	$(call Image/Build/Netgear/initramfs,$(1),$(2),$(3),$(4),$(5))
endef

ifdef CONFIG_PACKAGE_uboot-ar71xx-nbg460n_550n_550nh
  define Image/Build/ZyXEL
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),917504,2752512)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		if [ ! -f $(BIN_DIR)/$(IMG_PREFIX)-$(2)-u-boot.bin ]; then \
			echo "Warning: $(IMG_PREFIX)-$(2)-u-boot.bin not found" >&2; \
		else \
			$(STAGING_DIR_HOST)/bin/mkzynfw \
				-B $(4) \
				-b $(BIN_DIR)/$(IMG_PREFIX)-$(2)-u-boot.bin \
				-r $(call sysupname,$(1),$(2)):0x10000 \
				-o $(call factoryname,$(1),$(2)); \
	fi; fi
  endef
endif

define Image/Build/OpenMesh
	$(call MkuImageLzma,$(2))
	-sh $(TOPDIR)/scripts/om-fwupgradecfg-gen.sh \
		"$(4)" \
		"$(BUILD_DIR)/fwupgrade.cfg-$(4)" \
		"$(KDIR_TMP)/vmlinux-$(2).uImage" \
		"$(KDIR)/root.$(1)"
	-sh $(TOPDIR)/scripts/combined-ext-image.sh \
		"$(4)" "$(call factoryname,$(1),$(2))" \
		"$(BUILD_DIR)/fwupgrade.cfg-$(4)" "fwupgrade.cfg" \
		"$(KDIR_TMP)/vmlinux-$(2).uImage" "kernel" \
		"$(KDIR)/root.$(1)" "rootfs"
endef

define Image/Build/Zcomax
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/RKuImage,$(1),$(2),1507328,6356992)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		$(STAGING_DIR_HOST)/bin/mkzcfw \
			-B $(2) \
			-k $(KDIR_TMP)/vmlinux-$(2).uImage \
			-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
			-o $(call imgname,$(1),$(2))-factory.img; \
	fi
endef

define Image/Build/Zcomax/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(4))
endef

define Image/Build/Template/all/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/all/jffs2-64k
	$(call Image/Build/$(1),jffs2-64k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/all/jffs2-128k
	$(call Image/Build/$(1),jffs2-128k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/all/jffs2-256k
	$(call Image/Build/$(1),jffs2-256k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/squashfs-only/loader
	$(call Image/Build/$(1)/loader,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/squashfs-only/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/squashfs-only/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64k/loader
	$(call Image/Build/$(1)/loader,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64k/squashfs
	$(call Image/Build/$(1),squashfs-64k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64k/jffs2-64k
	$(call Image/Build/$(1),jffs2-64k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64kraw/loader
	$(call Image/Build/$(1)/loader,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64kraw/squashfs
	$(call Image/Build/$(1),squashfs-raw,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64kraw/jffs2-64k
	$(call Image/Build/$(1),jffs2-64k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/128k/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/128k/jffs2-128k
	$(call Image/Build/$(1),jffs2-128k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/256k/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/256k/jffs2-256k
	$(call Image/Build/$(1),jffs2-256k,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64k/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/64kraw/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/128k/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/256k/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

$(eval $(call SingleProfile,ALFA,64k,ALFANX,alfa-nx,ALFA-NX,ttyS0,115200,$$(alfa_mtdlayout_8M),1638400,6291456,vmlinux.gz.uImage,pb9x-2.6.31-jffs2))
$(eval $(call SingleProfile,ALFA,64k,HORNETUB,hornet-ub,HORNET-UB,ttyATH0,115200,$$(alfa_mtdlayout_8M),1638400,6291456,kernel_image,rootfs_image))

$(eval $(call SingleProfile,AthGzip,64k,AP81,ap81,AP81,ttyS0,115200,,1310720,6619136,KRuImage))
$(eval $(call SingleProfile,AthGzip,64k,AP83,ap83,AP83,ttyS0,115200,,1310720,6619136,KRuImage))
$(eval $(call SingleProfile,AthGzip,64k,AP96,ap96,AP96,ttyS0,115200,$$(ap96_mtdlayout),1769472,6291456,RKuImage))

$(eval $(call SingleProfile,AthLzma,64k,ALFAAP96,alfa-ap96,ALFA-AP96,ttyS0,115200,$$(alfa_ap96_mtdlayout),2097152,13631488,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,ALL0258N,all0258n,ALL0258N,ttyS0,115200,$$(all0258n_mtdlayout),65536,5373952,KRuImage))
$(eval $(call SingleProfile,AthLzma,256k,ALL0315N,all0315n,ALL0315N,ttyS0,115200,$$(all0315n_mtdlayout),262144,12845056,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP113,ap113,AP113,ttyS0,115200,$$(ap113_mtd_layout),917504,3080192,RK))
$(eval $(call SingleProfile,AthLzma,64k,AP121_2M,ap121-2M,AP121,ttyATH0,115200,$$(ap121_mtdlayout_2M),720896,1245184,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP121_4M,ap121-4M,AP121,ttyATH0,115200,$$(ap121_mtdlayout_4M),917504,2818048,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,CARAMBOLA2,carambola2,CARAMBOLA2,ttyATH0,115200,$$(carambola2_mtdlayout_16M),65536,14221312,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP121MINI,ap121-mini,AP121-MINI,ttyATH0,115200,$$(ap121_mtdlayout_4M),917504,2818048,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP132,ap132,AP132,ttyS0,115200,$$(ap132_mtdlayout),1441792,6553600,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP135,ap135-020,AP135-020,ttyS0,115200,$$(ap135_mtdlayout),1507328,14876672,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP136_010,ap136-010,AP136-010,ttyS0,115200,$$(ap136_mtdlayout),1441792,6488064,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,AP136_020,ap136-020,AP136-020,ttyS0,115200,$$(ap136_mtdlayout),1441792,6488064,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,CAP4200AG,cap4200ag,CAP4200AG,ttyS0,115200,$$(cap4200ag_mtdlayout),1572864,12386304,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,DB120,db120,DB120,ttyS0,115200,$$(db120_mtdlayout),1441792,6488064,RKuImage))
$(eval $(call SingleProfile,AthLzma,64k,EWDORINAP,ew-dorin,EW-DORIN,ttyATH0,115200,$$(ew-dorin_mtdlayout_4M),65536,2752512,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,EWDORINRT,ew-dorin-router,EW-DORIN-ROUTER,ttyATH0,115200,$$(ew-dorin_mtdlayout_4M),65536,2752512,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,HORNETUBx2,hornet-ub-x2,HORNET-UB,ttyATH0,115200,$$(alfa_mtdlayout_16M),65536,16318464,KRuImage))
$(eval $(call SingleProfile,AthLzma,64k,PB92,pb92,PB92,ttyS0,115200,$$(pb92_mtdlayout),917504,2818048,KRuImage))

$(eval $(call SingleProfile,Cameo7240,64k,DIR600A1,dir-600-a1,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-00"))
$(eval $(call SingleProfile,Cameo7240,64k,DIR601A1,dir-601-a1,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-02"))
$(eval $(call SingleProfile,Cameo7240,64k,DIR615E4,dir-615-e4,DIR-615-E4,ttyS0,115200,"AP99-AR7240-RT-091105-05"))
$(eval $(call SingleProfile,Cameo7240,64k,FR54RTR,fr-54rtr,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-01"))

$(eval $(call SingleProfile,Cameo913x,squashfs-only,A02RBW300N,a02-rb-w300n,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-03"))
$(eval $(call SingleProfile,Cameo913x,squashfs-only,DIR615C1,dir-615-c1,DIR-615-C1,ttyS0,115200,"AP81-AR9130-RT-070614-02"))
$(eval $(call SingleProfile,Cameo913x,squashfs-only,TEW632BRP,tew-632brp,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-00"))
$(eval $(call SingleProfile,Cameo913x,squashfs-only,TEW652BRP_FW,tew-652brp,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-080609-05"))
$(eval $(call SingleProfile,Cameo913x,squashfs-only,TEW652BRP_RECOVERY,tew-652brp-recovery,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-02"))

$(eval $(call SingleProfile,Cameo933x,squashfs-only,TEW712BR,tew-712br,TEW-712BR,ttyATH0,115200,"HORNET-RT-TEW712BR-3",1.99,""))
$(eval $(call SingleProfile,Cameo933x,squashfs-only,DIR601B1,dir-601-b1,TEW-712BR,ttyATH0,115200,"HORNET-RT-DIR601B1-3",2.99.99,"" "NA"))
$(eval $(call SingleProfile,Cameo933x_8M,squashfs-only,DIR505A1,dir-505-a1,DIR-505-A1,ttyATH0,115200,"HORNET-PACKET-DIR505A1-3",1.99.99,""))

$(eval $(call SingleProfile,Cameo934x,64k,DIR825C1,dir-825-c1,DIR-825-C1,ttyS0,115200,"00DB120AR9344-RT-101214-00"))
$(eval $(call SingleProfile,Cameo934x,64k,DIR835A1,dir-835-a1,DIR-835-A1,ttyS0,115200,"00DB120AR9344-RT-101214-00"))

$(eval $(call SingleProfile,CyberTAN,64k,WRT160NL,wrt160nl,WRT160NL,ttyS0,115200,1.00.01))

$(eval $(call SingleProfile,DIR825B1,64k,DIR825B1,dir-825-b1,DIR-825-B1,ttyS0,115200,01AP94-AR7161-RT-080619-00,00AP94-AR7161-RT-080619-00))
$(eval $(call SingleProfile,DIR825B1,64k,TEW673GRU,tew-673gru,TEW-673GRU,ttyS0,115200,01AP94-AR7161-RT-080619-01,00AP94-AR7161-RT-080619-01))

$(eval $(call SingleProfile,DLRTDEV,64k,DLRTDEV01,dlrtdev01,DIR-825-B1,ttyS0,115200,01AP94-AR7161-RT-080619-00,00AP94-AR7161-RT-080619-00))

$(eval $(call SingleProfile,MyLoader,64k,WP543_2M,wp543,,ttyS0,115200,0x200000,2M))
$(eval $(call SingleProfile,MyLoader,64k,WP543_4M,wp543,,ttyS0,115200,0x400000,4M))
$(eval $(call SingleProfile,MyLoader,64k,WP543_8M,wp543,,ttyS0,115200,0x800000,8M))
$(eval $(call SingleProfile,MyLoader,64k,WP543_16M,wp543,,ttyS0,115200,0x1000000,16M))
$(eval $(call SingleProfile,MyLoader,64k,WPE72_4M,wpe72,,ttyS0,115200,0x400000,4M))
$(eval $(call SingleProfile,MyLoader,64k,WPE72_8M,wpe72,,ttyS0,115200,0x800000,8M))
$(eval $(call SingleProfile,MyLoader,64k,WPE72_16M,wpe72,,ttyS0,115200,0x1000000,16M))

# This is used with AthGzip profile for now
$(eval $(call SingleProfile,AthGzip,64k,WNDAP360,wndap360,WNDAP360,ttyS0,9600,$$(wndap360_mtdlayout),1769472,6160384,KRuImage))
$(eval $(call SingleProfile,Netgear,64k,WNR2000V3,wnr2000v3,WNR2000V3,ttyS0,115200,$$(wnr2000v3_mtdlayout),2003,WNR2000V3,"" NA,))
$(eval $(call SingleProfile,Netgear,64k,WNR612V2,wnr612v2,WNR612V2,ttyS0,115200,$$(wnr2000v3_mtdlayout),2061,WNR612V2,"",))
$(eval $(call SingleProfile,Netgear,64k,WNDR3700V1,wndr3700,WNDR3700,ttyS0,115200,$$(wndr3700_mtdlayout),3700,WNDR3700,"" NA,))
$(eval $(call SingleProfile,Netgear,64k,WNDR3700V2,wndr3700v2,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDR3700v2,"",-H 29763654+16+64))
$(eval $(call SingleProfile,Netgear,64k,WNDR3800,wndr3800,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDR3800,"",-H 29763654+16+128))
$(eval $(call SingleProfile,Netgear,64k,WNDRMAC,wndrmac,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDRMAC,"",-H 29763654+16+64))
$(eval $(call SingleProfile,Netgear,64k,WNDRMACV2,wndrmacv2,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDRMACv2,"",-H 29763654+16+128))

$(eval $(call SingleProfile,NetgearNAND,64k,WNDR4300,wndr4300,WNDR4300,ttyS0,115200,$$(wndr4300_mtdlayout),3703,WNDR4300,"",-H 29763948+0+128+128+2x2+3x3))

$(eval $(call SingleProfile,OpenMesh,squashfs-only,OM2P,om2p,,,,OM2P))
$(eval $(call SingleProfile,OpenMesh,squashfs-only,MR600,mr600,,,,MR600))

$(eval $(call SingleProfile,PB4X,128k,ALL0305,all0305,ALL0305,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,128k,EAP7660D,eap7660d,EAP7660D,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,64k,JA76PF,ja76pf,JA76PF,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,64k,JA76PF2,ja76pf2,JA76PF2,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,64k,JWAP003,jwap003,JWAP003,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,64k,PB42,pb42,PB42,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,64k,PB44,pb44,PB44,ttyS0,115200))

$(eval $(call SingleProfile,Planex,64k,MZKW04NU,mzk-w04nu,MZK-W04NU,ttyS0,115200))
$(eval $(call SingleProfile,Planex,64k,MZKW300NH,mzk-w300nh,MZK-W300NH,ttyS0,115200))

$(eval $(call SingleProfile,TPLINKOLD,squashfs-only,TLWR841NV15,tl-wr841nd-v1.5,TL-WR841N-v1.5,ttyS0,115200,0x08410002,2,4M))

$(eval $(call SingleProfile,TPLINK,64kraw,RNXN360RT,rnx-n360rt,TL-WR941ND,ttyS0,115200,0x09410002,0x00420001,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLMR3220V1,tl-mr3220-v1,TL-MR3220,ttyS0,115200,0x32200001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLMR3420V1,tl-mr3420-v1,TL-MR3420,ttyS0,115200,0x34200001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA701,tl-wa701n-v1,TL-WA901ND,ttyS0,115200,0x07010001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA7510NV1,tl-wa7510n,TL-WA7510N,ttyS0,115200,0x75100001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA801NV1,tl-wa801nd-v1,TL-WA901ND,ttyS0,115200,0x08010001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA830RV1,tl-wa830re-v1,TL-WA901ND,ttyS0,115200,0x08300010,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA901NV1,tl-wa901nd-v1,TL-WA901ND,ttyS0,115200,0x09010001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWA901NV2,tl-wa901nd-v2,TL-WA901ND-v2,ttyS0,115200,0x09010002,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR740NV1,tl-wr740n-v1,TL-WR741ND,ttyS0,115200,0x07400001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR740NV3,tl-wr740n-v3,TL-WR741ND,ttyS0,115200,0x07400003,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR741NV1,tl-wr741nd-v1,TL-WR741ND,ttyS0,115200,0x07410001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR741NV2,tl-wr741nd-v2,TL-WR741ND,ttyS0,115200,0x07410001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR743NV1,tl-wr743nd-v1,TL-WR741ND,ttyS0,115200,0x07430001,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR841NV3,tl-wr841nd-v3,TL-WR941ND,ttyS0,115200,0x08410003,3,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR841NV5,tl-wr841nd-v5,TL-WR741ND,ttyS0,115200,0x08410005,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR841NV7,tl-wr841nd-v7,TL-WR841N-v7,ttyS0,115200,0x08410007,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR842,tl-wr842n-v1,TL-MR3420,ttyS0,115200,0x08420001,1,8M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR941NV2,tl-wr941nd-v2,TL-WR941ND,ttyS0,115200,0x09410002,2,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR941NV3,tl-wr941nd-v3,TL-WR941ND,ttyS0,115200,0x09410002,2,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR941NV4,tl-wr941nd-v4,TL-WR741ND,ttyS0,115200,0x09410004,1,4M))
$(eval $(call SingleProfile,TPLINK,64kraw,TLWR1043,tl-wr1043nd-v1,TL-WR1043ND,ttyS0,115200,0x10430001,1,8M))

$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,ARCHERC7,archer-c7-v1,ARCHER-C7,ttyS0,115200,0x75000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR10U,tl-mr10u-v1,TL-MR10U,ttyATH0,115200,0x00100101,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR11UV1,tl-mr11u-v1,TL-MR11U,ttyATH0,115200,0x00110101,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR11UV2,tl-mr11u-v2,TL-MR11U,ttyATH0,115200,0x00110102,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR3020,tl-mr3020-v1,TL-MR3020,ttyATH0,115200,0x30200001,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR3040,tl-mr3040-v1,TL-MR3040,ttyATH0,115200,0x30400001,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR3220V2,tl-mr3220-v2,TL-MR3220-v2,ttyATH0,115200,0x32200002,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLMR3420V2,tl-mr3420-v2,TL-MR3420-v2,ttyS0,115200,0x34200002,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR703,tl-wr703n-v1,TL-WR703N,ttyATH0,115200,0x07030101,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR710,tl-wr710n-v1,TL-WR710N,ttyATH0,115200,0x07100001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR720NV3,tl-wr720n-v3,TL-WR720N-v3,ttyATH0,115200,0x07200103,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR740NV4,tl-wr740n-v4,TL-WR741ND-v4,ttyATH0,115200,0x07400004,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR741NV4,tl-wr741nd-v4,TL-WR741ND-v4,ttyATH0,115200,0x07410004,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR743NV2,tl-wr743nd-v2,TL-WR741ND-v4,ttyATH0,115200,0x07430002,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR841NV8,tl-wr841n-v8,TL-WR841N-v8,ttyS0,115200,0x08410008,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR941NV6,tl-wr941nd-v6,TL-WDR3500,ttyS0,115200,0x09410006,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR1041,tl-wr1041n-v2,TL-WR1041N-v2,ttyS0,115200,0x10410002,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWR2543,tl-wr2543-v1,TL-WR2543N,ttyS0,115200,0x25430001,1,8Mlzma,-v 3.13.99))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWDR3500V1,tl-wdr3500-v1,TL-WDR3500,ttyS0,115200,0x35000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWDR3600V1,tl-wdr3600-v1,TL-WDR4300,ttyS0,115200,0x36000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWDR4300V1,tl-wdr4300-v1,TL-WDR4300,ttyS0,115200,0x43000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,TLWDR4310V1,tl-wdr4310-v1,TL-WDR4300,ttyS0,115200,0x43100001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,64kraw,MW4530RV1,mw4530r-v1,TL-WDR4300,ttyS0,115200,0x45300001,1,8Mlzma))

$(eval $(call SingleProfile,UAPPRO,64k,UAPPRO,ubnt-uap-pro,UAP-PRO,ttyS0,115200,BZ,BZ,ar934x))

$(eval $(call SingleProfile,UBDEV,64k,UBDEV01,ubdev01,UBNT-UF,ttyS0,115200,XM,XM,ar7240))

$(eval $(call SingleProfile,UBNT,64k,UBNTRS,ubnt-rs,UBNT-RS,ttyS0,115200,RS,RSx,ar7100))
$(eval $(call SingleProfile,UBNT,64k,UBNTRSPRO,ubnt-rspro,UBNT-RSPRO,ttyS0,115200,RSPRO,RSPRO,ar7100pro))
$(eval $(call SingleProfile,UBNT,64k,UBNTLSSR71,ubnt-ls-sr71,UBNT-LS-SR71,ttyS0,115200,LS-SR71,LS-SR71,ar7100))

$(eval $(call SingleProfile,UBNTXM,64k,RW2458N,rw2458n,RW2458N,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTAIRROUTER,ubnt-airrouter,UBNT-AR,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTBULLETM,ubnt-bullet-m,UBNT-BM,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTROCKETM,ubnt-rocket-m,UBNT-RM,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTNANOM,ubnt-nano-m,UBNT-NM,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTUNIFI,ubnt-unifi,UBNT-UF,ttyS0,115200,XM,XM,ar7240))
$(eval $(call SingleProfile,UBNTXM,64k,UBNTUNIFIOUTDOOR,ubnt-unifi-outdoor,UBNT-U20,ttyS0,115200,XM,XM,ar7240))

$(eval $(call SingleProfile,WHRHPG300N,64k,WHRG301N,whr-g301n,WHR-G301N,ttyS0,115200,$$(whrhpg300n_mtdlayout),WHR-G301N))
$(eval $(call SingleProfile,WHRHPG300N,64k,WHRHPG300N,whr-hp-g300n,WHR-HP-G300N,ttyS0,115200,$$(whrhpg300n_mtdlayout),WHR-HP-G300N))
$(eval $(call SingleProfile,WHRHPG300N,64k,WHRHPGN,whr-hp-gn,WHR-HP-GN,ttyS0,115200,$$(whrhpg300n_mtdlayout),WHR-HP-GN))
$(eval $(call SingleProfile,WHRHPG300N,64k,WLAEAG300N,wlae-ag300n,WLAE-AG300N,ttyS0,115200,$$(whrhpg300n_mtdlayout),WLAE-AG300N))

$(eval $(call SingleProfile,WRT400N,64k,WRT400N,wrt400n,WRT400N,ttyS0,115200))

$(eval $(call SingleProfile,WZRHP128K,128k,WZRHPG300NH,wzr-hp-g300nh,WZR-HP-G300NH,ttyS0,115200,WZR-HP-G300NH))
$(eval $(call SingleProfile,WZRHP64K,64k,WZRHPG300NH2,wzr-hp-g300nh2,WZR-HP-G300NH2,ttyS0,115200,WZR-HP-G300NH2))
$(eval $(call SingleProfile,WZRHP64K,64k,WZRHPAG300H,wzr-hp-ag300h,WZR-HP-AG300H,ttyS0,115200,WZR-HP-AG300H))
$(eval $(call SingleProfile,WZRHP64K,64k,WZRHPG450H,wzr-hp-g450h,WZR-HP-G450H,ttyS0,115200,WZR-HP-AG450H))
$(eval $(call SingleProfile,WZRHP64K,64k,WZR600DHP,wzr-600dhp,WZR-HP-AG300H,ttyS0,115200,WZR-600DHP))

$(eval $(call SingleProfile,Zcomax,64k,ZCN1523H28,zcn-1523h-2-8,ZCN-1523H-2,ttyS0,115200,$$(zcn1523h_mtdlayout)))
$(eval $(call SingleProfile,Zcomax,64k,ZCN1523H516,zcn-1523h-5-16,ZCN-1523H-5,ttyS0,115200,$$(zcn1523h_mtdlayout)))

$(eval $(call SingleProfile,ZyXEL,64k,NBG_460N_550N_550NH,nbg460n_550n_550nh,NBG460N,ttyS0,115200,NBG-460N))

$(eval $(call MultiProfile,AP121,AP121_2M AP121_4M))
$(eval $(call MultiProfile,AP136,AP136_010 AP136_020))
$(eval $(call MultiProfile,EWDORIN, EWDORINAP EWDORINRT))
$(eval $(call MultiProfile,OPENMESH,OM2P MR600))
$(eval $(call MultiProfile,TEW652BRP,TEW652BRP_FW TEW652BRP_RECOVERY))
$(eval $(call MultiProfile,TLMR11U,TLMR11UV1 TLMR11UV2))
$(eval $(call MultiProfile,TLMR3220,TLMR3220V1 TLMR3220V2))
$(eval $(call MultiProfile,TLMR3420,TLMR3420V1 TLMR3420V2))
$(eval $(call MultiProfile,TLWA801,TLWA801NV1))
$(eval $(call MultiProfile,TLWA830,TLWA830RV1))
$(eval $(call MultiProfile,TLWA901,TLWA901NV1 TLWA901NV2))
$(eval $(call MultiProfile,TLWA7510,TLWA7510NV1))
$(eval $(call MultiProfile,TLWR720,TLWR720NV3))
$(eval $(call MultiProfile,TLWR740,TLWR740NV1 TLWR740NV3 TLWR740NV4))
$(eval $(call MultiProfile,TLWR741,TLWR741NV1 TLWR741NV2 TLWR741NV4))
$(eval $(call MultiProfile,TLWR743,TLWR743NV1 TLWR743NV2))
$(eval $(call MultiProfile,TLWR841,TLWR841NV15 TLWR841NV3 TLWR841NV5 TLWR841NV7 TLWR841NV8))
$(eval $(call MultiProfile,TLWR941,TLWR941NV2 TLWR941NV3 TLWR941NV4 TLWR941NV6))
$(eval $(call MultiProfile,TLWDR4300,TLWDR3500V1 TLWDR3600V1 TLWDR4300V1 TLWDR4310V1 MW4530RV1))
$(eval $(call MultiProfile,UBNT,UBNTAIRROUTER UBNTRS UBNTRSPRO UBNTLSSR71 UBNTBULLETM UBNTROCKETM UBNTNANOM UBNTUNIFI UBNTUNIFIOUTDOOR UAPPRO))
$(eval $(call MultiProfile,WNDR3700,WNDR3700V1 WNDR3700V2 WNDR3800 WNDRMAC WNDRMACV2))
$(eval $(call MultiProfile,WP543,WP543_2M WP543_4M WP543_8M WP543_16M))
$(eval $(call MultiProfile,WPE72,WPE72_4M WPE72_8M WPE72_16M))

$(eval $(call MultiProfile,Default,$(SINGLE_PROFILES)))
$(eval $(call MultiProfile,Minimal,$(SINGLE_PROFILES)))
$(eval $(call MultiProfile,Madwifi,EAP7660D UBNTRS UBNTRSPRO UBNTLSSR71 WP543))

define Image/Build/squashfs
	cp $(KDIR)/root.squashfs $(KDIR)/root.squashfs-raw
	cp $(KDIR)/root.squashfs $(KDIR)/root.squashfs-64k
	$(STAGING_DIR_HOST)/bin/padjffs2 $(KDIR)/root.squashfs-64k 64
	cp $(KDIR)/root.squashfs-64k $(BIN_DIR)/$(IMG_PREFIX)-root.squashfs-64k
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),initramfs)
endef

define Image/Prepare
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	gzip -9 -c $(KDIR)/vmlinux-initramfs > $(KDIR)/vmlinux-initramfs.bin.gz
	$(call CompressLzma,$(KDIR)/vmlinux-initramfs,$(KDIR)/vmlinux-initramfs.bin.lzma)
	$(call Image/BuildLoader,generic,elf,,,-initramfs)
endif
	$(call Image/BuildLoader,generic,elf)
	$(call Image/Build/Profile/$(if $(CONFIG_IB),Default,$(PROFILE)),loader)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
