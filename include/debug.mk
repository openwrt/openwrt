# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# debug flags:
#
# d: show subdirectory tree
# t: show added targets
# l: show legacy targets
# v: verbose (no .SILENCE for common targets)

ifeq ($(DEBUG),all)
  build_debug:=dltv
else
  build_debug:=$(DEBUG)
endif

define debug
$$(findstring $(2),$$(if $$(DEBUG_DIR),$$(if $$(filter $$(DEBUG_DIR)%,$(1)),$(build_debug)),$(build_debug)))
endef

define warn
$$(if $(call debug,$(1),$(2)),$$(warning $(3)))
endef

define warn_eval
$(call warn,$(1),$(2),$(3)	$(4))
$(4)
endef


