#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define rootfs_align
$(patsubst %-128k,0x20000,$(patsubst %-64k,0x10000,$(patsubst squashfs-%,0x4,$(1))))
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

define SingleProfile
  define Image/Build/Profile/$(3)
	$$(call Image/Build/Template/$(2)/$$(1),$(1),$(4),$$(call mkcmdline,$(5),$(6),$(7)),$(8),$(9),$(10),$(11),$(12))
  endef
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
fs_squash:=squashfs-only
fs_64k:=64k
fs_64kraw:=64kraw
fs_128k:=128k
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	fs_squash:=initramfs
	fs_64k:=initramfs
	fs_64kraw:=initramfs
	fs_128k:=initramfs
	VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs
endif

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR_TMP)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR_TMP)/vmlinux-$(1) "$(strip $(2))"
	$(call CompressLzma,$(KDIR_TMP)/vmlinux-$(1),$(KDIR_TMP)/vmlinux-$(1).bin.lzma)
endef

define PatchKernelGzip
	cp $(KDIR)/vmlinux $(KDIR_TMP)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR_TMP)/vmlinux-$(1) "$(strip $(2))"
	gzip -9 -c $(KDIR_TMP)/vmlinux-$(1) > $(KDIR_TMP)/vmlinux-$(1).bin.gz
endef

define MkuImage
	mkimage -A mips -O linux -T kernel -a 0x80060000 -C $(1) $(2) \
		-e 0x80060000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(3) $(4)
endef

define MkuImageLzma
	$(call PatchKernelLzma,$(1),$(2))
	$(call MkuImage,lzma,,$(KDIR_TMP)/vmlinux-$(1).bin.lzma,$(KDIR_TMP)/vmlinux-$(1).uImage)
endef

define MkuImageLzma/initramfs
	$(call PatchKernelLzma,$(1),$(2))
	$(call MkuImage,lzma,,$(KDIR_TMP)/vmlinux-$(1).bin.lzma,$(call imgname,initramfs,$(1))-uImage.bin)
endef

define MkuImageGzip
	$(call PatchKernelGzip,$(1),$(2))
	$(call MkuImage,gzip,,$(KDIR_TMP)/vmlinux-$(1).bin.gz,$(KDIR_TMP)/vmlinux-$(1).uImage)
endef

define MkuImageGzip/initramfs
	$(call PatchKernelGzip,$(1),$(2))
	$(call MkuImage,gzip,,$(KDIR_TMP)/vmlinux-$(1).bin.gz,$(call imgname,initramfs,$(1))-uImage.bin)
endef

define MkuImageOKLI
	$(call MkuImage,lzma,-M 0x4f4b4c49,$(KDIR)/vmlinux.bin.lzma,$(KDIR_TMP)/vmlinux-$(1).okli)
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
		LOADER_DATA="$(KDIR)/vmlinux.bin.lzma" BOARD="$(1)" \
		compile loader.$(2)
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

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(VMLINUX).elf
	cp $(KDIR)/vmlinux $(VMLINUX).bin
	dd if=$(KDIR)/vmlinux.bin.lzma of=$(VMLINUX).lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(VMLINUX).gz bs=65536 conv=sync
	$(call MkuImage,gzip,,$(KDIR)/vmlinux.bin.gz,$(UIMAGE)-gzip.bin)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux.bin.lzma,$(UIMAGE)-lzma.bin)
	-mkdir -p $(KDIR_TMP)
	$(call Image/Build/Initramfs)
endef

define Image/Build/WRT400N
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),1310720,6488064)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		wrt400n $(KDIR_TMP)/vmlinux-$(2).uImage $(KDIR)/root.$(1) $(call factoryname,$(1),$(2)); \
	fi
endef

dir825b1_mtdlayout=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,1024k(kernel),5184k(rootfs),64k(caldata)ro,1600k(unknown)ro,6208k@0x50000(firmware)
define Image/Build/DIR825B1
	$(call MkuImageLzma,$(2),$(3) $(dir825b1_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),1048576,5308416)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		( \
			dd if=$(call sysupname,$(1),$(2)); \
			echo -n "$(4)"; \
		) > $(call imgname,$(1),$(2))-backup-loader.bin; \
		if [ `stat -c%s $(call sysupname,$(1),$(2))` -gt 4194304 ]; then \
			echo "Warning: $(call sysupname,$(1),$(2)) is too big"; \
		else \
			( \
				dd if=$(call sysupname,$(1),$(2)) bs=4096k conv=sync; \
				echo -n "$(5)"; \
			) > $(call factoryname,$(1),$(2)); \
		fi; \
	fi
endef

define Image/Build/WZRHPG30XNH
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),1048576,31850496)
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

