# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/verbose.mk

$(TMP_DIR):
	mkdir -p $@

prereq:
	@echo
	@if [ -f $(TMP_DIR)/.prereq-error ]; then \
		cat $(TMP_DIR)/.prereq-error; \
		echo; \
		rm -rf $(TMP_DIR); \
		false; \
	fi
	@rm -rf $(TMP_DIR)
	@mkdir -p $(TMP_DIR)

define Require
  ifeq ($$(CHECK_$(1)),)
    prereq: prereq-$(1)

    prereq-$(1): $(TMP_DIR) FORCE
		@echo -n "Checking '$(1)'... "
		@if $(NO_TRACE_MAKE) -f $(INCLUDE_DIR)/prereq.mk check-$(1) >/dev/null 2>/dev/null; then \
			echo 'ok.'; \
		else \
			echo 'failed.'; \
			echo -e "$(strip $(2))" >> $(TMP_DIR)/.prereq-error; \
		fi

    check-$(1): FORCE
	  $(call Require/$(1))
    CHECK_$(1):=1
  endif
endef


define RequireCommand
  define Require/$(1)
    which $(1)
  endef

  $$(eval $$(call Require,$(1),$(2)))
endef

# Required for the toolchain
define Require/working-make
	echo 'all: test' > $(TMP_DIR)/check.mk
	echo 'e0 = $$$$(foreach s,foobar,$$$$(eval $$$$s:))' >> $(TMP_DIR)/check.mk
	echo 'e1 = $$$$(foreach s,foobar, $$$$(eval $$$$s:))' >> $(TMP_DIR)/check.mk
	echo 'test: $$$$(strip $$$$(e0)) $$$$(strip $$$$(e1))' >> $(TMP_DIR)/check.mk
	$(NO_TRACE_MAKE) -f $(TMP_DIR)/check.mk
endef

$(eval $(call Require,working-make, \
	Your make version is buggy. Please install GNU make v3.81 or later. \
))

define Require/working-gcc
	echo 'int main(int argc, char **argv) { return 0; }' | \
		gcc -x c -o $(TMP_DIR)/a.out -
endef

$(eval $(call Require,working-gcc, \
	No working GNU C Compiler was found on your system. \
))

define Require/working-g++
	echo 'int main(int argc, char **argv) { return 0; }' | \
		g++ -x c++ -o $(TMP_DIR)/a.out -lstdc++ -
endef

$(eval $(call Require,working-g++, \
	No working GNU C++ Compiler was found on your system. \
))

define Require/zlib
	echo 'int main(int argc, char **argv) { gzdopen(0, "rb"); return 0; }' | \
		gcc -include zlib.h -x c -o $(TMP_DIR)/a.out -lz -
endef

$(eval $(call Require,zlib, \
	The development version of zlib was not found on your system. \
))


$(eval $(call RequireCommand,bison, \
	Please install GNU bison. \
))

$(eval $(call RequireCommand,flex, \
	Please install flex. \
))

$(eval $(call RequireCommand,python, \
	Please install python. \
))

$(eval $(call RequireCommand,unzip, \
	Please install unzip. \
))

$(eval $(call RequireCommand,bzip2, \
	Please install bzip2. \
))

$(eval $(call RequireCommand,patch, \
	Please install patch. \
))


