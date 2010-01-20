#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

IMGNAME:=$(BIN_DIR)/openwrt-$(BOARD)

define imgname
$(IMGNAME)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

VMLINUX:=$(IMGNAME)-vmlinux
UIMAGE:=$(IMGNAME)-uImage
fs_squash:=squashfs-only
fs_all:=all
fs_4k:=4k
fs_64k:=64k
fs_128k:=128k
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	fs_squash:=initramfs
	fs_all:=initramfs
	fs_4k:=initramfs
	fs_64k:=initramfs
	fs_128k:=initramfs
	VMLINUX:=$(IMGNAME)-vmlinux-initramfs
	UIMAGE:=$(IMGNAME)-uImage-initramfs
endif

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).bin.lzma)
endef

define PatchKernelGzip
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	gzip -9 -c $(KDIR)/vmlinux-$(1) > $(KDIR)/vmlinux-$(1).bin.gz
endef

define MkImageLzma
	mkimage -A mips -O linux -T kernel -a 0x80060000 -C lzma \
		-e 0x80060000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(1) $(2)
endef

define MkImageGzip
	mkimage -A mips -O linux -T kernel -a 0x80060000 -C gzip \
		-e 0x80060000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(1) $(2)
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(VMLINUX).elf
	cp $(KDIR)/vmlinux $(VMLINUX).bin
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	dd if=$(KDIR)/vmlinux.bin.lzma of=$(VMLINUX).lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(VMLINUX).gz bs=65536 conv=sync
	$(call MkImageGzip,$(KDIR)/vmlinux.bin.gz,$(UIMAGE)-gzip.bin)
	$(call MkImageLzma,$(KDIR)/vmlinux.bin.lzma,$(UIMAGE)-lzma.bin)
	$(call Image/Build/Initramfs)
endef

define Image/Build/WRT400N
	$(call PatchKernelLzma,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt 1310720 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 6488064 ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C lzma -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=1280k conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=6336k conv=sync; \
			echo -n $(4); \
		) > $(call imgname,$(1),$(2)).bin; \
		wrt400n $(KDIR)/vmlinux-$(2).uImage $(KDIR)/root.$(1) $(call imgname,$(1),$(2)).webui; \
	fi; fi
endef

dir600a1_mtdlayout=mtdparts=spi0.0:192k(u-boot)ro,64k(nvram)ro,896k(kernel),2816k(rootfs),64k(mac)ro,64k(art)ro,3712k@0x40000(firmware)
define Image/Build/DIR600A1
	$(call PatchKernelLzma,$(2),$(3) $(dir600a1_mtdlayout))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt 917504 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 2818048 ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C lzma -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=896k conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=2752k conv=sync; \
			echo -n "AP91-AR7240-RT-090223-00"; \
		) > $(call imgname,$(1),$(2))-universal.bin; \
	fi; fi
endef

dir825b1_mtdlayout=mtdparts=spi0.0:256k(uboot)ro,64k(config)ro,1024k(kernel),5184k(rootfs),64k(caldata)ro,1600k(unknown)ro,6208k@0x50000(firmware)
define Image/Build/DIR825B1
	$(call PatchKernelLzma,$(2),$(3) $(dir825b1_mtdlayout))
	$(call MkImageLzma,$(KDIR)/vmlinux-$(2).bin.lzma,$(call imgname,$(1),$(2)).bin)
	if [ `stat -c%s "$(call imgname,$(1),$(2)).bin"` -gt 1048576 ]; then \
	        echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
		rm -f $(call imgname,$(1),$(2)).bin; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 5308416 ]; then \
	        echo "Warning: $(KDIR)/root.$(1) is too big"; \
		rm -f $(call imgname,$(1),$(2)).bin; \
	else \
		dd if=$(KDIR)/root.$(1) of=$(call imgname,$(1),$(2)).bin bs=1k seek=1024; \
		cp $(call imgname,$(1),$(2)).bin $(call imgname,$(1),$(2))-backup-loader.bin; \
		echo -n "01AP94-AR7161-RT-080619-00" >> $(call imgname,$(1),$(2))-backup-loader.bin; \
	fi; fi
