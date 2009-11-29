#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

BLOCK_MENU:=Block Devices

define KernelPackage/ata-core
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Serial and Parallel ATA support
  DEPENDS:=@PCI_SUPPORT @LINUX_2_6 +kmod-scsi-core @!TARGET_ubicom32
  KCONFIG:=CONFIG_ATA
  FILES:=$(LINUX_DIR)/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,21,libata)
endef

$(eval $(call KernelPackage,ata-core))


define KernelPackage/ata-ahci
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=AHCI Serial ATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_SATA_AHCI
  FILES:=$(LINUX_DIR)/drivers/ata/ahci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,ahci)
endef

define KernelPackage/ata-ahci/description
 Support for AHCI Serial ATA controllers.
endef

$(eval $(call KernelPackage,ata-ahci))


define KernelPackage/ata-sil24
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Silicon Image 3124/3132 SATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_SATA_SIL24
  FILES:=$(LINUX_DIR)/drivers/ata/sata_sil24.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_sil24)
endef

define KernelPackage/ata-sil24/description
 Support for Silicon Image 3124/3132 Serial ATA controllers.
endef

$(eval $(call KernelPackage,ata-sil24))


define KernelPackage/ata-artop
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=ARTOP 6210/6260 PATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_PATA_ARTOP
  FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_artop)
endef

define KernelPackage/ata-artop/description
 PATA support for ARTOP 6210/6260 host controllers.
endef

$(eval $(call KernelPackage,ata-artop))

define KernelPackage/ata-octeon-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Octeon Compact Flash support
  DEPENDS:=kmod-ata-core @TARGET_octeon
  KCONFIG:=CONFIG_PATA_OCTEON_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_octeon_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_octeon_cf)
endef

define KernelPackage/ata-octeon-cf/description
  Octeon Compact Flash support.
endef

$(eval $(call KernelPackage,ata-octeon-cf))


define KernelPackage/ata-ixp4xx-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=IXP4XX Compact Flash support
  DEPENDS:=kmod-ata-core @TARGET_ixp4xx
  KCONFIG:=CONFIG_PATA_IXP4XX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_ixp4xx_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_ixp4xx_cf)
endef

define KernelPackage/ata-ixp4xx-cf/description
 IXP4XX Compact Flash support.
endef

$(eval $(call KernelPackage,ata-ixp4xx-cf))

define KernelPackage/ata-rb532-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=RB532 Compact Flash support
  DEPENDS:=@TARGET_rb532 kmod-ata-core @BROKEN
  KCONFIG:= \
  	CONFIG_PATA_PLATFORM \
  	CONFIG_PATA_RB532
  FILES:=\
  	$(LINUX_DIR)/drivers/ata/pata_platform.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ata/pata_rb532_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_platform pata_rb532_cf)
endef

define KernelPackage/ata-rb532-cf/description
  RB532 Compact Flash support.
endef

$(eval $(call KernelPackage,ata-rb532-cf))

define KernelPackage/ata-nvidia-sata
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Nvidia Serial ATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_SATA_NV
  FILES:=$(LINUX_DIR)/drivers/ata/sata_nv.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_nv)
endef

$(eval $(call KernelPackage,ata-nvidia-sata))


define KernelPackage/ata-magicbox-cf
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Magicbox v2/OpenRB Compact flash support (ATA)
  DEPENDS:=@TARGET_ppc40x +kmod-ata-core
  KCONFIG:=CONFIG_PATA_MAGICBOX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_magicbox_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_magicbox_cf)
endef

define KernelPackage/ata-magicbox-cf/description
  Support for Magicbox v2/OpenRB on-board CF slot. (ATA version).
endef

$(eval $(call KernelPackage,ata-magicbox-cf))


define KernelPackage/ata-piix
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Intel PIIX PATA/SATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_ATA_PIIX
  FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,ata_piix)
endef

define KernelPackage/ata-piix/description
 SATA support for Intel ICH5/6/7/8 series host controllers and
 PATA support for Intel ESB/ICH/PIIX3/PIIX4 series host controllers.
endef

$(eval $(call KernelPackage,ata-piix))


define KernelPackage/ata-via-sata
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=VIA SATA support
  DEPENDS:=kmod-ata-core
  KCONFIG:=CONFIG_SATA_VIA
  FILES:=$(LINUX_DIR)/drivers/ata/sata_via.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_via)
endef

define KernelPackage/ata-via-sata/description
 This option enables support for VIA Serial ATA.
endef

$(eval $(call KernelPackage,ata-via-sata))


