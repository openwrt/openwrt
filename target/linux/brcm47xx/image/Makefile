#
# Copyright (C) 2006-2012 OpenWrt.org
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
	# Optimized LZMA compression (with dictionary), handled by lzma-loader.
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma

	# Less optimal LZMA compression (no dictionary), handled by CFE.
	$(STAGING_DIR_HOST)/bin/lzma e -so -d16 $(KDIR)/vmlinux > $(KDIR)/vmlinux-nodictionary.lzma

	gzip -nc9 $(KDIR)/vmlinux > $(KDIR)/vmlinux.gz
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	cat $(KDIR)/vmlinux-initramfs | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux-initramfs.lzma
	$(STAGING_DIR_HOST)/bin/lzma e -so -d16 $(KDIR)/vmlinux-initramfs > $(KDIR)/vmlinux-initramfs-nodictionary.lzma
endif
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		clean install
	echo -ne "\\x00" >> $(KDIR)/loader.gz
	rm -f $(KDIR)/fs_mark
	echo -ne '\xde\xad\xc0\xde' > $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define Image/Build/wgt634u
	dd if=$(KDIR)/loader.elf of=$(BIN_DIR)/openwrt-wgt634u-$(2).bin bs=131072 conv=sync
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx >> $(BIN_DIR)/openwrt-wgt634u-$(2).bin
endef

define Image/Build/dwl3150
	cp $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx $(BIN_DIR)/openwrt-dwl3150-$(2).bin
	echo "BCM-5352-2050-0000000-01" >> $(BIN_DIR)/openwrt-dwl3150-$(2).bin
endef

# $(1): filesystem type.
# $(2): device model (used for output file).
# $(3): pattern (device specific magic).
# $(4): version.
# $(5): simplified filesystem type (without block size).
define Image/Build/CyberTAN
	$(STAGING_DIR_HOST)/bin/addpattern -4 -p $(3) -v v$(4) -i $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx -o $(BIN_DIR)/openwrt-$(2)-$(5).bin $(if $(6),-s $(6))
endef
define Image/Build/CyberTAN2
	$(STAGING_DIR_HOST)/bin/addpattern -4 -p $(3) -v v$(4) -i $(BIN_DIR)/openwrt-$(2)-$(5).noheader.bin -o $(BIN_DIR)/openwrt-$(2)-$(5).bin $(if $(6),-s $(6))
endef
define Image/Build/CyberTANHead
	$(STAGING_DIR_HOST)/bin/addpattern -5 -p $(3) -v v$(4) -i /dev/null -o $(KDIR)/openwrt-$(2)-header.bin $(if $(6),-s $(6))
endef

define Image/Build/Motorola
	$(STAGING_DIR_HOST)/bin/motorola-bin -$(3) $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(4).bin
endef

define Image/Build/USR
	$(STAGING_DIR_HOST)/bin/trx2usr $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(3).bin
endef

define Image/Build/Edi
	$(STAGING_DIR_HOST)/bin/trx2edips $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx $(BIN_DIR)/openwrt-$(2)-$(3).bin
endef

define Image/Build/Huawei
	dd if=/dev/zero of=$(BIN_DIR)/openwrt-$(2)-$(3)-gz.bin bs=92 count=1
	echo -ne 'HDR0\x08\x00\x00\x00' >> $(BIN_DIR)/openwrt-$(2)-$(3)-gz.bin
	cat $(BIN_DIR)/$(IMG_PREFIX)-$(1)-gz.trx >> $(BIN_DIR)/openwrt-$(2)-$(3)-gz.bin
endef

# $(1): filesystem type.
# $(2): device model (used for output file).
# $(3): board_id (device specific magic).
# $(4): region.
# $(5): simplified filesystem type (without block size).
define Image/Build/Chk
	$(STAGING_DIR_HOST)/bin/mkchkimg -o $(BIN_DIR)/openwrt-$(2)-$(5).chk -k $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx -b $(3) -r $(4)
endef

define trxalign/jffs2-128k
-a 0x20000 -f $(KDIR)/root.$(1)
endef
define trxalign/jffs2-64k
-a 0x10000 -f $(KDIR)/root.$(1)
endef
define trxalign/squashfs
-a 1024 -f $(KDIR)/root.$(1) $(if $(2),-f $(2)) -a 0x10000 -A $(KDIR)/fs_mark
endef

define Image/Build/trxV2
	$(call Image/Build/CyberTANHead,$(1),$(2),$(3),$(4),$(5),$(if $(6),$(6)))
	$(STAGING_DIR_HOST)/bin/trx -2 -o $(BIN_DIR)/openwrt-$(2)-$(5).noheader.bin \
		-f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(1),$(1),$(KDIR)/openwrt-$(2)-header.bin)
	$(call Image/Build/CyberTAN2,$(1),$(2),$(3),$(4),$(5),$(if $(6),$(6)))
endef

#################################################
# Subtarget generic
#################################################

# $(1): filesystem type.
define Image/Build/generic/jffs2-64k
	# BCM4705 with tg3
	$(call Image/Build/CyberTAN,$(1),wrt300n_v11,EWC2,1.51.2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt310n_v1,310N,1.0.10,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt350n_v1,EWCG,1.04.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt610n_v1,610N,1.0.1,$(patsubst jffs2-%,jffs2,$(1)))

	# BCMA SoC with SSB WiFi
	$(call Image/Build/CyberTAN,$(1),wrt610n_v2,610N,2.0.0,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e3000_v1,61XN,1.0.3,$(patsubst jffs2-%,jffs2,$(1)))
