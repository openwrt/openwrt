# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id:$

ifeq ($(NO_TRACE_MAKE),)
NO_TRACE_MAKE := $(MAKE) V=99
SUBMAKE := $(MAKE)
export NO_TRACE_MAKE
export SUBMAKE
endif

ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE:=0
endif
ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE:=$(V)
endif

ifeq ($(IS_TTY),1)
  _Y:=\\033[33m
  _N:=\\033[m
endif

ifneq ($(KBUILD_VERBOSE),99)
  define MESSAGE
	printf "$(_Y)%s$(_N)\n" "$(1)" >&3
  endef

  ifeq ($(QUIET),1)
    ifneq ($(CURDIR),$(TOPDIR))
      _DIR:=$(patsubst $(TOPDIR)/%,%,${CURDIR})
    else
      _DIR:=
    endif
    _NULL:=$(if $(MAKECMDGOALS),$(shell \
		$(call MESSAGE, make[$(MAKELEVEL)]$(if $(_DIR), -C $(_DIR)) $(MAKECMDGOALS)); \
    ))
  else
    ifeq ($(KBUILD_VERBOSE),0)
      MAKE:=>/dev/null 2>&1 $(MAKE)
    endif
    export QUIET:=1
    MAKE:=cmd() { $(MAKE) -s $$* || {  echo "make $$*: build failed. Please re-run make with V=99 to see what's going on"; false; } } 3>&1 4>&2; cmd
  endif

  .SILENT: $(MAKECMDGOALS)
else
  define MESSAGE
    printf "%s\n" "$(1)"
  endef
endif
