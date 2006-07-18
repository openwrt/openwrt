# Makefile for OpenWrt
#
# Copyright (C) 2006 by Felix Fietkau <openwrt@nbd.name>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

RELEASE:=Kamikaze
#VERSION:=2.0 # uncomment for final release

#--------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#--------------------------------------------------------------
TOPDIR=${shell pwd}
export TOPDIR

DEVELOPER=1
export DEVELOPER

all: world

.pkginfo: FORCE
ifneq ($(shell ./scripts/timestamp.pl -p .pkginfo package Makefile),.pkginfo)
	@echo Collecting package info...
	@-for dir in package/*/; do \
		echo Source-Makefile: $${dir}Makefile; \
		$(MAKE) --no-print-dir DUMP=1 -C $$dir 2>&- || true; \
	done > $@
endif

pkginfo-clean: FORCE
	-rm -f .pkginfo .config.in

package/%: .pkginfo FORCE
	$(MAKE) -C package $(patsubst package/%,%,$@) SDK=1

download: FORCE
	$(MAKE) package/download

world: FORCE
	$(MAKE) package/compile
	-( \
		cd package; \
		find . -maxdepth 2 -name Config.in | \
			sed -e 's,/Config.in,,g' | \
			xargs -r -n1 make compile -C; \
	)

clean: FORCE
	rm -rf build_* bin

distclean: clean
	rm -rf dl .pkg*

.PHONY: FORCE
FORCE:
