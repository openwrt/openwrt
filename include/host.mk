# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/.host.mk

$(TOPDIR)/.host.mk: $(INCLUDE_DIR)/host.mk
	echo "HOST_OS:=`uname`" > $@
	echo "HOST_ARCH:=` \
		$(HOSTCC) -dumpmachine | sed -e s'/-.*//' \
			-e 's/sparc.*/sparc/' \
			-e 's/arm.*/arm/' \
			-e 's/m68k.*/m68k/' \
			-e 's/ppc/powerpc/' \
			-e 's/v850.*/v850/' \
			-e 's/sh[234]/sh/' \
			-e 's/mips-.*/mips/' \
			-e 's/mipsel-.*/mipsel/' \
			-e 's/cris.*/cris/' \
			-e 's/i[3-9]86/i386/'`" >> $@
	echo "GNU_HOST_NAME:=`$(HOSTCC) -dumpmachine`" >> $@
	if tar --version 2>&1 | grep 'GNU' >/dev/null; then \
		echo "TAR_OPTIONS+=--wildcards" >> $@; \
	fi