define Image/Build/WHRHPG300N
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(call Sysupgrade/KRuImage,$(1),$(2),983040,2883584)
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
		mkcameofw -M HORNET -R "DEF" -S $(7) -V "1.99" \
			-K $(5) -I $$$$(($(5)+$(6))) \
			-k $(KDIR_TMP)/vmlinux-$(2).uImage \
			-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

cameo913x_mtdlayout=mtdparts=spi0.0:128k(u-boot)ro,64k(config)ro,960k(kernel),2880k(rootfs),64k(art)ro,3840k@0x30000(firmware)
define Image/Build/Cameo913x
	$(call Image/Build/Cameo,$(1),$(2),$(3),$(cameo913x_mtdlayout),983040,2949120,$(4))
endef

define Image/Build/Cameo913x/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo913x_mtdlayout))
endef

cameo7240_mtdlayout=mtdparts=spi0.0:192k(u-boot)ro,64k(nvram)ro,960k(kernel),2752k(rootfs),64k(mac)ro,64k(art)ro,3712k@0x40000(firmware)
define Image/Build/Cameo7240
	$(call Image/Build/Cameo,$(1),$(2),$(3),$(cameo7240_mtdlayout),983040,2818048,$(4))
endef

define Image/Build/Cameo7240/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo7240_mtdlayout))
endef

cameo933x_mtdlayout=mtdparts=spi0.0:64k(u-boot)ro,64k(art)ro,64k(mac)ro,64k(nvram)ro,192k(language)ro,896k(kernel),2752k(rootfs),3648k@0x70000(firmware)
define Image/Build/Cameo933x
	$(call Image/Build/CameoHornet,$(1),$(2),$(3),$(cameo933x_mtdlayout),917504,2818048,$(4))
endef

define Image/Build/Cameo933x/initramfs
	$(call MkuImageLzma/initramfs,$(2),$(3) $(cameo933x_mtdlayout))
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

ubntxm_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1024k(kernel),6528k(rootfs),256k(cfg)ro,64k(EEPROM)ro,7552k@0x50000(firmware)
define Image/Build/UBNTXM
	$(call MkuImageLzma,$(2),$(3) $(ubntxm_mtdlayout))
	$(call Sysupgrade/KRuImage,$(1),$(2),1048576,6684672)
	dd if=$(KDIR_TMP)/vmlinux-$(2).uImage of=$(KDIR_TMP)/vmlinux-$(2).uImage.bin bs=1024k conv=sync
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

planex_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,960k(kernel),6784k(rootfs),128k(art)ro,7744k@0x50000(firmware)
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
	$(call Sysupgrade/KR,$(1),$(KDIR_TMP)/kernel-$(2).bin,983040,6815744,$(2))
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		$(STAGING_DIR_HOST)/bin/mkplanexfw \
			-B $(2) \
			-v 2.00.00 \
			-i $(call sysupname,$(1),$(2)) \
			-o $(call factoryname,$(1),$(2)); \
	fi
endef

alfa_mtdlayout_8M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6144k(rootfs),1600k(kernel),64k(nvram),64k(art)ro,7744k@0x50000(firmware)
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
	$(call Image/BuildLoader,$(2),gz,$(3),0x80060000)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR)/loader-$(2).gz \
		-o $(call imgname,$(1),$(2))-uImage.bin
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
	$(call Image/BuildLoader,$(2),gz,$(3),0x80060000)
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) -s\
		-k $(KDIR)/loader-$(2).gz \
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
	$(call PatchKernelLzma,$(2),$(3))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-H $(4) -W $(5) -F $(6) -N OpenWrt -V $(REVISION) $(7) -s \
		-k $(KDIR_TMP)/vmlinux-$(2).bin.lzma \
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
	$(call MkuImageLzma,$(1),$(2) $(3))
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
	$(call MkuImageLzma,$(2),$(3) $(4))
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR_TMP)/vmlinux-$(2).uImage \
		$(call imgname,$(1),$(2))-uImage.bin \
		$(5)
endef

ifdef CONFIG_PACKAGE_uboot-ar71xx-nbg460n_550n_550nh
  define Image/Build/ZyXEL
	$(call MkuImageLzma,$(2),$(3))
	$(call Sysupgrade/KRuImage,$(1),$(2),917504,2752512)
	if [ -e "$(call sysupname,$(1),$(2))" ]; then \
		if [ ! -f $(BIN_DIR)/$(IMG_PREFIX)-$(2)-u-boot.bin ]; then \
			echo "Warning: $(IMG_PREFIX)-$(2)-u-boot.bin not found"; \
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
	-sh $(TOPDIR)/scripts/om2p-fwupgradecfg-gen.sh \
		"$(BUILD_DIR)/om2p-fwupgrade.cfg" \
		"$(KDIR_TMP)/vmlinux-$(2).uImage" \
		"$(KDIR)/root.$(1)"
	-sh $(TOPDIR)/scripts/combined-ext-image.sh \
		"$(3)" "$(call factoryname,$(1),$(2))" \
		"$(BUILD_DIR)/om2p-fwupgrade.cfg" "fwupgrade.cfg" \
		"$(KDIR_TMP)/vmlinux-$(2).uImage" "kernel" \
		"$(KDIR)/root.$(1)" "rootfs"