endef

define Image/Build/WZRHPG300NH
	$(call PatchKernelLzma,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt 1048576 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 31850496 ]; then \
	        echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C lzma -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=1024k conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
		) > $(call imgname,$(1),$(2))-sysupgrade.bin; \
		( \
			echo -n -e "# Airstation Public Fmt1\x00\x00\x00\x00\x00\x00\x00\x00"; \
			dd if=$(call imgname,$(1),$(2))-sysupgrade.bin; \
		) > $(call imgname,$(1),$(2))-tftp.bin; \
	fi; fi
endef

cameo_mtdlayout=mtdparts=spi0.0:128k(u-boot)ro,64k(config)ro,896k(kernel),2880k(rootfs),64k(art)ro,3776k@0x30000(firmware)
define Image/Build/Cameo
	$(call PatchKernelLzma,$(2),$(3) $(cameo_mtdlayout))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.lzma"` -gt 917504 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.lzma is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 2949120 ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C lzma -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.lzma $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=896k conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=2880k conv=sync; \
			echo -n $(4); \
		) > $(call imgname,$(1),$(2)).uni; \
	fi; fi
endef

define Image/Build/AP83
	$(call PatchKernelGzip,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.gz"` -gt 1310720 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.gz is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 6619136 ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C gzip -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.gz \
			$(KDIR)/vmlinux-$(2).uImage; \
		dd if=$(KDIR)/vmlinux-$(2).uImage \
			of=$(call imgname,kernel,$(2)).bin bs=64k conv=sync; \
		dd if=$(KDIR)/root.$(1) \
			of=$(call imgname,$(1),$(2)-rootfs).bin bs=128k conv=sync; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=1280k conv=sync; \
			dd if=$(KDIR)/root.$(1); \
		) > $(call imgname,$(1),$(2))-sysupgrade.bin; \
	fi; fi
endef

define Image/Build/PB4X
	$(call PatchKernelLzma,$(2),$(3))
	dd if=$(KDIR)/vmlinux-$(2).bin.lzma \
	   of=$(call imgname,kernel,$(2)).bin bs=64k conv=sync
	dd if=$(KDIR)/root.$(1) \
	   of=$(call imgname,$(1),$(2)-rootfs).bin bs=128k conv=sync
	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(call imgname,kernel,$(2)).bin" \
		"$(call imgname,$(1),$(2)-rootfs).bin" \
		$(call imgname,$(1),$(2))-sysupgrade.bin
endef

define Image/Build/MyLoader
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) \
		-p0x030000:0xd0000:al:0x80060000:kernel:$(KDIR)/vmlinux.bin.lzma \
		-p0x100000:0:::rootfs:$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2))-2M.img
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) -s 0x400000 \
		-p0x030000:0xd0000:al:0x80060000:kernel:$(KDIR)/vmlinux.bin.lzma \
		-p0x100000:0:::rootfs:$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2))-4M.img
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) -s 0x800000 \
		-p0x030000:0xd0000:al:0x80060000:kernel:$(KDIR)/vmlinux.bin.lzma \
		-p0x100000:0:::rootfs:$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2))-8M.img
	-$(STAGING_DIR_HOST)/bin/mkmylofw -B $(2) -s 0x1000000 \
		-p0x030000:0xd0000:al:0x80060000:kernel:$(KDIR)/vmlinux.bin.lzma \
		-p0x100000:0:::rootfs:$(KDIR)/root.$(1) \
		$(call imgname,$(1),$(2))-16M.img
endef

