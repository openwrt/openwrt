#############################################################
#
# rxvt
#
#############################################################
# Copyright (C) 2002 by Tom Walsh <Tom@OpenHardware.net>
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

RXVT_SOURCE:=rxvt-2.6.4.tar.bz2
RXVT_PATCH:=$(SOURCE_DIR)/rxvt-2.6.4.patch
RXVT_SITE:=ftp://ftp.rxvt.org/pub/rxvt/
RXVT_CAT:=bzcat
RXVT_DIR:=$(BUILD_DIR)/rxvt-2.6.4
RXVT_BINARY:=$(RXVT_DIR)/src/rxvt

$(DL_DIR)/$(RXVT_SOURCE):
	 $(WGET) -P $(DL_DIR) $(RXVT_SITE)/$(RXVT_SOURCE)

rxvt-source: $(DL_DIR)/$(RXVT_SOURCE)

$(RXVT_DIR)/.unpacked: $(DL_DIR)/$(RXVT_SOURCE)
	$(RXVT_CAT) $(DL_DIR)/$(RXVT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(RXVT_DIR)/.unpacked

$(RXVT_DIR)/.configured: $(RXVT_DIR)/.unpacked
	(cd $(RXVT_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr/X11R6 \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--x-includes=$(TINYX_DIR)/exports/include \
		--x-libraries=$(TINYX_DIR)/exports/lib \
	);
	cat $(RXVT_PATCH) | patch -d $(RXVT_DIR) -p1
	touch  $(RXVT_DIR)/.configured

$(RXVT_BINARY): $(RXVT_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(RXVT_DIR)
	$(STRIP) -x $(RXVT_BINARY)

$(TARGET_DIR)/usr/X11R6/bin/rxvt: $(RXVT_BINARY)
	cp -f $(RXVT_BINARY) $(TARGET_DIR)/usr/X11R6/bin

rxvt: tinyx $(TARGET_DIR)/usr/X11R6/bin/rxvt

rxvt-clean:
	rm -f $(TARGET_DIR)/usr/X11R6/bin/rxvt
	-$(MAKE) -C $(RXVT_DIR) clean

rxvt-dirclean:
	rm -rf $(RXVT_DIR)

