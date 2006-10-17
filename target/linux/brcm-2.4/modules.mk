define KernelPackage/lp/brcm-2.4
  FILES:= \
	$(MODULES_DIR)/kernel/drivers/parport/parport.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/parport/parport_*.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/lp.$(LINUX_KMOD_SUFFIX) \
	$(MODULES_DIR)/kernel/drivers/char/ppdev.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50, \
  	parport \
  	parport_splink \
  	lp \
  )
endef