endef

define Image/Build/Zcomax
	$(call MkuImageLzma,$(2),$(3))
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
	$(call MkuImageLzma/initramfs,$(2),$(3))
endef

define Image/Build/Template/initramfs/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
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

define Image/Build/Template/squashfs-only/loader
	$(call Image/Build/$(1)/loader,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
endef

define Image/Build/Template/squashfs-only/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7),$(8))
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

$(eval $(call SingleProfile,ALFA,$(fs_64k),ALFANX,alfa-nx,ALFA-NX,ttyS0,115200,$$(alfa_mtdlayout_8M),1638400,6291456,vmlinux.gz.uImage,pb9x-2.6.31-jffs2))

alfa_ap96_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,256k(u-boot-env)ro,13312k(rootfs),2048k(kernel),512k(caldata)ro,15360k@0x80000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),ALFAAP96,alfa-ap96,ALFA-AP96,ttyS0,115200,$$(alfa_ap96_mtdlayout),2097152,13631488,RKuImage))

all0258n_mtdlayout=mtdparts=spi0.0:256k(u-boot),64k(u-boot-env),1024k(kernel),5248k(rootfs),1536k(failsafe),64k(art),6272k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),ALL0258N,all0258n,ALL0258N,ttyS0,115200,$$(all0258n_mtdlayout),1048576,5373952,KRuImage))

$(eval $(call SingleProfile,PB4X,$(fs_128k),ALL0305,all0305,ALL0305,ttyS0,115200))

ap113_mtd_layout=mtdparts=spi0.0:64k(u-boot),3008k(rootfs),896k(uImage),64k(NVRAM),64k(ART),3904k@0x10000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),AP113,ap113,AP113,ttyS0,115200,$$(ap113_mtd_layout),917504,3080192,RK))

ap121_mtdlayout_2M=mtdparts=spi0.0:64k(u-boot)ro,1216k(rootfs),704k(kernel),64k(art)ro,1920k@0x10000(firmware)
ap121_mtdlayout_4M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,2752k(rootfs),896k(kernel),64k(nvram),64k(art)ro,3648k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),AP121_2M,ap121-2M,AP121,ttyATH0,115200,$$(ap121_mtdlayout_2M),720896,1245184,RKuImage))
$(eval $(call SingleProfile,AthLzma,$(fs_64k),AP121_4M,ap121-4M,AP121,ttyATH0,115200,$$(ap121_mtdlayout_4M),917504,2818048,RKuImage))

$(eval $(call MultiProfile,AP121,AP121_2M AP121_4M))

$(eval $(call SingleProfile,AthLzma,$(fs_64k),AP121MINI,ap121-mini,AP121-MINI,ttyATH0,115200,$$(ap121_mtdlayout_4M),917504,2818048,RKuImage))

ew-dorin_mtdlayout_4M=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env),1024k(kernel),2688k(rootfs),64k(art),3712k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),EWDORINAP,ew-dorin,EW-DORIN,ttyATH0,115200,$$(ew-dorin_mtdlayout_4M),1048576,2752512,KRuImage))
$(eval $(call SingleProfile,AthLzma,$(fs_64k),EWDORINRT,ew-dorin-router,EW-DORIN-ROUTER,ttyATH0,115200,$$(ew-dorin_mtdlayout_4M),1048576,2752512,KRuImage))

$(eval $(call MultiProfile,EWDORIN, EWDORINAP EWDORINRT))

$(eval $(call SingleProfile,AthGzip,$(fs_64k),AP81,ap81,AP81,ttyS0,115200,,1310720,6619136,KRuImage))
$(eval $(call SingleProfile,AthGzip,$(fs_64k),AP83,ap83,AP83,ttyS0,115200,,1310720,6619136,KRuImage))

ap96_mtdlayout=mtdparts=spi0.0:192k(u-boot)ro,64k(u-boot-env)ro,6144k(rootfs),1728k(kernel),64k(art)ro,7872k@0x40000(firmware)
$(eval $(call SingleProfile,AthGzip,$(fs_64k),AP96,ap96,AP96,ttyS0,115200,$$(ap96_mtdlayout),1769472,6291456,RKuImage))

db120_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6336k(rootfs),1408k(kernel),64k(nvram),64k(art)ro,7744k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),DB120,db120,DB120,ttyS0,115200,$$(db120_mtdlayout),1441792,6488064,RKuImage))

