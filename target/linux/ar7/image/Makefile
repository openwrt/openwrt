# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DROP_SECTIONS:=.reginfo .mdebug .comment .note .pdr .options .MIPS.options
OBJCOPY_SREC:=$(TARGET_CROSS)objcopy -S -O srec $(addprefix --remove-section=,$(DROP_SECTIONS))

LOADADDR:=0x94600000
KERNEL_ENTRY:=0x94100000
RAMSTART:=0x94000000
RAMSIZE:=0x00100000

EVA_LOADADDR := 0x94100000

LOADER_MAKEOPTS= \
	KDIR=$(KDIR) \
	LOADADDR=$(LOADADDR) \
	KERNEL_ENTRY=$(KERNEL_ENTRY) \
	RAMSTART=$(RAMSTART) \
	RAMSIZE=$(RAMSIZE)

CFLAGS := -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -Os \
	-fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic \
	-pipe -mlong-calls -fno-common \
	-mabi=32 -march=mips32 -Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap \
	-DLOADADDR=$(LOADADDR)

define Build/Clean
	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma

	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader \
		$(LOADER_MAKEOPTS) \
		clean compile
	$(OBJCOPY_SREC) $(KDIR)/loader.elf $(KDIR)/loader.srec
	srec2bin $(KDIR)/loader.srec $(KDIR)/loader.bin
endef

define align/jffs2-64k
bs=65536 conv=sync
endef

define align/jffs2-128k
bs=131072 conv=sync
endef

define align/squashfs
bs=65536 conv=sync
endef

define Image/Build/CyberTAN
	(dd if=/dev/zero bs=16 count=1; cat $(BIN_DIR)/openwrt-$(BOARD)-$(1).bin) | \
		$(STAGING_DIR_HOST)/bin/addpattern -p $(3) -o $(BIN_DIR)/openwrt-$(2)-$(4)-code.bin
endef

#define Image/Build/sErCoMm
#	cat sercomm/adam2.bin "$(BIN_DIR)/openwrt-$(BOARD)-$(1).bin" > "$(KDIR)/dgfw.tmp"
#	dd if=sercomm/$(2) of="$(KDIR)/dgfw.tmp" bs=$$$$((0x3e0000 - 80)) seek=1 conv=notrunc
#	$(STAGING_DIR_HOST)/bin/dgfirmware -f -w "$(BIN_DIR)/openwrt-$(2)-$(3).img" "$(KDIR)/dgfw.tmp"
#	rm -f "$(KDIR)/dgfw.tmp"
#endef

define Image/Build/EVA
	$(STAGING_DIR_HOST)/bin/lzma2eva $(EVA_LOADADDR) 0x$${shell $(TARGET_CROSS)nm $(KDIR)/linux-*/vmlinux | grep kernel_entry | cut -d' ' -f1} $(KDIR)/vmlinux.lzma $(KDIR)/loader.eva 
	dd if=$(KDIR)/loader.eva $(call align/$(1)) > $(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(1).bin
	cat $(KDIR)/root.$(1) >> $(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(1).bin 
	$(call prepare_generic_squashfs,$(BIN_DIR)/openwrt-$(2)-$(KERNEL)-$(1).bin)
endef

define Image/Build
	dd if=$(KDIR)/loader.bin $(call align/$(1)) > $(BIN_DIR)/openwrt-$(BOARD)-$(1).bin
	cat $(KDIR)/root.$(1) >> $(BIN_DIR)/openwrt-$(BOARD)-$(1).bin
	$(call prepare_generic_squashfs,$(BIN_DIR)/openwrt-$(BOARD)-$(1).bin)
	$(call Image/Build/CyberTAN,$(1),AG1B,AG1B,$(1))
	$(call Image/Build/CyberTAN,$(1),WA21,WA21,$(1))
	$(call Image/Build/CyberTAN,$(1),WA22,WA22,$(1))
	$(call Image/Build/CyberTAN,$(1),WAG2,WAG2,$(1))
	$(call Image/Build/CyberTAN,$(1),AG310,AV2A -b -r 1.0,$(1))
	$(call Image/Build/CyberTAN,$(1),AG241v2,AG3A -b -r 2.0,$(1))
	$(call Image/Build/CyberTAN,$(1),AG241v2b,AG3B -b -r 2.0,$(1))
	$(call Image/Build/CyberTAN,$(1),AG241v1,AG3A -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WAG54GP2v1,ATWL -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WAG54GP2v2,CTWL -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WA31,WA31 -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WA32,WA32 -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WA7A,WA7A -b,$(1))
	$(call Image/Build/CyberTAN,$(1),WA7B,WA7B -b,$(1))
#	$(call Image/Build/sErCoMm,$(1),dg834,$(1))
#	$(call Image/Build/sErCoMm,$(1),jdr454wb,$(1))
	$(call Image/Build/EVA,$(1),EVA)
endef

$(eval $(call BuildImage))
