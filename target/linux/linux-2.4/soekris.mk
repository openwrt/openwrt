# $Id$

# soekris specific kmod packages

$(eval $(call KMOD_template,NATSEMI,natsemi,\
	$(MODULES_DIR)/kernel/drivers/net/natsemi.o \
,CONFIG_NATSEMI))
$(eval $(call KMOD_template,WD1100,wd1100,\
	$(MODULES_DIR)/kernel/drivers/char/wd1100.o \
,CONFIG_WD1100))