ap136_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6336k(rootfs),1408k(kernel),64k(mib0),64k(art)ro,7744k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),AP136,ap136,AP136,ttyS0,115200,$$(ap136_mtdlayout),1441792,6488064,RKuImage))

$(eval $(call SingleProfile,PB4X,$(fs_64k),PB42,pb42,PB42,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,$(fs_64k),PB44,pb44,PB44,ttyS0,115200))

pb92_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,2752k(rootfs),896k(kernel),64k(nvram),64k(art)ro,3648k@0x50000(firmware)
$(eval $(call SingleProfile,AthLzma,$(fs_64k),PB92,pb92,PB92,ttyS0,115200,$$(pb92_mtdlayout),917504,2818048,KRuImage))

$(eval $(call SingleProfile,MyLoader,$(fs_64k),WP543_2M,wp543,,ttyS0,115200,0x200000,2M))
$(eval $(call SingleProfile,MyLoader,$(fs_64k),WP543_4M,wp543,,ttyS0,115200,0x400000,4M))
$(eval $(call SingleProfile,MyLoader,$(fs_64k),WP543_8M,wp543,,ttyS0,115200,0x800000,8M))
$(eval $(call SingleProfile,MyLoader,$(fs_64k),WP543_16M,wp543,,ttyS0,115200,0x1000000,16M))
$(eval $(call MultiProfile,WPE543,WPE543_2M WPE543_4M WPE543_8M WPE543_16M))

$(eval $(call SingleProfile,MyLoader,$(fs_64k),WPE72,wpe72,,ttyS0,115200,0x800000,8M))

