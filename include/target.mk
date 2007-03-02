# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TMP_DIR)/.target.mk

define AddProfile
  ifeq ($(CONFIG_LINUX_$(call confname,$(KERNEL)_$(1))),y)
    PROFILE=$(1)
  endif
endef

ifeq ($(IB),1)
$(TMP_DIR)/.target.mk: $(TOPDIR)/.target.mk
	$(CP) $< $@
else
$(TMP_DIR)/.target.mk: $(TMP_DIR)/.targetinfo
	$(SCRIPT_DIR)/metadata.pl target_mk < $(TMP_DIR)/.targetinfo > $@
endif

