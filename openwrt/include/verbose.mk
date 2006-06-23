# OpenWrt.org 2006
# $Id:$

ifndef KBUILD_VERBOSE
  ifeq ($(DUMP),)
    KBUILD_VERBOSE=0
  else
    KBUILD_VERBOSE=99
  endif
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
    MAKE:=3>&1 4>&2 $(MAKE)
  endif

  .SILENT: $(MAKECMDGOALS)
endif