define KernelPackage/ide-core
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=IDE (ATA/ATAPI) device support
  DEPENDS:=@PCI_SUPPORT
  KCONFIG:= \
	CONFIG_IDE \
	CONFIG_IDE_GENERIC \
	CONFIG_BLK_DEV_GENERIC \
	CONFIG_BLK_DEV_IDE \
	CONFIG_BLK_DEV_IDEDISK \
	CONFIG_IDE_GD \
	CONFIG_IDE_GD_ATA=y \
	CONFIG_IDE_GD_ATAPI=n \
	CONFIG_BLK_DEV_IDEDMA_PCI=y \
	CONFIG_BLK_DEV_IDEPCI=y
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.28)),1)
    FILES:= \
  	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ide/ide-gd_mod.$(LINUX_KMOD_SUFFIX)
      AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,40,ide-gd_mod)
else
    FILES:= \
  	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
      AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,40,ide-disk)
endif
endef

define KernelPackage/ide-core/2.4
  FILES+=$(LINUX_DIR)/drivers/ide/ide-detect.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD+=$(call AutoLoad,35,ide-detect)
endef

ifneq ($(CONFIG_arm)$(CONFIG_powerpc),y)
  define KernelPackage/ide-core/2.6
    FILES+=$(LINUX_DIR)/drivers/ide/ide-generic.$(LINUX_KMOD_SUFFIX)
    AUTOLOAD+=$(call AutoLoad,30,ide-generic)
  endef
endif

define KernelPackage/ide-core/description
 Kernel support for IDE, useful for usb mass storage devices (e.g. on WL-HDD)
 Includes:
 - ide-core
 - ide-detect
 - ide-gd_mod (or ide-disk)
endef

$(eval $(call KernelPackage,ide-core))


define KernelPackage/ide-aec62xx
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Acard AEC62xx IDE driver
  DEPENDS:=@PCI_SUPPORT +kmod-ide-core
  KCONFIG:=CONFIG_BLK_DEV_AEC62XX
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.28)),1)
  FILES:=$(LINUX_DIR)/drivers/ide/aec62xx.$(LINUX_KMOD_SUFFIX)
else
  FILES:=$(LINUX_DIR)/drivers/ide/pci/aec62xx.$(LINUX_KMOD_SUFFIX)
endif
  AUTOLOAD:=$(call AutoLoad,30,aec62xx)
endef

define KernelPackage/ide-aec62xx/description
 Support for Acard AEC62xx (Artop ATP8xx) IDE controllers.
endef

$(eval $(call KernelPackage,ide-aec62xx))


define KernelPackage/ide-magicbox
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Magicbox v2/OpenRB Compact flash support (IDE)
  DEPENDS:=@TARGET_ppc40x +kmod-ide-core
  KCONFIG:=CONFIG_BLK_DEV_IDE_MAGICBOX
  FILES:=$(LINUX_DIR)/drivers/ide/magicbox_ide.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,magicbox_ide)
endef

define KernelPackage/ide-magicbox/description
 Support for Magicbox v2/OpenRB on-board CF slot. (IDE version).
endef

$(eval $(call KernelPackage,ide-magicbox))


define KernelPackage/ide-pdc202xx
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Promise PDC202xx IDE driver
  DEPENDS:=+kmod-ide-core
  KCONFIG:=CONFIG_BLK_DEV_PDC202XX_OLD
 ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.28)),1)
  FILES:=$(LINUX_DIR)/drivers/ide/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
 else
  FILES:=$(LINUX_DIR)/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
 endif
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef

define KernelPackage/ide-pdc202xx/2.4
  FILES:=$(LINUX_DIR)/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ide-pdc202xx/description
 Support for the Promise Ultra 33/66/100 (PDC202{46|62|65|67|68}) IDE
 controllers.
endef

$(eval $(call KernelPackage,ide-pdc202xx))


define KernelPackage/ide-it821x
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=ITE IT821x IDE driver
  DEPENDS:=+kmod-ide-core
  KCONFIG:=CONFIG_BLK_DEV_IT821X
ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,2.6.28)),1)
  FILES=$(LINUX_DIR)/drivers/ide/it821x.$(LINUX_KMOD_SUFFIX)
else
  FILES=$(LINUX_DIR)/drivers/ide/pci/it821x.$(LINUX_KMOD_SUFFIX)
endif
  AUTOLOAD:=$(call AutoLoad,30,it821x)
endef

define KernelPackage/ide-it821x/description
  Kernel module for the ITE IDE821x IDE controllers.
endef

