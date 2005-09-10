# $Id$
#
# broadcom specific kmod packages
$(eval $(call KMOD_template,DIAG,diag,\
     $(MODULES_DIR)/kernel/drivers/net/diag/diag.ko \
))
