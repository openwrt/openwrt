# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

NO_TRACE_MAKE:=$(MAKE) V=99

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE=0
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE=$(V)
  endif
endif

ifneq ($(KBUILD_VERBOSE),99)
  ifeq ($(QUIET),1)
    $(MAKECMDGOALS): trace
    trace: FORCE
	@[ -f "$(MAKECMDGOALS)" ] || { \
		[ -z "$${PWD##$$TOPDIR}" ] || DIR=" -C $${PWD##$$TOPDIR/}"; \
		echo -e "\33[33mmake[$$(($(MAKELEVEL)+1))]$$DIR $(MAKECMDGOALS)\33[m" >&3; \
	}
  else
    export QUIET:=1
    ifeq ($(KBUILD_VERBOSE),0)
      MAKE:=&>/dev/null $(MAKE)
    endif
    MAKE:=cmd() { $(MAKE) $$* || {  echo "Build failed. Please re-run make with V=99 to see what's going on"; /bin/false; } } 3>&1 4>&2; cmd
  endif

  .SILENT: $(MAKECMDGOALS)
else
   NO_TRACE_MAKE:=$(MAKE)
endif
