# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2007-2020 OpenWrt.org

# define a dependency on a subtree
# parameters:
#	1: directories/files
#	2: directory dependency
#	3: tempfile for file listings
#	4: find options

DEP_FINDPARAMS := -x "*/.svn*" -x ".*" -x "*:*" -x "*\!*" -x "* *" -x "*\\\#*" -x "*/.*_check" -x "*/.*.swp" -x "*/.pkgdir*"

find_md5=$(TOPDIR)/scripts/timestamp.pl -a "-type f" $(DEP_FINDPARAMS) $(2) -- $(wildcard $(1)) | \
	 sort | $(MKHASH) md5

define rdep
  .PRECIOUS: $(2)
  .SILENT: $(2)_check

  $(2): $(2)_check
  check-depends: $(2)_check

ifneq ($(wildcard $(2)),)
  $(2)_check::
	$(if $(3), \
		$(call find_md5,$(1),$(4)) > $(3).1; \
		{ [ \! -f "$(3)" ] || diff $(3) $(3).1 >/dev/null; } && \
	) \
	{ \
		[ -f "$(2)_check.1" ] && $(MV) "$(2)_check.1"; \
		$(foreach depfile,$(1) $(2),$(TOPDIR)/scripts/timestamp.pl $(DEP_FINDPARAMS) $(4) -n $(depfile) &&) { \
			$(call debug_eval,$(SUBDIR),r,echo "No need to rebuild $(2)";) \
			touch -r "$(2)" "$(2)_check"; \
		} \
	} || { \
		$(call debug_eval,$(SUBDIR),r,echo "Need to rebuild $(2)";) \
		touch "$(2)_check"; \
	}
	$(if $(3), $(MV) $(3).1 $(3))
else
  $(2)_check::
	$(if $(3), $(RM) $(3) $(3).1)
	$(call debug_eval,$(SUBDIR),r,echo "Target $(2) not built")
endif

endef

ifeq ($(filter .%,$(MAKECMDGOALS)),$(if $(MAKECMDGOALS),$(MAKECMDGOALS),x))
  define rdep
    $(2): $(2)_check
  endef
endif