$(eval $(call KernelPackage,ide-it821x))


define KernelPackage/scsi-core
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=SCSI device support
  KCONFIG:= \
	CONFIG_SCSI \
	CONFIG_BLK_DEV_SD
  FILES:= \
	$(LINUX_DIR)/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,scsi_mod) $(call AutoLoad,40,sd_mod)
endef

$(eval $(call KernelPackage,scsi-core))


define KernelPackage/scsi-generic
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Kernel support for SCSI generic
  KCONFIG:= \
	CONFIG_CHR_DEV_SG
  FILES:= \
	$(LINUX_DIR)/drivers/scsi/sg.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,65,sg)
endef

$(eval $(call KernelPackage,scsi-generic))


define KernelPackage/loop
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Loopback device support
  KCONFIG:= \
	CONFIG_BLK_DEV_LOOP \
	CONFIG_BLK_DEV_CRYPTOLOOP=n
  FILES:=$(LINUX_DIR)/drivers/block/loop.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,loop)
endef

define KernelPackage/loop/description
 Kernel module for loopback device support
endef

$(eval $(call KernelPackage,loop))


define KernelPackage/nbd
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Network block device support
  KCONFIG:=CONFIG_BLK_DEV_NBD
  FILES:=$(LINUX_DIR)/drivers/block/nbd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,nbd)
endef

define KernelPackage/nbd/description
 Kernel module for network block device support
endef

$(eval $(call KernelPackage,nbd))


define KernelPackage/pata-rb153-cf
  SUBMENU:=$(BLOCK_MENU)
  DEPENDS:=kmod-ata-core @TARGET_adm5120_router_le
  TITLE:=RouterBOARD 153 CF Slot support
  KCONFIG:=CONFIG_PATA_RB153_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_rb153_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pata_rb153_cf)
endef

define KernelPackage/pata-rb153-cf/description
  Kernel support for the RouterBoard 153 CF slot.
endef

$(eval $(call KernelPackage,pata-rb153-cf))


define KernelPackage/aoe
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=ATA over Ethernet support
  KCONFIG:=CONFIG_ATA_OVER_ETH
  FILES:=$(LINUX_DIR)/drivers/block/aoe/aoe.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,aoe)
endef

define KernelPackage/aoe/description
  Kernel support for ATA over Ethernet
endef

$(eval $(call KernelPackage,aoe))

define KernelPackage/ps3vram
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=PS3 Video RAM Storage Driver
  DEPENDS:=@TARGET_ps3||TARGET_ps3chk
  KCONFIG:=CONFIG_PS3_VRAM
  FILES:=$(LINUX_DIR)/drivers/block/ps3vram.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01,ps3vram)
endef

define KernelPackage/ps3vram/description
  Kernel support for PS3 Video RAM Storage
endef

$(eval $(call KernelPackage,ps3vram))

define KernelPackage/axonram
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Axon DDR2 memory device driver
  DEPENDS:=@TARGET_pxcab
  KCONFIG:=CONFIG_AXON_RAM
  FILES:=$(LINUX_DIR)/arch/powerpc/sysdev/axonram.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,01,axonram)
endef

define KernelPackage/axonram/description
  Kernel support for Axon DDR2 memory device
endef

$(eval $(call KernelPackage,axonram))

define KernelPackage/libsas
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=SAS Domain Transport Attributes
  DEPENDS:=+kmod-scsi-core @TARGET_x86
  KCONFIG:=CONFIG_SCSI_SAS_LIBSAS \
	CONFIG_SCSI_SAS_ATTRS \
	CONFIG_SCSI_SAS_ATA=y \
	CONFIG_SCSI_SAS_HOST_SMP=y \
	CONFIG_SCSI_SAS_LIBSAS_DEBUG=y
  FILES:= \
	$(LINUX_DIR)/drivers/scsi/scsi_transport_sas.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/scsi/libsas/libsas.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,29,scsi_transport_sas libsas)
endef

define KernelPackage/libsas/description
  SAS Domain Transport Attributes support.
endef

$(eval $(call KernelPackage,libsas))

define KernelPackage/mvsas
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Marvell 88SE6440 SAS/SATA driver
  DEPENDS:=@TARGET_x86 +kmod-libsas
  KCONFIG:=CONFIG_SCSI_MVSAS
  FILES:=$(LINUX_DIR)/drivers/scsi/mvsas.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,mvsas)
endef

define KernelPackage/mvsas/description
  Kernel support for the Marvell SAS SCSI adapters
endef

$(eval $(call KernelPackage,mvsas))
