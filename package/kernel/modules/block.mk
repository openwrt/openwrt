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
  AUTOLOAD:=$(call AutoLoad,21,libata,1)
endef

$(eval $(call KernelPackage,ata-core))


define KernelPackage/ata/Depends
  SUBMENU:=$(BLOCK_MENU)
  DEPENDS:=kmod-ata-core $(1)
endef


define KernelPackage/ata-ahci
$(call KernelPackage/ata/Depends,)
  TITLE:=AHCI Serial ATA support
  KCONFIG:=CONFIG_SATA_AHCI
  FILES:=$(LINUX_DIR)/drivers/ata/ahci.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,ahci,1)
endef

define KernelPackage/ata-ahci/description
 Support for AHCI Serial ATA controllers.
endef

$(eval $(call KernelPackage,ata-ahci))

define KernelPackage/ata-sil
$(call KernelPackage/ata/Depends,)
  TITLE:=Silicon Image SATA support
  KCONFIG:=CONFIG_SATA_SIL
  FILES:=$(LINUX_DIR)/drivers/ata/sata_sil.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_sil,1)
endef

define KernelPackage/ata-sil/description
 Support for Silicon Image Serial ATA controllers.
endef

$(eval $(call KernelPackage,ata-sil))


define KernelPackage/ata-sil24
$(call KernelPackage/ata/Depends,)
  TITLE:=Silicon Image 3124/3132 SATA support
  KCONFIG:=CONFIG_SATA_SIL24
  FILES:=$(LINUX_DIR)/drivers/ata/sata_sil24.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_sil24,1)
endef

define KernelPackage/ata-sil24/description
 Support for Silicon Image 3124/3132 Serial ATA controllers.
endef

$(eval $(call KernelPackage,ata-sil24))


define KernelPackage/ata-artop
$(call KernelPackage/ata/Depends,)
  TITLE:=ARTOP 6210/6260 PATA support
  KCONFIG:=CONFIG_PATA_ARTOP
  FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_artop,1)
endef

define KernelPackage/ata-artop/description
 PATA support for ARTOP 6210/6260 host controllers.
endef

$(eval $(call KernelPackage,ata-artop))

define KernelPackage/ata-octeon-cf
$(call KernelPackage/ata/Depends,@TARGET_octeon)
  TITLE:=Octeon Compact Flash support
  KCONFIG:=CONFIG_PATA_OCTEON_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_octeon_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_octeon_cf,1)
endef

define KernelPackage/ata-octeon-cf/description
  Octeon Compact Flash support.
endef

$(eval $(call KernelPackage,ata-octeon-cf))


define KernelPackage/ata-ixp4xx-cf
$(call KernelPackage/ata/Depends,@TARGET_ixp4xx)
  TITLE:=IXP4XX Compact Flash support
  KCONFIG:=CONFIG_PATA_IXP4XX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_ixp4xx_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_ixp4xx_cf,1)
endef

define KernelPackage/ata-ixp4xx-cf/description
 IXP4XX Compact Flash support.
endef

$(eval $(call KernelPackage,ata-ixp4xx-cf))

define KernelPackage/ata-rb532-cf
$(call KernelPackage/ata/Depends,@TARGET_rb532 @BROKEN)
  TITLE:=RB532 Compact Flash support
  KCONFIG:= \
  	CONFIG_PATA_PLATFORM \
  	CONFIG_PATA_RB532
  FILES:=\
  	$(LINUX_DIR)/drivers/ata/pata_platform.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ata/pata_rb532_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_platform pata_rb532_cf,1)
endef

define KernelPackage/ata-rb532-cf/description
  RB532 Compact Flash support.
endef

$(eval $(call KernelPackage,ata-rb532-cf))

define KernelPackage/ata-nvidia-sata
$(call KernelPackage/ata/Depends,)
  TITLE:=Nvidia Serial ATA support
  KCONFIG:=CONFIG_SATA_NV
  FILES:=$(LINUX_DIR)/drivers/ata/sata_nv.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_nv,1)
endef

$(eval $(call KernelPackage,ata-nvidia-sata))


define KernelPackage/ata-magicbox-cf
$(call KernelPackage/ata/Depends,@TARGET_ppc40x)
  TITLE:=Magicbox v2/OpenRB Compact flash support (ATA)
  KCONFIG:=CONFIG_PATA_MAGICBOX_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_magicbox_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_magicbox_cf,1)
endef

define KernelPackage/ata-magicbox-cf/description
  Support for Magicbox v2/OpenRB on-board CF slot. (ATA version).
endef

$(eval $(call KernelPackage,ata-magicbox-cf))


define KernelPackage/ata-piix
$(call KernelPackage/ata/Depends,)
  TITLE:=Intel PIIX PATA/SATA support
  KCONFIG:=CONFIG_ATA_PIIX
  FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,ata_piix,1)
endef

define KernelPackage/ata-piix/description
 SATA support for Intel ICH5/6/7/8 series host controllers and
 PATA support for Intel ESB/ICH/PIIX3/PIIX4 series host controllers.
endef

$(eval $(call KernelPackage,ata-piix))


define KernelPackage/ata-via-sata
$(call KernelPackage/ata/Depends,)
  TITLE:=VIA SATA support
  KCONFIG:=CONFIG_SATA_VIA
  FILES:=$(LINUX_DIR)/drivers/ata/sata_via.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,sata_via,1)
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
  FILES:= \
  	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ide/ide-gd_mod.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:= \
	$(call AutoLoad,20,ide-core,1) \
	$(call AutoLoad,40,ide-gd_mod,1)
