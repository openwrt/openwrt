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

SDK=1
export SDK

export IS_TTY=$(shell tty -s && echo 1 || echo 0)
include $(TOPDIR)/include/verbose.mk

all: world

define stamp
tmp/info/.stamp-$(1)-$(shell ls $(2)/*/Makefile $(5) | (md5sum || md5) 2>/dev/null | cut -d' ' -f1)
endef

STAMP=$(call stamp,packageinfo,package)

$(STAMP):
	@mkdir -p tmp/info
	@rm -f tmp/info/.stamp-packageinfo*
	@touch $@

define filedep
$(foreach FILE,$(shell ls package/*/Makefile),
tmp/.packageinfo: $(FILE)
$(FILE):
	@true
)
endef

$(eval $(filedep))

tmp/.packageinfo: $(STAMP)
	@echo -n Collecting package info... 
	@$(NO_TRACE_MAKE) -s -f include/scan.mk SCAN_TARGET="packageinfo" SCAN_DIR="package" SCAN_NAME="package" SCAN_DEPS=""

package/%: FORCE
	@$(NO_TRACE_MAKE) -s tmp/.packageinfo
	$(MAKE) -C package $(patsubst package/%,%,$@) SDK=1

download: FORCE
	$(MAKE) package/download

world: FORCE
	$(MAKE) package/compile
	-( \
		cd package; \
		for configfile in `find . -maxdepth 2 -name Config.in` ; do \
			$(MAKE) compile -C `dirname $$configfile` ; \
		done \
	)

clean: FORCE
	rm -rf build_* bin

distclean: clean
	rm -rf dl .pkg*

.PHONY: FORCE
FORCE:
