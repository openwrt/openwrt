BLMENU:=Block Devices

define KernelPackage/ide-core
  SUBMENU:=$(BLMENU)
  TITLE:=Kernel support for IDE
  DESCRIPTION:=\
	Useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	- ide-core \\\
	- ide-detect \\\
	- ide-disk
  KCONFIG:=CONFIG_IDE CONFIG_BLK_DEV_IDE CONFIG_IDE_GENERIC CONFIG_BLK_DEV_IDEDISK
endef

define KernelPackage/ide-core/2.4
  FILES:= \
	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-detect.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-detect ide-disk)
endef

define KernelPackage/ide-core/2.6
  FILES:= \
	$(LINUX_DIR)/drivers/ide/ide-core.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-generic.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-generic ide-disk)
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
  KCONFIG:=CONFIG_SCSI CONFIG_BLK_DEV_SD
  FILES:= \
    $(LINUX_DIR)/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX)
endef

$(eval $(call KernelPackage,scsi-core))


define KernelPackage/libata
  SUBMENU:=$(BLMENU)
  TITLE:=libata
  DESCRIPTION:=libata
  DEPENDS:=@PCI_SUPPORT @LINUX_2_6
  KCONFIG:=CONFIG_ATA
  FILES:=$(LINUX_DIR)/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
endef

$(eval $(call KernelPackage,libata))


define KernelPackage/ata-piix
  SUBMENU:=$(BLMENU)
  TITLE:=ata-piix
  DESCRIPTION:=Kernel module for Intel PIIX SATA controller
  DEPENDS:=+kmod-libata +kmod-ide-core +kmod-scsi-core
  KCONFIG:=CONFIG_ATA_PIIX
  FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod ata_piix)
endef

$(eval $(call KernelPackage,ata-piix))


define KernelPackage/pata-artop
  SUBMENU:=$(BLMENU)
  TITLE:=pata-artop
  DESCRIPTION:=Kernel module for ARTOP PATA controller
  DEPENDS:=kmod-libata +kmod-scsi-core
  KCONFIG:=CONFIG_PATA_ARTOP
  FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod pata_artop)
endef

$(eval $(call KernelPackage,pata-artop))