endef

define KernelPackage/ide-core/2.4
  FILES:= \
  	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-detect.$(LINUX_KMOD_SUFFIX) \
  	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:= \
	$(call AutoLoad,20,ide-core,1) \
	$(call AutoLoad,35,ide-detect,1) \
	$(call AutoLoad,40,ide-disk,1)
endef

ifneq ($(CONFIG_arm)$(CONFIG_powerpc),y)
  define KernelPackage/ide-core/2.6
    FILES+=$(LINUX_DIR)/drivers/ide/ide-generic.$(LINUX_KMOD_SUFFIX)
    AUTOLOAD+=$(call AutoLoad,30,ide-generic,1)
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


define KernelPackage/ide/Depends
  SUBMENU:=$(BLOCK_MENU)
  DEPENDS:=kmod-ide-core $(1)
endef


define KernelPackage/ide-aec62xx
$(call KernelPackage/ide/Depends,@PCI_SUPPORT)
  TITLE:=Acard AEC62xx IDE driver
  KCONFIG:=CONFIG_BLK_DEV_AEC62XX
  FILES:=$(LINUX_DIR)/drivers/ide/aec62xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,aec62xx,1)
endef

define KernelPackage/ide-aec62xx/2.4
  FILES:=$(LINUX_DIR)/drivers/ide/pci/aec62xx.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ide-aec62xx/description
 Support for Acard AEC62xx (Artop ATP8xx) IDE controllers.
endef

$(eval $(call KernelPackage,ide-aec62xx,1))


define KernelPackage/ide-magicbox
$(call KernelPackage/ide/Depends,@TARGET_ppc40x @BROKEN)
  TITLE:=Magicbox v2/OpenRB Compact flash support (IDE)
  KCONFIG:=CONFIG_BLK_DEV_IDE_MAGICBOX
  FILES:=$(LINUX_DIR)/drivers/ide/magicbox_ide.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,magicbox_ide,1)
endef

define KernelPackage/ide-magicbox/description
 Support for Magicbox v2/OpenRB on-board CF slot. (IDE version).
endef

$(eval $(call KernelPackage,ide-magicbox))


define KernelPackage/ide-pdc202xx
$(call KernelPackage/ide/Depends,@PCI_SUPPORT)
  TITLE:=Promise PDC202xx IDE driver
  KCONFIG:=CONFIG_BLK_DEV_PDC202XX_OLD
  FILES:=$(LINUX_DIR)/drivers/ide/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old,1)
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
$(call KernelPackage/ide/Depends,@PCI_SUPPORT)
  TITLE:=ITE IT821x IDE driver
  KCONFIG:=CONFIG_BLK_DEV_IT821X
  FILES=$(LINUX_DIR)/drivers/ide/it821x.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,it821x,1)
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
	$(if $(findstring y,$(CONFIG_SCSI)),,$(LINUX_DIR)/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX)) \
	$(LINUX_DIR)/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,scsi_mod,1) $(call AutoLoad,40,sd_mod,1)
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


define KernelPackage/dm
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Device Mapper
  # All the "=n" are unnecessary, they're only there
  # to stop the config from asking the question.
  # MIRROR is M because I've needed it for pvmove.
  KCONFIG:= \
       CONFIG_BLK_DEV_MD=n \
       CONFIG_DM_DEBUG=n \
       CONFIG_DM_CRYPT=n \
       CONFIG_DM_UEVENT=n \
       CONFIG_DM_DELAY=n \
       CONFIG_DM_MULTIPATH=n \
       CONFIG_DM_ZERO=n \
       CONFIG_DM_SNAPSHOT=n \
       CONFIG_DM_LOG_USERSPACE=n \
       CONFIG_MD=y \
       CONFIG_BLK_DEV_DM \
       CONFIG_DM_MIRROR
  FILES:=$(LINUX_DIR)/drivers/md/dm-*.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,dm-mod dm-region-hash dm-mirror dm-log)
endef

define KernelPackage/dm/description
 Kernel module necessary for LVM2 support
endef

$(eval $(call KernelPackage,dm))


define KernelPackage/pata-rb153-cf
$(call KernelPackage/ata/Depends,@TARGET_adm5120_router_le)
  TITLE:=RouterBOARD 153 CF Slot support
  KCONFIG:=CONFIG_PATA_RB153_CF
  FILES:=$(LINUX_DIR)/drivers/ata/pata_rb153_cf.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pata_rb153_cf,1)
endef

define KernelPackage/pata-rb153-cf/description
  Kernel support for the RouterBoard 153 CF slot.
endef

$(eval $(call KernelPackage,pata-rb153-cf,1))


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
  AUTOLOAD:=$(call AutoLoad,29,scsi_transport_sas libsas,1)
endef

define KernelPackage/libsas/description
  SAS Domain Transport Attributes support.
endef

$(eval $(call KernelPackage,libsas,1))

define KernelPackage/mvsas
  SUBMENU:=$(BLOCK_MENU)
  TITLE:=Marvell 88SE6440 SAS/SATA driver
  DEPENDS:=@TARGET_x86 +kmod-libsas
  KCONFIG:=CONFIG_SCSI_MVSAS
  FILES:=$(LINUX_DIR)/drivers/scsi/mvsas.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,40,mvsas,1)
endef

define KernelPackage/mvsas/description
  Kernel support for the Marvell SAS SCSI adapters
endef

$(eval $(call KernelPackage,mvsas))
