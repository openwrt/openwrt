# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


define AddProfile
  ifeq ($(CONFIG_LINUX_$(1)),y)
    $(call Profile/$(1))
    PROFILE=$$(ID)
  endif
endef

include $(TMP_DIR)/.target.mk

ifeq ($(IB),1)
$(TMP_DIR)/.target.mk: $(TOPDIR)/.target.mk
	$(CP) $< $@
else
$(TMP_DIR)/.target.mk: $(TMP_DIR)/.targetinfo
	$(SCRIPT_DIR)/metadata.pl target_mk < $(TMP_DIR)/.targetinfo > $@
endif