$(eval $(call SingleProfile,Cameo7240,$(fs_64k),DIR600A1,dir-600-a1,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-00"))
$(eval $(call SingleProfile,Cameo7240,$(fs_64k),DIR601A1,dir-601-a1,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-02"))
$(eval $(call SingleProfile,Cameo7240,$(fs_64k),DIR615E4,dir-615-e4,DIR-615-E4,ttyS0,115200,"AP99-AR7240-RT-091105-05"))

$(eval $(call SingleProfile,PB4X,$(fs_128k),EAP7660D,eap7660d,EAP7660D,ttyS0,115200))

$(eval $(call SingleProfile,ALFA,$(fs_64k),HORNETUB,hornet-ub,HORNET-UB,ttyATH0,115200,$$(alfa_mtdlayout_8M),1638400,6291456,kernel_image,rootfs_image))

$(eval $(call SingleProfile,PB4X,$(fs_64k),JA76PF,ja76pf,JA76PF,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,$(fs_64k),JA76PF2,ja76pf2,JA76PF2,ttyS0,115200))
$(eval $(call SingleProfile,PB4X,$(fs_64k),JWAP003,jwap003,JWAP003,ttyS0,115200))

$(eval $(call SingleProfile,Cameo7240,$(fs_64k),FR54RTR,fr-54rtr,DIR-600-A1,ttyS0,115200,"AP91-AR7240-RT-090223-01"))

$(eval $(call SingleProfile,Cameo913x,$(fs_squash),DIR615C1,dir-615-c1,DIR-615-C1,ttyS0,115200,"AP81-AR9130-RT-070614-02"))

rw2458n_cmdline=board=RW2458N console=ttyS0,115200
define Image/Build/Profile/RW2458N
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,rw2458n,$(rw2458n_cmdline),XM,UBNTXM,ar7240)
endef

$(eval $(call SingleProfile,Cameo913x,$(fs_squash),TEW632BRP,tew-632brp,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-00"))

$(eval $(call SingleProfile,Cameo913x,$(fs_squash),TEW652BRP_FW,tew-652brp,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-080609-05"))
$(eval $(call SingleProfile,Cameo913x,$(fs_squash),TEW652BRP_RECOVERY,tew-652brp-recovery,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-02"))
$(eval $(call MultiProfile,TEW652BRP,TEW652BRP_FW TEW652BRP_RECOVERY))

$(eval $(call SingleProfile,Cameo933x,$(fs_squash),TEW712BR,tew-712br,TEW-712BR,ttyATH0,115200,"HORNET-RT-TEW712BR-3"))

$(eval $(call SingleProfile,Cameo913x,$(fs_squash),A02RBW300N,a02-rb-w300n,TEW-632BRP,ttyS0,115200,"AP81-AR9130-RT-070614-03"))

ubntairrouter_cmdline=board=UBNT-AR console=ttyS0,115200
define Image/Build/Profile/UBNTAIRROUTER
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-airrouter,$(ubntairrouter_cmdline),XM,UBNTXM,ar7240)
endef

ubntrs_cmdline=board=UBNT-RS console=ttyS0,115200
define Image/Build/Profile/UBNTRS
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-rs,$(ubntrs_cmdline),RS,RSx,ar7100)
endef

ubntrspro_cmdline=board=UBNT-RSPRO console=ttyS0,115200
define Image/Build/Profile/UBNTRSPRO
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-rspro,$(ubntrspro_cmdline),RSPRO,RSPRO,ar7100pro)
endef

ubntlssr71_cmdline=board=UBNT-LS-SR71 console=ttyS0,115200
define Image/Build/Profile/UBNTLSSR71
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-ls-sr71,$(ubntlssr71_cmdline),LS-SR71,LS-SR71,ar7100)
endef

ubntbulletm_cmdline=board=UBNT-BM console=ttyS0,115200
define Image/Build/Profile/UBNTBULLETM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-bullet-m,$(ubntbulletm_cmdline),XM,XM,ar7240)
endef

ubntrocketm_cmdline=board=UBNT-RM console=ttyS0,115200
define Image/Build/Profile/UBNTROCKETM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-rocket-m,$(ubntrocketm_cmdline),XM,XM,ar7240)
endef

ubntnanom_cmdline=board=UBNT-NM console=ttyS0,115200
define Image/Build/Profile/UBNTNANOM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-nano-m,$(ubntnanom_cmdline),XM,XM,ar7240)
endef

ubntunifi_cmdline=board=UBNT-UF console=ttyS0,115200
define Image/Build/Profile/UBNTUNIFI
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-unifi,$(ubntunifi_cmdline),XM,XM,ar7240)
endef

define Image/Build/Profile/UBNT
	$(call Image/Build/Profile/UBNTAIRROUTER,$(1))
	$(call Image/Build/Profile/UBNTRS,$(1))
	$(call Image/Build/Profile/UBNTRSPRO,$(1))
	$(call Image/Build/Profile/UBNTLSSR71,$(1))
	$(call Image/Build/Profile/UBNTBULLETM,$(1))
	$(call Image/Build/Profile/UBNTROCKETM,$(1))
	$(call Image/Build/Profile/UBNTNANOM,$(1))
	$(call Image/Build/Profile/UBNTUNIFI,$(1))
endef

$(eval $(call SingleProfile,Planex,$(fs_64k),MZKW04NU,mzk-w04nu,MZK-W04NU,ttyS0,115200))
$(eval $(call SingleProfile,Planex,$(fs_64k),MZKW300NH,mzk-w300nh,MZK-W300NH,ttyS0,115200))

nbg460n_cmdline=board=NBG460N console=ttyS0,115200
define Image/Build/Profile/NBG_460N_550N_550NH
	$(call Image/Build/Template/$(fs_64k)/$(1),ZyXEL,nbg460n_550n_550nh,$(nbg460n_cmdline),NBG-460N)
endef

$(eval $(call SingleProfile,OpenMesh,$(fs_squash),OM2P,om2p,OM2P))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLMR11U,tl-mr11u-v1,TL-MR11U,ttyATH0,115200,0x00110101,1,4Mlzma))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLMR3020,tl-mr3020-v1,TL-MR3020,ttyATH0,115200,0x30200001,1,4Mlzma))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLMR3040,tl-mr3040-v1,TL-MR11U,ttyATH0,115200,0x30400001,1,4Mlzma))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLMR3220,tl-mr3220-v1,TL-MR3220,ttyS0,115200,0x32200001,1,4M))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLMR3420,tl-mr3420-v1,TL-MR3420,ttyS0,115200,0x34200001,1,4M))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWA701NV1,tl-wa701n-v1,TL-WA901ND,ttyS0,115200,0x07010001,1,4M))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWA901NV1,tl-wa901nd-v1,TL-WA901ND,ttyS0,115200,0x09010001,1,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWA901NV2,tl-wa901nd-v2,TL-WA901ND-v2,ttyS0,115200,0x09010002,1,4M))
$(eval $(call MultiProfile,TLWA901,TLWA901NV1 TLWA901NV2))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWR703,tl-wr703n-v1,TL-WR703N,ttyATH0,115200,0x07030101,1,4Mlzma))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWR740NV4,tl-wr740n-v4,TL-WR741ND-v4,ttyATH0,115200,0x07400004,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR740NV1,tl-wr740n-v1,TL-WR741ND,ttyS0,115200,0x07400001,1,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR740NV3,tl-wr740n-v3,TL-WR741ND,ttyS0,115200,0x07400003,1,4M))
$(eval $(call MultiProfile,TLWR740,TLWR740NV1 TLWR740NV3 TLWR740NV4))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWR741NV4,tl-wr741nd-v4,TL-WR741ND-v4,ttyATH0,115200,0x07410004,1,4Mlzma))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR741NV1,tl-wr741nd-v1,TL-WR741ND,ttyS0,115200,0x07410001,1,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR741NV2,tl-wr741nd-v2,TL-WR741ND,ttyS0,115200,0x07410001,1,4M))
$(eval $(call MultiProfile,TLWR741,TLWR741NV1 TLWR741NV2 TLWR741NV4))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR743,tl-wr743nd-v1,TL-WR741ND,ttyS0,115200,0x07430001,1,4M))

