# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
curdir:=target

$(curdir)/builddirs:=linux sdk imagebuilder
$(curdir)/builddirs-prereq:=linux
$(curdir)/builddirs-default:=linux $(if $(CONFIG_SDK),sdk) $(if $(CONFIG_IB),imagebuilder)

$(curdir)/imagebuilder/prepare:=$(curdir)/linux/install

$(eval $(call stampfile,$(curdir),target,prereq))
$(eval $(call stampfile,$(curdir),target,compile))
$(eval $(call stampfile,$(curdir),target,install))

$($(curdir)/stamp-prereq): .config
$($(curdir)/stamp-compile): $(TMP_DIR)/.build
$($(curdir)/stamp-install): $($(curdir)/stamp-compile) 

$(eval $(call subdir,$(curdir)))