ubntxm_mtdlayout=mtdparts=spi0.0:256k(u-boot)ro,64k(u-boot-env)ro,1024k(kernel),6528k(rootfs),256k(cfg)ro,64k(EEPROM)ro,7552k@0x50000(firmware)
define Image/Build/UBNTXM
	$(call PatchKernelLzma,$(2),$(3) $(ubntxm_mtdlayout))
	$(call MkImageLzma,$(KDIR)/vmlinux-$(2).bin.lzma,$(KDIR)/vmlinux-$(2).uImage.bin)
	dd if=$(KDIR)/vmlinux-$(2).uImage.bin of=$(KDIR)/vmlinux-$(2).uImage bs=1024k conv=sync
	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(4) -v $(5).$(6).OpenWrt.$(REVISION) \
		-k $(KDIR)/vmlinux-$(2).uImage \
		-r $(BIN_DIR)/openwrt-$(BOARD)-root.$(1) \
		-o $(call imgname,$(1),$(2))-factory.bin
	( \
		dd if=$(KDIR)/vmlinux-$(2).uImage; \
		dd if=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1); \
	) > $(call imgname,$(1),$(2))-sysupgrade.bin
endef

define Image/Build/UBNT
	$(call PatchKernelLzma,$(2),$(3))
	dd if=$(KDIR)/vmlinux-$(2).bin.lzma of=$(KDIR)/vmlinux-$(2).lzma bs=64k conv=sync
	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B $(4) -v $(5).$(6).OpenWrt.$(REVISION) \
		-k $(KDIR)/vmlinux-$(2).lzma \
		-r $(BIN_DIR)/openwrt-$(BOARD)-root.$(1) \
		-o $(call imgname,$(1),$(2))-factory.bin
	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(KDIR)/vmlinux-$(2).lzma" \
		"$(BIN_DIR)/openwrt-$(BOARD)-root.$(1)" \
		$(call imgname,$(1),$(2))-sysupgrade.bin
endef

define Image/Build/Planex
	$(call PatchKernelGzip,$(2),$(3))
	if [ `stat -c%s "$(KDIR)/vmlinux-$(2).bin.gz"` -gt 1441792 ]; then \
		echo "Warning: $(KDIR)/vmlinux-$(2).bin.gz is too big"; \
	else if [ `stat -c%s $(KDIR)/root.$(1)` -gt 6356992 ]; then \
		echo "Warning: $(KDIR)/root.$(1) is too big"; \
	else \
		mkimage -A mips -O linux -T kernel -a 0x80060000 -C gzip -e \
			0x80060000 \
			-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(KDIR)/vmlinux-$(2).bin.gz $(KDIR)/vmlinux-$(2).uImage; \
		( \
			dd if=$(KDIR)/vmlinux-$(2).uImage bs=1408k conv=sync; \
			dd if=$(KDIR)/root.$(1) bs=6208k conv=sync; \
		) > $(call imgname,$(1),$(2)).bin; \
		$(STAGING_DIR_HOST)/bin/mkplanexfw \
			-B $(2) \
			-v 2.00.00 \
			-i $(call imgname,$(1),$(2)).bin \
			-o $(call imgname,$(1),$(2)).webui; \
	fi; fi
endef

define Image/Build/TPLINK
	$(call PatchKernelGzip,$(2),$(3))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw \
		-B $(4) -N OpenWrt -V $(REVISION)\
		-k $(KDIR)/vmlinux-$(2).bin.gz \
		-r $(BIN_DIR)/openwrt-$(BOARD)-root.$(1) \
		-o $(call imgname,$(1),$(2))-universal.bin
endef

define Image/Build/TPLINK/initramfs
	$(call PatchKernelGzip,$(2),$(3))
	-$(STAGING_DIR_HOST)/bin/mktplinkfw -c \
		-B $(4) -N OpenWrt -V $(REVISION)\
		-k $(KDIR)/vmlinux-$(2).bin.gz \
		-o $(call imgname,$(1),$(2))-uImage.bin
endef

define Image/Build/CyberTAN
	$(call PatchKernelGzip,$(2),$(3))
	$(call MkImageGzip,$(KDIR)/vmlinux-$(2).bin.gz,$(KDIR)/vmlinux-$(2).uImage)
	( \
		dd if=$(KDIR)/vmlinux-$(2).uImage bs=64k conv=sync; \
		dd if=/dev/zero bs=1 count=65476; \
		dd if=$(KDIR)/root.$(1) bs=64k; \
	) > $(KDIR)/vmlinux-$(2).image
	$(STAGING_DIR_HOST)/bin/trx -o $(KDIR)/vmlinux-$(2).trx \
		-f $(KDIR)/vmlinux-$(2).image
	-$(STAGING_DIR_HOST)/bin/addpattern -B $(2) -v v$(4) -g \
		-i $(KDIR)/vmlinux-$(2).trx \
		-o $(call imgname,$(1),$(2)).bin
