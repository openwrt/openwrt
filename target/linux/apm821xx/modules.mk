define KernelPackage/ata-dwc
  TITLE:=DesignWare Cores SATA support
  KCONFIG:=CONFIG_SATA_DWC
  FILES:=$(LINUX_DIR)/drivers/ata/sata_dwc_460ex.ko
  AUTOLOAD:=$(call AutoLoad,40,sata_dwc_460ex,1)
  $(call AddDepends/ata)
endef

define KernelPackage/ata-dwc/description
 Platform support for the on-chip SATA controller.
endef

$(eval $(call KernelPackage,ata-dwc))
