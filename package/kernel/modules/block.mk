BLMENU:=Block Devices

define KernelPackage/libata
	TITLE:=libata
	DESCRIPTION:=libata
	KCONFIG:=$(CONFIG_ATA)
	SUBMENU:=$(BLMENU)
	FILES:=$(LINUX_DIR)/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,libata))

define KernelPackage/ata-piix
	TITLE:=ata-piix
	DESCRIPTION:=Kernel module for Intel PIIX SATA controller
	KCONFIG:=$(CONFIG_ATA_PIIX)
	SUBMENU:=$(BLMENU)
	AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod ata_piix)
	FILES:=$(LINUX_DIR)/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=+kmod-libata +kmod-ide-core +kmod-scsi-core
endef
$(eval $(call KernelPackage,ata-piix))

define KernelPackage/pata-artop
	TITLE:=pata-artop
	DESCRIPTION:=Kernel module for ARTOP PATA controller
	KCONFIG:=$(CONFIG_PATA_ARTOP)
	SUBMENU:=$(BLMENU)
	AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod pata_artop)
	FILES:=$(LINUX_DIR)/drivers/ata/pata_artop.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=+kmod-libata +kmod-scsi-core
endef
$(eval $(call KernelPackage,pata-artop))