tlwr841v15_cmdline=board=TL-WR841N-v1.5 console=ttyS0,115200
define Image/Build/Profile/TLWR841V15
	$(call Image/Build/Template/$(fs_squash)/$(1),TPLINKOLD,tl-wr841n-v1.5,$(tlwr841v15_cmdline),0x08410002,2,4M)
endef

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR841NV3,tl-wr841nd-v3,TL-WR941ND,ttyS0,115200,0x08410003,3,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR841NV5,tl-wr841nd-v5,TL-WR741ND,ttyS0,115200,0x08410005,1,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR841NV7,tl-wr841nd-v7,TL-WR841N-v7,ttyS0,115200,0x08410007,1,4M))
$(eval $(call MultiProfile,TLWR841,TLWR841NV15 TLWR841NV3 TLWR841NV5 TLWR841NV7))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR842,tl-wr842n-v1,TL-MR3420,ttyS0,115200,0x08420001,1,8M))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR941NV2,tl-wr941nd-v2,TL-WR941ND,ttyS0,115200,0x09410002,2,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR941NV3,tl-wr941nd-v3,TL-WR941ND,ttyS0,115200,0x09410002,2,4M))
$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR941NV4,tl-wr941nd-v4,TL-WR741ND,ttyS0,115200,0x09410004,1,4M))
$(eval $(call MultiProfile,TLWR941,TLWR941NV2 TLWR941NV3 TLWR941NV4))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWR1041,tl-wr1041n-v1,TL-WR1041,ttyS0,115200,0x10410002,1,4Mlzma))

$(eval $(call SingleProfile,TPLINK,$(fs_64kraw),TLWR1043,tl-wr1043nd-v1,TL-WR1043,ttyS0,115200,0x10430001,1,8M))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWR2543,tl-wr2543-v1,TL-WR2543,ttyS0,115200,0x25430001,1,8Mlzma,-v 3.13.99))

$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWDR3600V1,tl-wdr3600-v1,TL-WDR4300,ttyS0,115200,0x36000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWDR4300V1,tl-wdr4300-v1,TL-WDR4300,ttyS0,115200,0x43000001,1,8Mlzma))
$(eval $(call SingleProfile,TPLINK-LZMA,$(fs_64kraw),TLWDR4310V1,tl-wdr4310-v1,TL-WDR4300,ttyS0,115200,0x43100001,1,8Mlzma))
$(eval $(call MultiProfile,TLWDR4300,TLWDR3600V1 TLWDR4300V1 TLWDR4310V1))

wndr3700_mtdlayout=mtdparts=spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,1024k(kernel),6656k(rootfs),64k(art)ro,7680k@0x70000(firmware)
wndr3700v2_mtdlayout=mtdparts=spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,1024k(kernel),14848k(rootfs),64k(art)ro,15872k@0x70000(firmware)
$(eval $(call SingleProfile,Netgear,$(fs_64k),WNDR3700V1,wndr3700,WNDR3700,ttyS0,115200,$$(wndr3700_mtdlayout),3700,WNDR3700,"" NA,))
$(eval $(call SingleProfile,Netgear,$(fs_64k),WNDR3700V2,wndr3700v2,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDR3700v2,"",-H 29763654+16+64))
$(eval $(call SingleProfile,Netgear,$(fs_64k),WNDR3800,wndr3800,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDR3800,"",-H 29763654+16+128))
$(eval $(call SingleProfile,Netgear,$(fs_64k),WNDRMAC,wndrmac,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDRMAC,"",-H 29763654+16+64))
$(eval $(call SingleProfile,Netgear,$(fs_64k),WNDRMACV2,wndrmacv2,WNDR3700,ttyS0,115200,$$(wndr3700v2_mtdlayout),3701,WNDRMACv2,"",-H 29763654+16+128))
$(eval $(call MultiProfile,WNDR3700,WNDR3700V1 WNDR3700V2 WNDR3800 WNDRMAC WNDRMACV2))

wr400n_cmdline=board=WRT400N console=ttyS0,115200
define Image/Build/Profile/WRT400N
	$(call Image/Build/Template/$(fs_64k)/$(1),WRT400N,wrt400n,$(wr400n_cmdline))
endef

$(eval $(call SingleProfile,DIR825B1,$(fs_64k),DIR825B1,dir-825-b1,DIR-825-B1,ttyS0,115200,01AP94-AR7161-RT-080619-00,00AP94-AR7161-RT-080619-00))
$(eval $(call SingleProfile,DIR825B1,$(fs_64k),TEW673GRU,tew-637gru,TEW-673GRU,ttyS0,115200,01AP94-AR7161-RT-080619-01,00AP94-AR7161-RT-080619-01))

