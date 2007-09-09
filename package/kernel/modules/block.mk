BLMENU:=Block Devices

define KernelPackage/ide-core
  SUBMENU:=$(BLMENU)
  TITLE:=Kernel support for IDE
  KCONFIG:= \
	CONFIG_IDE \
	CONFIG_IDE_GENERIC \
	CONFIG_BLK_DEV_IDE \
	CONFIG_IDE_GENERIC \
	CONFIG_BLK_DEV_IDEDISK
  FILES:= \
	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,40,ide-disk)
endef

define KernelPackage/ide-core/2.4
  FILES+=$(LINUX_DIR)/drivers/ide/ide-detect.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD+=$(call AutoLoad,30,ide-detect)
endef

define KernelPackage/ide-core/2.6
#  KCONFIG+=CONFIG_IDE_GENERIC
  FILES+=$(LINUX_DIR)/drivers/ide/ide-generic.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD+=$(call AutoLoad,30,ide-generic)
endef

define KernelPackage/ide-core/description
 Kernel support for IDE, useful for usb mass storage devices (e.g. on WL-HDD)
 Includes:
 - ide-core
 - ide-detect
 - ide-disk
endef

$(eval $(call KernelPackage,ide-core))


define KernelPackage/ide-pdc202xx
  SUBMENU:=$(BLMENU)
  TITLE:=PDC202xx IDE driver
  DEPENDS:=@LINUX_2_4
  KCONFIG:=CONFIG_BLK_DEV_PDC202XX_OLD
  FILES:=$(LINUX_DIR)/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef

$(eval $(call KernelPackage,ide-pdc202xx))


define KernelPackage/ide-aec62xx
  SUBMENU:=$(BLMENU)
  TITLE:=AEC62xx IDE driver
  KCONFIG:=CONFIG_BLK_DEV_AEC62XX
  FILES:=$(LINUX_DIR)/drivers/ide/pci/aec62xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,aec62xx)
endef

$(eval $(call KernelPackage,ide-aec62xx))


define KernelPackage/scsi-core
  SUBMENU:=$(BLMENU)
  TITLE:=Kernel support for SCSI
  KCONFIG:= \
	CONFIG_SCSI \
	CONFIG_BLK_DEV_SD
  FILES:= \
	$(LINUX_DIR)/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,scsi_mod) $(call AutoLoad,40,sd_mod)
endef

$(eval $(call KernelPackage,scsi-core))


define KernelPackage/libata
  SUBMENU:=$(BLMENU)
  TITLE:=libata
  DEPENDS:=@PCI_SUPPORT @LINUX_2_6
  KCONFIG:=CONFIG_ATA
  FILES:=$(LINUX_DIR)/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,21,libata)
endef

$(eval $(call KernelPackage,libata))


define KernelPackage/ata-piix
  SUBMENU:=$(BLMENU)
  TITLE:=ata-piix
  DEPENDS:=+kmod-libata +kmod-ide-core +kmod-scsi-core
  KCONFIG:=CONFIG_ATA_PIIX
  FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,ata_piix)
endef

define KernelPackage/ata-piix/description
 Kernel module for Intel PIIX SATA controller
endef

$(eval $(call KernelPackage,ata-piix))


define KernelPackage/pata-artop
  SUBMENU:=$(BLMENU)
  TITLE:=pata-artop
  DEPENDS:=+kmod-libata +kmod-scsi-core
  KCONFIG:=CONFIG_PATA_ARTOP
  FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,41,pata_artop)
endef

define KernelPackage/pata-artop/description
 Kernel module for ARTOP PATA controller
endef

$(eval $(call KernelPackage,pata-artop))
