# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/.host.mk

export TAR

$(TOPDIR)/.host.mk: $(INCLUDE_DIR)/host.mk
	@( \
		HOST_OS=`uname`; \
		case "$$HOST_OS" in \
			Linux) HOST_ARCH=`uname -m`;; \
			*) HOST_ARCH=`uname -p`;; \
		esac; \
		GNU_HOST_NAME=`gcc -dumpmachine`; \
		[ -n "$$GNU_HOST_NAME" ] || \
			GNU_HOST_NAME=`$(SCRIPT_DIR)/config.guess`; \
		echo "HOST_OS:=$$HOST_OS" > $@; \
		echo "HOST_ARCH:=$$HOST_ARCH" >> $@; \
		echo "GNU_HOST_NAME:=$$GNU_HOST_NAME" >> $@; \
		TAR=`which gtar`; \
		[ -n "$$TAR" -a -x "$$TAR" ] || TAR=`which tar`; \
		echo "TAR:=$$TAR" >> $@; \
		ZCAT=`which gzcat`; \
		[ -n "$$ZCAT" -a -x "$$ZCAT" ] || ZCAT=`which zcat`; \
		echo "ZCAT:=$$ZCAT" >> $@; \
	)

