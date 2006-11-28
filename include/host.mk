# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TMP_DIR)/.host.mk

export TAR

$(TMP_DIR)/.host.mk: $(TOPDIR)/include/host.mk
	@mkdir -p $(TMP_DIR)
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
		TAR=`which gtar 2>/dev/null`; \
		[ -n "$$TAR" -a -x "$$TAR" ] || TAR=`which tar 2>/dev/null`; \
		echo "TAR:=$$TAR" >> $@; \
		ZCAT=`which gzcat 2>/dev/null`; \
		[ -n "$$ZCAT" -a -x "$$ZCAT" ] || ZCAT=`which zcat 2>/dev/null`; \
		echo "ZCAT:=$$ZCAT" >> $@; \
		echo "BASH:=$(shell which bash)" >> $@; \
	)

