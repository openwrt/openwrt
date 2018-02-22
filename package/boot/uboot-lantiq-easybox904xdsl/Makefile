#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=open_uboot
PKG_VERSION:=2010.06
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/u-boot-$(PKG_VERSION)
PKG_SOURCE:=u-boot-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=cd42bc64b6edafa6930ce299a144503e
PKG_CAT:=bzcat
#PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep '^CONFIG_UBOOT_' $(TOPDIR)/.config | md5s)
endif
  
define Package/$(PKG_NAME)
	SECTION:=boot
	CATEGORY:=Boot Loaders
	TITLE:=Boot-Loader for VGV952CJW33-E-IR
	SUBMENU:=u-boot-VGV952CJW33-E-IR
	DEPENDS:=@TARGET_ifxmips||TARGET_ifxcpe||TARGET_ltqcpe||TARGET_lantiq_xrx200_DEVICE_VGV952CJW33-E-IR
	URL:=http://www.denx.de/wiki/UBoot/WebHome
endef

#define Build/Configure
#endef

ifdef CONFIG_TARGET_lantiq_xrx200_DEVICE_VGV952CJW33-E-IR
UBOOT_PLATFORM:=vr9
endif

ifdef CONFIG_TARGET_ltqcpe_platform_ar9
UBOOT_PLATFORM:=ar9
endif

ifdef CONFIG_TARGET_ltqcpe_platform_danube
UBOOT_PLATFORM:=danube
endif

ifdef CONFIG_TARGET_ltqcpe_platform_amazon_se
UBOOT_PLATFORM:=amazon_se
endif

ifdef CONFIG_TARGET_ltqcpe_platform_vr9
UBOOT_PLATFORM:=vr9
endif

ifdef CONFIG_TARGET_ltqcpe_platform_ar10
UBOOT_PLATFORM:=ar10
endif

define Package/$(PKG_NAME)/config
    menu "Configuration"
            depends on PACKAGE_$(PKG_NAME)
            source "$(SOURCE)/config/Kconfig"
    endmenu
    ##### 20100726 ctc added to support DT 723 model ######
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "Target board"
		default UBOOT_CONFIG_DT_723
	config UBOOT_CONFIG_XRX200
		bool "XRX-200 EVM"
	config UBOOT_CONFIG_DT_723
		bool "DT-723"
	config UBOOT_CONFIG_DT_921
		bool "DT-921"
	config UBOOT_CONFIG_KPN
		bool "KPN"
	config UBOOT_CONFIG_VOX2
		bool "VOX 2.0"
	config UBOOT_CONFIG_OSP1
		bool "OSP1"
	config UBOOT_CONFIG_KPN2
		bool "KPN2"
	config UBOOT_CONFIG_904LTE
		bool "904 LTE"
	config UBOOT_CONFIG_904DSL
		bool "904 DSL"
	endchoice
	###################################################
    ##### 20100920 ctc added to support writable JFFS2 ######
	config UBOOT_CONFIG_JFFS2_PART
		depends on PACKAGE_$(PKG_NAME) && UBOOT_CONFIG_NAND_FLASH && UBOOT_CONFIG_FIRMWARE_IN_ROOTFS
		bool "U-Boot for writable JFFS2 file system"
		default n
	###################################################
    ##### 20100920 ctc added to support writable JFFS2 ######
	config UBOOT_CONFIG_IN_SUPERTASK
		depends on PACKAGE_$(PKG_NAME) && ( UBOOT_CONFIG_NAND_FLASH || UBOOT_CONFIG_NOR_FLASH && (UBOOT_CONFIG_NOR_FLASH_16M || UBOOT_CONFIG_NOR_FLASH_32M) ) && UBOOT_CONFIG_FIRMWARE_IN_ROOTFS
		bool "boot from supertask"
		default n
	###################################################
    ##### 20110915 ctc added to support dual image ######
	config UBOOT_CONFIG_DUAL_IMAGE
		depends on PACKAGE_$(PKG_NAME) && ( UBOOT_CONFIG_NAND_FLASH || UBOOT_CONFIG_NOR_FLASH && UBOOT_CONFIG_NOR_FLASH_32M ) && UBOOT_CONFIG_FIRMWARE_IN_ROOTFS
		bool "dual image"
		default n
	###################################################
endef

    ##### 20100920 ctc added to support writable JFFS2 ######
UBJ_DIR=XX
ifeq ($(CONFIG_UBOOT_CONFIG_JFFS2_PART),y)
	UBJ_DIR=src.jffs2
endif
    ###################################################
    ##### 20100920 ctc added to support NOR_BOOT & NAND support ######
UBN_DIR=XX
ifeq ($(CONFIG_UBOOT_CONFIG_BOOT_FROM_NOR),y)
	ifeq ($(CONFIG_UBOOT_CONFIG_NAND_FLASH),y)
		UBN_DIR=src.nor.nand
	endif
endif
    ###################################################
    ##### 20100920 ctc added to support embedded-supertask ######
