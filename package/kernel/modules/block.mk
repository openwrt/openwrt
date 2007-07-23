BLMENU:=Block Devices

define KernelPackage/ide-core
  TITLE:=Kernel support for IDE
  DESCRIPTION:=\
	Useful for usb mass storage devices (e.g. on WL-HDD)\\\
	\\\
	Includes: \\\
	- ide-core \\\
	- ide-detect \\\
	- ide-disk
  KCONFIG:=CONFIG_IDE
  SUBMENU:=$(BLMENU)
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
	$(LINUX_DIR)/drivers/ide/ide-detect.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ide/ide-disk.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,ide-core) $(call AutoLoad,90,ide-generic ide-detect ide-disk)
endef
$(eval $(call KernelPackage,ide-core))

define KernelPackage/ide-pdc202xx
  TITLE:=PDC202xx IDE driver
  SUBMENU:=$(BLMENU)
  DEPENDS:=@LINUX_2_4
  KCONFIG:=CONFIG_BLK_DEV_PDC202XX_OLD
  FILES:=$(LINUX_DIR)/drivers/ide/pci/pdc202xx_old.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,pdc202xx_old)
endef
$(eval $(call KernelPackage,ide-pdc202xx))

define KernelPackage/ide-aec62xx
  TITLE:=AEC62xx IDE driver
  SUBMENU:=$(BLMENU)
  KCONFIG:=CONFIG_BLK_DEV_AEC62XX
  FILES:=$(LINUX_DIR)/drivers/ide/pci/aec62xx.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,30,aec62xx)
endef
$(eval $(call KernelPackage,ide-aec62xx))

define KernelPackage/scsi-core
  TITLE:=Kernel support for SCSI
  SUBMENU:=$(BLMENU)
  KCONFIG:=CONFIG_SCSI CONFIG_BLK_DEV_SD
  FILES:= \
    $(LINUX_DIR)/drivers/scsi/scsi_mod.$(LINUX_KMOD_SUFFIX) \
    $(LINUX_DIR)/drivers/scsi/sd_mod.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,scsi-core))

define KernelPackage/libata
	TITLE:=libata
	DESCRIPTION:=libata
	KCONFIG:=CONFIG_ATA
	SUBMENU:=$(BLMENU)
	FILES:=$(LINUX_DIR)/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=@PCI_SUPPORT @LINUX_2_6
endef
$(eval $(call KernelPackage,libata))

define KernelPackage/ata-piix
	TITLE:=ata-piix
	DESCRIPTION:=Kernel module for Intel PIIX SATA controller
	KCONFIG:=CONFIG_ATA_PIIX
	SUBMENU:=$(BLMENU)
	AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod ata_piix)
	FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=+kmod-libata +kmod-ide-core +kmod-scsi-core
endef
$(eval $(call KernelPackage,ata-piix))

define KernelPackage/pata-artop
	TITLE:=pata-artop
	DESCRIPTION:=Kernel module for ARTOP PATA controller
	KCONFIG:=CONFIG_PATA_ARTOP
	SUBMENU:=$(BLMENU)
	AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod pata_artop)
	FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=kmod-libata +kmod-scsi-core
endef
$(eval $(call KernelPackage,pata-artop))


