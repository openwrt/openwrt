# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TMP_DIR)/.target.mk

$(TMP_DIR)/.target.mk:
	$(SCRIPT_DIR)/gen_target_mk.pl < $(TMP_DIR)/.targetinfo > $@

