#############################################################
#
# ncurses
# this installs only a few vital termcap entries
#
#############################################################
# Copyright (C) 2002 by Ken Restivo <ken@246gt.com>
# $Id$
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

# TARGETS
NCURSES_SITE:=ftp://ftp.gnu.org/pub/gnu/ncurses
NCURSES_DIR:=$(BUILD_DIR)/ncurses-5.2
NCURSES_SOURCE:=ncurses-5.2.tar.gz

$(DL_DIR)/$(NCURSES_SOURCE):
	$(WGET) -P $(DL_DIR) $(NCURSES_SITE)/$(NCURSES_SOURCE)

$(NCURSES_DIR)/.dist: $(DL_DIR)/$(NCURSES_SOURCE)
	gunzip -c $(DL_DIR)/$(NCURSES_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	#use the local tic and not whatever the build system was going to find.
	$(SED) 's~\$$srcdir/shlib tic\$$suffix~/usr/bin/tic~' \
		$(NCURSES_DIR)/misc/run_tic.in
	touch  $(NCURSES_DIR)/.dist

$(NCURSES_DIR)/.configured: $(NCURSES_DIR)/.dist
	(cd $(NCURSES_DIR); rm -rf config.cache; \
		BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		--with-terminfo-dirs=/usr/share/terminfo \
		--with-default-terminfo-dir=/usr/share/terminfo \
		--libdir=$(STAGING_DIR)/lib \
		--with-shared --without-cxx --without-cxx-binding \
		--without-ada --without-progs $(DISABLE_NLS) \
		--without-profile --without-debug --disable-rpath \
		--enable-echo --enable-const --enable-overwrite \
	);
	touch  $(NCURSES_DIR)/.configured

$(NCURSES_DIR)/lib/libncurses.so.5.2: $(NCURSES_DIR)/.configured
	$(MAKE) BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) \
		BUILD_CCFLAGS="-I$(NCURSES_DIR) -I$(NCURSES_DIR)/include" \
		BUILD_LDFLAGS="" DESTDIR=$(STAGING_DIR) -C $(NCURSES_DIR) \
		libs panel menu form headers

$(STAGING_DIR)/lib/libncurses.a: $(NCURSES_DIR)/lib/libncurses.so.5.2
	BUILD_CC=$(TARGET_CC) HOSTCC=$(HOSTCC) CC=$(TARGET_CC) \
	$(MAKE) \
	    prefix=$(STAGING_DIR) \
	    exec_prefix=$(STAGING_DIR) \
	    bindir=$(STAGING_DIR)/bin \
	    sbindir=$(STAGING_DIR)/sbin \
	    libexecdir=$(STAGING_DIR)/lib \
	    datadir=$(STAGING_DIR)/usr/share \
	    sysconfdir=$(STAGING_DIR)/etc \
	    localstatedir=$(STAGING_DIR)/var \
	    libdir=$(STAGING_DIR)/lib \
	    infodir=$(STAGING_DIR)/info \
	    mandir=$(STAGING_DIR)/man \
	    includedir=$(STAGING_DIR)/include \
	    gxx_include_dir=$(STAGING_DIR)/include/c++ \
	    ticdir=$(STAGING_DIR)/usr/share/terminfo \
	    -C $(NCURSES_DIR) install;
	    chmod a-x $(NCURSES_DIR)/lib/libncurses.so*
	    touch -c $(STAGING_DIR)/lib/libncurses.a 

$(TARGET_DIR)/lib/libncurses.so.5.2: $(STAGING_DIR)/lib/libncurses.a
	cp -dpf $(NCURSES_DIR)/lib/libncurses.so* $(TARGET_DIR)/lib/
	-cp -dpf $(STAGING_DIR)/usr/lib/terminfo $(TARGET_DIR)/usr/lib/
	mkdir -p $(TARGET_DIR)/usr/share/terminfo
	for i in x/xterm x/xterm-color x/xterm-xfree86 v/vt100 v/vt200 a/ansi l/linux; do \
		cp -dpf $(STAGING_DIR)/usr/share/terminfo/$${i} $(TARGET_DIR)/usr/share/terminfo/; \
	done

$(TARGET_DIR)/usr/lib/libncurses.a: $(STAGING_DIR)/lib/libncurses.a
	cp -dpf $(NCURSES_DIR)/include/curses.h $(TARGET_DIR)/usr/include/ncurses.h
	cp -dpf $(NCURSES_DIR)/include/term.h $(TARGET_DIR)/usr/include/
	cp -dpf $(NCURSES_DIR)/include/unctrl.h $(TARGET_DIR)/usr/include/
	cp -dpf $(NCURSES_DIR)/include/termcap.h $(TARGET_DIR)/usr/include/
	cp -dpf $(NCURSES_DIR)/lib/libncurses.a $(TARGET_DIR)/usr/lib/
	rm -f $(TARGET_DIR)/usr/lib/terminfo
	(cd $(TARGET_DIR)/usr/lib; ln -fs /usr/share/terminfo)
	(cd $(TARGET_DIR)/usr/lib; ln -fs libncurses.a libcurses.a)
	(cd $(TARGET_DIR)/usr/lib; ln -fs libncurses.a libtermcap.a)
	(cd $(TARGET_DIR)/usr/include; ln -fs ncurses.h curses.h)
	rm -f $(TARGET_DIR)/lib/libncurses.so
	(cd $(TARGET_DIR)/usr/lib; ln -fs /lib/libncurses.so.5.2 libncurses.so)
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/lib/libncurses.so.5.2
	touch -c $(TARGET_DIR)/usr/lib/libncurses.a

ncurses-headers: $(TARGET_DIR)/usr/lib/libncurses.a

ncurses-source: $(DL_DIR)/$(NCURSES_SOURCE)

ncurses-clean: 
	rm -f $(STAGING_DIR)/lib/libncurses.so* $(TARGET_DIR)/lib/libncurses.so*
	rm -f $(STAGING_DIR)/usr/share/tabset $(TARGET_DIR)/usr/share/tabset
	rm -rf $(STAGING_DIR)/usr/share/terminfo $(TARGET_DIR)/usr/share/terminfo
	-$(MAKE) -C $(NCURSES_DIR) clean

ncurses-dirclean: 
	rm -rf $(NCURSES_DIR)

ncurses: $(TARGET_DIR)/lib/libncurses.so.5.2