endef

wndr3700_mtdlayout=mtdparts=spi0.0:320k(u-boot)ro,128k(u-boot-env)ro,1024k(kernel),6656k(rootfs),64k(art)ro,7680k@0x70000(firmware)
define Image/Build/WNDR3700
	$(call PatchKernelLzma,$(2),$(3) $(wndr3700_mtdlayout))
	$(call MkImageLzma,$(KDIR)/vmlinux-$(2).bin.lzma,$(KDIR)/vmlinux-$(2).uImage)
	mkdir -p $(KDIR)/wndr3700/image
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR)/vmlinux-$(2).uImage \
		$(KDIR)/wndr3700/image/uImage
	$(STAGING_DIR_HOST)/bin/mksquashfs-lzma \
		$(KDIR)/wndr3700 $(KDIR)/vmlinux-$(2).uImage.squashfs.tmp \
		-nopad -noappend -root-owned -be
	-rm -rf $(KDIR)/wndr3700
	mkimage -A mips -O linux -T filesystem -C none \
		-a 0xbf070000 -e 0xbf070000 \
		-n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(KDIR)/vmlinux-$(2).uImage.squashfs.tmp \
		$(KDIR)/vmlinux-$(2).uImage.squashfs.tmp2
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR)/vmlinux-$(2).uImage.squashfs.tmp2 \
		$(KDIR)/vmlinux-$(2).uImage.squashfs
	-rm -f $(KDIR)/vmlinux-$(2).uImage.squashfs.tmp*
	( \
		dd if=$(KDIR)/vmlinux-$(2).uImage.squashfs bs=1024k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=64k; \
	) > $(call imgname,$(1),$(2))-sysupgrade.bin
	$(STAGING_DIR_HOST)/bin/mkdniimg \
		-B WNDR3700 -v OpenWrt.$(REVISION) \
		-i $(call imgname,$(1),$(2))-sysupgrade.bin \
		-o $(call imgname,$(1),$(2))-factory.img
endef

define Image/Build/WNDR3700/initramfs
	$(call PatchKernelLzma,$(2),$(3) $(wndr3700_mtdlayout))
	$(call MkImageLzma,$(KDIR)/vmlinux-$(2).bin.lzma,$(KDIR)/vmlinux-$(2).uImage)
	$(STAGING_DIR_HOST)/bin/wndr3700 \
		$(KDIR)/vmlinux-$(2).uImage \
		$(call imgname,$(1),$(2))-uImage.bin
endef

