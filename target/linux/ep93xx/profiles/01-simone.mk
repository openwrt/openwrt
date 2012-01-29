#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Simone 
  NAME:=Simplemachines Sim.One
  PACKAGES:= \
	kmod-input-core \
	kmod-input-evdev \
	kmod-input-keyboard-ep93xx \
	kmod-sound-soc-ep93xx \
	kmod-sound-soc-ep93xx-ac97 \
	kmod-sound-soc-ep93xx-simone
endef

define Profile/Simone/Description
	Package set compatible with the Simplemachines Sim.One board.
endef
$(eval $(call Profile,Simone))
