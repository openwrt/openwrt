#############################################################
#
# lrzsz (provides zmodem)
#
#############################################################
# Copyright (C) 2001-2003 by Erik Andersen <andersen@codepoet.org>
# Copyright (C) 2002 by Tim Riker <Tim@Rikers.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

LRZSZ_SITE:=http://www.ohse.de/uwe/releases
LRZSZ_SOURCE:=lrzsz-0.12.20.tar.gz
LRZSZ_DIR:=$(BUILD_DIR)/lrzsz-0.12.20

$(DL_DIR)/$(LRZSZ_SOURCE):
	$(WGET) -P $(DL_DIR) $(LRZSZ_SITE)/$(LRZSZ_SOURCE)

lrzsz-source: $(DL_DIR)/$(LRZSZ_SOURCE)

$(LRZSZ_DIR)/.unpacked: $(DL_DIR)/$(LRZSZ_SOURCE)
	zcat $(DL_DIR)/$(LRZSZ_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch  $(LRZSZ_DIR)/.unpacked

$(LRZSZ_DIR)/.configured: $(LRZSZ_DIR)/.unpacked
	(cd $(LRZSZ_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/tmp \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		--disable-timesync \
	);
	$(SED) "s/-lnsl//;" $(LRZSZ_DIR)/src/Makefile
	$(SED) "s~\(#define ENABLE_SYSLOG.*\)~/* \1 */~;" $(LRZSZ_DIR)/config.h
	touch  $(LRZSZ_DIR)/.configured

$(LRZSZ_DIR)/src/lrz: $(LRZSZ_DIR)/.configured
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" prefix="$(TARGET_DIR)" -C $(LRZSZ_DIR)
	$(STRIP) $(LRZSZ_DIR)/src/lrz $(LRZSZ_DIR)/src/lsz

$(TARGET_DIR)/usr/bin/rz: $(LRZSZ_DIR)/src/lrz
	cp $(LRZSZ_DIR)/src/lrz $(TARGET_DIR)/usr/bin/rz
	cp $(LRZSZ_DIR)/src/lsz $(TARGET_DIR)/usr/bin/sz

lrzsz: uclibc $(TARGET_DIR)/usr/bin/rz

lrzsz-clean:
	rm -f $(TARGET_DIR)/usr/bin/rz
	-$(MAKE) -C $(LRZSZ_DIR) clean

lrzsz-dirclean:
	rm -rf $(LRZSZ_DIR)
