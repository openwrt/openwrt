# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

$(TMP_DIR):
	mkdir -p $@

prereq:
	if [ -f $(TMP_DIR)/.prereq-error ]; then \
		echo; \
		cat $(TMP_DIR)/.prereq-error; \
		echo; \
		rm -rf $(TMP_DIR); \
		false; \
	fi
	rm -rf $(TMP_DIR)
	mkdir -p $(TMP_DIR)

.SILENT: $(TMP_DIR) prereq

define Require
  export PREREQ_CHECK=1
  ifeq ($$(CHECK_$(1)),)
    prereq: prereq-$(1)

    prereq-$(1): $(TMP_DIR) FORCE
		echo -n "Checking '$(1)'... "
		if $(NO_TRACE_MAKE) -f $(firstword $(MAKEFILE_LIST)) check-$(1) >/dev/null 2>/dev/null; then \
			echo 'ok.'; \
		else \
			echo 'failed.'; \
			echo -e "$(strip $(2))" >> $(TMP_DIR)/.prereq-error; \
		fi

    check-$(1): FORCE
	  $(call Require/$(1))
    CHECK_$(1):=1

    .SILENT: prereq-$(1) check-$(1)
  endif
endef


define RequireCommand
  define Require/$(1)
    which $(1)
  endef

  $$(eval $$(call Require,$(1),$(2)))
endef

