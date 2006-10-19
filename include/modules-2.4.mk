# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


# Misc. devices

$(eval $(call KMOD_template,SOFTDOG,softdog,\
	$(MODULES_DIR)/kernel/drivers/char/softdog.o \
,CONFIG_SOFT_WATCHDOG,,95,softdog))