endef

# $(1): filesystem type.
define Image/Build/generic/squashfs
	$(call Image/Build/generic/jffs2-64k,$(1))
	$(call Image/Build/generic/jffs2-128k,$(1))
endef

#################################################
# Subtarget legacy
#################################################

# $(1): filesystem type.
define Image/Build/legacy/devices-with-128k-blocks
	$(call Image/Build/CyberTAN,$(1),wrt54gs,W54S,4.80.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrtsl54gs,W54U,2.08.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/trxV2,$(1),wrt54g3gv2-vf,3G2V,3.00.24,$(patsubst jffs2-%,jffs2,$(1)),6)
	$(call Image/Build/wgt634u,$(1),$(patsubst jffs2-%,jffs2,$(1)))
endef

# $(1): filesystem type.
define Image/Build/legacy/devices-with-64k-blocks
	$(call Image/Build/Chk,$(1),wgr614_v8,U12H072T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wndr3300_v1,U12H093T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr834b_v2,U12H081T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g3g,W54F,2.20.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g3g-em,W3GN,2.20.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54g,W54G,4.71.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt54gs_v4,W54s,1.09.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt150n,N150,1.51.3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt160n_v1,N150,1.50.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt300n_v1,EWCB,1.03.6,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/dwl3150,$(1),$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Edi,$(1),ps1208mfg,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Huawei,$(1),e970,$(1))
	$(call Image/Build/Motorola,$(1),wa840g,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),we800g,3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Motorola,$(1),wr850g,1,$(1))
	$(call Image/Build/USR,$(1),usr5461,$(1))
endef

# $(1): filesystem type.
define Image/Build/legacy/jffs2-64k
	$(call Image/Build/legacy/devices-with-64k-blocks,$(1))
endef

# $(1): filesystem type.
define Image/Build/legacy/jffs2-128k
	$(call Image/Build/legacy/devices-with-128k-blocks,$(1))
endef

# $(1): filesystem type.
define Image/Build/legacy/squashfs
	$(call Image/Build/legacy/devices-with-64k-blocks,$(1))
	$(call Image/Build/legacy/devices-with-128k-blocks,$(1))
endef

#################################################
# Subtarget mips74k
#################################################

define Image/Build/mips74k/devices-with-64k-blocks
	$(call Image/Build/Chk,$(1),wndr3400_v1,U12H155T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wndr3400_v2,U12H187T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wndr3700_v3,U12H194T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wndr4000,U12H181T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr1000_v3,U12H139T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr2000v2,U12H114T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr3000rp,U12H163T01_NETGEAR,1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr3500l_v1_north_america,U12H136T99_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr3500l_v1_other_regions,U12H136T99_NETGEAR,1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr3500l_v2,U12H172T00_NETGEAR,1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Chk,$(1),wnr3500_v2,U12H127T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt160n_v3,N150,3.0.3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt310n_v2,310N,2.0.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),wrt320n_v1,320N,1.0.5,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e900_v1,E900,1.0.4,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e1000_v1_v2,E100,1.1.2,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e1200_v1,E120,1.0.3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e1200_v2,E122,1.0.4,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e1500_v1,E150,1.0.5,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e1550_v1,1550,1.0.3,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e2000_v1,32XN,1.0.4,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e2500_v1_v2,E25X,1.0.7,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e3200_v1,3200,1.0.1,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/CyberTAN,$(1),e4200_v1,4200,1.0.5,$(patsubst jffs2-%,jffs2,$(1)))
endef

# $(1): filesystem type.
define Image/Build/mips74k/jffs2-64k
	$(call Image/Build/mips74k/devices-with-64k-blocks,$(1))
endef

# $(1): filesystem type.
define Image/Build/mips74k/jffs2-128k
	$(call Image/Build/mips74k/devices-with-128k-blocks,$(1))
endef

# $(1): filesystem type.
define Image/Build/mips74k/squashfs
	$(call Image/Build/mips74k/devices-with-64k-blocks,$(1))
	$(call Image/Build/mips74k/devices-with-128k-blocks,$(1))
endef

#################################################
# Shared BuildImage defines
#################################################

define Image/Build/Initramfs
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX)-initramfs.trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux-initramfs.lzma
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX)-initramfs-noloader-nodictionary.trx -f $(KDIR)/vmlinux-initramfs-nodictionary.lzma
endef

# $(1): filesystem type.
define Image/Build
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX)-$(1).trx \
		-f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(1),$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX)-$(1)-noloader-nodictionary.trx \
		-f $(KDIR)/vmlinux-nodictionary.lzma \
		$(call trxalign/$(1),$(1))
	$(STAGING_DIR_HOST)/bin/trx -o $(BIN_DIR)/$(IMG_PREFIX)-$(1)-gz.trx \
		-f $(KDIR)/vmlinux.gz \
		$(call trxalign/$(1),$(1))
	$(call Image/Build/$(SUBTARGET)/$(1),$(1))
#	$(call Image/Build/Chk,$(1),wgr614_v9,U12H094T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
#	$(call Image/Build/Chk,$(1),wndr3400_vcna,U12H155T01_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
#	$(call Image/Build/Chk,$(1),wnr3500U,U12H136T00_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
#	$(call Image/Build/Chk,$(1),wnr3500v2_VC,U12H127T70_NETGEAR,2,$(patsubst jffs2-%,jffs2,$(1)))
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/Build/Initramfs)
endif
endef

$(eval $(call BuildImage))