UBS_DIR=XX
ifeq ($(CONFIG_UBOOT_CONFIG_IN_SUPERTASK),y)
	ifeq ($(CONFIG_UBOOT_CONFIG_BOOT_FROM_NOR),y)
		UBS_DIR=src.supertask.nor
	endif
	ifeq ($(CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND),y)
		UBS_DIR=src.supertask.nand
	endif
endif
    ###################################################
    ##### 20100726 ctc added to support DT 723 model ######
UB_DIR=XX
ifeq ($(CONFIG_UBOOT_CONFIG_XRX200),y)
	UB_DIR=src.xrx200
endif
ifeq ($(CONFIG_UBOOT_CONFIG_DT_723),y)
	UB_DIR=src.dt723
endif
ifeq ($(CONFIG_UBOOT_CONFIG_DT_921),y)
	UB_DIR=src.dt921
endif
ifeq ($(CONFIG_UBOOT_CONFIG_KPN),y)
	UB_DIR=src.kpn
endif
ifeq ($(CONFIG_UBOOT_CONFIG_VOX2),y)
	UB_DIR=src.vox2
endif
ifeq ($(CONFIG_UBOOT_CONFIG_OSP1),y)
	UB_DIR=src.osp1
endif
ifeq ($(CONFIG_UBOOT_CONFIG_KPN2),y)
	UB_DIR=src.kpn2
endif
ifeq ($(CONFIG_UBOOT_CONFIG_904LTE),y)
	UB_DIR=src.904lte
endif
ifeq ($(CONFIG_UBOOT_CONFIG_904DSL),y)
	UB_DIR=src.904dsl
endif
    ###################################################


ifeq ($(CONFIG_UBOOT_CONFIG_IFX_MEMORY_SIZE),128)
	DDR_SIZE:=128M
endif
ifeq ($(CONFIG_UBOOT_CONFIG_IFX_MEMORY_SIZE),64)
	DDR_SIZE:=64M
endif
ifeq ($(CONFIG_UBOOT_CONFIG_IFX_MEMORY_SIZE),32)
	DDR_SIZE:=32M
endif

ifdef CONFIG_TARGET_ifxcpe_platform_ar9
UBOOT_PLATFORM:=ar9
endif

ifdef CONFIG_TARGET_ifxcpe_platform_danube
UBOOT_PLATFORM:=danube
endif

ifdef CONFIG_TARGET_ifxcpe_platform_amazon_se
UBOOT_PLATFORM:=amazon_se
endif

ifdef CONFIG_TARGET_ifxcpe_platform_vr9
UBOOT_PLATFORM:=vr9
endif

ifdef CONFIG_TARGET_ifxcpe_platform_ar10
UBOOT_PLATFORM:=ar10
endif

define Build/Prepare
	$(call Build/Prepare/Default)
	# modify DDR column
	if [ "$(DDR_SIZE)" == "128M" ] ; then \
		if [ "$(CONFIG_UBOOT_CONFIG_VR9_DDR2)" = "y" ] ; then \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_lp_settings.h ; \
			\
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_lp_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_lp_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_lp_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_settings.h ; \
			sed -i -e 's/MC_CCR03_VALUE	0x100/MC_CCR03_VALUE	0x101/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_lp_settings.h ; \
			sed -i -e 's/0xBF401030 0x100/0xBF401030 0x101/' $(PKG_BUILD_DIR)/board/vr9/xrx200_ddr2_197_CAS4_config.conf ; \
		fi ; \
	elif [ "$(DDR_SIZE)" == "64M" ] ; then \
		if [ "$(CONFIG_UBOOT_CONFIG_VR9_DDR1)" = "y" ] ; then \
			sed -i -e 's/0x3000301/0x2000301/' $(PKG_BUILD_DIR)/board/vr9/ddr1/xrx200_ddr1_167M_settings.h ; \
			sed -i -e 's/0x3000301/0x2000301/' $(PKG_BUILD_DIR)/board/vr9/ddr1/xrx200_ddr1_167M_lp_settings.h ; \
			sed -i -e 's/0x3000301/0x2000301/' $(PKG_BUILD_DIR)/board/vr9/ddr1/xrx200_ddr1_200M_settings.h ; \
			sed -i -e 's/0x3000301/0x2000301/' $(PKG_BUILD_DIR)/board/vr9/ddr1/xrx200_ddr1_200M_lp_settings.h ; \
		fi ; \
		if [ "$(CONFIG_UBOOT_CONFIG_VR9_DDR2)" = "y" ] ; then \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_167M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_200M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_250M_lp_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_settings.h ; \
			sed -i -e 's/0x3000401/0x2000401/' $(PKG_BUILD_DIR)/board/vr9/ddr2/xrx200_ddr2_300M_lp_settings.h ; \
		fi ; \
	fi
endef

### ctc ###
#UBOOT_IMAGE :=$(PKG_BUILD_DIR)/u-boot.lq
#
#ifdef CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND
#UBOOT_IMAGE +=$(PKG_BUILD_DIR)/u-boot-nand.bin
#endif
ifdef CONFIG_UBOOT_CONFIG_BOOT_FROM_NOR
	UBOOT_IMAGE=$(PKG_BUILD_DIR)/u-boot.lq
