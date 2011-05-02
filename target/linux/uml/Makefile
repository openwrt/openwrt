#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/host.mk

# UML only makes sense on linux
ifeq ($(HOST_OS),Linux)

ARCH:=$(shell uname -m | sed \
	-e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/mipseb/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
)
BOARD:=uml
BOARDNAME:=User Mode Linux
FEATURES:=ext4 audio

LINUX_VERSION:=2.6.38.2

include $(INCLUDE_DIR)/target.mk

LINUX_TARGET_CONFIG:=$(CURDIR)/config/$(ARCH)

DEFAULT_PACKAGES += wpad-mini kmod-mac80211-hwsim

endif

$(eval $(call BuildTarget))
