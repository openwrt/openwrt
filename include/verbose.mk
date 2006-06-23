# OpenWrt.org 2006
# $Id:$

ifeq ($(DUMP),)
  ifndef KBUILD_VERBOSE
    KBUILD_VERBOSE=0
    ifeq ("$(origin V)", "command line")
      KBUILD_VERBOSE=$(V)
    endif
  endif

  ifeq ($(QUIET),1)
    $(MAKECMDGOALS): trace
    trace: FORCE
	@[ -f "$(MAKECMDGOALS)" ] || { \
		[ -z "$${PWD##$$TOPDIR}" ] || DIR=" -C $${PWD##$$TOPDIR/}"; \
		echo -e "\33[33mmake[$$(($(MAKELEVEL)+1))]$$DIR $(MAKECMDGOALS)\33[m" >&3; \
	}
  else
    export QUIET:=1
    MAKE:=3>&1 4>&2 $(MAKE)
  endif

  .SILENT: $(MAKECMDGOALS)
endif
