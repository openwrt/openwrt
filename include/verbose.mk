# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

ifeq ($(NO_TRACE_MAKE),)
NO_TRACE_MAKE := $(MAKE) V=99
export NO_TRACE_MAKE
endif

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE:=0
endif
ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE:=$(V)
endif

ifeq ($(IS_TTY),1)
  _Y:="\\033[33m" # yellow
  _N:="\\033[m" #normal
endif

define MESSAGE
	echo -e "$(_Y)$(1)$(_N)" >&3
endef

ifneq ($(KBUILD_VERBOSE),99)
  ifeq ($(QUIET),1)
    $(MAKECMDGOALS): trace
    trace: FORCE
	@[ -f "$(MAKECMDGOALS)" ] || { \
		[ -z "$${PWD##$$TOPDIR}" ] || DIR=" -C $${PWD##$$TOPDIR/}"; \
		$(call MESSAGE, "make[$$(($(MAKELEVEL)+1))]$$DIR $(MAKECMDGOALS)"); \
	}
  else
    export QUIET:=1
    ifeq ($(KBUILD_VERBOSE),0)
      MAKE:=&>/dev/null $(MAKE)
    endif
    MAKE:=cmd() { $(MAKE) $$* || {  echo "Build failed. Please re-run make with V=99 to see what's going on"; false; } } 3>&1 4>&2; cmd
  endif

  .SILENT: $(MAKECMDGOALS)
endif
