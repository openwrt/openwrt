# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# define a dependency on a subtree
# parameters:
#	1: directory
#	2: directory dependency
#	3: tempfile for file listings
#	4: find options

FINDPARAMS := -type f -not -name ".*" -and -not -path "*.svn*" 
define rdep
  $(foreach file,$(shell find $(1) $(FINDPARAMS) -and -not -path "*:*" $(4)),
    $(2): $(file)
    $(file): ;
  )

  ifneq ($(shell find $(1) $(FINDPARAMS) $(4) 2>/dev/null | md5s),$(shell cat $(3) 2>/dev/null))
    $(2): $(3)
  endif
  
  $(3): FORCE
	@-find $(1) $(FINDPARAMS) $(4) 2>/dev/null | md5s > $$@

endef