define Image/Build/Template/initramfs/initramfs
	$(call Image/Build/$(1)/initramfs,initramfs,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/all/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/all/jffs2-64k
	$(call Image/Build/$(1),jffs2-64k,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/all/jffs2-128k
	$(call Image/Build/$(1),jffs2-128k,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/squashfs-only/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/4k/squashfs
	$(call Image/Build/$(1),squashfs-4k,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/64k/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/64k/jffs2-64k
	$(call Image/Build/$(1),jffs2-64k,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/128k/squashfs
	$(call Image/Build/$(1),squashfs,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Template/128k/jffs2-128k
	$(call Image/Build/$(1),jffs2-128k,$(2),$(3),$(4),$(5),$(6),$(7))
endef

define Image/Build/Profile/AP81
	$(call Image/Build/Template/$(fs_64k)/$(1),AP83,ap81,board=AP81)
endef

define Image/Build/Profile/AP83
	$(call Image/Build/Template/$(fs_64k)/$(1),AP83,ap83,board=AP83)
endef

define Image/Build/Profile/PB42
	$(call Image/Build/Template/$(fs_64k)/$(1),PB4X,pb42,board=PB42)
endef

define Image/Build/Profile/PB44
	$(call Image/Build/Template/$(fs_64k)/$(1),PB4X,pb44,board=PB44)
endef

define Image/Build/Profile/WP543
	$(call Image/Build/Template/$(fs_64k)/$(1),MyLoader,wp543)
endef

define Image/Build/Profile/DIR600A1
	$(call Image/Build/Template/$(fs_64k)/$(1),DIR600A1,dir-600-a1,board=DIR-600-A1)
endef

define Image/Build/Profile/DIR615C1
	$(call Image/Build/Template/$(fs_squash)/$(1),Cameo,dir-615-c1,board=DIR-615-C1,"AP81-AR9130-RT-070614-02")
endef

define Image/Build/Profile/TEW632BRP
	$(call Image/Build/Template/$(fs_squash)/$(1),Cameo,tew-632brp,board=TEW-632BRP,"AP81-AR9130-RT-070614-00")
endef

define Image/Build/Profile/TEW652BRP
	$(call Image/Build/Template/$(fs_squash)/$(1),Cameo,tew-652brp,board=TEW-632BRP,"AP81-AR9130-RT-080609-05")
endef

define Image/Build/Profile/A02RBW300N
	$(call Image/Build/Template/$(fs_squash)/$(1),Cameo,a02-rb-w300n,board=TEW-632BRP,"AP81-AR9130-RT-070614-03")
endef

define Image/Build/Profile/UBNTRS
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-rs,board=UBNT-RS,RS,RSx,ar7100)
endef

define Image/Build/Profile/UBNTRSPRO
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-rspro,board=UBNT-RSPRO,RSPRO,RSPRO,ar7100pro)
endef

define Image/Build/Profile/UBNTLSSR71
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNT,ubnt-ls-sr71,board=UBNT-LS-SR71,LS-SR71,LS-SR71,ar7100)
endef

define Image/Build/Profile/UBNTBULLETM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-bullet-m,board=UBNT-BM,XM,UBNTXM,ar7240)
endef

define Image/Build/Profile/UBNTROCKETM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-rocket-m,board=UBNT-RM,XM,UBNTXM,ar7240)
endef

define Image/Build/Profile/UBNTNANOM
	$(call Image/Build/Template/$(fs_64k)/$(1),UBNTXM,ubnt-nano-m,board=UBNT-NM,XM,UBNTXM,ar7240)
endef

define Image/Build/Profile/UBNT
	$(call Image/Build/Profile/UBNTRS,$(1))
	$(call Image/Build/Profile/UBNTRSPRO,$(1))
	$(call Image/Build/Profile/UBNTLSSR71,$(1))
	$(call Image/Build/Profile/UBNTBULLETM,$(1))
	$(call Image/Build/Profile/UBNTROCKETM,$(1))
	$(call Image/Build/Profile/UBNTNANOM,$(1))
endef

define Image/Build/Profile/MZKW04NU
	$(call Image/Build/Template/$(fs_64k)/$(1),Planex,mzk-w04nu,board=MZK-W04NU)
endef

define Image/Build/Profile/MZKW300NH
	$(call Image/Build/Template/$(fs_64k)/$(1),Planex,mzk-w300nh,board=MZK-W300NH)
endef

define Image/Build/Profile/TLWR741NDV1
	$(call Image/Build/Template/$(fs_4k)/$(1),TPLINK,tl-wr741nd-v1,board=TL-WR741ND,TL-WR741NDv1)
endef

define Image/Build/Profile/TLWR841NV15
	$(call Image/Build/Template/$(fs_squash)/$(1),TPLINK,tl-wr841n-v1.5,board=TL-WR841N-v1.5,TL-WR841Nv1.5)
endef

define Image/Build/Profile/TLWR841NDV3
	$(call Image/Build/Template/$(fs_squash)/$(1),TPLINK,tl-wr841nd-v3,board=TL-WR941ND,TL-WR841NDv3)
endef

define Image/Build/Profile/TLWR841NDV5
	$(call Image/Build/Template/$(fs_4k)/$(1),TPLINK,tl-wr841nd-v5,board=TL-WR741ND,TL-WR841NDv5)
endef

define Image/Build/Profile/TLWR941NDV2
	$(call Image/Build/Template/$(fs_squash)/$(1),TPLINK,tl-wr941nd-v2,board=TL-WR941ND,TL-WR941NDv2)
endef

define Image/Build/Profile/TLWR1043NDV1
	$(call Image/Build/Template/$(fs_squash)/$(1),TPLINK,tl-wr1043nd-v1,board=TL-WR1043ND,TL-WR1043NDv1)
endef

define Image/Build/Profile/WNDR3700
	$(call Image/Build/Template/$(fs_64k)/$(1),WNDR3700,wndr3700,board=WNDR3700)
endef

define Image/Build/Profile/WRT400N
	$(call Image/Build/Template/$(fs_64k)/$(1),WRT400N,wrt400n,board=WRT400N)
endef

define Image/Build/Profile/DIR825B1
	$(call Image/Build/Template/$(fs_64k)/$(1),DIR825B1,dir-825-b1,board=DIR-825-B1)
endef

define Image/Build/Profile/WRT160NL
	$(call Image/Build/Template/$(fs_64k)/$(1),CyberTAN,wrt160nl,board=WRT160NL,1.00.01)
endef

define Image/Build/Profile/WZRHPG300NH
	$(call Image/Build/Template/$(fs_128k)/$(1),WZRHPG300NH,wzr-hp-g300nh,board=WZR-HP-G300NH)
endef

define Image/Build/Profile/Default
	$(call Image/Build/Profile/AP81,$(1))
	$(call Image/Build/Profile/AP83,$(1))
	$(call Image/Build/Profile/A02RBW300N,$(1))
	$(call Image/Build/Profile/DIR600A1,$(1))
	$(call Image/Build/Profile/DIR615C1,$(1))
	$(call Image/Build/Profile/DIR825B1,$(1))
	$(call Image/Build/Profile/MZKW04NU,$(1))
	$(call Image/Build/Profile/MZKW300NH,$(1))
	$(call Image/Build/Profile/PB42,$(1))
	$(call Image/Build/Profile/PB44,$(1))
	$(call Image/Build/Profile/TEW632BRP,$(1))
	$(call Image/Build/Profile/TEW652BRP,$(1))
	$(call Image/Build/Profile/TLWR741NDV1,$(1))
	$(call Image/Build/Profile/TLWR841NV15,$(1))
	$(call Image/Build/Profile/TLWR841NDV3,$(1))
	$(call Image/Build/Profile/TLWR841NDV5,$(1))
	$(call Image/Build/Profile/TLWR941NDV2,$(1))
	$(call Image/Build/Profile/TLWR1043NDV1,$(1))
	$(call Image/Build/Profile/UBNT,$(1))
	$(call Image/Build/Profile/WP543,$(1))
	$(call Image/Build/Profile/WNDR3700,$(1))
	$(call Image/Build/Profile/WRT400N,$(1))
	$(call Image/Build/Profile/WRT160NL,$(1))
	$(call Image/Build/Profile/WZRHPG300NH,$(1))
endef

define Image/Build/Profile/Madwifi
	$(call Image/Build/Profile/UBNTRS,$(1))
	$(call Image/Build/Profile/UBNTRSPRO,$(1))
	$(call Image/Build/Profile/UBNTLSSR71,$(1))
	$(call Image/Build/Profile/WP543,$(1))
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	dd if=$(KDIR)/root.squashfs of=$(KDIR)/root.squashfs-4k.tmp0 bs=4k conv=sync
	$(call add_jffs2_mark,$(KDIR)/root.squashfs-4k.tmp0)
	dd if=$(KDIR)/root.squashfs-4k.tmp0 of=$(IMGNAME)-root.squashfs-4k bs=4k conv=sync
	$(call add_jffs2_mark,$(IMGNAME)-root.squashfs-4k)
	rm -f $(KDIR)/root.squashfs-4k.tmp0
endef

define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),initramfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(IMGNAME)-root.$(1) bs=128k conv=sync

	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
