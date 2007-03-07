BLMENU:=Block Devices

define KernelPackage/libata
	TITLE:=libata
	DESCRIPTION:=libata
	KCONFIG:=$(CONFIG_ATA)
	SUBMENU:=$(BLMENU)
	FILES:=$(MODULES_DIR)/kernel/drivers/ata/libata.$(LINUX_KMOD_SUFFIX)
endef
$(eval $(call KernelPackage,libata))

define KernelPackage/ata-piix
	TITLE:=ata-piix
	DESCRIPTION:=Kernel module for Intel PIIX SATA controller
	KCONFIG:=$(CONFIG_ATA_PIIX)
	SUBMENU:=$(BLMENU)
	AUTOLOAD:=$(call AutoLoad,30,scsi_mod libata sd_mod ata_piix)
	FILES:=$(MODULES_DIR)/kernel/drivers/ata/ata_piix.$(LINUX_KMOD_SUFFIX)
	DEPENDS:=+kmod-libata +kmod-ide-core +kmod-scsi-core
endef
$(eval $(call KernelPackage,ata-piix))
