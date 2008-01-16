# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		clean install
	echo -ne "\\x00" >> $(KDIR)/loader.gz
	rm -f $(KDIR)/fs_mark
	touch $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

ifneq ($(KERNEL),2.4)
  define Image/Build/wgt634u
	dd if=$(KDIR)/loader.elf of=$(BIN_DIR)/openwrt-wgt634u-$(2).bin bs=131072 conv=sync
	cat $(BIN_DIR)/openwrt-$(BOARD)-$(1).trx >> $(BIN_DIR)/openwrt-wgt634u-$(2).bin
  endef
endif

define Image/Build/CyberTAN
	$(STAGING_DIR_HOST)/bin/addpattern -4 -p $(3) -v v$(4) -i $(BIN_DIR)/openwrt-$(BOARD)-$(1).trx -o $(BIN_DIR)/openwrt-$(2)-$(5).bin
endef

define Image/Build/Motorola
	$(STAGING_DIR_HOST)/bin/motorola-bin -$(3) $(BIN_DIR)/openwrt-$(BOARD)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(4).bin
endef

define Image/Build/USR
	$(STAGING_DIR_HOST)/bin/trx2usr $(BIN_DIR)/openwrt-$(BOARD)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(3).bin
endef

define trxalign/jffs2-128k
-a 0x20000 -f $(KDIR)/root.$(1)
endef
define trxalign/jffs2-64k
-a 0x10000 -f $(KDIR)/root.$(1)
endef
define trxalign/squashfs
-a 1024 -f $(KDIR)/root.$(1) -a 0x10000 -A $(KDIR)/fs_mark
endef


define Image/Build/jffs2-128k
	$(call Image/Build/CyberTAN,$(1),wrt54gs,W54S,4.80.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrtsl54gs,W54U,2.08.1,$(patsubst jffs2-%,jffs2,$(1)))
  ifeq ($(KERNEL),2.6)
	$(call Image/Build/wgt634u,$(1),$(patsubst jffs2-%,jffs2,$(1)))
  endif
endef

define Image/Build/jffs2-64k
	$(call Image/Build/CyberTAN,$(1),wrt54g3g,W54F,2.20.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g3g-em,W3GN,2.20.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g,W54G,4.71.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54gs_v4,W54s,1.09.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt150n,N150,1.51.3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt300n_v1,EWCB,1.51.2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt350n_v1,EWCG,1.04.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),wa840g,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),we800g,3,$(patsubst jffs2-%,jffs2,$(1)))
endef

define Image/Build/squashfs
	$(call Image/Build/jffs2-64k,$(1))
	$(call Image/Build/jffs2-128k,$(1))
endef

define Image/Build/Initramfs
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-initramfs.trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma
endef

define Image/Build
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(1).trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1),$(1))
	$(call Image/Build/$(1),$(1))
	$(call Image/Build/Motorola,$(1),wr850g,1,$(1))
	$(call Image/Build/USR,$(1),usr5461,$(1))
endef

$(eval $(call BuildImage))
