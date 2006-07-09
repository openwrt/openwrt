# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define kernel_template
ifeq ($(CONFIG_LINUX_$(3)),y)
KERNEL:=$(1)
BOARD:=$(2)
endif
endef

$(eval $(call kernel_template,2.4,brcm,2_4_BRCM))
$(eval $(call kernel_template,2.4,ar7,2_4_AR7))
$(eval $(call kernel_template,2.4,x86,2_4_X86))
$(eval $(call kernel_template,2.6,brcm,2_6_BRCM))
$(eval $(call kernel_template,2.6,brcm63xx,2_6_BRCM63XX))
$(eval $(call kernel_template,2.6,rb532,2_6_RB532))
$(eval $(call kernel_template,2.6,x86,2_6_X86))
$(eval $(call kernel_template,2.4,ar531x,2_4_AR531X))
$(eval $(call kernel_template,2.6,aruba,2_6_ARUBA))
$(eval $(call kernel_template,2.6,au1000,2_6_AU1000))
$(eval $(call kernel_template,2.6,xscale,2_6_XSCALE))
$(eval $(call kernel_template,2.6,sibyte,2_6_SIBYTE))

export BOARD
export KERNEL


