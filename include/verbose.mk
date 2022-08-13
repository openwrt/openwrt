# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

ifndef OPENWRT_VERBOSE
  OPENWRT_VERBOSE:=
endif
ifeq ("$(origin V)", "command line")
  OPENWRT_VERBOSE:=$(V)
endif

ifeq ($(OPENWRT_VERBOSE),1)
  OPENWRT_VERBOSE:=w
endif
ifeq ($(OPENWRT_VERBOSE),99)
  OPENWRT_VERBOSE:=s
endif

# if verbose debug, override $(V)
ifneq ($(findstring v,$(DEBUG)),)
  OPENWRT_VERBOSE:=scw
endif

# suppress echo of command if not verbose (command setting)
ifndef Q
  Q:=$(if $(findstring c,$(OPENWRT_VERBOSE)),,@)
endif

# set make to be silent if not verbose (command setting)
ifndef S
  S:=$(if $(findstring c,$(OPENWRT_VERBOSE)),,-s --no-print-directory)
endif

ifeq ($(NO_TRACE_MAKE),)
NO_TRACE_MAKE := $(MAKE) $(S) $(if $(findstring s,$(OPENWRT_VERBOSE)),OPENWRT_VERBOSE=$(OPENWRT_VERBOSE),OPENWRT_VERBOSE=s$(OPENWRT_VERBOSE))
export NO_TRACE_MAKE
endif

ifeq ($(IS_TTY),1)
  ifneq ($(strip $(NO_COLOR)),1)
    _Y:=\\033[33m
    _R:=\\033[31m
    _N:=\\033[m
  endif
endif

define ERROR_MESSAGE
  ( \
	printf "$(_R)%s$(_N)\n" "$(1)" >&9 || \
	printf "$(_R)%s$(_N)\n" "$(1)" \
  ) 2>/dev/null | xargs -r >&2
endef

ifeq ($(findstring s,$(OPENWRT_VERBOSE)),)
  define MESSAGE
	( \
		printf "$(_Y)%s$(_N)\n" "$(1)" >&8 || \
		printf "$(_Y)%s$(_N)\n" "$(1)" \
	) 2>/dev/null | xargs -r >&2
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
    SUBMAKE=$(MAKE) $(S)
  else
    SILENT:=>/dev/null $(if $(findstring w,$(OPENWRT_VERBOSE)),,2>&1)
    export QUIET:=1
    SUBMAKE=cmd() { $(SILENT) $(MAKE) $(S) "$$@" < /dev/null || { echo "make $$*: build failed. Please re-run make with -j1 V=s or V=sc for a higher verbosity level to see what's going on"; false; } } 8>&1 9>&2; cmd
  endif

  .SILENT: $(MAKECMDGOALS)
else
  SUBMAKE=$(MAKE) -w
  define MESSAGE
    printf "%s\n" "$(1)"
  endef
endif
