# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

ifneq ($(__prereq_inc),1)
__prereq_inc:=1

prereq:
	if [ -f $(TMP_DIR)/.prereq-error ]; then \
		echo; \
		cat $(TMP_DIR)/.prereq-error; \
		rm -f $(TMP_DIR)/.prereq-error; \
		echo; \
		false; \
	fi

.SILENT: prereq
endif

PREREQ_PREV=

# 1: display name
# 2: error message
define Require
  export PREREQ_CHECK=1
  ifeq ($$(CHECK_$(1)),)
    prereq: prereq-$(1)

    prereq-$(1): $(if $(PREREQ_PREV),prereq-$(PREREQ_PREV)) FORCE
		printf "Checking '$(subst *,,$(1))'... "
		if $(NO_TRACE_MAKE) -f $(firstword $(MAKEFILE_LIST)) check-$(1) >/dev/null 2>/dev/null; then \
			echo 'ok.'; \
		elif $(NO_TRACE_MAKE) -f $(firstword $(MAKEFILE_LIST)) check-$(1) >/dev/null 2>/dev/null; then \
			echo 'updated.'; \
		else \
			echo 'failed.'; \
			echo "$(PKG_NAME): $(strip $(2))" >> $(TMP_DIR)/.prereq-error; \
		fi

    check-$(1): FORCE
	  $(call Require/$(1))
    CHECK_$(1):=1

    .SILENT: prereq-$(1) check-$(1)
    .NOTPARALLEL:
  endif

  PREREQ_PREV=$(1)
endef


define RequireCommand
  define Require/$(1)
    command -v $(1)
  endef

  $$(eval $$(call Require,$(1),$(2)))
endef

define RequireHeader
  define Require/$(1)
    [ -e "$(1)" ]
  endef

  $$(eval $$(call Require,$(1),$(2)))
endef

# 1: display name
# 2: failure message
# 3: test
# 4+ alternatives (optional)
define TestHostCommand
  define Require/$(1)
	$(if $(8),($(8) || $(7) || $(6) || $(5) || $(4) || $(3)), \
		$(if $(7),($(7) || $(6) || $(5) || $(4) || $(3)), \
			$(if $(6),($(6) || $(5) || $(4) || $(3)), \
				$(if $(5),($(5) || $(4) || $(3)), \
					$(if $(4),($(4) || $(3)), \
						$(if $(3),($(3)), \
							 exit 1 ; \
						) \
					) \
				) \
			) \
		) \
	) >/dev/null 2>/dev/null
  endef

  $$(eval $$(call Require,$(1),$(2)))
endef

# 1: canonical name
# 2: failure message
# 3+: candidates
define SetupHostCommand
  define Require/$(1)
	mkdir -p "$(STAGING_DIR_HOST)/bin"; \
	for cmd in '$(call aescape,$(3))' '$(call aescape,$(4))' \
	           '$(call aescape,$(5))' '$(call aescape,$(6))' \
	           '$(call aescape,$(7))' '$(call aescape,$(8))' \
	           '$(call aescape,$(9))' '$(call aescape,$(10))' \
	           '$(call aescape,$(11))' '$(call aescape,$(12))'; do \
		if [ -n "$$$$$$$$cmd" ]; then \
			bin="$$$$$$$$(PATH="$(subst $(space),:,$(filter-out $(STAGING_DIR_HOST)/%,$(subst :,$(space),$(PATH))))" \
				command -v "$$$$$$$${cmd%% *}")"; \
			if [ -x "$$$$$$$$bin" ] && eval "$$$$$$$$cmd" >/dev/null 2>/dev/null; then \
				case "$$$$$$$$(ls -dl -- $(STAGING_DIR_HOST)/bin/$(strip $(1)))" in \
					*" -> $$$$$$$$bin"*) \
						[ -x "$(STAGING_DIR_HOST)/bin/$(strip $(1))" ] && exit 0 \
						;; \
				esac; \
				ln -sf "$$$$$$$$bin" "$(STAGING_DIR_HOST)/bin/$(strip $(1))"; \
				exit 1; \
			fi; \
		fi; \
	done; \
	exit 1
  endef

  $$(eval $$(call Require,$(1),$(if $(2),$(2),Missing $(1) command)))
endef

# 1: canonical name
# 2: failure message
# 3+: candidates
define FindHostCommand
  define Require/$(1)
	mkdir -p "$(STAGING_DIR_HOST)/bin"; \
	for bin in $$$$$$$$($(call find_bin,'$(1)',$(MAC_HOST_PATHS))); do \
		for cmd in '$(call aescape,$(3))' '$(call aescape,$(4))' \
		           '$(call aescape,$(5))' '$(call aescape,$(6))'; do \
			if [ -n "$$$$$$$$cmd" ]; then \
				cmd="$$$$$$$${bin%/*}/$$$$$$$${cmd#$$$$$$$${cmd%%[! ]*}}"; \
				bin="$$$$$$$${cmd%% *}"; \
				if [ -x "$$$$$$$$bin" ] && [ ! -L "$$$$$$$$bin" ] && \
					eval "$$$$$$$$cmd" >/dev/null 2>/dev/null; then \
					case "$$$$$$$$(ls -dl -- $(STAGING_DIR_HOST)/bin/$(strip $(subst *,,$(1))))" in \
						*" -> $$$$$$$$bin"*) \
							[ -x "$(STAGING_DIR_HOST)/bin/$(strip $(subst *,,$(1)))" ] && exit 0 \
							;; \
					esac; \
					ln -sf "$$$$$$$$bin" "$(STAGING_DIR_HOST)/bin/$(strip $(subst *,,$(1)))"; \
					exit 1; \
				fi; \
			fi; \
		done; \
	done; \
	exit 1
  endef

  $$(eval $$(call Require,$(1),$(if $(2),$(2),Missing $(1) command)))
endef