endif
ifdef CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND
	UBOOT_IMAGE=$(PKG_BUILD_DIR)/u-boot-nand.bin
endif
###########


define Build/Configure
        rm -f $(PKG_BUILD_DIR)/.configured*
endef

define Build/Compile
	#ln -sdf $(TOPDIR)/../../../wlnsw/wireline_sw_linux26/open_uboot $(PKG_BUILD_DIR)/open_uboot
	cd $(PKG_BUILD_DIR);chmod a+x ./scripts_platform/*;make clean distclean;make $(UBOOT_PLATFORM)_config;\
        grep 'CONFIG_UBOOT_' $(TOPDIR)/.config | sed -e "s,\\(# \)\\?CONFIG_UBOOT_\\(.*\\),\\1\\2,g" > $(PKG_BUILD_DIR)/.config;\
        make oldconfig; make clean all CROSS_COMPILE=$(TARGET_CROSS)
endef

GPHY_FIRMWARE = $(PKG_BUILD_DIR)/board/vr9/gphy_firmware.img

define Package/$(PKG_NAME)/install
	mkdir -p $(BIN_DIR)/$(SUBTARGET)/
	### ctc ###
#	cp -f $(UBOOT_IMAGE) $(BIN_DIR)/$(SUBTARGET)/
	if [ -n $(UBOOT_IMAGE) ] ; then \
		cp -f $(UBOOT_IMAGE) $(BIN_DIR)/$(SUBTARGET)/u-boot.lq ; \
	fi \
	###########
	# generate u-boot.asc
	chmod a+x $(PKG_BUILD_DIR)/scripts_platform/gct
	if [ "$(CONFIG_UBOOT_CONFIG_VR9_DDR1)" = "y" ] ; then \
		$(PKG_BUILD_DIR)/scripts_platform/gct $(PKG_BUILD_DIR)/board/vr9/xrx200_ddr1_197_config.conf $(PKG_BUILD_DIR)/u-boot.srec $(PKG_BUILD_DIR)/u-boot.asc ; \
	fi
	if [ "$(CONFIG_UBOOT_CONFIG_VR9_DDR2)" = "y" ] ; then \
		$(PKG_BUILD_DIR)/scripts_platform/gct $(PKG_BUILD_DIR)/board/vr9/xrx200_ddr2_197_CAS4_config.conf $(PKG_BUILD_DIR)/u-boot.srec $(PKG_BUILD_DIR)/u-boot.asc ; \
	fi
	cp -f $(PKG_BUILD_DIR)/u-boot           $(BIN_DIR)/$(SUBTARGET)/u-boot.elf
	cp -f $(PKG_BUILD_DIR)/u-boot.asc       $(BIN_DIR)/$(SUBTARGET)/u-boot.asc
	cp -f $(PKG_BUILD_DIR)/u-boot.bin       $(BIN_DIR)/$(SUBTARGET)/u-boot.bin
	if [ "$(CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND)" = "y" ] ; then \
		cp -f $(BIN_DIR)/$(SUBTARGET)/u-boot.lq   $(BIN_DIR)/$(SUBTARGET)/u-boot.lq.pad.128k ;	\
		cp -f $(BIN_DIR)/$(SUBTARGET)/u-boot.lq   $(BIN_DIR)/$(SUBTARGET)/u-boot.lq.pad.256k ;	\
		$(PKG_BUILD_DIR)/scripts_platform/pad2align.sh -n 131072 $(BIN_DIR)/$(SUBTARGET)/u-boot.lq.pad.128k >& /dev/null ; \
		$(PKG_BUILD_DIR)/scripts_platform/pad2align.sh -n 262144 $(BIN_DIR)/$(SUBTARGET)/u-boot.lq.pad.256k >& /dev/null ; \
	fi
	#
	if [ -f $(GPHY_FIRMWARE) ] ; then \
		cp -f $(PKG_BUILD_DIR)/u-boot.bin -f $(PKG_BUILD_DIR)/u-boot.bin.phy ; \
		$(PKG_BUILD_DIR)/scripts_platform/pad2align.sh -n 65536 $(PKG_BUILD_DIR)/u-boot.bin.phy >& /dev/null ; \
		cat $(GPHY_FIRMWARE) >> $(PKG_BUILD_DIR)/u-boot.bin.phy ; \
		cp -f $(PKG_BUILD_DIR)/u-boot.bin.phy $(BIN_DIR)/$(SUBTARGET)/u-boot.bin.phy ; \
	fi
	#
	# change GPHY FW ADDR to 0xB0010000 for NOR flash u-boot.lq without NAND support
	# change GPHY FW ADDR to 0xB0020000 for NOR flash u-boot.lq with NAND support
	# change GPHY FW ADDR to 0xA0120000 for NAND flash u-boot.lq (0xA0100000 + GPHY_FW_offset - 0x4000) 
	# change GPHY FW ADDR to 0xA0430000 for u-boot.bin
	#
	# change BOOTSTRAP TEXT BASE to 0xB0000000 for NOR flash u-boot.lq
	# change BOOTSTRAP TEXT BASE to 0xA0100000 for NAND flash u-boot.lq
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