$(eval $(call SingleProfile,CyberTAN,$(fs_64k),WRT160NL,wrt160nl,WRT160NL,ttyS0,115200,1.00.01))

whrg301n_cmdline=board=WHR-G301N console=ttyS0,115200
define Image/Build/Profile/WHRG301N
	$(call Image/Build/Template/$(fs_64k)/$(1),WHRHPG300N,whr-g301n,$(whrg301n_cmdline),$(whrhpg300n_mtdlayout),WHR-G301N)
endef

whrhpg300n_cmdline=board=WHR-HP-G300N console=ttyS0,115200
whrhpg300n_mtdlayout=mtdparts=spi0.0:248k(u-boot)ro,8k(u-boot-env)ro,960k(kernel),2816k(rootfs),64k(art)ro,3712k@0x40000(firmware)
define Image/Build/Profile/WHRHPG300N
	$(call Image/Build/Template/$(fs_64k)/$(1),WHRHPG300N,whr-hp-g300n,$(whrhpg300n_cmdline),$(whrhpg300n_mtdlayout),WHR-HP-G300N)
endef

whrhpgn_cmdline=board=WHR-HP-GN console=ttyS0,115200
define Image/Build/Profile/WHRHPGN
	$(call Image/Build/Template/$(fs_64k)/$(1),WHRHPG300N,whr-hp-gn,$(whrhpgn_cmdline),$(whrhpg300n_mtdlayout),WHR-HP-GN)
endef

wlaeag300n_cmdline=board=WLAE-AG300N console=ttyS0,115200
define Image/Build/Profile/WLAEAG300N
	$(call Image/Build/Template/$(fs_64k)/$(1),WHRHPG300N,wlae-ag300n,$(wlaeag300n_cmdline),$(whrhpg300n_mtdlayout),WLAE-AG300N)
endef

wzrhpg300nh_cmdline=board=WZR-HP-G300NH console=ttyS0,115200
define Image/Build/Profile/WZRHPG300NH
	$(call Image/Build/Template/$(fs_128k)/$(1),WZRHPG30XNH,wzr-hp-g300nh,$(wzrhpg300nh_cmdline),WZR-HP-G300NH)
endef

wzrhpg300nh2_cmdline=board=WZR-HP-G300NH2 console=ttyS0,115200
define Image/Build/Profile/WZRHPG300NH2
	$(call Image/Build/Template/$(fs_128k)/$(1),WZRHPG30XNH,wzr-hp-g300nh2,$(wzrhpg300nh2_cmdline),WZR-HP-G300NH2)
endef

wzrhpag300h_cmdline=board=WZR-HP-AG300H console=ttyS0,115200
define Image/Build/Profile/WZRHPAG300H
	$(call Image/Build/Template/$(fs_64k)/$(1),WZRHPG30XNH,wzr-hp-ag300h,$(wzrhpag300h_cmdline),WZR-HP-AG300H)
endef

wzrhpg450h_cmdline=board=WZR-HP-G450H console=ttyS0,115200
define Image/Build/Profile/WZRHPG450H
	$(call Image/Build/Template/$(fs_64k)/$(1),WZRHPG30XNH,wzr-hp-g450h,$(wzrhpg450h_cmdline),WZR-HP-AG450H)
endef

zcn1523h_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,6208k(rootfs),1472k(kernel),64k(configure)ro,64k(mfg)ro,64k(art)ro,7680k@0x50000(firmware)
zcn1523h28_cmdline=board=ZCN-1523H-2 console=ttyS0,115200 $(zcn1523h_mtdlayout)
define Image/Build/Profile/ZCN1523H28
	$(call Image/Build/Template/$(fs_64k)/$(1),Zcomax,zcn-1523h-2-8,$(zcn1523h28_cmdline))
endef

zcn1523h516_cmdline=board=ZCN-1523H-5 console=ttyS0,115200 $(zcn1523h_mtdlayout)
define Image/Build/Profile/ZCN1523H516
	$(call Image/Build/Template/$(fs_64k)/$(1),Zcomax,zcn-1523h-5-16,$(zcn1523h516_cmdline))
endef

