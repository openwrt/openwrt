#############################################################
#
# jpeg (libraries needed by some apps)
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
JPEG_DIR=$(BUILD_DIR)/jpeg-6b
JPEG_SITE:=ftp://ftp.uu.net/graphics/jpeg/
JPEG_SOURCE=jpegsrc.v6b.tar.gz
JPEG_CAT:=zcat

$(DL_DIR)/$(JPEG_SOURCE):
	 $(WGET) -P $(DL_DIR) $(JPEG_SITE)/$(JPEG_SOURCE)

jpeg-source: $(DL_DIR)/$(JPEG_SOURCE)

$(JPEG_DIR)/.unpacked: $(DL_DIR)/$(JPEG_SOURCE)
	$(JPEG_CAT) $(DL_DIR)/$(JPEG_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	# The config.guess distributed with the package is not able
	# to handle cross compilation.  Use the one from binutils.
	cp $(BINUTILS_DIR)/config.guess $(JPEG_DIR)/
	touch $(JPEG_DIR)/.unpacked

$(JPEG_DIR)/.configured: $(JPEG_DIR)/.unpacked
	(cd $(JPEG_DIR); rm -rf config.cache; \
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
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--enable-shared \
	);
	touch  $(JPEG_DIR)/.configured

$(STAGING_DIR)/lib/libjpeg.so.62.0.0: $(JPEG_DIR)/.configured
	$(MAKE) -C $(JPEG_DIR) all
	# Note: This does not install the utilities.
	$(MAKE) -C $(JPEG_DIR) prefix=$(STAGING_DIR) exec_prefix=$(STAGING_DIR) install-headers install-lib

$(TARGET_DIR)/lib/libjpeg.so.62.0.0: $(STAGING_DIR)/lib/libjpeg.so.62.0.0
	cp -dpf $(STAGING_DIR)/lib/libjpeg.so* $(TARGET_DIR)/usr/lib/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libjpeg.so.62.0.0

jpeg: uclibc $(TARGET_DIR)/lib/libjpeg.so.62.0.0

jpeg-clean:
	-$(MAKE) -C $(JPEG_DIR) clean