define Image/Build/Profile/Default
	$(call Image/Build/Profile/ALFAAP96,$(1))
	$(call Image/Build/Profile/ALFANX,$(1))
	$(call Image/Build/Profile/ALL0258N,$(1))
	$(call Image/Build/Profile/ALL0305,$(1))
	$(call Image/Build/Profile/AP113,$(1))
	$(call Image/Build/Profile/AP121,$(1))
	$(call Image/Build/Profile/AP121MINI,$(1))
	$(call Image/Build/Profile/AP136,$(1))
	$(call Image/Build/Profile/AP81,$(1))
	$(call Image/Build/Profile/AP83,$(1))
	$(call Image/Build/Profile/A02RBW300N,$(1))
	$(call Image/Build/Profile/DB120,$(1))
	$(call Image/Build/Profile/DIR600A1,$(1))
	$(call Image/Build/Profile/DIR601A1,$(1))
	$(call Image/Build/Profile/DIR615C1,$(1))
	$(call Image/Build/Profile/DIR615E4,$(1))
	$(call Image/Build/Profile/DIR825B1,$(1))
	$(call Image/Build/Profile/EAP7660D,$(1))
	$(call Image/Build/Profile/EWDORIN,$(1))
	$(call Image/Build/Profile/FR54RTR,$(1))
	$(call Image/Build/Profile/HORNETUB,$(1))
	$(call Image/Build/Profile/JA76PF,$(1))
	$(call Image/Build/Profile/JA76PF2,$(1))
	$(call Image/Build/Profile/JWAP003,$(1))
	$(call Image/Build/Profile/MZKW04NU,$(1))
	$(call Image/Build/Profile/MZKW300NH,$(1))
	$(call Image/Build/Profile/NBG_460N_550N_550NH,$(1))
	$(call Image/Build/Profile/OM2P,$(1))
	$(call Image/Build/Profile/PB42,$(1))
	$(call Image/Build/Profile/PB44,$(1))
	$(call Image/Build/Profile/PB92,$(1))
	$(call Image/Build/Profile/RW2458N,$(1))
	$(call Image/Build/Profile/TEW632BRP,$(1))
	$(call Image/Build/Profile/TEW652BRP,$(1))
	$(call Image/Build/Profile/TEW673GRU,$(1))
	$(call Image/Build/Profile/TEW712BR,$(1))
	$(call Image/Build/Profile/TLMR11U,$(1))
	$(call Image/Build/Profile/TLMR3020,$(1))
	$(call Image/Build/Profile/TLMR3040,$(1))
	$(call Image/Build/Profile/TLMR3220,$(1))
	$(call Image/Build/Profile/TLMR3420,$(1))
	$(call Image/Build/Profile/TLWA701,$(1))
	$(call Image/Build/Profile/TLWA901,$(1))
	$(call Image/Build/Profile/TLWDR4300,$(1))
	$(call Image/Build/Profile/TLWR703,$(1))
	$(call Image/Build/Profile/TLWR740,$(1))
	$(call Image/Build/Profile/TLWR741,$(1))
	$(call Image/Build/Profile/TLWR743,$(1))
	$(call Image/Build/Profile/TLWR841,$(1))
	$(call Image/Build/Profile/TLWR842,$(1))
	$(call Image/Build/Profile/TLWR941,$(1))
	$(call Image/Build/Profile/TLWR1041,$(1))
	$(call Image/Build/Profile/TLWR1043,$(1))
	$(call Image/Build/Profile/TLWR2543,$(1))
	$(call Image/Build/Profile/UBNT,$(1))
	$(call Image/Build/Profile/WP543,$(1))
	$(call Image/Build/Profile/WPE72,$(1))
	$(call Image/Build/Profile/WNDR3700,$(1))
	$(call Image/Build/Profile/AP96,$(1))
	$(call Image/Build/Profile/WRT400N,$(1))
	$(call Image/Build/Profile/WRT160NL,$(1))
	$(call Image/Build/Profile/WHRG301N,$(1))
	$(call Image/Build/Profile/WHRHPG300N,$(1))
	$(call Image/Build/Profile/WHRHPGN,$(1))
	$(call Image/Build/Profile/WLAEAG300N,$(1))
	$(call Image/Build/Profile/WZRHPG300NH,$(1))
	$(call Image/Build/Profile/WZRHPG300NH2,$(1))
	$(call Image/Build/Profile/WZRHPAG300H,$(1))
	$(call Image/Build/Profile/WZRHPG450H,$(1))
	$(call Image/Build/Profile/ZCN1523H28,$(1))
	$(call Image/Build/Profile/ZCN1523H516,$(1))
endef

define Image/Build/Profile/Minimal
	$(call Image/Build/Profile/Default,$(1))
endef

define Image/Build/Profile/Madwifi
	$(call Image/Build/Profile/EAP7660D,$(1))
	$(call Image/Build/Profile/UBNTRS,$(1))
	$(call Image/Build/Profile/UBNTRSPRO,$(1))
	$(call Image/Build/Profile/UBNTLSSR71,$(1))
	$(call Image/Build/Profile/WP543,$(1))
endef

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
	$(call Image/Build/Profile/$(if $(CONFIG_IB),Default,$(PROFILE)),loader)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
